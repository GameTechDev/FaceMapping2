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

#include "CPUTAssetLibraryDX11.h"

// define the objects we'll need
//#include "CPUTModel.h"
#include "CPUTMaterialDX11.h"
#include "CPUTTextureDX11.h"
#include "CPUTRenderStateBlockDX11.h"
#include "CPUTLight.h"
#include "CPUTCamera.h"
#include "CPUTVertexShaderDX11.h"
#include "CPUTPixelShaderDX11.h"
#include "CPUTGeometryShaderDX11.h"
#include "CPUTComputeShaderDX11.h"
#include "CPUTHullShaderDX11.h"
#include "CPUTDomainShaderDX11.h"

#define LIBRARY_ASSERT(a, b) ASSERT(a, b)

std::vector<CPUTAsset<CPUTPixelShaderDX11>> CPUTAssetLibraryDX11::mpPixelShaderList;
std::vector<CPUTAsset<CPUTComputeShaderDX11>> CPUTAssetLibraryDX11::mpComputeShaderList;
std::vector<CPUTAsset<CPUTVertexShaderDX11>> CPUTAssetLibraryDX11::mpVertexShaderList;
std::vector<CPUTAsset<CPUTGeometryShaderDX11>> CPUTAssetLibraryDX11::mpGeometryShaderList;
std::vector<CPUTAsset<CPUTHullShaderDX11>> CPUTAssetLibraryDX11::mpHullShaderList;
std::vector<CPUTAsset<CPUTDomainShaderDX11>> CPUTAssetLibraryDX11::mpDomainShaderList;

CPUTAssetLibrary* CPUTAssetLibrary::GetAssetLibrary()
{
    if(NULL==mpAssetLibrary)
    {
        mpAssetLibrary = new CPUTAssetLibraryDX11();
    }
    return mpAssetLibrary;
}

// Deletes and properly releases all asset library lists that contain
// unwrapped IUnknown DirectX objects.
//-----------------------------------------------------------------------------
void CPUTAssetLibraryDX11::ReleaseAllLibraryLists()
{
    // TODO: we really need to wrap the DX assets so we don't need to distinguish their IUnknown type.
    SAFE_RELEASE_LIST(mpPixelShaderList);
    SAFE_RELEASE_LIST(mpComputeShaderList);
    SAFE_RELEASE_LIST(mpVertexShaderList);
    SAFE_RELEASE_LIST(mpGeometryShaderList);
    SAFE_RELEASE_LIST(mpHullShaderList);
    SAFE_RELEASE_LIST(mpDomainShaderList);

    // Call base class implementation to clean up the non-DX object lists
    CPUTAssetLibrary::ReleaseAllLibraryLists();
}

void CPUTAssetLibraryDX11::AddPixelShader(const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTPixelShaderDX11 *pShader)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pShader, mpPixelShaderList);
}

void CPUTAssetLibraryDX11::AddComputeShader(const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTComputeShaderDX11 *pShader)
    {
	AddAsset(name, prefixDecoration, suffixDecoration, pShader, mpComputeShaderList);
    }

void CPUTAssetLibraryDX11::AddVertexShader(const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTVertexShaderDX11 *pShader)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pShader, mpVertexShaderList);
}

void CPUTAssetLibraryDX11::AddGeometryShader(const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTGeometryShaderDX11 *pShader)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pShader, mpGeometryShaderList);
}

void CPUTAssetLibraryDX11::AddHullShader(const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTHullShaderDX11 *pShader)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pShader, mpHullShaderList);
}

void CPUTAssetLibraryDX11::AddDomainShader(const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTDomainShaderDX11 *pShader)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pShader, mpDomainShaderList);
}

