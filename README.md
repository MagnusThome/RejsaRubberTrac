# RejsaRubberTrac  

A cheap wireless tire temperatures and suspension movement logger built from a small cpu/bluetooth board + a 16-zone temperature sensor + a distance sensor.

*__Intended to track/view/logg tire temperatures on race- and trackday cars.__*

For around $100 per wheel you can build your own wireless sensors to log your driving. You'll then easily see if you are overheating the shoulder on one wheel in some particular corner, if one of the rear tires stays a bit too cold or if you need to adjust camber. And many more things about how you attack a course and how your car is set up.

- Temperatures are measured at sixteen different points over each tire's full width.
- Support for a distance sensor is also included for tracking/logging suspension movements.
- All data is available over Bluetooth Low Energy BLE.

The two major track loggers for mobile phones <a href=http://www.gps-laptimer.de>__Harry's Laptimer__</a> and <a href=http://www.racechrono.com>__Racechrono__</a> already have test units and are well on their way adding support for RejsaRubberTrac.

Watch this short video:  

<a href="http://www.youtube.com/watch?v=Yuy62oPXugs"><img src=images/earlyharrysbetayoutube.jpg></a>

Here below Racechrono is performing stress tests with four RejsaRubberTracs plus a Garmin GLO 10Hz GPS and an OBDLink MX. Antti reported these good news from the tests with a Google Pixel 2 phone and RaceChrono Pro: *"The ELM327 protocol for OBD-II is quite sensitive for lag/ping time, so it was affected by all the other Bluetooth connections as I expected. The highest update rate for OBDLink MX was still good, but the update rate fluctuates quite a bit, but even at lowest it was still faster than the Bluetooth LE OBD-II reader I tested. The update rate did not fluctuate on that one too much."*

<img src=images/racechronostresstests.jpg>

# Easy to build
<img align="right" width="231" src="images/partsizes.jpg">

To build it you need to purchase three small boards, connect a few wires between the boards and then finally upload the done and dusted code you find here to the main board using a USB cable. Detailed instructions further below. Then you're up and running! You probably want to get a small enclosure for it though before you mount it to your car!

- __Temperature IR-arraysensor MLX90621 GY-906LLC-BAB__    
(60 degrees field of view ends with BAB, 120 degrees with BAA)  
<a href="https://www.aliexpress.com/w/wholesale-MLX90621.html?SortType=price_asc&SearchText=MLX90621">www.aliexpress.com</a> (~ $47)  
<a href="https://eckstein-shop.de/GY-906LLC-BAB-IR-Array-Temperature-Sensor-Module">www.eckstein-shop.de</a> (~ 55€)  
<a href="https://www.mouser.se/ProductDetail/Melexis/MLX90621ESF-BAB-000-SP?qs=sGAEpiMZZMucenltShoSnqRDBUuVzCzKa3Zx6liDTBzqGCq1%252bjAaAA%3d%3d">www.mouser.com</a> (~ 35€ excl daugther board)  
<a href="https://www.digikey.com/product-detail/en/melexis-technologies-nv/MLX90621ESF-BAB-000-TU/MLX90621ESF-BAB-000-TU-ND/4968086">www.digikey.com</a> (~ 36€ excl daugther board)  

- __Laser distance sensor VL53L0XV2__  
<a href="https://www.ebay.com/sch/i.html?_nkw=vl53l0xv2&_sop=15">www.ebay.com</a> (~ $5)  
<a href="https://www.aliexpress.com/w/wholesale-vl53l0xv2.html?SortType=price_asc&SearchText=vl53l0xv2">www.aliexpress.com</a> (~ $2.50)  

- __CPU and Bluetooth board Adafruit BlueFruit nRF52__  
<a href="https://www.mouser.com/ProductDetail/485-3406">www.mouser.com</a> (~ $25)  
<a href="https://www.adafruit.com/product/3406">www.adafruit.com</a> (~ $25)  

- __OPTIONAL: Rechargable 3,7V Lipo battery__  
250mAh or larger with JST-PH 2.0mm connector  
<a href="https://www.ebay.com/sch/i.html?_nkw=3.7V+lipo+battery+JST-PH+2.0&_sop=15">www.ebay.com</a> (~ $8)  

# Supplying power to boards

The CPU board and the two sensor boards are all powered by connecting power to the CPU board's USB micro connector. The CPU board also has a connector for a Lipo battery - which is automatically charged via the USB connector - so the whole system can run completey wireless with it's own power source for roughly 24 hours with a 500mAh 3,7V Lipo battery. The system can also be run directly from the car's power, not using any Lipo battery, but then a 12 volt to USB 5 volt converter must be added.

# Positioning the sensors

