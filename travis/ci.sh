#!/usr/bin/env sh
git clone https://github.com/php/php-src
cd php-src/ext
git clone https://github.com/krakjoe/explain.git
cd ../
./buildconf --force
./configure --disable-all --enable-debug --enable-explain=static
make
TEST_PHP_EXECUTABLE=sapi/cli/php sapi/cli/php run-tests.php ext/explain
