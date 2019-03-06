# Interconnect carrier board to mount everything on

This is not at all needed to build the sensor kit. The minimum way of doing it is just five wires between the boards and nothing else, as you can see on the main page. 

But this board is handy if you want to run the kit from the car's 12V. Or if you want to use jumpers to set each sensor's wheel position. Or to add a power switch. Or if you buy the temperature sensor without it's daughter board, here you have a main board to mount it all on.

A list of all components needed with links to mouser can be found in an Excel sheet here:
https://github.com/MagnusThome/RejsaRubberTrac/edit/master/pcb

# More info coming...

But for now:

# Running from 12 volts

Here's how to mount the components neccesary for running everything from 12V. Check the excel file component list above on what components to get!

<img src="images/12V mounting.jpg">

# Sensors on daughter boards

Mount the two sensor boards togther with a six pin header.  
<img src="images/daughterboards/00.jpg">

Put insulation on the bottom so the components there can't touch the main board.
<img src="images/daughterboards/01.jpg">

It's a little fiddly to get it all soldered on perfectly straight. I suggest you solder only one pin first. Then try to get it straight. When perfect solder the rest of the pins.
<img src="images/daughterboards/02.jpg">

Yeah, straight.
<img src="images/daughterboards/03.jpg">

# Power switch 

Note: if you don't mount a switch everything will by default be switched on. No need for any jumpers to turn the board on.
<img src="images/daughterboards/04.jpg">

# Jumper pin headers

Mount the 4x2 pin headers for the jumpers which set wheel position (left front, right rear...) and mirror outside/inside edge of the tire.
<img src="images/daughterboards/05.jpg">

The jumpers are marked so you know which one sets what:
<img src="images/jumpersettings.jpg">

# Pin headers to cpu/bluetooth board

Mount the two long pin headers connecting to the cpu/bluetooth board. Note: you don't have to solder all pins if you don't want, there are markings on the board showing what pins you have to solder.

Solder on the cpu/bluetooth board. Again, not all pins needs to be soldered but if you're unsure, solder all.
<img src="images/daughterboards/06.jpg">

# Mounting a temperature sensor that comes __without__ a daughter board

As you can see in the component list you can order the temperature sensor separately as a component without the daughter board or buy it mounted on a daughter board. It is usually sold on a daughter board from China. If you get the version without a daughter board there are two extra components you need to solder on to the board. They are of course listed in the component list excel sheet.

In addition to this you might need to mount the temperature sensor 4mm above the board if you want to use any of the 3D printed cases for this board. This is to make sure the 60 degree of field of view isn't hindered by the limited size window on the case. Also make sure you turn the sensor so the right pins are inserted in the correct respective mounting holes. There is a marking on the board showing a circle with a protrusion that should match a small tab sticking out on the sensor.

<img src="images/separate%20temp%20sensor.jpg">

# Here's info on how to order the pcb

You can download the gerber files here and get boards done anywhere you want. Or you can order them here too, this is where I got my made. I think I might get a small kickback if you order here but again, get them done wherever you want :-) !

https://www.pcbway.com/project/shareproject/RejsaRubberTrac_____v1_1.html

# Again, more info coming but this is a start
