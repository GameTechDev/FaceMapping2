/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __CPUTASSETSET_H__
#define __CPUTASSETSET_H__

#include "CPUTRefCount.h"
#include "CPUTNullNode.h"
#include "CPUTCamera.h"

class CPUTRenderNode;
class CPUTNullNode;
class CPUTRenderParameters;

// initial size and growth defines
class CPUTAssetSet : public CPUTRefCount
{
protected:
    CPUTRenderNode **mppAssetList;
    UINT             mAssetCount;
    CPUTNullNode    *mpRootNode;
    CPUTCamera      *mpFirstCamera;
    UINT             mCameraCount;

    CPUTAssetSet();
    ~CPUTAssetSet(); // Destructor is not public.  Must release instead of delete.

public:
    static CPUTAssetSet *Create( const std::string &name, const std::string &absolutePathAndFilename, int numSystemMaterials=0, std::string *pSystemMaterialNames=NULL );

    UINT               GetAssetCount() { return mAssetCount; }
    UINT               GetCameraCount() { return mCameraCount; }
    CPUTResult         GetAssetByIndex(const UINT index, CPUTRenderNode **ppRenderNode);
	CPUTResult         GetAssetByName(const std::string &name, CPUTRenderNode **ppRenderNode);
    CPUTRenderNode    *GetRoot() { if(mpRootNode){mpRootNode->AddRef();} return mpRootNode; }
    void               SetRoot( CPUTNullNode *pRoot) { SAFE_RELEASE(mpRootNode); mpRootNode = pRoot; }
    CPUTCamera        *GetFirstCamera() { if(mpFirstCamera){mpFirstCamera->AddRef();} return mpFirstCamera; } // TODO: Consider supporting indexed access to each asset type
    void               RenderRecursive(CPUTRenderParameters &renderParams, int materialIndex=0);
    void               UpdateRecursive( float deltaSeconds );
    CPUTResult LoadAssetSet(std::string name, int numSystemMaterials=0, std::string *pSystemMaterialNames=NULL);
    void               GetBoundingBox(float3 *pCenter, float3 *pHalf);

	bool IsEnabled;
};

#endif // #ifndef __CPUTASSETSET_H__
