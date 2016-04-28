//--------------------------------------------------------------------------------------
// Copyright 2013 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------

#include "CPUTRenderTargetOGL.h"
//#include "CPUTAssetLibrary.h"
#include "CPUTTextureOGL.h"

CPUTFramebufferOGL::CPUTFramebufferOGL() 
        : mFramebuffer(0), mpColor(NULL), mpDepth(NULL), mpStencil(NULL)
{};

CPUTFramebufferOGL::~CPUTFramebufferOGL()
{
    SAFE_RELEASE(mpColor);
    SAFE_RELEASE(mpDepth);
    SAFE_RELEASE(mpStencil);
    if(mFramebuffer == 0)
    {
        glDeleteFramebuffers(1, &mFramebuffer);
    }
}
CPUTFramebufferOGL::CPUTFramebufferOGL(CPUTTextureOGL *pColor,
        CPUTTextureOGL *pDepth,
        CPUTTextureOGL *pStencil)
{
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	if(pDepth)
	{
		GLuint depthTexture = pDepth->GetTexture();
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0));
	}
	if(pColor)
	{
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pColor->GetTexture(), 0));
		GLenum pDrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		GL_CHECK(glDrawBuffers(1, pDrawBuffers));
	}
	else
	{
		//glDrawBuffer(GL_NONE);
	}
    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Failed creating Framebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    mFramebuffer = framebuffer;
    mpColor = pColor;
    if(pColor != NULL)
        pColor->AddRef();
    if(pDepth != NULL)
        pDepth->AddRef();
    mpDepth = pDepth;
    if(pStencil != NULL)
        pStencil->AddRef();
    mpStencil = pStencil;

    return;
} 

void CPUTFramebufferOGL::SetActive()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

void CPUTFramebufferOGL::UnSetActive()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint CPUTFramebufferOGL::GetFramebufferName() { return mFramebuffer;}
CPUTTextureOGL* CPUTFramebufferOGL::GetColorTexture() { return mpColor;}
CPUTTextureOGL* CPUTFramebufferOGL::GetDepthTexture() { return mpDepth;}

CPUTFramebufferOGL* CPUTFramebufferOGL::Create(CPUTTextureOGL *pColor, CPUTTextureOGL *pDepth, CPUTTextureOGL *pStencil)
{
	return new  CPUTFramebufferOGL(pColor, pDepth, pStencil);
}

//void SetColorTexture(CPUTTextureOGL *pTexture)
//void SetDepthTexture(CPUTTextureOGL *pTexture);
   

