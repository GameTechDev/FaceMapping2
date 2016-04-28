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
#include "CPUTHullShaderDX11.h"
#include "CPUTAssetLibraryDX11.h"

CPUTHullShaderDX11 *CPUTHullShaderDX11::Create(
    const std::string    &name,
    const std::string    &shaderMain,
    const std::string    &shaderProfile,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    ID3DBlob          *pCompiledBlob = NULL;
    ID3D11HullShader  *pNewHullShader = NULL;

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    CPUTResult result = pAssetLibrary->CompileShaderFromFile(name, shaderMain, shaderProfile, &pCompiledBlob, pShaderMacros);
    ASSERT( CPUTSUCCESS(result), "Error compiling Hull shader:\n\n" );
    UNREFERENCED_PARAMETER(result);

    // Create the Hull shader
    ID3D11Device      *pD3dDevice = CPUT_DX11::GetDevice();
    HRESULT hr = pD3dDevice->CreateHullShader( pCompiledBlob->GetBufferPointer(), pCompiledBlob->GetBufferSize(), NULL, &pNewHullShader );
    ASSERT( SUCCEEDED(hr), "Error creating Hull shader:\n\n" );
    UNREFERENCED_PARAMETER(hr);
    // std::string DebugName = "CPUTAssetLibraryDX11::GetHullShader "+name;
    // CPUTSetDebugName(pNewHullShader, DebugName);

    CPUTHullShaderDX11 *pNewCPUTHullShader = new CPUTHullShaderDX11( pNewHullShader, pCompiledBlob );

    // add shader to library
    pAssetLibrary->AddHullShader(name, "", shaderMain + shaderProfile, pNewCPUTHullShader);
    // pNewCPUTHullShader->Release(); // We've added it to the library, so release our reference

    // return the shader (and blob)
    return pNewCPUTHullShader;
}

//--------------------------------------------------------------------------------------
CPUTHullShaderDX11 *CPUTHullShaderDX11::CreateFromMemory(
    const std::string     &name,
    const std::string     &shaderMain,
    const std::string     &shaderProfile,
    const char        *pShaderSource,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    ID3DBlob          *pCompiledBlob = NULL;
    ID3D11HullShader  *pNewHullShader = NULL;

    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibrary::GetAssetLibrary();
    CPUTResult result = pAssetLibrary->CompileShaderFromMemory(pShaderSource, shaderMain, shaderProfile, &pCompiledBlob, pShaderMacros);
    ASSERT( CPUTSUCCESS(result), "Error compiling Hull shader:\n\n" );
    UNREFERENCED_PARAMETER(result);

    // Create the Hull shader
    ID3D11Device      *pD3dDevice = CPUT_DX11::GetDevice();
    HRESULT hr = pD3dDevice->CreateHullShader( pCompiledBlob->GetBufferPointer(), pCompiledBlob->GetBufferSize(), NULL, &pNewHullShader );
    ASSERT( SUCCEEDED(hr), "Error creating Hull shader:\n\n" );
    UNREFERENCED_PARAMETER(hr);
    // std::string DebugName = "CPUTAssetLibraryDX11::GetHullShader "+name;
    // CPUTSetDebugName(pNewHullShader, DebugName);

    CPUTHullShaderDX11 *pNewCPUTHullShader = new CPUTHullShaderDX11( pNewHullShader, pCompiledBlob );

    // add shader to library
    pAssetLibrary->AddHullShader(name, "", shaderMain + shaderProfile, pNewCPUTHullShader);
    // pNewCPUTHullShader->Release(); // We've added it to the library, so release our reference

    // return the shader (and blob)
    return pNewCPUTHullShader;
}
