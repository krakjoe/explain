#!/usr/bin/env sh
script_dir="$(cd "$(dirname "$0")"; pwd)"

git clone https://github.com/php/php-src
cd php-src/ext
rm -rf explain
# Symlink causes endless loop on run-tests.php, so copy all files
rsync -r --exclude=php-src "$(dirname "$script_dir")" ./
cd ../
./buildconf --force
./configure --disable-all --enable-debug --enable-explain=static
make
TEST_PHP_EXECUTABLE=sapi/cli/php sapi/cli/php run-tests.php --show-diff ext/explain | tee tests-output.txt && grep 'TEST SUMMARY$' tests-output.txt > /dev/null ; test $? -ne 0
