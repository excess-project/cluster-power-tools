#!/bin/sh

HOSTNAME=$( hostname )
echo $HOSTNAME
if [ "$HOSTNAME" = "fe.excess-project.eu" ]; then
  echo "!!!!!Usage only on the compute nodes!!!! The power measure is not started !!!"
  echo "Usage: start_monitoring.sh NODE[node01|node02|node03] PWM_ID[PBS_JOBID] PROFILE_ID[-1|dreamcloud]"
  exit 1
fi
NODE=$1 #node to measure
PWM_ID=$2 #generic experiment id to be used by the pwm
PROFILE_ID=$3 #PROFILES: -1 ; or dreamcloud
if [ -z ${NODE} ] || [ -z ${PWM_ID} ] || [ -z ${PROFILE_ID} ]; then
  echo "Usage: start_monitoring.sh NODE[node01|node02|node03] PWM_ID[PBS_JOBID] PROFILE_ID[-1|dreamcloud]"
  exit 1
fi
HPC_REVISION=$(cat /opt/power/hpcmeasure/revision)
HPCMEASURE_PATH=/opt/power/hpcmeasure/${HPC_REVISION}/
HPC_START_SCRIPT=$HPCMEASURE_PATH/start_hpcmeasure_compnode_user.sh 
HPC_STOP_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_compnode_user.sh 
PROFILE_ID=-1 # dream cloud "dreamcloud" #-1 - all components
SAVE_DATA=0 #allways 0
#STARTT POWER MEASUREMENT
${HPC_START_SCRIPT} ${PWM_ID} ${NODE} ${PROFILE_ID} ${SAVE_DATA}

