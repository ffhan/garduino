# Garduino

## Why?

I love building intricate systems and integrate technologies that usually don't pair well or at all. Arduino is a microcontroller great for a vast range of projects, but is seriously inequipped to handle talking to APIs. I wanted to research and learn how to achieve such a task in C/C++ language on a machine that can store 40 times less data than the long gone floppy disk.

Combine all of that with my idea for a project that does something in the physical world and it was obvious what I have to do: *A microcontroller system that micromanages all the facets of garden care, whilst talking with a ASP.NET Core web API and exchanging data.*

## Results

I built a [web App](https://github.com/ffhan/GarduinoWeb) (with a public web API in the background) that can talk to any system that is conforming to my "standard", which is defined in this repository, from the code itself to the equipment specs. I built a system that can measure real-life values such as soil moisture, air humidity, air temperature and store that information on microSD card or send it to my web API.

## Capabilities

### Networking
* Login
* Getting requests from network - control your device from anywhere in the world (through Garduino web API)
* Tracking measurements of Garduino system - automatic communication and storage both locally (on microSD card) and on a Garduino server
* Use either DHCP or static IP
* Automatic DHCP lease & login access token renewal
### Plant control
* Plant watering based on precise volumetric measurements (with capacitive soil moisture sensors)
* Precise control of the environment temperature
* Control light cycles
* Track air humidity, temperature, soil moisture

### Software components
1. Garduino main - main Garduino system
   1. Control - the heart of Garduino; controls all other subsystems.
   1. Measuring (name change due) - works with measurement values and measurement implementation
   1. Logging (name change due) - manages Logging subsystem; used to mainly control SD card writing/reading but since its' deprecation represents mostly legacy code until further updates.
   1. Remote - manages remote control
1. Web controller - system that talks to the [web API](http://garduinoproject.azurewebsites.net/)
1. Garduino menus - system that enables easy and efficient creation & use of Garduino GUI classes
   1. Item - base class; defines basic rules for how all objects in *menu system* will behave. Its' direct purpose is separator/simple title
   1. Menu - Item that holds other items. Pressing enter on its' selection opens up its' sub-menu.
      1. ItemNode - base class used in ItemList
	  1. ItemList - doubly linked list that adds any Item to its' tail, while binding elements together. A backbone of an easy to use Menu system.
   1. Setting (Trigger setting) - Item that calls up its' registered Event (a member function).
   1. ValueSetting - Expanded setting that can hold any value and change it. It is an abstract class, since it doesn't know how to increase or decrease the value it holds.
   1. BoolSetting - A ValueSetting that contains a boolean variable.
   1. IntSetting - A ValueSetting that contains an integer variable in range 0-9 (both including).
   1. Dummy - a temporary class that serves as an example of how to bind an Event with Settings and its' values with ValueSettings.
   
## System description

### Physical components
1. Arduino Uno/Mega (additional work is going to be needed to support both. Currently I am focusing exclusively on Arduino Mega support.)
1. Ethernet Shield (SD card optional, depends on Garduino controller version)
1. Capacitive soil moisture sensor
1. DHT22 air temperature/humidity sensor
1. 2 Grow LED lamps
1. 4-channel solid-state relays (for controlling LED lamps, heating, watering systems)
1. 12V solenoid valve (watering system)
1. 12V 300W PTC heating unit
1. HX1838 IR Receiver (remote control system - currently disabled - lack of memory)
1. 5V PNP transistors
1. DS3231 RTC - tracks time with extreme precision - drifts about a minute a year.
1. various other equipment (resistors, capacitors, wires, tubes, CAT cable..)

### Software
* Networking is done through extremely well constructed system
   * JWT token, device ID stored in EEPROM - extremely safe, long-lasting storage - uses less than 512 bytes of memory
   * All http headers, static strings & accessors stored in flash (program) memory
   * Basic JSON builder - done with delegate system that enables content length calculation & sending data to server without storing everything in-memory
   * Basic HTTP parser - parse anything from incoming stream just with "front" and "back" phrases
   * All communication is done consistently with only 1 byte; nothing is stored in big chunks of memory - enables fast throughput of data
   * Supports only basic communication with the server - fetch code, get my ID, login, post an entry, complete a code request - memory limitations
* Basic Garduino system takes up only 30KB. That includes all of its' libraries + dynamic data + EEPROM storage.
* Intuitive, simple & powerful *Menu system* enabling user control through GUI.

### Limitations
* Memory limitation - Only 30KB of program memory is available on Arduino Uno, with additional 512B of EEPROM storage. Dynamic memory (on Arduino Uno) is only 2KB, so working with JSON and HTTP requests/responses is extremely difficult.
* Power limitation - Powering LED lights, 300W heating unit, sensors and network connectivity takes up a considerable amount of power.
* C/C++ are not the best languages to process strings & work with APIs and require real care with how the data is passed.
* Network limitation - Ethernet shield for arduino supports max. 10MB connection, which sometimes doesn't play well with modern routers.
Additionally, some routers don't support DHCP serving to unknown devices, so you have to use static IP, which can easily break.
* Lack of time - I don't have much free time, so pretty much every free second that I had was spent on building both the API and this system.

## Pins

### Digital
TYPE | pin # | System | Description | Note
-----|-------|--------|-------------|------
`OUTPUT` | pin 2 | Sensor | Sensor power pin
`INPUT` | pin 3 | Remote | IR remote sensor pin
`INPUT/OUTPUT` | pin 4 | Logging | Controller pin for data logging module - Ethernet & microSD card reader collision switch
`OUTPUT` | pin 5 | Logging | Logging LED control
`OUTPUT` | pin 6 | Lighting | Controls relay connected to lights
`INPUT` | pin 7 | Sensor | Gets DHT22 sensor readings
`OUTPUT` | pin 8 | Heating | Controls a heating element | **NOT IN USE**
`OUTPUT` | pin 9 | Watering | Controls a solenoid valve (watering system) | **NOT IN USE**
**UNKNOWN** | pin 10 | **UNKNOWN** | Gives out a signal, meaning it's used for something. | Possibly used by SD card reader
`CONTROL` | pin 11 | Logging | SPI MOSI | Taken, don't use this pin - RTC, Ethernet
`CONTROL` | pin 12 | Logging | SPI MISO | Taken, don't use this pin - RTC, Ethernet
`CONTROL` | pin 13 | Logging | SPI clock | Taken, don't use this pin - RTC, Ethernet


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
0 | HOLD 0 | User mode | GLOBAL LOCK | Completely automatic system, user can request measurements and time. Switches between `User mode` and `Admin mode`
1 | 1 | Admin mode | Lighting state | Turns lights on/off if `Light admin` privileges are on
2 | 2 | Admin mode | Logging | Turns logging on/off | Disabled, see `Memory limitations`
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
200 | none | Restricted | Get device ID | Gets device ID from the current logged user with device name

## TODO
- [ ] *Garduino main* and *Menu* subsystem merge
- [ ] Fan speed setting
- [ ] Heating system
- [ ] Watering system
- [ ] Debug mode - prints out data from under the hood
- [ ] Consecutive measurements
- [ ] Isolated measurements frequencies (soil moisture/temperature/humidity/etc..)

## Theory
* PWM heating element custom resolution: 
  *Resolution definition within our current context* - frequency (or period) of heating element control. One period contains switching the element on and off exactly one time.
  
  Relay - Songle Relay, rated for `250VAC`/`30VDC` & `10A` of current. [Source](https://www.parallax.com/sites/default/files/downloads/27115-Single-Relay-Board-Datasheet.pdf)
  
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
   
   We can search for the optimal value we want to achieve. Considering this is just a minimum, I can safely assume that I want a full year of normal operations. This includes the fact that the electrical operations lifespan is 100 times bigger than the mechanical. All those factors mean we will in the worst possible scenario get at least a year out of one relay.
   
   We substitute f(t) = 365, and get that t = 10.512 minutes, which is approximately 10 minutes and 31 seconds.
   
   This value is the minimum resolution we will use for this project.
