Project Anarchy: 2D Toolset (Alpha)
===================================

![alt text](http://www.projectanarchy.com/sites/default/files/Project%20Anarchy%20Logo.png "Project Anarchy")


Features
--------

- Adds two new entities: Sprite and 2D Camera
- Automated sprite generation using [Shoebox][1]
- Runtime playback of spritesheets

Dependencies
------------

* Visual Studio 2010 
* [PyTools][3]
* [Shoebox][1] 

Generating Spritesheets
-----------------------

TODO
----

- Finish iOS and Android port
- Optimzied rendering that uses actual mesh buffers
- Use ParticleTrimmer to generate a convex hull for sprites
- Add blending modes for Sprite entity
- Add custom shader support for Sprite entity
- Add a SetDirection LUA call for setting orientation of sprite
- Convert the Sprite management LUA code over to C++ so that you can do SpriteManager:AddSprite
- Add a Clone LUA call to Sprite entity
- Add 2D physics component using Havok Physics
- Add support for using particle effects (as a child of the Sprite entity)
- Add a 2D manipulator in the editor
- Add a transform rule that automatically creates the sprite sheet
- Add a broadphase / sweep-and-prune implementation for collision detection
- Add pixel-perfect collision detection
- Add support for Box2D

Reference
---------

[1]: http://renderhjs.net/shoebox/
[2]: http://www.polycount.com/forum/showthread.php?t=91554&highlight=shoebox
[3]: http://pytools.codeplex.com/