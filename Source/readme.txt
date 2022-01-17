About
====

The original remame was modified, because used OpenGL 3.x-4.x core profile 

This demo is a technical demonstration of the lighting technique 
"Light Indexed Deferred Rendering".

A full explanation of the technique used in this demo can be found at:
https://github.com/dtrebilco/lightindexed-deferredrender/blob/master/LightIndexedDeferredLighting1.1.pdf
http://lightindexed-deferredrender.googlecode.com/files/LightIndexedDeferredLighting1.1.pdf

The scene consists of 255 lights with ~40 000 polygons. This demo allows you 
to switch between different "Light Indexed Deferred Rendering" techniques 
and standard multi-pass forward rendering.

To view most of the 255 lights in a single view, select "Set static scene view" 
from the F1 options menu.

There are several places in the demo scene where artifacts can be seen when the light 
index count is saturated. These have been left in to demonstrate what happens when 
the light count saturates. To easily see this effect, switch to "1 Light per fragment"
mode.

Also note that this lighting technique was designed for a scene with lots of objects, 
polygons and materials which this demo does not demonstrate. This demo is more of a 
stress test with 255 lights.


Author
======

This demo was made by Damian Trebilco ( dtrebilco@gmail.com ) 
based on a demo by Emil Persson, aka Humus.


Controls
========

 B - Toggles Depth bounbdd Test

Bugs 
====

Currently this demo has only tested with Geforce 1030/Geforce 1650, Intel UHD 630, AMD RX 560.
Latest drivers will be required for older cards.

MSAA is not handled correctly (see the paper for ways of handling MSAA)

Fullscreen mode may not work correctly in some multi-monitor configurations.

Legal stuff
===========

This demo is freeware and may be used by anyone for any purpose
and may be distributed freely to anyone using any distribution
media or distribution method as long as this file is included.

Compiling code
==============

The provided code was compiled with Visual Studio 2005 and should include all needed files.
Note that the FrameWork3 files have been changed slightly from the Humus framework code.


Special thanks
==============

Spacial thanks goes to 

  Emil Persson   - For providing the frame work/demos and ideas.
  
  Timothy Farrar - For suggesting the max-blend equation light index packing technique.
  
  Alistair Doolwind - For support and proofreading of the main document. 
  
  
=====================================================================================================================
Do not conform any longer to the pattern of this world, but be transformed by the renewing of your mind.
Then you will be able to test and approve what God's will is - his good, pleasing and perfect will. (Romans 12:2 NIV)
