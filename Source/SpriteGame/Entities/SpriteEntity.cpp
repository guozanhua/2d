// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM		: ALL
// PRODUCT		: PHYSICS_2012+VISION
// VISIBILITY		: PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

#include <Vision/Samples/Engine/SpriteGame/SpriteGamePluginPCH.h>

#include <Vision/Samples/Engine/SpriteGame/Entities/SpriteEntity.hpp>
#include <Vision/Samples/Engine/SpriteGame/SpriteGameApplication.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

V_IMPLEMENT_SERIAL(Sprite, VisBaseEntity_cl, 0, &g_sampleModule);

inline void InitVertex(int index, VisMBSimpleVertex_t *pVert, float x, float y, float z)
{
  VColorRef iColor((index==2)?0:255,(index==1)?0:255,(index==0)?0:255,(index%3)*85);
  pVert->color = iColor.GetNative();

  pVert->texcoord0[0] = x * 0.05f;
  pVert->texcoord0[1] = y * 0.05f;

  pVert->pos[0] = x;
  pVert->pos[1] = y;
  pVert->pos[2] = z;
}

// called by the engine when entity is created. Not when it is de-serialized!
void Sprite::InitFunction()
{
	// once at creation time get the original position
	m_vCenterPos = GetPosition();
	CommonInit();
}

// called by the engine when entity is destroyed
void Sprite::DeInitFunction()
{
	MyGameManager::GlobalManager().DecRemainingItems();
}

// called by our InitFunction and our de-serialization code
void Sprite::CommonInit()
{ 
	MyGameManager::GlobalManager().IncRemainingItems();

	// does not need to be serialized
	m_fTimePos = Vision::Game.GetFloatRand()*hkvMath::pi () * 2.0f;

	if (Vision::Editor.IsPlaying())
	{
		VisMeshBuffer_cl *pBuffer = new VisMeshBuffer_cl();

		// alloc
		pBuffer->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
		pBuffer->AllocateVertices(VisMBSimpleVertex_t::VertexDescriptor, 3);
		pBuffer->AllocateIndexList(6);

		// texture
		pBuffer->SetBaseTexture(m_spSpriteSheetTexture);

		VisMBSimpleVertex_t *pVert = (VisMBSimpleVertex_t *)pBuffer->LockVertices(0);

		float rad = 1.f;
		float zval = 0.9f;  

		InitVertex(0, pVert,-rad,0,zval); pVert++;
		InitVertex(1, pVert, rad,0,zval); pVert++;
		InitVertex(2, pVert,0,rad, zval); pVert++;

		pBuffer->UnLockVertices();

		unsigned short *pIndex = (unsigned short *)pBuffer->LockIndices(0);

		pIndex[0] = 0;
		pIndex[1] = 1;
		pIndex[2] = 2;
		pIndex[3] = 2;
		pIndex[4] = 1;
		pIndex[5] = 0;

		pBuffer->UnLockIndices();
	}
}

// called by the engine once a tick
void Sprite::ThinkFunction()
{
  float dtime = Vision::GetTimer()->GetTimeDifference();

  // rotate health pack
  IncOrientation(dtime*40.f,0.f,0.f);
  m_fTimePos = hkvMath::mod (m_fTimePos+dtime*3.f,hkvMath::pi () * 2.0f);

  hkvVec3 vPos = m_vCenterPos;
  vPos.z += 8.f*hkvMath::sinRad (m_fTimePos);
  SetPosition(vPos);
}

void Sprite::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
	if (!strcmp(pVar->name,"TextureFilename"))
	{
		if (value && value[0])
			m_spSpriteSheetTexture = Vision::TextureManager.Load2DTexture(value);
		else
			m_spSpriteSheetTexture = NULL;
	}
}

// Serialization function : Used when exported by vForge and loaded by scene viewer
void Sprite::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  if (ar.IsLoading())
  {
    m_vCenterPos.SerializeAsVisVector (ar);
  } else
  {
    m_vCenterPos.SerializeAsVisVector (ar);
  }
}

void Sprite::OnSerialized(VArchive &ar)
{
  VisBaseEntity_cl::OnSerialized(ar);

  // call this after VisBaseEntity_cl::OnSerialized(ar) because in that function components are attached
  CommonInit();
}

// called when this item is picked by player
void Sprite::Pickup()
{
  Remove();
}

// vartable; provide the following members to vForge. Although vPhysXEntity is the base class we use VisBaseEntity_cl here to hide physX properties
START_VAR_TABLE(Sprite, VisBaseEntity_cl, "Sprite", 0, "")
	DEFINE_VAR_STRING_CALLBACK(Sprite, TextureFilename, "Sprite sheet", "white.dds", DISPLAY_HINT_TEXTUREFILE, NULL);
END_VAR_TABLE
