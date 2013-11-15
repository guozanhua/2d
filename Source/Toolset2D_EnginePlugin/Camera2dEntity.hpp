#ifndef CAMERA_2D_ENTITY_HPP_INCLUDED
#define CAMERA_2D_ENTITY_HPP_INCLUDED

class Camera2D : public VisBaseEntity_cl
{
public:
	V_DECLARE_SERIAL_DLLEXP(Camera2D, TOOLSET_2D_IMPEXP);

	IMPLEMENT_OBJ_CLASS(Camera2D);

	TOOLSET_2D_IMPEXP Camera2D();
	TOOLSET_2D_IMPEXP ~Camera2D();

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

protected:
	void CommonInit();
	void CommonDeInit();

	void Clear();
};

#endif // CAMERA_2D_ENTITY_HPP_INCLUDED