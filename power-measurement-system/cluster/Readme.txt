=======================================================
╔═╗╦  ╦ ╦╔═╗╔╦╗╔═╗╦═╗  ╔═╗╔═╗╦ ╦╔═╗╦═╗  ╔╦╗╔═╗╔═╗╦  ╔═╗
║  ║  ║ ║╚═╗ ║ ║╣ ╠╦╝  ╠═╝║ ║║║║║╣ ╠╦╝   ║ ║ ║║ ║║  ╚═╗
╚═╝╩═╝╚═╝╚═╝ ╩ ╚═╝╩╚═  ╩  ╚═╝╚╩╝╚═╝╩╚═   ╩ ╚═╝╚═╝╩═╝╚═╝
=======================================================

 ██████╗██╗     ██╗   ██╗███████╗████████╗███████╗██████╗ 
██╔════╝██║     ██║   ██║██╔════╝╚══██╔══╝██╔════╝██╔══██╗
██║     ██║     ██║   ██║███████╗   ██║   █████╗  ██████╔╝
██║     ██║     ██║   ██║╚════██║   ██║   ██╔══╝  ██╔══██╗
╚██████╗███████╗╚██████╔╝███████║   ██║   ███████╗██║  ██║
 ╚═════╝╚══════╝ ╚═════╝ ╚══════╝   ╚═╝   ╚══════╝╚═╝  ╚═╝
                   ██████╗ ██████╗ ███████╗               
                   ██╔══██╗██╔══██╗██╔════╝               
                   ██████╔╝██████╔╝███████╗               
                   ██╔═══╝ ██╔══██╗╚════██║               
                   ██║     ██████╔╝███████║               
                   ╚═╝     ╚═════╝ ╚══════╝               

High Performance Computing Center Stuttgart (HLRS)
           University of Stuttgart
		     &
                ADDI-DATA GmbH
             77836 Rheinmuenster

=======================================================
 This is a free Software.  Code is distributed in the
 hope that it will be useful,  but WITHOUT ANY WARRANTY;
 without even the implied warranty  of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. 
=======================================================

=======================================================
-How to configurate the cluster for the power tools.
=======================================================
We show the configuration of the cluster on the base
of EXCESS clsuter. This Cluster is a heterogenous computing
platform of 3 compute nodes: node01, node02 and node03. 
Cluster power tools enable monitoring of real-time power consumption
of various EXCESS cluster components.
A TORQUE PBS Manager is installed on the Front-End server (FE)
and manage the access to the computational resources node01-node03.
FE, power measurement system (see top directory OS for the definition)
have shared folder /opt, which is used to save under others the
cluster power tools configuration. The folder /opt/power/hpcmeasure/
contains various revision of the tools. The current version is
saved in the file /opt/power/hpcmeasure/revision.
The following files are defined in the directory
-start_hpcmeasure_compnode.sh: This script will be called from
  the /var/spool/torque/mom_priv/prologue file, which must be
  installed on each compute nodes of the cluster throught TORQUE
  installation (see http://www.adaptivecomputing.com/products/open-source/torque/ for the
  instructions). The "prologue" will be called on the mom-node
  before the user PBS job script will be executed on the cluster.
  start_hpcmeasure_compnode.sh start the measurement on the 
  power measurement system (referenced as addi) throught ssh call
  of the service hpcmeasure start
  (defined in driver/100483_HLRS_/hpcus/sysfiles/hpcmeasure).
-start_hpcmeasure_addi.sh: This script will be called from the service 
 hpcmeasure. It calls directly the hpc_measure and hpc_convert
 processes.
-stop_hpcmeasure_compnode.sh: will be called from the
 var/spool/torque/mom_priv/epilogue file, which is also installed
 by TORQUE PBS. The script stop the power measurement throught ssh call
 of the service "service hpcmeasure stop" 
 (defined in driver/100483_HLRS_/hpcus/sysfiles/hpcmeasure).
-stop_hpcmeasure_addi.sh: This script will be called from the service
 hpcmeasure. It stops directly the hpc_measure and hpc_convert.
-copy_hpcmeasure_addi.sh: This script will be called in "epilogue" file
 to copy the recordered files with the power measurement during the PBS job
 in the user folder ${HOME}./pwm/
There are also other files to start and stop the power measurements,
which was defined for the extra features.
Please copy the whole /opt folder to your system.
The /var folder comtains the prologue and epilogue files, which must be
copied to the compute nodes. THe definition of prologue and epilogue contains
also the handle of the mf_agents of ATOM monitoring framework
(see https://github.com/excess-project). The power measurement system can
be used with and without ATOM.


This sub-directory contains:
- Readme.txt: You are reading it;
- opt: This directoty with the confiruration files and start scripts for the
  power monitoring of a cluster .
- var: This directory contains the prologue and epilogue files, which must be
  copied to the compute nodes with installed TORQUE clients.

=======================================================
Bugs to: khabi@hlrs.de 

High Performance Computing Center Stuttgart (HLRS)
University of Stuttgart
======================================================

========================================================
Acknowledgment

This project is partially realized through EXCESS. 
EXCESS is funded by the EU 7th Framework Programme
(FP7/2013-2016) under grant agreement number 611183.
========================================================
