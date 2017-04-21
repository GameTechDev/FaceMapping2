/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CPUT.h"
#ifdef CPUT_FOR_DX11
#include "CPUTMaterialDX11.h"
#include "CPUT_DX11.h"
#include "CPUTRenderStateBlockDX11.h"
#include "D3DCompiler.h"
#include "CPUTTextureDX11.h"
#include "CPUTBufferDX11.h"
#include "CPUTVertexShaderDX11.h"
#include "CPUTPixelShaderDX11.h"
#include "CPUTComputeShaderDX11.h"
#include "CPUTGeometryShaderDX11.h"
#include "CPUTDomainShaderDX11.h"
#include "CPUTHullShaderDX11.h"

#define OUTPUT_BINDING_DEBUG_INFO(x)

//
//-----------------------------------------------------------------------------
CPUTShaderParameters::~CPUTShaderParameters()
{
    for (int ii = 0; ii < CPUT_MATERIAL_MAX_TEXTURE_SLOTS; ii++)
    {
        SAFE_RELEASE(mpTexture[ii]);
    }
    for (int ii = 0; ii < CPUT_MATERIAL_MAX_UAV_SLOTS; ii++)
    {
        SAFE_RELEASE(mpUAV[ii]);
    }
    for (int ii = 0; ii < CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS; ii++)
    {
        SAFE_RELEASE(mpConstantBuffer[ii]);
    }
    SAFE_DELETE_ARRAY(mpTextureName);
    SAFE_DELETE_ARRAY(mpTextureBindPoint);
    SAFE_DELETE_ARRAY(mpUAVName);
    SAFE_DELETE_ARRAY(mpUAVBindPoint);
    SAFE_DELETE_ARRAY(mpConstantBufferName);
    SAFE_DELETE_ARRAY(mpConstantBufferBindPoint)
}

//-----------------------------------------------------------------------------
CPUTMaterialDX11::CPUTMaterialDX11() :
mpPixelShader(NULL),
mpComputeShader(NULL),
mpVertexShader(NULL),
mpGeometryShader(NULL),
mpHullShader(NULL),
mpDomainShader(NULL)
{
}

//-----------------------------------------------------------------------------
CPUTMaterialDX11::~CPUTMaterialDX11()
{
    SAFE_RELEASE(mpPixelShader);
    SAFE_RELEASE(mpComputeShader);
    SAFE_RELEASE(mpVertexShader);
    SAFE_RELEASE(mpGeometryShader);
    SAFE_RELEASE(mpHullShader);
    SAFE_RELEASE(mpDomainShader);
    SAFE_RELEASE(mpRenderStateBlock);
    CPUTMaterial::~CPUTMaterial();
}

//
void GetShaderResourceViews(CPUTShaderParameters& params, int& start, int& count, ID3D11ShaderResourceView* pSRVs[CPUT_MATERIAL_MAX_TEXTURE_SLOTS])
{
    if (params.mBindViewMax < params.mBindViewMin)
    {
        start = 0; 
        count = 0;
        return;
    }
    start = params.mBindViewMin;
    count = params.mBindViewMax - start + 1;
    ZeroMemory(pSRVs+start, sizeof(ID3D11ShaderResourceView*)*count);
    for (int texture = 0; texture < params.mTextureCount; texture++)
    {
        int bindPoint = params.mpTextureBindPoint[texture];
        CPUTTextureDX11* pTexture = ((CPUTTextureDX11*)params.mpTexture[texture]);
        if (pTexture != NULL)
        {
            pSRVs[bindPoint] = pTexture->GetShaderResourceView();
        }
    }
}

void GetConstantBufferViews(CPUTShaderParameters& params, int& start, int& count, ID3D11Buffer* pBuffers[CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS])
{
    if (params.mBindConstantBufferMax < params.mBindConstantBufferMin)
    {
        start = 0;
        count = 0;
        return;
    }
    start = params.mBindConstantBufferMin;
    count = params.mBindConstantBufferMax - start + 1;
    ZeroMemory(pBuffers + start, sizeof(ID3D11Buffer*)*count);
    for (int buffer = 0; buffer < params.mConstantBufferCount; buffer++)
    {

        int bindPoint = params.mpConstantBufferBindPoint[buffer];
        CPUTBufferDX11* pBuffer = (CPUTBufferDX11*)params.mpConstantBuffer[buffer];
        if (pBuffer != NULL)
        {            
            pBuffers[bindPoint] = pBuffer->GetNativeBuffer();
        }
    }
}

