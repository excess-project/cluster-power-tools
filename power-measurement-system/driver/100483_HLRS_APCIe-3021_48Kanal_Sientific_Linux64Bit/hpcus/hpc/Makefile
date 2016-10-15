CC:=x86_64-redhat-linux-gcc
CC_LIB:=gcc

ifeq ($(ADDIDATAINC),)
ADDIDATAINC:=../xpci3xxx/include/linux/addidata
endif

SRC_DIR=./
OBJ_DIR=./obj
LIB_DIR = ./lib
LIB_DIR_NAME=libhpcpower.a
CFLAGS = -lini_config -lpthread -lcurl  
#-D__DO_INFO_SYSLOG_	 -D__DO_DEBUG_SYSLOG_	
#-D__DO_INFO_SYSLOG_	 -D__DO_DEBUG_SYSLOG_	

all: dirs  hpc_measure hpc_convert hpc_convert_ac hpc_power_lib hpc_update

clean:
	-rm -f hpc_measure
	-rm -f hpc_convert
	-rm -f hpc_convert_ac
	-rm -f hpc_update
	-rm -f $(LIB_DIR)/*
	-rm -f $(OBJ_DIR)/*

revision:
	svn update
	./set_revision.sh

dirs:
	test -d $(OBJ_DIR) || mkdir $(OBJ_DIR) -p
	test -d $(LIB_DIR) || mkdir $(LIB_DIR) -p

SOURCE_NAMES=hpc_mf.c hpc_timespec.c hpc_signals.c hpc_statistic.c hpc_config.c  hpc_curl.c
#Erstellung einer Liste mit den Sourcedateien
SOURCES=$(foreach file,$(SOURCE_NAMES),$(SRC_DIR)/$(file))
#Erstellung einer Liste mit den Objectdateien
OBJECTS=$(foreach file,$(SOURCE_NAMES),$(OBJ_DIR)/$(basename $(file)).o)
# 
#
hpc_measure: hpc_measure.c findboards.c errormsg.c input.c hpc_signals.c hpc_config.c hpc_jitter.c hpc_threads.c
	$(CC) -O2  -Wall -I./ -I$(ADDIDATAINC) $(CFLAGS) $^ -o $@
	
hpc_convert: hpc_convert.c hpc_mf.c hpc_timespec.c hpc_signals.c hpc_statistic.c hpc_config.c  hpc_curl.c
	$(CC) -O2 -g  -Wall -I./ -I$(ADDIDATAINC) $(CFLAGS) $^ -o $@  -lm

hpc_convert_ac: hpc_convert_ac.c hpc_timespec.c hpc_signals.c hpc_statistic.c hpc_mf.c hpc_config.c hpc_curl.c
	$(CC) -O2  -Wall -I./ -I$(ADDIDATAINC) $(CFLAGS) $^ -o $@  -lm

hpc_update: hpc_update.c errormsg.c
	$(CC) -D__DO_INFO_SYSLOG_ -Wall -I./ -I$(ADDIDATAINC) $(CFLAGS) $^ -o $@	


#Erzeugung von Objectdateien
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC_LIB) -O2  -Wall -I./ -I$(ADDIDATAINC) $(CFLAGS)  -c $< -o $@

hpc_power_lib:  $(OBJECTS) 
	ar rcs $(LIB_DIR)/$(LIB_DIR_NAME) $(OBJECTS)
