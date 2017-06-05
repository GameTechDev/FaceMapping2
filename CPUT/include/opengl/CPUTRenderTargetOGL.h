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
