#include "Ray.h"

namespace gameplay
{

inline Ray& Ray::operator*=(const cocos2d::Mat4& matrix)
{
    transform(matrix);
    return *this;
}

inline const Ray operator*(const cocos2d::Mat4& matrix, const Ray& ray)
{
    Ray r(ray);
    r.transform(matrix);
    return r;
}

}
