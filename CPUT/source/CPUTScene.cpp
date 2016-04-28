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

#include "CPUTScene.h"
#include "CPUTAssetLibrary.h"

//-----------------------------------------------------------------------------
CPUTResult CPUTScene::LoadScene(const std::string &sceneFileName)
{
    CPUTResult result = CPUT_SUCCESS;

    result = mSceneFile.LoadFile(sceneFileName);
    if (CPUTFAILED(result)) {
        DEBUG_PRINT("Failed to load scene: %s\n", sceneFileName.data());
        return result;
    }

    CPUTConfigBlock *pAssetsBlock = mSceneFile.GetBlockByName("Assets");
    if (pAssetsBlock == NULL) {
        DEBUG_PRINT("Failed to load Assets\n");
        return CPUT_WARNING_NO_ASSETS_LOADED;
    }

    int numAssets = pAssetsBlock->ValueCount();
    if (numAssets <= 0) {
        DEBUG_PRINT("Failed to load Assets\n");
        return CPUT_WARNING_NO_ASSETS_LOADED;
    }

    CPUTAssetSet *pAssetSet = NULL;
    CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
    for (int i = 0; i < numAssets; ++i) {
         CPUTConfigEntry *pEntry = pAssetsBlock->GetValue(i);
         if (pEntry == NULL) {
             continue;
         }

         std::string resolvedAssetNameAndPath;
         CPUTFileSystem::ResolveAbsolutePathAndFilename(pAssetLibrary->GetMediaDirectoryName() + pEntry->NameAsString(), &resolvedAssetNameAndPath);

         //
         // Extract the set file name off the end of the path
         //
         const char *delimiters = "\\/";
         size_t pos = resolvedAssetNameAndPath.find_last_of(delimiters);
         if (pos == std::string::npos) {
             // then there are no directories in the path provided. There should always be at least /asset/
         }
         std::string assetFileName = resolvedAssetNameAndPath.substr(pos + 1); // +1 to skip the '/' or '\' character

         //
         // the assetname will always end in /asset/name.set
         //
         size_t pos1 = resolvedAssetNameAndPath.rfind("asset");
         size_t pos2 = resolvedAssetNameAndPath.rfind("Asset");
         if ((pos1 != std::string::npos) && (pos2 != std::string::npos)) {
             pos = std::max(pos1, pos2);
         } else {
             pos = std::min(pos1, pos2);
         }

         if (pos == std::string::npos) {
             // then the set file is not in the correct folder
         }
         std::string assetFilePath = resolvedAssetNameAndPath.substr(0, pos);

         pAssetLibrary->SetAssetDirectoryName(assetFilePath);

         pAssetSet  = pAssetLibrary->GetAssetSet(resolvedAssetNameAndPath, true); // need to state that this is the fully qualified path name so CPUT will not append a .set extension
        if (!pAssetSet)
            DEBUG_PRINT("Failed to load AssetSet\n");// %p", pAssetSet);
         ASSERT( pAssetSet, "Failed loading" + assetFilePath);
         mpAssetSetList[mNumAssetSets] = pAssetSet;
         mNumAssetSets++;

         ASSERT(mNumAssetSets <= MAX_NUM_ASSETS, "Number of Assets in scene file exceeds MAX_NUM_ASSETS");
    }

    CalculateBoundingBox();

    return result;
}

//-----------------------------------------------------------------------------
void CPUTScene::CalculateBoundingBox()
{
    mMinExtent.x = mMinExtent.y = mMinExtent.z =  FLT_MAX;
    mMaxExtent.x = mMaxExtent.y = mMaxExtent.z = -FLT_MAX;

    for (UINT i = 0; i < mNumAssetSets; ++i) {
        float3 lookAtPoint(0.0f, 0.0f, 0.0f);
        float3 half(1.0f, 1.0f, 1.0f);
        mpAssetSetList[i]->GetBoundingBox( &lookAtPoint, &half );

        mMinExtent = Min( (lookAtPoint - half), mMinExtent );
        mMaxExtent = Max( (lookAtPoint + half), mMaxExtent );
    }

    mSceneBoundingBoxCenter  = (mMaxExtent + mMinExtent) * 0.5f;
    mSceneBoundingBoxHalf    = (mMaxExtent - mMinExtent) * 0.5f;
}

//-----------------------------------------------------------------------------
void CPUTScene::Render(CPUTRenderParameters &renderParameters, int materialIndex)
{
    for (UINT i = 0; i < mNumAssetSets; ++i)
    {
        mpAssetSetList[i]->RenderRecursive(renderParameters, materialIndex);
    }
}
void CPUTScene::Update( float dt )
{
	for (UINT i = 0; i < mNumAssetSets; ++i)
	{
		mpAssetSetList[i]->UpdateRecursive(dt);
	}
}

CPUTScene * CPUTScene::Create()
{
	return new CPUTScene();
}
