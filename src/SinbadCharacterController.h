/******************************************************************
 * Ogre AStar Pathfinder Simulation                               *
 * by Balázs Béla, balazsbela@gmail.com                           *
 * 2010                                                           *
 * Based on the Ogre Wiki tutorial framework and on the Character *
 * sample in the Ogre Samples framework.                          *
 *                                                                *
 * Modified version of the original SinbadCharacterController     *
 * class in the Character sample from Ogre samples                *
 *                                                                *
 ******************************************************************/


/* Sinbad Character:
 * 
 * About: Sinbad Character Model
-----------------------------

Artist: Zi Ye
Date: 2009-2010
E-mail: omniter@gmail.com

This work is licensed under the Creative Commons Attribution-Share Alike 3.0 Unported License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.

This character is a gift to the OGRE community (http://www.ogre3d.org).
You do not need to give credit to the artist, but it would be appreciated. =)

This license applies to the following files:
- Sinbad.mesh
- Sinbad.skeleton
- Sinbad.blend
- sinbad_body.tga
- sinbad_clothes.tga
- sinbad_sword.tga
- Sword.mesh
*/


#ifndef __Sinbad_H__
#define __Sinbad_H__

#include <Ogre.h>
#include <OgreSceneQuery.h>
#include <OgreSceneManager.h>
#include <OIS.h>
#include "astar.h"

using namespace Ogre;

#define NUM_ANIMS 13           // number of animations the character has
#define CHAR_HEIGHT 5          // height of character's center of mass above ground
#define CAM_HEIGHT 3.5           // height of camera above character's center of mass
#define RUN_SPEED 17           // character running speed in units per second
#define TURN_SPEED 1000.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL 30.0f       // character jump acceleration in upward units per squared second
#define GRAVITY 80.0f          // gravity in downward units per squared second
#define AUTO_CAMERA 0       //should the camera turn with the character
#define MAX_DISTANCE 3.3     //how close can we get to an obstacle (the character's bounding box)
#define UNIT_WORLD 9.84     // the length of a distance, a unit in the A* matrix

class SinbadCharacterController {
private:

    // all the animations our character has, and a null ID
    // some of these affect separate body parts and will be blended together

    enum AnimID {
        ANIM_IDLE_BASE,
        ANIM_IDLE_TOP,
        ANIM_RUN_BASE,
        ANIM_RUN_TOP,
        ANIM_HANDS_CLOSED,
        ANIM_HANDS_RELAXED,
        ANIM_DRAW_SWORDS,
        ANIM_SLICE_VERTICAL,
        ANIM_SLICE_HORIZONTAL,
        ANIM_DANCE,
        ANIM_JUMP_START,
        ANIM_JUMP_LOOP,
        ANIM_JUMP_END,
        ANIM_NONE
    };


public:

    friend class PathFind;

    SinbadCharacterController(Camera* cam, SceneManager* scnmgr): mdirectionIndex(0), mfinished(1), mkeptDirection(0) {
        mSceneMgr = scnmgr;
        mLastPosition = Vector3::ZERO;
        mNeedPosition = 1;
        setupBody(cam->getSceneManager());
        setupCamera(cam);
        setupAnimations();
    }

  

    void addTime(Real deltaTime) {
        //if (mMovementTimer.getMilliseconds() > MOVEMENT_TIME) stop();
        updateBody(deltaTime);
        updateAnimations(deltaTime);
        updateCamera(deltaTime);

        detectObstacle();

    }

    //Moving functions

