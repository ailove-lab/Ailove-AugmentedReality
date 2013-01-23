#include "stdafx.h"
#include "Utils.h"
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreTimer.h>
// #include <BulletCollision\CollisionShapes\btTriangleMesh.h>
#include "ARException.h"

#include "GraphicsEngine.h"

using namespace Ogre;

boost::shared_ptr<Ogre::Timer> Utils::timer = boost::shared_ptr<Ogre::Timer>(new Ogre::Timer());
int Utils::cloneIdx = 0;
//-------------------------------------------------------------------------------------

Utils::Utils(void)
{
}

//-------------------------------------------------------------------------------------

Utils::~Utils(void)
{
}

//-------------------------------------------------------------------------------------
/*
void Utils::getAllChildren(Ogre::SceneNode* root, std::list<Ogre::SceneNode*>& nodes) {
    nodes.push_back(root);
    Ogre::SceneNode::ChildNodeIterator it = root->getChildIterator();
    while(it.hasMoreElements()) {
        getAllChildren(static_cast<Ogre::SceneNode*>(it.getNext()), nodes);
    }
}
*/
//-------------------------------------------------------------------------------------

AxisAlignedBox Utils::getBounds(SceneNode* node, int level) {
      Ogre::AxisAlignedBox aabb;
      aabb.setNull();
      
      Ogre::SceneNode::ChildNodeIterator it = node->getChildIterator();
      while(it.hasMoreElements())
      {
          Vector3 v0 = aabb.getSize();
          AxisAlignedBox box = getBounds(static_cast<Ogre::SceneNode*>(it.getNext()), level + 1);
          
          Vector3 v0box = box.getSize();
          aabb.merge(box);
      }

      node->_updateBounds();
     
      AxisAlignedBox b = node->_getWorldAABB();
      AxisAlignedBox entBox;
      unsigned short numAttachedObjects = node->numAttachedObjects();
      for(unsigned short i = 0; i < numAttachedObjects; ++i)
      {
         // Merge world bounds of each object 
          MovableObject* e = node->getAttachedObject(i);

          Entity* ent = dynamic_cast<Entity*>(e);
          //if (ent) {
          //    AxisAlignedBox b = ent->getMesh()->getBounds();
          //    aabb.merge(b);
          //}
          AxisAlignedBox b = node->getAttachedObject(i)->getBoundingBox();
          entBox.merge(b);
      }
      aabb.merge(entBox);
      Matrix4 m;
      m.makeTransform(node->getPosition(), node->getScale(), node->getOrientation());
      if (level != 0) {
      //  m.makeTransform(node->getPosition(), node->getScale(), node->getOrientation());
      } else {
        //  m = Matrix4::IDENTITY;
        //  m.setScale(node->getScale());
      }
      aabb.transform(m);
      return aabb;
}

