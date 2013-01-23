#include "CarController.h"
#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "Utils.h"

using namespace Ogre;

const float CarController::DELTA_ANGLE = Math::DegreesToRadians(100);
const float CarController::MAX_ANGLE = Math::DegreesToRadians(45);

const float CarController::DELTA_SPEED = 2;
const float CarController::MAX_SPEED = 4;

//-------------------------------------------------------------------------------------

#define getControlPoint(x) ControlPoint<Type>(x, #x)

CarController::CarController(void)
    : wheelAngle(0), carSpeed(0), carOrientation(0)
{
    
    controlPoints.push_back(getControlPoint(FL_WHEEL));
    controlPoints.push_back(getControlPoint(FR_WHEEL));
    controlPoints.push_back(getControlPoint(BL_WHEEL));
    controlPoints.push_back(getControlPoint(BR_WHEEL));
    controlPoints.push_back(getControlPoint(CAR_BODY));

}

//-------------------------------------------------------------------------------------

CarController::~CarController(void)
{
}

//-------------------------------------------------------------------------------------

void CarController::load()

{
}

void CarController::reset() 

{
    if (!isInitialized()) {
        return;
    }

    wheelAngle = 0;
    carSpeed = 0;
    carOrientation = 0;
    controlPoints[CAR_BODY].node->setPosition(controlPoints[CAR_BODY].pos);
}

//-------------------------------------------------------------------------------------
void CarController::update(float dt)

{
    if (!isInitialized()) {
        return;
    }

    if (getKeyboard()->isKeyDown(OIS::KC_LEFT)) {
        wheelAngle += DELTA_ANGLE * dt;
    }

    if (getKeyboard()->isKeyDown(OIS::KC_RIGHT)) {
        wheelAngle -= DELTA_ANGLE * dt;
    }

    float factor = 0;
    if (getKeyboard()->isKeyDown(OIS::KC_UP)) {
        factor = carSpeed >= 0 ? 1:4;
    }

    if (getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
        factor = carSpeed <= 0 ? -1:-4;
    }
    if (0 == factor) {
        factor = carSpeed > 0 ? -0.5f : 0.5f;
        carSpeed += factor * DELTA_SPEED * dt;
        if (carSpeed * factor > 0) {
            carSpeed = 0;
        }
    } else {
        carSpeed += factor * DELTA_SPEED * dt;
    }

    

    if (getKeyboard()->isKeyDown(OIS::KC_SPACE)) {
        reset();
    }

    wheelAngle = Math::Clamp(wheelAngle, -MAX_ANGLE, MAX_ANGLE);
    carSpeed = Math::Clamp(carSpeed, -MAX_SPEED, MAX_SPEED);

    Quaternion rot(Radian(wheelAngle), Vector3(0, 1, 0));
    ControlPoint<Type>& flWheel =  controlPoints[FL_WHEEL];
    flWheel.node->setOrientation(rot * Quaternion(Radian(flWheel.value), Vector3(-1, 0, 0)));

    ControlPoint<Type>& frWheel =  controlPoints[FR_WHEEL];
    frWheel.node->setOrientation(rot * Quaternion(Radian(frWheel.value), Vector3(-1, 0, 0)));

    ControlPoint<Type>& blWheel =  controlPoints[BL_WHEEL];
    blWheel.node->setOrientation(Quaternion(Radian(blWheel.value), Vector3(-1, 0, 0)));

    ControlPoint<Type>& brWheel =  controlPoints[BR_WHEEL];
    brWheel.node->setOrientation(Quaternion(Radian(brWheel.value), Vector3(-1, 0, 0)));
    
    float n = 10;
    float deltaT = dt / n;
    float sinA =sin(wheelAngle);
    float cosA= cos(wheelAngle);
    float L = (frontChassis - backChassis).length();

    float length[4] = {0, 0, 0, 0};
    Vector3 pivot(0, 0, 0);// = backChassis;
    float angle = 0;
    float rotPoint = 0.5f;//wheelAngle > 0 ? 0 : 1;
    for (int i = 0; i < n; ++i) {
        float dx = carSpeed * deltaT;
        float b = dx * sinA / L;
        pivot.z -= (dx*cos(angle)*cosA);
        pivot.x -= (dx*sin(angle)*cosA);
        angle += b;
        length[FL_WHEEL] += dx - chassisWidth*b * (1-rotPoint)*cosA;
        length[FR_WHEEL] += dx + chassisWidth*b * rotPoint*cosA;
        length[BL_WHEEL] += dx*cosA - chassisWidth * b * (1-rotPoint);
        length[BR_WHEEL] += dx*cosA + chassisWidth * b * (rotPoint);
    }


    Vector3 origin = controlPoints[BR_WHEEL].pos + (controlPoints[BL_WHEEL].pos - controlPoints[BR_WHEEL].pos)*rotPoint;
    Vector3 delta = controlPoints[CAR_BODY].pos - origin;

    Vector3 pos = controlPoints[CAR_BODY].node->getPosition() -  Quaternion(Radian((carOrientation)), Vector3(0, 1, 0)) * delta 
        + Quaternion(Radian((angle+carOrientation)), Vector3(0, 1, 0))*delta
        + Quaternion(Radian((carOrientation)), Vector3(0, 1, 0))*pivot;
    carOrientation += angle;
    while(carOrientation > 2*Math::PI) {
        carOrientation -= 2*Math::PI;
    }

    while(carOrientation < 0) {
        carOrientation += 2*Math::PI;
    }

    Quaternion rotCar(Radian(carOrientation), Vector3(0, 1, 0));
    controlPoints[CAR_BODY].node->setPosition(pos);
    controlPoints[CAR_BODY].node->setOrientation(rotCar);

    for(Type t = FL_WHEEL; t <= BR_WHEEL; t = (Type)(t + 1)) {
        controlPoints[t].value += length[t] / wheelRadius;
        while(controlPoints[t].value > 2*Math::PI) {
            controlPoints[t].value -= 2*Math::PI;
        }

        while(controlPoints[t].value < 0) {
            controlPoints[t].value += 2*Math::PI;
        }
    }
}

