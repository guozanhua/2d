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

  float GetWidth();
  float GetHeight();

  // Converts a pixel coordinate on the sprite to the new scaled/rotated/transformed
  // position, which is useful for attachment points
  hkvVec3 GetPoint(float x, float y, float z = 0.0f);

  void SetCenterPosition(hkvVec3 position);
  hkvVec3 GetCenterPosition();

  void SetPlayOnce(bool enabled);
  
  void SetCollision(bool enabled);

  %extend{
    VSWIG_CREATE_CAST(Sprite)
  }
};