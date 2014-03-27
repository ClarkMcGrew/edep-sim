#! /bin/bash
#
#  This generates the standard set of geometry files for use in the
#  reconstruction.
# 
set -e

${DETSIMROOT}/scripts/detsim-export-geometry.sh baseline 1.0
${DETSIMROOT}/scripts/detsim-export-geometry.sh mini-captain 1.0
