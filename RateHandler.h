#ifndef BOT_RATEHANDLER_H
#define BOT_RATEHANDLER_H
#include "utils/Curl.h"
#include "utils/Postgres.h"
#include "utils/Rate.h"
#include <string>
#include <string_view>
#include <vector>

namespace bot
{
    class RateHandler
    {
    public:
        RateHandler(utils::Curl* curl, utils::PostgreSql* db, const std::string_view& endpoint, const std::string_view& deviation, const std::vector<std::string>& currencyPairs);

        void retrieveAndCheck();

    protected:
        std::vector<std::unique_ptr<utils::RateTracker>> m_trackerList;
        utils::PostgreSql* m_db;
        utils::Curl* m_curl;
    };
}
#endif
