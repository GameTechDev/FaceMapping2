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
#include "CPUTComputeShaderDX11.h"
#include "CPUTAssetLibraryDX11.h"

CPUTComputeShaderDX11 *CPUTComputeShaderDX11::Create(
    const std::string     &name,
    const std::string     &shaderMain,
    const std::string     &shaderProfile,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    ID3DBlob            *pCompiledBlob = NULL;
    ID3D11ComputeShader *pNewComputeShader = NULL;

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    CPUTResult result = pAssetLibrary->CompileShaderFromFile(name, shaderMain, shaderProfile, &pCompiledBlob, pShaderMacros);
    ASSERT( CPUTSUCCESS(result), "Error compiling compute shader:\n\n" );
    UNREFERENCED_PARAMETER(result);

    // Create the compute shader
    ID3D11Device      *pD3dDevice = CPUT_DX11::GetDevice();
    HRESULT hr = pD3dDevice->CreateComputeShader( pCompiledBlob->GetBufferPointer(), pCompiledBlob->GetBufferSize(), NULL, &pNewComputeShader );
    ASSERT( SUCCEEDED(hr), "Error creating compute shader:\n\n" );
    UNREFERENCED_PARAMETER(hr);
    // std::string DebugName = "CPUTAssetLibraryDX11::GetComputeShader "+name;
    // CPUTSetDebugName(pNewComputeShader, DebugName);

    CPUTComputeShaderDX11 *pNewCPUTComputeShader = new CPUTComputeShaderDX11( pNewComputeShader, pCompiledBlob );

    // add shader to library
    pAssetLibrary->AddComputeShader(name, "", shaderMain + shaderProfile, pNewCPUTComputeShader);

    // return the shader
    return pNewCPUTComputeShader;
}

//--------------------------------------------------------------------------------------
CPUTComputeShaderDX11 *CPUTComputeShaderDX11::CreateFromMemory(
    const std::string     &name,
    const std::string     &shaderMain,
    const std::string     &shaderProfile,
    const char        *pShaderSource,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    ID3DBlob*           pCompiledBlob = NULL;
    ID3D11ComputeShader*  pNewComputeShader = NULL;

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    CPUTResult result = pAssetLibrary->CompileShaderFromMemory(pShaderSource, shaderMain, shaderProfile, &pCompiledBlob, pShaderMacros );
    ASSERT( CPUTSUCCESS(result), "Error compiling Compute shader:\n\n" );
    UNREFERENCED_PARAMETER(result);

    // Create the Compute shader
    ID3D11Device      *pD3dDevice = CPUT_DX11::GetDevice();
    HRESULT hr = pD3dDevice->CreateComputeShader( pCompiledBlob->GetBufferPointer(), pCompiledBlob->GetBufferSize(), NULL, &pNewComputeShader );
    ASSERT( SUCCEEDED(hr), "Error creating Compute shader:\n\n" );
    UNREFERENCED_PARAMETER(hr);

    // std::string DebugName = "CPUTAssetLibraryDX11::GetComputeShader "+name;
    // CPUTSetDebugName(pNewComputeShader, DebugName);
    CPUTComputeShaderDX11 *pNewCPUTComputeShader = new CPUTComputeShaderDX11( pNewComputeShader, pCompiledBlob );

    // add shader to library
    pAssetLibrary->AddComputeShader(name, "", shaderMain + shaderProfile, pNewCPUTComputeShader);
    // pNewCPUTComputeShader->Release(); // We've added it to the library, so release our reference

    // return the shader (and blob)
    return pNewCPUTComputeShader;
}