//-----------------------------------------------------------------------------
void CPUTMaterialDX11::SetRenderStates()
{
    SetMaterialStates(this, NULL);
    SetRenderStateBlock((CPUTRenderStateBlockDX11*)mpRenderStateBlock, NULL);
}

//-----------------------------------------------------------------------------
void CPUTMaterialDX11::ReadShaderSamplersAndTextures(ID3DBlob *pBlob, CPUTShaderParameters *pShaderParameter)
{
    // ***************************
    // Use shader reflection to get texture and sampler names.  We use them later to bind .mtl texture-specification to shader parameters/variables.
    // TODO: Currently do this only for PS.  Do for other shader types too.
    // TODO: Generalize, so easy to call for different shader types
    // ***************************
    ID3D11ShaderReflection *pReflector = NULL;
    D3D11_SHADER_INPUT_BIND_DESC desc;

    D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
    // Walk through the shader input bind descriptors.  Find the samplers and textures.
    int ii = 0;
    HRESULT hr = pReflector->GetResourceBindingDesc(ii++, &desc);
    while (SUCCEEDED(hr))
    {
        switch (desc.Type)
        {
        case D3D_SIT_TEXTURE:
            pShaderParameter->mTextureCount++;
            break;
        case D3D_SIT_SAMPLER:
            //TODO: need to count these?
            //pShaderParameter->mSamplerCount++;
            break;
        case D3D_SIT_CBUFFER:
            pShaderParameter->mConstantBufferCount++;
            break;

        case D3D_SIT_TBUFFER:
        case D3D_SIT_STRUCTURED:
        case D3D_SIT_BYTEADDRESS:
            assert(0);
            //pShaderParameter->mBufferCount++;
            break;

        case D3D_SIT_UAV_RWTYPED:
        case D3D_SIT_UAV_RWSTRUCTURED:
        case D3D_SIT_UAV_RWBYTEADDRESS:
        case D3D_SIT_UAV_APPEND_STRUCTURED:
        case D3D_SIT_UAV_CONSUME_STRUCTURED:
        case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
            pShaderParameter->mUAVCount++;
            break;
        }
        hr = pReflector->GetResourceBindingDesc(ii++, &desc);
    }

    pShaderParameter->mpTextureName = new std::string[pShaderParameter->mTextureCount];
    pShaderParameter->mpTextureBindPoint = new int[pShaderParameter->mTextureCount];
    pShaderParameter->mpUAVName = new std::string[pShaderParameter->mUAVCount];
    pShaderParameter->mpUAVBindPoint = new int[pShaderParameter->mUAVCount];
    pShaderParameter->mpConstantBufferName = new std::string[pShaderParameter->mConstantBufferCount];
    pShaderParameter->mpConstantBufferBindPoint = new int[pShaderParameter->mConstantBufferCount];

    // Start over.  This time, copy the names.
    ii = 0;
    UINT textureIndex = 0;
    UINT samplerIndex = 0;
    UINT bufferIndex = 0;
    UINT uavIndex = 0;
    UINT constantBufferIndex = 0;
    hr = pReflector->GetResourceBindingDesc(ii++, &desc);

    while (SUCCEEDED(hr))
    {
        std::string name = desc.Name;
        bool exclude = ((name.size() > 7) && name.substr(0, 7) == "nocput_");

        switch (desc.Type)
        {
        case D3D_SIT_TEXTURE:
            if (exclude)
            {
                pShaderParameter->mTextureCount--;
                pShaderParameter->mpTextureName[pShaderParameter->mTextureCount] = "excluded: " + name;
                pShaderParameter->mpTextureBindPoint[pShaderParameter->mTextureCount] = -1;
            }
            else
            {
                pShaderParameter->mpTextureName[textureIndex] = name;
                pShaderParameter->mpTextureBindPoint[textureIndex] = desc.BindPoint;
                textureIndex++;
            }
            break;
        case D3D_SIT_SAMPLER:
            if (exclude)
            {
                //ASSERT( pShaderParameter->mSamplerCount > 0, "Algorithm error" );
                //pShaderParameter->mSamplerCount--;
            }
            else
            {
                //pShaderParameter->mpSamplerParameterName[samplerIndex] = name;
                //pShaderParameter->mpSamplerBindPoint[samplerIndex] = desc.BindPoint;
                //samplerIndex++;
            }
            break;
        case D3D_SIT_CBUFFER:
            if (exclude)
            {
                ASSERT(pShaderParameter->mConstantBufferCount > 0, "Algorithm error");
                pShaderParameter->mConstantBufferCount--;
            }
            else
            {
                pShaderParameter->mpConstantBufferName[constantBufferIndex] = name;
                pShaderParameter->mpConstantBufferBindPoint[constantBufferIndex] = desc.BindPoint;
                if (!name.compare(EXTERNALS_SHADER_NAME))
                {
                    //get cb information;
                    ID3D11ShaderReflection* pShaderReflection = NULL;
                    D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pShaderReflection);
                    D3D11_SHADER_BUFFER_DESC desc;
                    ID3D11ShaderReflectionConstantBuffer* pSR = pShaderReflection->GetConstantBufferByIndex(constantBufferIndex);
                    pSR->GetDesc(&desc);


                    mConstantBuffer.pUniformArrayLengths = new int[desc.Variables];
                    mConstantBuffer.pUniformIndices = new int[desc.Variables];
                    mConstantBuffer.pUniformNames = new std::string[desc.Variables];
                    mConstantBuffer.pUniformOffsets = new int[desc.Variables];
                    mConstantBuffer.pUniformSizes = new int[desc.Variables];
                    mConstantBuffer.pUniformTypes = new int[desc.Variables];
                    mConstantBuffer.size = desc.Size;
                    mConstantBuffer.numUniforms = desc.Variables;
                    mConstantBuffer.bindPoint = constantBufferIndex;
                    mConstantBuffer.bufferName = "cbExternals";
                    mConstantBuffer.pData = new char[desc.Size];

                    for (UINT i = 0; i < desc.Variables; i++)
                    {
                        ID3D11ShaderReflectionVariable* pVariable = pSR->GetVariableByIndex(i);
                        D3D11_SHADER_VARIABLE_DESC svDesc;
                        pVariable->GetDesc(&svDesc);
                        mConstantBuffer.pUniformIndices[i] = i;
                        mConstantBuffer.pUniformNames[i] = std::string(svDesc.Name);
                        mConstantBuffer.pUniformOffsets[i] = svDesc.StartOffset;
                        mConstantBuffer.pUniformSizes[i] = svDesc.Size;
                        mConstantBuffer.pUniformArrayLengths[i] = 1; //FIXME: get array length
                    }

                    CPUTBufferDesc bufferdesc;
                    bufferdesc.cpuAccess = BUFFER_CPU_WRITE;
                    bufferdesc.memory = BUFFER_DYNAMIC;
                    bufferdesc.target = BUFFER_UNIFORM;
                    bufferdesc.pData = mConstantBuffer.pData;
                    bufferdesc.sizeBytes = mConstantBuffer.size;;
                    std::string buffername = "Material Externals";

                    mConstantBuffer.pBuffer = CPUTBuffer::Create(buffername, &bufferdesc);
                    pShaderParameter->mpConstantBuffer[constantBufferIndex] = mConstantBuffer.pBuffer;
                    mConstantBuffer.pBuffer->AddRef();

                }
                constantBufferIndex++;

            }
            break;
        case D3D_SIT_TBUFFER:
        case D3D_SIT_STRUCTURED:
        case D3D_SIT_BYTEADDRESS:
            if (exclude)
            {
                //ASSERT( pShaderParameter->mBufferCount > 0, "Algorithm error" );
                //pShaderParameter->mBufferCount--;
            }
            else
            {
                //pShaderParameter->mpBufferParameterName[bufferIndex] = name;
                //pShaderParameter->mpBufferBindPoint[bufferIndex] = desc.BindPoint;
                //bufferIndex++;
            }
            break;
        case D3D_SIT_UAV_RWTYPED:
        case D3D_SIT_UAV_RWSTRUCTURED:
        case D3D_SIT_UAV_RWBYTEADDRESS:
        case D3D_SIT_UAV_APPEND_STRUCTURED:
        case D3D_SIT_UAV_CONSUME_STRUCTURED:
        case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
            if (exclude)
            {
                ASSERT(pShaderParameter->mUAVCount > 0, "Algorithm error");
                pShaderParameter->mUAVCount--;
            }
            else
            {
                pShaderParameter->mpUAVName[uavIndex] = name;
                pShaderParameter->mpUAVBindPoint[uavIndex] = desc.BindPoint;
                uavIndex++;
            }
            break;
        }
        hr = pReflector->GetResourceBindingDesc(ii++, &desc);
    }
}



