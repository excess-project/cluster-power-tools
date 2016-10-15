#!/bin/bash

#create archiv with power data
#
PBS_USER=$1
PBS_JOBID=$2
DBKEY=$3
NODE=$4
REVISION_FILE=/opt/power/hpcmeasure/revision
HPC_REVISION=$(cat ${REVISION_FILE})
HPCMEASURE_PATH=/opt/power/hpcmeasure/${HPC_REVISION}/
HPC_ADDI_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_addi.sh
HPC_COMP_SCRIPT=$HPCMEASURE_PATH/start_hpcmeasure_compnode.sh
HPC_ADDI_STOP_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_addi.sh
HPC_COMP_STOP_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_compnode.sh
CONFIG_COPY_PATH=$HPCMEASURE_PATH/profiles
CONFIG_INI=$HPCMEASURE_PATH/profiles/config_${NODE}_ac.ini
POWER_FILE_PATH=/ext/ac/${NODE}/
RAW_FILE_PATH=/raw_data/ac/${NODE}/
TMP_FILE_PATH=/tmp_dummy/
MEASURE_BIN=$HPCMEASURE_PATH/bin/hpc_measure
CONVERT_BIN=$HPCMEASURE_PATH/bin/hpc_convert_ac
PROC_MEASURE_PIDFILE=${POWER_FILE_PATH}/measure_${NODE}.pid
PROC_CONVERT_PIDFILE=${POWER_FILE_PATH}/covert_${NODE}.pid
PWM_TMP_PATH=/ext/${NODE}_ac_tmp/
PWM_TMP_JOB_PATH=${PWM_TMP_PATH}/${PBS_JOBID}/
PWM_ARCH_PATH=/ext/ws/ac/
PWM_ARCH=${NODE}_ac_${PBS_JOBID}.tar.gz
PWM_PATH=/nas_home/${PBS_USER}/pwm/ac/${NODE}/
LOG_FILE=/var/log/hpcmeasure/addi_archiv_${NODE}_ac.log
DATE="$( date +'%c' )"
rm ${PROC_CONVERT_PIDFILE} -rf

#echo $DATE":--start copy_hpcmeasure_addi.sh---" >> $LOG_FILE
#echo $DATE":PBS_JOBID:"$PBS_JOBID  >>$LOG_FILE
#echo $DATE":HPCMEASURE_PATH:"$HPCMEASURE_PATH >>$LOG_FILE
#echo $DATE":HPC_ADDI_SCRIPT:"$HPC_ADDI_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_COMP_SCRIPT:"$HPC_COMP_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_ADDI_STOP_SCRIPT:"$HPC_ADDI_STOP_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_COMP_STOP_SCRIPT:"$HPC_COMP_STOP_SCRIPT >>$LOG_FILE
#echo $DATE":CONFIG_INI:"$CONFIG_INI >>$LOG_FILE
#echo $DATE":PBS_USER:"$PBS_USER >>$LOG_FILE
#echo $DATE":PWM_TMP_JOB_PATH:"$PWM_TMP_JOB_PATH >>$LOG_FILE
#echo $DATE":PWM_TMP_PATH:"$PWM_TMP_PATH >>$LOG_FILE
#echo $DATE":PWM_ARCH_PATH:"$PWM_ARCH_PATH >>$LOG_FILE
#echo $DATE":PWM_ARCH:"$PWM_ARCH >>$LOG_FILE
#echo $DATE":POWER_FILE_PATH:"$POWER_FILE_PATH  >>$LOG_FILE
#echo $DATE":RAW_FILE_PATH:"$RAW_FILE_PATH  >>$LOG_FILE
#echo $DATE":MEASURE_BIN:"$MEASURE_BIN  >>$LOG_FILE
#echo $DATE":CONVERT_BIN:"$CONVERT_BIN  >>$LOG_FILE
#echo $DATE":PROC_MEASURE_PIDFILE:"$PROC_MEASURE_PIDFILE  >>$LOG_FILE
#echo $DATE":PROC_CONVERT_PIDFILE:"$PROC_CONVERT_PIDFILE  >>$LOG_FILE
#echo $DATE":LOG_FILE:"$LOG_FILE  >>$LOG_FILE

if [ -z "$PBS_JOBID" ]; then
  echo $DATE":Error in copy_hpcmeasure_addi.sh: variable PBS_JOBID is not initialized" >> $LOG_FILE
  exit 1
fi

#make archiv directory
if [ ! -d "$PWM_ARCH_PATH" ]; then
  mkdir $PWM_ARCH_PATH -p
  chown power:ws $PWM_ARCH_PATH
  chmod g+s $PWM_ARCH_PATH
  chmod ug+rwx $PWM_ARCH_PATH
fi

#make tmp directory
if [ ! -d "$PWM_TMP_PATH" ]; then
  mkdir $PWM_TMP_PATH -p
fi

#make tmp pbs_jobid directory
if [ ! -d "$PWM_TMP_JOB_PATH" ]; then
  mkdir $PWM_TMP_JOB_PATH -p
  chmod g+s $PWM_TMP_JOB_PATH
fi
#make user home directory
if [ ! -d "$PWM_PATH" ]; then
  mkdir $PWM_PATH -p
  chmod g+s $PWM_PATH
  chmod ug+rwx $PWM_PATH
fi
#copy profile with config data
cp -r ${CONFIG_COPY_PATH} ${PWM_TMP_JOB_PATH}/
cp ${REVISION_FILE} ${PWM_TMP_JOB_PATH}/
#move power data to tmp directory pbs_jobid
for ch_num in `seq 0 32`;do
  mv ${POWER_FILE_PATH}/PChannel_${ch_num}_*.dat ${PWM_TMP_JOB_PATH}/  &> /dev/null
done
mv ${POWER_FILE_PATH}/* ${PWM_TMP_JOB_PATH}/  &> /dev/null
chown power:ws ${PWM_TMP_JOB_PATH}/* -R
chmod ug+rwx ${PWM_TMP_JOB_PATH}/*
tar -czf ${PWM_ARCH_PATH}/${PWM_ARCH} -C ${PWM_TMP_PATH} ./${PBS_JOBID}
chown power:ws ${PWM_ARCH_PATH}/${PWM_ARCH}
#start transfer of archive 
#echo $DATE":copy_hpcmeasure_addi.sh: copy the file ${PWM_ARCH_PATH}/${PWM_ARCH} to ${PWM_PATH}" >> $LOG_FILE
mv ${PWM_ARCH_PATH}/${PWM_ARCH} ${PWM_PATH}/${PWM_ARCH}   
#clean tmp data
#echo $DATE":copy_hpcmeasure_addi.sh: clean the directory ${PWM_TMP_JOB_PATH}" >> $LOG_FILE
for ch_num in `seq 0 32`;do
 rm ${PWM_TMP_JOB_PATH}/PChannel_${ch_num}_*.dat  &> /dev/null 
done
rm ${PWM_TMP_JOB_PATH} -rf

