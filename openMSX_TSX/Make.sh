#!/usr/bin/env bash

cd $(dirname "$0")

./configure
make -j20 CXXFLAGS=-Ofast