// TODO: these "Bind*" functions are almost identical, except they use different params.  Can we combine?
//-----------------------------------------------------------------------------
void CPUTMaterialDX11::BindTextures(CPUTShaderParameters &params, const CPUTModel *pModel, int meshIndex)
{
    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();

    for (int texture = 0; texture < params.mTextureCount; texture++)
    {
        std::string textureName;
        std::string tagName = params.mpTextureName[texture];

        CPUTConfigEntry *pValue = mpConfigBlock->GetValueByName(tagName);
        if (!pValue->IsValid())
        {
            //DEBUGMESSAGEBOX("Texture Load Error", "In material: " + mMaterialName + "\nCould not find texture parameter: " + tagName);
            std::string output = "CPUT WARNING: " + tagName + " not found in material " + mMaterialName + "\n";
            DEBUG_PRINT(output.c_str());
            continue;
        }
        textureName = pValue->ValueAsString();
        // If the texture name not specified.  Load default.dds instead
        if (0 == textureName.length()) { textureName = "default.dds"; }

        int bindPoint = params.mpTextureBindPoint[texture];
        ASSERT(bindPoint < CPUT_MATERIAL_MAX_TEXTURE_SLOTS, "Texture bind point out of range.");

        params.mBindViewMin = std::min(params.mBindViewMin, bindPoint);
        params.mBindViewMax = std::max(params.mBindViewMax, bindPoint);

        // Get the sRGB flag (default to true)
        std::string SRGBName = tagName + "sRGB";
        CPUTConfigEntry *pSRGBValue = mpConfigBlock->GetValueByName(SRGBName);
        bool loadAsSRGB = pSRGBValue->IsValid() ? loadAsSRGB = pSRGBValue->ValueAsBool() : false;

        if (!params.mpTexture[texture])
        {
            params.mpTexture[texture] = pAssetLibrary->GetTexture(textureName, false, loadAsSRGB);
            ASSERT(params.mpTexture[texture], "Failed getting texture " + textureName);
        }

        OUTPUT_BINDING_DEBUG_INFO((itoc(bindPoint) + " : " + params.mpTexture[textureCount]->GetName() + "\n").c_str());
    }
}

