#! /bin/bash
#
#  This generates the standard set of geometry files for use in the
#  reconstruction.
# 
set -e

${DETSIMROOT}/inputs/detsim-export-geometry.sh baseline 1.0
