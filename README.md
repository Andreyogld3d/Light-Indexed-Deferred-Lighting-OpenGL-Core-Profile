# Light Indexed Deferred Rendering using OpenGL Core Profile

This demo is based on original demo [Light Indexed Deferred Rendering](https://github.com/dtrebilco/lightindexed-deferredrender) 
The original demo was modified:

1) Replaced Old OpenGL 2.x/FFP to OpenGL 3.x-4.x Profile
2) Removed UI interface
3) Use 4 light sourced per pixels
4) Use only Depth Bound Test

# Purpose

1) Port from original OpenGL to Direct3D12/Vulkan, see issue https://github.com/Andreyogld3d/LightIndexedDeferredRendering_Direct3D12/issues/1
 We can see the same bug:
 ![Screenshot](https://github.com/Andreyogld3d/Light-Indexed-Deferred-Lighting-OpenGL-Core-Profile/blob/main/Source/DeferredLighting/lidr_gl.png)
 It can be reproduced using the followinf modification:
 1. Turn on macros https://github.com/Andreyogld3d/Light-Indexed-Deferred-Lighting-OpenGL-Core-Profile/blob/main/Source/DeferredLighting/App.cpp#L28
 2. Turn on macros https://github.com/Andreyogld3d/Light-Indexed-Deferred-Lighting-OpenGL-Core-Profile/blob/main/Source/DeferredLighting/App_Util.cpp#L29
3) Replace bit operations to integer textures
4) Upgrade to modern Deferred techique like as Tiled/Clustered Forward+ Shading 

# Requirements:

1. Visual Studio 2005 and above
2. GPU With OpenGL 3.3 Core profiler support: ATI HD 2xx/3xx/4xx/5xx/6xx/7xx/8xx, AMD R5/R7/R9, RX 4xx/5xx, Rx Vega, Radeon VII, Rx 5xxx/6xxx
GeForce 8400-9080, GeForce 200/400/500/600/700/800/900/10xx/16xx/20xx/300xx Series, Intel HD 2xxx/3xxx/4xxx/5xxx, Intel UHD 6xx

# Screen shot from the demo

![Screenshot](https://github.com/Andreyogld3d/Light-Indexed-Deferred-Lighting-OpenGL-Core-Profile/blob/main/screenshot.png)

                                             
