/*
-----------------------------------------------------------------------------
Filename:    GraphicsEngine.cpp
-----------------------------------------------------------------------------


This source file is generated by the
   ___                   _              __    __ _                  _ 
  /___\__ _ _ __ ___    /_\  _ __  _ __/ / /\ \ (_)______ _ _ __ __| |
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \ \/  \/ / |_  / _` | '__/ _` |
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \  /\  /| |/ / (_| | | | (_| |
\___/ \__, |_|  \___| \_/ \_/ .__/| .__/ \/  \/ |_/___\__,_|_|  \__,_|
      |___/                 |_|   |_|                                 
      Ogre 1.7.x Application Wizard for VC10 (June 2010)
      http://code.google.com/p/ogreappwizards/
-----------------------------------------------------------------------------
*/

#include <boost/lexical_cast.hpp>
#include "GraphicsEngine.h"
#include "ARManager.h"
#include "VideoFrame.h"
#include "XmlSerializer.h"
#include "OgreMaxScene.hpp"
#include "ARException.h"
#include "CarController.h"
#include "HelicopterController.h"
#include <highgui.h>

using namespace Ogre;

std::string GraphicsEngine::BACKGROUND_TEXTURE_NAME = "backgroundTexture";
std::string GraphicsEngine::BACKGROUND_NODE_NAME = "backgroundNode";
std::string GraphicsEngine::BACKGROUND_MAT_NAME = "backgroundMaterial";
unsigned long GraphicsEngine::VISIBILITY_TIME = 5000;

//-------------------------------------------------------------------------------------


GraphicsEngine::Logo::Logo() 

{
    x = y = width = height = 0;
}

//-------------------------------------------------------------------------------------

void GraphicsEngine::Logo::serialize(XmlSerializer& sr) 

{
	
	Member<std::string> memf = member("name", fileName);
    //TODO Make it safe!
    XmlAttributeHelper<float> helper = XmlAttributeHelper<float>() % x % y % width % height;
	Member< XmlAttributeHelper<float> > memx = member("size", helper); 
	
	//sr % member("name", fileName)
      // % member("size", XmlAttributeHelper<float>() % x % y % width % height);
	
	sr % memf % memx;
}

//-------------------------------------------------------------------------------------

GraphicsEngine::GraphicsEngine(ARManager* manager)
    : manager(manager), animationSpeed(1)
{

    // temporary
    // used to generate default xml if necessary
    models.push_back(Model("ogrehead.mesh"));
    models.back().setScale(0.01f);

    models.push_back(Model("fish.mesh"));
    models.back().setAnimationState("swim");
    models.back().setScale(0.2f);
    //models.back().setOrientation(Ogre::Quaternion();

    logos.push_back(Logo());
    logos.back().fileName = "Logo1.tga";
    logos.back().width = 0.2f;
    logos.back().height = 0.2f;

    showConfig = false;
    camPattern = -1;

    timer.reset(new Timer());

    ogreMaxScene.reset(new OgreMax::OgreMaxScene());

}
//-------------------------------------------------------------------------------------

GraphicsEngine::~GraphicsEngine(void)
{
}

//-------------------------------------------------------------------------------------

void GraphicsEngine::createLogos(void)

