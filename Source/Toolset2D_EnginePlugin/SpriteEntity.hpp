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
	SpriteState() :
	// Initialize the cells array to avoid allocations
	//   TODO: This might be a bigger than it needs to be
	cells(100)
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
	
	// Overridden entity functions
	TOOLSET_2D_IMPEXP VOVERRIDE void InitFunction();
	TOOLSET_2D_IMPEXP VOVERRIDE void DeInitFunction();
	TOOLSET_2D_IMPEXP VOVERRIDE void ThinkFunction();
  
	TOOLSET_2D_IMPEXP VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);
	TOOLSET_2D_IMPEXP VOVERRIDE BOOL AddComponent(IVObjectComponent *pComponent);

	// Serialization and type management
	TOOLSET_2D_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
	TOOLSET_2D_IMPEXP VOVERRIDE void OnSerialized( VArchive &ar );

	// Sender the shape in editor mode (or for debugging)
	TOOLSET_2D_IMPEXP void DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections=false) const;

	TOOLSET_2D_IMPEXP void Render(IVRender2DInterface *pRender, VSimpleRenderState_t& state);

	TOOLSET_2D_IMPEXP bool SetShoeBoxData(const char *spriteSheetFilename, const char *xmlFilename);

	TOOLSET_2D_IMPEXP const VArray<VString> GetStateNames() const;
	TOOLSET_2D_IMPEXP const SpriteState *GetCurrentState() const;

	TOOLSET_2D_IMPEXP int GetCurrentFrame() const;
	TOOLSET_2D_IMPEXP void SetCurrentFrame(int frame);

	//----- Utility functions exposed to LUA

	TOOLSET_2D_IMPEXP bool SetState(const char *state);

	// Specify a value between 0 and 1 and it will update the frame
	TOOLSET_2D_IMPEXP void SetFramePercent(float percent);

	TOOLSET_2D_IMPEXP void Pause();
	TOOLSET_2D_IMPEXP void Play();

	TOOLSET_2D_IMPEXP void SetScrollSpeed(hkvVec2 scrollSpeed);

protected:
	void CommonInit();
	void CommonDeInit();

private:

	//----- Properties

	float TextureScale;

	hkvVec2 ScrollSpeed;
	//int BlendMode;

	//----- Member variables

	bool m_loaded;

	int m_currentState;
	int m_currentFrame;
	float m_frameTime;
	bool m_paused;
	hkvVec2 m_scrollOffset;

	VTextureObjectPtr m_spSpriteSheetTexture;

	VString m_spriteSheetFilename;
	VString m_xmlDataFilename;
	
	VArray<SpriteCell> m_cells;
	VArray<SpriteState> m_states;
	VDictionary<int> m_stateNameToIndex;

	VisMeshBuffer_cl *m_spriteMeshBuffer;
	VisStaticMeshPtr m_staticMesh;
	VisStaticMeshInstance_cl *m_staticMeshInstance;
};

#endif
