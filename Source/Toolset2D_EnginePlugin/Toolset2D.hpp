//=======
//
// Author: Joel Van Eenwyk
// Purpose: Global include that *must* be included by everyone using the plugin
//
//=======

#ifndef TOOLSET2D_H_INCLUDED
#define TOOLSET2D_H_INCLUDED

// Uses Havok Physics to simulate 2D physics. The benefit here is that you'll be able to
// mix 2D and 3D physics together. It can be toggled off, however, if your game doesn't require
// it since it does increase the size of the final binary.
#if defined(WIN32)
#define USE_HAVOK_PHYSICS_2D	1
#else
#define USE_HAVOK_PHYSICS_2D	0
#endif

// #todo : not yet implemented
#define USE_BOX_2D				0

#endif // TOOLSET2D_H_INCLUDED
