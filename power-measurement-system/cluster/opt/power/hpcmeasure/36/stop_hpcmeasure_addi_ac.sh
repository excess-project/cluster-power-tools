#!/bin/bash

#stopt the power measurement for the computational nodes allocated in the pbs job on addi server
#stop_hpcmeasure_addi.sh - called in /opt/power/hpcmeasure/$ver/stop_hpcmeasure_compnode.sh
#stop_hpcmeasure_compnode.sh - called in /var/spool/torque/mom_priv/prologue
#svn repository  https://scm.projects.hlrs.de/svn/energyprofiler/trunk
#!warning the script will be executed under root - ToDo
PBS_USER=$1
PBS_JOBID=$2
DBKEY=$3
NODE=$4
PROFILE_ID=$5
SAVE_POWER_DATA=$6
HPC_REVISION=$(cat /opt/power/hpcmeasure/revision)
HPCMEASURE_PATH=/opt/power/hpcmeasure/${HPC_REVISION}/
HPC_ADDI_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_addi.sh
HPC_COPY_ADDI_SCRIPT=$HPCMEASURE_PATH/copy_hpcmeasure_addi_ac.sh
HPC_COMP_SCRIPT=$HPCMEASURE_PATH/start_hpcmeasure_compnode_ac.sh
HPC_ADDI_STOP_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_addi_ac.sh
HPC_COMP_STOP_SCRIPT=$HPCMEASURE_PATH/stop_hpcmeasure_compnode_ac.sh
CONFIG_INI=$HPCMEASURE_PATH/profiles/config_${NODE}.ini
POWER_FILE_PATH=/ext/ac/${NODE}/
RAW_FILE_PATH=/raw_data/ac/${NODE}/
TMP_FILE_PATH=/tmp_dummy/
MEASURE_BIN=$HPCMEASURE_PATH/bin/hpc_measure
CONVERT_BIN=$HPCMEASURE_PATH/bin/hpc_convert_ac
PROC_MEASURE_PIDFILE=${POWER_FILE_PATH}/measure_${NODE}.pid
PROC_CONVERT_PIDFILE=${POWER_FILE_PATH}/covert_${NODE}.pid
LOG_FILE=/var/log/hpcmeasure/addi_epologue_ac.log
DATE="$( date +'%c' )"

echo $DATE":--start initializing part of stop_hpcmeasure_addi.sh---" >> $LOG_FILE

