#ifndef SpriteGameAPPLICAION_HPP_INCLUDED
#define SpriteGameAPPLICAION_HPP_INCLUDED

#include <HUD.hpp>

#include "Entities/SpriteEntity.hpp"

#ifdef WIN32
class IVRemoteInput;
#endif

class SpriteGameManager : public IVisCallbackHandler_cl
{
public:
  VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  // called when plugin is loaded/unloaded
  void OneTimeInit();
  void OneTimeDeInit();

  // switch to play-the-game mode. When not in vForge, this is default
  void SetPlayTheGame(bool bStatus);

  // globally store the player entity. Important for the HUD
  //void SetPlayer(Player *pEntity) {m_pPlayerEntity=pEntity;}
  //Player *GetPlayer() {return m_pPlayerEntity;}

  // remaining items - increased when a health pack is created, decreased when picked
  inline void IncRemainingItems() {m_iRemainingItems++;}
  inline int GetRemainingItems() const {return m_iRemainingItems;}
  inline void ResetRemainingItems() {m_iRemainingItems=0;}
  void DecRemainingItems();

  // access one global instance of the fame manager
  static SpriteGameManager& GlobalManager() {return g_GameManager;}

private:
  bool m_bPlayingTheGame;
  HUDGUIContextPtr m_spHUD;
  //Player *m_pPlayerEntity;
  Sprite *m_pPlayerSprite;
  int m_iRemainingItems;
#ifdef WIN32
  IVRemoteInput *m_pRemoteInput;
#endif

  static SpriteGameManager g_GameManager;
};


#endif



