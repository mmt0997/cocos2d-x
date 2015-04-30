#include "BoundingSphere.h"

namespace gameplay
{

inline BoundingSphere& BoundingSphere::operator*=(const cocos2d::Mat4& matrix)
{
    transform(matrix);
    return *this;
}

inline const BoundingSphere operator*(const cocos2d::Mat4& matrix, const BoundingSphere& sphere)
{
    BoundingSphere s(sphere);
    s.transform(matrix);
    return s;
}

}