    void move() {
        mfinished = 0;
        if (mdirectionIndex < mdirections->size()) {
            PathFinder::Direction dir = (*mdirections)[mdirectionIndex];

            //mKeyDirection = Vector3::ZERO;
            // setBaseAnimation(ANIM_IDLE_BASE);
            //if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);

            if (dir == 0) {
                mKeyDirection.z = -1;
            } else if (dir == 1) {
                mKeyDirection.z = 1;
            } else if (dir == 2) {
                mKeyDirection.x = -1;
                if (AUTO_CAMERA) updateCameraGoal(90, 0, 0);
            } else if (dir == 3) {
                mKeyDirection.x = 1;
                if (AUTO_CAMERA) updateCameraGoal(-90, 0, 0);
            }

            if (mBaseAnimID == ANIM_IDLE_BASE) {
                setBaseAnimation(ANIM_RUN_BASE, true);
                if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
            }

        } else {
            //Start dancing because we are happy to get to the goal
            setBaseAnimation(ANIM_DANCE, true);
            setTopAnimation(ANIM_NONE);
            // disable hand animation because the dance controls hands
            mAnims[ANIM_HANDS_RELAXED]->setEnabled(false);
            mfinished = 1;
        }

    }

    void stop() {

        if (mdirectionIndex < mdirections->size()) {
            PathFinder::Direction dir1 = (*mdirections)[mdirectionIndex + 1];
            PathFinder::Direction dir2 = (*mdirections)[mdirectionIndex];
            std::cout << "Kept direction :" << mkeptDirection <<"\n";
            if (dir1 != dir2) {
                std::cout << "Changed direction " << dir1 << " " << dir2 << "\n";
                mkeptDirection = 0;
                mKeyDirection = Vector3::ZERO;
                setBaseAnimation(ANIM_IDLE_BASE);
                if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
            } else {
                mkeptDirection = 1;
            }
        }
        mdirectionIndex++;

        std::cout << "\nNow turning " << (*mdirections)[mdirectionIndex] << "\n";
        move();
        mNeedPosition = 1;

    }


    //Checks if an obstacle is in front of the character and calls stop, accordingly

    void detectObstacle() {

        if (mfinished) return;
        if (mNeedPosition) return;
        
        Vector3 pos = mBodyNode->getPosition();

        std::cout << mLastPosition <<" " << pos << "-" << mKeyDirection << "\n";
        std :: cout << "Traveled:" << mLastPosition.distance(pos) <<"\n";

        Real diff = UNIT_WORLD-mLastPosition.distance(pos);

        std::cout << diff <<"\n";
        if( (diff < 0.01 ) && (!mNeedPosition) ) {
            std::cout << "Traveled a unit, receiving next command \n";
            stop();
            return;
        }

        // Vector3 normal = mBodyNode->getOrientation() * Vector3::UNIT_Z;
        //int dist = mBodyNode->getAttachedObject(0)->getBoundingRadius() + 2;
        //Vector3 point = mBodyNode->getPosition() + mBodyNode->getOrientation()*(dist * Vector3::UNIT_Z);

        Vector3 point = mBodyNode->getPosition();
        Vector3 normal = mKeyDirection;

        Ray ray(Ogre::Vector3(point.x, point.y, point.z),
                Ogre::Vector3(normal.x, normal.y, normal.z));

        RaySceneQuery* rayQuery = mSceneMgr->createRayQuery(Ogre::Ray(), Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);
        rayQuery -> setSortByDistance(true);
        rayQuery -> setRay(ray);


        RaySceneQueryResult &query_result = rayQuery -> execute();
        if (query_result.size() <= 0) {
            std::cout << "Query had no results!!";
            return; // no obstacles
        }

        //Results are sorted by distance so we only care about the closest one

        Real distance = 0;
        bool foundObstacle = 0;

        for (int i = 0; i < query_result.size(); i++) {
            if (query_result[i].movable != NULL) {
                if (query_result[i].movable->getName().compare(0, 8, "Obstacle") == 0) {
                    query_result[i].movable->getParentSceneNode()->showBoundingBox(true);

                    if (query_result[i].distance < MAX_DISTANCE) {
                        std::cout << "Casting ray from:" << point.x << " " << point.y << " " << point.z << "\n";
                        std::cout << "In direction:" << normal.x << " " << normal.y << " " << normal.z << "\n";
                        std::cout << query_result[i].movable->getName() << " - " << query_result[i].distance << "\n";
                    }

                    distance = query_result[i].distance;
                    foundObstacle = 1;
                    break;
                }

            }
        }


        if ((distance < MAX_DISTANCE) && (foundObstacle)) {
            stop();
        }

    }

