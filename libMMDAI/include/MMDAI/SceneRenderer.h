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

#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include "MMDAI/Option.h"
#include "MMDAI/PMDObject.h"
#include "MMDAI/Stage.h"

#define RENDER_VIEWPOINT_CAMERA_Z     -100.0f
#define RENDER_VIEWPOINT_Y_OFFSET     -13.0f
#define RENDER_VIEWPOINT_FRUSTUM_NEAR 5.0f
#define RENDER_VIEWPOINT_FRUSTUM_FAR  2000.0f

/* Render: render */
class SceneRenderer
{
private:

  int m_width;             /* window width */
  int m_height;            /* winodw height */
  float m_scale;           /* target scale */
  btVector3 m_trans;       /* target trans vector */
  btQuaternion m_rot;      /* target rotation */
  btVector3 m_cameraTrans; /* position of camera */

  float m_scaleCurrent;         /* current scale */
  btVector3 m_transCurrent;     /* current trans vector */
  btQuaternion m_rotCurrent;    /* current rotation */
  btTransform m_transMatrix;    /* current trans vector + rotation matrix */
  btTransform m_transMatrixInv; /* current trans vector + inverse of rotation matrix */
  btScalar m_rotMatrix[16];     /* current rotation + OpenGL rotation matrix */
  btScalar m_rotMatrixInv[16];  /* current rotation + inverse of OpenGL rotation matrix */
  int m_highlightingModel;      /* highlighting model */

  float m_backgroundColor[3]; /* background color */

  bool m_enableShadowMapping;            /* true if shadow mapping */
  bool m_shadowMapInitialized;           /* true if initialized */
  GLuint m_depthTextureID;               /* depth texture for FBO */
  GLuint m_fboID;                        /* frame buffer object name */
  btVector3 m_lightVec;                  /* light vector for shadow maapping */
  btVector3 m_shadowMapAutoViewEyePoint; /* view point of shadow mapping */
  float m_shadowMapAutoViewRadius;       /* radius from view point */

  /* updateProjectionMatrix: update view information */
  void updateProjectionMatrix();

  /* applyProjectionMatirx: update projection matrix */
  void applyProjectionMatrix();

  /* updateModelViewMatrix: update model view matrix */
  void updateModelViewMatrix();

  /* update: update scale */
  void updateScale();

  /* updateTransRotMatrix:  update trans and rotation matrix */
  void updateTransRotMatrix();

  /* initializeShadowMap: initialize OpenGL for shadow mapping */
  void initializeShadowMap(int shadowMapTextureSize);

  /* renderSceneShadowMap: shadow mapping */
  void renderSceneShadowMap(Option *option, Stage *stage, PMDObject *objects, int size);

  /* renderScene: render scene */
  void renderScene(Option *option, Stage *stage, PMDObject *objects, int size);

  /* Render::initialize: initialzie Render */
  void initialize();

  /* Render::clear: free Render */
  void clear();

public:

  /* SceneRender: constructor */
  SceneRenderer();

  /* ~SceneRender: destructor */
  ~SceneRenderer();

  /* setup: initialize and setup Render */
  bool setup(int *size, float *campusColor, bool useShadowMapping, int shadowMapTextureSize, bool shadowMapLightFirst);

  /* setSize: set size */
  void setSize(int w, int h);

  /* getWidth: get width */
  int getWidth();

  /* getHeight: get height */
  int getHeight();

  /* setScale: set scale */
  void setScale(float scale);

  /* getScale: get scale */
  float getScale();

  /* translate: translate */
  void translate(float x, float y, float z);

  /* setShadowMapping: switch shadow mapping */
  void setShadowMapping(bool flag, int shadowMapTextureSize, bool shadowMapLightFirst);

  /* render: render all */
  void render(Option *option, Stage *stage, PMDObject *objects, int size);

  /* pickModel: pick up a model at the screen position */
  int pickModel(PMDObject *objects, int size, int x, int y, int *allowDropPicked);

  /* hilightModel: highlight selected model */
  void highlightModel(PMDObject *objects, int id);

  /* updateLigithing: update light */
  void updateLighting(bool useCartoonRendering, bool useMMDLikeCartoon, float *lightDirection, float lightIntensy, float *lightColor);

  /* updateDepthTextureViewParam: update center and radius information to get required range for shadow mapping */
  void updateDepthTextureViewParam(PMDObject *objList, int num);

  /* rotate: rotate scene */
  void rotate(float x, float y, float z);

  /* getScreenPointPosition: convert screen position to object position */
  void getScreenPointPosition(btVector3 *dst, btVector3 *src);
};

#endif // SCENERENDERER_H