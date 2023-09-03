#include "utils/Postgres.h"
#include <libpq-fe.h>
#include <iostream>
using namespace std::string_view_literals;

struct utils::PostgreSql::PsqlImpl
{
    PGconn* m_connection;
};

utils::PostgreSql::PostgreSql(std::string_view host, std::string_view port, std::string_view user, std::string_view pass, std::string_view dbname) noexcept
{
    if (dbname.empty() || user.empty() || pass.empty() || host.empty() || port.empty())
    {
        std::cerr << "One or more fields were missing {name: " << dbname << "; user:" << user << "; password:" << pass << "; host:" << host << "; port:" << port << "}." << std::endl;
        return;
    }


    std::string connectionInfo;
    connectionInfo.append("dbname=").append(dbname)
        .append(" user=").append(user)
        .append(" password=").append(pass)
        .append(" host=").append(host)
        .append(" port=").append(port);

    m_impl = std::make_unique<PsqlImpl>();
    m_impl->m_connection = PQconnectdb(connectionInfo.c_str());
    if (PQstatus(m_impl->m_connection) != CONNECTION_OK)
    {
        std::cerr << "Failed to establish connection.\n\tInfo:" << connectionInfo << "\n\tMessage:" << PQerrorMessage(m_impl->m_connection) << std::endl;
        PQfinish(m_impl->m_connection);
        m_impl->m_connection = nullptr;
    }
}
utils::PostgreSql::~PostgreSql()
{
    if (m_impl && m_impl->m_connection)
        PQfinish(m_impl->m_connection);
}


void utils::PostgreSql::addRun(long interval, const std::string_view& oscillation)
{
    if (!m_impl || !m_impl->m_connection)
        return;

    constexpr auto query = "INSERT INTO run (interval, oscillation) VALUES ($1, $2) RETURNING id"sv;
    constexpr auto numParams = 2;

    auto strInt = std::to_string(interval);
    const char* paramValues[numParams];
    paramValues[0] = strInt.c_str();
    paramValues[1] = oscillation.data();

    m_runId = insertRecord(query, numParams, paramValues);
}

void utils::PostgreSql::addBaseline(const std::string_view& currencyPair, const std::string& bid, const std::string& ask)
{
    if (!m_impl ||!m_impl->m_connection)
        return;

    constexpr auto query = "INSERT INTO baseline (run, currencyPair, bid, ask) VALUES ($1, $2, $3, $4) RETURNING id"sv;
    const auto numParams = 4;

    const char* paramValues[numParams];
    paramValues[0] = m_runId.c_str();
    paramValues[1] = currencyPair.data();
    paramValues[2] = bid.c_str();
    paramValues[3] = ask.c_str();

    auto baselineId = insertRecord(query, numParams, paramValues);

    if (!baselineId.empty() && !m_tickMap.contains(currencyPair))
        if (!m_tickMap.try_emplace(currencyPair, baselineId).second)
            std::cerr << "Failed to insert a baseline record (currency pair: " << currencyPair << "; baseline ID: " << baselineId << ")." << std::endl;
}

void utils::PostgreSql::addOscillation(const std::string_view& currencyPair, const std::string& rate, char side)
{
    if (!m_impl ||!m_impl->m_connection)
        return;

    constexpr auto query = "INSERT INTO tick (baseline, rate, side) VALUES ($1, $2, $3)"sv;
    const auto numParams = 3;

    char strSide[2];
    strSide[0] = side;
    strSide[0] = '\0';
    const char* paramValues[numParams];
    paramValues[0] = m_tickMap[currencyPair].c_str();
    paramValues[1] = rate.c_str();
    paramValues[2] = &strSide[0];

    insertRecord(query, numParams, paramValues);
}

std::string utils::PostgreSql::insertRecord(const std::string_view& query, int numParams, const char** paramValues)
{
    std::string retVal;
    PGresult* result = PQexecParams(m_impl->m_connection, query.data(), numParams, nullptr, paramValues, nullptr, nullptr, 0);
    auto status = PQresultStatus(result);
    if (status == PGRES_COMMAND_OK)
    {
        const char* cmdt = PQcmdTuples(result);
        auto affectedRows = std::stoi(cmdt);
        if (affectedRows != 1)
            std::cerr
                << "Error: Something went wrong on insertion (affected rows should be 1).\n"
                   "Query: " << query << "\n"
                   "Affected rows: " << cmdt << std::endl;

    }
    else if (status == PGRES_TUPLES_OK && PQntuples(result) == 1)
        retVal = PQgetvalue(result, 0, 0);
    else
        std::cerr
            << "Error: Unable to insert a new record.\n"
               "Query: " << PQcmdTuples(result) << "\n"
               "Message: " << PQresultErrorMessage(result) << std::endl;
    PQclear(result);
    return retVal;
}
