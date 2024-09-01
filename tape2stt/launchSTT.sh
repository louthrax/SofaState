#!/usr/bin/env bash

set -e

script_dir=$(dirname `readlink -e $0`)

export OPENMSX_HOME="${script_dir}/openmsx"
export OPENMSX_USER_DATA="${script_dir}/openmsx"
export OPENMSX_SYSTEM_DATA="${script_dir}/openmsx"

cp -r "$1" "${script_dir}/diska/AUTOEXEC.STT"

"${script_dir}/openmsx/openmsx" -script "${script_dir}/launchSTT.tcl" -setting "${script_dir}/openmsx/empty.xml" -diska "${script_dir}/diska" -machine Philips_VG_8235 \
-ext slotexpander \
-ext ram128kb     \
-ext msxdos2      \
-ext ram128kb     \
-ext ram128kb     \
\
-ext slotexpander \
-ext ram128kb     \
-ext ram256kb     \
-ext ram128kb     \
-ext ram128kb

"${script_dir}/openmsx/openmsx" -script "${script_dir}/launchSTT.tcl" -setting "${script_dir}/openmsx/empty.xml" -diska "${script_dir}/diska" -machine Sony_HB-F1XDJ \
-ext slotexpander \
-ext ram128kb     \
-ext ram128kb     \
-ext ram256kb     \
-ext ram128kb     \
\
-ext slotexpander \
-ext ram128kb     \
-ext msxdos2      \
-ext ram128kb     \
-ext ram128kb

"${script_dir}/openmsx/openmsx" -script "${script_dir}/launchSTT.tcl" -setting "${script_dir}/openmsx/empty.xml" -diska "${script_dir}/diska" -machine Panasonic_FS-A1ST
