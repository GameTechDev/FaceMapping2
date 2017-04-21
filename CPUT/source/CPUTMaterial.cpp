/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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
