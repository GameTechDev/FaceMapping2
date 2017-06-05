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
#include <algorithm> // for std::string.transform()
#include "CPUTAssetLibrary.h"
#include "CPUTRenderNode.h"
#include "CPUTAssetSet.h"
#include "CPUTTexture.h"
#include "CPUTMaterial.h"
#include "CPUTRenderStateBlock.h"
#include "CPUTModel.h"
#include "CPUTCamera.h"
#include "CPUTLight.h"
#include "CPUTFont.h"
#include "CPUTBuffer.h"
#include "CPUTAnimation.h"

#define LIBRARY_ASSERT(a, b) ASSERT(a, b)

CPUTAssetLibrary* CPUTAssetLibrary::mpAssetLibrary = nullptr;
std::vector<CPUTAsset<CPUTAssetSet>> CPUTAssetLibrary::mpAssetSetList;
std::vector<CPUTAsset<CPUTNullNode>> CPUTAssetLibrary::mpNullNodeList;
std::vector<CPUTAsset<CPUTModel>> CPUTAssetLibrary::mpModelList;
std::vector<CPUTAsset<CPUTCamera>> CPUTAssetLibrary::mpCameraList;
std::vector<CPUTAsset<CPUTLight>> CPUTAssetLibrary::mpLightList;
std::vector<CPUTAsset<CPUTMaterial>> CPUTAssetLibrary::mpMaterialList;
std::vector<CPUTAsset<CPUTTexture>> CPUTAssetLibrary::mpTextureList;
std::vector<CPUTAsset<CPUTBuffer>> CPUTAssetLibrary::mpBufferList;
std::vector<CPUTAsset<CPUTBuffer>> CPUTAssetLibrary::mpConstantBufferList;
std::vector<CPUTAsset<CPUTRenderStateBlock>> CPUTAssetLibrary::mpRenderStateBlockList;
std::vector<CPUTAsset<CPUTFont>> CPUTAssetLibrary::mpFontList;
std::vector<CPUTAsset<CPUTAnimation>> CPUTAssetLibrary::mpAnimationSetList;
//-----------------------------------------------------------------------------
void CPUTAssetLibrary::DeleteAssetLibrary()
{
    SAFE_DELETE(mpAssetLibrary);
}

//-----------------------------------------------------------------------------
void CPUTAssetLibrary::ReleaseAllLibraryLists()
{
    // Release philosophy:  Everyone that references releases.  If node refers to parent, then it should release parent, etc...
    // TODO: Traverse lists.  Print names and ref counts (as debug aid)
#undef SAFE_RELEASE_LIST
#define SAFE_RELEASE_LIST(x) {ReleaseList(x); x.clear();}

    SAFE_RELEASE_LIST(mpAssetSetList);
    SAFE_RELEASE_LIST(mpMaterialList);
    SAFE_RELEASE_LIST(mpModelList);
    SAFE_RELEASE_LIST(mpLightList);
    SAFE_RELEASE_LIST(mpCameraList);
    SAFE_RELEASE_LIST(mpNullNodeList);
    SAFE_RELEASE_LIST(mpTextureList );
    SAFE_RELEASE_LIST(mpBufferList );
    SAFE_RELEASE_LIST(mpConstantBufferList );
    SAFE_RELEASE_LIST(mpRenderStateBlockList );
    SAFE_RELEASE_LIST(mpFontList);
	SAFE_RELEASE_LIST(mpAnimationSetList);

    // The following -specific items are destroyed in the derived class
    // TODO.  Move their declaration and definition to the derived class too
    // SAFE_RELEASE_LIST(mpPixelShaderList);
    // SAFE_RELEASE_LIST(mpVertexShaderList);
    // SAFE_RELEASE_LIST(mpGeometryShaderList);
}

void CPUTAssetLibrary::AddAssetSet(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTAssetSet *pAssetSet)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pAssetSet, mpAssetSetList);
}

void CPUTAssetLibrary::AddNullNode(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTNullNode *pNullNode)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pNullNode, mpNullNodeList);
}

void CPUTAssetLibrary::AddModel(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTModel *pModel)
    {
	AddAsset(name, prefixDecoration, suffixDecoration, pModel, mpModelList);
    }

void CPUTAssetLibrary::AddLight(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTLight *pLight)
    {
	AddAsset(name, prefixDecoration, suffixDecoration, pLight, mpLightList);
    }

