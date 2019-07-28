# Calibration And Programming Guide

## Connection

Connect to the Fuel Gauge using a 5V TTL capable USB serial device. Any common device should work whether based on FTDI, CP210x or CH340 chipsets. Use 9600 baud 8 bits and no-parity. TTL serial works surprisingly well even in a noisy vehicle environment and it saves the expense of an RS232 serial transceiver for a function that is rarely used.

## Usage

The fuel gauge offers a simple text based programming interface. The commands supported can be obtained by pressing `u` and then Enter:

```
FuelGauge Version a6408e

Press "u" for usage

u
Usage:
p               - Program mode
r               - Run mode
d               - Display current tank input value and output gauge value
g <Value>       - Output raw gauge value
t               - One shot test map the current tank input to the gauge output
i <Bin> <Value> - Set the input bin number to a specific linear tank value
o <Bin> <Value> - Set the output bin number to a specific value
m               - Display the input and output maps
s               - Save input and output maps to persistent storage
l               - Load input and output maps from persistent storage
f <Value>       - Set the low fuel limit
c               - Continuously output values as the gauge runs
u               - This usage information

OK
```

## Command Details

 * `p` - Changes from the normal running of the gauge to program mode. In this mode the sender input is no longer mapped to the output. This allows the gauge output to be manually altered. In particular this allows the output map to be created.

 * `r` - Restores the gauge to normal running operation

 * `d` - Display the raw values being read from the sender input, being output to the gauge and whether the device is in program or run mode. For example: `Tank: 0x1dbc Gauge: 0xffc0 Mode: Run`

 * `g` - Only available in program mode. This sets the gauge output to the specified 4-digit hex value. Useful for verifying what value is required for a given fuel gauge display.

 * `t` - Available in program mode this allows a one-shot test of mapping the current sender input through to the gauge output. This is handy for testing a map in response to a change in the sender input.

 * `i` - Used to configure a specific input map bin. The input map consists of 9 bins numbered 0 to 8. Each bin maps a given raw sender input value to a fixed _real_ fuel level value. For example bin 0 corresponds to empty, 4 to 50% full and 8 is 100% full.

 * `o` - Used to configure a specific output map bin. The output map consists of 9 bins numbered 0 to 8. In contrast to the input map the output map takes a _real_ fuel value and determines the raw gauge output value that is required.

 * `m` - Used to display the input and output maps and the configured low fuel light level

 * `s` - Save the current configuration to EEPROM. If this is not done it will be lost at the next power cycle.

 * `l` - Load the current configuration from EEPROM. This can be used if an error has been made during programming.

 * `f` - Set the fuel level which will cause the low fuel level warning lamp to illuminate. The value is in _real_ linear fuel level values. So 8000 means 50%, 2000 means 12.5% and so on.

 * `c` - Continuous mode will continuously log the sender input, actual fuel level and gauge output to the serial console several times a second. This allows rapid changes in the values to be quantified. This only available in run mode and when the sender input is not disconnected (a sender value of 0xffff).

 ## Calibration Procedure

 __**WARNING: Calibrating fuel gauges will likely involve moving measuring quantities of fuel around a vehicle. Please ensure appropriate ventilation, safety equipment and fire extinguishers. **__

 The first step is to calibrate the gauge output. Put the gauge into program mode and use the `g` command to set various values. The device is capable of generating a PWM output from 0% to 100% duty cycle. It is likely that the 0% gauge level will require a modest current on most hot-wire gauges. Note the gauge output values that achieve a given reading on the actual fuel gauge. Use these to program the output map. The map should work well for gauges which are divided into 1/8ths but may require a spreadsheet to figure out the values for a different graduations (e.g. 1/10ths).

 The next step is to determine the fuel tank calibration. To do this requires an empty fuel tank. Measure the empty fuel level using the `d` command and then store this in bin 0 of the input map using the `i` command. Add 1/8th of a tank of fuel using a calibrated measure and repeat until full.

 Finally set the low fuel warning level and save the map. Using a logging terminal like PuTTY and printing out a copy of the map with `m` will allow the maps to be restored in the case of an error.

