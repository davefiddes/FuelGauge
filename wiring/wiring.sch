EESchema Schematic File Version 5
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Wiring Harness"
Date "2019-07-28"
Rev "1.0"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Conn_01x01_Female J1
U 1 1 5D3C8DB9
P 1850 1100
F 0 "J1" H 1742 875 50  0001 C CNN
F 1 "Gauge S - 1/4\" spade female" H 1730 1035 50  0000 C CNN
F 2 "" H 1850 1100 50  0001 C CNN
F 3 "~" H 1850 1100 50  0001 C CNN
	1    1850 1100
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x03_Female J9
U 1 1 5D3C95C1
P 1850 3300
F 0 "J9" H 1742 2975 50  0001 C CNN
F 1 "COM" H 1742 3066 50  0000 C CNN
F 2 "" H 1850 3300 50  0001 C CNN
F 3 "~" H 1850 3300 50  0001 C CNN
	1    1850 3300
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J2
U 1 1 5D3CC234
P 1850 1250
F 0 "J2" H 1742 1025 50  0001 C CNN
F 1 "Gauge GND - 1/4\" spade female" H 1775 1180 50  0000 C CNN
F 2 "" H 1850 1250 50  0001 C CNN
F 3 "~" H 1850 1250 50  0001 C CNN
	1    1850 1250
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J3
U 1 1 5D3CD00B
P 1850 1400
F 0 "J3" H 1742 1175 50  0001 C CNN
F 1 "Gauge +12V - 1/4\" spade female" H 1785 1340 50  0000 C CNN
F 2 "" H 1850 1400 50  0001 C CNN
F 3 "~" H 1850 1400 50  0001 C CNN
	1    1850 1400
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J5
U 1 1 5D3D5EF0
P 1850 1850
F 0 "J5" H 1742 1625 50  0001 C CNN
F 1 "Fuel Low +12V  - 2.8x0.8mm spade female" H 1865 1760 50  0000 C CNN
F 2 "" H 1850 1850 50  0001 C CNN
F 3 "~" H 1850 1850 50  0001 C CNN
	1    1850 1850
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J4
U 1 1 5D3D5EF1
P 1850 1700
F 0 "J4" H 1742 1475 50  0001 C CNN
F 1 "Fuel Low - 2.8x0.8mm spade female" H 1720 1595 50  0000 C CNN
F 2 "" H 1850 1700 50  0001 C CNN
F 3 "~" H 1850 1700 50  0001 C CNN
	1    1850 1700
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Male J7
U 1 1 5D3CD496
P 1850 2550
F 0 "J7" H 2228 2968 50  0001 R CNN
F 1 "Car Sender - 1/4\" spade male" H 2445 2605 50  0000 R CNN
F 2 "" H 2200 2900 50  0001 C CNN
F 3 "~" H 2200 2900 50  0001 C CNN
	1    1850 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x01_Male J8
U 1 1 5D3C90B0
P 1850 2700
F 0 "J8" H 1428 2618 50  0001 R CNN
F 1 "Car GND - 1/4\" spade male" H 2450 2750 50  0000 R CNN
F 2 "" H 1400 2550 50  0001 C CNN
F 3 "~" H 1400 2550 50  0001 C CNN
	1    1850 2700
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x01_Male J6
U 1 1 5D3CDDB1
P 1850 2400
F 0 "J6" H 1728 3218 50  0001 R CNN
F 1 "Car +12V  - 1/4\" spade male" H 2460 2465 50  0000 R CNN
F 2 "" H 1700 3150 50  0001 C CNN
F 3 "~" H 1700 3150 50  0001 C CNN
	1    1850 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 1400 2700 1400
Text Notes 3925 1100 2    50   ~ 0
Yellow/Blue - 16/0.2
Text Label 4880 1750 0    50   ~ 0
Gauge Out
Text Label 4880 1850 0    50   ~ 0
Gauge GND
Wire Wire Line
	2700 1850 3930 1850
Connection ~ 2700 1850
Wire Wire Line
	2700 1400 2700 1850
Wire Wire Line
	2050 1850 2700 1850
Text Label 4880 2650 0    50   ~ 0
Car GND
Text Label 4880 2550 0    50   ~ 0
Sender In
Text Label 4880 2450 0    50   ~ 0
COM GND
Text Label 4880 2350 0    50   ~ 0
COM RX
Wire Wire Line
	5380 1750 4780 1750
Wire Wire Line
	4780 2150 3930 2400
Wire Wire Line
	5380 2150 4780 2150
Wire Wire Line
	3930 3200 2050 3200
Wire Wire Line
	4780 2250 3930 3200
Wire Wire Line
	5380 2250 4780 2250
Wire Wire Line
	4780 2350 5380 2350
Wire Wire Line
	3930 3300 4780 2350
Wire Wire Line
	2050 3300 3930 3300
Wire Wire Line
	3930 3400 2050 3400
Wire Wire Line
	4780 2450 3930 3400
Wire Wire Line
	5380 2450 4780 2450
Wire Wire Line
	3930 2700 2050 2700
Wire Wire Line
	4780 2650 3930 2700
Wire Wire Line
	5380 2650 4780 2650
Wire Wire Line
	3930 2400 2050 2400
Wire Wire Line
	4780 2050 3930 1850
Wire Wire Line
	5380 2050 4780 2050
Wire Wire Line
	3930 1700 2050 1700
Wire Wire Line
	4780 1950 3930 1700
Wire Wire Line
	5380 1950 4780 1950
Wire Wire Line
	3930 1250 2050 1250
Wire Wire Line
	5380 1850 4780 1850
Wire Wire Line
	3930 1100 2050 1100
Wire Wire Line
	4780 1850 3930 1250
Wire Wire Line
	4780 1750 3930 1100
$Comp
L Connector:Conn_01x10_Female J10
U 1 1 5D3C8106
P 5580 2150
F 0 "J10" H 5608 2126 50  0000 L CNN
F 1 "Conn_01x10_Female" H 5608 2035 50  0000 L CNN
F 2 "" H 5580 2150 50  0001 C CNN
F 3 "~" H 5580 2150 50  0001 C CNN
	1    5580 2150
	1    0    0    -1  
$EndComp
Text Label 4880 2050 0    50   ~ 0
Gauge +12V
Text Label 4880 1950 0    50   ~ 0
Fuel Low
Text Label 4880 2150 0    50   ~ 0
Car +12V
Text Label 4880 2250 0    50   ~ 0
COM TX
Wire Wire Line
	2050 2550 5380 2550
Text Notes 3925 1250 2    50   ~ 0
Black - 16/0.2
Text Notes 3925 1700 2    50   ~ 0
Yellow/Blue - 16/0.2
Text Notes 3925 1850 2    50   ~ 0
Red - 16/0.2
Text Notes 3925 2400 2    50   ~ 0
Red - 16/0.2
Text Notes 3925 2550 2    50   ~ 0
Yellow/Blue - 16/0.2
Text Notes 3925 2700 2    50   ~ 0
Black - 16/0.2
Text Notes 3925 3200 2    50   ~ 0
Green - 7/0.2
Text Notes 3925 3300 2    50   ~ 0
Brown - 7/0.2
Text Notes 3925 3400 2    50   ~ 0
Black - 7/0.2
$EndSCHEMATC
