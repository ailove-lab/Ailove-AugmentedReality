#ifndef _ARENGINE_
#define _ARENGINE_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <OgreMatrix4.h>

namespace ARToolKitPlus {
    class TrackerSingleMarker;
}

class VideoFrame;
class PatternManager;
class XmlSerializer;

//-------------------------------------------------------------------------------------

/** Manages primary AR functionality */
class AREngine {
public:
    typedef std::vector<std::pair<int, Ogre::Matrix4> > PatternStorage;
    struct PatternsInfo {
        Ogre::Matrix4 projMatrix;
        PatternStorage patterns;
    };


    AREngine(void);
    ~AREngine(void);

    void calculate(boost::shared_ptr<VideoFrame> frame, const PatternManager& manager);
    boost::shared_ptr<PatternsInfo> getPatterns() const {
        return patterns;
    }

    void serialize(XmlSerializer& sr);

protected:

    void init(int width, int height);

private:
    boost::shared_ptr<ARToolKitPlus::TrackerSingleMarker> tracker;
    boost::shared_ptr<PatternsInfo> patterns;

    std::string calibrationFileName;
    float nearClip;
    float farClip;

    static int JITTER_CONSTANT;
};

//-------------------------------------------------------------------------------------

#endif;