_Click to view a larger version!_  
![Display](https://raw.githubusercontent.com/MagnusThome/RejsaRubberTrac/master/images/sensorpositioning.jpg)

The wider the tire the further away you need to position the sensor to get the full tire width within the sensors field of view (FOV). There are two versions of the temperature sensor, one with 60 degrees of FOV and one wide version with 120 degrees FOV. The latter can be a better choice if you have very wide tires and need to mount the sensors pretty close. Here below is a table with mounting distances at different tire widths.

__NOTE: You do not have to mount the sensor at exactly the necessary distance!! The distance is simply fine tuned by changing the angle of the sensor, se picture above!__

| Tire width 	| Distance from tire to sensor<br>with 60 degree narrow sensor 	| Distance from tire to sensor<br>with 120 degree wide sensor 	|
|-----------:	|----------------------------------------------------------:	|---------------------------------------------------------:	|
|     135 mm 	|                                                     12 cm 	|                                                     4 cm 	|
|     145 mm 	|                                                     13 cm 	|                                                     4 cm 	|
|     155 mm 	|                                                     13 cm 	|                                                     4 cm 	|
|     165 mm 	|                                                     14 cm 	|                                                     5 cm 	|
|     175 mm 	|                                                     15 cm 	|                                                     5 cm 	|
|     185 mm 	|                                                     16 cm 	|                                                     5 cm 	|
|     195 mm 	|                                                     17 cm 	|                                                     6 cm 	|
|     205 mm 	|                                                     18 cm 	|                                                     6 cm 	|
|     215 mm 	|                                                     19 cm 	|                                                     6 cm 	|
|     225 mm 	|                                                     19 cm 	|                                                     6 cm 	|
|     235 mm 	|                                                     20 cm 	|                                                     7 cm 	|
|     245 mm 	|                                                     21 cm 	|                                                     7 cm 	|
|     255 mm 	|                                                     22 cm 	|                                                     7 cm 	|
|     265 mm 	|                                                     23 cm 	|                                                     8 cm 	|
|     275 mm 	|                                                     24 cm 	|                                                     8 cm 	|
|     285 mm 	|                                                     25 cm 	|                                                     8 cm 	|
|     295 mm 	|                                                     26 cm 	|                                                     9 cm 	|
|     305 mm 	|                                                     26 cm 	|                                                     9 cm 	|
|     315 mm 	|                                                     27 cm 	|                                                     9 cm 	|
|     325 mm 	|                                                     28 cm 	|                                                     9 cm 	|
|     335 mm 	|                                                     29 cm 	|                                                    10 cm 	|
|     345 mm 	|                                                     30 cm 	|                                                    10 cm 	|
|     355 mm 	|                                                     31 cm 	|                                                    10 cm 	|
|     365 mm 	|                                                     32 cm 	|                                                    11 cm 	|
|     375 mm 	|                                                     32 cm 	|                                                    11 cm 	|
|     385 mm 	|                                                     33 cm 	|                                                    11 cm 	|


# Connecting the three boards

Four wires in a bus configuration connects the two sensors and the cpu board.  
One extra fifth wire connects to the distance sensor's XSHUT pin.

| Adafruit Bluefruit nRF52832 	| VL53L0X 	| MLX90621 	|
|-----------------------------	|---------	|----------	|
| 3.3V                        	| VIN     	| VIN      	|
| GND                         	| GND     	| GND      	|
| SCL                         	| SCL     	| SCL      	|
| SDA                         	| SDA     	| SDA      	|
| -                           	| GPIO1   	| -        	|
| MISO                         	| XSHUT   	| -        	|

![Display](images/connect-drawing.jpg)

The two sensorboards can easily be connected together electrically and mechanically by "sandwiching" them together with a pin header as in the picture below.

![Display](images/sensorsandwichmount.jpg)

This is the COMPLETE wiring needed. Currently looking at making a small pcb as an interconnect for all connections. Soldered wires are not optimal in this tough environment. The main board will probably be turned over to get its reset button aiming to the enclosure's front. 

![Display](images/connect-solder.jpg)

![Display](images/connectingSandwichRear.jpg)

# Compiling and uploading the code - Arduino IDE

Easy to do, just follow the <a href=/installArduino.md>complete step by step instruction here</a> to install the IDE and compile and upload the code. 

Here's info on the Adafruit Bluefruit nRF52 board if you're curious and want to dig deeper:  
https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide

# Testing Bluetooth BLE

Here are links to two Android Bluetooth BLE apps that can connect and show the live data that is transmitted. They show hex values only though so the sensor values are slightly obfuscated. But good for testing that everything is up and running.

https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp  
https://play.google.com/store/apps/details?id=com.punchthrough.lightblueexplorer

# Testing over USB

With the Arduino IDE (or other serial terminal software) you can view the printed output from the board over USB. Temperatures are shown as degrees in celsius times ten.

<img src="images/usbterminal.PNG">

# How often can I get measurements?

Depends. Measuring distance takes time, same with temperatures. And on top of that, with the current library used for Bluetooth communication, depending on how much data you're transmitting some delay is added too. So, __if you don't connect a distance sensor the temperatures are updated faster__. And if the application in the other end limits to subscribe to only eight temperature zones instead of all sixteen it will also all be faster. Compromises...

| Average speed 	| Data                       	|
|---------------	|----------------------------	|
| 6Hz           	| 16 temperatures + distance 	|
| 8Hz           	| 16 temperatures            	|
| 10Hz          	| 8 temperatures + distance  	|
| 16Hz          	| 8 temperatures             	|
| 16Hz          	| distance                   	|

# Bluetooth device name

The default Bluetooth name of each device is "RejsaRubber" __plus__ the last four bytes in the semi unique bluetooth MAC address. This automatic name will work in most situations, here's an example of how it can look:
```
"RejsaRubber6412051B" - for a device with MAC address CC:C9:64:12:05:1B
```
But you can set a specific code in the name for Front/Rear, Left/Right, like FL, RL and so on. You can set a device to one of six tire position names. Now only the three last bytes in the MAC address are tagged on at the end of the name instead of four. With this naming scheme each sensor can be more easily positioned properly in the logger app and still retain a unique address so not to clash with your friends nearby sensors.

- "RejsaRubber" + four adress bytes      - Default
- "RejsaRubberFL" + three adress bytes    
- "RejsaRubberFR" + three adress bytes 
- "RejsaRubberRL" + three adress bytes 
- "RejsaRubberRR" + three adress bytes 
- "RejsaRubberF" + one blank space + three adress bytes   - For motorbikes 
- "RejsaRubberR" + one blank space + three adress bytes   - For motorbikes

Examples for a device with MAC address CC:C9:64:12:05:1B:
```
"RejsaRubber6412051B" - non positional
"RejsaRubberFL12051B" - Front Left
"RejsaRubberFR12051B" - Front Right
"RejsaRubberRL12051B" - Rear Left
"RejsaRubberRR12051B" - Rear Right
"RejsaRubberF 12051B" - Front (motorbikes)
"RejsaRubberL 12051B" - Rear  (motorbikes)
```

# Set device name with jumpers

To set a specific device name you connect pins on the main board to ground. Preferably using jumper headers so you can move the jumpers easily to set the prefered name. Lets call the three jumpers CAR, FRONT and LEFT.

| CAR | FRONT | LEFT | Position |                    |
|:---:|:-----:|:----:|:--------:|:------------------:|
|  X  |   X   |   X  |    FL    |     Front Left     |
|  X  |   X   |      |    FR    |     Front Right    |
|  X  |       |   X  |    RL    |      Rear Left     |
|  X  |       |      |    RR    |     Rear Right     |
|     |   X   |   X  |     F    | Front (Motorcycle) |
|     |       |   X  |     R    |  Rear (Motorcycle) |
|     |       |      |    --    | Default auto names |

CAR is the connection marked "A4" on the main board  
FRONT is the connection marked "A5" on the main board  
LEFT is the connection marked "SCK" on the main board  
Connect them to the pin marked "GND".

# Mirror temperature zones on tire

If you happen to get the tire's outside shoulder temperature showing as the tire's inside temperature in your app you can reverse the out- and inside. By connecting one pin on the main board to ground you mirror the tire's temperature zones. The pin to connect to ground is marked "MISO" on the main board, connect it to the pin marked "GND".

# Work in progress...

The temperature part is rock stable and Bluetooth BLE seems to be running very nicely. But the distance sensor can at rare occasions hang, maybe a watchdog function needs to be added? The whole kit has NOT yet been properly field tested, just bench tests so far.

You can make your own enclosure of course but a small enclosure to 3D-print is being designed. This will include a design that enables changing the mounting angle to get the sensors aiming at the right spot on the tire, protection for the sensors and a snap-in holder so the whole enclosure can easily be removed to be recharged and later put back on the car. If printed in nylon/carbon fiber it will be very light and strong to endure the harsh environment in the wheel well. Files to print this enclousre will of course be freely available.

A small IR-transparent sensor protection window is on it's way to be sourced too.

# Questions and more info

Support forum: www.rejsa.nu/rejsarubbertrac

# Credits

The code for the IR temperature array sensor MLX90621 is 100% untouched from longjos https://github.com/longjos/MLX90621_Arduino_Camera which in turn is an adaption from robinvanemden https://github.com/robinvanemden/MLX90621_Arduino_Processing


