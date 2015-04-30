#include "Base.h"
#include "Transform.h"
#include "Game.h"
#include "Node.h"

namespace gameplay
{

int Transform::_suspendTransformChanged(0);
std::vector<Transform*> Transform::_transformsChanged;

Transform::Transform()
    : _matrixDirtyBits(0), _listeners(NULL)
{
    _scale.set(cocos2d::Vec3::ONE);
}

Transform::Transform(const cocos2d::Vec3& scale, const cocos2d::Quaternion& rotation, const cocos2d::Vec3& translation)
    : _matrixDirtyBits(0), _listeners(NULL)
{
    set(scale, rotation, translation);
}

Transform::Transform(const cocos2d::Vec3& scale, const cocos2d::Mat4& rotation, const cocos2d::Vec3& translation)
    : _matrixDirtyBits(0), _listeners(NULL)
{
    set(scale, rotation, translation);
}

Transform::Transform(const Transform& copy)
    : _matrixDirtyBits(0), _listeners(NULL)
{
    set(copy);
}

Transform::~Transform()
{
    SAFE_DELETE(_listeners);
}

void Transform::suspendTransformChanged()
{
    _suspendTransformChanged++;
}

void Transform::resumeTransformChanged()
{
    if (_suspendTransformChanged == 0) // We haven't suspended transformChanged() calls, so do nothing.
        return;
    
    if (_suspendTransformChanged == 1)
    {
        // Call transformChanged() on all transforms in the list
        size_t transformCount = _transformsChanged.size();
        for (size_t i = 0; i < transformCount; i++)
        {
            Transform* t = _transformsChanged.at(i);
            GP_ASSERT(t);
            t->transformChanged();
        }

        // Go through list and reset DIRTY_NOTIFY bit. The list could potentially be larger here if the 
        // transforms we were delaying calls to transformChanged() have any child nodes.
        transformCount = _transformsChanged.size();
        for (size_t i = 0; i < transformCount; i++)
        {
            Transform* t = _transformsChanged.at(i);
            GP_ASSERT(t);
            t->_matrixDirtyBits &= ~DIRTY_NOTIFY;
        }

        // empty list for next frame.
        _transformsChanged.clear();
    }
    _suspendTransformChanged--;
}

bool Transform::isTransformChangedSuspended()
{
    return (_suspendTransformChanged > 0);
}

const char* Transform::getTypeName() const
{
    return "Transform";
}

const cocos2d::Mat4& Transform::getMatrix() const
{
    if (_matrixDirtyBits)
    {
        if (!isStatic())
        {
            bool hasTranslation = !_translation.isZero();
            bool hasScale = !_scale.isOne();
            bool hasRotation = !_rotation.isIdentity();

            // Compose the matrix in TRS order since we use column-major matrices with column vectors and
            // multiply M*v (as opposed to XNA and DirectX that use row-major matrices with row vectors and multiply v*M).
            if (hasTranslation || (_matrixDirtyBits & DIRTY_TRANSLATION) == DIRTY_TRANSLATION)
            {
                cocos2d::Mat4::createTranslation(_translation, &_matrix);
                if (hasRotation || (_matrixDirtyBits & DIRTY_ROTATION) == DIRTY_ROTATION)
                {
                    _matrix.rotate(_rotation);
                }
                if (hasScale || (_matrixDirtyBits & DIRTY_SCALE) == DIRTY_SCALE)
                {
                    _matrix.scale(_scale);
                }
            }
            else if (hasRotation || (_matrixDirtyBits & DIRTY_ROTATION) == DIRTY_ROTATION)
            {
                cocos2d::Mat4::createRotation(_rotation, &_matrix);
                if (hasScale || (_matrixDirtyBits & DIRTY_SCALE) == DIRTY_SCALE)
                {
                    _matrix.scale(_scale);
                }
            }
            else if (hasScale || (_matrixDirtyBits & DIRTY_SCALE) == DIRTY_SCALE)
            {
                cocos2d::Mat4::createScale(_scale, &_matrix);
            }
        }

        _matrixDirtyBits &= ~DIRTY_TRANSLATION & ~DIRTY_ROTATION & ~DIRTY_SCALE;
    }

    return _matrix;
}

const cocos2d::Vec3& Transform::getScale() const
{
    return _scale;
}

void Transform::getScale(cocos2d::Vec3* scale) const
{
    GP_ASSERT(scale);
    scale->set(_scale);
}

float Transform::getScaleX() const
{
    return _scale.x;
}

float Transform::getScaleY() const
{
    return _scale.y;
}

float Transform::getScaleZ() const
{
    return _scale.z;
}

const cocos2d::Quaternion& Transform::getRotation() const
{
    return _rotation;
}

void Transform::getRotation(cocos2d::Quaternion* rotation) const
{
    GP_ASSERT(rotation);
    rotation->set(_rotation);
}

void Transform::getRotation(cocos2d::Mat4* rotation) const
{
    GP_ASSERT(rotation);
    cocos2d::Mat4::createRotation(_rotation, rotation);
}

float Transform::getRotation(cocos2d::Vec3* axis) const
{
    GP_ASSERT(axis);
    return _rotation.toAxisAngle(axis);
}

const cocos2d::Vec3& Transform::getTranslation() const
{
    return _translation;
}

void Transform::getTranslation(cocos2d::Vec3* translation) const
{
    GP_ASSERT(translation);
    translation->set(_translation);
}

float Transform::getTranslationX() const
{
    return _translation.x;
}

float Transform::getTranslationY() const
{
    return _translation.y;
}

float Transform::getTranslationZ() const
{
    return _translation.z;
}

cocos2d::Vec3 Transform::getForwardVector() const
{
    cocos2d::Vec3 v;
    getForwardVector(&v);
    return v;
}

void Transform::getForwardVector(cocos2d::Vec3* dst) const
{
    getMatrix().getForwardVector(dst);
}

cocos2d::Vec3 Transform::getBackVector() const
{
    cocos2d::Vec3 v;
    getBackVector(&v);
    return v;
}

void Transform::getBackVector(cocos2d::Vec3* dst) const
{
    getMatrix().getBackVector(dst);
}

cocos2d::Vec3 Transform::getUpVector() const
{
    cocos2d::Vec3 v;
    getUpVector(&v);
    return v;
}

void Transform::getUpVector(cocos2d::Vec3* dst) const
{
    getMatrix().getUpVector(dst);
}

cocos2d::Vec3 Transform::getDownVector() const
{
    cocos2d::Vec3 v;
    getDownVector(&v);
    return v;
}

void Transform::getDownVector(cocos2d::Vec3* dst) const
{
    getMatrix().getDownVector(dst);
}

cocos2d::Vec3 Transform::getLeftVector() const
{
    cocos2d::Vec3 v;
    getLeftVector(&v);
    return v;
}

void Transform::getLeftVector(cocos2d::Vec3* dst) const
{
    getMatrix().getLeftVector(dst);
}

cocos2d::Vec3 Transform::getRightVector() const
{
    cocos2d::Vec3 v;
    getRightVector(&v);
    return v;
}

void Transform::getRightVector(cocos2d::Vec3* dst) const
{
    getMatrix().getRightVector(dst);
}

void Transform::rotate(float qx, float qy, float qz, float qw)
{
    if (isStatic())
        return;

    cocos2d::Quaternion q(qx, qy, qz, qw);
    _rotation.multiply(q);
    dirty(DIRTY_ROTATION);
}

void Transform::rotate(const cocos2d::Quaternion& rotation)
{
    if (isStatic())
        return;

    _rotation.multiply(rotation);
    dirty(DIRTY_ROTATION);
}

void Transform::rotate(const cocos2d::Vec3& axis, float angle)
{
    if (isStatic())
        return;

    cocos2d::Quaternion rotationQuat;
    cocos2d::Quaternion::createFromAxisAngle(axis, angle, &rotationQuat);
    _rotation.multiply(rotationQuat);
    _rotation.normalize();
    dirty(DIRTY_ROTATION);
}

void Transform::rotate(const cocos2d::Mat4& rotation)
{
    if (isStatic())
        return;

    cocos2d::Quaternion rotationQuat;
    cocos2d::Quaternion::createFromRotationMatrix(rotation, &rotationQuat);
    _rotation.multiply(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::rotateX(float angle)
{
    if (isStatic())
        return;

    cocos2d::Quaternion rotationQuat;
    cocos2d::Quaternion::createFromAxisAngle(cocos2d::Vec3::UNIT_X, angle, &rotationQuat);
    _rotation.multiply(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::rotateY(float angle)
{
    if (isStatic())
        return;

    cocos2d::Quaternion rotationQuat;
    cocos2d::Quaternion::createFromAxisAngle(cocos2d::Vec3::UNIT_Y, angle, &rotationQuat);
    _rotation.multiply(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::rotateZ(float angle)
{
    if (isStatic())
        return;

    cocos2d::Quaternion rotationQuat;
    cocos2d::Quaternion::createFromAxisAngle(cocos2d::Vec3::UNIT_Z, angle, &rotationQuat);
    _rotation.multiply(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::scale(float scale)
{
    if (isStatic())
        return;

    _scale.scale(scale);
    dirty(DIRTY_SCALE);
}

void Transform::scale(float sx, float sy, float sz)
{
    if (isStatic())
        return;

    _scale.x *= sx;
    _scale.y *= sy;
    _scale.z *= sz;
    dirty(DIRTY_SCALE);
}

void Transform::scale(const cocos2d::Vec3& scale)
{
    if (isStatic())
        return;

    _scale.x *= scale.x;
    _scale.y *= scale.y;
    _scale.z *= scale.z;
    dirty(DIRTY_SCALE);
}

void Transform::scaleX(float sx)
{
    if (isStatic())
        return;

    _scale.x *= sx;
    dirty(DIRTY_SCALE);
}

void Transform::scaleY(float sy)
{
    if (isStatic())
        return;

    _scale.y *= sy;
    dirty(DIRTY_SCALE);
}

void Transform::scaleZ(float sz)
{
    if (isStatic())
        return;

    _scale.z *= sz;
    dirty(DIRTY_SCALE);
}

void Transform::set(const cocos2d::Vec3& scale, const cocos2d::Quaternion& rotation, const cocos2d::Vec3& translation)
{
    if (isStatic())
        return;

    _scale.set(scale);
    _rotation.set(rotation);
    _translation.set(translation);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::set(const cocos2d::Vec3& scale, const cocos2d::Mat4& rotation, const cocos2d::Vec3& translation)
{
    if (isStatic())
        return;

    _scale.set(scale);
    cocos2d::Quaternion rotationQuat;
    cocos2d::Quaternion::createFromRotationMatrix(rotation, &rotationQuat);
    _rotation.set(rotationQuat);
    _translation.set(translation);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::set(const cocos2d::Vec3& scale, const cocos2d::Vec3& axis, float angle, const cocos2d::Vec3& translation)
{
    if (isStatic())
        return;

    _scale.set(scale);
    _rotation.set(axis, angle);
    _translation.set(translation);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::set(const Transform& transform)
{
    if (isStatic())
        return;

    _scale.set(transform._scale);
    _rotation.set(transform._rotation);
    _translation.set(transform._translation);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::setIdentity()
{
    if (isStatic())
        return;

    _scale.set(1.0f, 1.0f, 1.0f);
    _rotation.setIdentity();
    _translation.set(0.0f, 0.0f, 0.0f);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::setScale(float scale)
{
    if (isStatic())
        return;

    _scale.set(scale, scale, scale);
    dirty(DIRTY_SCALE);
}

void Transform::setScale(float sx, float sy, float sz)
{
    if (isStatic())
        return;

    _scale.set(sx, sy, sz);
    dirty(DIRTY_SCALE);
}

void Transform::setScale(const cocos2d::Vec3& scale)
{
    _scale.set(scale);
    dirty(DIRTY_SCALE);
}

void Transform::setScaleX(float sx)
{
    if (isStatic())
        return;

    _scale.x = sx;
    dirty(DIRTY_SCALE);
}

void Transform::setScaleY(float sy)
{
    if (isStatic())
        return;

    _scale.y = sy;
    dirty(DIRTY_SCALE);
}

void Transform::setScaleZ(float sz)
{
    if (isStatic())
        return;

    _scale.z = sz;
    dirty(DIRTY_SCALE);
}

void Transform::setRotation(const cocos2d::Quaternion& rotation)
{
    if (isStatic())
        return;

    _rotation.set(rotation);
    dirty(DIRTY_ROTATION);
}

void Transform::setRotation(float qx, float qy, float qz, float qw)
{
    if (isStatic())
        return;

    _rotation.set(qx, qy, qz, qw);
    dirty(DIRTY_ROTATION);
}

void Transform::setRotation(const cocos2d::Mat4& rotation)
{
    if (isStatic())
        return;

    cocos2d::Quaternion rotationQuat;
    cocos2d::Quaternion::createFromRotationMatrix(rotation, &rotationQuat);
    _rotation.set(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::setRotation(const cocos2d::Vec3& axis, float angle)
{
    if (isStatic())
        return;

    _rotation.set(axis, angle);
    dirty(DIRTY_ROTATION);
}

void Transform::setTranslation(const cocos2d::Vec3& translation)
{
    if (isStatic())
        return;

    _translation.set(translation);
    dirty(DIRTY_TRANSLATION);
}

void Transform::setTranslation(float tx, float ty, float tz)
{
    if (isStatic())
        return;

    _translation.set(tx, ty, tz);
    dirty(DIRTY_TRANSLATION);
}

void Transform::setTranslationX(float tx)
{
    if (isStatic())
        return;

    _translation.x = tx;
    dirty(DIRTY_TRANSLATION);
}

void Transform::setTranslationY(float ty)
{
    if (isStatic())
        return;

    _translation.y = ty;
    dirty(DIRTY_TRANSLATION);
}

void Transform::setTranslationZ(float tz)
{
    if (isStatic())
        return;

    _translation.z = tz;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translate(float tx, float ty, float tz)
{
    if (isStatic())
        return;

    _translation.x += tx;
    _translation.y += ty;
    _translation.z += tz;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translate(const cocos2d::Vec3& translation)
{
    if (isStatic())
        return;

    _translation.x += translation.x;
    _translation.y += translation.y;
    _translation.z += translation.z;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translateX(float tx)
{
    if (isStatic())
        return;

    _translation.x += tx;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translateY(float ty)
{
    if (isStatic())
        return;

    _translation.y += ty;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translateZ(float tz)
{
    if (isStatic())
        return;

    _translation.z += tz;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translateLeft(float amount)
{
    if (isStatic())
        return;

    // Force the current transform matrix to be updated.
    getMatrix();

    cocos2d::Vec3 left;
    _matrix.getLeftVector(&left);
    left.normalize();
    left.scale(amount);

    translate(left);
}

void Transform::translateUp(float amount)
{
    if (isStatic())
        return;

    // Force the current transform matrix to be updated.
    getMatrix();

    cocos2d::Vec3 up;
    _matrix.getUpVector(&up);
    up.normalize();
    up.scale(amount);

    translate(up);
}

void Transform::translateForward(float amount)
{
    if (isStatic())
        return;

    // Force the current transform matrix to be updated.
    getMatrix();

    cocos2d::Vec3 forward;
    _matrix.getForwardVector(&forward);
    forward.normalize();
    forward.scale(amount);

    translate(forward);
}

void Transform::translateSmooth(const cocos2d::Vec3& target, float elapsedTime, float responseTime)
{
    if (isStatic())
        return;

    if (elapsedTime > 0)
    {
        _translation += (target - _translation) * (elapsedTime / (elapsedTime + responseTime));
        dirty(DIRTY_TRANSLATION);
    }
}

void Transform::transformPoint(cocos2d::Vec3* point)
{
    getMatrix();
    _matrix.transformPoint(point);
}

void Transform::transformPoint(const cocos2d::Vec3& point, cocos2d::Vec3* dst)
{
    getMatrix();
    _matrix.transformPoint(point, dst);
}

void Transform::transformVector(cocos2d::Vec3* normal)
{
    getMatrix();
    _matrix.transformVector(normal);
}

void Transform::transformVector(const cocos2d::Vec3& normal, cocos2d::Vec3* dst)
{
    getMatrix();
    _matrix.transformVector(normal, dst);
}

void Transform::transformVector(float x, float y, float z, float w, cocos2d::Vec3* dst)
{
    getMatrix();
    _matrix.transformVector(x, y, z, w, dst);
}

bool Transform::isStatic() const
{
    return false;
}

void Transform::dirty(char matrixDirtyBits)
{
    _matrixDirtyBits |= matrixDirtyBits;
    if (isTransformChangedSuspended())
    {
        if (!isDirty(DIRTY_NOTIFY))
        {
            suspendTransformChange(this);
        }
    }
    else
    {
        transformChanged();
    }
}

bool Transform::isDirty(char matrixDirtyBits) const
{
    return (_matrixDirtyBits & matrixDirtyBits) == matrixDirtyBits;
}

void Transform::suspendTransformChange(Transform* transform)
{
    GP_ASSERT(transform);
    transform->_matrixDirtyBits |= DIRTY_NOTIFY;
    _transformsChanged.push_back(transform);
}

void Transform::addListener(Transform::Listener* listener, long cookie)
{
    GP_ASSERT(listener);

    if (_listeners == NULL)
        _listeners = new std::list<TransformListener>();

    TransformListener l;
    l.listener = listener;
    l.cookie = cookie;
    _listeners->push_back(l);
}

void Transform::removeListener(Transform::Listener* listener)
{
    GP_ASSERT(listener);

    if (_listeners)
    {
        for (std::list<TransformListener>::iterator itr = _listeners->begin(); itr != _listeners->end(); ++itr)
        {
            if ((*itr).listener == listener)
            {
                _listeners->erase(itr);
                break;
            }
        }
    }
}

void Transform::transformChanged()
{
    if (_listeners)
    {
        for (std::list<TransformListener>::iterator itr = _listeners->begin(); itr != _listeners->end(); ++itr)
        {
            TransformListener& l = *itr;
            GP_ASSERT(l.listener);
            l.listener->transformChanged(this, l.cookie);
        }
    }
}

    
void Transform::cloneInto(Transform* transform, NodeCloneContext &context) const
{
    GP_ASSERT(transform);
        
    transform->_scale.set(_scale);
    transform->_rotation.set(_rotation);
    transform->_translation.set(_translation);
    transform->dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

}