void CPUTAssetLibrary::AddCamera(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTCamera *pCamera)
    {
	AddAsset(name, prefixDecoration, suffixDecoration, pCamera, mpCameraList);
}

void CPUTAssetLibrary::AddFont(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTFont *pFont)
    {
	AddAsset(name, prefixDecoration, suffixDecoration, pFont, mpFontList);
    }

void CPUTAssetLibrary::AddAnimationSet(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTAnimation *pAnimationSet)
    {
	AddAsset(name, prefixDecoration, suffixDecoration, pAnimationSet, mpAnimationSetList);
    }

void CPUTAssetLibrary::AddRenderStateBlock(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTRenderStateBlock *pRenderStateBlock)
    {
	AddAsset(name, prefixDecoration, suffixDecoration, pRenderStateBlock, mpRenderStateBlockList);
        }

void CPUTAssetLibrary::AddMaterial(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTMaterial *pMaterial)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pMaterial, mpMaterialList);
    }

void CPUTAssetLibrary::AddTexture(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTTexture *pTexture)
    {
	AddAsset(name, prefixDecoration, suffixDecoration, pTexture, mpTextureList);
        }

void CPUTAssetLibrary::AddBuffer(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTBuffer *pBuffer)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pBuffer, mpBufferList);
    }

void CPUTAssetLibrary::AddConstantBuffer(const std::string &name, const std::string prefixDecoration, const std::string suffixDecoration, CPUTBuffer *pBuffer)
{
	AddAsset(name, prefixDecoration, suffixDecoration, pBuffer, mpConstantBufferList);
	}

template<typename T>
void PrintAssetList(std::string const& listName, std::vector<CPUTAsset<T>>& assetList) {
	DEBUG_PRINT("\n%s:\n", listName.c_str());
	for (auto& currentItem : assetList) {
		DEBUG_PRINT("   %s", currentItem.name.c_str());
		DEBUG_PRINT("   %s", currentItem.fileName.c_str());
        DEBUG_PRINT("\n");
	}
    }

void CPUTAssetLibrary::PrintAssetLibrary()
{
	PrintAssetList("AssetList", mpAssetSetList);

	PrintAssetList("NullNodeList", mpNullNodeList);
	PrintAssetList("ModelList", mpModelList);
	PrintAssetList("CameraList", mpCameraList);
	PrintAssetList("LightList", mpLightList);
	PrintAssetList("MaterialList", mpMaterialList);
	PrintAssetList("TextureList", mpTextureList);
	PrintAssetList("BufferList", mpBufferList);
	PrintAssetList("ConstantBufferList", mpConstantBufferList);
	PrintAssetList("RenderStateBlockList", mpRenderStateBlockList);
	PrintAssetList("FontList", mpFontList);
	PrintAssetList("AnimationSetList", mpAnimationSetList);

    DEBUG_PRINT("\nEND\n");
}


void CPUTAssetLibrary::SetRootRelativeMediaDirectory(const std::string &directoryName)
{
	std::string mediaDir;
	CPUTFileSystem::GetMediaDirectory(&mediaDir);
	CPUTFileSystem::CombinePath(mediaDir, directoryName, &mediaDir );
	char lastChar = mediaDir[mediaDir.size() - 1];
	if (lastChar != '\\' || lastChar != '/')
	{
		mediaDir.append("\\");
	}
	SetMediaDirectoryName(mediaDir);
}

//-----------------------------------------------------------------------------
CPUTRenderStateBlock *CPUTAssetLibrary::GetRenderStateBlock(const std::string &name, bool nameIsFullPathAndFilename )
{
    // Resolve name to absolute path before searching
    std::string finalName;

    if( name.at(0) == '%' )
    {
#ifndef CPUT_OS_WINDOWS
        finalName = mSystemDirectoryName + "Shader/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
#else
        finalName = mSystemDirectoryName + "\\Shader\\" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
#endif
        CPUTFileSystem::ResolveAbsolutePathAndFilename(finalName, &finalName);
    } else if( name.at(0) == '$' )
    {
        finalName = name;
    } else
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mShaderDirectoryName + name), &finalName);
    }

    // see if the render state block is already in the library
    CPUTRenderStateBlock *pRenderStateBlock = FindRenderStateBlock(finalName, true);
    if(NULL==pRenderStateBlock)
    {
        CPUTRenderStateBlock *pBlock = CPUTRenderStateBlock::Create( name, finalName );
        AddRenderStateBlock(finalName, "", "", pBlock);
        return pBlock;
    }
    pRenderStateBlock->AddRef();
    return pRenderStateBlock;
}

