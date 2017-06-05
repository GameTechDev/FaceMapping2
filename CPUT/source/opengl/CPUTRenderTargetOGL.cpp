/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or imlied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

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
   

