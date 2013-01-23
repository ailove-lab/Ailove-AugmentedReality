// ARBaseProject.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <OgreException.h>
#include <OgreLogManager.h>
#include "ARManager.h"
#include "ARException.h"


#ifdef WIN32
    #include <Windows.h>
#endif

void logError(std::string s) 
{

    if (!Ogre::LogManager::getSingletonPtr()) {
        Ogre::LogManager *log = new Ogre::LogManager();
        Ogre::LogManager::getSingleton().createLog("Ogre.log");
    }
    Ogre::LogManager::getSingleton().logMessage("******************************EXCEPTION!!!******************************");
    Ogre::LogManager::getSingleton().logMessage(s);
    #ifdef WIN32
        ::MessageBoxA(NULL, s.c_str(), "AILove", MB_ICONWARNING);
    #endif
}

#ifdef WIN32
    int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
    int _tmain(int argc, _TCHAR* argv[])
#endif
{
    try {
        //should be outside of try/catch block to correctly manage errors
        ARManager manager;
        try {
            manager.init();
            manager.start();
         } catch (Ogre::Exception& e) {
             logError(e.getDescription());
         } catch(ARException& e) {
             logError(e.getMessage());
         } catch(...) {
             logError("Непредвиденная ошибка");
         }
    } catch(...) {
    }
     return 0;
}
