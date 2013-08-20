#ifndef SpriteENTITY_HPP_INCLUDED
#define SpriteENTITY_HPP_INCLUDED

#include "Toolset2D_EnginePluginPCH.h"

struct SpriteCell
{
	VString name;
	hkvVec2 offset;
	hkvVec2 pivot;
	int width;
	int height;
	int index;
};

struct SpriteState
{
	SpriteState() : cells(100)
	{

	}
	VString name;
	VArray<int> cells;
	float framerate;
};

class Sprite : public VisBaseEntity_cl
{
public:
	V_DECLARE_SERIAL_DLLEXP(Sprite, TOOLSET_2D_IMPEXP);

	IMPLEMENT_OBJ_CLASS(Sprite);
	
	// overridden entity functions
	TOOLSET_2D_IMPEXP VOVERRIDE void InitFunction();
	TOOLSET_2D_IMPEXP VOVERRIDE void DeInitFunction();
	TOOLSET_2D_IMPEXP VOVERRIDE void ThinkFunction();
  
	TOOLSET_2D_IMPEXP VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

	// serialization and type management
	TOOLSET_2D_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
	TOOLSET_2D_IMPEXP VOVERRIDE void OnSerialized( VArchive &ar );

	// render the shape in editor mode (or for debugging)
	TOOLSET_2D_IMPEXP void DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections=false) const;

	TOOLSET_2D_IMPEXP void Render(IVRender2DInterface *pRender, VSimpleRenderState_t& state);

	TOOLSET_2D_IMPEXP bool SetShoeBoxData(const char *spriteSheetFilename, const char *xmlFilename);

	TOOLSET_2D_IMPEXP bool SetState(const char *state);

protected:
	void CommonInit();
	void CommonDeInit();

private:

	// Properties
	//-----

	float TextureScale;

	// Member variables
	//-----

	VTextureObjectPtr m_spSpriteSheetTexture;

	VString m_spriteSheetFilename;
	VString m_xmlDataFilename;

	int m_currentState;
	int m_currentFrame;
	float m_frameTime;

	VArray<SpriteCell> m_cells;
	VArray<SpriteState> m_states;
	VDictionary<int> m_stateNameToIndex;

	VisMeshBuffer_cl *m_spriteMeshBuffer;
	VisStaticMeshPtr m_staticMesh;
	VisStaticMeshInstance_cl *m_staticMeshInstance;
};

#endif
