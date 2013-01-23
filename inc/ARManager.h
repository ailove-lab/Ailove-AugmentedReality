#ifndef _ARMANAGER_
#define _ARMANAGER_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "AREngine.h"


class GraphicsEngine;
class VideoInput;
class VideoFrame;
class PatternManager;
class XmlSerializer;

/**
 * Binds together ARToolkitPlus, VideoCapture, Ogre3D
 */
class ARManager {
public:

    struct Data {
        boost::shared_ptr<VideoFrame> currentFrame;
        boost::shared_ptr<AREngine::PatternsInfo> patterns;
    };

    /** Constructor */
    ARManager(void);

    /** Destructor */
    virtual ~ARManager(void);

    /** Initializes manager.*/
    void init();

    /** Starts a game.*/
    void start();

    boost::shared_ptr<Data> getFrameData();

    /** Registers pattern attached to node with the given name.
      * if pattern id is null then founded patterns will be associated with it automatically.
      */
    void registerPattern(const std::string& name, int id = -1);
    const char* getNodeNames(int patternId) const;

    void serialize(XmlSerializer& sr);

protected:
    void doUpdate();

private:
    boost::shared_ptr<GraphicsEngine> graphics;
    boost::shared_ptr<VideoInput> videoInput;
    boost::shared_ptr<PatternManager> patternManager;
    boost::shared_ptr<AREngine> arEngine;
    boost::shared_ptr<boost::thread> thread;
    
    boost::shared_ptr<Data> frameData;
    boost::mutex mutex;
    bool interrupted;

    static std::string CONFIG_FILE_NAME;
};

#endif

