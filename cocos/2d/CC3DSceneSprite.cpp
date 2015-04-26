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

#include "CC3DSceneSprite.h"
#include "renderer/CCRenderer.h"

NS_CC_BEGIN
//public:
//RenderToTexture(Size size, bool needDepthStencil);
//~RenderToTexture();
//};
//
//public:
//static Sprite3DScene* create(Size size);
//virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
//virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;
//CC_CONSTRUCTOR_ACCESS:
//Sprite3DScene();
//~Sprite3DScene();


//Size _size;
//Scene* _3DScene;
//RenderToTexture* _rt;
//QuadCommand _quadCommand;
//V3F_C4B_T2F_Quad _quad;

Sprite3DScene::RenderToTexture::RenderToTexture(Size size, bool needDepthStencil)
{
    _sizeInPixels = size;
    _needDepthStencil = needDepthStencil;
    
    //init gldata
    {
        GLint oldFrameBuffer;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
        glGenTextures(1, &_colorTexture);
        glBindTexture(GL_TEXTURE_2D, _colorTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width, size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        if(needDepthStencil)
        {
            glGenRenderbuffers(1, &_dsRenderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, _dsRenderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.width, size.height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTexture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _dsRenderBuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _dsRenderBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
        
        
    };
}

Sprite3DScene::RenderToTexture::~RenderToTexture()
{
    glDeleteFramebuffers(1, &_fbo);
    glDeleteTextures(1, &_colorTexture);
    glDeleteRenderbuffers(1, &_dsRenderBuffer);
    _fbo = _colorTexture = _dsRenderBuffer = 0;
}

void Sprite3DScene::RenderToTexture::DrawToFBO(std::function<void ()> drawFunc)
{
    GLint oldFrameBuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    if(drawFunc)
        drawFunc();
    glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
}

Sprite3DScene::Sprite3DScene()
{
    
}

Sprite3DScene::~Sprite3DScene()
{
    CC_SAFE_DELETE(_rt);
}

Sprite3DScene* Sprite3DScene::create(const Size& size)
{
    auto result = new (std::nothrow) Sprite3DScene();
    if(result && result->initWithSize(size))
    {
        result->autorelease();
        return result;
    }
    else
    {
        return nullptr;
    }
}

bool Sprite3DScene::initWithSize(const Size &size)
{
    bool result(true);
    _size = size;
    
    _quad.bl.colors = _quad.br.colors = _quad.tl.colors = _quad.tr.colors = Color4B(255, 255, 255, 255);
    _quad.bl.texCoords = Tex2F(0, 0);
    _quad.br.texCoords = Tex2F(1, 0);
    _quad.tl.texCoords = Tex2F(0, 1);
    _quad.tr.texCoords = Tex2F(1, 1);
    
    _quad.bl.vertices = Vec3(0, 0, 0);
    _quad.br.vertices = Vec3(_size.width, 0, 0);
    _quad.tl.vertices = Vec3(0, _size.height, 0);
    _quad.tr.vertices = Vec3(_size.width, _size.height, 0);
    
    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
    
    //init rt
    _rt = new Sprite3DScene::RenderToTexture(size,true);
    
    std::function<void()> func = nullptr;
    _rtDrawCommand.func = std::bind(&Sprite3DScene::RenderToTexture::DrawToFBO, _rt, func);
    return result;
}

void Sprite3DScene::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    renderer->addCommand(&_rtDrawCommand);
    Node::visit(renderer, parentTransform, parentFlags);
}

void Sprite3DScene::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
    _quadCommand.init(_globalZOrder, _rt->getColorTexture(), getGLProgramState(), BlendFunc::DISABLE, &_quad, 1, transform, flags);
    renderer->addCommand(&_quadCommand);
}

NS_CC_END
