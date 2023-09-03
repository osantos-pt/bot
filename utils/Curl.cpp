#include "utils/Curl.h"
#include <curl/curl.h>

struct utils::Curl::CurlImpl
{
public:
    CurlImpl() : m_curl(curl_easy_init()) , m_status(CURLE_OK) {}
    ~CurlImpl()
    {
        curl_easy_cleanup(m_curl);
    }

    CurlImpl& reset()
    {
        // curl_easy_reset(m_curl);
        m_status = CURLcode::CURLE_OK;
        return *this;
    }

    CurlImpl& set(CURLoption opt, const std::string& value)
    {
        return set(opt, value.c_str());
    }

    CurlImpl& set(CURLoption opt, const auto& value)
    {
        auto result = curl_easy_setopt(m_curl, opt, value);
        if (result != CURLE_OK)
            std::cerr << "Failed to set curl option (" << opt << ") with error " << curl_easy_strerror(result);
        return *this;
    }

    CurlImpl& performHttpGet()
    {
        m_status = curl_easy_perform(m_curl);
        return *this;
    }

    CURLcode getStatus()
    {
        return m_status;
    }

private:
    CURL*       m_curl;
    CURLcode    m_status;
};

bool utils::Curl::initialize()
{
    static auto initialized = []()
    {
        if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
        {
            std::cerr << "Failed to initialize libcurl." << std::endl;
            return false;
        }
        return true;
    }();
    return initialized;
}

utils::Curl::Curl()
{
    m_curlImpl = std::make_unique<CurlImpl>();

    m_curlImpl->set(CURLOPT_USERAGENT,      "bot/v1");
    m_curlImpl->set(CURLOPT_WRITEFUNCTION,  &callback);
    m_curlImpl->set(CURLOPT_WRITEDATA,      reinterpret_cast<void*>(this));
}

utils::Curl::~Curl() = default;

utils::Curl& utils::Curl::reset()
{
    m_curlImpl->reset();
    m_body.clear();
    return *this;
}

bool utils::Curl::performHttpGet(const std::string& url)
{
    m_curlImpl->set(CURLOPT_URL, url).performHttpGet();
    return m_curlImpl->getStatus() == CURLE_OK;
}

const std::string& utils::Curl::getResponseBody() const
{
    return m_body;
}

std::size_t utils::Curl::callback(void* buffer, std::size_t size, std::size_t nmemb, void* userData)
{
    auto* curl = reinterpret_cast<utils::Curl*>(userData);
    size_t totalSize = size * nmemb;
    curl->setResponseBody(static_cast<char*>(buffer), totalSize);
    return totalSize;
}

