/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CPUTSCENE_H
#define CPUTSCENE_H

#include "CPUTAssetSet.h"
#include "CPUTConfigBlock.h"

#include <string>
#include <float.h>

//
// Hardcode the number of assets possible in a scene to avoid any dynamic memory work. No 
// magic to the number, can be more or less.
//
const unsigned int MAX_NUM_ASSETS = 100;

class CPUTScene
{
public:
	static CPUTScene * Create();

    //
    // Destructor releases each asset set in the scene one time
    //
    ~CPUTScene()
    {
        for (UINT i = 0; i < mNumAssetSets; ++i) {
            mpAssetSetList[i]->Release();
        }
    }

    //
    // Loads the asset sets listed in the file. Calculates the bounding box/extents for the scene.
    //
    CPUTResult LoadScene(const std::string &sceneFileName);

    //
    // Adds the given asset set to the scene. Increments reference count of the asset set.
    //
    void AddAssetSet(CPUTAssetSet *pAssetSet)
    {
        mpAssetSetList[mNumAssetSets] = pAssetSet;
        mNumAssetSets++;
        pAssetSet->AddRef();

        // TODO: Calculating the bounding box can be expensive.
        // If it proves to be in the future, switch to a dirty flag, and update only on get.
        CalculateBoundingBox();
    }

    //
    // Renders each asset set in the scene by calling its renderrecursive function
    //
    void Render(CPUTRenderParameters &renderParameters, int materialIndex=0);

    //
	// Update frames
	//
	void Update(float dt);

    //
    // Gets the min/max extents for the entire scene. The extents encompass all asset sets loaded into the scene.
    //
    void GetSceneExtents(float3 *pMinExtent, float3 *pMaxExtent)
    {
        *pMinExtent = mMinExtent;
        *pMaxExtent = mMaxExtent;
    }

    //
    // Gets the bounding box for the scene as a center point and half vector. The box encompasses all asset sets loaded into the scene.
    //
    void GetBoundingBox(float3 *pCenter, float3 *pHalf)
    {
        *pCenter = mSceneBoundingBoxCenter;
        *pHalf   = mSceneBoundingBoxHalf;
    }

    //
    // Returns a pointer to the asset set at the given index. No range checking is performed.
    //
    CPUTAssetSet* GetAssetSet(unsigned int assetIndex)
    {
        ASSERT((assetIndex < mNumAssetSets), "Invalid assetIndex");

        return mpAssetSetList[assetIndex];
    }

    //
    // Returns the number of asset sets in the scene
    //
    inline unsigned int GetNumAssetSets() { return mNumAssetSets; }


protected:
    //
    // Don't allow CPUTScene to be copied because it holds pointers into the asset library
    // which don't copy properly with a shallow copy.
    //
    CPUTScene(const CPUTScene& that);
    CPUTScene& operator=(const CPUTScene& that);

    //
    // Calculates boudning box as both min/max extents and center/half vector for all objects in the scene.
    //
    void CalculateBoundingBox();

    CPUTConfigFile    mSceneFile;
    CPUTAssetSet     *mpAssetSetList[MAX_NUM_ASSETS]; // an stl::vector may be better here
    unsigned int      mNumAssetSets;
    float3            mMinExtent, mMaxExtent;
    float3            mSceneBoundingBoxCenter, mSceneBoundingBoxHalf;

private:
    CPUTScene() :
        mNumAssetSets(0),
        mMinExtent(FLT_MAX),
        mMaxExtent(-FLT_MAX),
        mSceneBoundingBoxCenter(0.0f),
        mSceneBoundingBoxHalf(0.0f)
    {
        memset( mpAssetSetList, 0, sizeof(mpAssetSetList) );
    }
};

#endif // CPUTSCENE_H
