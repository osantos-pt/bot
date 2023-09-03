#ifndef BOT_UTILS_RATE_H
#define BOT_UTILS_RATE_H
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <string>
#include <string_view>

namespace utils
{
    using Rate = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<40>>;

    enum RateChange : char
    {
        None    = 0x00,
        Bid     = 0x01,
        Ask     = 0x02,
    };

    class RateTracker
    {
    public:
        explicit RateTracker(const std::string_view& endpoint, std::string_view k, const std::string& deviation) noexcept
            : m_currencyPair(k)
            , m_deviation(deviation)
            , m_initialized(false)
        {
            m_endpoint.append(endpoint).append(m_currencyPair);
        }

        const std::string& getCurrencyPair() const { return m_currencyPair; }
        const std::string& getEndpoint() const { return m_endpoint; }
        bool isInitialized() const { return m_initialized; }
        const Rate& getBid() const { return m_bid; }
        const Rate& getAsk() const { return m_ask; }

        RateChange checkOscillation(const Rate& bid, const Rate& ask);

        void setBidAsk(Rate&& bid, Rate&& ask)
        {
            m_bid = std::move(bid);
            m_ask = std::move(ask);
            m_bidDeviation = m_bid * m_deviation;
            m_askDeviation = m_ask * m_deviation;
            m_initialized = true;
        }

    private:
        std::string m_currencyPair;
        std::string m_endpoint;
        Rate        m_deviation;
        Rate        m_bidDeviation;
        Rate        m_askDeviation;
        Rate        m_bid;
        Rate        m_ask;
        bool        m_initialized;
    };
}

#endif