    //------------------------------------------------

    void injectKeyDown(const OIS::KeyEvent& evt) {

        mKeyDirection = Vector3::ZERO;
        if (evt.key == OIS::KC_Q && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)) {
            // take swords out (or put them back, since it's the same animation but reversed)
            setTopAnimation(ANIM_DRAW_SWORDS, true);
            mTimer = 0;
        } else if (evt.key == OIS::KC_E && !mSwordsDrawn) {
            if (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP) {
                // start dancing
                setBaseAnimation(ANIM_DANCE, true);
                setTopAnimation(ANIM_NONE);
                // disable hand animation because the dance controls hands
                mAnims[ANIM_HANDS_RELAXED]->setEnabled(false);
            } else if (mBaseAnimID == ANIM_DANCE) {
                // stop dancing
                setBaseAnimation(ANIM_IDLE_BASE);
                setTopAnimation(ANIM_IDLE_TOP);
                // re-enable hand animation
                mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
            }
        }
            // keep track of the player's intended direction
        else if (evt.key == OIS::KC_W) mKeyDirection.z = -1;
        else if (evt.key == OIS::KC_A) {
            mKeyDirection.x = -1;
            if (AUTO_CAMERA) updateCameraGoal(90, 0, 0);
        } else if (evt.key == OIS::KC_S) mKeyDirection.z = 1;
        else if (evt.key == OIS::KC_D) {
            mKeyDirection.x = 1;
            if (AUTO_CAMERA) updateCameraGoal(-90, 0, 0);
        }
        else if (evt.key == OIS::KC_SPACE && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)) {
            // jump if on ground
            setBaseAnimation(ANIM_JUMP_START, true);
            setTopAnimation(ANIM_NONE);
            mTimer = 0;
        }

        if (!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE) {
            // start running if not already moving and the player wants to move
            setBaseAnimation(ANIM_RUN_BASE, true);
            if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
        }
    }

    void injectKeyUp(const OIS::KeyEvent& evt) {

        //keep track of the player's intended direction
        if (evt.key == OIS::KC_W && mKeyDirection.z == -1) mKeyDirection.z = 0;
        else if (evt.key == OIS::KC_A && mKeyDirection.x == -1) mKeyDirection.x = 0;
        else if (evt.key == OIS::KC_S && mKeyDirection.z == 1) mKeyDirection.z = 0;
        else if (evt.key == OIS::KC_D && mKeyDirection.x == 1) mKeyDirection.x = 0;

        //std::cout << "After:" << mKeyDirection.x <<mKeyDirection.z<<"\n";

        if (mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_RUN_BASE) {
            // stop running if already moving and the player doesn't want to move
            setBaseAnimation(ANIM_IDLE_BASE);
            if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
        }
    }

#if OGRE_PLATFORM == OGRE_PLATFORM_IPHONE

    void injectMouseMove(const OIS::MultiTouchEvent& evt) {
        // update camera goal based on mouse movement
        updateCameraGoal(-0.05f * evt.state.X.rel, -0.05f * evt.state.Y.rel, -0.0005f * evt.state.Z.rel);
    }

    void injectMouseDown(const OIS::MultiTouchEvent& evt) {
        if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)) {
            // if swords are out, and character's not doing something weird, then SLICE!
            setTopAnimation(ANIM_SLICE_VERTICAL, true);
            mTimer = 0;
        }
    }
#else

    void injectMouseMove(const OIS::MouseEvent& evt) {
        // update camera goal based on mouse movement
        updateCameraGoal(-0.1f * evt.state.X.rel, -0.1f * evt.state.Y.rel, -0.001f * evt.state.Z.rel);
    }

    void injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id) {
        if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)) {
            // if swords are out, and character's not doing something weird, then SLICE!
            if (id == OIS::MB_Left) setTopAnimation(ANIM_SLICE_VERTICAL, true);
            else if (id == OIS::MB_Right) setTopAnimation(ANIM_SLICE_HORIZONTAL, true);
            mTimer = 0;
        }
    }
#endif

