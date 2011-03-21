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
/* - Neither the name of the MMDAgent project team nor the names of  */
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

#ifndef MMDAI_SCENERENDERENGINE_H_
#define MMDAI_SCENERENDERENGINE_H_

#include <btBulletDynamicsCommon.h>
#include <MMDME/PMDRenderEngine.h>

class btConvexShape;

namespace MMDAI {

  class BulletPhysics;
  class Option;
  class PMDBone;
  class PMDModel;
  class PMDObject;
  class PMDTexture;
  class Stage;

  typedef struct PMDTextureNative PMDTextureNative;
  typedef struct PMDRenderCacheNative PMDRenderCacheNative;

  class SceneRenderEngine : public PMDRenderEngine {
  public:
    virtual ~SceneRenderEngine() {}

    virtual void renderModelCached(PMDModel *model,
                           PMDRenderCacheNative **ptr) = 0;
    virtual void renderTileTexture(PMDTexture *texture,
                           const float *color,
                           const float *normal,
                           const float *vertices1,
                           const float *vertices2,
                           const float *vertices3,
                           const float *vertices4,
                           const float nX,
                           const float nY,
                           const bool cullFace,
                           PMDRenderCacheNative **ptr) = 0;
    virtual void deleteCache(PMDRenderCacheNative **ptr) = 0;

    virtual bool setup(float *campusColor,
               bool useShadowMapping,
               int shadowMapTextureSize,
               bool shadowMapLightFirst) = 0;
    virtual void initializeShadowMap(int shadowMapTextureSize) = 0;
    virtual void setShadowMapping(bool flag,
                          int shadowMapTextureSize,
                          bool shadowMapLightFirst) = 0;
    virtual void prerender(Option *option,
                   PMDObject **objects,
                   int size) = 0;
    virtual void render(Option *option,
                Stage *stage,
                PMDObject **objects,
                int size) = 0;
    virtual int pickModel(PMDObject **objects,
                  int size,
                  int x,
                  int y,
                  int width,
                  int height,
                  double scale,
                  int *allowDropPicked) = 0;
    virtual void updateLighting(bool useCartoonRendering,
                        bool useMMDLikeCartoon,
                        float *lightDirection,
                        float lightIntensy,
                        float *lightColor) = 0;
    virtual void updateProjectionMatrix(const int width,
                                const int height,
                                const double scale) = 0;
    virtual void updateModelViewMatrix(const btTransform &transMatrix,
                               const btTransform &transMatrixInv) = 0;
    virtual void setShadowMapAutoView(const btVector3 &eyePoint,
                              const float radius) = 0;
    virtual void setModelViewMatrix(const btScalar modelView[16]) = 0;
    virtual void setProjectionMatrix(const btScalar projection[16]) = 0;
  };

} /* namespace */

#endif
