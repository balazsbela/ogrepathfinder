/**************************************************************************
 * Ogre AStar Pathfinder Simulation                                       *
 * by Balázs Béla, balazsbela@gmail.com                                   *
 * 2010                                                                   *
 * Based on the Ogre Wiki tutorial framework and on the Character sample  *
 * in the Ogre Samples framework                                          *
 *                                                                        *
 *   http://balazsbela.blogspot.com                                       *
 *                                                                        *
 *                                                                        *
 **************************************************************************/


#include "PathFind.h"

using namespace Ogre;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else

    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        // TutorialApplication app;
        PathFind app;

        try {
            app.go();
        } catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
