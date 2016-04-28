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
#include "CPUT.h"
#include "CPUTMaterial.h"
#include "CPUTBuffer.h"
#include "CPUTRenderStateBlock.h"
#ifdef CPUT_FOR_DX11
#include "CPUTMaterialDX11.h"
#else    
	#if (defined(CPUT_FOR_OGL) || defined(CPUT_FOR_OGLES))
	#include "CPUTMaterialOGL.h"
	#else
		#error You must supply a target graphics API (ex: #define CPUT_FOR_DX11), or implement the target API for this file.
	#endif
#endif

ConstantBufferDescription::ConstantBufferDescription() :
    pData(NULL),
    pBuffer(NULL),
    size(0),
    numUniforms(0),
    bindPoint(-1),
    bufferName(""),
    pUniformArrayLengths(NULL),
    pUniformIndices(NULL),
    pUniformNames(NULL),
    pUniformOffsets(NULL),
    pUniformSizes(NULL),
    pUniformTypes(NULL),
    modified(false)
{
}

ConstantBufferDescription::~ConstantBufferDescription()
{
    SAFE_RELEASE(pBuffer);
    SAFE_DELETE_ARRAY(pData);
    SAFE_DELETE_ARRAY(pUniformArrayLengths);
    SAFE_DELETE_ARRAY(pUniformIndices);
    SAFE_DELETE_ARRAY(pUniformNames);
    SAFE_DELETE_ARRAY(pUniformOffsets);
    SAFE_DELETE_ARRAY(pUniformSizes);
    SAFE_DELETE_ARRAY(pUniformSizes);
    SAFE_DELETE_ARRAY(pUniformTypes);
}

//--------------------------------------------------------------------------------------
CPUTMaterial *CPUTMaterial::Create(
    const std::string   &absolutePathAndFilename,
    CPUT_SHADER_MACRO* pShaderMacros
){
    // Create the material and load it from file.
#ifdef CPUT_FOR_DX11
	CPUTMaterial *pMaterial = CPUTMaterialDX11::Create();
#else    
	#if (defined(CPUT_FOR_OGL) || defined(CPUT_FOR_OGLES))
		CPUTMaterial *pMaterial = CPUTMaterialOGL::Create();
	#else
		#error You must supply a target graphics API (ex: #define CPUT_FOR_DX11), or implement the target API for this file.
	#endif
#endif

    CPUTResult result = pMaterial->LoadMaterial( absolutePathAndFilename, pShaderMacros );
    ASSERT( CPUTSUCCESS(result), "\nError - CPUTAssetLibrary::GetMaterial() - Error in material file: '"+absolutePathAndFilename+"'" );
    UNREFERENCED_PARAMETER(result);

    return pMaterial;
}
void CPUTMaterial::SetRenderStates()
{
    if (mpRenderStateBlock) { mpRenderStateBlock->SetRenderStates(); }
}
CPUTRenderStateBlock* CPUTMaterial::GetRenderStateBlock(){
    mpRenderStateBlock->AddRef();
    return mpRenderStateBlock;
};
