=======================================================
╔═╗╦  ╦ ╦╔═╗╔╦╗╔═╗╦═╗  ╔═╗╔═╗╦ ╦╔═╗╦═╗  ╔╦╗╔═╗╔═╗╦  ╔═╗
║  ║  ║ ║╚═╗ ║ ║╣ ╠╦╝  ╠═╝║ ║║║║║╣ ╠╦╝   ║ ║ ║║ ║║  ╚═╗
╚═╝╩═╝╚═╝╚═╝ ╩ ╚═╝╩╚═  ╩  ╚═╝╚╩╝╚═╝╩╚═   ╩ ╚═╝╚═╝╩═╝╚═╝
=======================================================
    ██████╗ ██████╗ ██╗██╗   ██╗███████╗██████╗ 
    ██╔══██╗██╔══██╗██║██║   ██║██╔════╝██╔══██╗
    ██║  ██║██████╔╝██║██║   ██║█████╗  ██████╔╝
    ██║  ██║██╔══██╗██║╚██╗ ██╔╝██╔══╝  ██╔══██╗
    ██████╔╝██║  ██║██║ ╚████╔╝ ███████╗██║  ██║
    ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝

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
-How to make and install the driver for  
 A/D converter APCIe-3021
-How to make two processes to start and log the output
 of the installed A/D converters
-How to flash the firmware update to APCIe-3021 
-How to start the measurement manually
=======================================================

=======================================================
      How to make and install the driver for
           A/D converter APCIe-3021
=======================================================
Go to the subdirectory 00483_HLRS_/hpcus and check
kernel-header  path in the Makefile. After that run 
the following commands:
$make
$sudo make install

Reboot the system and check the new module xpci3xxx:
$lsmod |grep xpci3xxx
xpci3xxx              107010  0 

If you don't see the similar output, check wether the
kernel module xpci3xxx.ko is properlys installed and
loaded during the boot-process.

To use the driver from user space, you need to change
the premissions to the new devices. In order to do it,
copy the file ./hpcus/sysfiles/addi_user_mode into the
directory /etc/init.d/ and add configure the services:
$sudo cp ./hpcus/sysfiles/addi_user_mode /etc/init.d/
$sudo chkconfig --add addi_user_mode
$sudo chkconfig chkconfig service_name on --level 2345
$sudo chkconfig chkconfig service_name off --level 016

To use the measurement system remotely, for example
within PBS system, copy one additional service to
the system directory:
$sudo cp ./hpcus/sysfiles/hpcmeasure /etc/init.d/
Don't add this servire to the system throught chkconfig.

======================================================
 How to make two processes to start and log the output
      of the installed A/D converters
======================================================
Go to the sub-directory 00483_HLRS_/hpcus/hpc and run
 the following command:
$make

Make produces three executable:
-hpc_measure: This process communicate directly with 
 APCIe-3021 and log the measured data in the raw files.
-hpc_convert: This process read the raw files and con-
 vert the measured values in the appropriate metrics.
 The appropriate conversion will be made on the basis 
 of the configuration files.
-hpc_update: This tool can update APCIe-3021 with the
 new firmware image APCIe3121_US11.rbf. This update
 corrects a bug, which caused the spontaneous error
 during the measurement. You don't need to use it, if
 you order the A/D converters with the HLRS Firmware 
 update. 

======================================================
   How to flash the firmware update to APCIe-3021 
      (you don't need to do it if you ordered
    APCIe-3021 with the HLRS firmware update)
======================================================
Go to the sub-directory 100483_HLRS_/hpcus/hpc and 
run the following actions
1 - Reboot the system
2 - Check all installed APCIe-3021 devices:
$ls /dev/dev/xpci3xxx_*
/dev/xpci3xxx_0  /dev/xpci3xxx_1  /dev/xpci3xxx_2 
/dev/xpci3xxx_3
3 - Update these:
$./hpc_update /dev/xpci3xxx_0
$./hpc_update /dev/xpci3xxx_1
$./hpc_update /dev/xpci3xxx_2
$./hpc_update /dev/xpci3xxx_4
Each of the update prints the new firmware version US11
4 - Reboot the system

======================================================
        How to start the measurement manually
