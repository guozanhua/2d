#include <SpriteShapeEnginePluginPCH.h>
#include <VNode.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

static int g_bModuleRegCount = 0;


/////////////////////////////////////////////////////////////////////////////
// VNodeCollection_cl class
/////////////////////////////////////////////////////////////////////////////

// implement the serialisation (SerializeX function)
V_IMPLEMENT_SERIALX( VNodeCollection_cl);
void VNodeCollection_cl::SerializeX( VArchive &ar )
{
  int i;
  if (ar.IsLoading())
  {
    int iCount;
    ar >> iCount;
    VASSERT(iCount>=0);
    Clear();
    EnsureCapacity(iCount);
    for (i=0;i<iCount;i++)
    {
      VNode_cl *pNode = (VNode_cl *)ar.ReadObject(NULL);
      VASSERT(pNode);
      Add(pNode);
    }

  } else
  {
    int iCount = Count();
    ar << iCount;
    for (i=0;i<iCount;i++)
      ar << GetAt(i);
  }
}


// render all nodes in the collection
void VNodeCollection_cl::DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections) const
{
  for (int i=0;i<Count();i++)
    GetAt(i)->DebugRender(pRenderer,fSize,iColor,bRenderConnections);
}

 
void VNodeCollection_cl::DisposeAll()
{
  for (int i=0;i<Count();i++)
    GetAt(i)->DisconnectAll();
  Clear();
}



/////////////////////////////////////////////////////////////////////////////
// VNode_cl class
/////////////////////////////////////////////////////////////////////////////


// constructor: add to global collection
VNode_cl::VNode_cl()
{
  VNodeMananger_cl::GlobalManager().Instances().Add(this);
}

// doesn't do anything...
VNode_cl::~VNode_cl()
{
  // must not be in the list anymore since the list is reference counted. Use VNode_cl::Dispose() to remove an object
  // but the assertion does not make sense because it also asserts while the collection is being cleared
  // VASSERT(!VNodeMananger_cl::GlobalManager().Instances().Contains(this)); 
}

// remove from global collection. Might delete the node
void VNode_cl::Dispose()
{
  VNodeMananger_cl::GlobalManager().Instances().Remove(this);
}

// render the node (and optionally the connections)
void VNode_cl::DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections) const
{
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  hkvAlignedBBox bbox;
  hkvVec3 vRad(fSize,fSize,fSize);
  bbox.m_vMin = m_vPos-vRad;
  bbox.m_vMax = m_vPos+vRad;
  pRenderer->RenderAABox(bbox,iColor,state);

  if (bRenderConnections)
  {
    for (int i=0;i<m_Connections.Count();i++)
      pRenderer->DrawLine(m_vPos,m_Connections.GetAt(i)->m_vPos,V_RGBA_GREEN,3.f);
  }
}


// implement the Serialize function. Perform a full serialisation
V_IMPLEMENT_SERIAL( VNode_cl, VisTypedEngineObject_cl, 0, &g_NodeModule );
void VNode_cl::Serialize( VArchive &ar )
{
  VisTypedEngineObject_cl::Serialize(ar); // call base class
  if (ar.IsLoading())
  {
    m_vPos.SerializeAsVec3 (ar);
    ar >> m_Connections;
  } else
  {
    m_vPos.SerializeAsVec3 (ar);
    ar << m_Connections;
  }
}


/////////////////////////////////////////////////////////////////////////////
// VNodeMananger_cl class
/////////////////////////////////////////////////////////////////////////////

VNodeMananger_cl VNodeMananger_cl::g_VNodeManager;

void VNodeMananger_cl::OneTimeInit()
{
  Vision::Callbacks.OnWorldDeInit += this;
}

void VNodeMananger_cl::OneTimeDeInit()
{
  Vision::Callbacks.OnWorldDeInit -= this;
}

VNodeMananger_cl &VNodeMananger_cl::GlobalManager()
{
  return g_VNodeManager;
}


void VNodeMananger_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    Instances().DisposeAll(); // purge the instances
  }
}


