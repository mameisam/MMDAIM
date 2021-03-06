/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2011  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                2010-2011  hkrn                                    */
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

#include "vpvl/vpvl.h"
#include "vpvl/internal/util.h"

namespace vpvl
{

#pragma pack(push, 1)

struct ConstraintChunk
{
    uint8_t name[Constraint::kNameSize];
    uint32_t bodyIDA;
    uint32_t bodyIDB;
    float position[3];
    float rotation[3];
    float limitPositionFrom[3];
    float limitPositionTo[3];
    float limitRotationFrom[3];
    float limitRotationTo[3];
    float stiffness[6];
};

#pragma pack(pop)

size_t Constraint::stride()
{
    return sizeof(ConstraintChunk);
}

Constraint::Constraint()
    : m_constraint(0)
{
    internal::zerofill(m_name, sizeof(m_name));
}

Constraint::~Constraint()
{
    internal::zerofill(m_name, sizeof(m_name));
    delete m_constraint;
    m_constraint = 0;
}

void Constraint::read(const uint8_t *data, const RigidBodyList &bodies, const btVector3 &offset)
{
    ConstraintChunk chunk;
    internal::copyBytes(reinterpret_cast<uint8_t *>(&chunk), data, sizeof(chunk));
    copyBytesSafe(m_name, chunk.name, sizeof(m_name));
    int32_t bodyID1 = chunk.bodyIDA;
    int32_t bodyID2 = chunk.bodyIDB;
    float *pos = chunk.position;
    float *rot = chunk.rotation;
    float *limitPosFrom = chunk.limitPositionFrom;
    float *limitPosTo = chunk.limitPositionTo;
    float *limitRotFrom = chunk.limitRotationFrom;
    float *limitRotTo = chunk.limitRotationTo;
    float *stiffness = chunk.stiffness;

    int nbodies = bodies.size();
    if (bodyID1 >= 0 && bodyID1 < nbodies &&bodyID2 >= 0 && bodyID2 < nbodies) {
        btTransform transform;
        btMatrix3x3 basis;
        transform.setIdentity();
#ifdef VPVL_COORDINATE_OPENGL
        btMatrix3x3 mx, my, mz;
        mx.setEulerZYX(-rot[0], 0.0f, 0.0f);
        my.setEulerZYX(0.0f, -rot[1], 0.0f);
        mz.setEulerZYX(0.0f, 0.0f, rot[2]);
        basis = my * mz * mx;
#else
        basis.setEulerZYX(rot[0], rot[1], rot[2]);
#endif
        transform.setBasis(basis);
#ifdef VPVL_COORDINATE_OPENGL
        transform.setOrigin(btVector3(pos[0], pos[1], -pos[2]) + offset);
#else
        transform.setOrigin(btVector3(pos[0], pos[1], pos[2]) + offset);
#endif
        btRigidBody *bodyA = bodies[bodyID1]->body(), *bodyB = bodies[bodyID2]->body();
        btTransform transformA = bodyA->getWorldTransform().inverse() * transform,
                transformB = bodyB->getWorldTransform().inverse() * transform;
        m_constraint = new btGeneric6DofSpringConstraint(*bodyA, *bodyB, transformA, transformB, true);
#ifdef VPVL_COORDINATE_OPENGL
        m_constraint->setLinearUpperLimit(btVector3(limitPosTo[0], limitPosTo[1], -limitPosFrom[2]));
        m_constraint->setLinearLowerLimit(btVector3(limitPosFrom[0], limitPosFrom[1], -limitPosTo[2]));
        m_constraint->setAngularUpperLimit(btVector3(-limitRotFrom[0], -limitRotFrom[1], limitRotTo[2]));
        m_constraint->setAngularLowerLimit(btVector3(-limitRotTo[0], -limitRotTo[1], limitRotFrom[2]));
#else
        m_constraint->setLinearUpperLimit(btVector3(limitPosTo[0], limitPosTo[1], limitPosTo[2]));
        m_constraint->setLinearLowerLimit(btVector3(limitPosFrom[0], limitPosFrom[1], limitPosFrom[2]));
        m_constraint->setAngularUpperLimit(btVector3(limitRotTo[0], limitRotTo[1], limitRotTo[2]));
        m_constraint->setAngularLowerLimit(btVector3(limitRotFrom[0], limitRotFrom[1], limitRotFrom[2]));
#endif

        for (int i = 0; i < 6; i++) {
            if (i >= 3 || stiffness[i] != 0.0f) {
                m_constraint->enableSpring(i, true);
                m_constraint->setStiffness(i, stiffness[i]);
            }
        }
    }
}

} /* namespace vpvl */
