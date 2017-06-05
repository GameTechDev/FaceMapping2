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
#ifndef __CPUTMATERIALOGL_H__
#define __CPUTMATERIALOGL_H__

#include "CPUTMaterial.h"

#include <vector>

class CPUTShaderOGL;

class CPUTShaderParameters
{
public:
    UINT                       mTextureCount;
    std::vector<std::string>   mpTextureName;
    std::vector<GLint>         mpTextureBindPoint;
    std::vector<GLint>         mpTextureLocation;
    CPUTTexture               *mpTexture[CPUT_MATERIAL_MAX_TEXTURE_SLOTS];
    UINT                       mBindTextureMin;
    UINT                       mBindTextureMax;

    UINT                       mUAVCount;
    std::vector<std::string>   mpUAVName;
    std::vector<GLint>         mpUAVBindPoint;
    std::vector<GLint>         mpUAVLocation;
    GLint                      mpUAVMode[CPUT_MATERIAL_MAX_UAV_SLOTS];
    CPUTTexture               *mpUAV[CPUT_MATERIAL_MAX_UAV_SLOTS];
    UINT                       mBindUAVMin;
    UINT                       mBindUAVMax;

    UINT                       mConstantBufferCount;
    std::vector<std::string>   mConstantBufferName;
    std::vector<GLint>         mConstantBufferBindPoint;
    CPUTBuffer                *mpConstantBuffer[CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS];
    UINT                       mBindConstantBufferMin;
    UINT                       mBindConstantBufferMax;

    CPUTShaderParameters() :
        mTextureCount(0),
        mUAVCount(0),
        mConstantBufferCount(0),
        mBindTextureMin(CPUT_MATERIAL_MAX_SRV_SLOTS),
        mBindTextureMax(0),
        mBindUAVMin(CPUT_MATERIAL_MAX_UAV_SLOTS),
        mBindUAVMax(0),
        mBindConstantBufferMin(CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS),
        mBindConstantBufferMax(0)
    {
        // initialize texture slot list to null
        for(int ii=0; ii<CPUT_MATERIAL_MAX_TEXTURE_SLOTS; ii++)
        {
            mpTexture[ii] = NULL;
        }
        for(int ii=0; ii<CPUT_MATERIAL_MAX_UAV_SLOTS; ii++)
        {
            mpUAV[ii] = NULL;
        }
        for(int ii=0; ii<CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS; ii++)
        {
            mpConstantBuffer[ii] = NULL;
        }
    };
    ~CPUTShaderParameters();
    void CloneShaderParameters( CPUTShaderParameters *pShaderParameter );
};

static const int CPUT_NUM_SHADER_PARAMETER_LISTS = 7;
class CPUTMaterialOGL : public CPUTMaterial
{
protected:
    CPUTMaterialOGL();
    ~CPUTMaterialOGL();  // Destructor is not public.  Must release instead of delete.
    void ReadShaderSamplersAndTextures( GLuint shaderProgram, CPUTShaderParameters *pShaderParameter );
    
    void BindTextures(        CPUTShaderParameters &params);
    void BindConstantBuffers( CPUTShaderParameters &params);
	void BindUAVs( CPUTShaderParameters &params );

    CPUTShaderOGL *mpFragmentShader;
    CPUTShaderOGL *mpVertexShader;
	CPUTShaderOGL *mpControlShader;
	CPUTShaderOGL *mpEvaluationShader;
	CPUTShaderOGL *mpGeometryShader;

    GLuint mShaderProgram;
public:
    CPUTResult            LoadMaterial(
        const std::string   &fileName,
        CPUT_SHADER_MACRO* pShaderMacros=NULL
    );

    CPUTShaderOGL   *GetVertexShader()      { return mpVertexShader; }
    CPUTShaderOGL   *GetPixelShader()       { return mpFragmentShader; }
	CPUTShaderOGL	*GetControlShader()     { return mpControlShader; }
	CPUTShaderOGL	*GetEvaluationShader()  { return mpEvaluationShader; }
    CPUTShaderOGL   *GetGeometryShader()    { return mpGeometryShader; }
    int GetShaderProgramID() { return mShaderProgram; };
    bool Tessellated() { return mpEvaluationShader != NULL; };

    virtual void          SetRenderStates();
    
    static CPUTMaterialOGL* Create() { return new CPUTMaterialOGL(); };

    // OGL does not support all of these but we keep them the same for now to match the DX version
    CPUTShaderParameters     mVertexShaderParameters;
    CPUTShaderParameters     mHullShaderParameters;
    CPUTShaderParameters     mDomainShaderParameters;
    CPUTShaderParameters     mGeometryShaderParameters;
    CPUTShaderParameters     mPixelShaderParameters;
    CPUTShaderParameters     mComputeShaderParameters;
    GLuint mSamplerIDs[CPUT_MATERIAL_MAX_SAMPLER_STATE];

};
#endif
