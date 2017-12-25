# hanoukia
Code for ESP12E based Hanoukia as described in Instructables
This project contains three parts:
1. this is the wood and plastic part, the real object
2. the circuit built around an ESP8266-12E wifi module, and here you'll find the program built with Arduino IDE (.ino extension)
3. the Android application built with MIT App inventor, and the code is available here (aia extension)
The Android app gets some data from the internet to know what is the actual sunset time for the first day of Hanouka, and derives from it when it is time (or not) to light the hanoukia, depending on the day in the week. Special features are built for Shabbat to make sure we know when it is too late to light the hanoukia on.
Depending on the Wifi network the hanoukia is connected to, it will allow to send a message to the hanoukia built as a web server. This message is mostly the number of the day, is it first, second, etc. until the 8th day.
When the Web server (Hanoukia in ESP) receives the message it lights all the corresponding lights on in the right order, and ends with the shamash.
