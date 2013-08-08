#include "SpriteGamePCH.h"

#include "VisSampleApp.hpp"

#include <Vision/Runtime/Base/VBase.hpp>

VISION_CALLBACK void VisionSampleAppAfterLoadingFunction();
VISION_CALLBACK bool VisionSampleAppRunFunction();

VISION_RUN
{
  //check if the scene loading is finished
  VisSampleApp::ApplicationState appState = static_cast<VisSampleApp*>(Vision::GetApplication())->GetApplicationState();
  if (appState == VisSampleApp::AS_LOADING)
  {
    return true;
  }
  else if (appState == VisSampleApp::AS_LOADING_ERROR)
  {
    return false;
  }
  else if (appState == VisSampleApp::AS_AFTER_LOADING)
  {
    VisionSampleAppAfterLoadingFunction();
  }  
  return VisionSampleAppRunFunction();
}

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
