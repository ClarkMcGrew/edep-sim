#! /bin/bash
#
set -x

# The validation directory.
VALIDATE=${DETSIMROOT}/check.d

# The input file.
INPUT=detsim-200GPSMuon5GeVThru.root
if [ ! -f ${INPUT} ]; then
    echo Cannot find ${INPUT}
    echo MISSING INPUT
    exit 1
fi

# The output file (not used).
OUTPUT=detsim-900GPSMuon5GeVThru.root

#  The macro to run in root.
MACRO=root900GPSMuon5GeVThru.C
cp ${VALIDATE}/${MACRO} .
root -n -q -b ${MACRO}++\(\"${INPUT}\",\"${OUTPUT}\"\)

