#!/bin/bash

if which pandoc &> /dev/null ; then
    pandoc ${EDEP_ROOT}/README.md -o ${EDEP_ROOT}/README.pdf
fi
