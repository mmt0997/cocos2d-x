/****************************************************************************
Copyright 2011 Jeff Lamarche
Copyright 2012 Goffredo Marocchi
Copyright 2012 Ricardo Quesada
Copyright 2012 cocos2d-x.org
Copyright 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN false EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "renderer/CCGLProgramState.h"

#include "renderer/CCGLProgram.h"
#include "renderer/CCGLProgramStateCache.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/ccGLStateCache.h"
#include "renderer/CCTexture2D.h"
#include "base/CCEventCustom.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventType.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"

NS_CC_BEGIN

//
//
// UniformValue
//
//

UniformValue::UniformValue()
: _uniform(nullptr)
, _glprogram(nullptr)
, _useCallback(false)
{
}

UniformValue::UniformValue(Uniform *uniform, GLProgram* glprogram)
: _uniform(uniform)
, _glprogram(glprogram)
, _useCallback(false)
{
}

UniformValue::~UniformValue()
{
	if (_useCallback)
		delete _value.callback;
}

void UniformValue::apply()
{
    if(_useCallback) {
        (*_value.callback)(_glprogram, _uniform);
    }
    
}

void UniformValue::setCallback(const std::function<void(GLProgram*, Uniform*)> &callback)
{
	// delete previously set callback
	// TODO: memory will leak if the user does:
	//    value->setCallback();
	//    value->setFloat();
	if (_useCallback)
		delete _value.callback;

    _value.callback = new std::function<void(GLProgram*, Uniform*)>();
	*_value.callback = callback;

    _useCallback = true;
}

void UniformValue::setFloat(float value)
{
    CCASSERT (_uniform->type == GL_FLOAT, "");
    _value.floatValue = value;
    _useCallback = false;
}

void UniformValue::setTexture(GLuint textureId, GLuint textureUnit)
{
    //CCASSERT(_uniform->type == GL_SAMPLER_2D, "Wrong type. expecting GL_SAMPLER_2D");
    _value.tex.textureId = textureId;
    _value.tex.textureUnit = textureUnit;
    _useCallback = false;
}
void UniformValue::setInt(int value)
{
    CCASSERT(_uniform->type == GL_INT, "Wrong type: expecting GL_INT");
    _value.intValue = value;
    _useCallback = false;
}

void UniformValue::setVec2(const Vec2& value)
{
    CCASSERT (_uniform->type == GL_FLOAT_VEC2, "");
	memcpy(_value.v2Value, &value, sizeof(_value.v2Value));
    _useCallback = false;
}

void UniformValue::setVec3(const Vec3& value)
{
    CCASSERT (_uniform->type == GL_FLOAT_VEC3, "");
	memcpy(_value.v3Value, &value, sizeof(_value.v3Value));
	_useCallback = false;
}

void UniformValue::setVec4(const Vec4& value)
{
    CCASSERT (_uniform->type == GL_FLOAT_VEC4, "");
	memcpy(_value.v4Value, &value, sizeof(_value.v4Value));
	_useCallback = false;
}

void UniformValue::setMat4(const Mat4& value)
{
    CCASSERT(_uniform->type == GL_FLOAT_MAT4, "");
	memcpy(_value.matrixValue, &value, sizeof(_value.matrixValue));
	_useCallback = false;
}

//
//
// VertexAttribValue
//
//

VertexAttribValue::VertexAttribValue()
: _vertexAttrib(nullptr)
, _useCallback(false)
, _enabled(false)
{
}

VertexAttribValue::VertexAttribValue(VertexAttrib *vertexAttrib)
: _vertexAttrib(vertexAttrib)
, _useCallback(false)
, _enabled(false)
{
}

VertexAttribValue::~VertexAttribValue()
{
	if (_useCallback)
		delete _value.callback;
}

void VertexAttribValue::apply()
{
    if(_enabled) {
        if(_useCallback) {
            (*_value.callback)(_vertexAttrib);
        }
        else
        {
            glVertexAttribPointer(_vertexAttrib->index,
                                  _value.pointer.size,
                                  _value.pointer.type,
                                  _value.pointer.normalized,
                                  _value.pointer.stride,
                                  _value.pointer.pointer);
        }
    }
}

void VertexAttribValue::setCallback(const std::function<void(VertexAttrib*)> &callback)
{
	_value.callback = new std::function<void(VertexAttrib*)>();
	*_value.callback = callback;
    _useCallback = true;
    _enabled = true;
}

void VertexAttribValue::setPointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer)
{
    _value.pointer.size = size;
    _value.pointer.type = type;
    _value.pointer.normalized = normalized;
    _value.pointer.stride = stride;
    _value.pointer.pointer = pointer;
    _enabled = true;
}

//
//
// GLProgramState
//
//

GLProgramState* GLProgramState::create(GLProgram *glprogram)
{
    GLProgramState* ret = nullptr;
    ret = new (std::nothrow) GLProgramState();
    if(ret && ret->init(glprogram))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GLProgramState* GLProgramState::getOrCreateWithGLProgramName(const std::string &glProgramName )
{
    GLProgram *glProgram = GLProgramCache::getInstance()->getGLProgram(glProgramName);
    if( glProgram )
        return getOrCreateWithGLProgram(glProgram);

    CCLOG("cocos2d: warning: GLProgram '%s' not found", glProgramName.c_str());
    return nullptr;
}


GLProgramState* GLProgramState::getOrCreateWithGLProgram(GLProgram *glprogram)
{
    GLProgramState* ret = GLProgramStateCache::getInstance()->getGLProgramState(glprogram);
    return ret;
}

GLProgramState::GLProgramState()
: _uniformAttributeValueDirty(true)
, _textureUnitIndex(1)
, _vertexAttribsFlags(0)
, _glprogram(nullptr)
, _buffer(nullptr)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    /** listen the event that renderer was recreated on Android/WP8 */
    CCLOG("create rendererRecreatedListener for GLProgramState");
    _backToForegroundlistener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, 
        [this](EventCustom*) 
        {
            CCLOG("Dirty Uniform and Attributes of GLProgramState"); 
            _uniformAttributeValueDirty = true;
        });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundlistener, -1);
