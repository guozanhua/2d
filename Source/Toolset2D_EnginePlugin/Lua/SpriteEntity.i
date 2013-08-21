%nodefaultctor Sprite;
%nodefaultdtor Sprite;

//custom headers for generated source file
%module Toolset2D
%{
  #include "SpriteEntity.hpp"
%}

class Sprite : public VisBaseEntity_cl
{
public:
  bool SetState(const char *state);
  void SetFramePercent(float percent);
  void Pause();
  void Play();

  %extend{
    VSWIG_CREATE_CAST(Sprite)
  }
};