//-----------------------------------------------------------------------------
void CPUTMaterialDX11::BindUAVs(CPUTShaderParameters &params, const CPUTModel *pModel, int meshIndex)
{
    CPUTConfigEntry *pValue;
    if (!params.mUAVCount) { return; }
    OUTPUT_BINDING_DEBUG_INFO("Bound UAVs");

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    for (int UAV = 0; UAV < params.mUAVCount; UAV++)
    {
        std::string uavName;
        UINT uavCount = params.mUAVCount;

        std::string tagName = params.mpUAVName[UAV];
        {
            pValue = mpConfigBlock->GetValueByName(tagName);
            ASSERT(pValue->IsValid(), "Can't find UAV '" + tagName + "'."); //  TODO: fix message
            uavName = pValue->ValueAsString();
        }
        int bindPoint = params.mpUAVBindPoint[uavCount];
        ASSERT(bindPoint < CPUT_MATERIAL_MAX_UAV_SLOTS, "UAV bind point out of range.");

        params.mBindUAVMin = std::min(params.mBindUAVMin, bindPoint);
        params.mBindUAVMax = std::max(params.mBindUAVMax, bindPoint);

        if (!params.mpUAV[uavCount])
        {
            params.mpUAV[uavCount] = pAssetLibrary->GetBuffer(uavName);
            ASSERT(params.mpUAV[uavCount], "Failed getting UAV " + uavName);
        }

        OUTPUT_BINDING_DEBUG_INFO((itoc(bindPoint) + " : " + params.mpUAV[uavCount]->GetName() + "\n").c_str());
    }
    OUTPUT_BINDING_DEBUG_INFO("\n");
}

