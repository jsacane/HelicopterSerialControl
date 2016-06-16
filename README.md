Helicopter Serial Control
=======================

Controls a Syma S107G model R/C helicopter through infrared LEDs, an Arduino, and Serial Monitor.
Code is based off of individual research done by Kerry D. Wong and Andrew Barry.
For this to work, simply wire an infrared LED to pin 3 on an Arduino board (Confirmed to work with Arduino Uno, never tested any others).
The TimerOne library is also needed.
For better distance control, an infrared LED array with increased amperage should work.

![alt text](http://i.imgur.com/ByYXFdc.jpg "Syma S107G Red")

Schematic:
-----------------------

![alt text](http://i.imgur.com/0wK7aYg.png "Syma S107G Red")

Note: If using the LED array for long range control, the value of resistor will change depending on source voltage and type of LED array.

Commands:
-----------------------

   t = Take off
   
   h = Hover
   
   l = Land
   
   w/s = Pitch forward/backward
   
   a/d = Pitch left/right
   
   q/e = Trim left/right
   
   u = Increase throttle
   
   j = Decrease throttle
   
   0 = Kill throttle
   
   1-9 = Throttle "settings" (Scales throttle by magnitude of number)
   
   r = Reset yaw, pitch, and trim
   
   
   Note: Multiple commands can be sent at the same time, but they will be read individually left to right. For example
         sending wwwww sends the w command 5 times, increasing forward pitch by 15.
