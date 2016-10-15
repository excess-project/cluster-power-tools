
	ADDI-DATA Linux driver (copyright 2007 - ADDI-DATA)
	===================================================


	1 - INTRODUCTION
	
	2 - INSTALLATION
		2.1 - Installation for building in kernel sources
		2.2 - Installation for building out of kernel sources
	
	3 - CONFIGURATION AND COMPILATION	
		3.1 - Building in kernel sources
		3.2 - Building out of kernel sources
	
	4 - LOADING / UNLOADING THE DRIVER MANUALY
		4.1 - IS THE DRIVER LOADED?
	
	5 - LOADING THE DRIVER AUTOMATICALLY AT BOOT TIME
		5.1 - IS THE DRIVER INSTALLED?
		5.2 - Tell the OS to load the driver at boot time
	
	6 - SAMPLES
	
	7 - GENERAL INFORMATION


 1 - INTRODUCTION
 ================
 
 The driver is designed as a kernel module compliant with 
 kernel 2.4 and 2.6.
 It may contains ioctl functions that can be called from a user level
 application, and exported kernel functions that can be called in a other  
 kernel module.
 The driver can be adapted to be used with realtime patches like RTAI.
 

 2 - INSTALLATION
 ================
 
 To be compiled, the driver uses the kernel compilation process.
 Driver sources have generally to be installed and built in the kernel sources directory.
 But, they can be installed and built out of kernel sources.

	WARNING: IF YOU WANTS TO USE DMA, PLEASE PATCH YOU KERNEL WITH THE BIG PHYSICAL AREA PATCH!
 
	2.1 - Installation for building in kernel sources
	=================================================
 	
 	Copy the driver package into e.g. you /tmp directory:
 	/tmp/xpci3xxx_rXXX.tar.bz2
 	
 	Untar the package archive:
  	tar -jvxf /tmp/xpci3xxx_rXXX.tar.bz2
 	
	This will extract driver sources under /tmp/xpci3xxx_rXXX
	
	Move headers into the kernel sources:
	mv /tmp/xpci3xxx_rXXX/include/linux/addidata /usr/src/linux/include/linux/
	
	Move the driver code into the kernel sources:
	mv /tmp/xpci3xxx_rXXX/drivers/addidata /usr/src/linux/drivers/
	
 	2.2 - Installation for building out of kernel sources
 	=====================================================
 	
 	Extract the package where you want e.g.: /tmp
 	
 	Copy the driver package into e.g. you tmp directory:
 	/tmp/xpci3xxx_rXXX.tar.bz2
 	
 	Untar the package archive:
 	tar -jvxf /tmp/xpci3xxx_rXXX.tar.bz2
 	
	This will extract driver sources under /tmp/xpci3xxx_rXXX
	
	
 3 - CONFIGURATION AND COMPILATION	
 =================================
 
 	3.1 - Building in kernel sources
 	================================ 
 	
 	If you have a link called linux, pointing on /usr/src/linux, 
 	and that you correctly installed sources, you don't have to configure anything.
 	Else please read the following section.
 	
 	3.2 - Building out of kernel sources
 	==================================== 
 	
 	In xpci3xxx_rXXX/drivers/addidata/xpci3xxx/, edit the "_makefile" file, you will see both fields:
 	
	####################################################################
	# relative path of the source of the kernel to build against
	# IF YOU ARE BUILDING THIS DRIVER OUTSIDE THE KERNEL SOURCE
	# PLEASE ADAPT THIS VARIABLE TO YOUR LOCAL INSTALLATION 
	KERNELSRCDIR:=/usr/src/linux/
	####################################################################


	####################################################################
	# relative path of the addidata include directory
	# IF YOU ARE BUILDING THIS DRIVER OUTSIDE THE KERNEL SOURCE
	# PLEASE ADAPT THIS VARIABLE TO YOUR LOCAL INSTALLATION 
	ADDIDATAINC:=/usr/src/linux/include/linux/addidata/
	#################################################################### 		
	
	KERNELSRCDIR is the path of kernel sources.
	ADDIDATAINC is the path of ADDI-DATA driver header.
	Edit these values if they are not correct.
	
	To compile the driver, go into the driver sources directory 
	e.g. xpci3xxx and type:
	
		- For kernel 2.4
			make -f _makefile 2.4_nodma
	
		- For kernel 2.6
			make -f _makefile 2.6_nodma

