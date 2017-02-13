# ESPloit

Written by Corey Harding from http://www.LegacySecurityGroup.com<br><br>
<b><i>WiFi controlled HID Keyboard Emulator</i></b><br>
<br>
Demo at: https://legacysecuritygroup.com/index.php/categories/11-hardware/27-esploit <br>
<br>
ESPloit uses an ESP8266 to host a web server acting as a command and control server for the "payloads"(keypresses) that get sent to the Arduino which acts as a keyboard emulator. The end result is not as good as a Hak5 USB Rubber Ducky and the language for writing the payloads is not as good as DuckyScript either. However this is a really simple project to replicate for under $20 plus it has WiFi, its kind of like an IoT keystroke injection attack. It is still a work in progress and its main purpose is to be a proof of concept.
<br>
<br>
<b>Hardware Required:</b><br>
Adafruit Huzzah ESP8266 Board (Web Interface/Command and Control Server)<br>
Arduino Beetle by CJMCU - ATmega32u4 Chip (HID Keyboard Emulator)<br>
Optional: USB Hub or Enclosure (To Hide Device Inside Of)<br>
<br><br>
<b>Software used:</b><br>
Arduino IDE: Arduino IDE 1.6.11<br>
Board Manager/Libraries: esp8266 by ESP8266 Community version 2.3.0<br>
 
<br>
 
<b>Setup Network:</b><br>
By default ESPloit creates an access point with a hidden SSID of "Exploit" and a WPA2 PASSWORD of "DotAgency".<br>
You can change this in the source of esp-sketch and even set it to join an existing network.<br>
The comments in the code should be able to walk you through things.<br>
Also this is the time to choose an IP Address for the server while your in the source.(default is 192.168.1.1)<br>
<br>
<br>
<b>Flash Boards:</b><br>
arduino-sketch goes to the 32u4 based board<br>
esp-sketch goes to the esp8266 device<br>
<i>NOTE: On first boot of the ESP8266 it may take up to around 90 seconds while the SPIFFS file system is formatted for the first time.  Also the ESP8266 is a power hungry beast my FTDI cable only put out enough current to program it(Adafruit Huzzah Breakout Board HACKADAY Edition) but not enough to allow it to fully boot.  So use a a separate power supply to boot it.  I am using a YWROBOT MB102 at 3v3 on a breadboard for testing.<br>
<br>
<br>
<b>Wire it Up:</b><br>
This assumes you are using an Adafruit Huzzah and a CJMCU Beetle Clone with the default software serial pins set in the sketches.<br>
<b><i>Arduino --- ESP8266 Huzzah</i></b><br>
--------------------------<br>
<b><i>5v  ---  V+ Pin(Huzzah Breakout) or USB Pin(Feather)</i></b> //If your ESP8266 does not have a voltage regulator to convert 5v-3v3 then do not do this instead use a voltage regulator<br>
<b><i>GND  ---  GND</i></b><br>
<b><i>D10(Soft-RX)  ---  Pin 4(Soft-TX)</i></b> //This is ok to do because we are transmitting from the 3v3 logic of the ESP8266 to the soft-rx of the Arduino, do NOT connect the 5v logic soft-tx of the Arduino to the 3v3 logic soft-rx of the ESP8266.  If it were real serial pins and not software serial and the Adafruit Huzzah family then this would be ok, but only with the physical tx/rx serial pins since the Huzzah has a level shifter on the serial rx pin.<br>
Now you can solder the pins on the usb plug of the CJMCU Beetle Arduino Clone to the corresponding USB wires inside a USB Hub or heat shrink the project and use it as is or hide it in clever usb enclosure.<br>
<br>
<br>
<b>Writing a Payload:</b><br>
Please refer to example_payloads folder and decimal_key_reference folder for reference.<br>
<br>
For individual keypresses or combinations of key presses<br>
"Press:X" or "Press:X+Y" or "Press:X+Y+Z" and so forth<br>
--Expects DECIMAL Values for X,Y,Z,etc<br>
--"Press:131+114" would output KEY_LEFT_GUI(Windows_Key)+r thus launching the run prompt if connected to a Windows machine<br>
--List of modifier keys(GUI,ALT,CTRL,ETC) at https://www.arduino.cc/en/Reference/KeyboardModifiers<br>
--ASCII table lookup at http://www.asciitable.com/<br>
<br>
To type out strings of text<br>
"Print:XYZ" Types out "XYZ"<br>
--"Print:www.Exploit.Agency" would type out "www.Exploit.Agency" on the machine connected via USB<br>
<br>
To type out strings of text followed by the ENTER Key<br>
"PrintLine:XYZ" Types out "XYZ" then presses enter<br>
--"PrintLine:www.Exploit.Agency" would type out "www.Exploit.Agency" on the machine connected via USB and then press ENTER<br>
<br>
To make a delay<br>
"Delay"<br>
--"Delay" would wait for X seconds(length of delay set in esp8266 sketch default=2000ms) before proceeding to next line in the payload<br>
<br>
<br>
<b>Uploading Payload:</b><br>
In web browser type 192.168.1.1(or IP you set)<br>
Click "Upload Payload"<br>
Select the File for the payload you wrote<br>
Click Upload<br>
<i>NOTE: There is a 21 character limit to the file name including the DOT and EXTENSION.  Example of a good filename: "video-demo-winpro.txt" and a bad file name: "video-demo-windows-pro.txt".  If your file does not show up in the payload list after uploading it then this will likely be the reason why.</i><br>
<br>
<br>
<b>Running your Payload:</b><br>
In web browser type 192.168.1.1(or IP you set)<br>
Click "Choose Payload"<br>
Click on the payload you want to run<br>
It will display the contents of the payload<br>
Press the "Run Payload" button to start running the payload<br>
