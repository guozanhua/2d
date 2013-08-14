#include "Toolset2D_EnginePluginPCH.h"

#include "SpriteEntity.hpp"

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

V_IMPLEMENT_SERIAL(Sprite, VisBaseEntity_cl, 0, &gSpriteShapeEngineModule);

inline void InitVertex(int index, VisMBSimpleVertex_t *pVert, float x, float y, float z)
{
	VColorRef iColor( (index == 2) ? 0 : 255,
					  (index == 1) ? 0 : 255,
					  (index == 0) ? 0 : 255,
					  (index % 3) * 85 );
	pVert->color = iColor.GetNative();
	
	pVert->texcoord0[0] = x * 0.05f;
	pVert->texcoord0[1] = y * 0.05f;

	pVert->pos[0] = x;
	pVert->pos[1] = y;
	pVert->pos[2] = z;
}

// See ticket #: 619-8332539
// Helper class that implements IVRender2DInterface which is passed to font printing. Then it
// re-directs the Draw2DBuffer function to fill a VDynamicMeshBuilder instance
class VFontToMeshPrinter : public IVRender2DInterface
{
public:
	VFontToMeshPrinter() 
	{
		m_iVertexCount = 0;
		m_pBuilder = NULL;

		// change these if needed or add new properties:
		m_Surface.SetLightingMode(VIS_LIGHTING_LIGHTGRID);
		m_Surface.SetTransparencyType(VIS_TRANSP_ALPHATEST);
		m_Surface.SetDoubleSided(true);
	}

	void Reset()
	{
		m_iVertexCount = 0;
	}

	int GetVertexCount() const { return m_iVertexCount; }
	void SetMeshBuilder(VDynamicMeshBuilder *builder) { m_pBuilder = builder; }
	const VisSurface_cl &GetVisSurface() const { return m_Surface; }

	virtual void Draw2DBuffer(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, const VSimpleRenderState_t &iProperties) 
	{
		if (m_pBuilder!=NULL)
		{
			for (int i = 0; i < iVertexCount; i++, pVertices++)
			{
				m_pBuilder->AddVertex(
					// position = write along x and z axis
					hkvVec3(pVertices->screenPos.x, 0, -pVertices->screenPos.y),
					// normal (hardcoded)
					hkvVec3(0, -1.f, 0),
					// tangent (hardcoded)
					hkvVec3(1, 0, 0),
					pVertices->texCoord,
					pVertices->color
					);
			}

			// add a trivial index list
			for (int i = 0; i < iVertexCount; i += 3)
			{
				m_pBuilder->AddTriangle(m_iVertexCount + i,
										m_iVertexCount + i + 1,
										m_iVertexCount + i + 2);
			}

			VASSERT_MSG(m_Surface.m_spDiffuseTexture==NULL || m_Surface.m_spDiffuseTexture==pTexture,
						"Font with multiple pages not supported in this sample");

			m_Surface.m_spDiffuseTexture = pTexture;
		}

		m_iVertexCount += iVertexCount;
	}

	virtual void Draw2DBufferWithShader(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, VCompiledShaderPass &shader)
	{
		VASSERT_MSG(false, "not implemented");
	}

	virtual void SetScissorRect(const VRectanglef *pScreenRect)
	{
		VASSERT_MSG(false, "not implemented");
	}

	virtual void SetDepth(float fZCoord)
	{
	}

	virtual float GetDepth() const
	{
		VASSERT_MSG(false, "not implemented");
		return 0.f;
	}

	virtual void SetTransformation(const hkvVec4 *pScaleAndOfs)
	{
		VASSERT_MSG(false, "not implemented");
	}

private:
	int m_iVertexCount;
	VDynamicMeshBuilder *m_pBuilder;
	VisSurface_cl m_Surface;
};