private:

    void setupBody(SceneManager* sceneMgr) {
        // create main model
        mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT);
        mBodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
        mBodyNode->attachObject(mBodyEnt);

        // create swords and attach to sheath
        mSword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
        mSword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
        mBodyEnt->attachObjectToBone("Sheath.L", mSword1);
        mBodyEnt->attachObjectToBone("Sheath.R", mSword2);

        // create a couple of ribbon trails for the swords, just for fun
        NameValuePairList params;
        params["numberOfChains"] = "2";
        params["maxElements"] = "80";
        mSwordTrail = (RibbonTrail*) sceneMgr->createMovableObject("RibbonTrail", &params);
        mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
        mSwordTrail->setTrailLength(20);
        //mSwordTrail->setVisible(false);
        sceneMgr->getRootSceneNode()->attachObject(mSwordTrail);


        for (int i = 0; i < 2; i++) {
            mSwordTrail->setInitialColour(i, 1, 0.8, 0);
            mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
            mSwordTrail->setWidthChange(i, 1);
            mSwordTrail->setInitialWidth(i, 0.5);
        }

        mKeyDirection = Vector3::ZERO;
        mVerticalVelocity = 0;

    }

    void setupAnimations() {
        // this is very important due to the nature of the exported animations
        mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

        String animNames[] ={"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
            "SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

        // populate our animation list
        for (int i = 0; i < NUM_ANIMS; i++) {
            mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
            mAnims[i]->setLoop(true);
            mFadingIn[i] = false;
            mFadingOut[i] = false;
        }

        // start off in the idle state (top and bottom together)
        setBaseAnimation(ANIM_IDLE_BASE);
        setTopAnimation(ANIM_IDLE_TOP);

        // relax the hands since we're not holding anything
        mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

        mSwordsDrawn = false;
    }

    void setupCamera(Camera* cam) {
        // create a pivot at roughly the character's shoulder
        mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
        // this is where the camera should be soon, and it spins around the pivot
        mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0, 0, 15));
        // this is where the camera actually is
        mCameraNode = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
        mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());

        mCameraPivot->setFixedYawAxis(true);
        mCameraGoal->setFixedYawAxis(true);
        mCameraNode->setFixedYawAxis(true);

        // our model is quite small, so reduce the clipping planes
        //cam->setNearClipDistance(0.1);
        cam->setNearClipDistance(0.8);
        //cam->setFarClipDistance(100);
        cam->setFarClipDistance(800);

        mCameraNode->attachObject(cam);

        mPivotPitch = 0;
    }

    void updateBody(Real deltaTime) {
        if (mKeyDirection != Vector3::ZERO && mBaseAnimID != ANIM_DANCE) {
            // calculate actually goal direction in world based on player's key directions
            //mGoalDirection += mKeyDirection.z * mCameraNode->getOrientation().zAxis();
            //mGoalDirection += mKeyDirection.x * mCameraNode->getOrientation().xAxis();
            mGoalDirection.z = mKeyDirection.z;
            mGoalDirection.x = mKeyDirection.x;

            mGoalDirection.y = 0;
            mGoalDirection.normalise();

            Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

            // calculate how much the character has to turn to face goal direction
            Real yawToGoal = toGoal.getYaw().valueDegrees();
            //std::cout <<"Need to turn:" << Degree(yawToGoal) <<"\n";
            // this is how much the character CAN turn this frame
            Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
            // reduce "turnability" if we're in midair
            if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

            // turn as much as we can, but not more than we need to
            if (yawToGoal < 0) yawToGoal = std::min<Real > (0, std::max<Real > (yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
            else if (yawToGoal > 0) yawToGoal = std::max<Real > (0, std::min<Real > (yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);

            mBodyNode->yaw(Degree(yawToGoal));

            if(Degree(yawToGoal) == Degree(0)) {

                if(mNeedPosition) {
                    std::cout << "Turning finished, updating last position \n";
                    mLastPosition = mBodyNode->getPosition();
                    mNeedPosition = 0;
                }
            }

            // move in current body direction (not the goal direction)
            mBodyNode->translate(0, 0, deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(),
                    Node::TS_LOCAL);

        }

        if (mBaseAnimID == ANIM_JUMP_LOOP) {
            // if we're jumping, add a vertical offset too, and apply gravity
            mBodyNode->translate(0, mVerticalVelocity * deltaTime, 0, Node::TS_LOCAL);
            mVerticalVelocity -= GRAVITY * deltaTime;

            Vector3 pos = mBodyNode->getPosition();
            if (pos.y <= CHAR_HEIGHT) {
                // if we've hit the ground, change to landing state
                pos.y = CHAR_HEIGHT;
                mBodyNode->setPosition(pos);
                setBaseAnimation(ANIM_JUMP_END, true);
                mTimer = 0;
            }
        }

    }

    void updateAnimations(Real deltaTime) {
        Real baseAnimSpeed = 1;
        Real topAnimSpeed = 1;

        mTimer += deltaTime;

        if (mTopAnimID == ANIM_DRAW_SWORDS) {
            // flip the draw swords animation if we need to put it back
            topAnimSpeed = mSwordsDrawn ? -1 : 1;

            // half-way through the animation is when the hand grasps the handles...
            if (mTimer >= mAnims[mTopAnimID]->getLength() / 2 &&
                    mTimer - deltaTime < mAnims[mTopAnimID]->getLength() / 2) {
                // so transfer the swords from the sheaths to the hands
                mBodyEnt->detachAllObjectsFromBone();
                mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L", mSword1);
                mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R", mSword2);
                // change the hand state to grab or let go
                mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
                mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);

                // toggle sword trails
                if (mSwordsDrawn) {
                    mSwordTrail->setVisible(false);
                    mSwordTrail->removeNode(mSword1->getParentNode());
                    mSwordTrail->removeNode(mSword2->getParentNode());
                } else {
                    mSwordTrail->setVisible(true);
                    mSwordTrail->addNode(mSword1->getParentNode());
                    mSwordTrail->addNode(mSword2->getParentNode());
                }
            }

            if (mTimer >= mAnims[mTopAnimID]->getLength()) {
                // animation is finished, so return to what we were doing before
                if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
                else {
                    setTopAnimation(ANIM_RUN_TOP);
                    mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
                }
                mSwordsDrawn = !mSwordsDrawn;
            }
        } else if (mTopAnimID == ANIM_SLICE_VERTICAL || mTopAnimID == ANIM_SLICE_HORIZONTAL) {
            if (mTimer >= mAnims[mTopAnimID]->getLength()) {
                // animation is finished, so return to what we were doing before
                if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
                else {
                    setTopAnimation(ANIM_RUN_TOP);
                    mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
                }
            }

            // don't sway hips from side to side when slicing. that's just embarrasing.
            if (mBaseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
        } else if (mBaseAnimID == ANIM_JUMP_START) {
            if (mTimer >= mAnims[mBaseAnimID]->getLength()) {
                // takeoff animation finished, so time to leave the ground!
                setBaseAnimation(ANIM_JUMP_LOOP, true);
                // apply a jump acceleration to the character
                mVerticalVelocity = JUMP_ACCEL;
            }
        } else if (mBaseAnimID == ANIM_JUMP_END) {
            if (mTimer >= mAnims[mBaseAnimID]->getLength()) {
                // safely landed, so go back to running or idling
                if (mKeyDirection == Vector3::ZERO) {
                    setBaseAnimation(ANIM_IDLE_BASE);
                    setTopAnimation(ANIM_IDLE_TOP);
                } else {
                    setBaseAnimation(ANIM_RUN_BASE, true);
                    setTopAnimation(ANIM_RUN_TOP, true);
                }
            }
        }

        // increment the current base and top animation times
        if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
        if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

        // apply smooth transitioning between our animations
        fadeAnimations(deltaTime);
    }

    void fadeAnimations(Real deltaTime) {
        for (int i = 0; i < NUM_ANIMS; i++) {
            if (mFadingIn[i]) {
                // slowly fade this animation in until it has full weight
                Real newWeight = mAnims[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
                mAnims[i]->setWeight(Math::Clamp<Real > (newWeight, 0, 1));
                if (newWeight >= 1) mFadingIn[i] = false;
            } else if (mFadingOut[i]) {
                // slowly fade this animation out until it has no weight, and then disable it
                Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
                mAnims[i]->setWeight(Math::Clamp<Real > (newWeight, 0, 1));
                if (newWeight <= 0) {
                    mAnims[i]->setEnabled(false);
                    mFadingOut[i] = false;
                }
            }
        }
    }

    void updateCamera(Real deltaTime) {
        // place the camera pivot roughly at the character's shoulder
        mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3::UNIT_Y * CAM_HEIGHT);
        // move the camera smoothly to the goal
        Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
        mCameraNode->translate(goalOffset * deltaTime * 9.0f);
        // always look at the pivot
        mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Node::TS_WORLD);
    }

    void updateCameraGoal(Real deltaYaw, Real deltaPitch, Real deltaZoom) {
        mCameraPivot->yaw(Degree(deltaYaw), Node::TS_WORLD);

        // bound the pitch
        if (!(mPivotPitch + deltaPitch > 25 && deltaPitch > 0) &&
                !(mPivotPitch + deltaPitch < -60 && deltaPitch < 0)) {
            mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
            mPivotPitch += deltaPitch;
        }

        Real dist = mCameraGoal->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());
        Real distChange = deltaZoom * dist;

        // bound the zoom
        if (!(dist + distChange < 8 && distChange < 0) &&
                !(dist + distChange > 105 && distChange > 0)) {
            mCameraGoal->translate(0, 0, distChange, Node::TS_LOCAL);
        }
    }

    void setBaseAnimation(AnimID id, bool reset = false) {
        if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS) {
            // if we have an old animation, fade it out
            mFadingIn[mBaseAnimID] = false;
            mFadingOut[mBaseAnimID] = true;
        }

        mBaseAnimID = id;

        if (id != ANIM_NONE) {
            // if we have a new animation, enable it and fade it in
            mAnims[id]->setEnabled(true);
            mAnims[id]->setWeight(0);
            mFadingOut[id] = false;
            mFadingIn[id] = true;
            if (reset) mAnims[id]->setTimePosition(0);
        }
    }

    void setTopAnimation(AnimID id, bool reset = false) {
        if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS) {
            // if we have an old animation, fade it out
            mFadingIn[mTopAnimID] = false;
            mFadingOut[mTopAnimID] = true;
        }

        mTopAnimID = id;

        if (id != ANIM_NONE) {
            // if we have a new animation, enable it and fade it in
            mAnims[id]->setEnabled(true);
            mAnims[id]->setWeight(0);
            mFadingOut[id] = false;
            mFadingIn[id] = true;
            if (reset) mAnims[id]->setTimePosition(0);
        }
    }

    // Members

    Camera* mCamera;
    SceneManager* mSceneMgr;
    SceneNode* mBodyNode;
    SceneNode* mCameraPivot;
    SceneNode* mCameraGoal;
    SceneNode* mCameraNode;
    Real mPivotPitch;
    Entity* mBodyEnt;
    Entity* mSword1;
    Entity* mSword2;
    RibbonTrail* mSwordTrail;
    AnimationState* mAnims[NUM_ANIMS]; // master animation list
    AnimID mBaseAnimID; // current base (full- or lower-body) animation
    AnimID mTopAnimID; // current top (upper-body) animation
    bool mFadingIn[NUM_ANIMS]; // which animations are fading in
    bool mFadingOut[NUM_ANIMS]; // which animations are fading out
    bool mSwordsDrawn;
    Vector3 mKeyDirection; // player's local intended direction based on WASD keys
    Vector3 mGoalDirection; // actual intended direction in world-space
    Real mVerticalVelocity; // for jumping
    Real mTimer; // general timer to see how long animations have been playing



    std::vector<PathFinder::Direction>* mdirections;
    int mdirectionIndex;
    bool mfinished;
    bool mkeptDirection;
    Vector3 mLastPosition;
    bool mNeedPosition;

};

#endif
