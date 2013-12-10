//=======
//
// Author: Joel Van Eenwyk
// Purpose: Handle sprite setup and rendering
//
//=======

#include "Toolset2D_EnginePluginPCH.h"

#include "Camera2dEntity.hpp"
#include "Toolset2dManager.hpp"

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/tinyxml.h>

#define CURRENT_CAMERA_2D_VERSION 1

V_IMPLEMENT_SERIAL(Camera2D, VisBaseEntity_cl, 0, &gToolset2D_EngineModule);

Camera2D::Camera2D()
{
}

Camera2D::~Camera2D()
{

}

// Called by the engine when entity is created. Not when it is de-serialized!
void Camera2D::InitFunction()
{
	Toolset2dManager::Instance()->SetCamera(this);

	VisBaseEntity_cl::InitFunction();
	SetObjectKey(NULL);

	Clear();
	CommonInit();
}

// called by the engine when entity is destroyed
void Camera2D::DeInitFunction()
{
	Toolset2dManager::Instance()->SetCamera(NULL);

	VisBaseEntity_cl::DeInitFunction();
	CommonDeInit();
}

// called by our InitFunction and our de-serialization code
void Camera2D::CommonInit()
{
}

void Camera2D::CommonDeInit()
{ 
	Clear();
}

void Camera2D::Clear()
{
	m_transform = hkvVec4(1.f, 1.f, 0.f, 0.f);
}

const hkvVec4 *Camera2D::GetTransform() const
{
	return &m_transform;
}

void Camera2D::Serialize(VArchive &ar)
{
	VisBaseEntity_cl::Serialize(ar);

	if (ar.IsLoading())
	{
		Clear();

		char spriteVersion;
		ar >> spriteVersion;
		VASSERT(spriteVersion <= CURRENT_CAMERA_2D_VERSION);
	} 
	else
	{
		ar << (char)CURRENT_CAMERA_2D_VERSION;
	}
}

void Camera2D::OnSerialized(VArchive &ar)
{
	VisBaseEntity_cl::OnSerialized(ar);

	CommonInit();
}

void Camera2D::OnVariableValueChanged(VisVariable_cl *pVar, const char *value)
{
}

void Camera2D::ThinkFunction()
{

}

START_VAR_TABLE(Camera2D, VisBaseEntity_cl, "Camera2D", 0, "")
END_VAR_TABLE