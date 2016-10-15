=======================================================
╔═╗╦  ╦ ╦╔═╗╔╦╗╔═╗╦═╗  ╔═╗╔═╗╦ ╦╔═╗╦═╗  ╔╦╗╔═╗╔═╗╦  ╔═╗
║  ║  ║ ║╚═╗ ║ ║╣ ╠╦╝  ╠═╝║ ║║║║║╣ ╠╦╝   ║ ║ ║║ ║║  ╚═╗
╚═╝╩═╝╚═╝╚═╝ ╩ ╚═╝╩╚═  ╩  ╚═╝╚╩╝╚═╝╩╚═   ╩ ╚═╝╚═╝╩═╝╚═╝
=======================================================

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

The power tools are intended to help the developers of
software and hardware to capture and analyse the power 
consumption of the various hasrdware components. We have
successfully integrated the developed power measurement
system in the EXCESS cluster. The report of it and eva-
luation results can the intrested reader find in the 
document ./EXCESS-Cluster-Power-Tools.pdf.
We have also developed the series of experiments to
evaluate CPUs, GPUs  and InfiniBand. The benchmark
can be found under the following link:
 https://github.com/excess-project/benchmarks

This repository contains the description and software
to install and configurate the cluster power tools.
The tools includes both hardware and software components.
The hardware components are described in the readme file
./hardware/Readme.txt
The subdirectory ./OS constains the description and system 
files for the configuration of the power measurement system.
The system can be also integrated in a HPC cluster environment
to measure the power consumption of the compute nodes components,
such as CPUs, GPU and other PCIe devices (i.e. IB adapter).
The main hardware components of the power measurement system
are the A/D converter APCIe-3021 of the company ADDI-DATA.
The driver of the system can be fond in the directory ./driver. 
The readme file ./driver/Readme.txt describes the installation
of the driver and other system components.
The subdirectory ./power_tools includes the source code for
the software tools, which provides the users to analyse the
recordered data by power measurement system. 
The subdirectory ./cluster includes the configurations file
in roder to integrate the system in a cluster environment,
see ./cluster/Readme.txt for more details.

======================================================

======================================================
Authors: 
Dmitry Khabi (HLRS) and
Björn Dick (HLRS)

High Performance Computing Center Stuttgart (HLRS)
University of Stuttgart
======================================================

========================================================
Acknowledgment:
This project is partially realized through EXCESS. 
EXCESS is funded by the EU 7th Framework Programme
(FP7/2013-2016) under grant agreement number 611183.
========================================================
