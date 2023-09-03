# bot.run

My custom bot to check price oscillations (from original/first price)

## Table of Contents

- [Requirements](#requirements)
- [How ro run](#run)

## Requirements
The libraries/packages named in this section refer to ubuntu; if you are using any other OS please find its equivalent:
- build: `uild-essential g++ cmake ninja-build`
- debug: `gdb`
- curl: `libcurl4-openssl-dev`
- boost: `libboost-all-dev`
- postgres: `libpq-dev`
  - note: if running locally the actual DB consider the following too: `postgresql postgresql-client-common postgresql-server-dev-all`
- gtest: `libgtest-dev`
- gmock: `libgmock-dev`

Other libraries/scripts used which do not require any action:
- nlohmann json
  - Source: https://github.com/nlohmann/json
  - License: https://github.com/nlohmann/json/blob/develop/LICENSES/MIT.txt
- wait-for-it
  - source: https://github.com/vishnubob/wait-for-it/
  - License: https://github.com/vishnubob/wait-for-it/blob/master/LICENSE


## How ro run
### compile
From root directory of the project do the following:
* mkdir build
* cd build
* cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja -S ../ -B .
* cmake --build . --target bot.run
### run
* `./bot.run -h` show help 
* `./bot.run -i 5 -o 0.0001 -c BTC-USD -c USD-BTC` check oscillations of 0.001 every 5 seconds for both rates (BTC-USD and USD-BTC)

### Docker

One can run the whole setup on docker:
* Build image (bot) only: `docker build -t bot-image .`
* Start docker-compose containers (bot and database): `docker-compose up -d`
* Shutdown docker-compose containers: `docker-compose down`
* Handy single command (shut-down && build && start): `docker-compose down && docker-compose up -d --build`

To check the database one can do:
* `psql -h 127.0.0.1 -U bot_user  -d bot_db -W`
* (insert password) `bot_password`
* Query as needed:
  * `select * from run;`
  * `select * from baseline;`
  * `select * from tick;`
