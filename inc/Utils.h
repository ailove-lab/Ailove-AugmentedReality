#ifndef UTILS_H
#define UTILS_H

#include <OgreAxisAlignedBox.h>

namespace Ogre {
    class SceneNode;
}

class Utils {
public:
    Utils(void);
    ~Utils(void);

    static Ogre::AxisAlignedBox getBounds(Ogre::SceneNode* node);

};

#endif