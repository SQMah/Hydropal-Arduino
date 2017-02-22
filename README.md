# Hydropal: Arduino Branch

## Description
This is the repository Arduino code for the Hydropal smart water bottle JA company. The code will primarily allow the Arduino Pro Mini board to read the ml/sec of liquids going though the nozzle of the bottle in order to measure the millitres of water that a person drinks throughout the day. It also syncs that data to a mobile device.

**Code for the water bottle itself is in the 'Hydropal folder!**

## Usage
Detailed instructions can be found here: https://github.com/Hydropal/Hydropal-Arduino/wiki

## Current Functionality
- [x] Checks sensor reading every second to find ml/sec passing through sensor
- [x] Adds sensor reading to total ml of liquid consumed throughout the day
- [x] LED indicators for total volume consumed, for 5 LEDs
- [x] Reset total counter midnight every day
- [x] Update correct date and time over Bluetooth
- [x] Update user preferences over Bluetooth
- [x] Send multiple day data over Bluetooth
- [X] Pulsing LED to remind user to sync
- [X] Blinking LED to remind user to drink
- [X] Proper sleep wake to control LED

## To-do
🎉 Nothing at the moment!

## Changelog:

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
