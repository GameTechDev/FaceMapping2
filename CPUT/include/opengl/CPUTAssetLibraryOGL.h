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
#ifndef __CPUTASSETLIBRARYOGL_H__
#define __CPUTASSETLIBRARYOGL_H__

#include "CPUTAssetLibrary.h"
#include "CPUTConfigBlock.h"
#include <vector>
class CPUTAssetSet;
class CPUTMaterial;
class CPUTModel;
class CPUTNullNode;
class CPUTCamera;
class CPUTRenderStateBlock;
class CPUTLight;
class CPUTTexture;
class CPUTShaderOGL;

//-----------------------------------------------------------------------------
struct CPUTSRGBLoadFlags
{
    bool bInterpretInputasSRGB;
    bool bWritetoSRGBOutput;
};

//-----------------------------------------------------------------------------
class CPUTAssetLibraryOGL:public CPUTAssetLibrary
{
protected:
    CPUTAssetLibraryOGL(){}
	
    static std::vector<CPUTAsset<CPUTShaderOGL>> mpPixelShaderList;
    static std::vector<CPUTAsset<CPUTShaderOGL>> mpComputeShaderList;
    static std::vector<CPUTAsset<CPUTShaderOGL>> mpVertexShaderList;
    static std::vector<CPUTAsset<CPUTShaderOGL>> mpGeometryShaderList;
    static std::vector<CPUTAsset<CPUTShaderOGL>> mpHullShaderList;
    static std::vector<CPUTAsset<CPUTShaderOGL>> mpDomainShaderList;

public:

	static CPUTAssetLibraryOGL* Create();

    virtual ~CPUTAssetLibraryOGL()
    {
        ReleaseAllLibraryLists();
    }

    virtual void ReleaseAllLibraryLists();
    
    void AddVertexShader(   const std::string &name, CPUTShaderOGL *pShader);
    void AddPixelShader(    const std::string &name, CPUTShaderOGL *pShader);

    void AddHullShader( const std::string &name, CPUTShaderOGL *pShader);
    void AddDomainShader( const std::string &name, CPUTShaderOGL *pShader);
    void AddGeometryShader( const std::string &name, CPUTShaderOGL *pShader);

	CPUTShaderOGL   *FindVertexShader(   const std::string &name, /*const std::string &decoration,*/ bool nameIsFullPathAndFilename=false ) { return   FindAsset( name, /*decoration,*/ mpVertexShaderList,   nameIsFullPathAndFilename ); }
    CPUTShaderOGL   *FindPixelShader(    const std::string &name, /*const std::string &decoration,*/ bool nameIsFullPathAndFilename=false ) { return   FindAsset( name, /*decoration,*/ mpPixelShaderList,    nameIsFullPathAndFilename ); }
	CPUTShaderOGL   *FindHullShader(     const std::string &name, /*const std::string &decoration,*/ bool nameIsFullPathAndFilename=false ) { return   FindAsset( name, /*decoration,*/ mpHullShaderList,     nameIsFullPathAndFilename ); }
    CPUTShaderOGL   *FindDomainShader(   const std::string &name, /*const std::string &decoration,*/ bool nameIsFullPathAndFilename=false ) { return   FindAsset( name, /*decoration,*/ mpDomainShaderList,   nameIsFullPathAndFilename ); }
    CPUTShaderOGL   *FindGeometryShader(   const std::string &name, /*const std::string &decoration,*/ bool nameIsFullPathAndFilename=false ) { return FindAsset( name, /*decoration,*/ mpGeometryShaderList,   nameIsFullPathAndFilename ); }

    CPUTResult GetVertexShader(    const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);
    CPUTResult GetVertexShader(    const std::vector<std::string> &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);
    CPUTResult GetPixelShader(     const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);
    CPUTResult GetPixelShader(     const std::vector<std::string> &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL       **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);

	CPUTResult GetHullShader(    const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);
    CPUTResult GetHullShader(    const std::vector<std::string> &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);
    CPUTResult GetDomainShader(  const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);
    CPUTResult GetDomainShader(  const std::vector<std::string> &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL       **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);

	CPUTResult GetGeometryShader(    const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);
    CPUTResult GetGeometryShader(    const std::vector<std::string> &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTShaderOGL   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL);
};

#endif // #ifndef __CPUTASSETLIBRARYDX11_H__
