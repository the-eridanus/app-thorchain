#!/usr/bin/env bash

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BUILDDIR=$SCRIPTDIR/../../cmake-build-fuzz
INPUTS=$SCRIPTDIR/../inputs

echo "$BUILDDIR"
AFL_SKIP_CPUFREQ=1 AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 afl-fuzz -t 50 -i "$INPUTS" -o "$BUILDDIR/syncdir" -M main -- "$BUILDDIR/bin/fuzzing_stub"

