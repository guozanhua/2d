#include "Toolset2D_ManagedPCH.h"

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include "Toolset2D_EnginePlugin/HavokSetup.cxx"

#using <mscorlib.dll>
using namespace System;

VIMPORT IVisPlugin_cl* GetEnginePlugin_Toolset2D_EnginePlugin();

namespace Toolset2D_Managed
{
	public ref class ManagedModule
	{
	public:
		static void InitManagedModule()
		{
			// Init the entity plugin directly since we link statically to it
			GetEnginePlugin_Toolset2D_EnginePlugin()->InitEnginePlugin();
		}
		
		static void DeInitManagedModule()
		{
			// Deinit the entity plugin directly since we link statically to it
			GetEnginePlugin_Toolset2D_EnginePlugin()->DeInitEnginePlugin();
		}
	};
}