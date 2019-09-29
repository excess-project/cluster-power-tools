#!/bin/sh

set -x
PROJECTDIR=$( pwd )
USERNAME=$USER
if [ -z "$1" ];  then
  echo "No JOB_ID provided (as first argument)"
  exit 1
fi
if [ -z "$2" ];  then
  echo "No NODE_ID provided (as second argument)"
  exit 2
fi
JOBID=$1
NODE=$2
BENCHMARK=$3
ENTPACK_POWER="1"
CALC_POWER="1"
DOMAIN="fe.excess-project.eu"
POWERDATADIR="$HOME/pwm/"
MESSAGEDIR="${PROJECTDIR}/messages/"
MESSAGEFILEOUT="${MESSAGEDIR}/${JOBID}.${DOMAIN}.out"
MESSAGEFILEERR="${MESSAGEDIR}/${JOBID}.${DOMAIN}.err"
DATEPATH=$( date +%Y-%m-%d )
ANALYSEDIR="${PROJECTDIR}/data/${JOBID}/"
PROFILE=${PROJECTDIR}/data/${JOBID}/${JOBID}-${BENCHMARK}-profile.dat
PERFORMANCEFILE=${PROJECTDIR}/data/${JOBID}/${JOBID}-${BENCHMARK}-performance.dat
if [ ! -d "$ANALYSEDIR" ]; then
  mkdir $ANALYSEDIR -p
fi

POWERTAR="${NODE}_${JOBID}.${DOMAIN}.tar.gz"
POWERTARFILE="${POWERDATADIR}/${NODE}/${POWERTAR}"
POWERDIR="${ANALYSEDIR}/tmp/${JOBID}.${DOMAIN}/"
if [ ! -d $POWERDIR ]; then
  mkdir ${POWERDIR} -p
fi
echo ${DATEPATH}>>${ANALYSEDIR}/log.log
if [ "$ENTPACK_POWER" == 1 ]; then
  echo "unpack archiv with power data: " ${POWERTARFILE}
  mkdir ${ANALYSEDIR}/tmp 
  tar xfz  ${POWERTARFILE} -C ${ANALYSEDIR}/tmp
fi
#calculate energy
HPC_REVISION=$(cat /opt/power/hpcmeasure/revision)
PROFILE_ID=
HPCMEASURE_PATH=/opt/power/hpcmeasure/${HPC_REVISION}/
HPCCONFIGFILE=${HPCMEASURE_PATH}/profiles/config_components_${NODE}${PROFILE_ID}.ini
RESULTDIR=${POWERDIR}/results/
ENERGYFILE=energy.dat
IDXFILE=idx.dat
ENERGYFILEPATH=${RESULTDIR}/${ENERGYFILE}
CPPERFORMANCEFILEPATH=${RESULTDIR}/${PERFORMANCEFILE}
RESULTFILEPREFIX=data
RESULTFILEEXT=dat
RESULTFILE=${RESULTFILEPREFIX}.${RESULTFILEEXT}
RESULTFILEPATH=${RESULTDIR}/${RESULTFILE}
POWER_TOOLS_REVISION=$(cat /opt/power/power_tools/revision)
filter_v="mean"
window_step_left_v=5
window_step_right_v=5
filter_a="median"
window_step_left_a=1
window_step_right_a=1

if [ "$CALC_POWER" == 1 ]; then
     echo "CALCULATE POWER"
     if [ -d "$RESULTDIR" ]; then
          rm $RESULTDIR -rf
     fi
     if [ ! -d "$RESULTDIR" ]; then
          mkdir $RESULTDIR -p

        module purge 
        module load power/power_tools/${POWER_TOOLS_REVISION}
        if [ ! -z "$PROFILE" ]; then
                 echo "power_calculate -verbosity  0 -prefixchannel Channel  -config_file "${HPCCONFIGFILE}"    -topdir "${POWERDIR}" -with_profile_file 1 -profile_topdir "/" -profile_file "${PROFILE}" -relative_time 0 -out_topdir "${RESULTDIR}" -out_filename ${ENERGYFILE} -out_idx_filename ${IDXFILE} -window_step_right_a ${window_step_right_a} -window_step_left_a ${window_step_left_a} -window_step_right ${window_step_right_v} -window_step_left ${window_step_left_v} -apply_shunt_correctur 1"
                 power_calculate -verbosity  0 -prefixchannel Channel  -config_file "${HPCCONFIGFILE}"    -topdir "${POWERDIR}" -with_profile_file 1 -profile_topdir "/" -profile_file "${PROFILE}" -relative_time 0 -out_topdir "${RESULTDIR}" -out_filename ${ENERGYFILE} -out_idx_filename ${IDXFILE} -window_step_right_a ${window_step_right_a} -window_step_left_a ${window_step_left_a} -window_step_right ${window_step_right_v} -window_step_left ${window_step_left_v} -apply_shunt_correction 1
          fi
      fi
      #copy results
      cp ${RESULTDIR} ${ANALYSEDIR} -r 
fi

echo "Results are in ${ANALYSEDIR}"
paste -d";" ${RESULTDIR}/${ENERGYFILE} ${PERFORMANCEFILE}  > ${RESULTFILEPATH}
echo "Generate pdf with the power data"
#correct the scripts
RSCRIPT_PW_TMP=${PROJECTDIR}/r/plot_profile_all.r 
RSCRIPT_PW=${PROJECTDIR}/r/plot_profile_all_$NODE.r 
sed_string='s/nodeXX/'${NODE}'/g'
sed "$sed_string" ${RSCRIPT_PW_TMP} > ${RSCRIPT_PW}
#run pdf generator
Rscript ${RSCRIPT_PW}  ${RESULTFILEPATH}  ${ANALYSEDIR} ${JOBID} 

echo "TABLE in ${RESULTFILEPATH}"
echo "PDF FILES in ${ANALYSEDIR}"

set +x
#delete raw data
#rm ${ANALYSEDIR}/tmp -rf 

