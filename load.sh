#!/bin/bash

#trap catch_errors ERR

if [ -z $ROOTVERS ]; then
  echo "ROOT version not set. Cannot load DooSoftware."
  exit 2
fi

echo "Loading DooSoftware for ROOT $ROOTVERS"

export DOOFITSYS=/doosoft/DooFit/ROOT_v$ROOTVERS

if [ -e "~/LocalDooSoftware.sh" ]; then
  source ~/LocalDooSoftware.sh
fi

echo "Using DooFit from: $DOOFITSYS"

export PATH=$PATH:$DOOFITSYS/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DOOFITSYS/lib


