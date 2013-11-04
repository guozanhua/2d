Project Anarchy: 2D Toolset (Alpha)
===================================

![alt text](http://www.projectanarchy.com/sites/default/files/Project%20Anarchy%20Logo.png "Project Anarchy")


Features
--------

- Support for [Shoebox][1]. For more information about it check out the [Polycount thread][2]

TODO
----

- Add blending modes for Sprite entity
- Add custom shader support for Sprite entity
- Optimzied rendering that uses actual meshes
- Add a SetDirection LUA call for setting orientation of sprite
- Convert the Sprite management LUA code over to C++ so that you can do SpriteManager:AddSprite
- Add a Clone LUA call to Sprite entity
- Add 2D physics component using Havok Physics
- Add support for using particle effects (as a child of the Sprite entity)
- Add support for hkvVec2 (see Source\Vision\Runtime\EnginePlugins\VisionEnginePlugin\Scripting\Lua\hkvVec3.i)
- Add a 2D manipulator in the editor
- Add a transform rule that automatically creates the sprite sheet
- Add a broadphase / sweep-and-prune implementation for collision detection
- Add pixel-perfect collision detection
- Add support for Box2D

Reference
---------

[1]: http://renderhjs.net/shoebox/
[2]: http://www.polycount.com/forum/showthread.php?t=91554&highlight=shoebox