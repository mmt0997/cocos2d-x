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

CommandBufferScissor& CommandBufferScissor::setBox(int x_, int y_, int width_, int height_)
{
    this->flags.setBox = true;
    this->x = (uint16_t)x_;
    this->y = (uint16_t)y_;
    this->width = (uint16_t)width_;
    this->height = (uint16_t)height_;
    return *this;
}

uint32_t VertexElementTypeSize(VertexElementType type)
{
    static uint32_t size[int(VertexElementType::TOTAL_COUNT)] =
    {
        4,  //FLOAT,
        1,  //BYTE,
        1,  //UBYTE,
    };
    return size[int(type)];
}

GLenum   VertexElementTypeToGLType(VertexElementType t)
{
    static GLenum type[int(VertexElementType::TOTAL_COUNT)] =
    {
        GL_FLOAT,           //FLOAT,
        GL_BYTE,            //BYTE,
        GL_UNSIGNED_BYTE,   //UBYTE,
    };
    return type[int(t)];
}

GLenum GeometryTypeToGLType(GeometryType t)
{
    static GLenum type[int(GeometryType::TOTAL_COUNT)] =
    {
        GL_POINT,       // POINTS,
        GL_LINE,        // LINES,
        GL_TRIANGLES,   // TRIANGLES,
    };
    return type[int(t)];
}

NS_CC_END
