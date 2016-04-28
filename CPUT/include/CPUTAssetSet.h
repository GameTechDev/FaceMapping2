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
