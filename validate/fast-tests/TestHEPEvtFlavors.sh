#!/bin/sh
#
# Demonstrates the usage of the HEPEvt reader. This
# example uses the ParticleBomb ("pbomb") input flavor.
# See README for details.

OUTPUT=100TestHEPEvtFlavors.root
# Modify the following line if using gdml file
#GEOM=$HOME/edep-sim/inputs/Module0.gdml

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

cat > TestHEPEvtFlavors.txt <<EOF
    0 0 2 -21.634 -16.103 -17.517 2.022
        1 13 0 0 0 0 0.291 -0.072 0.054 0.323 0.105658
        1 2212 1 1 0 0 0.091 -0.026 -0.831 1.257 0.9382
    1 0 2 29.209 -15.131 27.995 4.559
        1 2212 2 2 0 0 0.475 0.398 -0.861 1.417 0.9382
        1 13 3 3 0 0 -0.494 -0.116 -0.815 0.966 0.105658 
    2 0 2 -25.926 22.717 -48.278 1.706
        1 2212 4 4 0 0 1.201 1.108 -0.004 1.884 0.9382
        1 13 5 5 0 0 -0.256 -0.386 -0.688 0.836 0.105658 
EOF

cat > TestHEPEvtFlavors.mac <<EOF
#######################################
# Set the hit segment.
#######################################
/edep/hitSagitta drift 1.0 mm
/edep/hitLength drift 1.0 mm
/edep/update

/generator/kinematics/hepevt/input TestHEPEvtFlavors.txt
/generator/kinematics/hepevt/flavor pbomb
/generator/kinematics/hepevt/verbose 2
/generator/kinematics/set hepevt

/generator/count/fixed/number 1
/generator/count/set fixed
/generator/add
EOF

edep-sim -o ${OUTPUT} -C -e 3 TestHEPEvtFlavors.mac || exit 1

# Use the -g flag if using geometry file
#edep-sim -o ${OUTPUT} -C -e 1 -g ${GEOM} TestHEPEvtFlavors.mac || exit 1
