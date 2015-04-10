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

#ifndef __CC_COMMAND_BUFFER_H__
#define __CC_COMMAND_BUFFER_H__

#include <stdio.h>

#include "platform/CCGL.h"
#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

enum class CommandBufferType
{
    INVALID,
    BLEND,
    DEPTH,
    STENCIL,
};

class CommandBuffer
{
public:
    CommandBufferType _type;    
    virtual ~CommandBuffer() {}
    
    // this function is a temp function for restruct code, it will be removed at last
    void apply();
protected:
    CommandBuffer(CommandBufferType type = CommandBufferType::INVALID): _type(type){}
};

struct CommandBufferDepth : public CommandBuffer
{
    /**
     @{
     RenderState used for Depth test, if isEnabled is false, compareFunction will be discard.
     isEnabled is false by default.
     */
    bool isEnabled;
    GLenum compareFunction;
    /**@}*/
public:
    /**
     Constructor.
     */
    CommandBufferDepth()
    : CommandBuffer(CommandBufferType::DEPTH)
    , isEnabled(false), compareFunction(GL_ALWAYS)
    {
    }
    /**
     Constructor.
     @param enable Enable state of depth test.
     @param compareFunc compareFunction used for depth test.
     */
    CommandBufferDepth(bool enable, GLenum compareFunc)
    : CommandBuffer(CommandBufferType::DEPTH)
    , isEnabled(enable), compareFunction(compareFunc)
    {
    }
};

struct CommandBufferBlend : public CommandBuffer
{
    /**
     @{
     RenderState used for blend , if isEnabled is false, other states will be discard.
     isEnabled is false by default.
     */
    bool isEnabled;
    GLenum srcFactor;
    GLenum dstFactor;
    GLenum blendMode;
    /**@}*/
public:
    /**
     Constructor.
     */
    CommandBufferBlend()
    : CommandBuffer(CommandBufferType::BLEND)
    , isEnabled(false), srcFactor(GL_ONE), dstFactor(GL_ZERO),blendMode(GL_FUNC_ADD)
    {
    }
    /**
     Constructor.
     @param enable Enable state for blend.
     @param src source factor.
     @param dst destiny factor.
     @param mode Add or subtract.
     */
    CommandBufferBlend(bool enable, GLenum src, GLenum dst, GLenum mode)
    : CommandBuffer(CommandBufferType::BLEND)
    , isEnabled(enable), srcFactor(src), dstFactor(dst),blendMode(mode)
    {
    }
};

enum class FaceEnum
{
    FRONT,
    BACK,
    FRONT_AND_BACK,
};

/** Set stencil buffer status.
 */
class CommandBufferStencil : public CommandBuffer
{
public:
    /** Constructor.*/
    CommandBufferStencil():CommandBuffer(CommandBufferType::STENCIL), flag(0){}
    
    CommandBufferStencil& setEnable(bool enable);
    CommandBufferStencil& setFunc(uint32_t func, int32_t ref = 0, uint32_t mask = -1, FaceEnum face = FaceEnum::FRONT_AND_BACK);
    CommandBufferStencil& setMask(uint32_t mask, FaceEnum face = FaceEnum::FRONT_AND_BACK);
    CommandBufferStencil& setOp(uint32_t sfail, uint32_t dpfail, uint32_t dppass, FaceEnum face = FaceEnum::FRONT_AND_BACK);
    
    union {
        bool    flag;
        struct
        {
            unsigned setEnabled:1;
            unsigned setFuncFront:1;
            unsigned setFuncBack:1;
            unsigned setMaskFront:1;
            unsigned setMaskBack:1;
            unsigned setOpFront:1;
            unsigned setOpBack:1;
            unsigned enabled;
        }flags;
    };
    struct  // StencilFuncParam
    {
        uint16_t func;
        uint16_t ref;
        uint16_t mask;
    } func[2];
    uint32_t mask[2];
    struct // stencilOpParam
    {
        uint16_t sfail;
        uint16_t dpfail;
        uint16_t dppass;
    } op[2];
};

//todo: add more commandBuffer

struct CommandBufferDraw : public CommandBuffer
{

};

NS_CC_END

#endif /* defined(__CC_COMMAND_BUFFER_H__) */
