EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:ltc2630
LIBS:lt3010-5
LIBS:ad7280a
LIBS:myopto
LIBS:ina197
LIBS:hrs1k
LIBS:ntd5867nl
LIBS:SPI_Flash
LIBS:si8422
LIBS:tja1052i
LIBS:nup2105l
LIBS:lt3990-3
LIBS:inductor-coilcraft
LIBS:tle4266
LIBS:ft232rq
LIBS:lm2936
LIBS:bms-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 11
Title ""
Date "2 may 2016"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 4000 2950 0    60   ~ 0
GND
$Comp
L C C45
U 1 1 56016EB4
P 4200 4600
F 0 "C45" H 4200 4700 40  0000 L CNN
F 1 "1uF" H 4206 4515 40  0000 L CNN
F 2 "~" H 4238 4450 30  0000 C CNN
F 3 "~" H 4200 4600 60  0000 C CNN
	1    4200 4600
	1    0    0    -1  
$EndComp
Text Label 4000 2300 0    60   ~ 0
VIN
Text HLabel 7700 2400 2    60   Input ~ 0
VBAT
Text Label 4850 5500 0    60   ~ 0
GND
Text HLabel 3050 5500 0    60   Input ~ 0
GND
$Comp
L C C44
U 1 1 56017B92
P 3850 4600
F 0 "C44" H 3850 4700 40  0000 L CNN
F 1 "0.1uF" H 3856 4515 40  0000 L CNN
F 2 "~" H 3888 4450 30  0000 C CNN
F 3 "~" H 3850 4600 60  0000 C CNN
	1    3850 4600
	1    0    0    -1  
$EndComp
$Comp
L ZENER D6
U 1 1 5601F9AB
P 5000 2600
F 0 "D6" H 5000 2700 50  0000 C CNN
F 1 "ZENER 55V" H 5000 2500 40  0000 C CNN
F 2 "~" H 5000 2600 60  0000 C CNN
F 3 "~" H 5000 2600 60  0000 C CNN
	1    5000 2600
	0    -1   -1   0   
$EndComp
$Comp
L C C49
U 1 1 56CED685
P 7600 4850
F 0 "C49" H 7600 4950 40  0000 L CNN
F 1 "0.22uF100V" H 7606 4765 40  0000 L CNN
F 2 "~" H 7638 4700 30  0000 C CNN
F 3 "~" H 7600 4850 60  0000 C CNN
	1    7600 4850
	1    0    0    -1  
$EndComp
Text Label 3650 4100 0    60   ~ 0
3.3V
Text HLabel 3050 4100 0    60   Output ~ 0
3.3V
$Comp
L SWITCH_INV SW2
U 1 1 56D09D61
P 7000 2300
F 0 "SW2" H 6800 2450 50  0000 C CNN
F 1 "SWITCH_INV" H 6850 2150 50  0000 C CNN
F 2 "~" H 7000 2300 60  0000 C CNN
F 3 "~" H 7000 2300 60  0000 C CNN
	1    7000 2300
	1    0    0    -1  
$EndComp
Text Notes 5600 1950 0    60   ~ 0
This switch protects the AD7280A stack against\nreverse currents when assembling the battery pack.
NoConn ~ 7500 2200
$Comp
L C C47
U 1 1 57127BAB
P 4550 4600
F 0 "C47" H 4550 4700 40  0000 L CNN
F 1 "22uF16V" H 4556 4515 40  0000 L CNN
F 2 "~" H 4588 4450 30  0000 C CNN
F 3 "~" H 4550 4600 60  0000 C CNN
	1    4550 4600
	1    0    0    -1  
$EndComp
Text Label 5300 4400 0    60   ~ 0
VIN
$Comp
L R R73
U 1 1 57127BF8
P 8100 4500
F 0 "R73" V 8180 4500 40  0000 C CNN
F 1 "374K" V 8107 4501 40  0000 C CNN
F 2 "~" V 8030 4500 30  0000 C CNN
F 3 "~" H 8100 4500 30  0000 C CNN
	1    8100 4500
	1    0    0    -1  
$EndComp
$Comp
L C C48
U 1 1 57127C0A
P 5450 4850
F 0 "C48" H 5450 4950 40  0000 L CNN
F 1 "2,2uF100V" H 5456 4765 40  0000 L CNN
F 2 "~" H 5488 4700 30  0000 C CNN
F 3 "~" H 5450 4850 60  0000 C CNN
	1    5450 4850
	1    0    0    -1  
$EndComp
$Comp
L DIODE D7
U 1 1 57127C17
P 5350 2300
F 0 "D7" H 5350 2400 40  0000 C CNN
F 1 "DIODE" H 5350 2200 40  0000 C CNN
F 2 "~" H 5350 2300 60  0000 C CNN
F 3 "~" H 5350 2300 60  0000 C CNN
	1    5350 2300
	-1   0    0    1   
