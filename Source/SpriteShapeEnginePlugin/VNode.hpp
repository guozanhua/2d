#ifndef VNODE_HPP_INCLUDED
#define VNODE_HPP_INCLUDED

#include "SpriteShapeEnginePluginPCH.h"

#include <Vision/Runtime/Engine/System/VisApiCallbacks.hpp>

class VNodeMananger_cl;
class VNode_cl;
typedef VSmartPtr<VNode_cl> VNodePtr;

// 
// *** VNodeCollection_cl ***
//
// Synopsis:
//    - Collection of VNode_cl objects
//
class VNodeCollection_cl : public VRefCountedCollection<VNode_cl>
{
public:
	// debug rendering (all nodes in the collection)
	SPRITE_IMPEXP void DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections=false) const;

	// clears the collection and also clears the link collection of each (to avoid dead-locks) 
	SPRITE_IMPEXP void DisposeAll();

	//serialisation:
	V_DECLARE_SERIALX( VNodeCollection_cl, SPRITE_IMPEXP );
	SPRITE_IMPEXP void SerializeX( VArchive &ar );
};



// 
// *** VNode_cl ***
//
// Synopsis:
//    - Sample class for a single point that can be linked to other instances of VNode_cl
//
// Description:
//    - Sample class for a single point that can be linked to other instances of VNode_cl.
//    - This is an example of how to make a grid of connected nodes that can for instance be used for AI operations.
//    - This class is written in native C++ code and provided through a vPlugin to the engine.
//
class VNode_cl : public VRefCounter, public VisTypedEngineObject_cl
{
public:

  // Constructor :  Adds this node to the collection in the global node manager VNodeMananger_cl::GlobalManager()
  SPRITE_IMPEXP VNode_cl();

  // Destructor
  virtual ~VNode_cl();

  // Dispose function: Removes the node from the global list, but does not destroy it (this is up to the refcounter)
  SPRITE_IMPEXP void Dispose();

  // set/get position of the node in world space
  inline void SetPosition(const hkvVec3 &vPos) {m_vPos=vPos;}
  inline void SetPosition(float x,float y,float z) {m_vPos.set(x,y,z);}
  inline const hkvVec3& GetPosition() const {return m_vPos;}

  // connections to other node objects
  SPRITE_IMPEXP bool IsConnected(const VNode_cl* pPoint) {return m_Connections.Contains(pPoint);}
  SPRITE_IMPEXP void ConnectPoint(VNode_cl* pPoint)      {m_Connections.Add(pPoint);}
  SPRITE_IMPEXP void DisconnectPoint(VNode_cl* pPoint)   {m_Connections.Remove(pPoint);}
  SPRITE_IMPEXP void DisconnectAll()                     {m_Connections.Clear();}

  // render the shape in editor mode (or for debugging)
  SPRITE_IMPEXP void DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections=false) const;

  // serialisation:
  V_DECLARE_SERIAL_DLLEXP( VNode_cl,  SPRITE_IMPEXP );
  SPRITE_IMPEXP virtual void Serialize( VArchive &ar );

private:
  // per-node data:
  hkvVec3 m_vPos; // position in world space
  VNodeCollection_cl m_Connections; // collection of linked children
};


// 
// *** VNodeMananger_cl ***
//
// Synopsis:
//    - Manages a collection of nodes
//
// Description:
//    - An instance of VNodeMananger_cl has a collection of node instances and additionally
//      functionality to handle them. 
//    - For instance it purges the instances when a new scene is loaded.
//
class VNodeMananger_cl : public IVisCallbackHandler_cl
{
public:
  VNodeMananger_cl() {}
  virtual ~VNodeMananger_cl() {m_Instances.Clear();}

  SPRITE_IMPEXP void OneTimeInit();
  SPRITE_IMPEXP void OneTimeDeInit();

  // access the collection of node instances
  SPRITE_IMPEXP VNodeCollection_cl& Instances() {return m_Instances;}

  // access the global node manager instance
  SPRITE_IMPEXP static VNodeMananger_cl& GlobalManager();

protected:
  VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  VNodeCollection_cl m_Instances;
  static VNodeMananger_cl g_VNodeManager;
};


#endif