//-----------------------------------------------------------------------------
void CPUTMaterialDX11::BindConstantBuffers(CPUTShaderParameters &params, const CPUTModel *pModel, int meshIndex)
{
    CPUTConfigEntry *pValue;
    if (!params.mConstantBufferCount) { return; }
    OUTPUT_BINDING_DEBUG_INFO("Bound Constant Buffers\n");

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    for (int buffer = 0; buffer < params.mConstantBufferCount; buffer++)
    {
        std::string constantBufferName;
        UINT constantBufferCount = params.mConstantBufferCount;

        std::string tagName = params.mpConstantBufferName[buffer];
        if (!tagName.compare(EXTERNALS_SHADER_NAME))
        {
            constantBufferName = tagName;
        }
        else
        {
            pValue = mpConfigBlock->GetValueByName(tagName);
            if (!pValue->IsValid())
            {
                DEBUGMESSAGEBOX("Material Error", "Material " + mMaterialName + "\nDoes not specify paramter : " + tagName + ".");
                ASSERT(pValue->IsValid(), "Material Error -- Material " + mMaterialName + " does not specify paramter : " + tagName + ".");
            }
            constantBufferName = pValue->ValueAsString();
        }
        int bindPoint = params.mpConstantBufferBindPoint[buffer];
        ASSERT(bindPoint < CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS, "Constant buffer bind point out of range.");

        params.mBindConstantBufferMin = std::min(params.mBindConstantBufferMin, bindPoint);
        params.mBindConstantBufferMax = std::max(params.mBindConstantBufferMax, bindPoint);

        if (!params.mpConstantBuffer[buffer])
        {
            params.mpConstantBuffer[buffer] = pAssetLibrary->GetConstantBuffer(constantBufferName);// , pWhichModel, whichMesh );
            if (params.mpConstantBuffer[buffer] == NULL)
            {
                DEBUGMESSAGEBOX("Material Error", "Material " + mMaterialName + "\ncould not find constant buffer : " + constantBufferName + "\nFor paramter : " + tagName + ".");
                ASSERT(params.mpConstantBuffer[buffer], "Material Error -- Material " + mMaterialName + " could not find constant buffer : " + constantBufferName + " for paramter: " + tagName + ".");
            }
        }

        OUTPUT_BINDING_DEBUG_INFO((itoc(bindPoint) + " : " + params.mpConstantBuffer[constantBufferCount]->GetName() + "\n").c_str());
    }
    OUTPUT_BINDING_DEBUG_INFO("\n");
}



