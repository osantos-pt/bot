#ifndef BOT_UTILS_CURL_H
#define BOT_UTILS_CURL_H
#include <iostream>
#include <memory>
#include <string>

namespace utils
{
    class Curl
    {
    public:
        Curl();
        virtual ~Curl();

        Curl(const Curl& other) = delete;
        Curl& operator=(const Curl& other) = delete;
        Curl(const Curl&& other) = delete;
        Curl& operator=(const Curl&& other) = delete;

        static bool initialize();
        virtual Curl& reset();
        virtual bool performHttpGet(const std::string& url);
        virtual const std::string& getResponseBody() const;

    private:
        static std::size_t callback(void*, std::size_t, std::size_t, void*);

        void setResponseBody(const char* data, std::size_t length) { m_body.append(data, length); }

        struct CurlImpl;
        std::unique_ptr<CurlImpl> m_curlImpl;
        std::string m_body;
    };
}

#endif
