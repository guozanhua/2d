#ifndef SPRITE_ENTITY_HPP_INCLUDED
#define SPRITE_ENTITY_HPP_INCLUDED

struct SpriteState;
struct SpriteCell;

class SpriteData;

class Sprite : public VisBaseEntity_cl
{
public:
	V_DECLARE_SERIAL_DLLEXP(Sprite, TOOLSET_2D_IMPEXP);

	IMPLEMENT_OBJ_CLASS(Sprite);

	enum VERTEX_INDICES
	{
		VERTEX_TOP_LEFT = 0,
		VERTEX_TOP_RIGHT,
		VERTEX_BOTTOM_LEFT,
		VERTEX_BOTTOM_RIGHT,
		VERTEX_NUM_VERTS
	};

	TOOLSET_2D_IMPEXP Sprite();
	TOOLSET_2D_IMPEXP ~Sprite();

	// Overridden entity functions
	TOOLSET_2D_IMPEXP VOVERRIDE void InitFunction();
	TOOLSET_2D_IMPEXP VOVERRIDE void DeInitFunction();
	TOOLSET_2D_IMPEXP VOVERRIDE void ThinkFunction();
  
	TOOLSET_2D_IMPEXP VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

	// Serialization and type management
	TOOLSET_2D_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
	TOOLSET_2D_IMPEXP VOVERRIDE void OnSerialized( VArchive &ar );

	// Sender the shape in editor mode (or for debugging)
	TOOLSET_2D_IMPEXP void DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections=false) const;

	TOOLSET_2D_IMPEXP void Render(IVRender2DInterface *pRender, VSimpleRenderState_t& state);

	TOOLSET_2D_IMPEXP bool SetSpriteSheetData(const char *spriteSheetFilename, const char *xmlFilename);

	TOOLSET_2D_IMPEXP const VArray<VString> GetStateNames() const;
	TOOLSET_2D_IMPEXP const SpriteState *GetCurrentState() const;

	TOOLSET_2D_IMPEXP int GetCurrentFrame() const;
	TOOLSET_2D_IMPEXP void SetCurrentFrame(int frame);

	TOOLSET_2D_IMPEXP float GetCellWidth() const;
	TOOLSET_2D_IMPEXP float GetCellHeight() const;

	TOOLSET_2D_IMPEXP float GetOriginalCellWidth() const;
	TOOLSET_2D_IMPEXP float GetOriginalCellHeight() const;

	TOOLSET_2D_IMPEXP bool IsOverlapping(Sprite *other) const;

	TOOLSET_2D_IMPEXP void Update();

	TOOLSET_2D_IMPEXP const hkvVec2 *GetVertices() const;
	TOOLSET_2D_IMPEXP hkvAlignedBBox GetBBox() const;

	TOOLSET_2D_IMPEXP const SpriteCell *GetCurrentCell() const;
	
	//----- Utility functions exposed to LUA

	TOOLSET_2D_IMPEXP void OnCollision(Sprite *other);

	TOOLSET_2D_IMPEXP bool SetState(const char *state);

	// Specify a value between 0 and 1 and it will update the frame
	TOOLSET_2D_IMPEXP void SetFramePercent(float percent);

	TOOLSET_2D_IMPEXP void Pause();
	TOOLSET_2D_IMPEXP void Play();

	TOOLSET_2D_IMPEXP void SetScrollSpeed(hkvVec2 m_scrollSpeed);
	TOOLSET_2D_IMPEXP const hkvVec2 &GetScrollSpeed() const;

	TOOLSET_2D_IMPEXP void SetFullscreenMode(bool enabled);
	TOOLSET_2D_IMPEXP bool IsFullscreenMode() const;

	TOOLSET_2D_IMPEXP void SetPlayOnce(bool enabled);
	TOOLSET_2D_IMPEXP bool IsPlayOnce() const;

	TOOLSET_2D_IMPEXP void SetCollision(bool enabled);
	TOOLSET_2D_IMPEXP bool IsColliding() const;

	TOOLSET_2D_IMPEXP void SetConvexHullCollision(bool enabled);
	TOOLSET_2D_IMPEXP bool IsConvexHullCollision() const;

	TOOLSET_2D_IMPEXP hkvVec3 GetPoint(float x, float y, float z = 0.0f) const;
	TOOLSET_2D_IMPEXP void SetCenterPosition(const hkvVec3 &position);
	TOOLSET_2D_IMPEXP hkvVec3 GetCenterPosition() const;

	TOOLSET_2D_IMPEXP const hkpConvexTransformShape *GetShape() const;
	TOOLSET_2D_IMPEXP hkQsTransform GetTransform() const;

	// Managed code doesn't like dealing with hkVector4 so providing a helper
	TOOLSET_2D_IMPEXP hkvVec2 TransformVertex(const hkVector4 &vertex) const;

	TOOLSET_2D_IMPEXP float GetWidth() const;
	TOOLSET_2D_IMPEXP float GetHeight() const;

	TOOLSET_2D_IMPEXP void SetWidth(float width);
	TOOLSET_2D_IMPEXP void SetHeight(float height);

	TOOLSET_2D_IMPEXP Sprite *Clone(const hkvVec3 *position = NULL) const;

protected:
	void CommonInit();
	void CommonDeInit();

	void Clear();

	void RemoveShape();

	void UpdateTextures();
	VTextureObject *GetTexture() const;

	hkvVec2 GetDimensions() const;

private:

	// stores the last cell used to generate the shape
	const SpriteCell *m_lastGeneratedCell;
	hkpConvexTransformShape *m_shape2D;

	hkvVec2 m_scrollSpeed;
	bool m_fullscreen;
	int m_currentState;
	int m_currentFrame;
	float m_frameTime;
	bool m_paused;
	bool m_playOnce;
	bool m_collide;
	hkvVec2 m_scrollOffset;
	bool m_convexHullCollision;

	const SpriteData *m_spriteData;

	Overlay2DVertex_t m_renderVertices[6];
	hkvVec2 m_vertices[4];
	hkvVec2 m_texCoords[4];

	VString m_spriteSheetFilename;
	VString m_xmlDataFilename;

	bool m_offscreen;
};

#endif // SPRITE_ENTITY_HPP_INCLUDED