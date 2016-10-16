=======================================================
╔═╗╦  ╦ ╦╔═╗╔╦╗╔═╗╦═╗  ╔═╗╔═╗╦ ╦╔═╗╦═╗  ╔╦╗╔═╗╔═╗╦  ╔═╗
║  ║  ║ ║╚═╗ ║ ║╣ ╠╦╝  ╠═╝║ ║║║║║╣ ╠╦╝   ║ ║ ║║ ║║  ╚═╗
╚═╝╩═╝╚═╝╚═╝ ╩ ╚═╝╩╚═  ╩  ╚═╝╚╩╝╚═╝╩╚═   ╩ ╚═╝╚═╝╩═╝╚═╝
=======================================================


██╗  ██╗ █████╗ ██████╗ ██████╗ ██╗    ██╗ █████╗ ██████╗ ███████╗
██║  ██║██╔══██╗██╔══██╗██╔══██╗██║    ██║██╔══██╗██╔══██╗██╔════╝
███████║███████║██████╔╝██║  ██║██║ █╗ ██║███████║██████╔╝█████╗  
██╔══██║██╔══██║██╔══██╗██║  ██║██║███╗██║██╔══██║██╔══██╗██╔══╝  
██║  ██║██║  ██║██║  ██║██████╔╝╚███╔███╔╝██║  ██║██║  ██║███████╗
╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝  ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝
                                                                  

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
The power measurement system is a linux server equiped
with one or several A/D converters in the form of a PCIe 
card. In our system we use four cards ACPIe-3021 of the
company ADDI-DATA GmbH. The document ./datenblatt_apcie-3021.pdf
contains a description of the card. To use other A/D converters,
the drivers (see ../driver/Readme.txt) must be extended
with the new functionality to control the cards in compatible way.
The document power_sensors.pdf contains a description and
electric schema for the additional devices, which must
be connected to the input channels of A/D converters.
The power tools are tested with four A/D converters in
one server. The system can be expanded with at least two
more cards.
You can order the server by the company or build you own.
We advise the following configuration of the server
for 1-6 cards:
-Use desktop processor with 4 cores  
 (for example Intel(R) Core(TM) i5-3570 CPU). 
-Hard disk for the OS: We use WDC WD5000HHTZ-04N21V1)
-Hard disk for the converted data, output of hpc_converter
 (see ../driver/Readme.txt about hpc_converter):
 We use WDC WD1000DHTZ 
-SSD disk for the output of hpc_measure
 We use Samsung SSD 840 EVO 250GB
-Use PCIe Expansion Backplane if you want to install several 
 more ACPIE-3021 cards than avalaible PCIe slots on a mother board.


========================================================
Acknowledgment

This project is partially realized through EXCESS.
EXCESS is funded by the EU 7th Framework Programme
(FP7/2013-2016) under grant agreement number 611183.
========================================================



