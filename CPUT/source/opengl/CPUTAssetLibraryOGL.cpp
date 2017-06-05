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
//#include "D3dx9tex.h"  // super-annoying - must be first or you get new() operator overloading errors during compile b/c of D3DXGetImageInfoFromFile() function

#include "CPUTAssetLibraryOGL.h"

// define the objects we'll need
#include "CPUTMaterialOGL.h"
#include "CPUTTextureOGL.h"
#include "CPUTLight.h"
#include "CPUTCamera.h"
#include "CPUTShaderOGL.h"

// MPF: opengl es - yipe - can't do both at the same time - need to have it bind dynamically/via compile-time
std::vector<CPUTAsset<CPUTShaderOGL>> CPUTAssetLibraryOGL::mpPixelShaderList;
std::vector<CPUTAsset<CPUTShaderOGL>> CPUTAssetLibraryOGL::mpComputeShaderList;
std::vector<CPUTAsset<CPUTShaderOGL>> CPUTAssetLibraryOGL::mpVertexShaderList;
std::vector<CPUTAsset<CPUTShaderOGL>> CPUTAssetLibraryOGL::mpGeometryShaderList;
std::vector<CPUTAsset<CPUTShaderOGL>> CPUTAssetLibraryOGL::mpHullShaderList;
std::vector<CPUTAsset<CPUTShaderOGL>> CPUTAssetLibraryOGL::mpDomainShaderList;

CPUTAssetLibrary* CPUTAssetLibrary::GetAssetLibrary()
{
    if(NULL==mpAssetLibrary)
    {
        mpAssetLibrary = CPUTAssetLibraryOGL::Create();
    }
    return mpAssetLibrary;
}

// TODO: Change OS Services to a flat list of CPUT* functions.  Avoid calls all over the place like:
// CPUTOSServices::GetOSServices();

// Deletes and properly releases all asset library lists that contain
// unwrapped IUnknown DirectX objects.
//-----------------------------------------------------------------------------
void CPUTAssetLibraryOGL::ReleaseAllLibraryLists()
{
    // TODO: we really need to wrap the DX assets so we don't need to distinguish their IUnknown type.
    SAFE_RELEASE_LIST(mpPixelShaderList);
    SAFE_RELEASE_LIST(mpComputeShaderList);
    SAFE_RELEASE_LIST(mpVertexShaderList);
    SAFE_RELEASE_LIST(mpGeometryShaderList);
    SAFE_RELEASE_LIST(mpHullShaderList);
    SAFE_RELEASE_LIST(mpDomainShaderList);

    // Call base class implementation to clean up the non-DX object lists
    return CPUTAssetLibrary::ReleaseAllLibraryLists();
}

void CPUTAssetLibraryOGL::AddVertexShader(const std::string &name, CPUTShaderOGL *pShader)
{
	AddAsset("", name, "", pShader, mpVertexShaderList);
}

void CPUTAssetLibraryOGL::AddPixelShader(const std::string &name, CPUTShaderOGL *pShader)
{
	AddAsset("", name, "", pShader, mpPixelShaderList);
}

void CPUTAssetLibraryOGL::AddHullShader(const std::string &name, CPUTShaderOGL *pShader)
{
	AddAsset("", name, "", pShader, mpHullShaderList);
}

void CPUTAssetLibraryOGL::AddDomainShader(const std::string &name, CPUTShaderOGL *pShader)
{
	AddAsset("", name, "", pShader, mpDomainShaderList);
}

void CPUTAssetLibraryOGL::AddGeometryShader(const std::string &name, CPUTShaderOGL *pShader)
{
	AddAsset("", name, "", pShader, mpGeometryShaderList);
}

// Retrieve specified pixel shader
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryOGL::GetPixelShader(
    const std::string        &name,
    const std::string        &shaderMain,
    const std::string        &shaderProfile,
    CPUTShaderOGL  **ppPixelShader,
    bool                  nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO    *pShaderMacros
)
{
    std::vector<std::string> filenames;
    filenames.push_back(name);
    return GetPixelShader(filenames, shaderMain, shaderProfile, ppPixelShader, nameIsFullPathAndFilename, pShaderMacros);
}

