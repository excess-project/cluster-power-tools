#!/bin/bash

# Use only with kernel 2.6
# Load the APCI-3xxx and create the device node 
# under /dev/apci3xxx and the minor number of the board.
ADDIDATA_INCLUDE="../../../include/linux/addidata"

DRIVER_NAME="xpci3xxx"
MODULE_NAME="$DRIVER_NAME.ko"


#checking privileges
if [ `id -u` -ne 0 ];
then
	echo "$0: You must have root privileges to run this script shell"
	exit 1
fi

#Test the kernel version
if [ `uname -r | cut -d'.' -f 2` -ne 6 ];
then
	echo "$0: This script is only for kernel 2.6.X"
	exit 1
fi

#checking args
if [ $# -eq 1 ];
then
	echo "Using directory $1"
	ADDIDATA_INCLUDE=$1
elif [ $# -gt 1 ];
then
	echo "Usage: $0 [path_to_addidata_include]";
	exit 2
else
	echo "Using default directory $ADDIDATA_INCLUDE"
fi

#checking file
if [ ! -f $ADDIDATA_INCLUDE/$DRIVER_NAME.h ];
then
	echo -e "Can't find $DRIVER_NAME.h in the directory $ADDIDATA_INCLUDE.\nPlease pass the correct path to the $DRIVER_NAME.h file in argument 1 of this script.\nFor example: $0 /usr/src/linux/include/linux/addidata/"
	exit 3
fi


#hash table of the name and the id of the boards. It comes from the xpci3xxx.h file
HASH_TABLE=$(egrep "#define xpci3.*BOARD_DEVICE_ID" $ADDIDATA_INCLUDE/xpci3xxx.h|sed s/"\s\+"/"\t"/g|cut -f 2,3)
BOARD_NAMES=`echo "$HASH_TABLE"|cut -f 1|sed s/_BOARD_DEVICE_ID//`


#index
I=0

#number of board found
NBOARDS_TOTAL=0

echo -n "looking for boards"
for BOARD_NAME in $BOARD_NAMES;
do
        echo -n "."
	I=$(($I+2))
	BOARD_ID=`echo $HASH_TABLE|cut -f $I -d' '|cut -c3-`
	BOARD_ID=`echo 15b8$BOARD_ID`

	#echo "Looking for board $BOARD_NAME of id $BOARD_ID"


	# Search the number of apci-3xxx in the computer
	NBOARDS=$(cat /proc/bus/pci/devices |grep -c $BOARD_ID)
	#all the boards found
	NBOARDS_TOTAL=$(($NBOARDS_TOTAL+$NBOARDS))

	# Board found?
	if [ $? -eq 1 ]; then
		echo "Fail to detect $BOARD_NAME"
		exit 1
	fi

	#if [ $NBOARDS -eq 0 ]; then
	#	echo "No $BOARD_NAME detected"
	#	exit 1
	#fi


        if [ $NBOARDS -ne 0 ]; then
		echo -e "\n$NBOARDS $BOARD_NAME detected"
	fi


done
echo -e "\nfound $NBOARDS_TOTAL PCI boards to manage with this driver"

# is the module already loaded ?
cat /proc/modules | grep $DRIVER_NAME > /dev/null

# remove already loaded module 
if [ $? -eq 0 ]; then
	rmmod $MODULE_NAME > /dev/null
fi

#load module
insmod $MODULE_NAME

if [ $? -ne 0 ];
then
	echo "aborting"
	exit 1
fi


mkdir -p "/dev/$DRIVER_NAME"
cd "/dev/$DRIVER_NAME"

if [ $? -ne 0 ]; 
then
	echo "aborting"
	exit 1
fi

# Search the major number
MAJOR=$(grep $DRIVER_NAME /proc/devices | cut -f 1 -d\ )

if [ "$MAJOR" == "" ]; 
then
	echo "no major number attributed to driver"
	exit 1
fi

# Create nodes
i=0
while [ $i -lt $NBOARDS_TOTAL ]; do

	if [ -a "/dev/$DRIVER_NAME/$i" ]; then
		rm "/dev/$DRIVER_NAME/$i"
	fi

  	echo "making special character file /dev/$DRIVER_NAME/$i ( major=$MAJOR )"
	mknod $i c $MAJOR $i
	chmod 666 /dev/$DRIVER_NAME/$i
    i=$((i+1))
done

echo done

exit 0





