#include "SpriteGamePCH.h"

#include "SpriteGameApplication.hpp"
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

#ifdef WIN32
#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/IVRemoteInput.hpp>
#endif

// one global static instance
MyGameManager MyGameManager::g_GameManager;

void MyGameManager::OneTimeInit()
{
  m_bPlayingTheGame = false;
  m_iRemainingItems = 0;

  // the game manager should listen to the following callbacks:
  Vision::Callbacks.OnEditorModeChanged += this;
  Vision::Callbacks.OnBeforeSceneLoaded += this;
  Vision::Callbacks.OnAfterSceneLoaded += this;
  Vision::Callbacks.OnWorldDeInit += this;

  //load the remote input plugin
#if defined(WIN32) && !defined(_VISION_WINRT)
  Vision::Callbacks.OnUpdateSceneFinished += this;
  m_pRemoteInput = NULL;
  VISION_PLUGIN_ENSURE_LOADED(vRemoteInput);
  m_pRemoteInput = (IVRemoteInput*)Vision::Plugins.GetRegisteredPlugin("vRemoteInput");
  //if(Vision::Editor.IsInEditor() && m_pRemoteInput)
    //m_pRemoteInput->SetResizeOnConnect(false);
#endif
}

void MyGameManager::OneTimeDeInit()
{
  Vision::Callbacks.OnEditorModeChanged -= this;
  Vision::Callbacks.OnBeforeSceneLoaded -= this;
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
#if defined(WIN32) && !defined(_VISION_WINRT)
  Vision::Callbacks.OnUpdateSceneFinished -= this;
#endif
}

void MyGameManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{

  if (pData->m_pSender==&Vision::Callbacks.OnEditorModeChanged)
  {
    // when vForge switches back from EDITORMODE_PLAYING_IN_GAME, turn off our play the game mode
    if (((VisEditorModeChangedDataObject_cl *)pData)->m_eNewMode != VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME)
      SetPlayTheGame(false);
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnBeforeSceneLoaded)
  {
    ResetRemainingItems();
    // specifically in the Citymap we need accurate assignment because of nested Viszones
    Vision::World.SetUseGeometryForVisibilityClassification(true);
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneLoaded)
  {
    if (Vision::Editor.IsPlayingTheGame()) // init play-the-game only in this vForge mode (or outside vForge)
      SetPlayTheGame(true);
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit) // this is important when running outside vForge
  {
    SetPlayTheGame(false);
    return;
  }
  if(pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
#ifdef WIN32
    if(m_pRemoteInput != NULL)
    {
      /// uncomment the following 2 lines to enable the debug drawing of the vRemoteInput plugin
      //m_pRemoteInput->DebugDrawTouchAreas(VColorRef(255,255,255));
      //m_pRemoteInput->DebugDrawTouchPoints(VColorRef(255,0,0));

      //if(GetPlayer() != NULL)
      {
        //PlayerData &playerData = GetPlayer()->GetPlayerData();
       // m_pRemoteInput->UpdateVariable("health",playerData.GetHealth());
     } 

      m_pRemoteInput->UpdateVariable("remaining",GetRemainingItems());
    }
#endif
  }
}


// decrease the number of remaining items. Called in the DeInitFunction of HealthPack entity
void MyGameManager::DecRemainingItems()
{
  if (m_bPlayingTheGame)
  {
    m_iRemainingItems--;
    VASSERT(m_iRemainingItems>=0);
    //if (m_pPlayerEntity && m_iRemainingItems==0) // player wins -> stop player interaction
    //  m_pPlayerEntity->SetThinkFunctionStatus(FALSE);
  }
}


// switch to play-the-game mode. Do some initialization such as HUD display
void MyGameManager::SetPlayTheGame(bool bStatus)
{
  if (m_bPlayingTheGame==bStatus)
    return;

  m_bPlayingTheGame = bStatus;

  if (m_bPlayingTheGame)
  {
    m_spHUD = new HUDGUIContext(NULL);
    m_spHUD->SetActivate(true);

#if defined(WIN32) && !defined(_VISION_WINRT)
    if(m_pRemoteInput)
    {
      m_pRemoteInput->InitEmulatedDevices();

      ///uncomment to disable touch input smoothing
      //m_pRemoteInput->SetSmoothTouchInput(false);

      //m_pRemoteInput->StartServer("SpriteGame\\RemoteGui");
      m_pRemoteInput->AddVariable("health",100.0f);
      m_pRemoteInput->AddVariable("remaining",6);
    }

    //if(m_pPlayerEntity)
    //  m_pPlayerEntity->InitTouchInput();
#endif
  } else
  {
    // deactivate the HUD
    if (m_spHUD)
    {
      m_spHUD->SetActivate(false);
      m_spHUD->GetManager()->Contexts().SafeRemove(m_spHUD); // unattach this context
      m_spHUD = NULL;
    }
#if defined(WIN32) && !defined(_VISION_WINRT)
    //if(m_pPlayerEntity)
    //  m_pPlayerEntity->DeinitTouchInput();

    if(m_pRemoteInput)
    {
      m_pRemoteInput->StopServer();
      m_pRemoteInput->RemoveVariable("health");
      m_pRemoteInput->RemoveVariable("remaining");
    }
#endif

    // clean up all pending particle effect instances
    VisParticleGroupManager_cl::GlobalManager().Instances().Purge();
  }

}

