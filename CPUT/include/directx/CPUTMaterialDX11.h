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
#ifndef __CPUTMATERIALDX11_H__
#define __CPUTMATERIALDX11_H__

#include "CPUTMaterial.h"

#ifdef CPUT_FOR_DX11
class CPUTPixelShaderDX11;
class CPUTComputeShaderDX11;
class CPUTVertexShaderDX11;
class CPUTGeometryShaderDX11;
class CPUTHullShaderDX11;
class CPUTDomainShaderDX11;
class CPUTModel;
class CPUTRenderStateBlockDX11;
class CPUTShaderParameters
{
public:
    int                       mTextureCount;
    std::string               *mpTextureName;
    int                      *mpTextureBindPoint;
    CPUTTexture               *mpTexture[CPUT_MATERIAL_MAX_TEXTURE_SLOTS];
    int                       mBindViewMin;
    int                       mBindViewMax;

    int                       mUAVCount;
    std::string               *mpUAVName;
    int                      *mpUAVBindPoint;
    CPUTBuffer                *mpUAV[CPUT_MATERIAL_MAX_UAV_SLOTS];
    int                       mBindUAVMin;
    int                       mBindUAVMax;
    
    int                       mConstantBufferCount;
    std::string               *mpConstantBufferName;
    int                      *mpConstantBufferBindPoint;
    CPUTBuffer                *mpConstantBuffer[CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS];
    int                       mBindConstantBufferMin;
    int                       mBindConstantBufferMax;


    CPUTShaderParameters() :
        mTextureCount(0),
        mpTextureName(NULL),
        mpTextureBindPoint(NULL),
        mBindViewMin(CPUT_MATERIAL_MAX_SRV_SLOTS),
        mBindViewMax(0),
        mUAVCount(0),
        mpUAVName(NULL),
        mpUAVBindPoint(NULL),
        mBindUAVMin(CPUT_MATERIAL_MAX_UAV_SLOTS),
        mBindUAVMax(0),
        mConstantBufferCount(0),
        mpConstantBufferName(NULL),
        mpConstantBufferBindPoint(NULL),
        mBindConstantBufferMin(CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS),
        mBindConstantBufferMax(0)
    {
        memset( mpTexture,              0, sizeof(mpTexture) );
        memset( mpUAV,                  0, sizeof(mpUAV) );
        memset( mpConstantBuffer,       0, sizeof(mpConstantBuffer) );
    };
    ~CPUTShaderParameters();
};

class CPUTMaterialDX11 : public CPUTMaterial
{
protected:
    CPUTPixelShaderDX11      *mpPixelShader;
    CPUTComputeShaderDX11    *mpComputeShader;
    CPUTVertexShaderDX11     *mpVertexShader;
    CPUTGeometryShaderDX11   *mpGeometryShader;
    CPUTHullShaderDX11       *mpHullShader;
    CPUTDomainShaderDX11     *mpDomainShader;

public:
    CPUTShaderParameters     mPixelShaderParameters;
    CPUTShaderParameters     mComputeShaderParameters;
    CPUTShaderParameters     mVertexShaderParameters;
    CPUTShaderParameters     mGeometryShaderParameters;
    CPUTShaderParameters     mHullShaderParameters;
    CPUTShaderParameters     mDomainShaderParameters;

protected:

    CPUTMaterialDX11();
    ~CPUTMaterialDX11();  // Destructor is not public.  Must release instead of delete.

    void ReadShaderSamplersAndTextures(   ID3DBlob *pBlob, CPUTShaderParameters *pShaderParameter );

    void BindTextures(        CPUTShaderParameters &params, const CPUTModel *pModel=NULL, int meshIndex=-1 );
    void BindUAVs(            CPUTShaderParameters &params, const CPUTModel *pModel=NULL, int meshIndex=-1 );
    void BindConstantBuffers( CPUTShaderParameters &params, const CPUTModel *pModel=NULL, int meshIndex=-1 );

public:

	static CPUTMaterialDX11* Create();

    CPUTResult    LoadMaterial(
        const std::string   &fileName,
        CPUT_SHADER_MACRO *pShaderMacros=NULL
    );
    CPUTVertexShaderDX11   *GetVertexShader()   { return mpVertexShader; }
    CPUTPixelShaderDX11    *GetPixelShader()    { return mpPixelShader; }
    CPUTGeometryShaderDX11 *GetGeometryShader() { return mpGeometryShader; }
    CPUTComputeShaderDX11  *GetComputeShader()  { return mpComputeShader; }
    CPUTDomainShaderDX11   *GetDomainShader()   { return mpDomainShader; }
    CPUTHullShaderDX11     *GetHullShader()     { return mpHullShader; }

	
    void OverrideGeometryShader(CPUTGeometryShaderDX11 *pGSShader);
    
    virtual void OverridePSTexture(int index, CPUTTexture *texture);

    void SetRenderStates();

	virtual CPUTTexture *GetPixelShaderTexture(int index);
};
#endif
#endif
