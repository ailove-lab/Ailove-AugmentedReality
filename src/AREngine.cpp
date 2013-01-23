#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
#include <OgreMatrix4.h>
#include "AREngine.h"
#include "VideoFrame.h"
#include "PatternManager.h"
#include "XmlSerializer.h"


//-------------------------------------------------------------------------------------

using namespace ARToolKitPlus;
using namespace Ogre;

//-------------------------------------------------------------------------------------

int AREngine::JITTER_CONSTANT = 4;

//-------------------------------------------------------------------------------------

AREngine::AREngine(void)
{
    calibrationFileName = "data/LogitechPro4000.dat";
    nearClip = 1;
    farClip = 1000;
    
}

//-------------------------------------------------------------------------------------

AREngine::~AREngine(void)
{
}

//-------------------------------------------------------------------------------------

void initMatrix(Matrix4 &m, const ARFloat* p) {
	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j) {
			m[j][i] = *p++;
		}
}

//-------------------------------------------------------------------------------------

void initMatrix(Matrix4 &m, const ARFloat transf[3][4]) {
	const ARFloat *p = &transf[0][0];
	for(int i = 0; i < 3; ++i)
	for(int j = 0; j < 4; ++j) {
		m[i][j] = *p++;
	}
	m[3][0]=m[3][1]=m[3][2]=0;
	m[3][3]=1;
}

//-------------------------------------------------------------------------------------

void AREngine::calculate(boost::shared_ptr<VideoFrame> frame, const PatternManager& manager) 

{
    PatternsInfo oldPatterns;
    if (patterns.get()) {
        oldPatterns = *patterns;
    }
    patterns.reset(new PatternsInfo());

    if (!tracker.get()) {
        init(frame->getWidth(), frame->getHeight());
    }

	ARToolKitPlus::ARMarkerInfo* markers = NULL;
	int numMarkers = 0;

    patterns->patterns.reserve(tracker->getNumLoadablePatterns());
    initMatrix(patterns->projMatrix, tracker->getProjectionMatrix());

    int markerId = tracker->calc(frame->getFrame(), -1, true, &markers, &numMarkers);
	float conf = (float)tracker->getConfidence();
	if ( markerId != -1) {
		for(int i = 0; i < numMarkers; ++i) {
            int id = markers[i].id;
            if(id != -1) {
                const PatternManager::PatternToNode* pattern = manager.get(id);
                if (pattern) {
			        ARFloat center[] = {0, 0};
			        ARFloat tr[3][4];
			        if(-1 == tracker->executeSingleMarkerPoseEstimator(&markers[i], center, 1, tr)){
				        continue;
			        }

			        Matrix4 m; 
                    initMatrix(m, tr);

                    for(PatternStorage::iterator it = oldPatterns.patterns.begin(); oldPatterns.patterns.end() != it; ++it) {
                        if (it->first == id) {
                            m = it->second + (m-it->second) * (1.f/JITTER_CONSTANT);
                        }
                    }


                    patterns->patterns.push_back(std::make_pair(id, m));
                }
            }
		}
    }
}

//-------------------------------------------------------------------------------------

void AREngine::init(int width, int height) 

{
    tracker.reset(new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6,1, 8>(width, height));
    
	const bool useBCH = false;
	tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_RGB);
    if(!tracker->init(calibrationFileName.c_str(), nearClip, farClip)) {
		exit(0);
	}

	// the marker in the BCH test image has a thin border...
    tracker->setBorderWidth(useBCH ? 0.125f : 0.250f);

    // set a threshold. alternatively we could also activate automatic thresholding
    tracker->setThreshold(150);

	tracker->activateAutoThreshold(true);

    // let's use lookup-table undistortion for high-speed
    // note: LUT only works with images up to 1024x1024
    tracker->setUndistortionMode(ARToolKitPlus::UNDIST_LUT);

    // RPP is more robust than ARToolKit's standard pose estimator
    // NOTE: it hangs "calc" function!!!
    //  tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);

    // switch to simple ID based markers
    // use the tool in tools/IdPatGen to generate markers
    tracker->setMarkerMode(useBCH ? ARToolKitPlus::MARKER_ID_BCH : ARToolKitPlus::MARKER_ID_SIMPLE);
	
}

//-------------------------------------------------------------------------------------

void AREngine::serialize(XmlSerializer& sr) 

{   
	Member<std::string> memc = member("calibration", calibrationFileName);
	Member<float> memn = member("nearClip", nearClip);
	Member<float> memf = member("farClip", farClip);
//	
//	sr % member("calibration", calibrationFileName)
//       % member("nearClip", nearClip)
//       % member("farClip", farClip);
	
	sr % memc % memn % memf;
}


//-------------------------------------------------------------------------------------
