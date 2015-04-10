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
    DEPTH,
    BLEND,
};

class CommandBuffer
{
private:
    void (*applyImplementation)(CommandBuffer*);
public:
    CommandBufferType _type;    
    CommandBuffer(void (*applyFunction)(CommandBuffer*)): _type(CommandBufferType::INVALID), applyImplementation(applyFunction) {}
    
    void apply()
    {
        applyImplementation(this);
    }
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
    : CommandBuffer(rendererApplyDepth)
    , isEnabled(false), compareFunction(GL_ALWAYS)
    {
        _type = CommandBufferType::DEPTH;
    }
    /**
     Constructor.
     @param enable Enable state of depth test.
     @param compareFunc compareFunction used for depth test.
     */
    CommandBufferDepth(bool enable, GLenum compareFunc)
    : CommandBuffer(rendererApplyDepth)
    , isEnabled(enable), compareFunction(compareFunc)
    {
        _type = CommandBufferType::DEPTH;
    }
    
private:
    static void rendererApplyDepth(CommandBuffer*);
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
    : CommandBuffer(rendererApplyBlend)
    , isEnabled(false), srcFactor(GL_ONE), dstFactor(GL_ZERO),blendMode(GL_FUNC_ADD)
    {
        _type = CommandBufferType::BLEND;
    }
    /**
     Constructor.
     @param enable Enable state for blend.
     @param src source factor.
     @param dst destiny factor.
     @param mode Add or subtract.
     */
    CommandBufferBlend(bool enable, GLenum src, GLenum dst, GLenum mode)
    : CommandBuffer(rendererApplyBlend)
    , isEnabled(enable), srcFactor(src), dstFactor(dst),blendMode(mode)
    {
        _type = CommandBufferType::BLEND;
    }
private:
    static void rendererApplyBlend(CommandBuffer*);
};

//todo: add more commandBuffer

enum VertexSemantic
{
    PLACEHOLDER = 0,    //no semantic, just used as place holders
    POSIITON = PLACEHOLDER + 1,       //position
    NORMAL = PLACEHOLDER + 2,         //normal
    COLOR = PLACEHOLDER + 3,          //color
    BLENDWEIGHT = PLACEHOLDER + 4,    //blend weight, used for hardware skinning
    BLENDINDEX = PLACEHOLDER + 5,     //blend index, used for hardware skinning
    TEXCOORD0 = PLACEHOLDER + 6,      //texture coordinate 0-7
    TEXCOORD1 = PLACEHOLDER + 7,
    TEXCOORD2 = PLACEHOLDER + 8,
    TEXCOORD3 = PLACEHOLDER + 9,
    TEXCOORD4 = PLACEHOLDER + 10,
    TEXCOORD5 = PLACEHOLDER + 11,
    TEXCOORD6 = PLACEHOLDER + 12,
    TEXCOORD7 = PLACEHOLDER + 13,
    
    COUNT = 14,
};

enum class VertexElementType
{
    FLOAT,
    INT,
    BYTE,
};

class VertexElement
{
public:
    VertexSemantic _semantic;
    VertexElementType _type;
    int _count; //1-4
    
    VertexElement(VertexSemantic semantic, VertexElementType type, int count)
    :_semantic(semantic), _type(type), _count(count)
    {
    }
};

typedef std::vector<VertexElement> VertexBufferLayout;

struct VertexStream
{
    VertexBuffer* buffer;
    VertexBufferLayout layout;
};

typedef std::vector<VertexStream> VertexStreams;

typedef unsigned char SemanticAttributeIndexMap[VertexSemantic::COUNT];

SemanticAttributeIndexMap s_testMap;

enum class GeometryType
{
    Points,
    Lines,
    Triangles,
};

struct CommandBufferBlendVertexStreams : public CommandBuffer
{
    VertexStreams streams;
    
public:
    CommandBufferBlendVertexStreams(const VertexStreams& stream)
    : streams(stream)
    {
        
    }
};

struct CommandBufferDraw : public CommandBuffer
{
    int start;
    int count;
    GeometryType type;
public:
    CommandBufferDraw(GeometryType type,int start, int count)
    :type(type),start(start), count(count)
    {
        
    }
};

struct CommandBufferDrawIndexed : public CommandBuffer
{
    IndexBuffer* indices;
    int start;
    int count;
    GeometryType type;
public:
    CommandBufferDrawIndexed(IndexBuffer* buffer,GeometryType type,int start, int count)
    :indices(buffer),type(type),start(start), count(count)
    {
        
    }
};

NS_CC_END


#endif /* defined(__CC_COMMAND_BUFFER_H__) */
