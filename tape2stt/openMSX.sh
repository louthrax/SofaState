#!/usr/bin/env bash

set -e

script_dir=$(dirname `readlink -e $0`)

export OPENMSX_HOME="${script_dir}/openmsx"
export OPENMSX_USER_DATA="${script_dir}/openmsx"
export OPENMSX_SYSTEM_DATA="${script_dir}/openmsx"

"${script_dir}/openmsx/openmsx" -setting "${script_dir}/openmsx/empty.xml" -machine "Toshiba_HX-10_64KB"
