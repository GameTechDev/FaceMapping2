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
#ifndef __CPUTMATERIAL_H__
#define __CPUTMATERIAL_H__

#include "CPUTRefCount.h"
class CPUTConfigBlock;
class CPUTModel;
class CPUTBuffer;
const std::string EXTERNALS_SHADER_NAME = "cbExternals";
const std::string EXTERNALS_BLOCK_NAME = "externals";

#define CPUT_MATERIAL_MAX_TEXTURE_SLOTS         32
#define CPUT_MATERIAL_MAX_BUFFER_SLOTS          32
#define CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS 32
#define CPUT_MATERIAL_MAX_SRV_SLOTS             32
#define CPUT_MATERIAL_MAX_SAMPLER_STATE         16
#if 1 // Need to handle >=DX11 vs. < DX11, where max UAV slots == 1;
#   define CPUT_MATERIAL_MAX_UAV_SLOTS             7
#else
#   define CPUT_MATERIAL_MAX_UAV_SLOTS             1
#endif


class ConstantBufferDescription
{
public:
	char* pData;
	CPUTBuffer* pBuffer;
	int size;
	int numUniforms;
	int bindPoint;
	std::string bufferName;
    int* pUniformIndices;  
	std::string* pUniformNames;
    int* pUniformOffsets;
    int* pUniformArrayLengths;
    int* pUniformSizes;
    int* pUniformTypes;
    bool modified;
    ConstantBufferDescription();
    ~ConstantBufferDescription();
};

class CPUTMaterial:public CPUTRefCount
{
protected:
    std::string               mMaterialName;
    CPUTConfigBlock          *mpConfigBlock;
    CPUTRenderStateBlock *mpRenderStateBlock;
    ConstantBufferDescription        mConstantBuffer;

    // Destructor is not public.  Must release instead of delete.
    virtual ~CPUTMaterial(){};

    CPUTMaterial() :
	      mMaterialName("not initialized"),
        mpConfigBlock(NULL),
	    mpRenderStateBlock(NULL)
    {
    };
    
public:
	static CPUTMaterial *Create(const std::string &absolutePathAndFilename, CPUT_SHADER_MACRO* pShaderMacros=NULL);

 
    CPUTRenderStateBlock* GetRenderStateBlock();
    void                  SetMaterialName(const std::string materialName) { 
        mMaterialName = materialName; 
    }
    std::string              *GetMaterialName() { return &mMaterialName; }
	virtual void			OverridePSTexture(int index, CPUTTexture *texture) {}
	virtual CPUTTexture *GetPixelShaderTexture(int index) { return NULL; }
	virtual CPUTTexture		*GetTexture(int index) { return NULL; }
    virtual void          SetRenderStates();
    virtual CPUTResult    LoadMaterial(
        const std::string   &fileName,
                            CPUT_SHADER_MACRO* pShaderMacros=NULL
    ) = 0;
};

void SetMaterialStates(CPUTMaterial* pNewMaterial, CPUTMaterial* pCurrent);
#endif
