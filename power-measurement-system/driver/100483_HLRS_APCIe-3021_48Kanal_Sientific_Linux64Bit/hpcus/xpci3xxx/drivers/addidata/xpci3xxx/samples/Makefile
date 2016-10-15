# change that if you want to cross-compile 
CC:=$(CROSS)gcc

####################################################################
# relative path of the addidata include directory
# IF YOU ARE BUILDING THIS DRIVER OUTSIDE THE KERNEL SOURCE
# PLEASE ADAPT THIS VARIABLE TO YOUR LOCAL INSTALLATION 
ifeq ($(ADDIDATAINC),)
ADDIDATAINC:=/usr/src/linux/include/linux/addidata/
endif
####################################################################

OBJS:=sample_xpci3xxx \
		sample_apci3501\
		sample_xpci3xxx_read_1_analog_input\
		sample_xpci3xxx_timer \
		sample_xpci3xxx_counter \
		sample_xpci3xxx_autorefresh\
		sample_xpci3xxx_analog_output\
		sample_xpci3xxx_sequence \
		sample_xpci3xxx_ttl_input \
		sample_xpci3xxx_ttl_output


all: $(OBJS)

clean:
	-rm $(OBJS)

# Compile the sample apps
sample_xpci3xxx: sample_xpci3xxx.c findboards.c 
	$(CC) -Wall -I$(ADDIDATAINC) $^ -o $@
	
sample_xpci3xxx_timer: timer.c findboards.c 
	$(CC) -Wall -I$(ADDIDATAINC) $^ -o $@

sample_xpci3xxx_counter: counter.c findboards.c 
	$(CC) -Wall -I$(ADDIDATAINC) $^ -o $@

sample_apci3501: apci3501.c findboards.c 
	$(CC) -Wall -I$(ADDIDATAINC) $^ -o $@

sample_xpci3xxx_analog_output: analog_output.c findboards.c errormsg.c input.c
	$(CC) -Wall -I$(ADDIDATAINC) -I./ $^ -o $@

sample_xpci3xxx_read_1_analog_input: read_1_analog_input.c findboards.c errormsg.c input.c
	$(CC) -Wall -I$(ADDIDATAINC) -I./ $^ -o $@

sample_xpci3xxx_autorefresh: autorefresh.c findboards.c errormsg.c input.c
	$(CC) -Wall -I$(ADDIDATAINC) -I./ $^ -o $@

sample_xpci3xxx_sequence: sequence.c findboards.c errormsg.c input.c
	$(CC) -static -lthread -Wall -I$(ADDIDATAINC) -I./ $^ -o $@

sample_xpci3xxx_ttl_input: ttl_input.c findboards.c errormsg.c input.c
	$(CC) -Wall -I$(ADDIDATAINC) -I./ $^ -o $@
	
sample_xpci3xxx_ttl_output: ttl_output.c findboards.c errormsg.c input.c
	$(CC) -Wall -I$(ADDIDATAINC) -I./ $^ -o $@
