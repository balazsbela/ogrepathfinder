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


#ifndef __PathFind_h_
#define __PathFind_h_

#include "BaseApplication.h"
#include "SinbadCharacterController.h"
#include "astar.h"

class PathFind : public BaseApplication {
public:
    PathFind(void);
    virtual ~PathFind(void);

protected:

    SinbadCharacterController* mChara;
    PathFinder* mpathFind;
    SceneNode** mobsNodes;

    void initPathFinder();
    void createScene(void);
    void loadObstacles();
    void moveToGoal();
    bool frameRenderingQueued(const FrameEvent& evt);
    bool keyPressed(const OIS::KeyEvent& evt);
    bool keyReleased(const OIS::KeyEvent& evt);

    void delay(float t);

#if OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
    bool touchPressed(const OIS::MultiTouchEvent& evt);
    bool touchMoved(const OIS::MultiTouchEvent& evt);

#else        
    bool mouseMoved(const OIS::MouseEvent& evt);
    bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
#endif
};

#endif // #ifndef __TutorialApplication_h_
