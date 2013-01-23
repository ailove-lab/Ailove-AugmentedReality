#include "HelicopterController.h"
#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "Utils.h"

using namespace Ogre;

const float HelicopterController::DELTA_ANGLE = Math::DegreesToRadians(100);
const float HelicopterController::MAX_PITCH = Math::DegreesToRadians(25);
const float HelicopterController::MAX_ROLL = Math::DegreesToRadians(45);

const float HelicopterController::MAX_SPEED = 15;
const float HelicopterController::MIN_RADIUS = 3;


//-------------------------------------------------------------------------------------

#define getControlPoint(x) ControlPoint<Type>(x, #x)

HelicopterController::HelicopterController(void)
{
    
    controlPoints.push_back(getControlPoint(HELICOPTER));
    controlPoints.push_back(getControlPoint(HELICOPTER_BODY));
    controlPoints.push_back(getControlPoint(HELICOPTER_SHADOWS));
    pitch = 0;
    roll = 0;
    speed = Vector3(0, 0, 0);
}

//-------------------------------------------------------------------------------------

HelicopterController::~HelicopterController(void)
{
}

//-------------------------------------------------------------------------------------

void HelicopterController::load()

{
}

//-------------------------------------------------------------------------------------

void HelicopterController::reset() 

{
    if (!isInitialized()) {
        return;
    }

    //speed = 0;
    speed = Vector3(0, 0, 0);
    pitch = 0;
    roll = 0;

    for(Type c = HELICOPTER; c < SIZE; c = Type(c + 1)) {
        controlPoints[c].node->setPosition(controlPoints[c].pos);
        controlPoints[c].node->setOrientation(controlPoints[c].orientation);
    }
}

//-------------------------------------------------------------------------------------

void calcSpeed(float& speed, float& accel, float force, float resistance, float brakingFactor, float maxValue, float dt) 
{
    accel = force + resistance;
    
    if (accel * speed < 0) {
         accel *= brakingFactor;
    } else if (fabs(force) < 0.2 * fabs(resistance)) {
        accel *= 3*brakingFactor;
    }

    speed += accel*dt;
    speed = Math::Clamp(speed, -maxValue, maxValue);
}

//-------------------------------------------------------------------------------------

void HelicopterController::update(float dt)

{
    if (!isInitialized()) {
        return;
    }
    
    float pitchFactor = 0;
    float rollFactor = 0;
    if (getKeyboard()->isKeyDown(OIS::KC_UP)) {
        pitchFactor = pitch >= 0 ? 1 : 4;
    }

    if (getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
        pitchFactor = pitch <= 0 ? -1 : -4;
    }

    if (getKeyboard()->isKeyDown(OIS::KC_RIGHT)) {
        rollFactor = roll >= 0 ? 1 : 4;
    }

    if (getKeyboard()->isKeyDown(OIS::KC_LEFT)) {
        rollFactor = roll <= 0 ? -1 : -4;
    }

    if (0 == pitchFactor) {
        pitchFactor = pitch > 0 ? -0.5f : 0.5f;
        pitch += pitchFactor * DELTA_ANGLE * dt;
        if (pitch * pitchFactor > 0) {
            pitch = 0;
        }
    } else {
        pitch += pitchFactor * DELTA_ANGLE * dt;
    }

    if (0 == rollFactor) {
        rollFactor = roll > 0 ? -0.5f : 0.5f;
        roll += rollFactor * DELTA_ANGLE * dt;
        if (roll * rollFactor > 0) {
            roll = 0;
        }
    } else {
        roll += rollFactor * DELTA_ANGLE * dt;
    }
    
    if (getKeyboard()->isKeyDown(OIS::KC_SPACE)) {
        reset();
    }

    SceneNode* node = controlPoints[HELICOPTER_BODY].node;

    pitch = Math::Clamp(pitch, -MAX_PITCH, MAX_PITCH);
    roll = Math::Clamp(roll, -MAX_ROLL, MAX_ROLL);

    Quaternion rot1(Radian(pitch), Vector3(1, 0, 0));
    Quaternion rot2(Radian(roll), Vector3(0, 0, 1));
    
    Quaternion q = node->getOrientation();
    q = Quaternion::Slerp(dt, q, rot2 * rot1, true);
    node->setOrientation(q);

    const float force = 5.f;

    Vector2 localSpeed(speed.z, speed.x);
    Vector2 accel(0,0);

    float fx = force *sin(q.getPitch().valueRadians());
    float rx = -force * sin(MAX_PITCH)/MAX_SPEED/2*localSpeed.x;
    calcSpeed(localSpeed.x, accel.x, fx, rx, 4, MAX_SPEED, dt);

    float fy = force *sin(-q.getRoll().valueRadians());
    float ry = - force * sin(MAX_ROLL)/MAX_SPEED/2*localSpeed.y;
    calcSpeed(localSpeed.y, accel.y, fy, ry, 4, MAX_SPEED, dt);

    float rad = accel.y != 0 ? 0.3*(localSpeed.x * localSpeed.x + localSpeed.y*localSpeed.y)/ accel.y : 1e10;
    if (fabs(rad) < MIN_RADIUS) {
        rad = accel.y > 0 ? MIN_RADIUS : -MIN_RADIUS;
    }

    float rot = localSpeed.x * dt / rad;
    speed.z = localSpeed.x;
    
    speed.x = localSpeed.y*exp(-localSpeed.x * localSpeed.x / MAX_SPEED/MAX_SPEED/4);// * (MAX_SPEED - fabs(localSpeed.x))/MAX_SPEED;

    Vector3 pos = controlPoints[HELICOPTER].node->getPosition();
    
    Quaternion qRot = controlPoints[HELICOPTER].node->getOrientation() * Quaternion(Radian(rot), Vector3(0, 1, 0));
    pos += qRot * speed*dt;

    controlPoints[HELICOPTER].node->setPosition(pos);
    controlPoints[HELICOPTER].node->setOrientation(qRot);

    Vector3 scale(cos(q.getRoll().valueRadians()), 1, cos(q.getPitch().valueRadians()));
    node = controlPoints[HELICOPTER_SHADOWS].node;
    Ogre::SceneNode::ChildNodeIterator it = node->getChildIterator();
    while(it.hasMoreElements()) {
        SceneNode *child = static_cast<Ogre::SceneNode*>(it.getNext());
        Vector3 s = scale;
        Vector3 s2(s);
        child->setScale(s2); 
    }

}

//-------------------------------------------------------------------------------------

void HelicopterController::init(Ogre::SceneManager* mgr, Ogre::SceneNode* rootNode, const std::string& prefix, OIS::Keyboard* keyboard)
{
    ModelController::init(mgr, rootNode, prefix, keyboard);

    for(std::vector<ControlPoint<Type>>::iterator it = controlPoints.begin(); controlPoints.end() != it; ++it) {
        try {
            it->node = mgr->getSceneNode(prefix + it->nodeName);
            it->pos = it->node->getPosition();
            it->orientation = it->node->getOrientation();
        } catch (...) {
            return;
        }
    }

    target = controlPoints[HELICOPTER_BODY].node->getOrientation();
    setInitialized(true);
}