//-----------------------------------------------------------------------------
CPUTAssetSet *CPUTAssetLibrary::GetAssetSet( const std::string &name, bool nameIsFullPathAndFilename)
{
    // Resolve the absolute path
    std::string absolutePathAndFilename;
    CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename ? name
        : (mAssetSetDirectoryName + name + ".set"), &absolutePathAndFilename );
    absolutePathAndFilename = nameIsFullPathAndFilename ? name : absolutePathAndFilename;

    CPUTAssetSet *pAssetSet = FindAssetSet(absolutePathAndFilename, true);
    if(NULL == pAssetSet)
    {
        return CPUTAssetSet::Create(name, absolutePathAndFilename);
    }
    pAssetSet->AddRef();
    return pAssetSet;
}

// TODO: All of these Get() functions look very similar.
// Keep them all for their interface, but have them call a common function
//-----------------------------------------------------------------------------
CPUTMaterial *CPUTAssetLibrary::GetMaterial(
    const std::string   &name,
    bool                 nameIsFullPathAndFilename
){
    // Resolve name to absolute path before searching
    std::string absolutePathAndFilename;
    if (name[0] == '%')
    {
        absolutePathAndFilename = mSystemDirectoryName + "Material/" + name.substr(1) + ".mtl";  // TODO: Instead of having the Material/directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        CPUTFileSystem::ResolveAbsolutePathAndFilename(absolutePathAndFilename, &absolutePathAndFilename);
    } else if( !nameIsFullPathAndFilename )
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( mMaterialDirectoryName + name + ".mtl", &absolutePathAndFilename);
    } else
    {
        absolutePathAndFilename = name;
    }

    CPUTMaterial *pMaterial = NULL;

    pMaterial = FindMaterial(absolutePathAndFilename, true);

    if( pMaterial )
    {
        pMaterial->AddRef();
    }
    else
    {
        pMaterial = CPUTMaterial::Create(absolutePathAndFilename);
        LIBRARY_ASSERT(pMaterial, "Failed creating material Effect.");
        if (pMaterial != NULL)
        {
            AddMaterial(absolutePathAndFilename, "", "", pMaterial);
        }
    }
    return pMaterial;
}

// Get CPUTModel from asset library
// If the model exists, then the existing model is Addref'ed and returned
//-----------------------------------------------------------------------------
CPUTModel *CPUTAssetLibrary::GetModel(const std::string &name, bool nameIsFullPathAndFilename)
{
    // Resolve name to absolute path before searching
    std::string absolutePathAndFilename;

    if (!nameIsFullPathAndFilename && name.at(0) == '%')
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename(mSystemDirectoryName + "Asset/" + name.substr(1) + ".mdl", &absolutePathAndFilename);
    } else if (!nameIsFullPathAndFilename) {
        CPUTFileSystem::ResolveAbsolutePathAndFilename(mModelDirectoryName + name + ".mdl", &absolutePathAndFilename);
    } else {
        CPUTFileSystem::ResolveAbsolutePathAndFilename(name, &absolutePathAndFilename);
    }

    // If we already have one by this name, then return it
    CPUTModel *pModel = FindModel(absolutePathAndFilename, true);
    if(NULL!=pModel)
    {
        pModel->AddRef();
        return pModel;
    }

    return pModel;
}

