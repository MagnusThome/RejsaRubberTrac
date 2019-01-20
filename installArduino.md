# Install Arduino IDE to compile and upload the code

- Download the released version of Arduino IDE __and__ the beta build (not the hourly)
https://www.arduino.cc/en/Main/Software

- Install the released version

- Unzip the beta build on top of your newly installed folder, replacing the just installed files with the newer ones from the zip

- Start Arduino IDE, choose "File" > "Preferences" and copy this line below into the "Additional Boards Manager URLs" field

  https://www.adafruit.com/package_adafruit_index.json

<img hspace="50" src="images/installArduinoIDE-0.gif">

- Go to "Boards Manager"

<img hspace="50" src="images/installArduinoIDE-1.gif">

- Enter "nRF52" in the search box and then install "Adafruit nRF52 Boards". This will take a while to complete...

<img hspace="50" src="images/installArduinoIDE-2.gif">

- Choose "Adafruit Bluefruit nRF52832 Feather"

<img hspace="50" src="images/installArduinoIDE-3.gif">

- Go to "Manage Libraries"

<img hspace="50" src="images/installArduinoIDE-4.gif">

- Enter "vl53" in the search box and then install "Adafruit_VL53L0X"

<img hspace="50" src="images/installArduinoIDE-5.gif">

- Connect the Adafruit Bluefruit board to your computer's USB and choose the corresponding COM port

<img hspace="50" src="images/installArduinoIDE-6.gif">

- Place all the downloaded src files in a directory called "main"

- Open the main.ino file and choose "Upload"

<img hspace="50" src="images/installArduinoIDE-7.gif">

- When done you can open the "Serial Monitor" under the "Tools" menu and view the Arduino board's status and data transmitted

<img src="images/usbterminal.PNG">

- Have fun!
