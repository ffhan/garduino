# Garduino

## Pins

### Digital
TYPE | pin # | System | Description | Note
-----|-------|--------|-------------|------
`OUTPUT` | pin 2 | Sensor | Sensor power pin
`INPUT` | pin 3 | Remote | IR remote sensor pin
`INPUT/OUTPUT` | pin 4 | Logging | Controller pin for data logging module
`OUTPUT` | pin 5 | Logging | Logging LED control
`OUTPUT` | pin 6 | Lighting | Controls relay connected to lights
`INPUT` | pin 7 | Sensor | Gets DHT22 sensor readings
`OUTPUT` | pin 8 | Heating | Controls a heating element | **NOT IN USE**
`OUTPUT` | pin 9 | Watering | Controls a solenoid valve (watering system) | **NOT IN USE**
**UNKNOWN** | pin 10 | **UNKNOWN** | Gives out a signal, meaning it's used for something. | Possibly used by SD card reader
`CONTROL` | pin 11 | Logging | SPI MOSI | Taken, don't use this pin
`CONTROL` | pin 12 | Logging | SPI MISO | Taken, don't use this pin
`CONTROL` | pin 13 | Logging | SPI clock | Taken, don't use this pin


### Analog
TYPE | pin # | System | Description | Note
-----|-------|--------|-------------|------
`OUTPUT` | pin A0 | System | Controls red ticker color
`INPUT` | pin A1 | Sensor | Gets soil moisture sensor readings
`OUTPUT` | pin A2 | System | Controls green ticker color
`CONTROL` | pin A4 | Sensor | RTC I2C (SDA) connection | **CRITICAL**
`CONTROL` | pin A5 | Sensor | RTC I2C (SCL) connection | **CRITICAL**

## Modes
MODE CODE | REMOTE BUTTON| MODE TYPE | MODE NAME | DESCRIPTION | Note
----------|--------------|-----------|-----------|-------------|-----
0 | 0 | User mode | GLOBAL LOCK | Completely automatic system, user can request measurements and time. Switches between `User mode` and `Admin mode`
1 | 1 | Admin mode | Lighting state | Turns lights on/off if `Light admin` privileges are on
2 | 2 | Admin mode | Logging | Turns logging on/off
3 | 3 | Admin mode | Heating state | Turns heating on/of if `Heat admin` privileges are on | Currently not in use
4 | HOLD 1 | Admin mode | Light admin | Controls lighting privileges
5 | 5 | User mode | Measure | Measures input from sensors and logs the data
6 | HOLD 3 | Admin mode | Heat admin | Controls heating privileges
7 | 4 | Admin mode | Watering state | Turns watering on/off if `Watering admin` privileges are on. | Currently not in use
8 | 8 | User mode | Time | Retrieve system time
9 | 6 | Admin mode | Fan speed set | Starts fan speed setting | Currently not in use
10 | HOLD 4 | Admin mode | Watering admin | Controls watering privileges | Currently not in use
11 | HOLD 6 | Admin mode | Fan admin | Controls fan privileges | Currently not in use
12345 | none | Restricted | STOP command | Stops the whole processor and all operations | Accessible only through serial PC connection

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
