/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUTGEOMETRYSHADERDX11_H
#define _CPUTGEOMETRYSHADERDX11_H

#include "CPUT.h"
#include "CPUTShaderDX11.h"

class CPUTGeometryShaderDX11 : public CPUTShaderDX11
{
protected:
    ID3D11GeometryShader *mpGeometryShader;

    CPUTGeometryShaderDX11() : mpGeometryShader(NULL), CPUTShaderDX11(NULL) {}
    CPUTGeometryShaderDX11(ID3D11GeometryShader *pD3D11GeometryShader, ID3DBlob *pBlob) : mpGeometryShader(pD3D11GeometryShader), CPUTShaderDX11(pBlob) {}

    // Destructor is not public.  Must release instead of delete.
    ~CPUTGeometryShaderDX11(){ SAFE_RELEASE(mpGeometryShader); }

public:
    static CPUTGeometryShaderDX11 *CreateFromNativeGeometryShader(
        ID3D11GeometryShader *pNativeShader
    );
    static CPUTGeometryShaderDX11 *Create(
        const std::string     &name,
        const std::string     &shaderMain,
        const std::string     &shaderProfile,
        CPUT_SHADER_MACRO *pShaderMacros=NULL
    );
    static CPUTGeometryShaderDX11 *CreateFromMemory(
        const std::string     &name,
        const std::string     &shaderMain,
        const std::string     &shaderProfile,
        const char        *pShaderSource,
        CPUT_SHADER_MACRO *pShaderMacros=NULL
    );

    ID3DBlob *GetBlob() { return mpBlob; }
    ID3D11GeometryShader *GetNativeGeometryShader() { return mpGeometryShader; }
};

#endif //_CPUTGEOMETRYSHADER_H
