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
#ifndef _CPUTHULLSHADERDX11_H
#define _CPUTHULLSHADERDX11_H

#include "CPUT.h"
#include "CPUTShaderDX11.h"

class CPUTHullShaderDX11 : public CPUTShaderDX11
{
protected:
    ID3D11HullShader *mpHullShader;

    CPUTHullShaderDX11() : mpHullShader(NULL), CPUTShaderDX11(NULL) {}
    CPUTHullShaderDX11(ID3D11HullShader *pD3D11HullShader, ID3DBlob *pBlob) : mpHullShader(pD3D11HullShader), CPUTShaderDX11(pBlob) {}

    // Destructor is not public.  Must release instead of delete.
    ~CPUTHullShaderDX11(){ SAFE_RELEASE(mpHullShader); }

public:
    static CPUTHullShaderDX11 *Create(
        const std::string     &name,
        const std::string     &shaderMain,
        const std::string     &shaderProfile,
        CPUT_SHADER_MACRO *pShaderMacros=NULL
    );
    static CPUTHullShaderDX11 *CreateFromMemory(
        const std::string     &name,
        const std::string     &shaderMain,
        const std::string     &shaderProfile,
        const char        *pShaderSource,
        CPUT_SHADER_MACRO *pShaderMacros=NULL
    );

    ID3DBlob *GetBlob() { return mpBlob; }
    ID3D11HullShader *GetNativeHullShader() { return mpHullShader; }
};

#endif //_CPUTHULLSHADER_H
