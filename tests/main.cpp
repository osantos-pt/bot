#include "utils/Curl.h"
#include "utils/Rate.h"
#include "RateHandler.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
using namespace std::literals;
using Rate = utils::Rate;
using RateTracker = utils::RateTracker;

TEST(rate_tracker, basics)
{
    EXPECT_NO_THROW(RateTracker("https://endpoint/", "USD-BTC", "0.001"));
    EXPECT_NO_THROW(RateTracker("", "USD-BTC", "0.001"));
    EXPECT_NO_THROW(RateTracker("https://endpoint/", "", "0.001"));

    RateTracker rt("https://endpoint/", "USD-BTC", "0.111");
    EXPECT_STREQ(rt.getCurrencyPair().c_str(), "USD-BTC");
    EXPECT_STREQ(rt.getEndpoint().c_str(), "https://endpoint/USD-BTC");
    EXPECT_FALSE(rt.isInitialized());
    EXPECT_EQ(rt.getBid(), Rate("0.0"));
    EXPECT_EQ(rt.getAsk(), Rate("0.0"));

    rt.setBidAsk(Rate("10000"), Rate("10000"));
    EXPECT_TRUE(rt.isInitialized());
    EXPECT_EQ(rt.getBid(), Rate("10000"));
    EXPECT_EQ(rt.getAsk(), Rate("10000"));
}

namespace utils
{
    // Pretty printer for test name
    std::ostream& operator<<(std::ostream& os, const utils::RateChange& e)
    {
        switch (e)
        {
            case utils::RateChange::None:   os << 'N'; break;
            case utils::RateChange::Bid:    os << 'B'; break;
            case utils::RateChange::Ask:    os << 'A'; break;
        }
        return os;
    }
}

class rate_tracker_oscillation : public ::testing::TestWithParam<std::tuple<std::string_view, std::string_view, std::string, std::string_view, std::string_view, utils::RateChange>>
{
public:
    static std::vector<ParamType> getyData()
    {
        return
        {
            // FirstBid                 FirstAsk                Deviation   Bid                     Ask                     FlagOscillation
            { "10000",                  "10000",                "0.01",     "10101",                "10000",                utils::RateChange::Bid    },
            { "10000",                  "10000",                "0.01",     "10100",                "10000",                utils::RateChange::Bid    },
            { "10000",                  "10000",                "0.01",     "10099",                "10000",                utils::RateChange::None   },
            { "10000",                  "10000",                "0.01",     "10000",                "10000",                utils::RateChange::None   },
            { "10000",                  "10000",                "0.01",     "9901",                 "10000",                utils::RateChange::None   },
            { "10000",                  "10000",                "0.01",     "9900",                 "10000",                utils::RateChange::Bid    },
            { "10000",                  "10000",                "0.01",     "10000",                "10101",                utils::RateChange::Ask    },
            { "10000",                  "10000",                "0.01",     "10000",                "10100",                utils::RateChange::Ask    },
            { "10000",                  "10000",                "0.01",     "10000",                "10099",                utils::RateChange::None   },
            { "10000",                  "10000",                "0.01",     "10000",                "10000",                utils::RateChange::None   },
            { "10000",                  "10000",                "0.01",     "10000",                "9901",                 utils::RateChange::None   },
            { "10000",                  "10000",                "0.01",     "10000",                "9900",                 utils::RateChange::Ask    },

            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010000000",  "0.00000000010010100",  utils::RateChange::Ask    },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010000000",  "0.00000000010010000",  utils::RateChange::Ask    },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010000000",  "0.00000000010009999",  utils::RateChange::None   },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010000000",  "0.00000000010000000",  utils::RateChange::None   },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010000000",  "0.00000000009999999",  utils::RateChange::None   },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010000000",  "0.00000000009990001",  utils::RateChange::None   },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010000000",  "0.00000000009990000",  utils::RateChange::Ask    },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010010100",  "0.00000000010000000",  utils::RateChange::Bid    },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010010000",  "0.00000000010000000",  utils::RateChange::Bid    },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010009999",  "0.00000000010000000",  utils::RateChange::None   },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000010000000",  "0.00000000010000000",  utils::RateChange::None   },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000009999999",  "0.00000000010000000",  utils::RateChange::None   },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000009990001",  "0.00000000010000000",  utils::RateChange::None   },
            { "0.00000000010000000",   "0.00000000010000000",   "0.001",    "0.00000000009990000",  "0.00000000010000000",  utils::RateChange::Bid    },
        };
    }
};

