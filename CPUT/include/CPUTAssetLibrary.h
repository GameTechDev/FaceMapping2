/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __CPUTASSETLIBRARY_H__
#define __CPUTASSETLIBRARY_H__

#include "CPUT.h"
#include "CPUTOSServices.h" // TODO: why is this windows-specific?
#include <memory>
#include <vector>
#include <algorithm>
#include "CPUTRefCount.h"

// Global Asset Library
//
// The purpose of this library is to keep a copy of all loaded assets and
// provide a one-stop-loading system.  All assets that are loaded into the
// system via the Getxxx() operators stays in the library.  Further Getxxx()
// operations on an already loaded object will addref and return the previously
// loaded object.
//-----------------------------------------------------------------------------


// node that holds a single library object
template <typename T>
struct CPUTAsset
{
    UINT                hash;
    std::string         name;
	std::string         fileName; // potentially non-unique
	T*       pData;

	CPUTAsset(){
		static_assert(std::is_base_of<CPUTRefCount, T>::value, "Asset types must derive from CPUTRefCount");
	}
};

#define SAFE_RELEASE_LIST(list) ReleaseList(list);list.clear();
class CPUTRenderNode;
class CPUTAssetSet;
class CPUTNullNode;
class CPUTModel;
class CPUTMaterial;
class CPUTLight;
class CPUTCamera;
class CPUTTexture;
class CPUTBuffer;
class CPUTRenderStateBlock;
class CPUTFont;
class CPUTNodeAnimation;
class CPUTAnimation;

class CPUTAssetLibrary
{
protected:
    static CPUTAssetLibrary *mpAssetLibrary;

    // simple linked lists for now, but if we want to optimize or load blocks
    // we can change these to dynamically re-sizing arrays and then just do
    // memcopies into the structs.
    // Note: No camera, light, or NullNode directory names - they don't have payload files (i.e., they are defined completely in the .set file)
    std::string  mMediaDirectoryName;
    std::string  mAssetDirectoryName;
    std::string  mAssetSetDirectoryName;
    std::string  mModelDirectoryName;
    std::string  mMaterialDirectoryName;
    std::string  mTextureDirectoryName;
    std::string  mShaderDirectoryName;
    std::string  mFontDirectoryName;
    std::string  mSystemDirectoryName;
	std::string  mAnimationSetDirectoryName;
public: // TODO: temporary for debug.
    // TODO: Make these lists static.  Share assets (e.g., texture) across all requests for this process.
    static std::vector<CPUTAsset<CPUTAssetSet>>         mpAssetSetList;
    static std::vector<CPUTAsset<CPUTNullNode>>         mpNullNodeList;
    static std::vector<CPUTAsset<CPUTModel>>            mpModelList;
    static std::vector<CPUTAsset<CPUTCamera>>           mpCameraList;
    static std::vector<CPUTAsset<CPUTLight>>            mpLightList;
    static std::vector<CPUTAsset<CPUTMaterial>>         mpMaterialList;
    static std::vector<CPUTAsset<CPUTTexture>>          mpTextureList;
    static std::vector<CPUTAsset<CPUTBuffer>>           mpBufferList;
    static std::vector<CPUTAsset<CPUTBuffer>>           mpConstantBufferList;
    static std::vector<CPUTAsset<CPUTRenderStateBlock>> mpRenderStateBlockList;
    static std::vector<CPUTAsset<CPUTFont>>             mpFontList;
	static std::vector<CPUTAsset<CPUTAnimation>>        mpAnimationSetList;

public:
    static CPUTAssetLibrary *GetAssetLibrary();
    static void              DeleteAssetLibrary();
    void PrintAssetLibrary();

    CPUTAssetLibrary() {}
    virtual ~CPUTAssetLibrary() {}

	template<typename T>
	T* FindAsset(const std::string& name, std::vector<CPUTAsset<T>> const& pList, bool nameIsFullPathAndFilename = false);

	template <typename T>
	T* FindAssetByName(std::string const& name, std::vector<CPUTAsset<T>>& assetList);
	
	template <typename T>
	T* FindAssetByNameNoPath(std::string const& name, std::vector<CPUTAsset<T>>& assetList);

    virtual void ReleaseAllLibraryLists();

