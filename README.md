# garduino

## Pins
* `OUTPUT` Digital pin 2 - Sensor power pin
* `OUTPUT` Digital pin 6 - Controls relay connected to lights
* `INPUT` Digital pin 7 - Gets DHT22 sensor readings
* `INPUT/OUTPUT` Digital pin 4 - Controller pin for data logging module
* `INPUT` Analog pin A1 - Gets soil moisture sensor readings

## Modes
* 0 - **Normal mode**: arduino controls when to turn lights on/off, when to measure and log data. Completely automatic system
* 1 - **Light control**: controls lights, disables the normal mode
  * 10 - turns lights off
  * 11 - turns lights on
* 2 - **Data logging switch**
  * 20 - enables safe removal of an SD card from the slot. Disables data logging and measurements. Closes a file for writing.
  * 21 - enables data logging and measurements. Requires an entered SD card, opens a file for writing.
* 3 - **Time control**
  * 30 - print current rtc time
  * 31 - sets current rtc time to system time
* 12345 - **Closing switch**: disables all arduino action (Stops everything).
