#ifndef SPRITE_ENTITY_HPP_INCLUDED
#define SPRITE_ENTITY_HPP_INCLUDED

struct SpriteCell
{
	VString name;
	hkvVec2 offset;
	hkvVec2 pivot;
	int width;
	int height;
	int originalWidth;
	int originalHeight;
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

	enum VERTEX_INDICES
	{
		VERTEX_TOP_LEFT = 0,
		VERTEX_TOP_RIGHT = 1,
		VERTEX_BOTTOM_LEFT = 2,
		VERTEX_BOTTOM_RIGHT = 3
	};

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

	TOOLSET_2D_IMPEXP bool IsOverlapping(Sprite *other) const;

	TOOLSET_2D_IMPEXP void Update();

	//----- Utility functions exposed to LUA

	TOOLSET_2D_IMPEXP void OnCollision(Sprite *other);

	TOOLSET_2D_IMPEXP bool SetState(const char *state);

	// Specify a value between 0 and 1 and it will update the frame
	TOOLSET_2D_IMPEXP void SetFramePercent(float percent);

	TOOLSET_2D_IMPEXP void Pause();
	TOOLSET_2D_IMPEXP void Play();

	TOOLSET_2D_IMPEXP void SetScrollSpeed(hkvVec2 scrollSpeed);
	TOOLSET_2D_IMPEXP const hkvVec2 &GetScrollSpeed() const;

	TOOLSET_2D_IMPEXP void SetFullscreenMode(bool enabled);
	TOOLSET_2D_IMPEXP bool IsFullscreenMode() const;

	TOOLSET_2D_IMPEXP void SetPlayOnce(bool enabled);
	TOOLSET_2D_IMPEXP bool IsPlayOnce() const;

	TOOLSET_2D_IMPEXP void SetCollision(bool enabled);
	TOOLSET_2D_IMPEXP bool IsColliding() const;

	TOOLSET_2D_IMPEXP hkvVec3 GetPoint(float x, float y, float z = 0.0f) const;
	TOOLSET_2D_IMPEXP void SetCenterPosition(const hkvVec3 &position);
	TOOLSET_2D_IMPEXP hkvVec3 GetCenterPosition();

	TOOLSET_2D_IMPEXP float GetWidth() const;
	TOOLSET_2D_IMPEXP float GetHeight() const;

protected:
	void CommonInit();
	void CommonDeInit();

	void Clear();

	void ClearTextures();
	bool UpdateTextures();
	VTextureObject *GetTexture() const;
	const hkvVec2 *GetVertices() const;

	hkvVec2 GetDimensions() const;

private:

	//----- Properties

	float TextureScale;

	hkvVec2 ScrollSpeed;
	//int BlendMode;
	bool Fullscreen;

	//----- Member variables

	// Tracks whether or not the spritesheet and XML data have been loaded
	bool m_loaded;

	int m_sourceWidth;
	int m_sourceHeight;

	int m_currentState;
	int m_currentFrame;
	float m_frameTime;
	bool m_paused;
	bool m_playOnce;
	bool m_collide;
	hkvVec2 m_scrollOffset;

	VTextureObjectPtr m_spSpriteSheetTexture;
	VisTextureAnimInstancePtr m_spTextureAnimation;

	VString m_spriteSheetFilename;
	VString m_xmlDataFilename;
	
	VArray<SpriteCell> m_cells;
	VArray<SpriteState> m_states;
	hkvVec2 m_vertices[4];
	hkvVec2 m_texCoords[4];

	VDictionary<int> m_stateNameToIndex;
};

#endif // SPRITE_ENTITY_HPP_INCLUDED