#! /bin/sh

# Enable error trapping.
set -e

INPUT=detsim-100GPSMuon5GeVThru.root
if [ ! -f $INPUT ]; then
    echo Cannot find ${INPUT}
    echo MISSING INPUT
    exit 1
fi

OUTPUT=detsim-200GPSMuon5GeVThru.root
if [ -f $OUTPUT ]; then
    rm $OUTPUT
fi

detSim-sElec.exe -a -o $OUTPUT $INPUT

