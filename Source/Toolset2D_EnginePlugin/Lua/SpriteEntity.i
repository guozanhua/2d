#ifndef VLUA_APIDOC

%nodefaultctor Sprite;
%nodefaultdtor Sprite;

//custom headers for generated source file
%module FireLight
%{
  #include "SpriteEntity.hpp"
%}

class Sprite : public VisBaseEntity_cl
{
public:
  bool SetState(const char *state);
};

#endif