VDynamicMesh *CreateTextMesh(VisFont_cl *pFont, const char *szText, VColorRef iColor)
{
	VFontToMeshPrinter printer;
	VDynamicMesh* pMesh = NULL;

	// count the vertices:
	pFont->PrintText(&printer, hkvVec2(0, 0), szText, iColor);

	const int vertexCount = printer.GetVertexCount();

	// build the mesh
	if (vertexCount > 0)
	{ 
		// scope, because the builder holds a reference which would crash at the end of the function when we add VRESOURCEFLAG_AUTODELETE
		VDynamicMeshBuilder builder(vertexCount, vertexCount / 3, 0, 1);

		printer.Reset();

		// set the builder as an output
		printer.SetMeshBuilder(&builder);

		// print again to add vertices
		pFont->PrintText(&printer, hkvVec2(0, 0), szText, iColor);

		// setup surface properties (see VFontToMeshPrinter c'tor)
		builder.CopySurfaceFrom(0, printer.GetVisSurface());

		pMesh = builder.Finalize();
	}

	if (pMesh != NULL)
	{
		pMesh->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
	}

	return pMesh;
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
	//m_spriteMeshBuffer->Remove;
	m_spriteMeshBuffer = NULL;
}

// called by our InitFunction and our de-serialization code
void Sprite::CommonInit()
{ 
	// does not need to be serialized
	m_fTimePos = Vision::Game.GetFloatRand()*hkvMath::pi () * 2.0f;

	if (Vision::Editor.IsPlaying())
	{
		VDynamicMesh* pMesh = NULL;

		{
			VDynamicMeshBuilder b(4, 2, 0, 1);
			const hkvVec3 n;
			const hkvVec3 t;
			const hkvVec2 uv;
			VColorRef color;
			b.AddVertex( hkvVec3(0, 30, 0), n, t, uv, color );
			b.AddVertex( hkvVec3(0, 30, 30), n, t, uv, color );
			b.AddVertex( hkvVec3(0, 0, 30), n, t, uv, color );
			b.AddVertex( hkvVec3(0, 0, 0), n, t, uv, color );
			b.AddTriangle(0, 1, 2);
			b.AddTriangle(2, 1, 0);
			pMesh = b.Finalize();
		}

		if (pMesh != NULL)
		{
			pMesh->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
		}

		SetMesh(pMesh);

		/*
		m_spriteMeshBuffer = new VisMeshBuffer_cl();
		
		// texture
		m_spSpriteSheetTexture = Vision::TextureManager.Load2DTexture("Textures\\SpriteSheets\\EnemyShip.png", VTM_FLAG_DEFAULT_MIPMAPPED);
		VASSERT(m_spSpriteSheetTexture);
		m_spriteMeshBuffer->SetBaseTexture(m_spSpriteSheetTexture);

		// alloc
		m_spriteMeshBuffer->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
		m_spriteMeshBuffer->AllocateVertices(VisMBSimpleVertex_t::VertexDescriptor, 4);
		m_spriteMeshBuffer->AllocateIndexList(3);
		
		VisMBSimpleVertex_t *pVert = (VisMBSimpleVertex_t *)m_spriteMeshBuffer->LockVertices(0);

		InitVertex(0, pVert,-30.f,0,0); pVert++;
		InitVertex(1, pVert,30.f,0,0); pVert++;
		InitVertex(2, pVert,0,30.f,0); pVert++;
		InitVertex(3, pVert,0.f,0,45.f); pVert++;

		m_spriteMeshBuffer->UnLockVertices();

		unsigned short *pIndex = (unsigned short *)m_spriteMeshBuffer->LockIndices(0);
		pIndex[0] = 0;
		pIndex[1] = 1;
		pIndex[2] = 2;

		m_spriteMeshBuffer->UnLockIndices();
		*/

		// alloc
		/*
		m_spriteMeshBuffer->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
		m_spriteMeshBuffer->AllocateVertices(VisMBSimpleVertex_t::VertexDescriptor, 3);
		m_spriteMeshBuffer->AllocateIndexList(6);

		const float rad = 1.f;
		const float zval = 0.9f;  
		VisMBSimpleVertex_t *pVert = (VisMBSimpleVertex_t *)m_spriteMeshBuffer->LockVertices(0);
		InitVertex(0, pVert, -rad,   0, zval); pVert++;
		InitVertex(1, pVert,  rad,   0, zval); pVert++;
		InitVertex(2, pVert,    0, rad, zval); pVert++;
		m_spriteMeshBuffer->UnLockVertices();

		unsigned short *pIndex = (unsigned short *)m_spriteMeshBuffer->LockIndices(0);
		pIndex[0] = 0;
		pIndex[1] = 1;
		pIndex[2] = 2;
		pIndex[3] = 2;
		pIndex[4] = 1;
		pIndex[5] = 0;
		m_spriteMeshBuffer->UnLockIndices();
		*/

		/*
		// create a static mesh using the mesh buffer previously created
		m_staticMesh = new VisStaticMesh_cl();
		m_staticMesh->FlagAsLoaded();
		m_staticMesh->SetFilename("<static mesh filename>");
		//m_staticMesh->SetBoundingBox(bbox); // compute bounding box out of vertex data
		m_staticMesh->SetMeshBuffer(m_spriteMeshBuffer);

		// set the material
		m_staticMesh->AllocateSurfaces(1);
		VisSurface_cl* pMaterial = m_staticMesh->GetSurface(0);
		pMaterial->SetBaseTexture(m_spriteMeshBuffer->GetChannelTexture(0));
		pMaterial->SetLightingMode(VIS_LIGHTING_FULLBRIGHT);
		pMaterial->SetEffect(NULL); // let the shader provider find an effect

		// initialize the submesh
		m_staticMesh->AllocateSubmeshes(1);

		VisStaticSubmesh_cl* pSubmesh = m_staticMesh->GetSubmesh(0);
		//pSubmesh->SetBoundingBox(bbox);
		pSubmesh->SetRenderRange(0, m_spriteMeshBuffer->GetIndexCount());
		pSubmesh->SetRenderVertexRange(0, m_spriteMeshBuffer->GetVertexCount());
		pSubmesh->SetSurface(pMaterial, 0);

		hkvMat4 mTransform(hkvNoInitialization);
		mTransform.setIdentity();

		m_staticMeshInstance = m_staticMesh->CreateInstance(mTransform);

		// insert into visibility zones
		m_staticMeshInstance->AssignToVisibilityZones();
		*/

		/*
		char szNewText[1024];
		sprintf(szNewText,"This is Frame #%i",20);
		VisFont_cl *pFont = &Vision::Fonts.ConsoleFont();
		SetMesh(CreateTextMesh(pFont, szNewText, V_RGBA_WHITE));
		*/
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

	VSimpleRenderState_t	state;
	hkvVec2					tl(450,80);hkvVec2	br(850,850);
	hkvVec2					tl2(50,50);hkvVec2	br2(650,350);
	
	IVRender2DInterface *pRender = Vision::RenderLoopHelper.BeginOverlayRendering();
	pRender->SetDepth(512.f);
	pRender->SetScissorRect(NULL);
	state.SetTransparency(VIS_TRANSP_ALPHA);

	pRender->DrawSolidQuad(tl,br,VColorRef(255,0,255,100),state);
	pRender->DrawSolidQuad(tl2,br2,VColorRef(0,255,0,128),state);
	
	Vision::RenderLoopHelper.EndOverlayRendering();
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

// render the node (and optionally the connections)
void Sprite::DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections) const
{
	/*
	VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
	hkvAlignedBBox bbox;
	hkvVec3 vRad(fSize,fSize,fSize);
	bbox.m_vMin = m_vPos-vRad;
	bbox.m_vMax = m_vPos+vRad;
	pRenderer->RenderAABox(bbox,iColor,state);
	*/
}

// Serialization function : Used when exported by vForge and loaded by scene viewer
void Sprite::Serialize( VArchive &ar )
{
	VisBaseEntity_cl::Serialize(ar);
	if (ar.IsLoading())
	{
		m_vCenterPos.SerializeAsVisVector (ar);
	} 
	else
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

// vartable; provide the following members to vForge. Although vPhysXEntity is the base class we use VisBaseEntity_cl here to hide physX properties
START_VAR_TABLE(Sprite, VisBaseEntity_cl, "Sprite", 0, "")
	DEFINE_VAR_STRING_CALLBACK(Sprite, TextureFilename, "Sprite sheet", "white.dds", DISPLAY_HINT_TEXTUREFILE, NULL);
END_VAR_TABLE