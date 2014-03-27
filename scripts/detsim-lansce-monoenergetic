#!/bin/bash

# Simulate a mono-energetic neutron beam with a geometry setup similar
# to the one to be used at LANSCE.

source captain-control.bash

function usage () {
    echo "usage: lansce [run] [events] [MeV]"
    echo "     [run]     -- The run number."
    echo "     [events]  -- The number of events to generate."
    echo "     [MeV]     -- The kinetic energy of the neutrons (in MeV)."
}

# Set the run number from the command line.
captain-run-number 0
if [ ${#1} != 0 ]; then
    captain-run-number ${1}
else
    usage
    captain-error "The macro file must be passed as the first argument."
    exit 1
fi

# Set the number of events to generate
if [ ${#2} != 0 ]; then
    EVENTS=${2}
else
    usage
    captain-error "Must provide the number of events."
    exit 1
fi

# Set the number of events to generate
if [ ${#3} != 0 ]; then
    ENERGY=${3}
else
    usage
    captain-error "Must provide neutron kinetic energy."
    exit 1
fi

# Set the file name fields.
captain-experiment nb
captain-data-source pg
captain-run-type ${ENERGY}-MeV

DETSIM_MACRO=$(mktemp)
cat >> ${DETSIM_MACRO} <<EOF
/dsim/control baseline 1.0

# This needs to be more than twice the target to CAPTAIN distance.
/Captain/length 60 m
/dsim/update

# Create the particle source.  This source will be used to specify the
# position of the vertex.  The any of the GPS direction and energy
# commands can be used.
/gps/particle neutron
/gps/ene/mono ${ENERGY} MeV

# This starts the beam at the rough position of the LANSCE target
# relative to the CAPTAIN detector (i.e. it's 25 m from the detector
# in the negative X direction).  This means that the neutron time of
# flight will be right.  The neutrons come from a 1 cm diameter sphere.
/gps/position -2500.0 0.0 -50.0 cm

# The beam spot is 1 cm in diameter.
/gps/pos/type Volume
/gps/pos/shape Sphere
/gps/pos/radius 5 mm

# This generates the direction of the neutron beam
/gps/direction 1 0 0 

/run/beamOn ${EVENTS}

EOF

#####################################################
# The is the meat of the script: Run DETSIM
#####################################################
captain-process-detsim-macro ${DETSIM_MACRO}

# Remove the temporary macro created above.
rm ${DETSIM_MACRO}
