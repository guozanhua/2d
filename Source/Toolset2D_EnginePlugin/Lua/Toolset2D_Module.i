// ===========================================================
// Sprite Entity Lua Module - SWIG Interface
// ===========================================================
// This interface file generates Toolset2D_Module_wrapper.cpp
// and is represented in Lua as the "Toolset 2D" module.

//custom headers for generated source file
%module Toolset2dModule
%{
	#include "Toolset2D_EnginePluginPCH.h"
%}

//get access to all vision specific things...
%include <windows.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

// dependencies of ObjectComponent:
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VColor.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTypedObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiTypedEngineObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObjectComponent.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObject3D.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiBaseEntity.i>

%include <SpriteEntity.i>
%include <Camera2dEntity.i>
%include <Toolset2dManager.i>