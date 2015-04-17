/****************************************************************************
 Copyright (c) 2015 Chukong Technologies Inc.
 
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
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "renderer/CCCommandBuffer.h"
#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"

NS_CC_BEGIN
void CommandBuffer::apply()
{
    static Renderer* render(nullptr);
    if(render == nullptr)
    {
        render = Director::getInstance()->getRenderer();
    }
    if (render)
    {
        render->applyCommandBuffer(this);
    }
}

CommandBufferDepth& CommandBufferDepth::setEnable(bool enable)
{
    this->flags.setEnabled = true;
    this->flags.enabled = enable;
    return *this;
}

CommandBufferDepth& CommandBufferDepth::setFunction(uint32_t func)
{
    this->flags.setFunction = true;
    this->function = func;
    return *this;
}

CommandBufferDepth& CommandBufferDepth::setWriteMask(bool enable)
{
    this->flags.setWriteMask = true;
    this->flags.writeEnabled = enable;
    return *this;
}

CommandBufferDepth& CommandBufferDepth::setRangef(float near, float far)
{
    this->flags.setRangef = true;
    this->rangefNear = near;
    this->rangfFar = far;
    return *this;
}

CommandBufferDepth& CommandBufferDepth::setPolygonOffset(float factor, float units)
{
    this->flags.setPolygonOffset = true;
    this->polygonOffsetFactor = factor;
    this->polygonOffsetUnits = units;
    return *this;
}

CommandBufferBlend& CommandBufferBlend::setEnable(bool enable)
{
    this->flags.setEnabled = true;
    this->flags.enabled = enable;
    return *this;
}

CommandBufferBlend& CommandBufferBlend::setColor(float r, float g, float b, float a)
{
    this->flags.setColor = true;
    this->color.r = r;
    this->color.g = g;
    this->color.b = b;
    this->color.a = a;
    return *this;
}

CommandBufferBlend& CommandBufferBlend::setEquation(uint32_t mode, uint32_t modeAlpha)
{
    this->flags.setEquation = true;
    this->equation[0] = mode;
    this->equation[1] = (modeAlpha == GL_INVALID_ENUM) ? mode : modeAlpha;
    return *this;
}

CommandBufferBlend& CommandBufferBlend::setFunction(uint32_t src, uint32_t dst, uint32_t srcAlpha, uint32_t dstAlpha)
{
    this->flags.setFunction = true;
    this->srcFunc[0] = src;
    this->dstFunc[0] = dst;
    this->srcFunc[1] = (srcAlpha == GL_INVALID_ENUM) ? src : srcAlpha;
    this->dstFunc[1] = (dstAlpha == GL_INVALID_ENUM) ? dst : dstAlpha;
    return *this;
}

CommandBufferStencil& CommandBufferStencil::setEnable(bool enable)
{
    this->flags.setEnabled = true;
    this->flags.enabled = enable;
    return *this;
}

CommandBufferStencil& CommandBufferStencil::setFunc(uint32_t f, int32_t ref, uint32_t m, FaceEnum face)
{
    if (FaceEnum::FRONT == face)
    {
        this->flags.setFuncFront = true;
    }
    else if (FaceEnum::BACK == face)
    {
        this->flags.setFuncBack = true;
    }
    else if (FaceEnum::FRONT_AND_BACK == face)
    {
        this->flags.setFuncFront = this->flags.setFuncBack = true;
    }
    if(this->flags.setFuncFront)
    {
        this->func[0].func = (uint16_t)f;
        this->func[0].ref = (uint16_t)ref;
        this->func[0].mask = (uint16_t)m;
    }
    if(this->flags.setFuncBack)
    {
        this->func[1].func = (uint16_t)f;
        this->func[1].ref = (uint16_t)ref;
        this->func[1].mask = (uint16_t)m;
    }
    return *this;
}

CommandBufferStencil& CommandBufferStencil::setMask(uint32_t m, FaceEnum face)
{
    if (FaceEnum::FRONT == face)
    {
        this->flags.setMaskFront = true;
    }
    else if (FaceEnum::BACK == face)
    {
        this->flags.setMaskBack = true;
    }
    else if (FaceEnum::FRONT_AND_BACK == face)
    {
        this->flags.setMaskFront = this->flags.setMaskBack = true;
    }
    if(this->flags.setMaskFront)
    {
        this->mask[0] = m;
    }
    if(this->flags.setMaskBack)
    {
        this->mask[1] = m;
    }
    return *this;
}

CommandBufferStencil& CommandBufferStencil::setOp(uint32_t sfail, uint32_t dpfail, uint32_t dppass, FaceEnum face)
{
    if (FaceEnum::FRONT == face)
    {
        this->flags.setOpFront = true;
    }
    else if (FaceEnum::BACK == face)
    {
        this->flags.setOpBack = true;
    }
    else if (FaceEnum::FRONT_AND_BACK == face)
    {
        this->flags.setOpFront = this->flags.setOpBack = true;
    }
    if(this->flags.setOpFront)
    {
        this->op[0].sfail = sfail;
        this->op[0].dpfail = dpfail;
        this->op[0].dppass = dppass;
    }
    if(this->flags.setOpBack)
    {
        this->op[1].sfail = sfail;
        this->op[1].dpfail = dpfail;
        this->op[1].dppass = dppass;
    }
    return *this;
}

CommandBufferCulling& CommandBufferCulling::setEnable(bool enable)
{
    this->flags.setEnabled = true;
    this->flags.enabled = enable;
    return *this;
}

CommandBufferCulling& CommandBufferCulling::setCullFace(uint32_t mode)
{
    this->flags.setCullFace = true;
    this->cullFace = mode;
    return *this;
}

CommandBufferCulling& CommandBufferCulling::setFrontFace(uint32_t mode)
{
    this->flags.setFrontFace = true;
    this->frontFace = mode;
    return *this;
}

CommandBufferScissor& CommandBufferScissor::setEnable(bool enable)
{
    this->flags.setEnabled = true;
    this->flags.enabled = enable;
    return *this;
}

CommandBufferScissor& CommandBufferScissor::setBox(int x, int y, int width, int height)
{
    this->flags.setBox = true;
    this->x = (uint16_t)x;
    this->y = (uint16_t)y;
    this->width = (uint16_t)width;
    this->height = (uint16_t)height;
    return *this;
}

void UniformBuffer::setUniformInt1(const std::string &uniformName, int value)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::INT);
    data[index].update(&value, sizeof(int));
}
void UniformBuffer::setUniformInt2(const std::string &uniformName, int value1, int value2)
{
    int array[2] = {value1,value2};
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::INT2);
    data[index].update(&array, sizeof(int) * 2);
}

void UniformBuffer::setUniformInt3(const std::string &uniformName, int value1, int value2, int value3)
{
    int array[3] = {value1,value2,value3};
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::INT3);
    data[index].update(&array, sizeof(int) * 3);
}

void UniformBuffer::setUniformInt4(const std::string &uniformName, int value1, int value2, int value3, int value4)
{
    int array[4] = {value1,value2,value3,value4};
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::INT4);
    data[index].update(&array, sizeof(int) * 4);
}

void UniformBuffer::setUniformInt1v(const std::string& uniformName, const int* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::INT);
    data[index].update(value, sizeof(int) * size);
}

void UniformBuffer::setUniformInt2v(const std::string& uniformName, const int* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::INT2);
    data[index].update(value, sizeof(int) * 2 *size);
}

void UniformBuffer::setUniformInt3v(const std::string& uniformName, const int* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::INT3);
    data[index].update(value, sizeof(int) * 3 *size);
}

void UniformBuffer::setUniformInt4v(const std::string& uniformName, const int* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::INT4);
    data[index].update(value, sizeof(int) * 4 *size);
}

void UniformBuffer::setUniformFloat(const std::string &uniformName, float value)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::FLOAT);
    data[index].update(&value, sizeof(float) );
}

void UniformBuffer::setUniformFloat2(const std::string &uniformName, const Vec2& value)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::FLOAT2);
    data[index].update(&value, sizeof(float) * 2);
}

void UniformBuffer::setUniformFloat3(const std::string &uniformName, const Vec3& value)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::FLOAT3);
    data[index].update(&value, sizeof(float) * 3);
}

void UniformBuffer::setUniformFloat4(const std::string &uniformName, const Vec4& value)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::FLOAT4);
    data[index].update(&value, sizeof(float) * 4);
}

void UniformBuffer::setUniformFloat1v(const std::string& uniformName, const float* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::FLOAT);
    data[index].update(value, sizeof(float) * size);
}

void UniformBuffer::setUniformFloat2v(const std::string& uniformName, const Vec2* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::FLOAT2);
    data[index].update(value, sizeof(float) * 2 * size);
}

void UniformBuffer::setUniformFloat3v(const std::string& uniformName, const Vec3* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::FLOAT3);
    data[index].update(value, sizeof(float) * 3 * size);
}

void UniformBuffer::setUniformFloat4v(const std::string& uniformName, const Vec4* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::FLOAT4);
    data[index].update(value, sizeof(float) * 4 * size);
}

void UniformBuffer::setUniformMat3(const std::string &uniformName, const float* value)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::FMAT3X3);
    data[index].update(value, sizeof(float) * 9 );
}

void UniformBuffer::setUniformMat3v(const std::string &uniformName, const float* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::FMAT3X3);
    data[index].update(value, sizeof(float) * 9 * size);
}

void UniformBuffer::setUniformMat4(const std::string &uniformName, const Mat4& value)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && layout[index].type == ConstantType::FMAT4X4);
    data[index].update(&value, sizeof(float) * 16 );
}

void UniformBuffer::setUniformMat4v(const std::string &uniformName, const Mat4* value, unsigned int size)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == size && layout[index].type == ConstantType::FMAT4X4);
    data[index].update(value, sizeof(float) * 16 * size);
}

void UniformBuffer::setUniformTexture(const std::string &uniformName, GLuint textureId)
{
    auto index = getUniformIndex(uniformName);
    if(-1 == index) return;
    CC_ASSERT(layout[index].count == 1 && (layout[index].type == (ConstantType::TEXTURE) || layout[index].type == (ConstantType::TEXTURECUBE)));
    int textureUnit = (int)textureUniformSlots[uniformName];
    int array[2] = {textureUnit, (int)textureId};
    data[index].update(array, sizeof(int) * 2 );
}

int UniformBuffer::getUniformIndex(const std::string &uniformName) const
{
    auto iter = uniformIndex.find(uniformName);
    if(iter == uniformIndex.end()) return -1;
    else return iter->second;
}

void UniformBuffer::build()
{
    textureUniformSlots.clear();
    uniformIndex.clear();
    unsigned int textureUnitslot = 0;
    for(int index = 0; index < names.size(); ++index)
    {
        uniformIndex[names[index]] = index;
        if(layout[index].type == ConstantType::TEXTURE || layout[index].type == ConstantType::TEXTURECUBE)
        {
            textureUniformSlots[names[index]] = textureUnitslot;
            textureUnitslot++;
        }
    }
}

NS_CC_END
