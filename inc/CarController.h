#ifndef CAR_CONTROLLER
#define CAR_CONTROLLER

#include "ModelController.h"

#include <OgreAxisAlignedBox.h>

//-------------------------------------------------------------------------------------

class CarController :  public ModelController {
public:
    CarController();
    virtual ~CarController(void);

    virtual void load();
    virtual void update(float dt);
    virtual void init(Ogre::SceneManager* mgr, Ogre::SceneNode* rootNode, const std::string& prefix, OIS::Keyboard* keyboard);
    virtual void reset();
protected:

    enum Type {
        FL_WHEEL,
        FR_WHEEL,
        BL_WHEEL,
        BR_WHEEL,
        CAR_BODY,
        SIZE
    };

private:
    std::vector<ControlPoint<Type> > controlPoints;
    float wheelAngle;

    Ogre::Vector3 frontChassis;
    Ogre::Vector3 backChassis;
    Ogre::Vector3 carCenter;
    float carOrientation;
    float carSpeed;
    float chassisWidth;
    float wheelRadius;
    

    static const float DELTA_ANGLE;
    static const float DELTA_SPEED;
    static const float MAX_ANGLE;
    static const float MAX_SPEED;
};

//----------------------------------------------l---------------------------------------

#endif

