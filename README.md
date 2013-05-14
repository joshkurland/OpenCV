OpenCV
======

OpenCV Color Identification and Tracking
Drive.cpp

May 2013

This program was written by me as part of my senior design project.  

It performs 2 roles:

  HSV Color Identification
  Image Tracking
  
  
The HSV color identification will first need to be calibrated.  While running the program, press the button 'c' on 
the keyboard to begin calibration.  Fill the calibration box with the image.  (Note only solid colored objects may be 
idendified at this time.)  Once the color has been calibrated the HSV threshold range will sort the image frame into 
a binary set.  Any object falling within the color range will be highlighted, the rest will be ignored.

Color tracking uses the weighted moment of the current objects in the frame.  Based on the separation of the pixel 
density, the algorithm will determine the posistion of the object.  This will only work for a single object.  If two 
or more colors are in the frame, the weighted average will fall in the middle, and the tracking dot will not function.

Keyboard Controls
-----------------
c - Calibrate
t - Track
q - Pause
r - Reset
esc - Quit
