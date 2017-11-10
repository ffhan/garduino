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
* 4 - **Heating control**
  * 40 - disable heating
  * 41 - enable heating
* 12345 - **Closing switch**: disables all arduino action (Stops everything).

## Theory
* PWM heating element custom resolution: 
  *Resolution definition within our current context* - frequency (or period) of heating element control. One period contains switching the element on and off exactly one time.
  
  Relay we are using in this project is Songle Relay, rated for `250VAC`/`30VDC` & `10A` of current. [Source](https://www.parallax.com/sites/default/files/downloads/27115-Single-Relay-Board-Datasheet.pdf)
  
  According to the datasheet, my particular relay has a rating of Min. 10<sup>5</sup> electrical and 10<sup>7</sup> mechanical operations in its' lifespan.
  On of the main goals is to increase the lifespan of my equipment, so calculations are neccessary to ensure that goal is fulfilled.
  
  ### Minimal resolution
   * Unit of time: t[1 minute]
   * Min. lifespan: 10<sup>5</sup> operations.
   * Function returns unit of time[1 day].
   
   Function describing (approximate) lifespan of a relay under our operation:
   
   *f(t [in minutes]) = min. operations * t / ( number of operations in a cycle * number of minutes in an hour * number of hours in a day)*
   
   Which gives us:
   
   *f(t) = 10<sup>5</sup> * t / (2 * 60 * 24)*
   
   We can search for the optimal value we want to achieve. Considering this is just a minimum, I can safely assume I want a full year of normal operations. This includes the fact that the electrical operations lifespan is 100 times bigger than the mechanical. All those factors mean we will in the worst possible scenario get at least a year out of one relay.
   
   We substitute f(t) = 365, and get that t = 10.512 minutes, which is approximately 10 minutes and 31 seconds.
   
   This value is the minimum resolution we will use for this project.
