#!/bin/bash
# Test if ADDI-DATA udev rules exist, if not create them.

DRIVER_NAME=xpci3xxx

UDEVRULEFILENAME=/etc/udev/addidata.rules 
if [ ! -f "$UDEVRULEFILENAME" ] 
then 
echo "file $UDEVRULEFILENAME doesn't exists - creating it" 
        touch /etc/udev/addidata.rules 
        echo "# Generated automatically by ADDI-DATA installation script" >> $UDEVRULEFILENAME  
        ln -s $UDEVRULEFILENAME  /etc/udev/rules.d/addidata.rules 
else 
        echo "file $UDEVRULEFILENAME already exists" 
     
fi;

TMPSTR=`grep "SUBSYSTEMS==\"$DRIVER_NAME\"" $UDEVRULEFILENAME` 
if [ "" == "$TMPSTR" ];  
then 
        echo adding rule for $DRIVER_NAME to $UDEVRULEFILENAME 
        echo "SUBSYSTEMS==\"$DRIVER_NAME\", SYMLINK+=\"$DRIVER_NAME/%m\"" >> $UDEVRULEFILENAME   
        echo reloading udev 
                if [ "`which udevcontrol`" == "" ]; 
                then  
                        udevadm control --help | grep "reload-rules" && udevadm control --reload-rules || udevadm control --reload_rules 
                else 
                        udevcontrol reload_rules 
                fi;      
        echo a rule for $DRIVER_NAME already exists in file $UDEVRULEFILENAME 
 
fi;
