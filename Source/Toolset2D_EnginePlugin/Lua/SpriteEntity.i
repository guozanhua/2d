%nodefaultctor Sprite;
%nodefaultdtor Sprite;

// custom headers for generated source file
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

  // Converts a pixel coordinate on the sprite to the new scaled/rotated/transformed
  // position, which is useful for attachment points
  hkvVec3 GetPoint(float x, float y);

  void SetCenterPosition(hkvVec3 position);

  %extend{
    VSWIG_CREATE_CAST(Sprite)
  }
};