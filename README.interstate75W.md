Interstate 75W is an RP2350 version of Adafruit's S3 Matrixportal (which uses an ESP32 S3 chip).

People are excited about the RP2350 because it's around 0.50c to $1 cheaper than ESP32-S3, but I found it a much worse chip to work with:
- lack of support and things that "just work" on ESP32 and have worked for 5 to 10 years
- lack of NVRAM ( #include <Preferences.h> )
- lack of crash dump on serial port that can be trivially deocded to find what crashed
- fragile serial port support (emulated in software and can disappear at runtime)
- Well established ESP32 code like OmEspHelpers does not work since it's another chip
- OTA is supposed to be supported, but does not work the same


flashing:
- default/UF2 is fragile, it relies on arduino connecting via the fragile serial port connection, telling the chip to reboot as usb-storage and uploading a binary. The arduino env used to be able to send the serial command to switch the chip to usb-storage, but as I write this it is repeatedly failing with:
```
Resetting /dev/ttyACM0
Converting to uf2, output size: 546304, start address: 0x2000
Scanning for RP2040 devices
No drive to deploy.
An error occurred while uploading the sketch
```
Even if ttyACM0 happens to have working serial at the time (not a given as I wrote above).

workarounds:  
1) force the switch to usb-storage:
```
- hold boot button
- push reset button
- release hold button
```
and you get:
```
[74736.918907] sd 0:0:0:0: Attached scsi generic sg0 type 0
[74736.961850]  sda: sda1
[74736.961948] sd 0:0:0:0: [sda] Attached SCSI removable disk
```
Then flashing will work

2) use my 'B' command in TME69 that calls 'rp2040.rebootToBootloader();'.  

But yeah, this is sad, this "just worked" on ESP32-S3 and I've wasted multiple hours with it not working on this RP2350 chip.

How about flashing with picotool?

picotool is supposedly a more robust flasher than the UF2 method, but it won't work any better than UF2 if the chip isn't being rebooted in usb-storage mode.  It won't work out of the box unless you are root, but you can do this:
```
sauron:~$ sudo chown root:root ~/.arduino15/packages/arduino/tools/rp2040tools/1.0.6/picotool
sauron:~$ sudo chmod 4755 ~/.arduino15/packages/arduino/tools/rp2040tools/1.0.6/picotool
```

Picotool will then flash:
```
Sketch uses 250816 bytes (12%) of program storage space. Maximum is 2088960 bytes.
Global variables use 41008 bytes (7%) of dynamic memory, leaving 483280 bytes for local variables. Maximum is 524288 bytes.
/home/merlin/.arduino15/packages/rp2040/tools/pqt-picotool/5.0.0-9576866/picotool load /tmp/arduino_build_830579/Table_Mark_Estes69.ino.uf2 -f -x 
Family ID 'rp2350-arm-s' can be downloaded in absolute space:
  00000000->02000000
```

Annoying, it reboots with errors and failure on serial port, but serial port seems to work in the end:
```
The device was rebooted to start the application.
processing.app.SerialException: Error opening serial port '/dev/ttyACM0'. Try consulting the documentation at http://playground.arduino.cc/Linux/All#Permission
	at processing.app.Serial.<init>(Serial.java:150)
	at processing.app.Serial.<init>(Serial.java:82)
	at processing.app.SerialMonitor$2.<init>(SerialMonitor.java:132)
	at processing.app.SerialMonitor.open(SerialMonitor.java:132)
	at processing.app.AbstractMonitor.resume(AbstractMonitor.java:132)
	at processing.app.Editor.resumeOrCloseSerialMonitor(Editor.java:2126)
	at processing.app.Editor.access$1300(Editor.java:116)
	at processing.app.Editor$UploadHandler.run(Editor.java:2095)
	at java.lang.Thread.run(Thread.java:748)
Error opening serial port '/dev/ttyACM0'. Try consulting the documentation at http://playground.arduino.cc/Linux/All#Permission
```

I'll note that here is a pretty big issue with rebooting the device and catching early boot messages as the serial port is not visible on linux for a while. You can add huge delays or while (! Serial), but if you do that you're also stopping the device from booting if there is no USB cable connected.

Long story short, it's not pretty, and it works *so* much better on ESP32.  So no, these were hours lost for getting inferior and more fragile functionality.

