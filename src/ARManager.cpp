#include "ARManager.h"
#include "AREngine.h"
#include "GraphicsEngine.h"
#include "VideoInput.h"
#include "VideoFrame.h"
#include "PatternManager.h"

#include  "XmlSerializer.h"


using namespace boost;

std::string ARManager::CONFIG_FILE_NAME="config.xml";

//-----------------------------------------------------------------------------

ARManager::ARManager(void)
    : interrupted(false)
{
}

//-----------------------------------------------------------------------------

ARManager::~ARManager(void)

{
    if (thread.get()) {
        interrupted = true;
        thread->join();
    }
}

//-----------------------------------------------------------------------------

void ARManager::init()

{
    graphics.reset(new GraphicsEngine(this));
    videoInput.reset(new VideoInput());
    videoInput->init();

    arEngine.reset(new AREngine());
    patternManager.reset(new PatternManager());

    
  //  XmlSerializer::save(CONFIG_FILE_NAME, "ArManager", *this);
    XmlSerializer::load(CONFIG_FILE_NAME, "ArManager", *this);
    

    thread.reset(new boost::thread(&ARManager::doUpdate, this));
}

//-----------------------------------------------------------------------------

boost::shared_ptr<ARManager::Data> ARManager::getFrameData()

{
    boost::lock_guard<boost::mutex> lock(mutex); 
    return frameData;
}

//-----------------------------------------------------------------------------

void ARManager::registerPattern(const std::string& name, int id)

{
    boost::lock_guard<boost::mutex> lock(mutex); 
    patternManager->registerPattern(name, id);
}

//-----------------------------------------------------------------------------

void ARManager::doUpdate() 

{
    while(!interrupted) {
        // camera frame rate is restricted thus we capture image in the separate thread to avoid CPU halt
        videoInput->update();

        arEngine->calculate(videoInput->getFrame(), *patternManager);

        boost::lock_guard<boost::mutex> lock(mutex);

        boost::shared_ptr<AREngine::PatternsInfo> patterns = arEngine->getPatterns();
        std::vector<int> ids;
        ids.reserve(patterns->patterns.size());
        
        for(AREngine::PatternStorage::const_iterator it = patterns->patterns.begin(); patterns->patterns.end() != it; ++it) {
            ids.push_back(it->first);
        }

        // associate patterns automatically
        patternManager->updateIds(ids);

        // copy frame and patterns to separate structure to give synchronized access to frame and patterns simultaneously
        frameData.reset(new Data());
        frameData->currentFrame = videoInput->getFrame();
        frameData->patterns = arEngine->getPatterns();
    }
}

//-----------------------------------------------------------------------------

const char* ARManager::getNodeNames(int patternId) const

{
    const PatternManager* p = patternManager.get();
    const PatternManager::PatternToNode* pattern = p->get(patternId);
    return pattern ? pattern->nodeName.c_str() : NULL;
}

//-----------------------------------------------------------------------------

void ARManager::start() 

{
    graphics->go();
}

//-----------------------------------------------------------------------------

void ARManager::serialize(XmlSerializer& sr) {
	Member<GraphicsEngine> memg = member("Graphics", *graphics);
	Member<AREngine> mema = member("AREngine", *arEngine);
//	sr % (member("Graphics", *graphics)) % (member("AREngine", *arEngine));
	sr % memg % mema;
}