TEST_P(rate_tracker_oscillation, check_oscilation)
{
    RateTracker tracker("https://endpoint", "CURRENCY_PAIR", std::get<2>(GetParam()));
    tracker.setBidAsk(Rate(std::get<0>(GetParam())), Rate(std::get<1>(GetParam())));
    EXPECT_TRUE(tracker.checkOscillation(Rate(std::get<3>(GetParam())), Rate(std::get<4>(GetParam()))) == std::get<5>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(,
    rate_tracker_oscillation,
    ::testing::ValuesIn(rate_tracker_oscillation::getyData()));

class MockPostgreSql : public utils::PostgreSql
{
public:
    MockPostgreSql() : utils::PostgreSql("host", "port", "user", "password", "name") {}

    MOCK_METHOD(void, addBaseline,      (const std::string& currencyPair, const std::string& bid, const std::string& ask),  (override));
    MOCK_METHOD(void, addOscillation,   (const std::string& currencyPair, const std::string& rate, char side),              (override));
};

class MockCurl : public utils::Curl
{
public:
    MOCK_METHOD(MockCurl&,          reset,              (),                     (override));
    MOCK_METHOD(bool,               performHttpGet,     (const std::string&),   (override));
    MOCK_METHOD(const std::string&, getResponseBody,    (),                     (const, override));
};

TEST(rate_handler, basics)
{
    auto endpoint = "https://endpoint/"sv;
    auto oscillation = "0.01"s;
    auto currencyPair = "EUR-BTC"s;
    std::vector<std::string> currencyPairs({ currencyPair });

    auto requestUrl = std::string(endpoint).append(currencyPair);
    auto ask = "25931.8733718823"s;
    auto bid = "25794.1035993456"s;
    auto curlResponseBody =  std::string(R"({"ask": ")").append(ask).append(R"(", "bid": ")").append(bid).append(R"(", "currency": "USD"})");

    MockCurl mockCurl;
    MockPostgreSql mockDb;
    bot::RateHandler handler(&mockCurl, &mockDb, endpoint, oscillation, currencyPairs);

    EXPECT_CALL(mockCurl, reset()).Times(1).WillOnce(::testing::ReturnRef(mockCurl));
    EXPECT_CALL(mockCurl, performHttpGet(requestUrl)).Times(1).WillOnce(::testing::Return(true));
    EXPECT_CALL(mockCurl, getResponseBody()).Times(1).WillOnce(::testing::ReturnRef(curlResponseBody));
    EXPECT_CALL(mockDb, addBaseline(currencyPair, bid, ask)).Times(1);
    handler.retrieveAndCheck();

    // further updates; massive change to trigger both updates
    ask = "50.123123123";
    bid = "50.321321321";
    curlResponseBody = std::string(R"({"ask": ")").append(ask).append(R"(", "bid": ")").append(bid).append(R"(", "currency": "USD"})");

    EXPECT_CALL(mockCurl, reset()).Times(1).WillOnce(::testing::ReturnRef(mockCurl));
    EXPECT_CALL(mockCurl, performHttpGet(requestUrl)).Times(1).WillOnce(::testing::Return(true));
    EXPECT_CALL(mockCurl, getResponseBody()).Times(1).WillOnce(::testing::ReturnRef(curlResponseBody));
    EXPECT_CALL(mockDb, addOscillation(currencyPair, bid, 'B')).Times(1);
    EXPECT_CALL(mockDb, addOscillation(currencyPair, ask, 'A')).Times(1);
    handler.retrieveAndCheck();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
