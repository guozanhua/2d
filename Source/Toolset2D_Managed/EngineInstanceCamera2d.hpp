#pragma once

using namespace System;
using namespace System::Diagnostics;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;

class Camera2D;

namespace Toolset2D_Managed
{
	/// <summary>
	/// EngineInstanceCamera2d : class that manages the native Sprite entity class
	/// </summary>
	public ref class EngineInstanceCamera2d : public IEngineShapeInstance
	{
	public:
		EngineInstanceCamera2d();

		VOVERRIDE void DisposeObject() override;

		VOVERRIDE void SetVisible(bool bStatus) override;
		VOVERRIDE void SetObjectKey(String ^key) override;
		VOVERRIDE void SetPosition(float x,float y,float z) override;
		VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
		VOVERRIDE bool GetOrientation(Vector3F %orientation) override;
		VOVERRIDE void SetScaling(float x, float y, float z) override;
		VOVERRIDE bool GetLocalBoundingBox(BoundingBox^ %bbox) override;
		VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult^ %result) override;
		VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
		VOVERRIDE void SetUniqueID(unsigned __int64 iID) override;

		VOVERRIDE bool CanAttachComponent(ShapeComponent ^component, String ^%sError) override;
		VOVERRIDE void OnAttachComponent(ShapeComponent ^component) override;

		VOVERRIDE IntPtr GetNativeObject() override
		{
			return System::IntPtr(GetCamera2dEntity());
		}

		inline Camera2D *GetCamera2dEntity()
		{
			Debug::Assert( m_pEntityWP != nullptr );
			return (Camera2D *)m_pEntityWP->GetPtr();
		}

		// special functions
		void RenderShape(VisionViewBase ^view, CSharpFramework::Shapes::ShapeRenderMode mode);

	private:		
		// pointer to native engine object
		VWeakPtr<VisBaseEntity_cl> *m_pEntityWP;
	};
}
