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
#include <vector>
#include <string>

NS_CC_BEGIN

enum class CommandBufferType
{
    INVALID,
    DEPTH,
    BLEND,
    STENCIL,
    CULLING,
    SCISSOR,
    
    STEAMS,
    DRAW,
    DRAWINDEXED,
    GPUPROGRAM,
    UNIFORM,
};

class CommandBuffer
{
public:
    CommandBufferType _type;
    
    // this function is a temp function for restruct code, it will be removed at last
    void apply();
protected:
    CommandBuffer(CommandBufferType type = CommandBufferType::INVALID): _type(type){}
};

struct CommandBufferDepth : public CommandBuffer
{
public:
    /**
     Constructor.
     */
    CommandBufferDepth(): CommandBuffer(CommandBufferType::DEPTH), flag(0)
    {
    }
    
    CommandBufferDepth& setEnable(bool enable);
    CommandBufferDepth& setFunction(uint32_t func);
    CommandBufferDepth& setWriteMask(bool enable);
    CommandBufferDepth& setRangef(float near, float far);
    CommandBufferDepth& setPolygonOffset(float factor, float units);

    union {
        bool    flag;
        struct
        {
            unsigned setEnabled:1;
            unsigned setWriteMask:1;
            unsigned setFunction:1;
            unsigned setRangef:1;
            unsigned setPolygonOffset:1;
            unsigned enabled:1;
            unsigned writeEnabled:1;
        }flags;
    };
    uint32_t function;
    float rangefNear;
    float rangfFar;
    float polygonOffsetFactor;
    float polygonOffsetUnits;
};

#define TEST_COMMAND_BUFFER_BLEND 1
struct CommandBufferBlend : public CommandBuffer
{
public:
    /**
     Constructor.
     */
    CommandBufferBlend():CommandBuffer(CommandBufferType::BLEND), flag(0)
    {
    }
    
    CommandBufferBlend& setEnable(bool enable);
    CommandBufferBlend& setColor(float r, float g, float b, float a);
    CommandBufferBlend& setEquation(uint32_t mode, uint32_t modeAlpha = GL_INVALID_ENUM);
    CommandBufferBlend& setFunction(uint32_t src, uint32_t dst, uint32_t srcAlpha = GL_INVALID_ENUM, uint32_t dstAlpha = GL_INVALID_ENUM);

    union {
        bool    flag;
        struct
        {
            unsigned setEnabled:1;
            unsigned setColor:1;
            unsigned setEquation:1;
            unsigned setFunction:1;
            unsigned enabled:1;
        }flags;
    };
    struct {
        float r;
        float g;
        float b;
        float a;
    } color;
    
    uint32_t equation[2];
    uint32_t srcFunc[2];
    uint32_t dstFunc[2];
};

enum class FaceEnum
{
    FRONT,
    BACK,
    FRONT_AND_BACK,
};

#define TEST_COMMAND_BUFFER_STENCIL 1
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
            unsigned enabled:1;
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

#define TEST_COMMAND_BUFFER_CULLING 1

class CommandBufferCulling : public CommandBuffer
{
public:
    /** Constructor.*/
    CommandBufferCulling():CommandBuffer(CommandBufferType::CULLING), flag(0)
    {
    }
    
    CommandBufferCulling& setEnable(bool enable);
    CommandBufferCulling& setCullFace(uint32_t mode);
    CommandBufferCulling& setFrontFace(uint32_t mode);
    
    union {
        bool    flag;
        struct
        {
            unsigned setEnabled:1;
            unsigned setCullFace:1;
            unsigned setFrontFace:1;
            unsigned enabled:1;
        }flags;
    };
    uint32_t cullFace;
    uint32_t frontFace;
};

#define TEST_COMMAND_BUFFER_SCISSOR 1

class CommandBufferScissor : public CommandBuffer
{
public:
    /** Constructor.*/
    CommandBufferScissor():CommandBuffer(CommandBufferType::SCISSOR), flag(0)
    {
    }
    
    CommandBufferScissor& setEnable(bool enable);
    CommandBufferScissor& setBox(int x, int y, int width, int height);
    
    union {
        bool    flag;
        struct
        {
            unsigned setEnabled:1;
            unsigned setBox:1;
            unsigned enabled:1;
        }flags;
    };
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
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
    BYTE,
    UBYTE,
};

class VertexElement
{
public:
    VertexSemantic _semantic;
    VertexElementType _type;
    int _count; //1-4
    bool _normalize;
    VertexElement(VertexSemantic semantic, VertexElementType type, int count, bool normalize = true)
    :_semantic(semantic), _type(type), _count(count), _normalize(normalize)
    {
    }
};

typedef std::vector<VertexElement> VertexBufferLayout;

class VertexBuffer;
struct VertexStream
{
    VertexBuffer* buffer;
    VertexBufferLayout layout;
};

typedef std::vector<VertexStream> VertexStreams;


enum class GeometryType
{
    Points,
    Lines,
    Triangles,
};

struct CommandBufferVertexStreams : public CommandBuffer
{
    VertexStreams streams;
    
public:
    CommandBufferVertexStreams(const VertexStreams& stream)
    : CommandBuffer(CommandBufferType::STEAMS), streams(stream)
    {
        
    }
};

struct CommandBufferDraw : public CommandBuffer
{
    int start;
    int count;
    GeometryType type;
public:
    CommandBufferDraw(GeometryType type_, int start_, int count_)
    : CommandBuffer(CommandBufferType::DRAW), type(type_),start(start_), count(count_)
    {
        
    }
};

class IndexBuffer;
struct CommandBufferDrawIndexed : public CommandBuffer
{
    IndexBuffer* indices;
    int start;
    int count;
    GeometryType type;
public:
    CommandBufferDrawIndexed(IndexBuffer* buffer_,GeometryType type_, int start_, int count_)
    : CommandBuffer(CommandBufferType::DRAWINDEXED), indices(buffer_),type(type_),start(start_), count(count_)
    {
        
    }
};

class GLProgram;
struct CommandBufferGPUProgram : public CommandBuffer
{
    GLProgram* program;
public:
    CommandBufferGPUProgram(GLProgram* glProgram)
    :CommandBuffer(CommandBufferType::GPUPROGRAM), program(glProgram)
    {
    }
};

class UniformBuffer
{
public:
    enum class ConstantType
    {
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        FMAT4X4,
        INT,
        INT2,
        INT3,
        INT4,
        TEXTURE,
        
    };
    
    typedef std::vector<std::string> ConstantNames;

    struct ConstantElement
    {
        int constantSlot;
        ConstantType type;
        int count;
    };

    struct Data
    {
        //data stored, in GL, texture would use int2(textureunit, textureID) for recording
        void* data;
    };
    
    typedef std::vector<ConstantElement> ConstantLayouts;
    typedef std::vector<Data> ConstantData;
    
    ConstantNames names;
    ConstantLayouts layout;
    ConstantData data;
};

struct CommandBufferUniform : public CommandBuffer
{
    UniformBuffer buffer;
public:
    CommandBufferUniform(UniformBuffer buff)
    :CommandBuffer(CommandBufferType::UNIFORM), buffer(buff)
    {
    }
};

struct CommandBufferRenderTargetViewport : public CommandBuffer
{
    
};

NS_CC_END

#endif /* defined(__CC_COMMAND_BUFFER_H__) */
