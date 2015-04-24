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

#ifndef __SPRITE_NODE_CC3DSCENESPRITE_H__
#define __SPRITE_NODE_CC3DSCENESPRITE_H__

#include <string>
#include "2d/CCNode.h"
#include "renderer/CCQuadCommand.h"
#include "renderer/CCGroupCommand.h"
#include "renderer/CCCustomCommand.h"

NS_CC_BEGIN


class CC_DLL Sprite3DScene : public Node
{
public:
    class RenderToTexture
    {
        Size _sizeInPixels;
        bool _needDepthStencil;

        //glcontext data
        GLuint _colorTexture;
        GLuint _dsRenderBuffer;
        GLuint _fbo;
    public:
        RenderToTexture(Size size, bool needDepthStencil);
        ~RenderToTexture();
        GLuint getColorTexture() const { return _colorTexture; }
        void DrawToFBO(std::function<void()> drawFunc);
    };
    
public:
    static Sprite3DScene* create(const Size& size);
    
    bool initWithSize(const Size& size);
    
    virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
    virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;
CC_CONSTRUCTOR_ACCESS:
    Sprite3DScene();
    ~Sprite3DScene();
private:
    Size _size;
    //Scene* _3DScene;
    RenderToTexture* _rt;
    QuadCommand _quadCommand;
    //GroupCommand _rtRenderGroup;
    CustomCommand _rtDrawCommand;
    //CustomCommand _rtEndCommand;
    V3F_C4B_T2F_Quad _quad;
};

NS_CC_END

#endif /* defined(__SPRITE_NODE_CC3DSCENESPRITE_H__) */
