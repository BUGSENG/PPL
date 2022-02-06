#!/bin/bash
# Stop immediately if any executed command has exit status different from 0.
set -ex

usage() {
    echo "Usage: analyze.sh ANALYSIS" 1>&2
    echo "  where ANALYSIS is basename of an ECL configuration file" 1>&2
}

if [ $# -ne 1 ]
then
    usage
    exit 1
fi

# Absolute path of the ECLAIR bin directory.
ECLAIR_BIN_DIR="/opt/bugseng/eclair/bin"

# Directory where this script resides: usually in a directory named "ECLAIR".
SCRIPT_DIR="$(cd "$(dirname $0)" ; echo ${PWD})"
# Directory where to put all ECLAIR output and temporary files.
ECLAIR_OUTPUT_DIR="${PWD}/ECLAIR/out"

ANALYSIS="$1"

export CC_ALIASES=/usr/bin/gcc
export CXX_ALIASES=/usr/bin/g++
export LD_ALIASES=/usr/bin/ld
export LD_ALIASES=/usr/bin/as
export AR_ALIASES=/usr/bin/ar
export FILEMANIP_ALIASES="cp mv objcopy"

# ECLAIR binary data directory and workspace.
export ECLAIR_DATA_DIR="${ECLAIR_OUTPUT_DIR}/.data"
# ECLAIR workspace.
export ECLAIR_WORKSPACE="${ECLAIR_DATA_DIR}/eclair_workspace"
# Destination file for the ECLAIR diagnostics.
export ECLAIR_DIAGNOSTICS_OUTPUT="${ECLAIR_OUTPUT_DIR}/DIAGNOSTICS.txt"

# Identifies the particular build of the project.
export ECLAIR_PROJECT_NAME="PPL_${ANALYSIS}"
# All paths mentioned in ECLAIR reports that are below this directory
# will be presented as relative to ECLAIR_PROJECT_ROOT.
export ECLAIR_PROJECT_ROOT="${PWD}../../"

# Erase and recreate the output directory and the data directory.
rm -rf "${ECLAIR_OUTPUT_DIR}"
mkdir -p "${ECLAIR_DATA_DIR}"

if [ -f /proc/cpuinfo ]
then
  PROCESSORS=$(grep -c ^processor /proc/cpuinfo)
else
  PROCESSORS=6
fi

(
  pwd
  autoreconf
  ./configure --enable-interfaces=C

  # Perform the build (from scratch) in an ECLAIR environment.
  "${ECLAIR_BIN_DIR}/eclair_env"                   \
      "-eval_file='${SCRIPT_DIR}/${ANALYSIS}.ecl'" \
      -- make -j${PROCESSORS} -l20.0
)

# Create the project database.
PROJECT_ECD="${ECLAIR_OUTPUT_DIR}/PROJECT.ecd"
find "${ECLAIR_DATA_DIR}" -maxdepth 1 -name "FRAME.*.ecb" \
    | sort | xargs cat \
    | "${ECLAIR_BIN_DIR}/eclair_report" \
          "-create_db='${PROJECT_ECD}'" \
          -load=/dev/stdin

# Create the Jenkins reports file.
JENKINS_XML="${PWD}/ECLAIR/jenkins.xml"
"${ECLAIR_BIN_DIR}/eclair_report" \
    "-db='${PROJECT_ECD}'" \
    "-eval_file='${SCRIPT_DIR}/${ANALYSIS}_report.ecl'" \
    "-reports_jenkins='${JENKINS_XML}'"
