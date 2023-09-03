#include "RateHandler.h"
#include "utils/Curl.h"
#include "utils/Rate.h"
#include "json.hpp"
#include <iostream>
using namespace utils;

bot::RateHandler::RateHandler(utils::Curl* curl, utils::PostgreSql* db, const std::string_view& endpoint, const std::string& deviation, const std::vector<std::string>& currencyPairs)
    : m_db(db)
    , m_curl(curl)
{
    m_trackerList.resize(currencyPairs.size());
    std::transform(currencyPairs.begin(), currencyPairs.end(), m_trackerList.begin(), [&endpoint, &deviation](const auto& currency) { return std::make_unique<RateTracker>(endpoint, currency, deviation); });
}

void bot::RateHandler::retrieveAndCheck()
{
    for (auto& current : m_trackerList)
    {
        if (m_curl->reset().performHttpGet(current->getEndpoint()))
        {
            const auto& response = m_curl->getResponseBody();
            auto json = nlohmann::json::parse(response);

            if (json.size() != 3) [[unlikely]]
            {
                // sanity check!
                // if this happens one should maybe consider to remove it from the list!
                std::cerr << "Something went wrong; endpoint returned " << json.size() << " results." << std::endl;
                continue;
            }
            const std::string& bid = json["bid"];
            const std::string& ask = json["ask"];
            auto bidRate = Rate(bid);
            auto askRate = Rate(ask);
            if (current->isInitialized())
            {
                auto change = current->checkOscillation(bidRate, askRate);
                if (change&RateChange::Bid)
                {
                    std::cout << '[' << current->getCurrencyPair() << "] BID: " << current->getBid().str() << " -> " << bid << std::endl;
                    m_db->addOscillation(current->getCurrencyPair(), bid, 'B');
                }
                if (change&RateChange::Ask)
                {
                    std::cout << '[' << current->getCurrencyPair() << "] ASK: " << current->getAsk().str() << " -> " << ask << std::endl;
                    m_db->addOscillation(current->getCurrencyPair(), ask, 'A');
                }
            }
            else
            {
                std::cout << '[' << current->getCurrencyPair() << "] BID: " << bid << "; ASK: " << ask << std::endl;
                current->setBidAsk(std::move(bidRate), std::move(askRate));
                m_db->addBaseline(current->getCurrencyPair(), bid, ask);
            }
        }
    }
}