CPUTResult CPUTAssetLibraryOGL::GetPixelShader(
    const std::vector<std::string>  &fileNames,
    const std::string               &shaderMain,
    const std::string               &shaderProfile,
    CPUTShaderOGL          **ppPixelShader,
    bool                        nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO           *pShaderMacros
)
{
    CPUTResult result = CPUT_SUCCESS;
    std::vector<std::string> finalNames;
    std::string libName;
    for(UINT i=0; i<fileNames.size(); i++)   
    {   
        std::string name = fileNames[i];
        std::string finalName;
        if( fileNames[i].at(0) == '%' )
        {
            // Consider moving slashes to some sub variable dependent from OS,
            // or even MACRO would be better in such situation.
            finalName = mSystemDirectoryName + "Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        } else if( fileNames[i].at(0) == '$' )
        {
            finalName = fileNames[i];
        } else
        {
            CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName );
        }
        libName += finalName;
        finalNames.push_back(finalName);
    }
    std::string name = CPUTShaderOGL::CreateShaderName(finalNames, GL_FRAGMENT_SHADER, pShaderMacros);
    // see if the shader is already in the library
    void *pShader = FindPixelShader(name, true);
    if(NULL!=pShader)
    {
        *ppPixelShader = (CPUTShaderOGL*)pShader;
        (*ppPixelShader)->AddRef();
        return result;
    }
    *ppPixelShader = CPUTShaderOGL::CreateFromFiles( finalNames, GL_FRAGMENT_SHADER, CPUT_OGL::GLSL_VERSION, CPUT_OGL::DEFAULT_MACROS, pShaderMacros);
    AddPixelShader(name, *ppPixelShader);
    return result;
}

// Retrieve specified vertex shader
//-----------------------------------------------------------------------------
CPUTResult CPUTAssetLibraryOGL::GetVertexShader(
    const std::string       &name,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTShaderOGL       **ppVertexShader,
    bool                nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
)
{
    std::vector<std::string> filenames;
    filenames.push_back(name);
    return GetVertexShader(filenames, shaderMain, shaderProfile, ppVertexShader, nameIsFullPathAndFilename, pShaderMacros);
}

CPUTResult CPUTAssetLibraryOGL::GetVertexShader(
    const std::vector<std::string> &fileNames,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTShaderOGL       **ppVertexShader,
    bool                nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
)
{
    CPUTResult result = CPUT_SUCCESS;
    std::vector<std::string> finalNames;
    std::string libName;
    for(UINT i=0; i<fileNames.size(); i++)   
    {   
        std::string name = fileNames[i];
        std::string finalName;
        if( fileNames[i].at(0) == '%' )
        {
            // Consider moving slashes to some sub variable dependent from OS,
            // or even MACRO would be better in such situation.
            finalName = mSystemDirectoryName + "Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        } else if( fileNames[i].at(0) == '$' )
        {
            finalName = fileNames[i];
        } else
        {
            CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName );
        }
        libName += finalName;
        finalNames.push_back(finalName);
    }
    // see if the shader is already in the library
    std::string name = CPUTShaderOGL::CreateShaderName(finalNames, GL_VERTEX_SHADER, pShaderMacros);

    void *pShader = FindVertexShader(name, true);
    if(NULL!=pShader)
    {
        *ppVertexShader = (CPUTShaderOGL*)pShader;
        (*ppVertexShader)->AddRef();
        return result;
    }
    *ppVertexShader = CPUTShaderOGL::CreateFromFiles( finalNames, GL_VERTEX_SHADER, CPUT_OGL::GLSL_VERSION, CPUT_OGL::DEFAULT_MACROS, pShaderMacros);
    AddVertexShader(name, *ppVertexShader);

    return result;
}

CPUTResult CPUTAssetLibraryOGL::GetHullShader(
    const std::string       &name,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTShaderOGL       **ppHullShader,
    bool                nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
)
{
    std::vector<std::string> filenames;
    filenames.push_back(name);
    return GetHullShader(filenames, shaderMain, shaderProfile, ppHullShader, nameIsFullPathAndFilename, pShaderMacros);
}

CPUTResult CPUTAssetLibraryOGL::GetHullShader(
    const std::vector<std::string> &fileNames,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTShaderOGL       **ppHullShader,
    bool                nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
)
{
    CPUTResult result = CPUT_SUCCESS;
    std::vector<std::string> finalNames;
    std::string libName;
    for(UINT i=0; i<fileNames.size(); i++)   
    {   
        std::string name = fileNames[i];
        std::string finalName;
        if( fileNames[i].at(0) == '%' )
        {
            // Consider moving slashes to some sub variable dependent from OS,
            // or even MACRO would be better in such situation.
            finalName = mSystemDirectoryName + "Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        } else if( fileNames[i].at(0) == '$' )
        {
            finalName = fileNames[i];
        } else
        {
            CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName );
        }
        libName += finalName;
        finalNames.push_back(finalName);
    }

    std::string name = CPUTShaderOGL::CreateShaderName(finalNames, GL_TESS_CONTROL_SHADER, pShaderMacros);
    // see if the shader is already in the library
    void *pShader = FindHullShader(name, true);
    if(NULL!=pShader)
    {
        *ppHullShader = (CPUTShaderOGL*)pShader;
        (*ppHullShader)->AddRef();
        return result;
    }
    *ppHullShader = CPUTShaderOGL::CreateFromFiles( finalNames, GL_TESS_CONTROL_SHADER, CPUT_OGL::GLSL_VERSION, CPUT_OGL::DEFAULT_MACROS, pShaderMacros);
    AddHullShader(name, *ppHullShader);

    return result;
}