if [ -z "$HPCMEASURE_PATH" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable HPCMEASURE_PATH is not initialized" >> $LOG_FILE
  exit 1
fi
if [ -z "$HPC_ADDI_SCRIPT" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable HPC_ADDI_SCRIPT  is not initialized" >> $LOG_FILE
  exit 1
fi
if [ -z "$HPC_ADDI_STOP_SCRIPT" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable HPC_ADDI_STOP_SCRIPT is not initialized" >> $LOG_FILE
  exit 1
fi
if [ -z "$HPC_COMP_STOP_SCRIPT" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable HPC_COMP_STOP_SCRIPT is not initialized" >> $LOG_FILE
  exit 1
fi
if [ -z "$HPC_COMP_SCRIPT" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable HPC_COMP_SCRIPT is not initialized" >> $LOG_FILE
  exit 2
fi

if [ -z "$CONFIG_INI" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable CONFIG_INI is not initialized" >> $LOG_FILE
  exit 3
fi
if [ -z "$POWER_FILE_PATH" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable POWER_FILE_PATH is not initialized" >> $LOG_FILE
  exit 4
fi
if [ -z "$RAW_FILE_PATH" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable RAW_FILE_PATH is not initialized" >> $LOG_FILE
  exit 5
fi
if [ -z "$MEASURE_BIN" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable MEASURE_BIN is not initialized" >> $LOG_FILE
  exit 6
fi
if [ -z "$CONVERT_BIN" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable CONVERT_BIN is not initialized" >> $LOG_FILE
  exit 7
fi
if [ -z "$PROC_MEASURE_PIDFILE" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable PROC_MEASURE_PIDFILE is not initialized" >> $LOG_FILE
  exit 8
fi
if [ -z "$PROC_CONVERT_PIDFILE" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable PROC_CONVERT_PIDFILE is not initialized" >> $LOG_FILE
  exit 9
fi
if [ -z "$LOG_FILE" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable LOG_FILE is not initialized" >> $LOG_FILE
  exit 10
fi

#log details about the job
#echo $DATE":--start stop_hpcmeasure_addi.sh---" >> $LOG_FILE
#echo $DATE":HPCMEASURE_PATH:"$HPCMEASURE_PATH >>$LOG_FILE
#echo $DATE":HPC_ADDI_SCRIPT:"$HPC_ADDI_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_COMP_SCRIPT:"$HPC_COMP_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_ADDI_STOP_SCRIPT:"$HPC_ADDI_STOP_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_COMP_STOP_SCRIPT:"$HPC_COMP_STOP_SCRIPT >>$LOG_FILE
echo $DATE":CONFIG_INI:"$CONFIG_INI >>$LOG_FILE
echo $DATE":POWER_FILE_PATH:"$POWER_FILE_PATH  >>$LOG_FILE
echo $DATE":RAW_FILE_PATH:"$RAW_FILE_PATH  >>$LOG_FILE
echo $DATE":PBS_USER:"$PBS_USER >>$LOG_FILE
echo $DATE":PBS_JOBID:"$PBS_JOBID >>$LOG_FILE

echo $DATE":MEASURE_BIN:"$MEASURE_BIN  >>$LOG_FILE
echo $DATE":CONVERT_BIN:"$CONVERT_BIN  >>$LOG_FILE
echo $DATE":PROC_MEASURE_PIDFILE:"$PROC_MEASURE_PIDFILE  >>$LOG_FILE
echo $DATE":PROC_CONVERT_PIDFILE:"$PROC_CONVERT_PIDFILE  >>$LOG_FILE
#echo $DATE":LOG_FILE:"$LOG_FILE  >>$LOG_FILE


#check the file with process pid of converter
if [ ! -f "$PROC_CONVERT_PIDFILE" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: The file with the pid of the hpc converter doesn't exist." >> $LOG_FILE
  exit 11
fi

#read the CONVERTER_PID
CONVERTER_PID="$( cat ${PROC_CONVERT_PIDFILE} )"
echo $DATE":stop_hpcmeasure_addi.sh: The hpc converter process id is:"$CONVERTER_PID >> $LOG_FILE
if [ -z "$CONVERTER_PID" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable CONVERTER_PID is not initialized" >> $LOG_FILE
  exit 12
fi

#check the file with process pid of hpcmeasure
if [ ! -f "$PROC_MEASURE_PIDFILE" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: The file with the pid of the hpc measure doesn't exist." >> $LOG_FILE
  exit 13
fi
#read the MEASURE_PID
MEASURE_PID="$( cat ${PROC_MEASURE_PIDFILE} )"
echo $DATE":stop_hpcmeasure_addi.sh: The hpc measure process id is:"$MEASURE_PID >> $LOG_FILE
if [ -z "$MEASURE_PID" ]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: variable MEASURE_PID is not initialized" >> $LOG_FILE
  exit 14
fi
#complete sequence of the measurement
#kill the hpc measure
KILL_SIGNAL_OK="$( kill -SIGINT  $MEASURE_PID )"
if [[ "${KILL_SIGNAL_OK}" -ne "0" ]]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: KILL_SIGNAL for hpc measure is not handled" >> $LOG_FILE
fi
#echo $DATE":stop_hpcmeasure_addi.sh: The hpc measure process will be stopped:"$KILL_SIGNAL_OK >> $LOG_FILE
#wait
while [ -e /proc/$MEASURE_PID ]; do sleep 0.1; done &>> $LOG_FILE
#wait $MEASURE_PID &>> $LOG_FILE
echo $DATE":stop_hpcmeasure_addi.sh: The hpc measure process is stopped" >> $LOG_FILE
#give few seconds for the hpc converter
flag="0"
check_flag="0"
while [ $flag -lt 1 ]
do 
    if [ `find ${RAW_FILE_PATH}/|wc -l` -ne 1 ] ; then 
#      echo $DATE":stop_hpcmeasure_addi.sh: wait hpc_converter" >> $LOG_FILE
        sleep 1
        check_flag=$[$check_flag+1]
        if [ "$check_flag" -gt 120 ]
        then
          rm ${RAW_FILE_PATH}/*
          error_message="ERROR: power measurement on $NODE was corrupt, please inform the administrator - error code: hpc_converter line $LINENO"
          ssh root@${NODE} "echo $error_message"
          break  # Skip entire rest of loop.
        fi
    else 
       echo $DATE":stop_hpcmeasure_addi.sh: wait hpc_converter is completed" >> $LOG_FILE
       flag=$[$flag+1] 
    fi 
done

#kill the hpc converter
echo $DATE":stop_hpcmeasure_addi.sh: The hpc converter process id is:"$CONVERTER_PID >> $LOG_FILE
KILL_SIGNAL_OK="$(kill -SIGINT  $CONVERTER_PID)"
if [[ "${KILL_SIGNAL_OK}" -ne "0" ]]; then
  echo $DATE":Error in stop_hpcmeasure_addi.sh: KILL_SIGNAL for hpc converter is not handled" >> $LOG_FILE
fi

echo $DATE":stop_hpcmeasure_addi.sh: The hpc converter process will be stopped:"$CONVERTER_PID >> $LOG_FILE
#wait
while [ -e /proc/$CONVERTER_PID ]; do sleep 0.1; done &>> $LOG_FILE
echo $DATE":stop_hpcmeasure_addi.sh: The hpc converter process is stopped" >> $LOG_FILE
echo $DATE":stop_hpcmeasure_addi.sh: archive power data" >> $LOG_FILE
#archiv and copy the power data in archiv directory on addi
if [ ${SAVE_POWER_DATA} -eq 1 ]; then
 echo $DATE:":stop_hpcmeasure_addi.sh: ${HPC_COPY_ADDI_SCRIPT}"  >> $LOG_FILE
 ${HPC_COPY_ADDI_SCRIPT} ${PBS_USER} ${PBS_JOBID} ${DBKEY} ${NODE}
fi
echo $DATE":stop_hpcmeasure_addi.sh: ready with the archive" >> $LOG_FILE
rm ${PROC_MEASURE_PIDFILE} -rf
rm ${PROC_CONVERT_PIDFILE} -rf

echo $DATE":---end stop_hpcmeasure_addi.sh---" >> $LOG_FILE