    void SetMediaDirectoryName( const std::string &directoryName )
    {
        mMediaDirectoryName = directoryName;

        SetAssetDirectoryName(mMediaDirectoryName);
    }

	void SetRootRelativeMediaDirectory(const std::string &directoryName);

    void SetAssetDirectoryName( const std::string &directoryName ) {
        mAssetDirectoryName        = directoryName;
        mAssetSetDirectoryName     = mAssetDirectoryName + "Asset/";
        mModelDirectoryName        = mAssetDirectoryName + "Asset/";
        mMaterialDirectoryName     = mAssetDirectoryName + "Material/";
        mTextureDirectoryName      = mAssetDirectoryName + "Texture/";
        mShaderDirectoryName       = mAssetDirectoryName + "Shader/";
		mAnimationSetDirectoryName = mAssetDirectoryName + "Animation/";
    }

    void SetAssetSetDirectoryName(        const std::string &directoryName) { mAssetSetDirectoryName  = directoryName; }
    void SetModelDirectoryName(           const std::string &directoryName) { mModelDirectoryName     = directoryName; }
    void SetMaterialDirectoryName(        const std::string &directoryName) { mMaterialDirectoryName  = directoryName; }
    void SetTextureDirectoryName(         const std::string &directoryName) { mTextureDirectoryName   = directoryName; }
    void SetShaderDirectoryName(          const std::string &directoryName) { mShaderDirectoryName    = directoryName; }
    void SetFontDirectoryName(            const std::string &directoryName) { mFontDirectoryName      = directoryName; }
	void SetAnimationSetDirectoryName(      const std::string &directoryName) { mAnimationSetDirectoryName = directoryName; }
    void SetAllAssetDirectoryNames(       const std::string &directoryName) {
            mAssetSetDirectoryName       = directoryName;
            mModelDirectoryName          = directoryName;
            mMaterialDirectoryName       = directoryName;
            mTextureDirectoryName        = directoryName;
            mShaderDirectoryName         = directoryName;
            mFontDirectoryName           = directoryName;
			mAnimationSetDirectoryName	 = directoryName;
    };
    void SetSystemDirectoryName(          const std::string &directoryName ) { mSystemDirectoryName   = directoryName; }

    const std::string &GetMediaDirectoryName()    { return mMediaDirectoryName; }
    const std::string &GetAssetDirectoryName()    { return mAssetDirectoryName; }
    const std::string &GetAssetSetDirectoryName() { return mAssetSetDirectoryName; }
    const std::string &GetModelDirectoryName()    { return mModelDirectoryName; }
    const std::string &GetMaterialDirectoryName() { return mMaterialDirectoryName; }
    const std::string &GetTextureDirectoryName()  { return mTextureDirectoryName; }
    const std::string &GetShaderDirectoryName()   { return mShaderDirectoryName; }
    const std::string &GetFontDirectoryName()     { return mFontDirectoryName; }
    const std::string &GetSystemDirectoryName()   { return mSystemDirectoryName; }
	const std::string &GetAnimationSetDirectoryName(){ return mAnimationSetDirectoryName; }

    void AddAssetSet(        const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTAssetSet         *pAssetSet);
    void AddNullNode(        const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTNullNode         *pNullNode);
    void AddModel(           const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTModel            *pModel);
    void AddLight(           const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTLight            *pLight);
    void AddCamera(          const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTCamera           *pCamera);
    void AddFont(            const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTFont             *pFont);
	void AddAnimationSet(    const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTAnimation	       *pAnimationSet);
    void AddRenderStateBlock(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTRenderStateBlock *pRenderStateBlock);
    void AddMaterial(        const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTMaterial *pMaterial );
    void AddTexture(         const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTTexture  *pTexture  );
    void AddBuffer(          const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTBuffer   *pBuffer   );
    void AddConstantBuffer(  const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTBuffer   *pBuffer   );

