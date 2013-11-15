%nodefaultctor Camera2D;
%nodefaultdtor Camera2D;

// custom headers for generated source file
%module Toolset2D
%{
  #include "Camera2dEntity.hpp"
%}

class Camera2D : public VisBaseEntity_cl
{
public:
	%extend{
		VSWIG_CREATE_CAST(Camera2D)
	}
};