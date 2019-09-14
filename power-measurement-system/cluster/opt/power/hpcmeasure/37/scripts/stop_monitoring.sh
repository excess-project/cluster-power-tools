#!/bin/sh

NODE=$1 #node to measure
if [ -z ${NODE} ]; then
  echo "Usage: stop_monitoring.sh NODE"
  exit 1
fi

HPC_REVISION=$(cat /opt/power/hpcmeasure/revision)
HPCMEASURE_PATH=/opt/power/hpcmeasure/${HPC_REVISION}/
HPC_START_SCRIPT=$HPCMEASURE_PATH/start_hpcmeasure_compnode_user.sh 
HPC_STOP_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_compnode_user.sh 
PROFILE_ID="dreamcloud" #allways -1
#STOP POWER MEASUREMENT
${HPC_STOP_SCRIPT} ${PROFILE_ID} ${NODE}

