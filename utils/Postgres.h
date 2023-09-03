#ifndef BOT_UTILS_POSTGRES_H
#define BOT_UTILS_POSTGRES_H
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace utils
{
    class PostgreSql
    {
    public:
        PostgreSql(std::string_view host, std::string_view port, std::string_view user, std::string_view password, std::string_view name) noexcept;
        ~PostgreSql();

        virtual void addRun(long interval, const std::string_view& oscillation);
        virtual void addBaseline(const std::string_view& currencyPair, const std::string& bid, const std::string& ask);
        virtual void addOscillation(const std::string_view& currencyPair, const std::string& rate, char side);

    private:
        std::string insertRecord(const std::string_view& query, int numParams, const char** paramValues);

        struct PsqlImpl;
        std::unique_ptr<PsqlImpl> m_impl;
        std::string m_runId;
        std::map<std::string_view, std::string> m_tickMap;
    };
}

#endif