Or, to compile with DMA support:

		- For kernel 2.4
			make -f _makefile 2.4
	
		- For kernel 2.6
			make -f _makefile 2.6

Note: the driver supports the use of the bigphysarea patch to allow allocating big amount of memory continuously.
If this patch is not applied to the kernel, you may not be able to use DMA in all cases of acquisition.

 4 - LOADING / UNLOADING THE DRIVER MANUALY
 ==========================================
 
	REMARK: Those operations will probably require root privileges.
 	
 	Kernel 2.4:
 		The driver is named xpci3xxx.o, to load it use:
 		insmod xpci3xxx.o 	
 		
 		To unload the driver:
 		rmmod xpci3xxx
 	
 	Kernel 2.6:

 		The driver is named xpci3xxx.ko, to load it use:
 		insmod xpci3xxx.ko 	
 		
 		To unload the driver:
 		rmmod xpci3xxx


	4.1 - IS THE DRIVER LOADED?
	===========================

 	You can check if the driver has been successfully loaded with the command lsmod.
	The names of the loaded drivers appear.

	REMARK: for more information about lsmod, use "man lsmod".


5 - LOADING THE DRIVER AUTOMATICALLY AT BOOT TIME
=================================================

	REMARK: Those operations will probably require root privileges.

	Maybe you want to load the driver at each boot automatically.

	Go into your driver sources directory, e.g.: /tmp/xpci3xxx/
	Then, launch these simple commands:
		make -f _makefile install_26
		depmod -ae

	This will copy the module in the subdirectory
	/lib/modules/<kernel_version_of_the_kernel_you_are_building>/extra/ with a debian system.

	And create the udev rules under /etc/udev/addidata.rules and a link under /etc/udev/rules.d/addidata.rules

	5.1 - IS THE DRIVER INSTALLED?
	==============================
	
	You can check if the driver file takes place in one of the subdirectories of 
	/lib/modules/<kernel_version_of_the_kernel_you_are_building>
	e.g.: /lib/modules/2.6.15

	To do so, launch the following command:
		find /lib/modules/<kernel_version_of_the_kernel_you_are_building> -name <name_of_the_module>

	e.g. for a 2.4 kernel:
		find /lib/modules/2.4.27 -name xpci3xxx.o
		(the module extension is .o)

	e.g. for a 2.6 kernel:
		find /lib/modules/2.6.15 -name xpci3xxx.ko 
		(the module extension is .ko)

	If there is no result, it means that something failed.


	5.2 - Tell the OS to load the driver at boot time
 	=================================================

 	Kernel 2.4 and 2.6:
		Edit the file /etc/modules, and add the name of the module, e.g. xpci3xxx, 
		or simply use the command:
		echo "apcixxx">>/etc/modules

	You probably have to be root.
	If you do not find the /etc/modules file, please refer to your distribution 
	documentation (man modules).



6 - SAMPLES
===========
 
 	One or more samples are available. 
	They are located in the samples directory.
 	To build them, go into the driver samples sources directory:
 	make -f _makefile samples
 	
 	REMARK: Before to run them, the driver has to be loaded.
 	

7 - GENERAL INFORMATION
=======================

 In the ADDI-DATA header directory (default: /usr/src/linux/include/linux/addidata/)
 See xpci3xxx.h for description of the IOCTL commands provided.
 See xpci3xxx-kapi.h for description of the kernel functions provided.
  
 These two commands are deprecated since there is possibility 
 to retrieve the same information from OS facilities :
 * CMD_XPCI3XXX_CheckAndGetPCISlotNumber   
 * CMD_XPCI3XXX_GetHardwareInformation     

 For each board detected the driver dynamically creates a 
 special character device file in /proc/sys/xpci3xxx (kernel 2.4).

 * The major device number is provided by the operating system. 
 * The minor device number is actually the ordinal in the PCI device list (order of detection)
 * the name of the file is the PCI slot identification : BUS/SLOT/FUNCTION in hexadecimal

 On kernel 2.6, special character device file in /proc/sys/ are no more allowed, 
 a node is created under /dev/xpci3xxx.

 WARNING: This driver has not been tested with a true PCI hotplug system.

 For any request or remark please contact us:
 info@addi-data.com