CPUTResult CPUTAssetLibraryOGL::GetDomainShader(
    const std::string       &name,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTShaderOGL       **ppDomainShader,
    bool                nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
)
{
    std::vector<std::string> filenames;
    filenames.push_back(name);
    return GetDomainShader(filenames, shaderMain, shaderProfile, ppDomainShader, nameIsFullPathAndFilename, pShaderMacros);
}

CPUTResult CPUTAssetLibraryOGL::GetDomainShader(
    const std::vector<std::string> &fileNames,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTShaderOGL       **ppDomainShader,
    bool                nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
)
{
    CPUTResult result = CPUT_SUCCESS;
    std::vector<std::string> finalNames;
    std::string libName;
    for(UINT i=0; i<fileNames.size(); i++)   
    {   
        std::string name = fileNames[i];
        std::string finalName;
        if( fileNames[i].at(0) == '%' )
        {
            // Consider moving slashes to some sub variable dependent from OS,
            // or even MACRO would be better in such situation.
            finalName = mSystemDirectoryName + "Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        } else if( fileNames[i].at(0) == '$' )
        {
            finalName = fileNames[i];
        } else
        {
            CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName );
        }
        libName += finalName;
        finalNames.push_back(finalName);
    }

    std::string name = CPUTShaderOGL::CreateShaderName(finalNames, GL_TESS_EVALUATION_SHADER, pShaderMacros);
    // see if the shader is already in the library
    void *pShader = FindDomainShader(name, true);
    if(NULL!=pShader)
    {
        *ppDomainShader = (CPUTShaderOGL*)pShader;
        (*ppDomainShader)->AddRef();
        return result;
    }
    *ppDomainShader = CPUTShaderOGL::CreateFromFiles( finalNames, GL_TESS_EVALUATION_SHADER, CPUT_OGL::GLSL_VERSION, CPUT_OGL::DEFAULT_MACROS, pShaderMacros);
    AddDomainShader(name, *ppDomainShader);

    return result;
}

CPUTResult CPUTAssetLibraryOGL::GetGeometryShader(
    const std::string       &name,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTShaderOGL       **ppGeometryShader,
    bool                nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
)
{
    std::vector<std::string> filenames;
    filenames.push_back(name);
    return GetGeometryShader(filenames, shaderMain, shaderProfile, ppGeometryShader, nameIsFullPathAndFilename, pShaderMacros);
}

CPUTResult CPUTAssetLibraryOGL::GetGeometryShader(
    const std::vector<std::string> &fileNames,
    const std::string       &shaderMain,
    const std::string       &shaderProfile,
    CPUTShaderOGL       **ppGeometryShader,
    bool                nameIsFullPathAndFilename,
    CPUT_SHADER_MACRO   *pShaderMacros
)
{
    CPUTResult result = CPUT_SUCCESS;
    std::vector<std::string> finalNames;
    std::string libName;
    for(UINT i=0; i<fileNames.size(); i++)   
    {   
        std::string name = fileNames[i];
        std::string finalName;
        if( fileNames[i].at(0) == '%' )
        {
            // Consider moving slashes to some sub variable dependent from OS,
            // or even MACRO would be better in such situation.
            finalName = mSystemDirectoryName + "Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        } else if( fileNames[i].at(0) == '$' )
        {
            finalName = fileNames[i];
        } else
        {
            CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName );
        }
        libName += finalName;
        finalNames.push_back(finalName);
    }

    std::string name = CPUTShaderOGL::CreateShaderName(finalNames, GL_GEOMETRY_SHADER, pShaderMacros);
    // see if the shader is already in the library
    void *pShader = FindGeometryShader(name, true);
    if(NULL!=pShader)
    {
        *ppGeometryShader = (CPUTShaderOGL*)pShader;
        (*ppGeometryShader)->AddRef();
        return result;
    }
    *ppGeometryShader = CPUTShaderOGL::CreateFromFiles( finalNames, GL_GEOMETRY_SHADER, CPUT_OGL::GLSL_VERSION, CPUT_OGL::DEFAULT_MACROS, pShaderMacros);
    AddGeometryShader(name, *ppGeometryShader);
    return result;
}

CPUTAssetLibraryOGL* CPUTAssetLibraryOGL::Create()
{
	return new CPUTAssetLibraryOGL();
}
