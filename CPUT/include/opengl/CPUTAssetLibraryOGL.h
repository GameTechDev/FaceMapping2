//--------------------------------------------------------------------------------------
// Copyright 2011 Intel Corporation
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
