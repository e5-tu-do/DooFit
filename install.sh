#!/bin/bash
uuid=`uuidgen -r`

function catch_errors {
  echo "Could not complete build for DaVinci $1"
  rm -rf /tmp/$uuid
  exit 1
}

trap catch_errors ERR

echo "Installing software for DaVinci $1"
source /lhcbsoft/tu-dortmund/LHCbSoftwareSetup.sh x86_64-slc5-gcc43-opt > /dev/null 2>&1
source SetupProject.sh DaVinci $1 > /dev/null 2>&1
mkdir /tmp/$uuid
cd /tmp/$uuid
echo "Starting build"
echo

cmake -DCMAKE_INSTALL_PREFIX=/doosoft/DooFit/$ROOTVERS $2
make VERBOSE=""
sudo make install
cd /
rm -rf /tmp/$uuid

echo
echo "Build successful for DaVinci $1"
echo