$EndComp
$Comp
L INDUCTOR L1
U 1 1 57127C26
P 5050 4600
F 0 "L1" V 5000 4600 40  0000 C CNN
F 1 "33µH" V 5150 4600 40  0000 C CNN
F 2 "~" H 5050 4600 60  0000 C CNN
F 3 "~" H 5050 4600 60  0000 C CNN
	1    5050 4600
	1    0    0    -1  
$EndComp
Text Label 7600 4400 2    60   ~ 0
BD
Text Label 5300 3800 2    60   ~ 0
BD
Text Label 7600 3950 2    60   ~ 0
GND
Text Label 7000 5100 2    60   ~ 0
SW
Text Label 5300 5100 2    60   ~ 0
SW
Text Label 7600 4300 2    60   ~ 0
PG
$Comp
L R R72
U 1 1 57128302
P 3450 4600
F 0 "R72" V 3530 4600 40  0000 C CNN
F 1 "100K" V 3457 4601 40  0000 C CNN
F 2 "~" V 3380 4600 30  0000 C CNN
F 3 "~" H 3450 4600 30  0000 C CNN
	1    3450 4600
	1    0    0    -1  
$EndComp
Text Label 3150 5000 2    60   ~ 0
PG
$Comp
L LT3990-3.3 U5
U 1 1 5714C0AF
P 6500 4900
F 0 "U5" H 6300 4850 60  0000 C CNN
F 1 "LT3990-3.3" H 6500 4750 60  0000 C CNN
F 2 "~" H 6500 4900 60  0000 C CNN
F 3 "~" H 6500 4900 60  0000 C CNN
	1    6500 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 2300 5000 2400
Connection ~ 5000 2300
Wire Wire Line
	5000 2950 5000 2800
Wire Wire Line
	7700 2400 7500 2400
Wire Wire Line
	6500 2300 6350 2300
Wire Wire Line
	5650 2300 5550 2300
Wire Wire Line
	3050 4100 5800 4100
Wire Wire Line
	5650 4100 5650 4200
Wire Wire Line
	5650 4200 5800 4200
Connection ~ 5650 4100
Wire Wire Line
	5800 5500 5800 4800
Wire Wire Line
	3050 5500 8100 5500
Wire Wire Line
	7200 4100 8100 4100
Wire Wire Line
	8100 4100 8100 4250
Wire Wire Line
	5800 4600 5450 4600
Wire Wire Line
	5450 4400 5450 4650
Wire Wire Line
	5450 5050 5450 5500
Connection ~ 5450 5500
Wire Wire Line
	7200 4400 7600 4400
Wire Wire Line
	7200 4600 7600 4600
Wire Wire Line
	7600 4600 7600 4650
Wire Wire Line
	7600 5100 7600 5050
Wire Wire Line
	7000 5100 7600 5100
Wire Wire Line
	7200 5100 7200 4800
Connection ~ 5050 4100
Wire Wire Line
	5050 3800 5300 3800
Wire Wire Line
	8100 5500 8100 4750
Connection ~ 5800 5500
Wire Wire Line
	7200 3950 7600 3950
Wire Wire Line
	5050 3800 5050 4300
Wire Wire Line
	4550 4100 4550 4400
Wire Wire Line
	4550 4800 4550 5500
Connection ~ 7200 5100
Wire Wire Line
	5050 4900 5050 5100
Wire Wire Line
	4200 4100 4200 4400
Connection ~ 4550 4100
Wire Wire Line
	4200 4800 4200 5500
Connection ~ 4550 5500
Connection ~ 4200 5500
Wire Wire Line
	3850 4800 3850 5500
Connection ~ 3850 5500
Wire Wire Line
	3850 4400 3850 4100
Connection ~ 3850 4100
Wire Wire Line
	5800 4400 5300 4400
Connection ~ 5450 4400
Connection ~ 5450 4600
Wire Wire Line
	7200 4300 7600 4300
Connection ~ 4200 4100
Connection ~ 3450 4100
Wire Wire Line
	3450 5000 3150 5000
Wire Wire Line
	5050 5100 5300 5100
Wire Wire Line
	3450 4100 3450 4350
Wire Wire Line
	3450 5000 3450 4850
Connection ~ 5000 2950
Wire Wire Line
	6000 2950 4000 2950
Wire Wire Line
	6000 2950 6000 2550
$Comp
L EMI_FILTER2 FI1
U 1 1 56CE826B
P 6000 2300
F 0 "FI1" H 6000 2400 50  0000 C CNN
F 1 "EMI_FILTER2" H 6050 2150 50  0000 L CNN
F 2 "~" H 6000 2300 60  0000 C CNN
F 3 "~" H 6000 2300 60  0000 C CNN
	1    6000 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 2300 5150 2300
$EndSCHEMATC