// Retrieve specified pixel shader
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::GetPixelShader(
    const std::string        &name,
    const std::string        &shaderMain,
    const std::string        &shaderProfile,
    CPUTPixelShaderDX11 **ppPixelShader,
    bool                  nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO    *pShaderMacros
)
{
    CPUTResult result = CPUT_SUCCESS;
    std::string finalName;

    if( name.at(0) == '%' )
    {
        finalName = mSystemDirectoryName + "/Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        CPUTFileSystem::ResolveAbsolutePathAndFilename(finalName, &finalName);
    } else if( name.at(0) == '$' )
    {
        finalName = name;
    } else
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName );
    }

    // see if the shader is already in the library
    void *pShader = FindPixelShader(finalName + shaderMain + shaderProfile, true);
    if(NULL!=pShader)
    {
        *ppPixelShader = (CPUTPixelShaderDX11*) pShader;
        (*ppPixelShader)->AddRef();
        return result;
    }
    *ppPixelShader = CPUTPixelShaderDX11::Create( finalName, shaderMain, shaderProfile, pShaderMacros );

    return result;
}

// Retrieve specified pixel shader
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::GetComputeShader(
    const std::string          &name,
    const std::string          &shaderMain,
    const std::string          &shaderProfile,
    CPUTComputeShaderDX11 **ppComputeShader,
    bool                    nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO      *pShaderMacros
)
{
    CPUTResult result = CPUT_SUCCESS;
    std::string finalName;
    if( name.at(0) == '%' )
    {
        finalName = mSystemDirectoryName + "/Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        CPUTFileSystem::ResolveAbsolutePathAndFilename(finalName, &finalName);
    } else if( name.at(0) == '$' )
    {
        finalName = name;
    } else
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName);
    }

    // see if the shader is already in the library
    void *pShader = FindComputeShader(finalName + shaderMain + shaderProfile, true);
    if(NULL!=pShader)
    {
        *ppComputeShader = (CPUTComputeShaderDX11*) pShader;
        (*ppComputeShader)->AddRef();
        return result;
    }
    *ppComputeShader = CPUTComputeShaderDX11::Create( finalName, shaderMain, shaderProfile, pShaderMacros );

    return result;
}

// Retrieve specified vertex shader
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::GetVertexShader(
    const std::string         &name,
    const std::string         &shaderMain,
    const std::string         &shaderProfile,
    CPUTVertexShaderDX11 **ppVertexShader,
    bool                   nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO     *pShaderMacros
)
{
    CPUTResult result = CPUT_SUCCESS;
    std::string finalName;
    if( name.at(0) == '%' )
    {
        finalName = mSystemDirectoryName + "/Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        CPUTFileSystem::ResolveAbsolutePathAndFilename(finalName, &finalName);
    } else if( name.at(0) == '$' )
    {
        finalName = name;
    } else
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName);
    }

    // see if the shader is already in the library
    void *pShader = FindVertexShader(finalName + shaderMain + shaderProfile, true);
    if(NULL!=pShader)
    {
        *ppVertexShader = (CPUTVertexShaderDX11*) pShader;
        (*ppVertexShader)->AddRef();
        return result;
    }
    *ppVertexShader = CPUTVertexShaderDX11::Create( finalName, shaderMain, shaderProfile, pShaderMacros );

    return result;
}

// Retrieve specified geometry shader
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::GetGeometryShader(
    const std::string           &name,
    const std::string           &shaderMain,
    const std::string           &shaderProfile,
    CPUTGeometryShaderDX11 **ppGeometryShader,
    bool                     nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO       *pShaderMacros
    )
{
    CPUTResult result = CPUT_SUCCESS;
    std::string finalName;
    if( name.at(0) == '%' )
    {
        finalName = mSystemDirectoryName + "/Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        CPUTFileSystem::ResolveAbsolutePathAndFilename(finalName, &finalName);
    } else if( name.at(0) == '$' )
    {
        finalName = name;
    } else
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName);
    }

    // see if the shader is already in the library
    void *pShader = FindGeometryShader(finalName + shaderMain + shaderProfile, true);
    if(NULL!=pShader)
    {
        *ppGeometryShader = (CPUTGeometryShaderDX11*) pShader;
        (*ppGeometryShader)->AddRef();
        return result;
    }
    *ppGeometryShader = CPUTGeometryShaderDX11::Create( finalName, shaderMain, shaderProfile, pShaderMacros );

    return result;
}

