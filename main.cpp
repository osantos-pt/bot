#include "RateHandler.h"
#include "utils/Curl.h"
#include "utils/Postgres.h"
#include <atomic>
#include <boost/program_options.hpp>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>
namespace po = boost::program_options;
using namespace std::literals;
using namespace utils;

///@{ === signal handler ========================
std::atomic<bool> keepRunning(true);

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        std::cout << "Exiting gracefully..." << std::endl;
        keepRunning.store(false);
    }
}
///@}

int main(int argc, char** argv)
{
    if (!Curl::initialize())
    {
        std::cout << "Exiting due to failure to initialize curl" << std::endl;
        return -1;
    }

    // handle exit
    std::signal(SIGINT, signalHandler);

    constexpr auto empty = ""sv;
    auto getEnvironment = [&empty](const char* env) -> std::string_view
    {
        const auto* value = std::getenv(env);
        if (value != nullptr && value[0] != '\0')
            return value;
        return empty;
    };

    // parameters
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h",                                                                  "help message")
        ("interval,i",      po::value<unsigned short>()->default_value(5),          "fetch interval in seconds (default: 5)")
        ("oscillation,o",   po::value<std::string>()->default_value("0.01"),        "price oscillation percentage (default: 0.01)")
        ("currency,c",      po::value<std::vector<std::string>>()->multitoken(),    "currency pairs"); // not set as required here to allow -h to be performed
    po::positional_options_description p;
    p.add("currency", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }
    if (!vm.contains("currency"))
    {
        std::cout << "No currency oscillation provided.\n" << desc << std::endl;
        return 1;
    }

    auto interval = std::chrono::seconds(vm["interval"].as<unsigned short>());
    auto oscillation = vm["oscillation"].as<std::string>();
    const auto& currencyPairs = vm["currency"].as<std::vector<std::string>>();

    Curl curlWrapper;
    PostgreSql database(
        getEnvironment("PG_HOST"),
        getEnvironment("PG_PORT"),
        getEnvironment("PG_USER"),
        getEnvironment("PG_PASSWORD"),
        getEnvironment("PG_DATABASE"));
    database.addRun(interval.count(), oscillation);

    constexpr auto g_tickerApi = "https://api.uphold.com/v0/ticker/"sv;
    bot::RateHandler rateHandler(&curlWrapper, &database, g_tickerApi, oscillation, currencyPairs);
    while (keepRunning.load())
    {
        rateHandler.retrieveAndCheck();

        std::this_thread::sleep_for(interval);
    }
}
