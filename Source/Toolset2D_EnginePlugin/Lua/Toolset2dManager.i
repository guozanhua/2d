%nodefaultctor Sprite;
%nodefaultdtor Sprite;

// custom headers for generated source file
%module Toolset2dModule
%{
	#include "Toolset2dManager.hpp"
%}

class Toolset2dManager
{
public:
	int GetNumSprites();
	Sprite *CreateSprite(const hkvVec3 &position, const VString &spriteSheetFilename, const VString &xmlDataFilename);

	void SetCamera(Camera2D *camera);
	Camera2D *GetCamera();

	%extend
	{
		VSWIG_CREATE_CAST_UNSAFE(Toolset2dManager)
	}
};
