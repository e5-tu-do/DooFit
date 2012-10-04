#!/bin/bash

source /lhcbsoft/tu-dortmund/LHCbSoftwareSetup.sh x86_64-slc5-gcc43-opt > /dev/null 2>&1

for ver in `SetupProject.sh DaVinci --list-versions | cut -d " " -f 1`
do
  bash ./install.sh $ver $1
done
