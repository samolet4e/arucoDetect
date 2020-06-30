# arucoDetect
A code for perspective transformation of region of interest

This is a short program for perspective transformation of a quadrialteral defined by fiducial markers. All you need is a video stream in which four Aruco markers are clearly visible. The markers define a region of interest. Whenever a key is pressed, the region of interest is stretched so as to fill up the screen area. In this way, spontaneous camera shaking is eliminated. The process is similar to image crop.

Prerequisites:
OpenCV library with installed contributory modules (Aruco module in particular)
Either GNU compiler collection or MS visual Studio installed

The result could be watched here:

https://youtu.be/7XbZWsa5_4g
