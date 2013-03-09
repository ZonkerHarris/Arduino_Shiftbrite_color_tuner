Arduino_Shiftbrite_color_tuner
==============================

A project to compare collor values in an array with a fully-variable color trio, to improve the color array

I created a color array for my Sheri_sign project. 
In this array, there were groups of 15 values, representing the R-G-B values (0-1023) for 5 colors.
The concept was to have 5 shades of red, from bright to dark, but I guessed at the values. I was wrong.

After I had delivered the sign, I built this project for two reasons; 
  I wanted to improve the color array, and that required a box with three potentiometers to tune colors.
  I would need to view the R-G-B values for the tuned color, and for colors in the array...
  I could use the psuedo-serial port, but I opted to learn how to use Serial.print to an LCD display.
  
The box was a success, and the color array has been improved!

Pictures in my dropbox at  https://www.dropbox.com/sh/nckn6lbzgh5olul/jiHCv4wJYn
