#ifndef SpriteENTITY_HPP_INCLUDED
#define SpriteENTITY_HPP_INCLUDED

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
	VOVERRIDE void InitFunction();
	VOVERRIDE void DeInitFunction();
	VOVERRIDE void ThinkFunction();
  
	void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

	// call this to pick up the item by player entity
	void Pickup();

protected:
	void CommonInit();

	VTextureObjectPtr m_spSpriteSheetTexture;
  
	hkvVec3 m_vCenterPos;
	float m_fTimePos;

	VisMeshBuffer_cl *m_spriteMeshBuffer;

public:
	// serialization and type management
	V_DECLARE_SERIAL_DLLEXP( Sprite, SAMPLEPLUGIN_IMPEXP )
	SAMPLEPLUGIN_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
	SAMPLEPLUGIN_IMPEXP VOVERRIDE void OnSerialized( VArchive &ar );

	// entity class registration
	IMPLEMENT_OBJ_CLASS(Sprite);

};

#endif
