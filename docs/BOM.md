Fuel Gauge Bill of Materials
--------------------------------

Below is the Bill of Materials for the Fuel Gauge 1.0 board:

```
Ref(s)      Value                                   RS Components P/N
-------------------------------------------------------------------------------
C1,C2       1nF 100Vdc Multi-layer Ceramic 10%          699-5109
C3          1uF 100Vdc Polyester 10%                    126-2282
C4          22uF Aluminium Electrolytic 35Vdc           715-2729
C5          47uF 6.3V Radial Tantalum                   868-5690
C6          100nF 50Vdc Multi-layer Ceramic 10%         538-1310
D1,D2,D4,D6 1N4001                                      628-8931
D3          5V1 1W Zener Diode                          166-2747
D5          12V 1W Zener Diode                          687-5459
D7          22V 1.3W Zener Diode                        813-3915
J1          Molex KK 254 7395, 2.54mm Pitch, 10 Way     173-2994
            1 Row, Right Angle PCB Header
J2          6 Way 1 Row 2.54mm Pitch header
Q1          BC547 NPN Transistor TO-92                  671-1113
Q2          PHB55N03LT N-MOSFET transistor TO-263
R1          120R 1/4W 1% thin-film resistor
R2          2K2 1/4W 1% thin-film resistor
R3          1K 1/4W 1% thin-film resistor
R4          10R 1/4W 1% thin-film resistor
R5          4K7 1/4W 1% thin-film resistor
R6          10K 1/4W 1% thin-film resistor
RV1         Panasonic ERZV05D220 22V MOV                226-7817
U1          LM317L 100mA Linear regulator TO92          661-6796
U2          LM7805 700mA Linear Regulator TO220         785-6762
U3          Microchip PIC12F1840-I/P DIP-8              743-2697
```

Notes:
 * Q2 may be hard to find. Any power N-channel MOSFET in TO-263 package should suffice. I used a device recovered from a PC motherboard as that what I had available.