======================================================
There are two process, which manage the measurement:
hpc_measure and hpc_converter:

hpc_measure:
  hpc_measurement.c - This source code defines the process to record 
  the output of the A/D converters APCIe-3021. Each of the converters
  has eight analog channels. This process configures and records each
  of the channels of several APCIe-3021s cards. Currently, its support
  four cards. If you install more or less ACPIe-3012 change its
  number in the header hpc.h. You may start several process parallel,
  each for one of cards.  Consider, that it is not allowed
  to manage and record the same card simultaneously from different processes.
  The configure file is divided in the GLOBAL part and in 
  the channel definition parts for each of the installed card.
  The frequency of the A/D converter is 100kHz, which is shared between the
  eight channels. The frequency can be changed in the field
  "acquisition_time".
  The process write out the recordered values after an interrupt. The
  interval  can be defined in the field "number_of_sequences_per_interrupt". 
  In case, that  all eight channels are on and the frequency equals to 100kHz
  (acquisition_time=10), the number 12500 in that field let to produce the outpu in each
  second.  The process hpc_measure saves the raw data in the unformated format (32-bit
  floating point numbers). After each interrupt also the timestamp will be
  written out   in a separate file in the unformated format (64 bit unsighed int). One
  record   consists on two numbers: seconds and nonoseconds of interrupt occuriency.
  The field ac_voltage_channel_id is used only if you measure also the AC
  power  consumption with help of additional devices to record the voltage profil.
  The field  "duration_time" let you to stop the process after a certain
  interval. Use 0 for the continues mode. The process with the number pid can 
  be stoped through the signal  * from the bash: "kill -SIGINT pid" 
  You are able to turn on/off each of the channels throught
  the use of the field "channel_0_on_off". 
  In case, you would like to carry out the measurements with the several
  cards and need the synchronisation between each paar of the channels use the master
  bus, which connect the cards and set the field "trigger_on_off" to 1. The drift
  between the channels will be 10 nansoseconds.
  There are also the parameters for the channels of the card.  Depending on
  the   maximal voltage on the input, chooce the appropriate gain in the field
  "channel_0_gain": 10 is for the voltage range [0;1] (V) in Bipolar mode and [-1;1] (V) in
  Polar mode.
  The channel coefficients  are also defined in the configuration files. For
  example see ../profiles/config_node02.ini.

hpc_converter:
  This process read the output ot hpc_measure, combined together the channels
  for the voltage and electric current together. The results will be saved 
  in a certaine directory and sends to the monitoring framework ATO; if
  (wished). The example of the configuration file can be found in 
  ../profiles/config_components_node02.ini.

  
Bash command Lines:
hpc_measure <param1> 
  param1 - Path to the configure file

hpc_convert:
    -r <string>: Directory, which contains the files with raw data
		  (see config_nodeXX.ini)\n") ;
    -c <string>: Path to the file, which contains the description
                 and and measure configuration of the components (i.e. CPU);
                 The address of the monitoring framework ATOM is also deined in
                 the file; (config_components_nodeXX.ini);
    -b <int>: Median filter - left window half-size 
              (used if dbkey present and !="0");
    -f <int>: Median filter - right window half-size 
              (used if dbkey present and !="0");
    -s <int>: 1- Save the power data; 0- Not to save the power
                 data;
    -d <string>: String with db key to send the converted data to
                the monitoring framewotk ATOM;
                If empty or equals to "0", the data wouldn't be sent;
    -n <int>: Job name (used if dbkey present and !="0");
    -u <int>: User name (used if dbkey present and !="0");
    -p <int>: Hostname of the compute node (used if dbkey present);
    -h : show this help text;




This sub-directory contains:
- Readme.txt: You are reading it;
- 100483_HLRS_APCIe-3021_48Kanal_Sientific_Linux64Bit: top directory
  (referenced in this document as 100483_HLRS_)
-100483_HLRS_/hpcus/sysfiles: Services for the system
-100483_HLRS_/hpcus/sysfiles/xpci3xxx: Treiber for the system
-100483_HLRS_/hpcus/hpc: Daemon to record and translate the 
  output of A/D converters.

======================================================
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
