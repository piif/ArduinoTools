ArduinoTools
============

some tools for arduino programming :

 * banner : to handle characters "fonts" and draw them like banner unix command
 * led7 : handle a specific 7 segment display
 * pwm : some helper functions to handle PWM. Should work with Arduino Uno, Mega, Nano and ATtiny85 µcontroler
 * serialInput : handle simple input from serial
 * setInterval : implements a method similar to setInterval js function
 * strop : C++ like "<<" operator to send strings and numbers to streams

This lib was initially configured to compile with an eclipse custom toolschain, then adapted to use with an arduino-cli based Makefile.
It certainly doesn't work into standard arduino Ide.