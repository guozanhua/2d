#include "Toolset2D_ManagedPCH.h"

#include "EngineInstanceSprite.hpp"

#include "Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h"

using namespace ManagedFramework;
#using <mscorlib.dll>

namespace Toolset2D_Managed
{
	// create the native instance in the constructor
	EngineInstanceSprite::EngineInstanceSprite()
	{
		m_bIsVisible = true;
		m_pSprite = (Sprite *)Vision::Game.CreateEntity("Sprite", hkvVec3(0, 0, 0));
	}

	// destroy the native instance of the node as well
	void EngineInstanceSprite::DisposeObject()
	{
		if (m_pSprite)
		{
			VASSERT(m_pSprite->GetRefCount() == 1);
			m_pSprite->DisposeObject();
			m_pSprite = NULL;
		}
	}

	void EngineInstanceSprite::SetPosition(float x,float y,float z)
	{
		m_pSprite->SetPosition(x, y, z);
	}

	void EngineInstanceSprite::SetOrientation(float yaw,float pitch,float roll)
	{
		m_pSprite->SetOrientation(yaw, pitch, roll);
	}

	void EngineInstanceSprite::RenderShape(VisionViewBase^ /*view*/, CSharpFramework::Shapes::ShapeRenderMode mode)
	{
		if (m_pSprite != NULL)
		{
			IVRenderInterface* pRI = Vision::Contexts.GetCurrentContext()->GetRenderInterface();

			// render this shape as a small bounding box. Use a slightly larger bounding box if it is selected
			switch (mode)
			{
			case ShapeRenderMode::Normal:
				if (m_bIsVisible)
				{
					m_pSprite->DebugRender(pRI,4.f,VColorRef(255,0,0,80), false);
				}
				break;
			case ShapeRenderMode::Selected:
				m_pSprite->DebugRender(pRI,5.f,VColorRef(255,160,0,140), false);
				break;
			}
		}
	}

	bool EngineInstanceSprite::GetLocalBoundingBox(BoundingBox^ %bbox)
	{
		// create a small bounding box for picking
		float fSize = 5.f;
		(*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
		return true;
	}

	void EngineInstanceSprite::TraceShape(Shape3D^ /* ownerShape*/, Vector3F /*rayStart*/,Vector3F /*rayEnd*/, ShapeTraceResult^% /*result*/)
	{
		// don't do anything because the shape code already does the picking
	}
	
	bool EngineInstanceSprite::OnExport(SceneExportInfo ^info)
	{
		// do the export: Get the export binary archive and serialize into it.
		// Requires the native node class to support serialization (derive from VisTypedEngineObject_cl and 
		// implement Serialize function)
		VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
		ar.WriteObject(m_pSprite);
		return true;
	}

	void EngineInstanceSprite::SetShoeBoxData(String ^pFileName, String ^pXml)
	{
		VString sFileName;
		ConversionUtils::StringToVString(pFileName, sFileName);

		VString sXml;
		ConversionUtils::StringToVString(pXml, sXml);

		if (!sFileName.IsEmpty())
		{
			m_pSprite->SetShoeBoxData(sFileName, sXml);
		}
	}
}