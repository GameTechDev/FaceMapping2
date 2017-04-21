/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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