//-------------------------------------------------------------------------------------
/*
boost::shared_ptr<btTriangleMesh> Utils::retreiveTriangleMesh(Ogre::SceneNode* node, std::vector<Vector3>& geom){
    std::list<SceneNode*> nodes;
    Utils::getAllChildren(node, nodes);
    typedef std::map<SceneNode*, Matrix4> NodeMatrices;
    std::map<SceneNode*, Matrix4> matrices;
    boost::shared_ptr<btTriangleMesh> triMesh(new btTriangleMesh());

    int c = 0;
    for(std::list<SceneNode*>::iterator it = nodes.begin(); nodes.end() != it; ++it) {
        SceneNode* child = (*it);
        //TODO MAKE IT MORE FLEXIBLE
        if (child->getName().find("SHADOW") != -1 && child->getName().find("CAR_WITHOUT_SHADOW") == -1) {
            continue;
        }

        const std::string& n = child->getName();
        if (n.find("CLD") == -1) {//n.size() - 3) {
        //    continue;
        }
        ++c;
        
        SceneNode::ObjectIterator jt = child->getAttachedObjectIterator();
        Vector3 pos = child->getPosition();
        Vector3 scale = child->getScale();
        Quaternion orient = child->getOrientation();
        Matrix4 m;
      //  scale.x = scale.y = scale.z = 0.7;
        if (c == 1) {
            pos = Vector3(0,0,0);
            orient = Quaternion();
        }
        m.makeTransform(pos, scale, orient);
       

        NodeMatrices::const_iterator mt = matrices.find(child->getParentSceneNode());
        if (matrices.end() != mt) {
            m = (mt->second) * m;
        }
        if (c==1) {
         // m = Matrix4::IDENTITY;
        }
        matrices.insert(std::make_pair(child, m));
        while(jt.hasMoreElements()) {
            MovableObject* mo = jt.getNext();
            Entity* ent = dynamic_cast<Entity*>(mo);
            
            if (ent) {
                MeshPtr mesh = ent->getMesh();
                if (mesh.get()) {
                    int n = mesh->getNumSubMeshes();
                    for(int i = 0; i < n; ++i) {
                        retreiveTriangleMesh(triMesh, ent, i, m, geom);
                    }
                }
            }
        }
    }
    return triMesh;
}
*/
//-------------------------------------------------------------------------------------
/*
void Utils::retreiveTriangleMesh(boost::shared_ptr<btTriangleMesh>& triMesh, Entity* ent, int subMeshIdx,  const Ogre::Matrix4& transf, std::vector<Vector3>& geom) 

{
   // Get the mesh from the entity
   Ogre::MeshPtr myMesh = ent->getMesh();

   // Get the submesh and associated data
   Ogre::SubMesh* subMesh = myMesh->getSubMesh(subMeshIdx);

   Ogre::IndexData*  indexData = subMesh->indexData;
   Ogre::VertexData* vertexData = subMesh->useSharedVertices ? myMesh->sharedVertexData : subMesh->vertexData;

   // -------------------------------------------------------

   // Get the position element
   const Ogre::VertexElement* posElem = vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

   // Get a pointer to the vertex buffer
   Ogre::HardwareVertexBufferSharedPtr vBuffer = vertexData->vertexBufferBinding->getBuffer(posElem->getSource());

   // Get a pointer to the index buffer
   Ogre::HardwareIndexBufferSharedPtr iBuffer = indexData->indexBuffer;

   // -------------------------------------------------------

   // The vertices and indices used to create the triangle mesh
   std::vector<Ogre::Vector3> vertices;
   vertices.reserve(vertexData->vertexCount);

   std::vector<unsigned long> indices;
   indices.reserve(indexData->indexCount);

   // -------------------------------------------------------

   // Lock the Vertex Buffer (READ ONLY)
   unsigned char* vertex = static_cast<unsigned char*>(vBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
   float* pReal = NULL;

   for (size_t j = 0; j < vertexData->vertexCount; ++j, vertex += vBuffer->getVertexSize() ) {
      posElem->baseVertexPointerToElement(vertex, &pReal);
      Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
      pt = transf * pt;
      vertices.push_back(pt);
   }

   vBuffer->unlock();

   // -------------------------------------------------------

   bool use32bitindexes = (iBuffer->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

   // ----

   // Lock the Index Buffer (READ ONLY)
   unsigned long* pLong = static_cast<unsigned long*>(iBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
   unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);
    int indexstride = 0;
    int numFaces = 0;
    if ( subMesh->operationType == Ogre::RenderOperation::OT_TRIANGLE_LIST ){
        indexstride = (int) iBuffer->getIndexSize()*3;
        numFaces = (int) indexData->indexCount / 3;
    } else if ( subMesh->operationType == Ogre::RenderOperation::OT_TRIANGLE_STRIP ){
        indexstride = (int) iBuffer->getIndexSize();
        numFaces = (int) indexData->indexCount -2;
    }


   if (use32bitindexes)  {
      for (size_t k = 0; k < indexData->indexCount; ++k) {
         indices.push_back(pLong[k]);
      }
   } else {
      for (size_t k = 0; k < indexData->indexCount; ++k) {
         indices.push_back(static_cast<unsigned long>(pShort[k]) );
      }
   }

   iBuffer->unlock();

   // -------------------------------------------------------

   // We now have vertices and indices ready to go

   // Create the triangle mesh
   if (!triMesh.get()) {
       // possible bug if different submeshes uses dif flag (16/32 bit index)
        triMesh.reset(new btTriangleMesh(use32bitindexes));
   }
   btVector3 vert0, vert1, vert2;
   int i=0;

   // For every triangle
  // geom.reserve((int)indexData->indexCount);
   for (size_t y=0; y<indexData->indexCount/3; y++) {
      // Set each vertex
      vert0.setValue(vertices[indices[i]].x, vertices[indices[i]].y, vertices[indices[i]].z);
      vert1.setValue(vertices[indices[i+1]].x, vertices[indices[i+1]].y, vertices[indices[i+1]].z);
      vert2.setValue(vertices[indices[i+2]].x, vertices[indices[i+2]].y, vertices[indices[i+2]].z);

      btVector3 v1 = vert0-vert1;
      btVector3 v2 = vert0-vert2;
      btVector3 v3 = vert1-vert2;

      if (v1.length2() < 0.0025 || v2.length2() < 0.0025 || v2.length2() < 0.0025) {
        //  continue;
      }

      geom.push_back(Vector3(vert0.x(), vert0.y(), vert0.z()));
      geom.push_back(Vector3(vert1.x(), vert1.y(), vert1.z()));
      geom.push_back(Vector3(vert2.x(), vert2.y(), vert2.z()));

      // Add the triangle into the triangle mesh
      triMesh->addTriangle(vert0, vert2, vert1);

      // Increase index count
      i += 3;
   }

}
*/
//-------------------------------------------------------------------------------------
/*
std::string Utils::getCloneName(const std::string& name)  {
    const std::string postfix = "_CLONE_";
    int pos = name.find(postfix);
    if (pos != -1) {
        return name.substr(0, pos) + postfix + StringConverter::toString(cloneIdx++);
    } else {
        return name + postfix + StringConverter::toString(cloneIdx++);
    }
}
*/
//-------------------------------------------------------------------------------------
/*
Ogre::SceneNode* Utils::clone(SceneManager* manager, SceneNode* node) {
       
    SceneNode* copy = manager->createSceneNode(getCloneName(node->getName()));
    copy->setPosition(node->getPosition());
    copy->setScale(node->getScale());
    copy->setOrientation(node->getOrientation());
    SceneNode::ObjectIterator it = node->getAttachedObjectIterator();
    while (it.hasMoreElements()) {
        Entity* ent = dynamic_cast<Entity*>(it.getNext());
        if (ent) {
            Entity* copyEnt = ent->clone(getCloneName(ent->getName()));
            copy->attachObject(copyEnt);
        }
    }
   
    std::list<SceneNode*> nodes;
    Utils::getAllChildren(node, nodes);
    for(std::list<SceneNode*>::iterator it = nodes.begin(); nodes.end() != it; ++it) {
        if (node == *it) {
            continue;
        }
        SceneNode *child = clone(manager, *it);
        copy->addChild(child);
    }

    return copy;
}
*/
//-------------------------------------------------------------------------------------
/*
Ogre::SceneNode* Utils::findNode(GraphicsEngine* engine, const std::string& promtPrefix, const std::string& name) {
    try {
        return engine->getSceneManager()->getSceneNode(promtPrefix + name);
    } catch(...) {
    }

    int count = engine->getModelsCount();
    for(int i = 0; i < count; ++i) {
        const std::string modelName = engine->getModel(i).getName();
        if (modelName != promtPrefix) {
            try {
                return engine->getSceneManager()->getSceneNode(modelName + name);
            } catch (...) {
            }
        }
    }
    
    //TODO: make smart error generator
    std::string err = std::string("Не найден объект: ") + promtPrefix + " " + name;
    Ogre::LogManager::getSingleton().logMessage(err);
    throw ARException(err);
}
*/
//-------------------------------------------------------------------------------------
/*
void Utils::destroyNode(SceneNode* node)  {
   
    // Destroy all the attached objects
   SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

   while ( itObject.hasMoreElements() )
   {
      MovableObject* pObject = static_cast<MovableObject*>(itObject.getNext());
      node->getCreator()->destroyMovableObject( pObject );
   }

   // Recurse to child SceneNodes
   SceneNode::ChildNodeIterator itChild = node->getChildIterator();

   while ( itChild.hasMoreElements() )
   {
      SceneNode* pChildNode = static_cast<SceneNode*>(itChild.getNext());
      destroyNode( pChildNode );
   }

   SceneManager* manager = node->getCreator();
   node->removeAndDestroyAllChildren();
   manager->destroySceneNode(node);
}*/