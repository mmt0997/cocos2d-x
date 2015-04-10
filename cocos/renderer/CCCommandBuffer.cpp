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

CommandBufferStencil& CommandBufferStencil::setEnable(bool enable)
{
    this->flags.setEnabled = true;
    this->flags.enabled = enable;
    return *this;
}

CommandBufferStencil& CommandBufferStencil::setFunc(uint32_t func, int32_t ref, uint32_t mask, FaceEnum face)
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
        this->func[0].func = (uint16_t)func;
        this->func[0].ref = (uint16_t)ref;
        this->func[0].mask = (uint16_t)mask;
    }
    if(this->flags.setFuncBack)
    {
        this->func[1].func = (uint16_t)func;
        this->func[1].ref = (uint16_t)ref;
        this->func[1].mask = (uint16_t)mask;
    }
    return *this;
}

CommandBufferStencil& CommandBufferStencil::setMask(uint32_t mask, FaceEnum face)
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
        this->mask[0] = mask;
    }
    if(this->flags.setMaskBack)
    {
        this->mask[1] = mask;
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

NS_CC_END
