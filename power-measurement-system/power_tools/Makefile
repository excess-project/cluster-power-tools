#######################
##Make for power-tools#
#######################

#PATH TO libhpcpower.a
#We use this library to read the config
#file, which also used by hpc_converter
#See ../driver/Readme.txt about hpc_converter
LIB_HPCPOWER_PATH=../../power-measurement-system/driver/100483_HLRS_APCIe-3021_48Kanal_Sientific_Linux64Bit/hpcus/hpc/lib
#Compiler-Auswahl
#GNU compiler
F90=gfortran
# -fbounds-check
FFLAGS=-O2 -g -Wall -ffast-math -fstrict-aliasing -funroll-loops -Warray-bounds  -fbounds-check
#  LFLAGS=-O0 -g -Wall -ffast-math -fstrict-aliasing -funroll-loops
#  FFLAGS += -J./$(MOD_DIR)


EXEC_NAME=power_average
EXEC_NAME_2=power_calculate
EXEC_NAME_3=power_calculate_avx
EXEC_NAME_4=power_filter
F90_SRC_DIR=sources
MOD_DIR=mod
OBJ_DIR=obj


EXEC_SOURCE_NAMES=power_average.f90
EXEC_SOURCE_NAMES_2=power_calculate.f90
EXEC_SOURCE_NAMES_3=power_calculate_avx.f90
EXEC_SOURCE_NAMES_4=power_filter.f90
F90_SOURCE_NAMES=power_type.f90   power_extract_command.f90 power_input_parameter.f90 power_timestep.f90 power_rawdata.f90  power_statistic.f90 power_operations.f90
F90_SOURCE_NAMES_2=power_type.f90   power_extract_command.f90 power_input_parameter.f90 power_profile.f90 power_hpc_config.f90 power_hpc_config_interface.f90 power_timestep.f90 power_rawdata.f90 power_energy_data.f90  power_statistic.f90 power_operations.f90
F90_SOURCE_NAMES_3=power_type.f90   power_extract_command.f90 power_input_parameter.f90 power_profile.f90 power_io_avx.f90 power_hpc_config.f90 power_hpc_config_interface.f90 power_timestep.f90 power_rawdata.f90 power_energy_data.f90  power_statistic.f90 power_operations.f90
F90_SOURCE_NAMES_4=power_type.f90   power_extract_command.f90 power_input_parameter.f90 power_timestep.f90 power_rawdata.f90  power_statistic.f90 power_operations.f90
#Erstellung einer Liste mit den Sourcedateien
F90_SOURCES=$(foreach file,$(F90_SOURCE_NAMES),$(F90_SRC_DIR)/$(file))
F90_SOURCES_2=$(foreach file,$(F90_SOURCE_NAMES_2),$(F90_SRC_DIR)/$(file))
F90_SOURCES_3=$(foreach file,$(F90_SOURCE_NAMES_3),$(F90_SRC_DIR)/$(file))
F90_SOURCES_4=$(foreach file,$(F90_SOURCE_NAMES_4),$(F90_SRC_DIR)/$(file))
EXEC_SOURCES=$(foreach file,$(EXEC_SOURCE_NAMES),$(F90_SRC_DIR)/$(file))
EXEC_SOURCES_2=$(foreach file,$(EXEC_SOURCE_NAMES_2),$(F90_SRC_DIR)/$(file))
EXEC_SOURCES_3=$(foreach file,$(EXEC_SOURCE_NAMES_3),$(F90_SRC_DIR)/$(file))
EXEC_SOURCES_4=$(foreach file,$(EXEC_SOURCE_NAMES_4),$(F90_SRC_DIR)/$(file))

#Erstellung einer Liste mit den Objectdateien
F90_OBJECTS=$(foreach file,$(F90_SOURCE_NAMES),$(OBJ_DIR)/$(basename $(file)).o)
F90_OBJECTS_2=$(foreach file,$(F90_SOURCE_NAMES_2),$(OBJ_DIR)/$(basename $(file)).o)
F90_OBJECTS_3=$(foreach file,$(F90_SOURCE_NAMES_3),$(OBJ_DIR)/$(basename $(file)).o)
F90_OBJECTS_4=$(foreach file,$(F90_SOURCE_NAMES_4),$(OBJ_DIR)/$(basename $(file)).o)
EXEC_OBJECTS=$(foreach file,$(EXEC_SOURCE_NAMES),$(OBJ_DIR)/$(basename $(file)).o)
EXEC_OBJECTS_2=$(foreach file,$(EXEC_SOURCE_NAMES_2),$(OBJ_DIR)/$(basename $(file)).o)
EXEC_OBJECTS_3=$(foreach file,$(EXEC_SOURCE_NAMES_3),$(OBJ_DIR)/$(basename $(file)).o)
EXEC_OBJECTS_4=$(foreach file,$(EXEC_SOURCE_NAMES_4),$(OBJ_DIR)/$(basename $(file)).o)

all: dirs $(EXEC_NAME) $(EXEC_NAME_2) $(EXEC_NAME_3) $(EXEC_NAME_4)

#Erstellung von Verzecihnissen für Object- und Moddateien
dirs:
	test -d $(MOD_DIR) || mkdir $(MOD_DIR) -p
	test -d $(OBJ_DIR) || mkdir $(OBJ_DIR) -p

#Erzeugung von Objectdateien
$(OBJ_DIR)/%.o: $(F90_SRC_DIR)/%.f90
	$(F90) $(FFLAGS) $(INCLUDES) -J$(MOD_DIR) -I$(MOD_DIR) -c $< -o $@ 

#Erzeugung von ausführbarer Datei
$(EXEC_NAME): $(F90_OBJECTS) $(EXEC_OBJECTS)
	$(F90) $(LFLAGS) -J$(MOD_DIR) -I$(MOD_DIR) -o  $(EXEC_NAME) $(F90_OBJECTS) $(EXEC_OBJECTS) 

$(EXEC_NAME_2): $(F90_OBJECTS_2) $(EXEC_OBJECTS_2)
	$(F90) $(LFLAGS) -J$(MOD_DIR) -I$(MOD_DIR) -o  $(EXEC_NAME_2) $(F90_OBJECTS_2) $(EXEC_OBJECTS_2) ${LIB_HPCPOWER_PATH}/libhpcpower.a -lini_config -lcurl 

$(EXEC_NAME_3): $(F90_OBJECTS_3) $(EXEC_OBJECTS_3)
	$(F90) $(LFLAGS) -J$(MOD_DIR) -I$(MOD_DIR) -o  $(EXEC_NAME_3) $(F90_OBJECTS_3) $(EXEC_OBJECTS_3) ${LIB_HPCPOWER_PATH}/libhpcpower.a -lini_config -lcurl 

$(EXEC_NAME_4): $(F90_OBJECTS_4) $(EXEC_OBJECTS_4)
	$(F90) $(LFLAGS) -J$(MOD_DIR) -I$(MOD_DIR) -o  $(EXEC_NAME_4) $(F90_OBJECTS_4) $(EXEC_OBJECTS_4) 
	
clean:
	rm $(MOD_DIR) -rf
	rm $(OBJ_DIR) -rf
	rm $(EXEC_NAME) -f
	rm $(EXEC_NAME_2) -f
	rm $(EXEC_NAME_3) -f
	rm $(EXEC_NAME_4) -f