//-----------------------------------------------------------------------------
CPUTCamera *CPUTAssetLibrary::GetCamera(const std::string &name)
{
    // TODO: Should we prefix camera names with a path anyway?  To keep them unique?
    // If we already have one by this name, then return it
    CPUTCamera *pCamera = FindCamera(name, true);
    if(NULL!=pCamera)
    {
        pCamera->AddRef();
        return pCamera;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
CPUTLight *CPUTAssetLibrary::GetLight(const std::string &name)
{
    // If we already have one by this name, then return it
    CPUTLight *pLight = FindLight(name, true);
    if(NULL!=pLight)
    {
        pLight->AddRef();
        return pLight;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
CPUTTexture *CPUTAssetLibrary::GetTexture(const std::string &name, bool nameIsFullPathAndFilename, bool loadAsSRGB )
{
    std::string finalName;

    if( name.at(0) == '%' )
    {
#ifndef CPUT_OS_WINDOWS
        finalName = mSystemDirectoryName + "Texture/" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
#else
        finalName = mSystemDirectoryName + "\\Texture\\" + name.substr(1);  // TODO: Instead of having the Shader/ directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
#endif
        CPUTFileSystem::ResolveAbsolutePathAndFilename(finalName, &finalName);
    } else if( name.at(0) == '$' )
    {
        finalName = name;
    } else
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name : (mTextureDirectoryName + name), &finalName);
    }
    // If we already have one by this name, then return it
    CPUTTexture *pTexture = FindTexture(finalName, true);
    if(NULL==pTexture)
    {
        CPUTTexture *pTex = CPUTTexture::Create( name, finalName, loadAsSRGB);
        if (pTex)
        {
        AddTexture(finalName, "", "", pTex);
        return pTex;
    }
        return NULL;
    }
    pTexture->AddRef();
    return pTexture;
}

//-----------------------------------------------------------------------------
CPUTBuffer *CPUTAssetLibrary::GetBuffer(const std::string &name)
{
    // If we already have one by this name, then return it
    CPUTBuffer *pBuffer = FindBuffer(name, true);
    LIBRARY_ASSERT(pBuffer, "Can't find buffer " + name);
    if (pBuffer != NULL)
    pBuffer->AddRef();
    return pBuffer;
}

//-----------------------------------------------------------------------------
CPUTBuffer *CPUTAssetLibrary::GetConstantBuffer(const std::string &name)
{
    // If we already have one by this name, then return it
    CPUTBuffer *pBuffer = FindConstantBuffer(name, true);
    LIBRARY_ASSERT(pBuffer, "Can't find constant buffer " + name);
    if(pBuffer != NULL)
    pBuffer->AddRef();
    return pBuffer;
}

//-----------------------------------------------------------------------------
CPUTFont *CPUTAssetLibrary::GetFont(const std::string &name )
{
    // Resolve name to absolute path
    std::string absolutePathAndFilename;
    CPUTFileSystem::ResolveAbsolutePathAndFilename( (mFontDirectoryName + name), &absolutePathAndFilename);

    // If we already have one by this name, then return it
    CPUTFont *pFont = FindFont(absolutePathAndFilename, true);
    if(NULL==pFont)
    {
        CPUTFont *pFont = CPUTFont::Create( name, absolutePathAndFilename);
        LIBRARY_ASSERT(pFont, "Could not create font " + absolutePathAndFilename);
        if (pFont != NULL)
        AddFont(name, "", "", pFont);
        return pFont; 
    }
    pFont->AddRef();
    return pFont;
}

//-----------------------------------------------------------------------------
CPUTAnimation * CPUTAssetLibrary::GetAnimation( const std::string &name, bool nameIsFullPathAndFilename )
{
	std::string animationFileName;

	CPUTFileSystem::ResolveAbsolutePathAndFilename( nameIsFullPathAndFilename? name + ".anm" : (mAnimationSetDirectoryName + name + ".anm"), &animationFileName);

	// If we already have one by this name, then return it
	CPUTAnimation *pAnimation = FindAnimation(animationFileName,true);
	if(pAnimation == NULL)
	{
        CPUTAnimation *pAnimation = CPUTAnimation::Create(animationFileName);
        LIBRARY_ASSERT(pAnimation, "Could not create animation " + animationFileName);
        if (pAnimation != NULL)
        AddAnimationSet(animationFileName, "", "", pAnimation);
		return pAnimation;
	}
	pAnimation->AddRef();
	return pAnimation;
}

CPUTAssetSet *CPUTAssetLibrary::GetAssetSetByName(const std::string &name)
{
    CPUTAssetSet *pAssetSet = (CPUTAssetSet *) FindAssetByName(name, mpAssetSetList);
    if (pAssetSet != NULL)
    {
        pAssetSet->AddRef();
        return pAssetSet;
    }

    return NULL;
}

CPUTModel *CPUTAssetLibrary::GetModelByName(const std::string &name)
{
    CPUTModel *pModel = (CPUTModel *) FindAssetByName(name, mpModelList);
    if (pModel != NULL)
    {
        pModel->AddRef();
        return pModel;
    }

    return NULL;
}

CPUTTexture *CPUTAssetLibrary::GetTextureByName(const std::string &name)
{
    CPUTTexture *pTexture = (CPUTTexture *) FindAssetByName(name, mpTextureList);
    if (pTexture != NULL)
    {
        pTexture->AddRef();
        return pTexture;
    }

    return NULL;
}

CPUTNullNode *CPUTAssetLibrary::GetNullNodeByName(const std::string &name)
{
    CPUTNullNode *pNullNode = (CPUTNullNode *) FindAssetByName(name, mpNullNodeList);
    if (pNullNode != NULL)
    {
        pNullNode->AddRef();
        return pNullNode;
    }

    return NULL;
}

CPUTLight *CPUTAssetLibrary::GetLightByName(const std::string &name)
{
    CPUTLight *pLight = (CPUTLight *) FindAssetByName(name, mpLightList);
    if (pLight != NULL)
    {
        pLight->AddRef();
        return pLight;
    }

    return NULL;
}

CPUTCamera *CPUTAssetLibrary::GetCameraByName(const std::string &name)
{
    CPUTCamera *pCamera = (CPUTCamera *) FindAssetByName(name, mpCameraList);
    if (pCamera != NULL)
    {
        pCamera->AddRef();
        return pCamera;
    }

    return NULL;
}

CPUTFont *CPUTAssetLibrary::GetFontByName(const std::string &name)
{
    CPUTFont *pFont = (CPUTFont *) FindAssetByName(name, mpFontList);
    if (pFont != NULL)
    {
        pFont->AddRef();
        return pFont;
    }

    return NULL;
}

CPUTAnimation *CPUTAssetLibrary::GetAnimationSetByName(const std::string &name)
{
    CPUTAnimation *pAnimation = (CPUTAnimation *) FindAssetByName(name, mpAnimationSetList);
    if (pAnimation != NULL)
    {
        pAnimation->AddRef();
        return pAnimation;
    }

    return NULL;
}

CPUTRenderStateBlock *CPUTAssetLibrary::GetRenderStateBlockByName(const std::string &name)
{
    CPUTRenderStateBlock *pRenderStateBlock = (CPUTRenderStateBlock *) FindAssetByName(name, mpRenderStateBlockList);
    if (pRenderStateBlock != NULL)
    {
        pRenderStateBlock->AddRef();
        return pRenderStateBlock;
    }

    return NULL;
}

CPUTBuffer *CPUTAssetLibrary::GetBufferByName(const std::string &name)
{
    CPUTBuffer *pBuffer = (CPUTBuffer *) FindAssetByName(name, mpBufferList);
    if (pBuffer != NULL)
    {
        pBuffer->AddRef();
        return pBuffer;
    }

    return NULL;
}

CPUTBuffer *CPUTAssetLibrary::GetConstantBufferByName(const std::string &name)
{
    CPUTBuffer *pConstantBuffer = (CPUTBuffer *) FindAssetByName(name, mpConstantBufferList);
    if (pConstantBuffer != NULL)
    {
        pConstantBuffer->AddRef();
        return pConstantBuffer;
    }

    return NULL;
}

CPUTMaterial *CPUTAssetLibrary::GetMaterialByName(const std::string &name)
{
    CPUTMaterial *pMaterial = (CPUTMaterial *)FindAssetByName(name, mpMaterialList);
    if (pMaterial != NULL)
    {
        pMaterial->AddRef();
        return pMaterial;
    }

    return NULL;
}

std::string CPUTAssetLibrary::GetMaterialPath(const std::string   &name,bool       nameIsFullPathAndFilename)
{
    // Resolve name to absolute path before searching
    std::string absolutePathAndFilename;
    if (name[0] == '%')
    {
        absolutePathAndFilename = mSystemDirectoryName + "Material/" + name.substr(1) + ".mtl";  // TODO: Instead of having the Material/directory hardcoded here it could be set like the normal material directory. But then there would need to be a bunch new variables like SetSystemMaterialDirectory
        CPUTFileSystem::ResolveAbsolutePathAndFilename(absolutePathAndFilename, &absolutePathAndFilename);
    } else if( !nameIsFullPathAndFilename )
    {
        CPUTFileSystem::ResolveAbsolutePathAndFilename( mMaterialDirectoryName + name + ".mtl", &absolutePathAndFilename);
    } else
    {
        absolutePathAndFilename = name;
    }
	return absolutePathAndFilename;
}
