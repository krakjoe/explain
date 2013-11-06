#!/usr/bin/env sh
git clone https://github.com/krakjoe/explain.git
cd explain
phpize
./configure
make && make test

