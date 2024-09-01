#!/usr/bin/env bash

cd $(dirname "$0")

g++ *.cpp -o compress -Os
