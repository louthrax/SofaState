#!/usr/bin/env bash

set -e

script_dir=$(dirname `readlink -e $0`)

for var in "$@"
do
    "${script_dir}/tape2stt.py" "$1" "${var%.*}.stt"
done
