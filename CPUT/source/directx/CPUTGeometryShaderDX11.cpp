/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////


#include "CPUT_DX11.h"
#include "CPUTGeometryShaderDX11.h"
#include "CPUTAssetLibraryDX11.h"

CPUTGeometryShaderDX11 *CPUTGeometryShaderDX11::CreateFromNativeGeometryShader(
    ID3D11GeometryShader *pNativeShader
)
{
    return new CPUTGeometryShaderDX11( pNativeShader, NULL );
}

CPUTGeometryShaderDX11 *CPUTGeometryShaderDX11::Create(
    const std::string    &name,
    const std::string    &shaderMain,
    const std::string    &shaderProfile,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    ID3DBlob             *pCompiledBlob = NULL;
    ID3D11GeometryShader *pNewGeometryShader = NULL;

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    CPUTResult result = pAssetLibrary->CompileShaderFromFile(name, shaderMain, shaderProfile, &pCompiledBlob, pShaderMacros );
    ASSERT( CPUTSUCCESS(result), "Error compiling Geometry shader:\n\n" );
    UNREFERENCED_PARAMETER(result);

    // Create the Geometry shader
    ID3D11Device      *pD3dDevice = CPUT_DX11::GetDevice();
    HRESULT hr = pD3dDevice->CreateGeometryShader( pCompiledBlob->GetBufferPointer(), pCompiledBlob->GetBufferSize(), NULL, &pNewGeometryShader );
    ASSERT( SUCCEEDED(hr), "Error creating Geometry shader:\n\n" );
    UNREFERENCED_PARAMETER(hr);
    // std::string DebugName = "CPUTAssetLibraryDX11::GetGeometryShader "+name;
    // CPUTSetDebugName(pNewGeometryShader, DebugName);

    CPUTGeometryShaderDX11 *pNewCPUTGeometryShader = new CPUTGeometryShaderDX11( pNewGeometryShader, pCompiledBlob );

    // add shader to library
    pAssetLibrary->AddGeometryShader(name, "", shaderMain + shaderProfile, pNewCPUTGeometryShader);
    // pNewCPUTGeometryShader->Release(); // We've added it to the library, so release our reference

    // return the shader (and blob)
    return pNewCPUTGeometryShader;
}

//--------------------------------------------------------------------------------------
CPUTGeometryShaderDX11 *CPUTGeometryShaderDX11::CreateFromMemory(
    const std::string     &name,
    const std::string     &shaderMain,
    const std::string     &shaderProfile,
    const char        *pShaderSource,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    ID3DBlob             *pCompiledBlob = NULL;
    ID3D11GeometryShader *pNewGeometryShader = NULL;

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    CPUTResult result = pAssetLibrary->CompileShaderFromMemory(pShaderSource, shaderMain, shaderProfile, &pCompiledBlob, pShaderMacros );
    ASSERT( CPUTSUCCESS(result), "Error creating Geometry shader:\n\n" );
    UNREFERENCED_PARAMETER(result);

    // Create the Geometry shader
    ID3D11Device      *pD3dDevice = CPUT_DX11::GetDevice();
    HRESULT hr = pD3dDevice->CreateGeometryShader( pCompiledBlob->GetBufferPointer(), pCompiledBlob->GetBufferSize(), NULL, &pNewGeometryShader );
    ASSERT( SUCCEEDED(hr), "Error creating Geometry shader:\n\n" );
    UNREFERENCED_PARAMETER(hr);
    // std::string DebugName = "CPUTAssetLibraryDX11::GetGeometryShader "+name;
    // CPUTSetDebugName(pNewGeometryShader, DebugName);

    CPUTGeometryShaderDX11 *pNewCPUTGeometryShader = new CPUTGeometryShaderDX11( pNewGeometryShader, pCompiledBlob );

    // add shader to library
    pAssetLibrary->AddGeometryShader(name, "", shaderMain + shaderProfile, pNewCPUTGeometryShader);
    // pNewCPUTGeometryShader->Release(); // We've added it to the library, so release our reference

    // return the shader (and blob)
    return pNewCPUTGeometryShader;
}
