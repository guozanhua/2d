#include "Toolset2D_ManagedPCH.h"

#include "EngineInstanceSprite.hpp"
#include "Toolset2D_EnginePlugin/SpriteEntity.hpp"
#include "Toolset2D_EnginePlugin/SpriteManager.hpp"

using namespace ManagedFramework;
#using <mscorlib.dll>

namespace Toolset2D_Managed
{
	EngineInstanceSprite::EngineInstanceSprite() : IEngineShapeInstance()
	{
		m_bIsVisible = true;

		// #warning, #verify (jve) - It seems necessary to use CreateEntity since it calls Init/InitVars, which can't be
		// called manually inside Sprite due to protected linkage.
		m_pSprite = (Sprite *)Vision::Game.CreateEntity("Sprite", hkvVec3(0, 0, 0));
	}

	void EngineInstanceSprite::DisposeObject()
	{
		if (m_pSprite)
		{
			// Make sure nobody accidentally started referencing this without our knowledge
			VASSERT(m_pSprite->GetRefCount() == 1);
			m_pSprite->DisposeObject();
			m_pSprite = NULL;
		}
	}

	void EngineInstanceSprite::SetObjectKey(String ^key)
	{
		if (!m_pSprite)
			return;

		VString sKey;
		ConversionUtils::StringToVString(key, sKey);

		if (sKey.IsEmpty())
			m_pSprite->SetObjectKey(NULL);
		else
			m_pSprite->SetObjectKey(sKey);

		// inform owner object and its components, that the object key has changed
		m_pSprite->SendMsg(m_pSprite, VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR)"ObjectKey", 0);
	}

	void EngineInstanceSprite::SetPosition(float x, float y, float z)
	{
		m_pSprite->SetPosition(x, y, z);
	}

	void EngineInstanceSprite::SetOrientation(float yaw, float pitch, float roll)
	{
		m_pSprite->SetOrientation(yaw, pitch, roll);
	}

	void EngineInstanceSprite::SetScaling(float x, float y, float z)
	{
		m_pSprite->SetScaling( hkvVec3(x, y, z) );
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
					m_pSprite->DebugRender(pRI, 4.f, VColorRef(255,0,0,80), false);
				}
				break;

			case ShapeRenderMode::Selected:
				m_pSprite->DebugRender(pRI, 5.f, VColorRef(255,160,0,140), false);
				break;
			}
		}
	}

	bool EngineInstanceSprite::GetLocalBoundingBox(BoundingBox^ %bbox)
	{
		// create a small bounding box for picking
		float fSize = 5.f;
		(*bbox).Set(-fSize, -fSize, -fSize, fSize, fSize, fSize);
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

	bool EngineInstanceSprite::CanAttachComponent(ShapeComponent ^component, String ^%sError)
	{
		// Default implementation:
		//	return ConversionUtils::CanAttachComponent(GetO3DPtr(),component,sError);

		// For now, just allow script components
		return (component->PrefabIdentifier == "VScriptComponent");
	}

	void EngineInstanceSprite::OnAttachComponent(ShapeComponent ^component)
	{
		ConversionUtils::OnAttachComponent(m_pSprite, component);
	}

	void EngineInstanceSprite::SetShoeBoxData(String ^pFileName, String ^pXml)
	{
		VString sFileName;
		ConversionUtils::StringToVString(pFileName, sFileName);

		hkStringBuf filename;
		bool validFilename = convertToAssetPath(sFileName, filename);

		if (validFilename)
		{
			VString sXml;
			ConversionUtils::StringToVString(pXml, sXml);

			hkStringBuf xml;
			bool validXml = convertToAssetPath(sXml, xml);
			if (!validXml)
			{
				xml = "";
			}

			m_pSprite->SetShoeBoxData(filename, xml);
		}
	}

	array<String^>^ EngineInstanceSprite::GetStateNames()
	{
		const VArray<VString> stateNames = m_pSprite->GetStateNames();

		array<String^>^ names = gcnew array<String^>(stateNames.GetSize());
		for (int i = 0; i < stateNames.GetSize(); i++)
		{
			names[i] = ConversionUtils::VStringToString(stateNames[i]);
		}

		return names;
	}

	String^ EngineInstanceSprite::GetCurrentState()
	{
		String^ stateName = "NONE";
		const SpriteState *state = m_pSprite->GetCurrentState();
		if (state != NULL)
		{
			stateName = ConversionUtils::VStringToString(state->name);
		}
		return stateName;
	}

	void EngineInstanceSprite::SetCurrentState(String^ state)
	{
		VString stateName;
		ConversionUtils::StringToVString(state, stateName);
		m_pSprite->SetState(stateName);
	}

	int EngineInstanceSprite::GetCurrentFrame()
	{
		return m_pSprite->GetCurrentFrame();
	}

	void EngineInstanceSprite::SetCurrentFrame(int frame)
	{
		m_pSprite->SetCurrentFrame(frame);
	}

	float EngineInstanceSprite::GetScrollX()
	{
		return m_pSprite->GetScrollSpeed().x;
	}

	float EngineInstanceSprite::GetScrollY()
	{
		return m_pSprite->GetScrollSpeed().y;
	}

	void EngineInstanceSprite::SetScroll(float x, float y)
	{
		m_pSprite->SetScrollSpeed(hkvVec2(x, y));
	}

	void EngineInstanceSprite::SetFullscreenMode(bool enabled)
	{
		m_pSprite->SetFullscreenMode(enabled);
	}

	bool EngineInstanceSprite::IsFullscreenMode()
	{
		return m_pSprite->IsFullscreenMode();
	}
}