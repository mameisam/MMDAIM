/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2010  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                2010-2011  hkrn (libMMDAI)                         */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the MMDAI project team nor the names of     */
/*   its contributors may be used to endorse or promote products     */
/*   derived from this software without specific prior written       */
/*   permission.                                                     */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

/* headers */

#include "MMDAI/SceneController.h"

namespace MMDAI {

/* command names */
const char *SceneEventHandler::kModelAddCommand = "MODEL_ADD";
const char *SceneEventHandler::kModelChangeCommand = "MODEL_CHANGE";
const char *SceneEventHandler::kModelDeleteCommand = "MODEL_DELETE";
const char *SceneEventHandler::kMotionAddCommand = "MOTION_ADD";
const char *SceneEventHandler::kMotionChangeCommand = "MOTION_CHANGE";
const char *SceneEventHandler::kMotionDeleteCommand = "MOTION_DELETE";
const char *SceneEventHandler::kMoveStartCommand = "MOVE_START";
const char *SceneEventHandler::kMoveStopCommand = "MOVE_STOP";
const char *SceneEventHandler::kTurnStartCommand = "TURN_START";
const char *SceneEventHandler::kTurnStopCommand = "TURN_STOP";
const char *SceneEventHandler::kRotateStartCommand = "ROTATE_START";
const char *SceneEventHandler::kRotateStopCommand = "ROTATE_STOP";
// const char *SceneEventHandler::kSoundStartCommand = "SOUND_START";
// const char *SceneEventHandler::kSoundStopCommand = "SOUND_STOP";
const char *SceneEventHandler::kStageCommand = "STAGE";
const char *SceneEventHandler::kFloorCommand = "FLOOR";
const char *SceneEventHandler::kBackgroundCommand = "BACKGROUND";
const char *SceneEventHandler::kLightColorCommand = "LIGHTCOLOR";
const char *SceneEventHandler::kLightDirectionCommand = "LIGHTDIRECTION";
const char *SceneEventHandler::kLipSyncStartCommand = "LIPSYNC_START";
const char *SceneEventHandler::kLipSyncStopCommand = "LIPSYNC_STOP";

const char *SceneEventHandler::kModelAddEvent = "MODEL_EVENT_ADD";
const char *SceneEventHandler::kModelChangeEvent = "MODEL_EVENT_CHANGE";
const char *SceneEventHandler::kModelDeleteEvent = "MODEL_EVENT_DELETE";
const char *SceneEventHandler::kMotionAddEvent = "MOTION_EVENT_ADD";
const char *SceneEventHandler::kMotionDeleteEvent = "MOTION_EVENT_DELETE";
const char *SceneEventHandler::kMotionChangeEvent = "MOTION_EVENT_CHANGE";
const char *SceneEventHandler::kMotionLoopEvent = "MOTION_EVENT_LOOP";
const char *SceneEventHandler::kMoveStartEvent = "MOVE_EVENT_START";
const char *SceneEventHandler::kMoveStopEvent = "MOVE_EVENT_STOP";
const char *SceneEventHandler::kTurnStartEvent = "TURN_EVENT_START";
const char *SceneEventHandler::kTurnStopEvent = "TURN_EVENT_STOP";
const char *SceneEventHandler::kRotateStartEvent = "ROTATE_EVENT_START";
const char *SceneEventHandler::kRotateStopEvent = "ROTATE_EVENT_STOP";
// const char *SceneEventHandler::kSoundStartEvent = "SOUND_EVENT_START";
// const char *SceneEventHandler::kSoundStopEvent = "SOUND_EVENT_STOP";
const char *SceneEventHandler::kStageEvent = "STAGE";
const char *SceneEventHandler::kFloorEvent = "FLOOR";
const char *SceneEventHandler::kBackgroundEvent = "BACKGROUND";
const char *SceneEventHandler::kLightColorEvent = "LIGHTCOLOR";
const char *SceneEventHandler::kLightDirectionEvent = "LIGHTDIRECTION";
const char *SceneEventHandler::kLipSyncStartEvent = "LIPSYNC_EVENT_START";
const char *SceneEventHandler::kLipSyncStopEvent = "LIPSYNC_EVENT_STOP";
const char *SceneEventHandler::kKeyEvent = "KEY";

// from util.h
static int getNumDigit(int in)
{
  int out = 0;
  if(in == 0)
    return 1;
  if (in < 0) {
    out = 1;
    in *= -1;
  }
  for (; in != 0; out++)
    in /= 10;
  return out;
}

SceneController::SceneController(SceneEventHandler *handler)
  : m_highlightModel(0),
    m_handler(handler),
    m_text(NULL),
    m_numModel(0),
    m_selectedModel(-1),
    m_enablePhysicsSimulation(true)
{
}

SceneController::~SceneController()
{
  for (int i = 0; i < m_numModel; i++) {
    m_objects[i].release();
  }
}

void SceneController::updateLight()
{
  int i = 0;
  float *f;
  btVector3 l;
  m_scene.updateLighting(m_option.getUseCartoonRendering(),
                         m_option.getUseMMDLikeCartoon(),
                         m_option.getLightDirection(),
                         m_option.getLightIntensity(),
                         m_option.getLightColor());
  f = m_option.getLightDirection();
  m_stage.updateShadowMatrix(f);
  l = btVector3(f[0], f[1], f[2]);
  for (i = 0; i < m_numModel; i++) {
    PMDObject *object = &m_objects[i];
    if (object->isEnable())
      object->setLightForToon(&l);
  }
}

/* SceneController::setFloor: set floor image */
bool SceneController::loadFloor(PMDModelLoader *loader)
{
  /* load floor */
  const char *fileName = loader->getLocation();
  if (fileName == NULL)
    return false;
  if (m_stage.loadFloor(loader, &m_bullet) == false) {
    MMDAILogWarn("cannot set floor %s.", fileName);
    return false;
  }

  /* send event message */
  sendEvent1(SceneEventHandler::kFloorEvent, fileName);

  return true;
}

/* SceneController::setBackground: set background image */
bool SceneController::loadBackground(PMDModelLoader *loader)
{
  /* load background */
  const char *fileName = loader->getLocation();
  if (fileName == NULL)
    return false;
  if (m_stage.loadBackground(loader, &m_bullet) == false) {
    MMDAILogWarn("cannot set background %s.", fileName);
    return false;
  }

  /* send event message */
  sendEvent1(SceneEventHandler::kBackgroundEvent, fileName);

  return true;
}

/* SceneController::setStage: set stage */
bool SceneController::loadStage(PMDModelLoader *loader)
{
  /* load stage */
  const char *fileName = loader->getLocation();
  if (fileName == NULL)
    return false;
  if (m_stage.loadStagePMD(loader, &m_bullet) == false) {
    MMDAILogWarn("cannot set stage %s.", fileName);
    return false;
  }

  /* send event message */
  sendEvent1(SceneEventHandler::kStageEvent, fileName);

  return true;
}

/* SceneController::getNewModelId: return new model ID */
PMDObject *SceneController::allocatePMDObject()
{
  PMDObject *object = NULL;
  for (int i = 0; i < m_numModel; i++) {
    PMDObject *object = &m_objects[i];
    if (!object->isEnable())
      return object; /* re-use it */
  }
  if (m_numModel >= MAX_MODEL)
    return NULL; /* no more room */
  object = &m_objects[m_numModel];
  m_numModel++;
  object->setEnableFlag(false); /* model is not loaded yet */
  return object;
}

PMDObject *SceneController::findPMDObject(PMDObject *object)
{
  return findPMDObject(object->getAlias());
}

PMDObject *SceneController::findPMDObject(const char *alias)
{
  for (int i = 0; i < m_numModel; i++) {
    PMDObject *object = &m_objects[i];
    if (object->isEnable() && MMDAIStringEquals(object->getAlias(), alias))
      return object;
  }
  return NULL;
}

PMDObject *SceneController::getPMDObject(int index)
{
  if (index < 0 || index > m_numModel)
    return NULL;
  return &m_objects[index];
}

int SceneController::countPMDObjects() const
{
  return m_numModel;
}

/* SceneController::addMotion: add motion */
bool SceneController::addMotion(PMDObject *object, VMDLoader *loader)
{
  return addMotion(object, NULL, loader, false, true, true, true);
}

/* SceneController::addMotion: add motion */
bool SceneController::addMotion(PMDObject *object,
                                const char *motionAlias,
                                VMDLoader *loader,
                                bool full,
                                bool once,
                                bool enableSmooth,
                                bool enableRePos)
{
  int i;
  bool find;
  VMD *vmd;
  MotionPlayer *motionPlayer;
  char *name;
  size_t allocSize;

  /* motion file */
  vmd = m_motion.loadFromLoader(loader);
  if (vmd == NULL) {
    MMDAILogWarn("addMotion: failed to load %s.", loader->getLocation());
    return false;
  }

  /* alias */
  if (motionAlias && MMDAIStringLength(motionAlias) > 0) {
    /* check the same alias */
    name = MMDAIStringClone(motionAlias);
    motionPlayer = object->getMotionManager()->getMotionPlayerList();
    for (; motionPlayer != NULL; motionPlayer = motionPlayer->next) {
      if (motionPlayer->active && MMDAIStringEquals(motionPlayer->name, name)) {
        MMDAILogWarn("addMotion: motion alias \"%s\" is already used.", name);
        MMDAIMemoryRelease(name);
        return false;
      }
    }
  } else {
    /* if motion alias is not specified, unused digit is used */
    for (i = 0;; i++) {
      find = false;
      allocSize = sizeof(char) * (getNumDigit(i) + 1);
      name = static_cast<char *>(MMDAIMemoryAllocate(allocSize));
      if (name == NULL)
        return false;
      MMDAIStringFormat(name, allocSize, "%d", i);
      motionPlayer = object->getMotionManager()->getMotionPlayerList();
      for (; motionPlayer != NULL; motionPlayer = motionPlayer->next) {
        if (motionPlayer->active && MMDAIStringEquals(motionPlayer->name, name)) {
          find = true;
          break;
        }
      }
      if(find == false)
        break;
      MMDAIMemoryRelease(name);
    }
  }

  /* start motion */
  if (!object->startMotion(vmd, name, full, once, enableSmooth, enableRePos)) {
    MMDAIMemoryRelease(name);
    return false;
  }

  /* send event message */
  sendEvent2(SceneEventHandler::kMotionAddEvent, object->getAlias(), name);

  MMDAIMemoryRelease(name);
  return true;
}

/* SceneController::changeMotion: change motion */
bool SceneController::changeMotion(PMDObject *object, const char *motionAlias, VMDLoader *loader)
{
  VMD *vmd, *old = NULL;
  MotionPlayer *motionPlayer;

  /* check */
  if (!motionAlias) {
    MMDAILogWarn("changeMotion: not specified %s.", motionAlias);
    return false;
  }

  /* motion file */
  vmd = m_motion.loadFromLoader(loader);
  if (vmd == NULL) {
    MMDAILogWarn("changeMotion: failed to load %s.", loader->getLocation());
    return false;
  }

  /* get motion before change */
  motionPlayer = object->getMotionManager()->getMotionPlayerList();
  for (; motionPlayer != NULL; motionPlayer = motionPlayer->next) {
    if (motionPlayer->active && MMDAIStringEquals(motionPlayer->name, motionAlias)) {
      old = motionPlayer->vmd;
      break;
    }
  }
  if (old == NULL) {
    MMDAILogWarn("changeMotion: motion alias \"%s\"is not found.", motionAlias);
    m_motion.unload(vmd);
    return false;
  }

  /* change motion */
  if (object->swapMotion(vmd, motionAlias) == false) {
    MMDAILogWarn("changeMotion: motion alias \"%s\"is not found.", motionAlias);
    m_motion.unload(vmd);
    return false;
  }

  /* send event message */
  sendEvent2(SceneEventHandler::kMotionChangeEvent, object->getAlias(), motionAlias);

  /* unload old motion from motion stocker */
  m_motion.unload(old);

  return true;
}

/* SceneController::deleteMotion: delete motion */
bool SceneController::deleteMotion(PMDObject *object, const char *motionAlias)
{
  /* delete motion */
  if (!object->getMotionManager()->deleteMotion(motionAlias)) {
    MMDAILogWarn("deleteMotion: motion alias \"%s\"is not found.", motionAlias);
    return false;
  }

  /* don't send event message */
  return true;
}

/* SceneController::addModel: add model */
bool SceneController::addModel(PMDModelLoader *modelLoader, LipSyncLoader *lipSyncLoader)
{
  return addModel(NULL, modelLoader, lipSyncLoader, NULL, NULL, NULL, NULL);
}

/* SceneController::addModel: add model */
bool SceneController::addModel(const char *modelAlias,
                               PMDModelLoader *modelLoader,
                               LipSyncLoader *lipSyncLoader,
                               btVector3 *pos,
                               btQuaternion *rot,
                               const char *baseModelAlias,
                               const char *baseBoneName)
{
  int i;
  char *name;
  btVector3 offsetPos = btVector3(0.0f, 0.0f, 0.0f);
  btQuaternion offsetRot = btQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  bool forcedPosition = false;
  PMDBone *assignBone = NULL;
  PMDObject *assignObject = NULL, *newObject = NULL;
  float *l = m_option.getLightDirection();
  btVector3 light = btVector3(l[0], l[1], l[2]);

  /* set */
  if (pos)
    offsetPos = (*pos);
  if (rot)
    offsetRot = (*rot);
  if (pos || rot)
    forcedPosition = true;
  if (baseModelAlias) {
    assignObject = findPMDObject(baseModelAlias);
    if (assignObject == NULL) {
      MMDAILogWarn("model alias \"%s\" is not found", baseModelAlias);
      return false;
    }
    PMDModel *model = assignObject->getPMDModel();
    if (baseBoneName) {
      assignBone = model->getBone(baseBoneName);
    } else {
      assignBone = model->getCenterBone();
    }
    if (assignBone == NULL) {
      if (baseBoneName)
        MMDAILogWarn("bone \"%s\" is not exist on %s.", baseBoneName, baseModelAlias);
      else
        MMDAILogWarn("cannot assign to bone of %s.", baseModelAlias);
      return false;
    }
  }

  /* ID */
  newObject = allocatePMDObject();
  if (newObject == NULL) {
    MMDAILogWarnString("addModel: too many models.");
    return false;
  }

  /* determine name */
  if (modelAlias && MMDAIStringLength(modelAlias) > 0) {
    /* check the same alias */
    name = MMDAIStringClone(modelAlias);
    if (findPMDObject(name) != NULL) {
      MMDAILogWarn("addModel: model alias \"%s\" is already used.", name);
      MMDAIMemoryRelease(name);
      return false;
    }
  } else {
    /* if model alias is not specified, unused digit is used */
    for(i = 0;; i++) {
      size_t allocSize = sizeof(char) * (getNumDigit(i) + 1);
      name = static_cast<char *>(MMDAIMemoryAllocate(allocSize));
      if (name == NULL)
        return false;
      MMDAIStringFormat(name, allocSize, "%d", i);
      if (findPMDObject(name) != NULL)
        MMDAIMemoryRelease(name);
      else
        break;
    }
  }

  /* add model */
  if (!newObject->load(modelLoader,
                       lipSyncLoader,
                       &offsetPos,
                       &offsetRot,
                       forcedPosition,
                       assignBone,
                       assignObject,
                       &m_bullet,
                       m_option.getUseCartoonRendering(),
                       m_option.getCartoonEdgeWidth(),
                       &light)) {
    MMDAILogWarn("addModel: failed to load %s.", modelLoader->getLocation());
    newObject->release();
    MMDAIMemoryRelease(name);
    return false;
  }

  /* initialize motion manager */
  newObject->resetMotionManager();
  newObject->setAlias(name);
  m_numModel++;

  /* send event message */
  sendEvent1(SceneEventHandler::kModelAddEvent, name);

  MMDAIMemoryRelease(name);
  return true;
}

/* SceneController::changeModel: change model */
bool SceneController::changeModel(PMDObject *object,
                                  PMDModelLoader *modelLoader,
                                  LipSyncLoader *lipSyncLoader)
{
  int i;
  MotionPlayer *motionPlayer;
  double currentFrame;
  float *l = m_option.getLightDirection();
  const char *modelAlias = object->getAlias();
  btVector3 light = btVector3(l[0], l[1], l[2]);

  /* load model */
  if (!object->load(modelLoader,
                    lipSyncLoader,
                    NULL,
                    NULL,
                    false,
                    NULL,
                    NULL,
                    &m_bullet,
                    m_option.getUseCartoonRendering(),
                    m_option.getCartoonEdgeWidth(),
                    &light)) {
    MMDAILogWarn("changeModel: failed to load model %s.", modelLoader->getLocation());
    return false;
  }

  /* update motion manager */
  MotionManager *manager = object->getMotionManager();
  if (manager != NULL) {
    motionPlayer = manager->getMotionPlayerList();
    for (; motionPlayer != NULL; motionPlayer = motionPlayer->next) {
      if (motionPlayer->active) {
        currentFrame = motionPlayer->mc.getCurrentFrame();
        manager->startMotionSub(motionPlayer->vmd, motionPlayer);
        motionPlayer->mc.setCurrentFrame(currentFrame);
      }
    }
  }

  /* set alias */
  object->setAlias(modelAlias);

  /* delete accessories  */
  for (i = 0; i < MAX_MODEL; i++) {
    PMDObject *assoc = &m_objects[i];
    if (assoc->isEnable() && assoc->getAssignedModel() == object)
      deleteModel(assoc);
  }

  /* send event message */
  sendEvent1(SceneEventHandler::kModelChangeEvent, modelAlias);

  return true;
}

/* SceneController::deleteModel: delete model */
void SceneController::deleteModel(PMDObject *object)
{
  /* delete accessories  */
  for (int i = 0; i < MAX_MODEL; i++) {
    PMDObject *assoc = &m_objects[i];
    if (assoc->isEnable() && assoc->getAssignedModel() == object)
      deleteModel(assoc);
  }

  /* set frame from now to disappear */
  object->startDisappear();
  m_numModel--;

  /* send event message */
  sendEvent1(SceneEventHandler::kModelDeleteEvent, object->getAlias());
}

/* SceneController::changeLightDirection: change light direction */
void SceneController::changeLightDirection(float x, float y, float z)
{
  float f[4];

  f[0] = x;
  f[1] = y;
  f[2] = z;
  f[3] = 0.0f;
  m_option.setLightDirection(f);
  updateLight();

  /* send event message */
  if (m_handler != NULL) {
    char buf[BUFSIZ];
    MMDAIStringFormat(buf, sizeof(BUFSIZ), "%.2f,%.2f,%.2f", x, y, z);
    sendEvent1(SceneEventHandler::kLightDirectionEvent, buf);
  }
}

/* SceneController::changeLightColor: change light color */
void SceneController::changeLightColor(float r, float g, float b)
{
  float f[3];

  f[0] = r;
  f[1] = g;
  f[2] = b;
  m_option.setLightColor(f);
  updateLight();

  /* send event message */
  if (m_handler != NULL) {
    char buf[BUFSIZ];
    MMDAIStringFormat(buf, sizeof(BUFSIZ), "%.2f,%.2f,%.2f", r, g, b);
    sendEvent1(SceneEventHandler::kLightColorEvent, buf);
  }
}

/* SceneController::startMove: start moving */
void SceneController::startMove(PMDObject *object, btVector3 *pos, bool local, float speed)
{
  btVector3 currentPos;
  btQuaternion currentRot;
  btVector3 targetPos;
  btTransform tr;

  /* set */
  object->getPosition(currentPos);
  targetPos = (*pos);

  /* local or global */
  if (local) {
    object->getRotation(currentRot);
    tr = btTransform(currentRot, currentPos);
    targetPos = tr * targetPos;
  }

  /* not need to start */
  if (currentPos == targetPos)
    return;

  object->setMoveSpeed(speed);
  object->setPosition(targetPos);

  /* send event message */
  sendEvent1(SceneEventHandler::kMoveStartEvent, object->getAlias());
}

/* SceneController::stopMove: stop moving */
void SceneController::stopMove(PMDObject *object)
{
  btVector3 targetPos;
  btVector3 currentPos;

  /* set */
  targetPos = (*(object->getPMDModel()->getRootBone()->getOffset()));
  object->getPosition(currentPos);

  /* not need to stop */
  if (currentPos == targetPos)
    return;

  object->setPosition(targetPos);

  /* send event message */
  sendEvent1(SceneEventHandler::kMoveStopEvent, object->getAlias());
}

/* SceneController::startRotation: start rotation */
void SceneController::startRotation(PMDObject *object, btQuaternion *rot, bool local, float speed)
{
  btQuaternion targetRot;
  btQuaternion currentRot;

  /* set */
  object->getRotation(currentRot);
  targetRot = (*rot);

  /* local or global */
  if (local)
    targetRot += currentRot;

  /* not need to start */
  if (currentRot == targetRot)
    return;

  object->setSpinSpeed(speed);
  object->setRotation(targetRot);

  /* send event message */
  sendEvent1(SceneEventHandler::kRotateStartEvent, object->getAlias());
}

/* SceneController::stopRotation: stop rotation */
void SceneController::stopRotation(PMDObject *object)
{
  btQuaternion currentRot;
  btQuaternion targetRot;

  /* set */
  targetRot = (*(object->getPMDModel()->getRootBone()->getCurrentRotation()));
  object->getRotation(currentRot);

  /* not need to rotate */
  if (currentRot == targetRot)
    return;

  object->setRotation(targetRot);

  /* send event message */
  sendEvent1(SceneEventHandler::kRotateStopEvent, object->getAlias());
}

/* SceneController::startTurn: start turn */
void SceneController::startTurn(PMDObject *object, btVector3 *pos, bool local, float speed)
{
  btVector3 currentPos;
  btQuaternion currentRot;
  btVector3 targetPos;
  btQuaternion targetRot;
  btTransform tr;

  btVector3 diffPos;
  float z, rad;

  /* set */
  targetPos = (*pos);
  object->getRotation(currentRot);

  /* local or global */
  if (local) {
    object->getPosition(currentPos);
    tr = btTransform(currentRot, currentPos);
    targetPos = tr * targetPos;
  }

  /* get vector from current position to target position */
  diffPos = (*(object->getPMDModel()->getRootBone()->getOffset()));
  diffPos = targetPos - diffPos;
  diffPos.normalize();

  z = diffPos.z();
  if (z > 1.0f) z = 1.0f;
  if (z < -1.0f) z = -1.0f;
  rad = acosf(z);
  if (diffPos.x() < 0.0f) rad = -rad;
  targetRot.setEulerZYX(0, rad, 0);

  /* not need to turn */
  if (currentRot == targetRot)
    return;

  object->setSpinSpeed(speed);
  object->setRotation(targetRot);
  object->setTurningFlag(true);

  /* send event message */
  sendEvent1(SceneEventHandler::kTurnStartEvent, object->getAlias());
}

/* SceneController::stopTurn: stop turn */
void SceneController::stopTurn(PMDObject *object)
{
  btQuaternion currentRot;
  btQuaternion targetRot;

  /* not need to stop turn */
  if (!object->isTurning())
    return;

  /* set */
  targetRot = (*(object->getPMDModel()->getRootBone()->getCurrentRotation()));
  object->getRotation(currentRot);

  /* not need to turn */
  if (currentRot == targetRot)
    return;

  object->setRotation(targetRot);

  /* send event message */
  sendEvent1(SceneEventHandler::kTurnStopEvent, object->getAlias());
}

/* SceneController::startLipSync: start lip sync */
bool SceneController::startLipSync(PMDObject *object, const char *seq)
{
  unsigned char *vmdData;
  size_t vmdSize;
  VMD *vmd;
  bool found = false;
  MotionPlayer *motionPlayer;
  const char *name = LipSync::getMotionName();

  /* create motion */
  if(object->createLipSyncMotion(seq, &vmdData, &vmdSize) == false) {
    MMDAILogWarnString("cannot create lip motion.");
    return false;
  }
  vmd = m_motion.loadFromData(vmdData, vmdSize);
  MMDAIMemoryRelease(vmdData);

  /* search running lip motion */
  motionPlayer = object->getMotionManager()->getMotionPlayerList();
  for (; motionPlayer != NULL; motionPlayer = motionPlayer->next) {
    if (motionPlayer->active && MMDAIStringEquals(motionPlayer->name, name)) {
      found = true;
      break;
    }
  }

  /* start lip sync */
  if (found) {
    if (!object->swapMotion(vmd, name)) {
      MMDAILogWarnString("cannot start lip sync.");
      m_motion.unload(vmd);
      return false;
    }
  } else {
    if (!object->startMotion(vmd, name, false, true, true, true)) {
      MMDAILogWarnString("cannot start lip sync.");
      m_motion.unload(vmd);
      return false;
    }
  }

  /* send event message */
  sendEvent1(SceneEventHandler::kLipSyncStartEvent, object->getAlias());

  return true;
}

/* SceneController::stopLipSync: stop lip sync */
bool SceneController::stopLipSync(PMDObject *object)
{
  /* stop lip sync */
  if (object->getMotionManager()->deleteMotion(LipSync::getMotionName()) == false) {
    MMDAILogWarnString("lipsync motion not found");
    return false;
  }

  /* don't send message */
  return true;
}

void SceneController::init(int *size)
{
  m_bullet.setup(m_option.getBulletFps());
  m_scene.setup(size,
                m_option.getCampusColor(),
                m_option.getUseShadowMapping(),
                m_option.getShadowMappingTextureSize(),
                m_option.getShadowMappingLightFirst());
  m_stage.setSize(m_option.getStageSize(), 1.0f, 1.0f);
}

void SceneController::getScreenPointPosition(btVector3 *dst, btVector3 *src)
{
  m_scene.getScreenPointPosition(dst, src);
}

void SceneController::setShadowMapping(bool value)
{
  m_scene.setShadowMapping(value, m_option.getShadowMappingTextureSize(), m_option.getShadowMappingLightFirst());
}

float SceneController::getScale()
{
  return m_scene.getScale();
}

void SceneController::setScale(float value)
{
  m_scene.setScale(value);
}

void SceneController::rotate(float x, float y, float z)
{
  m_scene.rotate(x, y, z);
}

void SceneController::translate(float x, float y, float z)
{
  m_scene.translate(x, y, z);
}

void SceneController::setRect(int width, int height)
{
  m_scene.setSize(width, height);
}

void SceneController::selectPMDObject(PMDObject *object)
{
  const char *alias = object->getAlias();
  for (int i = 0; i < m_numModel; i++) {
    PMDObject *o = &m_objects[i];
    if (o->isEnable() && MMDAIStringEquals(o->getAlias(), alias)) {
      m_selectedModel = i;
      break;
    }
  }
}

void SceneController::selectPMDObject(int x, int y)
{
  m_selectedModel = m_scene.pickModel(m_objects, m_numModel, x, y, NULL);
}

void SceneController::selectPMDObject(int x, int y, PMDObject **dropAllowedModel)
{
  int dropAllowedModelID = -1;
  m_selectedModel = m_scene.pickModel(m_objects, m_numModel, x, y, &dropAllowedModelID);
  if (m_selectedModel == -1)
    *dropAllowedModel = getPMDObject(dropAllowedModelID);
}

void SceneController::setHighlightPMDObject(PMDObject *object)
{
  float col[4];

  if (m_highlightModel != NULL) {
    /* reset current highlighted model */
    col[0] = PMDModel::kEdgeColorR;
    col[1] = PMDModel::kEdgeColorG;
    col[2] = PMDModel::kEdgeColorB;
    col[3] = PMDModel::kEdgeColorA;
    m_highlightModel->getPMDModel()->setEdgeColor(col);
  }
  if (object != NULL) {
    /* set highlight to the specified model */
    col[0] = 1.0f;
    col[1] = col[2] = 0.0f;
    col[3] = 1.0f;
    object->getPMDModel()->setEdgeColor(col);
  }

  m_highlightModel = object;
}

PMDObject *SceneController::getSelectedPMDObject()
{
  return getPMDObject(m_selectedModel);
}

void SceneController::updateMotion(double procFrame, double adjustFrame)
{
  const char *lipSyncMotion = LipSync::getMotionName();
  for (int i = 0; i < m_numModel; i++) {
    PMDObject *object = &m_objects[i];
    if (object->isEnable()) {
      object->updateRootBone();
      if (object->updateMotion(procFrame + adjustFrame)) {
        MotionPlayer *player = object->getMotionManager()->getMotionPlayerList();
        for (; player != NULL; player = player->next) {
          if (player->statusFlag == MOTION_STATUS_DELETED) {
            if (MMDAIStringEquals(player->name, lipSyncMotion)) {
              sendEvent1(SceneEventHandler::kLipSyncStopEvent, object->getAlias());
            }
            else {
              sendEvent2(SceneEventHandler::kMotionDeleteEvent, object->getAlias(), player->name);
            }
            m_motion.unload(player->vmd);
          }
        }
      }
      if (object->updateAlpha(procFrame + adjustFrame)) {
        deleteAssociatedModels(object);
      }
    }
  }
  m_bullet.update((float)procFrame);
}

void SceneController::deleteAssociatedModels(PMDObject *object)
{
  /* remove assigned accessories */
  for (int i = 0; i < MAX_MODEL; i++) {
    PMDObject *assoc = &m_objects[i];
    if (assoc->isEnable() && assoc->getAssignedModel() == object)
      deleteAssociatedModels(assoc);
  }
  const char *lipSyncMotion = LipSync::getMotionName();
  MotionPlayer *player = object->getMotionManager()->getMotionPlayerList();
  for (; player != NULL; player = player->next) {
    if (MMDAIStringEquals(player->name, lipSyncMotion)) {
      sendEvent1(SceneEventHandler::kLipSyncStopEvent, object->getAlias());
    }
    else {
      sendEvent2(SceneEventHandler::kMotionDeleteEvent, object->getAlias(), player->name);
    }
    m_motion.unload(player->vmd);
  }
  /* remove model */
  object->release();
}

void SceneController::updateAfterSimulation()
{
  /* update after simulation */
  for (int i = 0; i < m_numModel; i++) {
    PMDObject *object = &m_objects[i];
    object->updateAfterSimulation(m_enablePhysicsSimulation);
  }
}

void SceneController::updateDepthTextureViewParam()
{
  /* calculate rendering range for shadow mapping */
  if (m_option.getUseShadowMapping())
    m_scene.updateDepthTextureViewParam(m_objects, m_numModel);
}

void SceneController::updateModelPositionAndRotation(double fps)
{
  for (int i = 0; i < m_numModel; i++) {
    PMDObject *object = &m_objects[i];
    if (object->isEnable()) {
      if (object->updateModelRootOffset(fps)) {
        sendEvent1(SceneEventHandler::kMoveStopEvent, object->getAlias());
      }
      if (object->updateModelRootRotation(fps)) {
        if (object->isTurning()) {
          sendEvent1(SceneEventHandler::kTurnStopEvent, object->getAlias());
          object->setTurningFlag(false);
        } else {
          sendEvent1(SceneEventHandler::kRotateStopEvent, object->getAlias());
        }
      }
    }
  }
}

inline void SceneController::sendEvent1(const char *type, const char *arg1)
{
  if (m_handler != NULL) {
    m_handler->handleEventMessage(type, 1, arg1);
  }
}

inline void SceneController::sendEvent2(const char *type, const char *arg1, const char *arg2)
{
  if (m_handler != NULL) {
    m_handler->handleEventMessage(type, 2, arg1, arg2);
  }
}

void SceneController::renderScene()
{
  m_scene.render(&m_option, &m_stage, m_objects, m_numModel);
}

void SceneController::renderBulletForDebug()
{
  m_bullet.debugDisplay();
}

void SceneController::renderPMDObjectsForDebug()
{
  for (int i = 0; i < m_numModel; i++) {
    PMDObject *object = &m_objects[i];
    if (object->isEnable()) {
      object->renderDebug(m_text);
    }
  }
}

void SceneController::renderLogger()
{
  /* g_logger.render(m_text); */
}

Option *SceneController::getOption()
{
  return &m_option;
}

Stage *SceneController::getStage()
{
  return &m_stage;
}

int SceneController::getWidth()
{
  return m_scene.getWidth();
}

int SceneController::getHeight()
{
  return m_scene.getHeight();
}

} /* namespace */