// Retrieve specified hull shader
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::GetHullShader(
    const std::string       &name,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTHullShaderDX11 **ppHullShader,
    bool                 nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
    )
{
    CPUTResult result = CPUT_SUCCESS;
    std::string finalName;
    if( name.at(0) == '%' )
    {
        finalName = mSystemDirectoryName + "/Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        CPUTFileSystem::ResolveAbsolutePathAndFilename(finalName, &finalName);
    } else if( name.at(0) == '$' )
    {
        finalName = name;
    } else
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName);
    }

    // see if the shader is already in the library
    void *pShader = FindHullShader(finalName + shaderMain + shaderProfile, true);
    if(NULL!=pShader)
    {
        *ppHullShader = (CPUTHullShaderDX11*) pShader;
        (*ppHullShader)->AddRef();
        return result;
    }
    *ppHullShader = CPUTHullShaderDX11::Create( finalName, shaderMain, shaderProfile, pShaderMacros );

    return result;

}

// Retrieve specified domain shader
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::GetDomainShader(
    const std::string         &name,
    const std::string         &shaderMain,
    const std::string         &shaderProfile,
    CPUTDomainShaderDX11 **ppDomainShader,
    bool                   nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO     *pShaderMacros
    )
{
    CPUTResult result = CPUT_SUCCESS;
    std::string finalName;
    if( name.at(0) == '%' )
    {
        finalName = mSystemDirectoryName + "/Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        CPUTFileSystem::ResolveAbsolutePathAndFilename(finalName, &finalName);
    } else if( name.at(0) == '$' )
    {
        finalName = name;
    } else
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName);
    }

    // see if the shader is already in the library
    void *pShader = FindDomainShader(finalName + shaderMain + shaderProfile, true);
    if(NULL!=pShader)
    {
        *ppDomainShader = (CPUTDomainShaderDX11*) pShader;
        (*ppDomainShader)->AddRef();
        return result;
    }
    *ppDomainShader = CPUTDomainShaderDX11::Create( finalName, shaderMain, shaderProfile, pShaderMacros );

    return result;
}

//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::CreatePixelShaderFromMemory(
    const std::string        &name,
    const std::string        &shaderMain,
    const std::string        &shaderProfile,
    CPUTPixelShaderDX11 **ppShader,
    char                 *pShaderSource,
    CPUT_SHADER_MACRO    *pShaderMacros
)
{
#ifdef _DEBUG
    void *pShader = FindPixelShader(name + shaderMain + shaderProfile, true);
    LIBRARY_ASSERT( NULL == pShader, "Shader already exists." );
#endif
    *ppShader = CPUTPixelShaderDX11::CreateFromMemory( name, shaderMain, shaderProfile, pShaderSource, pShaderMacros);
    return CPUT_SUCCESS;
}

//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::CreateVertexShaderFromMemory(
    const std::string        &name,
    const std::string        &shaderMain,
    const std::string        &shaderProfile,
    CPUTVertexShaderDX11 **ppShader,
    char                 *pShaderSource,
    CPUT_SHADER_MACRO    *pShaderMacros
)
{
#ifdef _DEBUG
    void *pShader = FindVertexShader(name + shaderMain + shaderProfile, true);
    LIBRARY_ASSERT( NULL == pShader, "Shader already exists." );
#endif
    *ppShader = CPUTVertexShaderDX11::CreateFromMemory( name, shaderMain, shaderProfile, pShaderSource, pShaderMacros );
    return CPUT_SUCCESS;
}