#endif
}

GLProgramState::~GLProgramState()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundlistener);
#endif
    
    CC_SAFE_RELEASE(_glprogram);
    CC_SAFE_DELETE(_buffer);
}

bool GLProgramState::init(GLProgram* glprogram)
{
    CCASSERT(glprogram, "invalid shader");

    _glprogram = glprogram;
    _glprogram->retain();

    for(auto &attrib : _glprogram->_vertexAttribs) {
        VertexAttribValue value(&attrib.second);
        _attributes[attrib.first] = value;
    }

    for(auto &uniform : _glprogram->_userUniforms) {
        UniformValue value(&uniform.second, _glprogram);
        _uniforms[uniform.second.location] = value;
        _uniformsByName[uniform.first] = uniform.second.location;
    }
    
    if(_buffer)
    {
        CC_SAFE_DELETE(_buffer);
    }
    _buffer = new UniformBuffer(_glprogram->getDefaultUniformBuffer());
    return true;
}

void GLProgramState::resetGLProgram()
{
    CC_SAFE_RELEASE(_glprogram);
    _uniforms.clear();
    _attributes.clear();
    // first texture is GL_TEXTURE1
    _textureUnitIndex = 1;
}

void GLProgramState::apply(const Mat4& modelView)
{
    applyGLProgram(modelView);

    applyAttributes();

    applyUniforms();
}

void GLProgramState::updateUniformsAndAttributes()
{
    CCASSERT(_glprogram, "invalid glprogram");
    if(_uniformAttributeValueDirty)
    {
        for(auto& uniformLocation : _uniformsByName)
        {
            _uniforms[uniformLocation.second]._uniform = _glprogram->getUniform(uniformLocation.first);
        }
        
        _vertexAttribsFlags = 0;
        for(auto& attributeValue : _attributes)
        {
            attributeValue.second._vertexAttrib = _glprogram->getVertexAttrib(attributeValue.first);;
            if(attributeValue.second._enabled)
                _vertexAttribsFlags |= 1 << attributeValue.second._vertexAttrib->index;
        }
        
        _uniformAttributeValueDirty = false;
        
    }
}

void GLProgramState::applyGLProgram(const Mat4& modelView)
{
    CCASSERT(_glprogram, "invalid glprogram");
    updateUniformsAndAttributes();
    // set shader
    _glprogram->use();
    _glprogram->setUniformsForBuiltins(modelView);
}
void GLProgramState::applyAttributes(bool applyAttribFlags)
{
    // Don't set attributes if they weren't set
    // Use Case: Auto-batching
    updateUniformsAndAttributes();
    if(_vertexAttribsFlags) {
        // enable/disable vertex attribs
        if (applyAttribFlags)
            GL::enableVertexAttribs(_vertexAttribsFlags);
        // set attributes
        for(auto &attribute : _attributes)
        {
            attribute.second.apply();
        }
    }
}
void GLProgramState::applyUniforms()
{
    CommandBufferUniform(*_buffer).apply();
    // set uniforms
    updateUniformsAndAttributes();
    for(auto& uniform : _uniforms) {
        uniform.second.apply();
    }
}

void GLProgramState::setGLProgram(GLProgram *glprogram)
{
    CCASSERT(glprogram, "invalid GLProgram");

    if( _glprogram != glprogram) {
        resetGLProgram();
        init(glprogram);
    }
}

UniformValue* GLProgramState::getUniformValue(GLint uniformLocation)
{
    updateUniformsAndAttributes();
    const auto itr = _uniforms.find(uniformLocation);
    if (itr != _uniforms.end())
        return &itr->second;
    return nullptr;
}

UniformValue* GLProgramState::getUniformValue(const std::string &name)
{
    updateUniformsAndAttributes();
    const auto itr = _uniformsByName.find(name);
    if (itr != _uniformsByName.end())
        return &_uniforms[itr->second];
    return nullptr;
}

