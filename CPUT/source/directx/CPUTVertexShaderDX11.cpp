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
