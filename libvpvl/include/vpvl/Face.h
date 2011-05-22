#ifndef VPVL_FACE_H_
#define VPVL_FACE_H_

#include <LinearMath/btAlignedObjectArray.h>
#include <LinearMath/btVector3.h>
#include "vpvl/common.h"

namespace vpvl
{

enum FaceType {
    kBase,
    kEyebrow,
    kEye,
    kLip,
    kOther
};

struct FaceVertex
{
    int id;
    btVector3 position;
};

class Face
{
public:
    Face();
    ~Face();

    static const int kMaxVertexID = 65536;
    static size_t stride(const char *data);

    void read(const char *data);
    void convertIndices(const Face &base);
    void applyToVertices(btAlignedObjectArray<btVector3> &vertices);
    void addToVertices(btAlignedObjectArray<btVector3> &vertices, float rate);

    const char *name() const {
        return m_name;
    }
    FaceType type() const {
        return m_type;
    }
    float weight() const {
        return m_weight;
    }

    void setName(const char *value) {
        vpvlStringCopySafe(m_name, value, sizeof(m_name));
    }
    void setWeight(float value) {
        m_weight = value;
    }

private:
    char m_name[20];
    FaceType m_type;
    btAlignedObjectArray<FaceVertex> m_vertices;
    float m_weight;
};

typedef btAlignedObjectArray<Face> FaceList;

} /* namespace vpvl */

#endif