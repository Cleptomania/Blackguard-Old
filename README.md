# Blackguard
A small rogue-like dungeon crawler in C++.

Using Libtcod and Boost serialization library. Will add more info later.

## Setting up in Visual Studio 2015.
Download version 1.5.1 for MSVC of libtcod from [this](https://bitbucket.org/libtcod/libtcod/downloads) link, and extracts the contents to any location on your computer(we'll use C:/libtcod in this example).

In Visual Studio, right click on the Blackguard project, and select properties. From there go to Configuration Properties -> VC++ Directories and add the following directories from the libtcod directory.

**Include Directories**: add the "include" folder.  
**Library Directories**: add the "lib" folder.  
**Source Directories**: add the "src" folder.  

Now navigate to Configuration Properties -> Linker -> Input and add "libtcod-VS.lib" under "Additional Dependencies".

Now navigate to Configuration Properties -> Linker -> General and add the "lib" and root folder of libtcod to **Additional Library Directories**

Under Configuration Properties -> Debugging, change the working directory from `$(ProjectDir)` to `$(ProjectDir)Debug`.

Then in your project folder, move the files libtcod-VS.dll, SDL.dll, and arial12x12.png to the Debug folder in your project's root directory(If folder is not there, create it).