    CPUTModel     *FindModel(                   const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpModelList,            nameIsFullPathAndFilename ); }
    CPUTAssetSet  *FindAssetSet(                const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpAssetSetList,         nameIsFullPathAndFilename ); }
    CPUTTexture   *FindTexture(                 const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpTextureList,          nameIsFullPathAndFilename ); }
    CPUTNullNode  *FindNullNode(                const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpNullNodeList,         nameIsFullPathAndFilename ); }
    CPUTLight     *FindLight(                   const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpLightList,            nameIsFullPathAndFilename ); }
    CPUTCamera    *FindCamera(                  const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpCameraList,           nameIsFullPathAndFilename ); }
    CPUTFont      *FindFont(                    const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpFontList,             nameIsFullPathAndFilename ); }
	CPUTAnimation *FindAnimation(               const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpAnimationSetList,     nameIsFullPathAndFilename ); }
    CPUTRenderStateBlock *FindRenderStateBlock( const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpRenderStateBlockList, nameIsFullPathAndFilename ); }
    CPUTBuffer   *FindBuffer(                   const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpBufferList,           nameIsFullPathAndFilename ); }
    CPUTBuffer   *FindConstantBuffer(           const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpConstantBufferList,   nameIsFullPathAndFilename ); }
    CPUTMaterial *FindMaterial(                 const std::string &name, bool nameIsFullPathAndFilename=false) { return FindAsset( name, mpMaterialList,         nameIsFullPathAndFilename ); }
    
	// If the asset exists, these 'Get' methods will addref and return it. Otherwise,
	// they will return NULL.
    CPUTAssetSet         *GetAssetSetByName(         const std::string &name);
	CPUTModel            *GetModelByName(            const std::string &name);
    CPUTTexture          *GetTextureByName(          const std::string &name);
    CPUTNullNode         *GetNullNodeByName(         const std::string &name);
    CPUTLight            *GetLightByName(            const std::string &name);
    CPUTCamera           *GetCameraByName(           const std::string &name);
    CPUTFont             *GetFontByName(             const std::string &name);
	CPUTAnimation	     *GetAnimationSetByName(     const std::string &name);
    CPUTRenderStateBlock *GetRenderStateBlockByName( const std::string &name);
    CPUTBuffer           *GetBufferByName(           const std::string &name);
    CPUTBuffer           *GetConstantBufferByName(   const std::string &name);
    CPUTMaterial         *GetMaterialByName(         const std::string &name);

    // If the asset exists, these 'Get' methods will addref and return it.  Otherwise,
    // they will create it and return it.
    CPUTAssetSet         *GetAssetSet(        const std::string &name, bool nameIsFullPathAndFilename=false );
    CPUTModel            *GetModel(           const std::string &name, bool nameIsFullPathAndFilename=false );
    CPUTTexture          *GetTexture(         const std::string &name, bool nameIsFullPathAndFilename=false, bool loadAsSRGB=true );
    CPUTRenderStateBlock *GetRenderStateBlock(const std::string &name, bool nameIsFullPathAndFilename=false);
    CPUTBuffer           *GetBuffer(          const std::string &name );
    CPUTBuffer           *GetConstantBuffer(  const std::string &name );
    CPUTCamera           *GetCamera(    const std::string &name );
    CPUTLight            *GetLight(     const std::string &name );
    CPUTFont             *GetFont(      const std::string &name );
    CPUTMaterial         *GetMaterial(  const std::string &name, bool nameIsFullPathAndFilename=false );

	std::string GetMaterialPath(const std::string   &name,bool       nameIsFullPathAndFilename);
    
    CPUTAnimation	 *GetAnimation(	  const std::string &name, bool nameIsFullPathAndFilename=false );

protected:
    // helper functions
	template<typename T>
    void ReleaseList(std::vector<CPUTAsset<T>>& pLibraryRoot)
	{
		for (auto& item : pLibraryRoot) {
			item.pData->Release();
			HEAPCHECK;
		}
		pLibraryRoot.clear();
	}

template<typename T> void AddAsset(const std::string &name, const std::string &prefixDecoration, const std::string &suffixDecoration, T* pAsset, std::vector<CPUTAsset<T>>& pHead);

    UINT CPUTComputeHash( const std::string &string )
    {
        unsigned int b    = 378551;
        unsigned int a    = 63689;
        unsigned int hash = 0;
        size_t length = string.length();

        for(std::size_t i = 0; i < length; i++)
        {
            hash = hash * a + string[i];
            a    = a * b;
        }

        return hash;
    }
};
// --------------------------------------------------------------------------------------------------------------------
#include "CPUTAssetLibrary.hpp"
// --------------------------------------------------------------------------------------------------------------------
#endif //#ifndef __CPUTASSETLIBRARY_H__