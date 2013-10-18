%nodefaultctor Sprite;
%nodefaultdtor Sprite;

// custom headers for generated source file
%module Toolset2D
%{
  #include "SpriteManager.hpp"
%}

class SpriteManager
{
	int GetNumSprites();
};
