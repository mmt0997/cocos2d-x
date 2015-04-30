#include "Plane.h"

namespace gameplay
{

inline Plane& Plane::operator*=(const cocos2d::Mat4& matrix)
{
    transform(matrix);
    return *this;
}

inline const Plane operator*(const cocos2d::Mat4& matrix, const Plane& plane)
{
    Plane p(plane);
    p.transform(matrix);
    return p;
}

}