//-----------------------------------------------------------------------------
CPUTResult CPUTMaterialDX11::LoadMaterial(
    const std::string   &fileName,
    CPUT_SHADER_MACRO* pShaderMacros
    ){
    CPUTResult result = CPUT_SUCCESS;

    mMaterialName = fileName;
    // Open/parse the file
    CPUTConfigFile file;
    result = file.LoadFile(fileName);
    if (CPUTFAILED(result))
    {
        return result;
    }

    // Make a local copy of all the parameters
    CPUTConfigBlock *pBlock = file.GetBlock(0);
    ASSERT(pBlock, "Error getting parameter block");
    if (!pBlock)
    {
        return CPUT_ERROR_PARAMETER_BLOCK_NOT_FOUND;
    }
    mpConfigBlock = pBlock;

    // get necessary device and AssetLibrary pointers
    ID3D11Device *pD3dDevice = CPUT_DX11::GetDevice();
    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();

    CPUTConfigEntry *pValue;

    CPUTConfigEntry *pEntryPointName, *pProfileName;
    pValue = mpConfigBlock->GetValueByName("VertexShaderFile");
    if (pValue->IsValid())
    {
        pEntryPointName = mpConfigBlock->GetValueByName("VertexShaderMain");
        pProfileName = mpConfigBlock->GetValueByName("VertexShaderProfile");
        pAssetLibrary->GetVertexShader(
            pValue->ValueAsString(),
            pEntryPointName->ValueAsString(),
            pProfileName->ValueAsString(),
            &mpVertexShader,
            false);
        ReadShaderSamplersAndTextures(mpVertexShader->GetBlob(), &mVertexShaderParameters);
    }

    // load and store the pixel shader if it was specified
    pValue = mpConfigBlock->GetValueByName("PixelShaderFile");
    if (pValue->IsValid())
    {
        pEntryPointName = mpConfigBlock->GetValueByName("PixelShaderMain");
        pProfileName = mpConfigBlock->GetValueByName("PixelShaderProfile");
        pAssetLibrary->GetPixelShader(
            pValue->ValueAsString(),
            pEntryPointName->ValueAsString(),
            pProfileName->ValueAsString(),
            &mpPixelShader,
            false);
        ReadShaderSamplersAndTextures(mpPixelShader->GetBlob(), &mPixelShaderParameters);
    }

    // load and store the compute shader if it was specified
    pValue = mpConfigBlock->GetValueByName("ComputeShaderFile");
    if (pValue->IsValid())
    {
        pEntryPointName = mpConfigBlock->GetValueByName("ComputeShaderMain");
        pProfileName = mpConfigBlock->GetValueByName("ComputeShaderProfile");
        pAssetLibrary->GetComputeShader(
            pValue->ValueAsString(),
            pEntryPointName->ValueAsString(),
            pProfileName->ValueAsString(),
            &mpComputeShader,
            false);
        ReadShaderSamplersAndTextures(mpComputeShader->GetBlob(), &mComputeShaderParameters);
    }

    // load and store the geometry shader if it was specified
    pValue = mpConfigBlock->GetValueByName("GeometryShaderFile");
    if (pValue->IsValid())
    {
        pEntryPointName = mpConfigBlock->GetValueByName("GeometryShaderMain");
        pProfileName = mpConfigBlock->GetValueByName("GeometryShaderProfile");
        pAssetLibrary->GetGeometryShader(
            pValue->ValueAsString(),
            pEntryPointName->ValueAsString(),
            pProfileName->ValueAsString(),
            &mpGeometryShader,
            false);
        ReadShaderSamplersAndTextures(mpGeometryShader->GetBlob(), &mGeometryShaderParameters);
    }

    // load and store the hull shader if it was specified
    pValue = mpConfigBlock->GetValueByName("HullShaderFile");
    if (pValue->IsValid())
    {
        pEntryPointName = mpConfigBlock->GetValueByName("HullShaderMain");
        pProfileName = mpConfigBlock->GetValueByName("HullShaderProfile");
        pAssetLibrary->GetHullShader(
            pValue->ValueAsString(),
            pEntryPointName->ValueAsString(),
            pProfileName->ValueAsString(),
            &mpHullShader,
            false);
        ReadShaderSamplersAndTextures(mpHullShader->GetBlob(), &mHullShaderParameters);
    }

    // load and store the domain shader if it was specified
    pValue = mpConfigBlock->GetValueByName("DomainShaderFile");
    if (pValue->IsValid())
    {
        pEntryPointName = mpConfigBlock->GetValueByName("DomainShaderMain");
        pProfileName = mpConfigBlock->GetValueByName("DomainShaderProfile");
        pAssetLibrary->GetDomainShader(
            pValue->ValueAsString(),
            pEntryPointName->ValueAsString(),
            pProfileName->ValueAsString(),
            &mpDomainShader,
            false);
        ReadShaderSamplersAndTextures(mpDomainShader->GetBlob(), &mDomainShaderParameters);
    }

    // load and store the render state file if it was specified
    pValue = mpConfigBlock->GetValueByName("RenderStateFile");
    if (pValue->IsValid())
    {
        mpRenderStateBlock = pAssetLibrary->GetRenderStateBlock(pValue->ValueAsString());
    }

    OUTPUT_BINDING_DEBUG_INFO(("Bindings for : " + mMaterialName + "\n").c_str());

    // For each of the shader stages, bind shaders and buffers
    if (mpVertexShader)
    {
        OUTPUT_BINDING_DEBUG_INFO("Vertex Shader Parameters\n");
        BindTextures(mVertexShaderParameters);
        BindUAVs(mVertexShaderParameters);
        BindConstantBuffers(mVertexShaderParameters);
        OUTPUT_BINDING_DEBUG_INFO("\n");
    }

    if (mpHullShader)
    {
        OUTPUT_BINDING_DEBUG_INFO("Vertex Shader Parameters\n");
        BindTextures(mHullShaderParameters);
        BindUAVs(mHullShaderParameters);
        BindConstantBuffers(mHullShaderParameters);
        OUTPUT_BINDING_DEBUG_INFO("\n");
    }

    if (mpDomainShader)
    {
        OUTPUT_BINDING_DEBUG_INFO("Vertex Shader Parameters\n");
        BindTextures(mDomainShaderParameters);
        BindUAVs(mDomainShaderParameters);
        BindConstantBuffers(mDomainShaderParameters);
        OUTPUT_BINDING_DEBUG_INFO("\n");
    }

    if (mpGeometryShader)
    {
        OUTPUT_BINDING_DEBUG_INFO("Vertex Shader Parameters\n");
        BindTextures(mGeometryShaderParameters);
        BindUAVs(mGeometryShaderParameters);
        BindConstantBuffers(mGeometryShaderParameters);
        OUTPUT_BINDING_DEBUG_INFO("\n");
    }

    if (mpPixelShader)
    {
        OUTPUT_BINDING_DEBUG_INFO("Vertex Shader Parameters\n");
        BindTextures(mPixelShaderParameters);
        BindUAVs(mPixelShaderParameters);
        BindConstantBuffers(mPixelShaderParameters);
        OUTPUT_BINDING_DEBUG_INFO("\n");
    }

    if (mpComputeShader)
    {
        OUTPUT_BINDING_DEBUG_INFO("Vertex Shader Parameters\n");
        BindTextures(mComputeShaderParameters);
        BindUAVs(mComputeShaderParameters);
        BindConstantBuffers(mComputeShaderParameters);
        OUTPUT_BINDING_DEBUG_INFO("\n");
    }

    CPUTConfigBlock* pExternalBlock = file.GetBlockByName(EXTERNALS_BLOCK_NAME);
    if (pExternalBlock != NULL && mConstantBuffer.numUniforms > 0)
    {
        for (int i = 0; i < mConstantBuffer.numUniforms; i++)
        {
            float f[16];
            pValue = pExternalBlock->GetValueByName(mConstantBuffer.pUniformNames[i]);
            if (pValue->IsValid())
            {
                pValue->ValueAsFloatArray(f, sizeof(f) / sizeof(float));
                memcpy(mConstantBuffer.pData + mConstantBuffer.pUniformOffsets[i], f, mConstantBuffer.pUniformSizes[i]);
            }
            mConstantBuffer.pBuffer->SetData(0, mConstantBuffer.size, mConstantBuffer.pData);
        }
    }

    return result;
}

