2D Toolset Sample (Alpha)
=========================

![alt text](http://www.projectanarchy.com/sites/default/files/Project%20Anarchy%20Logo.png "Project Anarchy")

**IMPORTANT:** This is still very much in the alpha stage, so please be patient with us as we work out the kinks.

Although you can make 2D games with Project Anarchy out-of-the-box, it takes some custom code to get spritesheets working and doing basic
2D collision through LUA. This sample 2D toolset gives you a couple new entities and utilities to create 2D games entirely through LUA without
having to write any custom C++ code. There is still a lot of work left before this can be considered polished, but this is the first step
and we wanted to give you, the community, a first look at this so that you can provide us with feedback early on.

Features
--------

- Adds two new entities: **Sprite** and **2D Camera**
- Automated sprite generation using [Shoebox][1]
- Runtime playback of spritesheets
- Collision detection and LUA callbacks

Dependencies
------------

* [Visual Studio 2010][6] - **(Required)** Needed for compiling and you can use [Visual Studio 2010 Express][5]
* [Python 2.7][4] - **(Required)** There are numerous helper scripts that are executed automatically in Visual Studio when you build, so you need to install this and put it in your PATH.
* [PyTools][3] - (Optional) Useful if you want to debug Python scripts
* [Shoebox][1] - (Optional) Needed if you want to generate new spritesheets using the provided Python scripts

Samples
-------

* Shooter (`Assets\Scenes\Shooter.scene`)
* Impossible (`Assets\Scenes\Impossible.scene`)
* Physics (`Assets\Scenes\Physics.scene`) 

Compiling
---------

To build on Windows, simply open Workspace\2D_Toolset_Win32_VS2010_DX9.sln and build either Debug DLL or Dev DLL
configurations. It will automatically copy the binaries to the Anarchy SDK folder.

* `2D_Toolset_Win32_VS2010_DX9.sln` - Doesn't include PyTools projects, but requires a non-Express version of Visual Studio. This includes everything you need to build the 2D toolset.
* `2D_Toolset_Win32_VS2010_DX9_All.sln` - Includes PyTools projects and requires a non-Express version of Visual Studio
* `2D_Toolset_Win32_VS2010_DX9_C++.sln` / `2D_Toolset_Win32_VS2010_DX9_C#.sln` - If you have Visual Studio Express, you need to build these two separately

Generating Spritesheets
-----------------------

We have a Python script Source\BuildSystem\spritesheet.py that can be used to generate a sprite sheet. Just pass in either a folder or set of files and it
will call [Shoebox][1], generate the spritesheet along with needed XML files, and copy it to the Assets folder.

Credits
-------

Special thanks to [Howard Day][8] for the explosion effects in the shooter sample!

Remaining Work
--------------

###Beta

- Optimized rendering that uses actual mesh buffers
  - Already have a working version of this internally, but need to port it over
- Finalize Havok Physics integration and convert to a component
- Add support for Havok Physics on Android, iOS and Tizen
  - Need to implement serialization so that convex hull generation only happens on PC
- Add blending modes for Sprite entity
- Add a SetDirection LUA call for setting orientation of sprite
- Convert the Sprite management LUA code over to C++ so that you can do SpriteManager:CreateSimpleSprite
- Add support for Tizen and Android x86

###Wishlist

- Add custom shader support for Sprite entity
- Add a broadphase / sweep-and-prune implementation for collision detection
- Add support for using particle effects (as a child of the Sprite entity)
- Add pixel-perfect collision detection
- Add a transform rule that automatically creates the sprite sheet
- Add support for Box2D

[1]: http://renderhjs.net/shoebox/
[2]: http://www.polycount.com/forum/showthread.php?t=91554&highlight=shoebox
[3]: http://pytools.codeplex.com/
[4]: http://www.python.org/download/releases/2.7.6/
[5]: http://www.visualstudio.com/en-us/downloads#d-2010-express
[6]: http://www.visualstudio.com/
[7]: http://flukedude.com/theimpossiblegame/
[8]: http://www.hedfiles.net/