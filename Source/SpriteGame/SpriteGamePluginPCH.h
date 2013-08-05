// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM		: ALL
// PRODUCT		: PHYSICS_2012+VISION
// VISIBILITY		: PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__EDITORPLUGIN__INCLUDED_)
#define AFX_STDAFX_H__EDITORPLUGIN__INCLUDED_

#if defined( _MSC_VER) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#if defined(WIN32)  || defined(_VISION_XENON) 
  #define NOMINMAX //Required by Novodex
#endif

#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Common/VisSampleApp.hpp>

//Physics module
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__EDITORPLUGIN__INCLUDED_)

