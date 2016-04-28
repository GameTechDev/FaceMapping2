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
#ifndef __CPUTASSETLIBRARYDX11_H__
#define __CPUTASSETLIBRARYDX11_H__

#include "CPUTAssetLibrary.h"
#include "CPUTConfigBlock.h"

#include <d3d11.h>

class CPUTAssetSet;
class CPUTMaterial;
class CPUTModel;
class CPUTNullNode;
class CPUTCamera;
class CPUTRenderStateBlock;
class CPUTLight;
class CPUTTexture;
class CPUTVertexShaderDX11;
class CPUTPixelShaderDX11;
class CPUTComputeShaderDX11;
class CPUTGeometryShaderDX11;
class CPUTHullShaderDX11;
class CPUTDomainShaderDX11;

//-----------------------------------------------------------------------------
struct CPUTSRGBLoadFlags
{
    bool bInterpretInputasSRGB;
    bool bWritetoSRGBOutput;
};

//-----------------------------------------------------------------------------
class CPUTAssetLibraryDX11:public CPUTAssetLibrary
{
protected:
    static std::vector<CPUTAsset<CPUTPixelShaderDX11>> mpPixelShaderList;
    static std::vector<CPUTAsset<CPUTComputeShaderDX11>> mpComputeShaderList;
    static std::vector<CPUTAsset<CPUTVertexShaderDX11>> mpVertexShaderList;
    static std::vector<CPUTAsset<CPUTGeometryShaderDX11>> mpGeometryShaderList;
    static std::vector<CPUTAsset<CPUTHullShaderDX11>> mpHullShaderList;
    static std::vector<CPUTAsset<CPUTDomainShaderDX11>> mpDomainShaderList;

public:
    CPUTAssetLibraryDX11(){}
    virtual ~CPUTAssetLibraryDX11()
    {
        ReleaseAllLibraryLists();
    }

    virtual void ReleaseAllLibraryLists();

    void AddPixelShader(    const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTPixelShaderDX11    *pShader);
    void AddComputeShader(  const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTComputeShaderDX11  *pShader);
    void AddVertexShader(   const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTVertexShaderDX11   *pShader);
    void AddGeometryShader( const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTGeometryShaderDX11 *pShader);
    void AddHullShader(     const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTHullShaderDX11     *pShader);
    void AddDomainShader(   const std::string &name, const std::string prefixDecoration, const std::string &suffixDecoration, CPUTDomainShaderDX11   *pShader);
    
    CPUTPixelShaderDX11    *FindPixelShader(    const std::string &name, bool nameIsFullPathAndFilename=false ) { return    (CPUTPixelShaderDX11*)FindAsset( name, mpPixelShaderList,     nameIsFullPathAndFilename);}
    CPUTComputeShaderDX11  *FindComputeShader(  const std::string &name, bool nameIsFullPathAndFilename=false ) { return  (CPUTComputeShaderDX11*)FindAsset( name, mpComputeShaderList,   nameIsFullPathAndFilename);}
    CPUTVertexShaderDX11   *FindVertexShader(   const std::string &name, bool nameIsFullPathAndFilename=false ) { return   (CPUTVertexShaderDX11*)FindAsset( name, mpVertexShaderList,    nameIsFullPathAndFilename);}
    CPUTGeometryShaderDX11 *FindGeometryShader( const std::string &name, bool nameIsFullPathAndFilename=false )  { return (CPUTGeometryShaderDX11*)FindAsset( name, mpGeometryShaderList, nameIsFullPathAndFilename);}
    CPUTHullShaderDX11     *FindHullShader(     const std::string &name, bool nameIsFullPathAndFilename=false ) { return     (CPUTHullShaderDX11*)FindAsset( name, mpHullShaderList,      nameIsFullPathAndFilename);}
    CPUTDomainShaderDX11   *FindDomainShader(   const std::string &name, bool nameIsFullPathAndFilename=false ) { return   (CPUTDomainShaderDX11*)FindAsset( name, mpDomainShaderList,    nameIsFullPathAndFilename);}

    // shaders - vertex, pixel
    CPUTResult GetPixelShader(     const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTPixelShaderDX11    **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult GetComputeShader(   const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTComputeShaderDX11  **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult GetVertexShader(    const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTVertexShaderDX11   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult GetGeometryShader(  const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTGeometryShaderDX11 **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult GetHullShader(      const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTHullShaderDX11     **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult GetDomainShader(    const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTDomainShaderDX11   **ppShader, bool nameIsFullPathAndFilename=false, CPUT_SHADER_MACRO *pShaderMacros=NULL );
 
    // shaders - vertex, pixel
    CPUTResult CreatePixelShaderFromMemory(     const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTPixelShaderDX11    **ppShader, char *pShaderSource, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult CreateComputeShaderFromMemory(   const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTComputeShaderDX11  **ppShader, char *pShaderSource, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult CreateVertexShaderFromMemory(    const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTVertexShaderDX11   **ppShader, char *pShaderSource, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult CreateGeometryShaderFromMemory(  const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTGeometryShaderDX11 **ppShader, char *pShaderSource, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult CreateHullShaderFromMemory(      const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTHullShaderDX11     **ppShader, char *pShaderSource, CPUT_SHADER_MACRO *pShaderMacros=NULL );
    CPUTResult CreateDomainShaderFromMemory(    const std::string &name, const std::string &shaderMain, const std::string &shaderProfile, CPUTDomainShaderDX11   **ppShader, char *pShaderSource, CPUT_SHADER_MACRO *pShaderMacros=NULL );
 
    CPUTResult CompileShaderFromFile(  const std::string &fileName,   const std::string &shaderMain, const std::string &shaderProfile, ID3DBlob **ppBlob, CPUT_SHADER_MACRO  *pShaderMacros=NULL );
    CPUTResult CompileShaderFromMemory(const char *pShaderSource, const std::string &shaderMain, const std::string &shaderProfile, ID3DBlob **ppBlob, CPUT_SHADER_MACRO  *pShaderMacros=NULL );
};

#endif // #ifndef __CPUTASSETLIBRARYDX11_H__
