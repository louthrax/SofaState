#!/usr/bin/env bash

set -e

script_dir=$(dirname `readlink -e $0`)

export OPENMSX_HOME="${script_dir}/openmsx"
export OPENMSX_USER_DATA="${script_dir}/openmsx"
export OPENMSX_SYSTEM_DATA="${script_dir}/openmsx"

"${script_dir}/openmsx/openmsx" -script "${script_dir}/launchTape.tcl" -setting "${script_dir}/openmsx/empty.xml" -machine "Philips_VG_8235_noDiskDrive" -cassetteplayer "$1"
