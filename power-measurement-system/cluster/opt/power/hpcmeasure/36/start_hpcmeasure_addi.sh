#!/bin/bash

#start the power measurement for the computational nodes allocated in the pbs job on addi server
#start_hpcmeasure_addi.sh - called in /opt/power/hpcmeasure/$ver/start_hpcmeasure_compnode.sh
#start_hpcmeasure_compnode.sh - called in /var/spool/torque/mom_priv/prologue
#svn repository  https://scm.projects.hlrs.de/svn/energyprofiler/trunk
#!warning the script will be executed under root

DBKEY=$1
NODE=$2
PROFILE_ID=$3
POWER_SAVE_ON=$4
PBS_USER=$5
PBS_JOBNAME=$6
PLATFORM_NAME=${NODE}.excess-project.eu
HPC_REVISION=$(cat /opt/power/hpcmeasure/revision)
HPCMEASURE_PATH=/opt/power/hpcmeasure/${HPC_REVISION}/
HPC_ADDI_SCRIPT=$HPCMEASURE_PATH/start_hpcmeasure_addi.sh
HPC_COMP_SCRIPT=$HPCMEASURE_PATH/start_hpcmeasure_compnode.sh
POWER_FILE_PATH=/ext/${NODE}/
RAW_FILE_PATH=/raw_data/${NODE}/
TMP_FILE_PATH=/tmp_dummy/
MEASURE_BIN=$HPCMEASURE_PATH/bin/hpc_measure
CONVERT_BIN=$HPCMEASURE_PATH/bin/hpc_convert
PROC_MEASURE_PIDFILE=${POWER_FILE_PATH}/measure_${NODE}.pid
PROC_CONVERT_PIDFILE=${POWER_FILE_PATH}/covert_${NODE}.pid
LOG_FILE=/var/log/hpcmeasure/addi_prologue_${NODE}.log
LOG_FILE_CONVERTER=/var/log/hpcmeasure/addi_prologue_converter_${NODE}.log
LOG_FILE_MEASURE=/var/log/hpcmeasure/addi_prologue_measure_${NODE}.log

CONFIG_INI=$HPCMEASURE_PATH/profiles/config_${NODE}.ini
POWER_CONFIG_INI=$HPCMEASURE_PATH/profiles/config_components_${NODE}.ini

if [ -z "$POWER_SAVE_ON" ]; then
 POWER_SAVE_ON=0
fi
if [ -z "$PROFILE_ID" ]; then
   PROFILE_ID=-1
fi

if [ "$PROFILE_ID" = "1" ] || [ "$PROFILE_ID" = "2" ] || [ "$PROFILE_ID" = "1_2" ] ; then
  CONFIG_INI=$HPCMEASURE_PATH/profiles/config_${NODE}_${PROFILE_ID}.ini
  POWER_CONFIG_INI=$HPCMEASURE_PATH/profiles/config_components_${NODE}_${PROFILE_ID}.ini
else
 if [ "$PROFILE_ID" = "dreamcloud" ] ; then
  POWER_CONFIG_INI=$HPCMEASURE_PATH/profiles/config_components_${NODE}_${PROFILE_ID}.ini
 fi 
  echo $DATE":no valid PROFILE_ID provided:"${PROFILE_ID} "; use default profile for " ${NODE} >> $LOG_FILE
fi

#pre-check
DATE="$( date +'%c'  )"
echo $DATE":--start initializing part of start_hpcmeasure_addi.sh---" >> $LOG_FILE

if [ -f "${HPCMEASURE_PATH}/start_hpcmeasure_addi_ac.sh" ]; then
  if [ -f "${POWER_CONFIG_INI}" ]; then
    ${HPCMEASURE_PATH}/start_hpcmeasure_addi_ac.sh ${DBKEY} ${NODE} ${PROFILE_ID} ${POWER_SAVE_ON} ${PBS_USER} ${PBS_JOBNAME}
  fi
fi

