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
#include <OgreColourValue.h>

#define COEF 10

using namespace Ogre;

PathFind::PathFind(void) {
    mChara = NULL;
    mpathFind = NULL;
}

PathFind::~PathFind(void) {
    if (mChara) delete mChara;
    if (mpathFind) delete mpathFind;
    if (mobsNodes) delete[] mobsNodes;

    MeshManager::getSingleton().remove("floor");
}

void PathFind::createScene(void) {
    mSceneMgr->setFog(Ogre::FOG_LINEAR, ColourValue(1.0f, 1.0f, 0.8f), 0, 15, 500);
    mCamera->getViewport()->setBackgroundColour(ColourValue(1.0f, 1.0f, 0.8f));
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
    mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
    mSceneMgr->setShadowTextureSize(1024);
    mSceneMgr->setShadowTextureCount(1);

    mCameraMan->setStyle(OgreBites::CS_MANUAL);

    // add a bright light above the scene
    Light* light = mSceneMgr->createLight();
    light->setType(Light::LT_POINT);
    //light->setPosition(-10, 70, 20);
    light->setPosition(-20, 120, 20);
    light->setSpecularColour(ColourValue::White);


    //init pathfinder
    mpathFind = new PathFinder();
    if (mpathFind->AStar()) {

        /*for (int i=0; i < pathFind -> obstacles.size(); i++) {
            std::cout << pathFind -> obstacles[i].x << " " << pathFind -> obstacles[i].y << "\n";
        }*/

        mpathFind->reconstructPath();
        //In order to actually exit the labyrinth
        mpathFind->directions.push_back(mpathFind->directions[mpathFind->directions.size() - 1]);

        for (int i = 0; i < mpathFind->directions.size(); i++) {
            std::cout << mpathFind -> directions[i] << "\n";
        }

        mpathFind->printGrid();
        // cout << "\nLength of path:" << pathF.getLength() << "\n";
    } else {
        std::cout << "There is no way to exit the labyrinth!\n";
    }



    // create a floor mesh resource
    MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Plane(Vector3::UNIT_Y, 0), mpathFind->grd->n * COEF * 8, mpathFind->grd->m * COEF * 8,
            3.4, 3.4, true, 1, 100, 100, Vector3::UNIT_Z);

    // create a floor entity, give it a material, and place it at the origin
    Entity* floor = mSceneMgr->createEntity("Floor", "floor");
    floor->setMaterialName("Examples/Rockwall");
    floor->setCastShadows(false);
    mSceneMgr->getRootSceneNode()->attachObject(floor);

    // create our character controller
    mChara = new SinbadCharacterController(mCameraMan->getCamera(), mSceneMgr);
    /*std::stringstream oss;
    oss << evt.timeSinceLastFrame;
    oss << " " <<      mAnimationState->getAnimationName();*/
    Ogre::LogManager::getSingleton().logMessage("Loaded CHARACTERS!!!");

    loadObstacles();

    moveToGoal();
}

void PathFind::loadObstacles() {

    mobsNodes = new SceneNode*[mpathFind -> obstacles.size()];

    //Ogre::LogManager::getSingleton().logMessage("Obstacle size:"+ss.str());
    //I wanted to use StaticGeometry but it doesn't work with raycasting
    // StaticGeometry *sg = mSceneMgr->createStaticGeometry("Maze");

    int sizex = mpathFind->grd->n;
    int sizey = mpathFind->grd->m;
    //  sg->setRegionDimensions(Vector3(sizex*COEF, 20 ,sizey*COEF));
    //sg->setOrigin(Vector3(-600, 0, -600));
    //  sg->setOrigin(Vector3(0, 0, 0));

    for (int k = 0; k < mpathFind -> obstacles.size(); k++) {
        int i = mpathFind -> obstacles[k].x;
        int j = mpathFind -> obstacles[k].y;
        Vector3 pos(i * COEF - sizex / 2 * COEF, 3.4, j * COEF - sizey / 2 * COEF);
        Vector3 scale(5, 7, 5);
        Quaternion orientation = Ogre::Quaternion::IDENTITY;

        stringstream name;
        name << "Obstacle";
        name << k;
        Entity* obstacle = mSceneMgr->createEntity(name.str(), "obstacle.mesh");
        name.str().clear();
        mobsNodes[k] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mobsNodes[k] ->attachObject(obstacle);
        mobsNodes[k] ->setOrientation(orientation);
        mobsNodes[k] ->setPosition(pos);
        mobsNodes[k] -> setScale(scale);
        //sg -> addEntity(obstacle,pos,orientation,scale);
    }

    // sg -> build();

    //sg->setCastShadows(true);

    int startx = mpathFind->grd->start.x;
    int starty = mpathFind->grd->start.y;

    mChara->mBodyNode->translate(startx * COEF - sizex / 2 * COEF, 0, starty * COEF - sizey / 2 * COEF);
}

bool PathFind::frameRenderingQueued(const FrameEvent& evt) {
    // let character update animations and camera
    if (mChara) mChara->addTime(evt.timeSinceLastFrame);
    return BaseApplication::frameRenderingQueued(evt);
}

bool PathFind::keyPressed(const OIS::KeyEvent& evt) {
    // relay input events to character controller
    if (evt.key == OIS::KC_ESCAPE) {
        mShutDown = true;
    }

    if (mChara) {// mChara->injectKeyDown(evt);
    }

    //Ogre::LogManager::getSingleton().logMessage("Key Pressed!");
    //return keyPressed(evt);
}

bool PathFind::keyReleased(const OIS::KeyEvent& evt) {
    // relay input events to character controller
    //if(mChara)  mChara->injectKeyUp(evt);
    //return keyReleased(evt);
}

#if OGRE_PLATFORM == OGRE_PLATFORM_IPHONE

bool touchPressed(const OIS::MultiTouchEvent& evt) {
    // relay input events to character controller
    if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt);
    return touchPressed(evt);
}

bool touchMoved(const OIS::MultiTouchEvent& evt) {
    // relay input events to character controller
    if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);
    return touchMoved(evt);
}
#else

bool PathFind::mouseMoved(const OIS::MouseEvent& evt) {
    // relay input events to character controller
    if (mChara) mChara->injectMouseMove(evt);
}

bool PathFind::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id) {
    // relay input events to character controller
    if (mChara) mChara->injectMouseDown(evt, id);
    //Ogre::LogManager::getSingleton().logMessage("Mouse Pressed!");

    //return mousePressed(evt, id);
}

void PathFind::moveToGoal() {

    if (mChara) {

        //we have to rotate the directions
        for (int i = 0; i < mpathFind -> directions.size(); i++) {
            if (mpathFind -> directions[i] == PathFinder::Up) {
                mpathFind -> directions[i] = PathFinder::Left;
            } else if (mpathFind -> directions[i] == PathFinder::Down) {
                mpathFind -> directions[i] = PathFinder::Right;
            } else if (mpathFind -> directions[i] == PathFinder::Left) {
                mpathFind -> directions[i] = PathFinder::Up;
            } else if (mpathFind -> directions[i] == PathFinder::Right) {
                mpathFind -> directions[i] = PathFinder::Down;
            }

        }

        //print directions:

        for (int i = 0; i < mpathFind->directions.size(); i++) {
            std::cout << mpathFind->directions[i] << "-";
        }

        mChara->mdirections = &mpathFind -> directions;
        mChara->move();

    }

}



#endif