VertexAttribValue* GLProgramState::getVertexAttribValue(const std::string &name)
{
    updateUniformsAndAttributes();
    const auto itr = _attributes.find(name);
    if( itr != _attributes.end())
        return &itr->second;
    return nullptr;
}

// VertexAttrib Setters
void GLProgramState::setVertexAttribCallback(const std::string &name, const std::function<void(VertexAttrib*)> &callback)
{
    VertexAttribValue *v = getVertexAttribValue(name);
    if(v) {
        v->setCallback(callback);
        _vertexAttribsFlags |= 1 << v->_vertexAttrib->index;
    }
    else
    {
		CCLOG("cocos2d: warning: Attribute not found: %s", name.c_str());
	}
}

void GLProgramState::setVertexAttribPointer(const std::string &name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer)
{
    auto v = getVertexAttribValue(name);
    if(v) {
        v->setPointer(size, type, normalized, stride, pointer);
        _vertexAttribsFlags |= 1 << v->_vertexAttrib->index;
    }
    else
    {
		CCLOG("cocos2d: warning: Attribute not found: %s", name.c_str());
    }
}

// Uniform Setters

void GLProgramState::setUniformCallback(const std::string &uniformName, const std::function<void(GLProgram*, Uniform*)> &callback)
{
    auto v = getUniformValue(uniformName);
    if (v)
        v->setCallback(callback);
    else
        CCLOG("cocos2d: warning: Uniform not found: %s", uniformName.c_str());
}

void GLProgramState::setUniformCallback(GLint uniformLocation, const std::function<void(GLProgram*, Uniform*)> &callback)
{
    auto v = getUniformValue(uniformLocation);
    if (v)
        v->setCallback(callback);
    else
        CCLOG("cocos2d: warning: Uniform at location not found: %i", uniformLocation);
}

void GLProgramState::setUniformFloat(const std::string &uniformName, float value)
{
    if(_buffer)
        _buffer->setUniformFloat(uniformName, value);
//    auto v = getUniformValue(uniformName);
//    if (v)
//        v->setFloat(value);
//    else
//        CCLOG("cocos2d: warning: Uniform not found: %s", uniformName.c_str());
}

void GLProgramState::setUniformFloat(GLint uniformLocation, float value)
{
    CC_ASSERT("set uniform by location is not supported");
//    auto v = getUniformValue(uniformLocation);
//    if (v)
//        v->setFloat(value);
//    else
//        CCLOG("cocos2d: warning: Uniform at location not found: %i", uniformLocation);
}

void GLProgramState::setUniformInt(const std::string &uniformName, int value)
{
    if(_buffer)
        _buffer->setUniformInt1(uniformName, value);
}

void GLProgramState::setUniformInt(GLint uniformLocation, int value)
{
    CC_ASSERT("set uniform by location is not supported");
}

void GLProgramState::setUniformVec2(const std::string &uniformName, const Vec2& value)
{
    if(_buffer)
        _buffer->setUniformFloat2(uniformName, value);
}

void GLProgramState::setUniformVec2(GLint uniformLocation, const Vec2& value)
{
    CC_ASSERT("set uniform by location is not supported");
}

void GLProgramState::setUniformVec3(const std::string &uniformName, const Vec3& value)
{
    if(_buffer)
        _buffer->setUniformFloat3(uniformName, value);
}

void GLProgramState::setUniformVec3(GLint uniformLocation, const Vec3& value)
{
    CC_ASSERT("set uniform by location is not supported");
}

void GLProgramState::setUniformVec4(const std::string &uniformName, const Vec4& value)
{
    if(_buffer)
        _buffer->setUniformFloat4(uniformName, value);
}

void GLProgramState::setUniformVec4(GLint uniformLocation, const Vec4& value)
{
    CC_ASSERT("set uniform by location is not supported");
}

void GLProgramState::setUniformMat4(const std::string &uniformName, const Mat4& value)
{
    if(_buffer)
        _buffer->setUniformMat4(uniformName, value);
}

void GLProgramState::setUniformMat4(GLint uniformLocation, const Mat4& value)
{
    CC_ASSERT("set uniform by location is not supported");
}

// Textures

void GLProgramState::setUniformTexture(const std::string &uniformName, Texture2D *texture)
{
    CCASSERT(texture, "Invalid texture");
    setUniformTexture(uniformName, texture->getName());
}

void GLProgramState::setUniformTexture(GLint uniformLocation, Texture2D *texture)
{
    CC_ASSERT("set uniform by location is not supported");
}

void GLProgramState::setUniformTexture(const std::string &uniformName, GLuint textureId)
{
    if(_buffer)
        _buffer->setUniformTexture(uniformName, textureId);
}

void GLProgramState::setUniformTexture(GLint uniformLocation, GLuint textureId)
{
    CC_ASSERT("set uniform by location is not supported");
}

NS_CC_END
