# Hydropal: Arduino Branch

## Description
This is the Arduino code for the Hydropal smart water bottle JA company. The code will primarily allow the Arduino Pro Mini board to read the ml/sec of liquids going though the nozzle of the bottle in order to measure the millitres of water that a person drinks throughout the day. It also syncs that data to a mobile device.

## Usage
Detailed instructions can be found here: https://github.com/Hydropal/Hydropal-Arduino/wiki

## Current Functionality
- [X] Checks sensor reading every second to find ml/sec passing through sensor
- [X] Adds sensor reading to total ml of liquid consumed throughout the day
- [X] Reset total counter midnight every day
- [X] Update correct date and time over Bluetooth
- [X] Update user preferences over Bluetooth
- [X] Send multiple day data over Bluetooth
- [X] Pulsing LED to remind user to sync
- [X] Blinking LED to remind user to drink
- [X] Proper sleep wake to control LED
- [X] Adjusts internal data to reflect time changes and disreprencies (should work in different timezones)
- [X] Checks if sync data is valid

## Deprecated Functionality
- [X] LED indicators for total volume consumed, for 5 LEDs (present in Alpha 2.0)

## To-do
- [ ] Flow rate calibration

## Changelog
## Release 1.0
- Checks if time from device mismatches internal clock and shifts volume array backwards and forwards as necessary
- Checks if data sent from device is corrupted over transmission and should help prevent errors that require a hard reset
- Change baud rate to 38400 instead of 112000 to lower chances of data corruption during transmission

## Alpha 6.0
- Arduino checks for the largest value and sends it back to the device
- LED pulses when need sync
- LED blinks when need drink

## Alpha 5.0
- Groundwork for sleep wake times
- Groundwork for pulsing LED
- Multiple day data is sent and reset

## Alpha 4.0
- Data is sent to mobile device over Bluetooth
- Data from mobile device is parsed: things such as time and user preferences are updated

## Alpha 3.0
- Water consumed is now reset every midnight

### Alpha 2.0
- LEDs can indicate how much has been consumed, one for each 1 L
- LEDs in progress of being filled up blink

### Alpha 1.0
- Initial release
- Measures flowrate in ml/sec and adds up to total water consumed
