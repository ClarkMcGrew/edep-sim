#!/bin/sh
#
# Write a ROOT file in the default geometry.  This output file should
# be tested to make sure that all of the fields are readable.

OUTPUT=101FailHEPEvt.root

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

cat > 101FailHEPEvt.txt <<EOF
1.0 13.0 0.0 0.0 0.0 0.0 1.0392633396995 0.09623568146043286 0.3492655213021866 1.105658 0.105658 482.8700598805462 331.1324933745929 300.63208929581367 2.9219691707960083
EOF

cat > 101FailHEPEvt.mac <<EOF
#######################################
# Set the hit segment.
#######################################
/edep/hitSagitta drift 1.0 mm
/edep/hitLength drift 1.0 mm
/edep/update

/generator/kinematics/hepevt/input 101FailHEPEvt.txt
/generator/kinematics/hepevt/verbose 2
/generator/kinematics/set hepevt

/generator/count/fixed/number 1
/generator/count/set fixed
/generator/add
EOF

edep-sim -o ${OUTPUT} -C -e 3 101FailHEPEvt.mac | tee 101FailHEPEvt.output 

grep "ERROR:.*EDepSimHEPEVT.*Syntax error" 101FailHEPEvt.output || exit 1

echo SUCCESS