//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::CreateComputeShaderFromMemory(
    const std::string          &name,
    const std::string          &shaderMain,
    const std::string          &shaderProfile,
    CPUTComputeShaderDX11 **ppShader,
    char                   *pShaderSource,
    CPUT_SHADER_MACRO      *pShaderMacros
)
{
#ifdef _DEBUG
    void *pShader = FindComputeShader(name + shaderMain + shaderProfile, true);
    LIBRARY_ASSERT( NULL == pShader, "Shader already exists." );
#endif
    *ppShader = CPUTComputeShaderDX11::CreateFromMemory( name, shaderMain, shaderProfile, pShaderSource, pShaderMacros );
    return CPUT_SUCCESS;
}

// If filename ends in .fxo or .cso, then simply read the binary contents to an ID3DBlob.
// Otherwise, generate the ID3DBlob using D3DCompileFromFile().
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::CompileShaderFromFile(
    const std::string     &fileName,
    const std::string     &shaderMain,
    const std::string     &shaderProfile,
    ID3DBlob         **ppBlob,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    size_t len = fileName.length();
    const char *pExt = &(fileName.c_str())[len-3];
    if( strcmp( pExt, "fxo" ) == 0 || _stricmp( pExt, "cso" ) == 0 )
    {
        // Binary file, load from file.
        FILE *fp;
        errno_t ret = fopen_s( &fp, fileName.c_str(), "rb" );
        LIBRARY_ASSERT( ret == 0, "Failed opening file " + fileName );
        if( ret != 0 )
        {
            return CPUT_ERROR_FILE_NOT_FOUND;
        }

        fseek( fp, 0, SEEK_END );
        int size = ftell( fp );
        fseek( fp, 0L, SEEK_SET );

        D3DCreateBlob( size, ppBlob );
        fread( (*ppBlob)->GetBufferPointer(), size, 1, fp );
        fclose( fp );
    }
    else
    {
        char pShaderMainAsChar[128];
        char pShaderProfileAsChar[128];
        LIBRARY_ASSERT( shaderMain.length()     < 128, "Shader main name '"    + shaderMain    + "' longer than 128 chars." );
        LIBRARY_ASSERT( shaderProfile.length()  < 128, "Shader profile name '" + shaderProfile + "' longer than 128 chars." );
        strcpy_s( pShaderMainAsChar, 128, shaderMain.c_str());
        strcpy_s( pShaderProfileAsChar, 128, shaderProfile.c_str());

        // Prepare the macros
        int macroCount = 0;
        D3D_SHADER_MACRO *pTmp = (D3D_SHADER_MACRO *)pShaderMacros;
        if( pTmp )
        {
            while( (pTmp++)->Name ) { macroCount++; }
        }
        const UINT NUM_ADDITIONAL_MACROS = 5;
        D3D_SHADER_MACRO pAdditionalMacros[NUM_ADDITIONAL_MACROS] = {
            { "_CPUT", "1" },
            { "FOG_COLOR", "(float3( 0.89f, 0.92f, 0.88f )*0.5f)" },
            { "FOG_START", "2000.0f" },
            { "FOG_END",   "6000.0f" },
            { NULL, NULL }
        };
        const int MAX_DEFINES = 128; // Note: use MAX_DEFINES to avoid dynamic allocation.  Arbitrarily choose 128.  Not sure if there is a real limit.
        LIBRARY_ASSERT( macroCount<(MAX_DEFINES-NUM_ADDITIONAL_MACROS), "Too many shader macros." );
        D3D_SHADER_MACRO pFinalShaderMacros[MAX_DEFINES];
        memcpy( pFinalShaderMacros, pShaderMacros, macroCount*sizeof(D3D_SHADER_MACRO));
        memcpy( &pFinalShaderMacros[macroCount], pAdditionalMacros, sizeof(pAdditionalMacros));

        ID3DBlob *pErrorBlob = NULL;
        uint32_t numWChars = MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, NULL, 0);
        wchar_t* wstr = new wchar_t[numWChars];
        MultiByteToWideChar( CP_UTF8 , 0 , fileName.c_str() , -1, wstr , numWChars );

        HRESULT hr = D3DCompileFromFile(
            wstr,
            pFinalShaderMacros,
            D3D_COMPILE_STANDARD_FILE_INCLUDE, // includes
            pShaderMainAsChar,
            pShaderProfileAsChar,
			D3DCOMPILE_DEBUG,                    // flags 1
            0,                    // flags 2
            ppBlob,
            &pErrorBlob
        );
        delete[] wstr;
        if (FAILED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                DEBUGMESSAGEBOX("File load error", "File not found: " + fileName + ".")
            else
                DEBUGMESSAGEBOX("Error Shader Creation", "Could not create shader: " + fileName + ".");
//            LIBRARY_ASSERT(false, "Failed to load shader: " + fileName)
  //              return CPUT_ERROR;
        }
        LIBRARY_ASSERT( SUCCEEDED(hr), "Error compiling shader '" + fileName + "'.\n" + (pErrorBlob ? (char*)pErrorBlob->GetBufferPointer() : "no error message" ) );
        UNREFERENCED_PARAMETER(hr);

        if(pErrorBlob)
        {
            pErrorBlob->Release();
        }
    } // Compiled from source
    return CPUT_SUCCESS;
}

