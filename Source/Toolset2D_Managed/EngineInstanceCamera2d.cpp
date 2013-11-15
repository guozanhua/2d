#include "Toolset2D_ManagedPCH.h"

#include "EngineInstanceCamera2d.hpp"

#include "Toolset2D_EnginePlugin/Camera2dEntity.hpp"

using namespace ManagedFramework;
#using <mscorlib.dll>

namespace Toolset2D_Managed
{
	EngineInstanceCamera2d::EngineInstanceCamera2d() : IEngineShapeInstance()
	{
		Camera2D *camera = (Camera2D *)Vision::Game.CreateEntity("Camera2D", hkvVec3(0, 0, 0));
		Debug::Assert(camera != nullptr, "Could not create Camera 2D entity!");

		m_pEntityWP = new VWeakPtr<VisBaseEntity_cl>(camera->GetWeakReference());
	}

	void EngineInstanceCamera2d::DisposeObject()
	{
		Camera2D *pEntity = GetCamera2dEntity();
		V_SAFE_DISPOSEOBJECT(pEntity);
	}

	void EngineInstanceCamera2d::SetVisible(bool bStatus)
	{
		if (GetCamera2dEntity())
		{
			GetCamera2dEntity()->SetVisibleBitmask(bStatus ? VIS_ENTITY_VISIBLE : VIS_ENTITY_INVISIBLE);
		}
	}

	void EngineInstanceCamera2d::SetObjectKey(String ^key)
	{
		if (!GetCamera2dEntity())
			return;

		VString sKey;
		ConversionUtils::StringToVString(key, sKey);

		if (sKey.IsEmpty())
			GetCamera2dEntity()->SetObjectKey(NULL);
		else
			GetCamera2dEntity()->SetObjectKey(sKey);

		// inform owner object and its components, that the object key has changed
		GetCamera2dEntity()->SendMsg(GetCamera2dEntity(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR)"ObjectKey", 0);
	}

	void EngineInstanceCamera2d::SetPosition(float x, float y, float z)
	{
		if (GetCamera2dEntity())
		{
			GetCamera2dEntity()->SetPosition(x, y, z);
		}
	}

	void EngineInstanceCamera2d::SetOrientation(float yaw, float pitch, float roll)
	{
		if (GetCamera2dEntity())
		{
			GetCamera2dEntity()->SetOrientation(yaw, pitch, roll);
		}
	}

	bool EngineInstanceCamera2d::GetOrientation(Vector3F %orientation)
	{
		if (GetCamera2dEntity())
		{
			const hkvVec3 &o = GetCamera2dEntity()->GetOrientation();
			orientation.X = o.x;
			orientation.Y = o.y;
			orientation.Z = o.z;
		}
		return (GetCamera2dEntity() != NULL);
	}

	void EngineInstanceCamera2d::SetScaling(float x, float y, float z)
	{
		if (GetCamera2dEntity())
		{
			GetCamera2dEntity()->SetScaling( hkvVec3(x, y, z) );
		}
	}

	void EngineInstanceCamera2d::RenderShape(VisionViewBase^ /*view*/, CSharpFramework::Shapes::ShapeRenderMode mode)
	{
	}

	bool EngineInstanceCamera2d::GetLocalBoundingBox(BoundingBox^ %bbox)
	{
		// create a small bounding box for picking
		float fSize = 5.f;
		(*bbox).Set(-fSize, -fSize, -fSize, fSize, fSize, fSize);
		return true;
	}

	void EngineInstanceCamera2d::TraceShape(Shape3D^ /* ownerShape*/, Vector3F /*rayStart*/,Vector3F /*rayEnd*/, ShapeTraceResult^% /*result*/)
	{
		// don't do anything because the shape code already does the picking
	}
	
	bool EngineInstanceCamera2d::OnExport(SceneExportInfo ^info)
	{
		// do the export: Get the export binary archive and serialize into it.
		// Requires the native node class to support serialization (derive from VisTypedEngineObject_cl and 
		// implement Serialize function)
		if (GetCamera2dEntity())
		{
			VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
			ar.WriteObject(GetCamera2dEntity());
		}
		return true;
	}

	void EngineInstanceCamera2d::SetUniqueID(unsigned __int64 iID)
	{
		if (GetCamera2dEntity())
		{
			GetCamera2dEntity()->SetUniqueID(iID);
		}
	}

	bool EngineInstanceCamera2d::CanAttachComponent(ShapeComponent ^component, String ^%sError)
	{
		// For now, just allow script components
		return (component->PrefabIdentifier == "VScriptComponent");
	}

	void EngineInstanceCamera2d::OnAttachComponent(ShapeComponent ^component)
	{
		if (GetCamera2dEntity())
		{
			ConversionUtils::OnAttachComponent(GetCamera2dEntity(), component);
		}
	}
}