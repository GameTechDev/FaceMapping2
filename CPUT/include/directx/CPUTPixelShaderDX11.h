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
#ifndef _CPUTPIXELSHADERDX11_H
#define _CPUTPIXELSHADERDX11_H

#include "CPUT.h"
#include "CPUTShaderDX11.h"

class CPUTPixelShaderDX11 : public CPUTShaderDX11
{
protected:
    ID3D11PixelShader *mpPixelShader;

    CPUTPixelShaderDX11() : mpPixelShader(NULL), CPUTShaderDX11(NULL) {}
    CPUTPixelShaderDX11(ID3D11PixelShader *pD3D11PixelShader, ID3DBlob *pBlob) : mpPixelShader(pD3D11PixelShader), CPUTShaderDX11(pBlob) {}

     // Destructor is not public.  Must release instead of delete.
    ~CPUTPixelShaderDX11(){ SAFE_RELEASE(mpPixelShader); }

public:
    static CPUTPixelShaderDX11 *Create(
        const std::string     &name,
        const std::string     &shaderMain,
        const std::string     &shaderProfile,
        CPUT_SHADER_MACRO *pShaderMacros=NULL
    );

    static CPUTPixelShaderDX11 *CreateFromMemory(
        const std::string     &name,
        const std::string     &shaderMain,
        const std::string     &shaderProfile,
        const char        *pShaderSource,
        CPUT_SHADER_MACRO *pShaderMacros=NULL
    );

    ID3D11PixelShader *GetNativePixelShader() { return mpPixelShader; }
};

#endif //_CPUTPIXELSHADER_H