void SetMaterialStates(CPUTMaterial* pNewBase, CPUTMaterial* pCurrentBase)
{
    if (pNewBase == pCurrentBase)
        return;
    
    CPUTMaterialDX11* pNew = (CPUTMaterialDX11*)pNewBase;
    CPUTMaterialDX11* pCurrent = (CPUTMaterialDX11*)pCurrentBase;
    ID3D11DeviceContext *pContext = CPUT_DX11::GetContext();
    ID3D11ShaderResourceView* pSRVs[CPUT_MATERIAL_MAX_TEXTURE_SLOTS];
    ID3D11Buffer* pBuffers[CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS];

    CPUTVertexShaderDX11* pVertexShader = pNew->GetVertexShader();
    if (pVertexShader)
    {
        int start, count;
        GetConstantBufferViews(pNew->mVertexShaderParameters, start, count, pBuffers);
        pContext->VSSetConstantBuffers(start, count, pBuffers + start);
        GetShaderResourceViews(pNew->mVertexShaderParameters, start, count, pSRVs);
        pContext->VSSetShaderResources(start, count, pSRVs + start);
    }
    if (pCurrent == NULL || pVertexShader != pCurrent->GetVertexShader())
    {
        ID3D11VertexShader* pShader = NULL;
        if (pVertexShader)
        {
            pShader = pVertexShader->GetNativeVertexShader();
        }
        pContext->VSSetShader(pShader, NULL, NULL);
    }

    CPUTHullShaderDX11* pHullShader = pNew->GetHullShader();
    if (pCurrent == NULL || pHullShader != pCurrent->GetHullShader())
    {
        int start, count;
        GetConstantBufferViews(pNew->mHullShaderParameters, start, count, pBuffers);
        pContext->HSSetConstantBuffers(start, count, pBuffers + start);
        GetShaderResourceViews(pNew->mHullShaderParameters, start, count, pSRVs);
        pContext->HSSetShaderResources(start, count, pSRVs + start);
    }
    if (pCurrent == NULL || pHullShader != pCurrent->GetHullShader())
    {
        ID3D11HullShader* pShader = NULL;
        if (pHullShader)
        {
            pShader = pHullShader->GetNativeHullShader();
        }
        pContext->HSSetShader(pShader, NULL, NULL);
    }

    CPUTDomainShaderDX11* pDomainShader = pNew->GetDomainShader();
    if (pDomainShader)
    {
        int start, count;
        GetConstantBufferViews(pNew->mDomainShaderParameters, start, count, pBuffers);
        pContext->DSSetConstantBuffers(start, count, pBuffers + start);
        GetShaderResourceViews(pNew->mDomainShaderParameters, start, count, pSRVs);
        pContext->DSSetShaderResources(start, count, pSRVs + start);
    }
    if (pCurrent == NULL || pDomainShader != pCurrent->GetDomainShader())
    {
        ID3D11DomainShader* pShader = NULL;
        if (pDomainShader)
        {
            pShader = pDomainShader->GetNativeDomainShader();
        }
        pContext->DSSetShader(pShader, NULL, NULL);
    }

    CPUTGeometryShaderDX11* pGeometryShader = pNew->GetGeometryShader();
    if (pGeometryShader)
    {
        int start, count;
        GetConstantBufferViews(pNew->mGeometryShaderParameters, start, count, pBuffers);
        pContext->GSSetConstantBuffers(start, count, pBuffers + start);
        GetShaderResourceViews(pNew->mGeometryShaderParameters, start, count, pSRVs);
        pContext->GSSetShaderResources(start, count, pSRVs + start);
    }
    if (pCurrent == NULL || pGeometryShader != pCurrent->GetGeometryShader())
    {
        ID3D11GeometryShader* pShader = NULL;
        if (pGeometryShader)
        {
            pShader = pGeometryShader->GetNativeGeometryShader();
        }
        pContext->GSSetShader(pShader, NULL, NULL);

    }

    CPUTPixelShaderDX11* pPixelShader = pNew->GetPixelShader();
    if (pPixelShader)
    {
        int start, count;
        GetConstantBufferViews(pNew->mPixelShaderParameters, start, count, pBuffers);
        pContext->PSSetConstantBuffers(start, count, pBuffers + start);
        GetShaderResourceViews(pNew->mPixelShaderParameters, start, count, pSRVs);
        pContext->PSSetShaderResources(start, count, pSRVs + start);
    }
    if (pCurrent == NULL || pPixelShader != pCurrent->GetPixelShader())
    {
        ID3D11PixelShader* pShader = NULL;
        if (pPixelShader)
        {
            pShader = pPixelShader->GetNativePixelShader();
        }
        pContext->PSSetShader(pShader, NULL, NULL);
    }

    CPUTComputeShaderDX11* pComputeShader = pNew->GetComputeShader();
    CPUTComputeShaderDX11* pCurrentCS = pCurrent != NULL ? pCurrent->GetComputeShader() : NULL;
    if (pComputeShader != pCurrentCS)
    {

        if (pComputeShader)
        {
            pContext->CSSetShader(pComputeShader->GetNativeComputeShader(), NULL, NULL);
            int start, count;
            GetConstantBufferViews(pNew->mComputeShaderParameters, start, count, pBuffers);
            pContext->CSSetConstantBuffers(start, count, pBuffers + start);
            GetShaderResourceViews(pNew->mComputeShaderParameters, start, count, pSRVs);
            pContext->CSSetShaderResources(start, count, pSRVs + start);
        }
        else
        {
            pContext->CSSetShader(NULL, NULL, NULL);
        }
    }
}

CPUTMaterialDX11* CPUTMaterialDX11::Create()
{
    return new CPUTMaterialDX11();
}

void CPUTMaterialDX11::OverrideGeometryShader(CPUTGeometryShaderDX11 *pGSShader) 
{ 
    SAFE_RELEASE(mpGeometryShader);
    pGSShader->AddRef();

    mpGeometryShader = pGSShader; 
}

void CPUTMaterialDX11::OverridePSTexture(int index, CPUTTexture *texture)
{
	SAFE_RELEASE(mPixelShaderParameters.mpTexture[index]);
	texture->AddRef();
	mPixelShaderParameters.mpTexture[index] = texture;
}

CPUTTexture *CPUTMaterialDX11::GetPixelShaderTexture(int index)
{
	return mPixelShaderParameters.mpTexture[index];
}

#endif
