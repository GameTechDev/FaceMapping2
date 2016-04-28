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

#ifndef _CPUTRENDERTARGET_OGL_H
#define _CPUTRENDERTARGET_OGL_H

#include "CPUT_OGL.h"

class CPUTMaterial;
class CPUTRenderParameters;
class CPUTTexture;
class CPUTBuffer;
class CPUTRenderTargetDepth;

class CPUTTextureOGL;
class CPUTFramebufferOGL
{
public:

	static CPUTFramebufferOGL* Create(CPUTTextureOGL *pColor, CPUTTextureOGL *pDepth, CPUTTextureOGL *pStencil = NULL);

    ~CPUTFramebufferOGL();

    GLuint GetFramebufferName();
    CPUTTextureOGL* GetColorTexture();
    CPUTTextureOGL* GetDepthTexture();

    void SetActive();
    static void UnSetActive();

    // TODO: Support this?
    //void SetColorTexture(CPUTTextureOGL *pTexture);
    //void SetDepthTexture(CPUTTextureOGL *pTexture);
    
private:

    CPUTFramebufferOGL();

    CPUTFramebufferOGL(
        CPUTTextureOGL *pColor,
        CPUTTextureOGL *pDepth,
        CPUTTextureOGL *pStencil = NULL);

    GLuint mFramebuffer;
    CPUTTextureOGL *mpColor, *mpDepth, *mpStencil;
};
#endif // _CPUTRENDERTARGET_H
