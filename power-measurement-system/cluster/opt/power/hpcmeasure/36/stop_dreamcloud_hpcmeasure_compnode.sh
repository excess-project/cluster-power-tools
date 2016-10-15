#!/bin/sh

#stop the power measurement for the computational nodes allocated in the pbs job from computational node
#stop_hpcmeasure_compnode.sh - called in /var/spool/torque/mom_priv/epilogue
#stop_hpcmeasure_addi.sh - called in /opt/power/hpcmeasure/$ver/stop_hpcmeasure_compnode.sh
#svn repository  https://scm.projects.hlrs.de/svn/energyprofiler/trunk
#!warning the script will be executed under root - ToDo

DBKEY=$1
NODE=$2
DATE="$( date +'%c' )"
#echo $DATE":stop_hpcmeasure_compnode.sh DBKEY=${DBKEY}  NODE=${NODE}" >> /var/log/hpcmeasure/epilogue.log
HPC_REVISION=$(cat /opt/power/hpcmeasure/revision)
HPCMEASURE_PATH=/opt/power/hpcmeasure/${HPC_REVISION}/
HPC_ADDI_SCRIPT=$HPCMEASURE_PATH/start_hpcmeasure_addi.sh
HPC_COMP_SCRIPT=$HPCMEASURE_PATH/start_hpcmeasure_compnode.sh
HPC_ADDI_STOP_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_addi.sh
HPC_COMP_STOP_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_compnode.sh
CONFIG_INI=$HPCMEASURE_PATH/profiles/config_${NODE}.ini
PWM_LOG_DIR=/ext
PWM_RAWLOG_DIR=/raw_data
MEASURE_BIN=$HPCMEASURE_PATH/bin/hpc_measure
CONVERT_BIN=$HPCMEASURE_PATH/bin/hpc_convert
LOG_FILE=/var/log/hpcmeasure/stop_dreamcloud.log
HPCM_SERVICE=hpcmeasure
POWER_FILE_PATH=${PWM_LOG_DIR}/${NODE}
RAW_FILE_PATH=${PWM_RAWLOG_DIR}/${NODE}
PROC_MEASURE_PIDFILE=${POWER_FILE_PATH}/measure_${NODE}.pid
PROC_CONVERT_PIDFILE=${POWER_FILE_PATH}/covert_${NODE}.pid
PBS_USER=power
PBS_JOBID=777
POWER_SAVE_ON=0
PROFILE_ID=-1
#pre-check
DATE="$( date +'%c' )"

if [ -z "$NODE" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable NODE is not initialized" >> $LOG_FILE
  exit 1
fi
#check the hostname
HOST="$( hostname )"
#echo $DATE":HOST:"$HOST  >> $LOG_FILE
if [ -z "$HOST" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: Variable HOST is not initialized" >> $LOG_FILE
  exit 12
fi


#echo $DATE":--start initializing part of stop_hpcmeasure_compnode.sh---" >> $LOG_FILE
if [ -z "$HPCM_SERVICE" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable HPCM_SERVICE is not initialized" >> $LOG_FILE
  exit 1
fi
if [ -z "$HPCMEASURE_PATH" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable HPCMEASURE_PATH is not initialized" >> $LOG_FILE
  exit 1
fi
if [ -z "$HPC_ADDI_SCRIPT" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable HPC_ADDI_SCRIPT  is not initialized" >> $LOG_FILE
  exit 22
fi
if [ -z "$HPC_COMP_SCRIPT" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable HPC_COMP_SCRIPT is not initialized" >> $LOG_FILE
  exit 3
fi
if [ -z "$CONFIG_INI" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable CONFIG_INI is not initialized" >> $LOG_FILE
  exit 4
fi
if [ -z "$POWER_FILE_PATH" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable POWER_FILE_PATH is not initialized" >> $LOG_FILE
  exit 5
fi
if [ -z "$RAW_FILE_PATH" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable RAW_FILE_PATH is not initialized" >> $LOG_FILE
  exit 6
fi
if [ -z "$MEASURE_BIN" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable MEASURE_BIN is not initialized" >> $LOG_FILE
  exit 7
fi
if [ -z "$CONVERT_BIN" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable CONVERT_BIN is not initialized" >> $LOG_FILE
  exit 8
fi
if [ -z "$PROC_MEASURE_PIDFILE" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable PROC_MEASURE_PIDFILE is not initialized" >> $LOG_FILE
  exit 9
fi
if [ -z "$PROC_CONVERT_PIDFILE" ]; then
  echo $DATE":Error in stop_hpcmeasure_compnode.sh: variable PROC_CONVERT_PIDFILE is not initialized" >> $LOG_FILE
  exit 10
fi

#log details about the job
#echo $DATE":HPCMEASURE_PATH:"$HPCMEASURE_PATH >>$LOG_FILE
#echo $DATE":HPC_ADDI_SCRIPT:"$HPC_ADDI_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_COMP_SCRIPT:"$HPC_COMP_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_ADDI_STOP_SCRIPT:"$HPC_ADDI_STOP_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_COMP_STOP_SCRIPT:"$HPC_COMP_STOP_SCRIPT >>$LOG_FILE
#echo $DATE":CONFIG_INI:"$CONFIG_INI >>$LOG_FILE
#echo $DATE":POWER_FILE_PATH:"$POWER_FILE_PATH  >>$LOG_FILE
#echo $DATE":RAW_FILE_PATH:"$RAW_FILE_PATH  >>$LOG_FILE
#echo $DATE":MEASURE_BIN:"$MEASURE_BIN  >>$LOG_FILE
#echo $DATE":CONVERT_BIN:"$CONVERT_BIN  >>$LOG_FILE
#echo $DATE":PROC_MEASURE_PIDFILE:"$PROC_MEASURE_PIDFILE  >>$LOG_FILE
#echo $DATE":PROC_CONVERT_PIDFILE:"$PROC_CONVERT_PIDFILE  >>$LOG_FILE
#echo $DATE":LOG_FILE:"$LOG_FILE  >>$LOG_FILE

ssh root@addi "service ${HPCM_SERVICE} stop ${PBS_USER} ${PBS_JOBID} ${DBKEY} ${NODE} ${PROFILE_ID} ${POWER_SAVE_ON}"
#echo $DATE":stop_hpcmeasure_compnode.sh service ${HPCM_SERVICE} on addi system for  ${DBKEY} ${NODE} is stoped" >> $LOG_FILE

#echo $DATE":stop_hpcmeasure_compnode.sh: ---end---" >> $LOG_FILE
exit 0
