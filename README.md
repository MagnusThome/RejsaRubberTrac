# RejsaRubberTrac

A CPU board with a bluetooth BLE transmitter + IR multi point temperature sensor + laser based distance sensor.

*__Intended to track/view/logg tire temperatures on race- or trackday cars.__*

Temperatures are measured at sixteen different points over each tire's full width.

A distance sensor is also included for tracking/logging suspension movements.

All data is available over Bluetooth Low Energy BLE.

Current code is for Adafruit's Bluefruit nRF52832 board. 

# Parts
<img align="right" width="231" src="images/partsizes.jpg">

- Temperature IR-arraysensor MLX90621 GY-906LLC-BAB (60 degrees field of view. Ends with BAA for 120 degrees FOV)  
Example of where to find it: https://eckstein-shop.de/GY-906LLC-BAB-IR-Array-Temperature-Sensor-Module 

 - Laser distance sensor VLX53L0X  
Example of where to find it: https://www.ebay.co.uk/sch/i.html?_nkw=VL53L0X&_sop=15  

 - CPU and Bluetooth board Adafruit BlueFruit nRF52  
Example of where to find it: https://www.electrokit.com/produkt/adafruit-feather-nrf52-bluefruit-le-nrf52832/ 

 - Optional: Rechargable 3,7V Lipo battery 250mAh or larger with JST-PH 2.0mm connector  
Example where to find it: https://www.ebay.co.uk/sch/i.html?_nkw=3.7V+lipo+JST-PH+2.0&_sop=15 

# Power supply

The CPU board and the two sensor boards are all powered by connecting power to the CPU board's USB micro connector. The CPU board also has a connector for a Lipo battery - which is automatically charged via the USB connector - so the whole system can run completey wireless with it's own power source for roughly 24 hours with a 500mAh 3,7V Lipo battery. The system can also be run directly from the car's power, not using any Lipo battery, but then a 12 volt to USB 5 volt converter must be added.

# Connecting the three boards

Four wires in a bus configuration connects the two sensors and the cpu board.

![Display](images/connecting.gif)
![Display](images/connect-drawing.jpg)

# Arduino

Basic info to add the Adafruit Bluefruit nRF52 board to the Arduino IDE: https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/arduino-bsp-setup

# Work in progress...
<img align="right" width="231" src="images/harrys_early_preview.jpg">

The temperature part is rock stable and Bluetooth BLE seems to be running very nicely. But the distance sensor drops out from the I2C bus sometimes. Some code to get it more stable and/or soft reset it so it restarts properly is missing.

A small enclosure to 3D-print must be designed. This will include a design that protects the sensors and a snap-in holder so the whole enclosure can easily be removed and put back on the car. If printed in nylon/carbon fiber it will be very light and strong to endure the harsh environment in the wheel well.

A small IR-transparent sensor protection window is on it's way to be source too.

The two major track loggers for mobile phones www.gps-laptimer.de and www.racechrono.com already have units and have both done initial tests for integrating support for it. Picture to the right from an early preview of Harry's Laptimer.

# Questions and more info

The main discussion thread (in Swedish but feel absolutely free to write in english!) https://rejsa.nu/forum/viewtopic.php?t=113976


# Credits

The code for the IR temperature array sensor MLX90621 is 100% untouched from longjos https://github.com/longjos/MLX90621_Arduino_Camera which in turn is an adaption from robinvanemden https://github.com/robinvanemden/MLX90621_Arduino_Processing


