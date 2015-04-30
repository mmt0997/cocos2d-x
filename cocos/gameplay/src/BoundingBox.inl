#include "BoundingBox.h"

namespace gameplay
{

inline BoundingBox& BoundingBox::operator*=(const cocos2d::Mat4& matrix)
{
    transform(matrix);
    return *this;
}

inline const BoundingBox operator*(const cocos2d::Mat4& matrix, const BoundingBox& box)
{
    BoundingBox b(box);
    b.transform(matrix);
    return b;
}

}
