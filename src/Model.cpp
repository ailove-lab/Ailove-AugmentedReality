#include <OgreMath.h>
#include "Model.h"
#include "XmlSerializer.h"

//-------------------------------------------------------------------------------------

Model::Model(const std::string& name)
{
    init();
    this->name = name;
}

//-------------------------------------------------------------------------------------

Model::Model() 
    
{
    init();
}

//-------------------------------------------------------------------------------------

void Model::init() 
{
    scale = 1.f;
    position = Ogre::Vector3::ZERO;
    orientation = Ogre::Vector4::ZERO;
    patternId = -1;
    groupId = -1;
    initType();
}

void Model::initType() 
{
    isSceneType = (name.find(".scene") == name.size() - 6);
}

//-------------------------------------------------------------------------------------

void Model::serialize(XmlSerializer& sr) 

{
    sr % member("name", name) 
       % member("patternId", patternId)
       % member("anim", animationState)
       % member("scale", scale)
       % member("groupId", groupId)
       % member("pos", XmlAttributeHelper<float>() % position.x % position.y % position.z)
       % member("orientation", XmlAttributeHelper<float>() % orientation.w % orientation.x % orientation.y % orientation.z);

    initType();

}

//-------------------------------------------------------------------------------------

const Ogre::Quaternion Model::getOrientation() const 
{
    return Ogre::Quaternion(Ogre::Radian(Ogre::Math::DegreesToRadians(orientation.w)), Ogre::Vector3(orientation.x,orientation.y,orientation.z));
}

//-------------------------------------------------------------------------------------

void Model::setOrientation(const Ogre::Quaternion& q)

{
/*    float s = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z);
    orientation.w = Ogre::Math::RadiansToDegrees(q.w);
    orientation.x = q.x/s;
    orientation.y = q.y/s;
    orientation.z = q.z/s;
    orientation.w = Ogre::Math::RadiansToDegrees(q.w/s);  */  
}

//-------------------------------------------------------------------------------------
