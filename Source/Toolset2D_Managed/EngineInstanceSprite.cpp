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

	void EngineInstanceSprite::SetVisible(bool bStatus)
	{
		if (m_pSprite)
		{
			m_pSprite->SetVisibleBitmask(bStatus ? VIS_ENTITY_VISIBLE : VIS_ENTITY_INVISIBLE);
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
			VColorRef color;
			bool render = false;
			float width = 1.f;

			// render this shape as a small bounding box. Use a slightly larger bounding box if it is selected
			switch (mode)
			{
			case ShapeRenderMode::Normal:
				if (m_pSprite->GetVisibleBitmask() & VIS_ENTITY_VISIBLE)
				{
					color = VColorRef(255, 0, 0, 80);
					render = true;
				}
				break;

			case ShapeRenderMode::Selected:
				color = VColorRef(255, 160, 0, 140);
				render = true;
				break;
			}

			if (render && !m_pSprite->IsFullscreenMode())
			{
				// Make sure the vertices are updated before rendering
				m_pSprite->Update();

				const int edges[8] = {
					0, 1,
					1, 3,
					3, 2,
					2, 0};

				const hkvVec2 *vertices = m_pSprite->GetVertices();

				for (int edgeIndex = 0; edgeIndex < 4; edgeIndex++)
				{
					const int p1 = edges[edgeIndex * 2 + 0];
					const int p2 = edges[edgeIndex * 2 + 1];
					Vision::Game.DrawSingleLine2D(
						vertices[p1].x, vertices[p1].y,
						vertices[p2].x, vertices[p2].y,
						color, width);
				}

				hkvVec2 position = m_pSprite->GetPosition().getAsVec2();
				hkvVec2 topLeft = position;
				hkvVec2 topRight = topLeft + hkvVec2(m_pSprite->GetOriginalCellWidth(), 0);
				hkvVec2 bottomLeft = topLeft + hkvVec2(0, m_pSprite->GetOriginalCellHeight());
				hkvVec2 bottomRight = bottomLeft + hkvVec2(m_pSprite->GetOriginalCellWidth(), 0);
				const hkvVec2 border[8] = {
					topLeft, topRight,
					topRight, bottomRight,
					bottomRight, bottomLeft,
					bottomLeft, topLeft};

				for (int borderEdgeIndex = 0; borderEdgeIndex < 4; borderEdgeIndex++)
				{
					const hkvVec2 p1 = border[borderEdgeIndex * 2 + 0];
					const hkvVec2 p2 = border[borderEdgeIndex * 2 + 1];
					Vision::Game.DrawSingleLine2D(
						p1.x, p1.y,
						p2.x, p2.y,
						VColorRef(255, 255, 0, 30), width);
				}
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
		// For now, just allow script components
		return (component->PrefabIdentifier == "VScriptComponent");
	}

	void EngineInstanceSprite::OnAttachComponent(ShapeComponent ^component)
	{
		ConversionUtils::OnAttachComponent(m_pSprite, component);
	}

	void EngineInstanceSprite::SetSpriteSheetData(String ^pFileName, String ^pXml)
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

			m_pSprite->SetSpriteSheetData(filename, xml);
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

	void EngineInstanceSprite::SetPlayOnce(bool enabled)
	{
		m_pSprite->SetPlayOnce(enabled);
	}

	bool EngineInstanceSprite::IsPlayOnce()
	{
		return m_pSprite->IsPlayOnce();
	}

	void EngineInstanceSprite::SetCollision(bool enabled)
	{
		m_pSprite->SetCollision(enabled);
	}

	bool EngineInstanceSprite::IsColliding()
	{
		return m_pSprite->IsColliding();
	}
}