//-------------------------------------------------------------------------------------

void CarController::init(Ogre::SceneManager* mgr, Ogre::SceneNode* rootNode, const std::string& prefix, OIS::Keyboard* keyboard)
{
    ModelController::init(mgr, rootNode, prefix, keyboard);

    for(std::vector<ControlPoint<Type> >::iterator it = controlPoints.begin(); controlPoints.end() != it; ++it) {
        try {
            it->node = mgr->getSceneNode(prefix + it->nodeName);
            it->bBox = Utils::getBounds(it->node);
            it->pos = it->bBox.getCenter();
            if (Type::CAR_BODY != it->type) {
                it->value = it->node->getOrientation().getRoll().valueRadians();
            }

        } catch (...) {
            return;
        }
    }

    frontChassis = (controlPoints[FL_WHEEL].bBox.getCenter() + controlPoints[FR_WHEEL].bBox.getCenter())/2;
    backChassis = (controlPoints[BL_WHEEL].bBox.getCenter() + controlPoints[BR_WHEEL].bBox.getCenter())/2;
    chassisWidth = (controlPoints[FL_WHEEL].bBox.getCenter() - controlPoints[FR_WHEEL].bBox.getCenter()).length();
    carCenter = controlPoints[CAR_BODY].node->getPosition();

    controlPoints[CAR_BODY].pos =  controlPoints[CAR_BODY].node->getPosition();
    wheelRadius = std::max(controlPoints[FL_WHEEL].bBox.getSize().x, controlPoints[FL_WHEEL].bBox.getSize().y)/2;
    
    setInitialized(true);
}