// Use DX11 compile from file method to do all the heavy lifting
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryDX11::CompileShaderFromMemory(
    const char        *pShaderSource,
    const std::string     &shaderMain,
    const std::string     &shaderProfile,
    ID3DBlob         **ppBlob,
    CPUT_SHADER_MACRO *pShaderMacros
)
{
    char pShaderMainAsChar[128];
    char pShaderProfileAsChar[128];
    LIBRARY_ASSERT( shaderMain.length()     < 128, "Shader main name '"    + shaderMain    + "' longer than 128 chars." );
    LIBRARY_ASSERT( shaderProfile.length()  < 128, "Shader profile name '" + shaderProfile + "' longer than 128 chars." );
    strcpy_s( pShaderMainAsChar, 128, shaderMain.c_str());
    strcpy_s( pShaderProfileAsChar, 128, shaderProfile.c_str());

    // Prepare the macros
    int macroCount = 0;
    D3D_SHADER_MACRO *pTmp = (D3D_SHADER_MACRO *)pShaderMacros;
    if( pTmp )
    {
        while( (pTmp++)->Name ) { macroCount++; }
    }

    const int MAX_DEFINES = 128; // Note: use MAX_DEFINES to avoid dynamic allocation.  Arbitrarily choose 128.  Not sure if there is a real limit.
    LIBRARY_ASSERT( macroCount<MAX_DEFINES, "Error compiling shader.\nToo many shader macros." );
    D3D_SHADER_MACRO pFinalShaderMacros[MAX_DEFINES]; 
    memcpy( pFinalShaderMacros, pShaderMacros, macroCount*sizeof(D3D_SHADER_MACRO));

    ID3DBlob *pErrorBlob = NULL;
    HRESULT hr = D3DCompile(
        pShaderSource,
        strlen( pShaderSource ),
        shaderMain.c_str(),   // Use entrypoint as file name
        pFinalShaderMacros,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // includes
        pShaderMainAsChar,
        pShaderProfileAsChar,
		0,//D3DCOMPILE_DEBUG,                    // flags 1
        0,                    // flags 2
        ppBlob,
        &pErrorBlob
    );
    LIBRARY_ASSERT( SUCCEEDED(hr), "Error compiling shader '" + shaderMain + "'.\n" + (pErrorBlob ? (char*)pErrorBlob->GetBufferPointer() : "no error message" ) );
    UNREFERENCED_PARAMETER(hr);
    if(pErrorBlob)
    {
        pErrorBlob->Release();
    }
    return CPUT_SUCCESS;
}
