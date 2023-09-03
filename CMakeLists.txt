cmake_minimum_required(VERSION 3.16)
project(bazinga)

# --- gcc settings -------------------------------------------------------
set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_EXTENSIONS            OFF)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")

set(CMAKE_CXX_FLAGS
    "-fconstexpr-depth=2500                         \
     -fdiagnostics-color=always                     \
     -fvisibility=internal                          \
     -g                                             \
     -mrdrnd                                        \
     -pedantic                                      \
     -Wall                                          \
     -Wcast-align                                   \
     -Wcast-qual                                    \
     -Wdisabled-optimization                        \
     -Wextra                                        \
     -Werror                                        \
     -Wformat                                       \
     -Wunused-but-set-variable                      \
     -Wunused-function                              \
     -Wunused-parameter                             \
     -Wunused-variable                              \
     -Woverlength-strings                           \
     -Wparentheses                                  \
     -Wredundant-decls                              \
     -Wsign-compare                                 \
     -Wstrict-overflow                              \
     -Wswitch                                       \
     -Wundef                                        \
     -Wuninitialized                                \
     -Wunreachable-code                             \
     -Wunused                                       \
     -Wvla                                          \
     -Wwrite-strings                                \
     -Waddress                                      \
     -Wduplicated-cond                              \
     -Walloc-zero                                   \
     -Wshift-overflow                               \
     -Wsizeof-pointer-div                           \
     -Wtrampolines                                  \
     -Wduplicated-branches                          \
     -Walloca                                       \
     -Wlogical-op                                   \
     -Wshadow                                       \
     -Wfloat-equal                                  \
     -Wconversion                                   \
     -Wmissing-field-initializers                   \
     -Wmissing-braces"
)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG   "-ftrapv -O0")


# Libraries
find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(PostgreSQL REQUIRED)

include(GoogleTest)
find_package(GTest REQUIRED)
enable_testing()

# Add the external library directory to the include directories
include_directories(${CMAKE_SOURCE_DIR}/external)

include_directories(.)

# Bot
add_executable(
    bot.run
    main.cpp
    RateHandler.cpp
    utils/Curl.cpp
    utils/Rate.cpp
    utils/Postgres.cpp
)
target_link_libraries(
    bot.run
    CURL::libcurl
    Boost::boost
    Boost::program_options
    PostgreSQL::PostgreSQL
)

# tests
add_executable(
    unittest.run
    RateHandler.cpp
    tests/main.cpp
    utils/Curl.cpp
    utils/Rate.cpp
    utils/Postgres.cpp
)
target_link_libraries(
    unittest.run
    CURL::libcurl
    Boost::boost
    GTest::gtest
    GTest::gmock
    PostgreSQL::PostgreSQL
    pthread
)
