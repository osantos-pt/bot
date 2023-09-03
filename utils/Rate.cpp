#include "utils/Rate.h"

utils::RateChange utils::RateTracker::checkOscillation(const Rate& bid, const Rate& ask)
{
    RateChange result = None;
    if (boost::multiprecision::fabs(bid - m_bid) >= m_bidDeviation)
        result = static_cast<RateChange>(result | Bid);
    if (boost::multiprecision::fabs(ask - m_ask) >= m_askDeviation)
        result = static_cast<RateChange>(result | Ask);
    return result;
}
