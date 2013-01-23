#ifndef IMODELCONTROLLER
#define IMODELCONTROLLER

#include <string>
#include <OISKeyboard.h>
#include <OgreAxisAlignedBox.h>

namespace Ogre{
    class SceneManager;
    class SceneNode;
}

namespace OIS {
    class Keyboard;
}
//-------------------------------------------------------------------------------------

template<class T>
struct ControlPoint {
    T type;
    Ogre::Vector3 pos;
    Ogre::Quaternion orientation;
    Ogre::AxisAlignedBox bBox;
    Ogre::SceneNode* node;
    std::string nodeName;
    float value;

    ControlPoint(typename T type, std::string name) {
        node = NULL;
        nodeName = name;
        value = NULL;
        this->type = type;
    }
};

class ModelController {
public:

    const Ogre::SceneManager* getSceneManager() const { return mSceneMgr;}

    virtual void init(Ogre::SceneManager* mgr, Ogre::SceneNode* rootNode, const std::string& prefix, OIS::Keyboard* keyboard) {
        mSceneMgr = mgr;
        this->rootNode = rootNode;
        this->prefix = prefix;
        this->keyboard = keyboard;
    }
    virtual void load() = 0;
    virtual void update(float dt) = 0;
    virtual void handleInputEvent(const OIS::KeyEvent &arg) {}
    virtual void reset() {}
    
    ModelController() : initialized(false) {}
    virtual ~ModelController(void) {}

    void setInitialized(bool v) { initialized = v;}
    bool isInitialized() { return initialized;}

    const Ogre::SceneNode* getRoot() const { return rootNode; }

protected:
    const std::string& getPrefix() const { return prefix; }
    OIS::Keyboard* getKeyboard() { return keyboard;}
private:

    Ogre::SceneManager* mSceneMgr;
    Ogre::SceneNode* rootNode;
    std::string prefix;
    OIS::Keyboard* keyboard;
    bool initialized;
};

#endif