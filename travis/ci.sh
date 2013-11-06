#!/usr/bin/env sh
wget http://uk3.php.net/get/php-5.4.7.tar.bz2/from/this/mirror -O "php-5.4.7.tar.bz2" 
tar -xf php-5.4.7.tar.bz2
cd php-5.4.7
cd ext
git clone https://github.com/krakjoe/explain.git
cd ../
./buildconf --force
./configure --disable-all 
make
TEST_PHP_EXECUTABLE=sapi/cli/php sapi/cli/php run-tests.php ext/explain
