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

#include "CPUT_DX11.h"
#include "CPUTVertexShaderDX11.h"
#include "CPUTAssetLibraryDX11.h"

CPUTVertexShaderDX11 *CPUTVertexShaderDX11::Create(
    const std::string         &name,
    const std::string         &shaderMain,
    const std::string         &shaderProfile,
    CPUT_SHADER_MACRO     *pShaderMacros
)
{
    ID3DBlob            *pCompiledBlob = NULL;
    ID3D11VertexShader  *pNewVertexShader = NULL;

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    CPUTResult result = pAssetLibrary->CompileShaderFromFile(name, shaderMain, shaderProfile, &pCompiledBlob, pShaderMacros );
    ASSERT( CPUTSUCCESS(result), "Error compiling vertex shader:\n\n" );
    UNREFERENCED_PARAMETER(result);

    // Create the vertex shader
    ID3D11Device      *pD3dDevice = CPUT_DX11::GetDevice();
    HRESULT hr = pD3dDevice->CreateVertexShader( pCompiledBlob->GetBufferPointer(), pCompiledBlob->GetBufferSize(), NULL, &pNewVertexShader );
    ASSERT( SUCCEEDED(hr), "Error creating vertex shader:\n\n" );
    UNREFERENCED_PARAMETER(hr);
    // std::string DebugName = "CPUTAssetLibraryDX11::GetVertexShader "+name;
    // CPUTSetDebugName(pNewVertexShader, DebugName);

    CPUTVertexShaderDX11 *pNewCPUTVertexShader = new CPUTVertexShaderDX11( pNewVertexShader, pCompiledBlob );

    // add shader to library
    pAssetLibrary->AddVertexShader(name, "", shaderMain + shaderProfile, pNewCPUTVertexShader);//, pShaderMacros);
    // pNewCPUTVertexShader->Release(); // We've added it to the library, so release our reference

    // return the shader (and blob)
    return pNewCPUTVertexShader;
}

//--------------------------------------------------------------------------------------
CPUTVertexShaderDX11 *CPUTVertexShaderDX11::CreateFromMemory(
    const std::string     &name,
    const std::string     &shaderMain,
    const std::string     &shaderProfile,
    const char        *pShaderSource,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    ID3DBlob           *pCompiledBlob = NULL;
    ID3D11VertexShader *pNewVertexShader = NULL;

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    CPUTResult result = pAssetLibrary->CompileShaderFromMemory(pShaderSource, shaderMain, shaderProfile, &pCompiledBlob, pShaderMacros);
    ASSERT( CPUTSUCCESS(result), "Error compiling vertex shader:\n\n" );
    UNREFERENCED_PARAMETER(result);

    // Create the vertex shader
    ID3D11Device      *pD3dDevice = CPUT_DX11::GetDevice();
    HRESULT hr = pD3dDevice->CreateVertexShader( pCompiledBlob->GetBufferPointer(), pCompiledBlob->GetBufferSize(), NULL, &pNewVertexShader );
    ASSERT( SUCCEEDED(hr), "Error creating vertex shader:\n\n" );
    UNREFERENCED_PARAMETER(hr);
    // std::string DebugName = "CPUTAssetLibraryDX11::GetVertexShader "+name;
    // CPUTSetDebugName(pNewVertexShader, DebugName);

    CPUTVertexShaderDX11 *pNewCPUTVertexShader = new CPUTVertexShaderDX11( pNewVertexShader, pCompiledBlob );

    // add shader to library
    pAssetLibrary->AddVertexShader(name, "", shaderMain + shaderProfile, pNewCPUTVertexShader);//, pShaderMacros);
    // pNewCPUTVertexShader->Release(); // We've added it to the library, so release our reference

    // return the shader (and blob)
    return pNewCPUTVertexShader;
}
