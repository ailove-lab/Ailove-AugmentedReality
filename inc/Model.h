#ifndef _MODEL_
#define _MODEL_

#include <string>
#include <OgreVector4.h>
#include <OgreQuaternion.h>

//-------------------------------------------------------------------------------------
class XmlSerializer;

/** Store information about model*/
class Model {
public:
    Model(const std::string& name);

    /** Default constructor is required to support serialization of collections.*/
    Model();

    /** Return name of node.*/
    const std::string& getName() const              { return name;}

    /** If model is loaded more then once then node name is different from model name*/
    const std::string& getNodeName() const          { return nodeName;}
    void setNodeName(const std::string& name)       { nodeName = name;}

    /** Return associated pattern. It can be -1 for automatic associations */
    int getPatternId() const                        { return patternId;}
    int getGroupId() const                          { return groupId;}
    
    float getScale() const                          { return scale;}
    void setScale(float s)                          { scale = s;}
    
    void setAnimationState(const std::string name)  { animationState = name; }
    const std::string& getAnimationState() const    { return animationState;}
    
    const Ogre::Vector3 getPosition() const         { return position; }
    
    const Ogre::Quaternion getOrientation() const;
    void setOrientation(const Ogre::Quaternion& q);

    void serialize(XmlSerializer& sr);

    const bool isScene() const                      { return isSceneType;}
protected:
    void init();

    void initType();

private:

    bool isSceneType;
    std::string name;
    std::string nodeName;
    std::string animationState;
    int patternId;
    int groupId;

    float scale;
    Ogre::Vector3 position;
    Ogre::Vector4 orientation;
};

//-------------------------------------------------------------------------------------

#endif

