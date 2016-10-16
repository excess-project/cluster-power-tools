=======================================================
╔═╗╦  ╦ ╦╔═╗╔╦╗╔═╗╦═╗  ╔═╗╔═╗╦ ╦╔═╗╦═╗  ╔╦╗╔═╗╔═╗╦  ╔═╗
║  ║  ║ ║╚═╗ ║ ║╣ ╠╦╝  ╠═╝║ ║║║║║╣ ╠╦╝   ║ ║ ║║ ║║  ╚═╗
╚═╝╩═╝╚═╝╚═╝ ╩ ╚═╝╩╚═  ╩  ╚═╝╚╩╝╚═╝╩╚═   ╩ ╚═╝╚═╝╩═╝╚═╝
=======================================================
		 ██████╗ ███████╗
		██╔═══██╗██╔════╝
		██║   ██║███████╗
		██║   ██║╚════██║
		╚██████╔╝███████║
		 ╚═════╝ ╚══════╝

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

=====================================================
-How to configurate the operating system on the power
 measurement system addi.
=======================================================
To enjoy all the benefits  of the power tools and avoid
unnecessary  complications,  use a  Linux  distribution
for  x86 with  the kernel  2.6.32. The  server in  HLRS
uses "Scientific  Linux release 6.6 (Carbon)"  with the
kernel  2.6.32-431. The  kernel  must  be patched  with
Bigphysarea Patch. This is necessary to ensure that you
can  uninterruptible measure  the power  consumption of
the cluster components for  several hours or even  days
with a high measure frequency between 12.5  and 100 kHz.
Consalt the instructions in readme_patch.txt  to  patch
the kernel.  There will  always be  some variations  in
detail.  For example,  the  grub command  line must  be
expanded  with  the   parameter  bigphysarea=262144  in
/boot/grub/grub.conf and not  in /etc/lilo.conf. The                                                                           
number  262144  defines   the  number of  pages  in the
memory,  which  will  be  used  by  the  A/D converters
APCIe-3021 to save the measure values using DMA.

After the installation of the operating system, install
the packages, which are listed in the file 
"installed-software.log", even if a major part of the
packages are optional. To install everything from the
list, use the command:
$yum -y install $(cat installed-software.log)
Turn off automatic updating using one new line in the file
/etc/yum.conf:
exclude=kernel.x86_64 kernel-debug.x86_64 \
kernel-debug-devel.x86_64 \
kernel-devel.x86_64 kernel-doc.noarch \
kernel-firmware.noarch \
kernel-headers.x86_64

This sub-directory contains:
- Readme.txt: You are reading it;
- config-2.6.32-431.17.1.el6.BPA.x86_64 kernel configuration of 
  HLRS-addi;
- readme_patch.txt: Instructions how to patch the kernel;
- installed-software.log: List of the system packages

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