{
      if (!logos.empty()) {
        Overlay* overlay = OverlayManager::getSingleton().create("Logo");

        typedef std::map<std::string, MaterialPtr> MapByName;
        MapByName materials;

        // index to enable usage of same logo several times
        int i = 0;
        for(std::list<Logo>::const_iterator it = logos.begin(); logos.end() != it; ++it, ++i) {
            const std::string& name = it->fileName;
            Ogre::OverlayContainer *overlayCont = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", name + boost::lexical_cast<std::string>(i)));
            overlayCont->setMetricsMode(GMM_RELATIVE);
            overlayCont->setDimensions(it->width, it->height);
            overlayCont->setPosition(it->x, it->y);

            overlay->add2D(overlayCont);

            MapByName::iterator jt = materials.find(name);
            if (materials.end() == jt) {
                TexturePtr texture = TextureManager::getSingleton().create(it->fileName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            
	            MaterialPtr material = MaterialManager::getSingleton().create(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
                material->getTechnique(0)->getPass(0)->setSceneBlending(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
	            material->getTechnique(0)->getPass(0)->createTextureUnitState(name);
                materials.insert(std::make_pair(name, material));
            }
            overlayCont->setMaterialName(name);
            //overlayCont->show();
        }
        overlay->show();
    }
}

//-------------------------------------------------------------------------------------

void GraphicsEngine::createScene(void)

{
    createLogos();

    std::map<std::string, int> repeatCount;
    std::map<int, Ogre::SceneNode*> groupNode;
    for(std::list<Model>::iterator it = models.begin(); models.end() != it; ++it) {

        // checks model repetition
        std::string name = it->getName();
        if (repeatCount.find(name) != repeatCount.end()) {
            int& count = repeatCount[name];
            ++count;
            name = name + boost::lexical_cast<std::string>(count);
        } else {
            repeatCount[name] = 0;
        }

        it->setNodeName(name);

        // checks grouping
        int groupId = it->getGroupId();
        Ogre::SceneNode* nodeParent = groupNode[groupId];
        if (!nodeParent) {
            nodeParent = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
            // to be sure it is invisible at start
            nodeParent->setPosition(1e10, 1e10, 1e10);
            if (groupId != -1) {
                groupNode[groupId] = nodeParent;
                // associate only first model with pattern
                manager->registerPattern(name, it->getPatternId());
            }
        }
       
        
        Ogre::SceneNode* node = nodeParent->createChildSceneNode(name + "_child");

        float s = it->getScale();
        node->setScale(s, s, s);
        node->setPosition(it->getPosition());
        node->setOrientation(it->getOrientation());

        if (it->isScene()) {
            ogreMaxScene->SetNamePrefix(name, OgreMax::OgreMaxScene::ALL_NAME_PREFIXES);
            String resName = name;
            ResourceGroupManager::getSingleton().createResourceGroup(resName);
            FileInfoListPtr fp = ResourceGroupManager::getSingleton().findResourceFileInfo(ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, it->getName());
            if (!fp->empty()) {
                const FileInfo& info = *fp->begin();
                const String& sceneName = it->getName();
                String path = info.archive->getName() + "/" + sceneName.substr(0, sceneName.size() - 6) + "_res";
                ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", resName, true);
                ResourceGroupManager::getSingleton().initialiseResourceGroup(resName);
            }
            ogreMaxScene->Load(it->getName(), mWindow, OgreMax::OgreMaxScene::NO_OPTIONS, mSceneMgr, node, 0, resName);
            
            boost::shared_ptr<ModelController> controller(new CarController());
            controller->init(mSceneMgr, mSceneMgr->getSceneNode(it->getName()), name, mKeyboard);
            modelControllers.push_back(controller);

            controller.reset(new HelicopterController());
            controller->init(mSceneMgr, mSceneMgr->getSceneNode(it->getName()), name, mKeyboard);
            modelControllers.push_back(controller);

        } else {
            Ogre::Entity* entity = mSceneMgr->createEntity(name, it->getName());
            node->attachObject(entity);
            
         /*   {

              AnimationStateSet *set = entity->getAllAnimationStates();
               AnimationStateSet::iterator it = set->begin();
               while(it != set->end())
               {
                  AnimationState &anim = it->second;
                  anim.setEnabled(false);
                  anim.setWeight(0);
                  anim.setTimePosition(0);
                  ++it;
               }
               mSource = mEntity->getAnimationState( animation );
               mSource->setEnabled(true);
               mSource->setWeight(1);
               mTimeleft = 0;
               mDuration = 1;
               mTarget = 0;
               complete = false;
               loop = l;
            }*/


            if (entity->getAllAnimationStates()) {
                Ogre::AnimationStateIterator jt = entity->getAllAnimationStates()->getAnimationStateIterator();
                while(jt.hasMoreElements()) {
                    AnimationState* state = jt.getNext();
                    
                    std::string name = state->getAnimationName();
                    Ogre::LogManager::getSingleton().logMessage((std::string("AnimationState: ") + name).c_str());
                }
            }
            try {
                AnimationState* anim = entity->getAnimationState(it->getAnimationState());
	            anim->setEnabled(true);
	            anim->setLoop(true);
            } catch(...) {
            }
        }
    }

    SceneManager::MovableObjectIterator it = mSceneMgr->getMovableObjectIterator(LightFactory::FACTORY_TYPE_NAME);
    while(it.hasMoreElements()) {
		Ogre::Light *l = reinterpret_cast<Ogre::Light*>(it.getNext());
        l->setVisible(false);
	}

    // Set ambient light
    //mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));

    // Create a light
    //Ogre::Light* l = mSceneMgr->createLight("MainLight");
    //l->setPosition(20,80,50);
    //l->setsoit
}

//-------------------------------------------------------------------------------------

void decompose(Matrix4& m, Vector3& pos, Vector3& scale, Quaternion& orient) {
	Vector3 x = Vector3(m[0][0], m[1][0], m[2][0]);
	Vector3 y = Vector3(m[0][1], m[1][1], m[2][1]);
	Vector3 z = Vector3(m[0][2], m[1][2], m[2][2]);

	scale = Vector3(x.length(), y.length(), z.length());
	pos = Vector3(m[0][3], m[1][3], m[2][3]);
	orient = Quaternion(x.normalisedCopy(), y.normalisedCopy(), z.normalisedCopy());
}

//-------------------------------------------------------------------------------------

void GraphicsEngine::updateARModels(boost::shared_ptr<ARManager::Data> frameData) 

{
    if (!frameData.get()) {
        return;
    }

    boost::shared_ptr<AREngine::PatternsInfo> patterns = frameData->patterns;

    unsigned long currentTime = timer->getMilliseconds();
    mCamera->setCustomProjectionMatrix(true, patterns->projMatrix);
    

    if (camPattern < 0 && !patterns->patterns.empty()) {
        camPattern = patterns->patterns[0].first;
        Matrix4 view = patterns->patterns[0].second * Quaternion(Radian(Degree(90)), Vector3(1, 0, 0));
        mCamera->setCustomViewMatrix(true, view);
    }

    Matrix4 invMV = mCamera->getViewMatrix().inverse();
    for(AREngine::PatternStorage::const_iterator it = patterns->patterns.begin(); patterns->patterns.end() != it; ++it) {
        lastUpdatedTime[it->first] = currentTime;
    
        const char* name = manager->getNodeNames(it->first);
        if (name) {
            SceneNode* node = mSceneMgr->getSceneNode(name);
            Vector3 pos, scale;
		    Quaternion orient;
            Ogre::Matrix4 matrix = invMV * it->second;
		    decompose(matrix, pos, scale, orient);
            node->setPosition(pos);
		    node->setOrientation(orient);
            showSceneLight(name, true);
        }
    }

    for(std::map<int, unsigned long>::iterator it = lastUpdatedTime.begin(); lastUpdatedTime.end() != it; ++it) {
        if (currentTime - it->second > VISIBILITY_TIME) {
            const char* name = manager->getNodeNames(it->first);
            if (name) {
                SceneNode* node = mSceneMgr->getSceneNode(name);
		        node->setPosition(1e10, 1e10, 1e10);
                showSceneLight(name, false);

                for(int i = 0; i < modelControllers.size(); ++i) {
                   if (node == modelControllers[i].get()->getRoot()) {
                       modelControllers[i].get()->reset();
                   }
                }

                if (camPattern == it->second) {
                    camPattern = -1;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------

void copyFrame(boost::shared_ptr<VideoFrame> frame, uint8* pDest, TexturePtr texture) 

{
    int width = texture->getWidth();
    int height = texture->getHeight(); 

    const unsigned char* src = frame->getFrameRGBA();
    if (width == frame->getWidth()) {
        memcpy(pDest, src, width * height * 4);
    } else {
        int wSrc = frame->getWidth() * 4;
        int wDst = width * 4;
        for(int i = 0; i < frame->getHeight(); ++i) {
            memcpy(pDest, src, wSrc);
            src += wSrc;
            pDest += wDst;
        }
    }
}


//-------------------------------------------------------------------------------------

void GraphicsEngine::updateBackground(boost::shared_ptr<ARManager::Data> frameData) 

{
    if (!frameData.get()) {
        return;
    }

    boost::shared_ptr<VideoFrame> frame = frameData->currentFrame;
    
    if (!prevFrame.get() || prevFrame->getFrameNumber() !=frame->getFrameNumber()) {
        TexturePtr texture = TextureManager::getSingleton().getByName(BACKGROUND_TEXTURE_NAME);
        if (!texture.get()) {
            initBackground(frame->getWidth(), frame->getHeight());
            texture = TextureManager::getSingleton().getByName(BACKGROUND_TEXTURE_NAME);
        }

        HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
 
	    pixelBuffer->lock(HardwareBuffer::HBL_DISCARD);
	    const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
 
	    uint8* pDest = static_cast<uint8*>(pixelBox.data);
        copyFrame(frame, pDest, texture);
	    pixelBuffer->unlock();
    }
}

//-------------------------------------------------------------------------------------

void GraphicsEngine::initBackground(int width, int height) 

{
    int textWidth = 1;
    while(textWidth < width)  {
        textWidth *= 2;
    }

    int textHeight = 1;
    while(textHeight < height)  {
        textHeight *= 2;
    }
    TexturePtr texture = TextureManager::getSingleton().createManual(
		BACKGROUND_TEXTURE_NAME,
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D,      
		textWidth, textHeight,        
		0,               
		PF_X8R8G8B8,    
		TU_DEFAULT);

    if (texture->getFormat() != PF_X8R8G8B8) {
        std::cout << "Background texture format: " << texture->getFormat() << std::endl;
        throw ARException::CAN_NOT_INIT_TEXTURE;            
    }

    MaterialPtr material = MaterialManager::getSingleton().create(BACKGROUND_MAT_NAME, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	material->getTechnique(0)->getPass(0)->createTextureUnitState(BACKGROUND_TEXTURE_NAME);

    Rectangle2D* rect = new Rectangle2D(true);
    rect->setCorners(-1, 1, -1 + 2.f * texture->getWidth() / width, 1 - (2.f*texture->getHeight() / height));
	rect->setMaterial(material->getName());

	rect->setRenderQueueGroup(RENDER_QUEUE_BACKGROUND);

	// Use infinite AAB to always stay visible
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	rect->setBoundingBox(aabInf);

	// Attach background to the scene
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(BACKGROUND_NODE_NAME);
	node->attachObject(rect);
}

//-------------------------------------------------------------------------------------

bool GraphicsEngine::frameRenderingQueued(const Ogre::FrameEvent& evt) 

{
    if (!BaseApplication::frameRenderingQueued(evt)) {
        return false;
    }

    boost::shared_ptr<ARManager::Data> data = manager->getFrameData();
    updateBackground(data);
    updateARModels(data);

    for(int i = 0; i < modelControllers.size(); ++i) {
        modelControllers[i].get()->update(evt.timeSinceLastFrame);
    }

    if (mKeyboard->isKeyDown(OIS::KC_ADD)) {
        animationSpeed += evt.timeSinceLastFrame*10;
    }

    if (mKeyboard->isKeyDown(OIS::KC_SUBTRACT)) {
        animationSpeed -= evt.timeSinceLastFrame * 10;
    }

    animationSpeed = Math::Clamp(animationSpeed, 0.1f, 100.f);

    ogreMaxScene->Update(evt.timeSinceLastFrame*animationSpeed);
    //temporary
    for(std::list<Model>::iterator it = models.begin(); models.end() != it; ++it) {

        //TODO: modify to using AnimationManager for example instead of name
        if (!it->getAnimationState().empty()) {
            try {
                Ogre::Entity* ent = mSceneMgr->getEntity(it->getNodeName());
                AnimationState* anim = ent->getAnimationState(it->getAnimationState());
                anim->addTime(evt.timeSinceLastFrame);
            } catch(...) {
                Ogre::LogManager::getSingleton().logMessage("Can't update animation");
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------

void GraphicsEngine::serialize(XmlSerializer& sr) {
	
	Member< std::list<Model> > memm = member("models", models);
	Member< std::list<Logo> > meml = member("logos", logos);
	
	sr % memm % meml % member("showConfig", showConfig);
}

//-------------------------------------------------------------------------------------

bool GraphicsEngine::showConfigDialog(void)

{
    if (showConfig || !mRoot->restoreConfig()) {
        return mRoot->showConfigDialog();
    }

    return true;
}

//-------------------------------------------------------------------------------------

//#define WRITE_VIDEO
#ifdef WRITE_VIDEO

cv::VideoWriter videoWriter;
cv::Mat screenCopy;
bool GraphicsEngine::frameEnded(const FrameEvent& evt) 

{

    int w = mWindow->getWidth();
    int h = mWindow->getHeight();
    if (!videoWriter.isOpened()) {
        videoWriter = cv::VideoWriter("out.avi", -1, 25, cv::Size(w, h));
        unsigned char* data = new unsigned char[w*h*3];
        screenCopy = cv::Mat(h, w, CV_8UC3, data);
        //std::cout << "**********************************width  " << w << " " << h << std::endl;
    }

    mWindow->copyContentsToMemory(Ogre::PixelBox(w, h, 1, Ogre::PixelFormat::PF_R8G8B8, screenCopy.data)); 
    if (videoWriter.isOpened()) {
        videoWriter << screenCopy;
    }

    //static int frame = 0;
    //char buf[256];
    //sprintf(buf, "scr\\image%04d.bmp", frame++);

    ////std::cout << "**********************************width  " << w << " " << h << std::endl;

    ////mWindow->writeContentsToFile(buf);
    return BaseApplication::frameEnded(evt);
}

#else
bool GraphicsEngine::frameEnded(const FrameEvent& evt) 

{
    return BaseApplication::frameEnded(evt);
}

#endif

//-------------------------------------------------------------------------------------

void GraphicsEngine::showSceneLight(const std::string& name, bool show)

{
    SceneManager::MovableObjectIterator it = mSceneMgr->getMovableObjectIterator(LightFactory::FACTORY_TYPE_NAME);
    while(it.hasMoreElements()) {
		Ogre::Light *l = reinterpret_cast<Ogre::Light*>(it.getNext());
        if (l->getName().find(name) == 0 && l->getVisible() != show) {
            l->setVisible(show);
        }
	}
}

//-------------------------------------------------------------------------------------

bool GraphicsEngine::keyPressed( const OIS::KeyEvent &arg )

{
    
    return BaseApplication::keyPressed(arg);
    
}