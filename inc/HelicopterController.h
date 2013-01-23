#ifndef HELICOPTER_CONTROLLER
#define HELICOPTER_CONTROLLER

#include "ModelController.h"

#include <OgreAxisAlignedBox.h>

//-------------------------------------------------------------------------------------

class HelicopterController :  public ModelController {
public:
    HelicopterController();
    virtual ~HelicopterController(void);

    virtual void load();
    virtual void update(float dt);
    virtual void init(Ogre::SceneManager* mgr, Ogre::SceneNode* rootNode, const std::string& prefix, OIS::Keyboard* keyboard);
    virtual void reset();
protected:

    enum Type {
        HELICOPTER,
        HELICOPTER_BODY,
        HELICOPTER_SHADOWS,
        SIZE
    };

private:
    std::vector<ControlPoint<Type> > controlPoints;
    Ogre::Quaternion current;
    Ogre::Quaternion target;

    float pitch;
    float roll;

    Ogre::Vector3 speed;

    static const float DELTA_ANGLE;

    static const float MAX_PITCH;
    static const float MAX_ROLL;
    static const float MAX_SPEED;
    static const float MIN_RADIUS;
};

//----------------------------------------------l---------------------------------------

#endif

