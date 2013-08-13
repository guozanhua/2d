#ifndef SpriteENTITY_HPP_INCLUDED
#define SpriteENTITY_HPP_INCLUDED

#include "SpriteShapeEnginePluginPCH.h"

// this include defines the SAMPLEPLUGIN_IMPEXP and declares the g_sampleModule
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/SamplePluginModule.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>

///////////////////////////////////////////////////////////////////////////////////////
// class Sprite : Entity class for health packs
///////////////////////////////////////////////////////////////////////////////////////
class Sprite : public VisBaseEntity_cl
{
public:

	// overridden entity functions
	SPRITE_IMPEXP VOVERRIDE void InitFunction();
	SPRITE_IMPEXP VOVERRIDE void DeInitFunction();
	SPRITE_IMPEXP VOVERRIDE void ThinkFunction();
  
	SPRITE_IMPEXP void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

	// call this to pick up the item by player entity
	SPRITE_IMPEXP void Pickup();

protected:
	void CommonInit();

	VTextureObjectPtr m_spSpriteSheetTexture;
  
	hkvVec3 m_vCenterPos;
	float m_fTimePos;

	VisMeshBuffer_cl *m_spriteMeshBuffer;
	VisStaticMeshPtr m_staticMesh;
	VisStaticMeshInstance_cl *m_staticMeshInstance;

public:
	// serialization and type management
	V_DECLARE_SERIAL_DLLEXP( Sprite, SPRITE_IMPEXP )

	SPRITE_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
	SPRITE_IMPEXP VOVERRIDE void OnSerialized( VArchive &ar );

	// entity class registration
	IMPLEMENT_OBJ_CLASS(Sprite);

};

#endif