if [ -z "$HPCMEASURE_PATH" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable HPCMEASURE_PATH is not initialized" >> $LOG_FILE
  exit 1
fi
if [ -z "$HPC_ADDI_SCRIPT" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable HPC_ADDI_SCRIPT  is not initialized" >> $LOG_FILE
  exit 1
fi
if [ -z "$HPC_COMP_SCRIPT" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable HPC_COMP_SCRIPT is not initialized" >> $LOG_FILE
  exit 2
fi

if [ -z "$CONFIG_INI" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable CONFIG_INI is not initialized" >> $LOG_FILE
  exit 3
fi
if [ -z "$POWER_CONFIG_INI" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable POWER_CONFIG_INI is not initialized" >> $LOG_FILE
  exit 3
fi

if [ -z "$POWER_FILE_PATH" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable POWER_FILE_PATH is not initialized" >> $LOG_FILE
  exit 4
fi
if [ -z "$RAW_FILE_PATH" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable RAW_FILE_PATH is not initialized" >> $LOG_FILE
  exit 5
fi
if [ -z "$MEASURE_BIN" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable MEASURE_BIN is not initialized" >> $LOG_FILE
  exit 6
fi
if [ -z "$CONVERT_BIN" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable CONVERT_BIN is not initialized" >> $LOG_FILE
  exit 7
fi
if [ -z "$PROC_MEASURE_PIDFILE" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable PROC_MEASURE_PIDFILE is not initialized" >> $LOG_FILE
  exit 8
fi
if [ -z "$PROC_CONVERT_PIDFILE" ]; then
  echo $DATE":Error in start_hpcmeasure_addi.sh: variable PROC_CONVERT_PIDFILE is not initialized" >> $LOG_FILE
  exit 9
fi

#check directories
if [ ! -d "$POWER_FILE_PATH" ]; then
  mkdir ${POWER_FILE_PATH} -p
  chown -R power:ws ${POWER_FILE_PATH}
  chmod ug+rwx -R ${POWER_FILE_PATH}
  chmod g+s -R ${POWER_FILE_PATH}
else
  rm $POWER_FILE_PATH/* -rf
fi
if [ ! -d "$RAW_FILE_PATH" ]; then
  mkdir ${RAW_FILE_PATH} -p
  chown -R power:ws ${RAW_FILE_PATH}
  chmod ug+rwx -R ${RAW_FILE_PATH}
  chmod g+s -R ${RAW_FILE_PATH}
else
  rm $RAW_FILE_PATH/* -rf
fi
#log details about the job
#echo $DATE":--start start_hpcmeasure_addi.sh---" >> $LOG_FILE
#echo $DATE":HPCMEASURE_PATH:"$HPCMEASURE_PATH >>$LOG_FILE
#echo $DATE":HPC_ADDI_SCRIPT:"$HPC_ADDI_SCRIPT >>$LOG_FILE
#echo $DATE":HPC_COMP_SCRIPT:"$HPC_COMP_SCRIPT >>$LOG_FILE
#echo $DATE":CONFIG_INI:"$CONFIG_INI >>$LOG_FILE
#echo $DATE":DBKEY:"$DBKEY >>$LOG_FILE
#echo $DATE":POWER_FILE_PATH:"$POWER_FILE_PATH  >>$LOG_FILE
#echo $DATE":RAW_FILE_PATH:"$RAW_FILE_PATH  >>$LOG_FILE
#echo $DATE":MEASURE_BIN:"$MEASURE_BIN  >>$LOG_FILE
#echo $DATE":CONVERT_BIN:"$CONVERT_BIN  >>$LOG_FILE
#echo $DATE":PROC_MEASURE_PIDFILE:"$PROC_MEASURE_PIDFILE  >>$LOG_FILE
#echo $DATE":PROC_CONVERT_PIDFILE:"$PROC_CONVERT_PIDFILE  >>$LOG_FILE
#echo $DATE":LOG_FILE:"$LOG_FILE  >>$LOG_FILE



#start converter and save the process id
#echo $DATE": $CONVERT_BIN -r $RAW_FILE_PATH -c $POWER_CONFIG_INI -d $DBKEY -b 5 -f 5 -s ${POWER_SAVE_ON}"  >>$LOG_FILE
#$CONVERT_BIN -r $RAW_FILE_PATH -c $POWER_CONFIG_INI -d $DBKEY -b 5 -f 5 &>>$LOG_FILE_CONVERTER &
echo "$CONVERT_BIN -r $RAW_FILE_PATH -c $POWER_CONFIG_INI -d $DBKEY -b 5 -f 5 -s ${POWER_SAVE_ON} -u ${PBS_USER} -n ${PBS_JOBNAME} -t 1 -p ${PLATFORM_NAME}" >> $LOG_FILE
$CONVERT_BIN -r $RAW_FILE_PATH -c $POWER_CONFIG_INI -d $DBKEY -b 5 -f 5 -s ${POWER_SAVE_ON} -u ${PBS_USER} -n ${PBS_JOBNAME} -t 1 -p ${PLATFORM_NAME}  &>>/dev/null &
#$CONVERT_BIN -r $RAW_FILE_PATH -c $POWER_CONFIG_INI -d 0 &>/dev/null &
CONVERTER_PID=$!
echo $CONVERTER_PID > $PROC_CONVERT_PIDFILE

#start measure and save the process id

echo "$MEASURE_BIN $CONFIG_INI" >> $LOG_FILE
$MEASURE_BIN $CONFIG_INI &>>/dev/null &
#$MEASURE_BIN $CONFIG_INI > /dev/null &
MEASURE_PID=$!
echo $MEASURE_PID > $PROC_MEASURE_PIDFILE

#log more details about the job
#echo $DATE":CONVERTER_PID:"$CONVERTER_PID >>$LOG_FILE
#echo $DATE":MEASURE_PID:"$MEASURE_PID  >>$LOG_FILE
#echo $DATE":CONFIG_INI:"$CONFIG_INI  >>$LOG_FILE


exit 0
