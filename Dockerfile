# configurable options
ARG BUILD_TYPE=Debug

FROM ubuntu:22.04 AS bot-image
RUN set -x                      \
    && apt update -y            \
    && apt install -y           \
        build-essential         \
        gcc                     \
        g++                     \
        cmake                   \
        ninja-build             \
        libcurl4-openssl-dev    \
        libpq-dev               \
        libboost-all-dev        \
        libgtest-dev            \
        libgmock-dev            \
    && mkdir /app

#maybe overkill, but for the project we keep it all inside the image
WORKDIR /app
COPY . /app

RUN set -x                                                          \
    && mkdir build                                                  \
    && cd build                                                     \
    && cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -G Ninja -S ../ -B .  \
    && cmake --build . --target bot.run unittest.run                \
    && ./unittest.run                                               \
    && cp bot.run ../.

ENTRYPOINT ["/app/bot.run"]
CMD []
