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
#ifndef __CPUTMODEL_H__
#define __CPUTMODEL_H__

// Define the following to support drawing bounding boxes.
// Note that you also need bounding-box materials and shaders.  TODO: Include them in the exe.
//#define SUPPORT_DRAWING_BOUNDING_BOXES 1

#include "CPUTRenderNode.h"
class CPUTMaterial;
class CPUTMesh;
class CPUTConfigBlock;
struct CPUTSkeleton;

typedef bool (*DrawModelCallBackFunc)(CPUTModel*, CPUTRenderParameters &renderParams, CPUTMesh*, CPUTMaterial*, CPUTMaterial* pMaterial, void* );

//-----------------------------------------------------------------------------
class CPUTModel : public CPUTRenderNode
{
public:  
    CPUTMesh      **mpMesh;
    int           *mpMaterialCount;
	CPUTMaterial ***mpMaterial;

    int             mMeshCount;
    bool            mIsRenderable;
    float3          mBoundingBoxCenterObjectSpace;
    float3          mBoundingBoxHalfObjectSpace;
    float3          mBoundingBoxCenterWorldSpace;
    float3          mBoundingBoxHalfWorldSpace;
	float4			mUserData1;
	float4			mUserData2;
	float4			mUserData3;
	static DrawModelCallBackFunc mDrawModelCallBackFunc;

    CPUTSkeleton *mSkeleton;
    CPUTModel():
        mMeshCount(0),
        mpMaterialCount(NULL),
		mpMaterial(NULL),
        mpMesh(NULL),
        mIsRenderable(true),
        mBoundingBoxCenterObjectSpace(0.0f),
        mBoundingBoxHalfObjectSpace(0.0f),
        mBoundingBoxCenterWorldSpace(0.0f),
        mBoundingBoxHalfWorldSpace(0.0f),
        mSkeleton(NULL)
    {}

public:
    static CPUTModel* Create();
    virtual ~CPUTModel();
    void UpdateShaderConstants(CPUTRenderParameters &renderParams);

    CPUTMesh* GetMesh(const int index) const
    {
        return index < mMeshCount ? mpMesh[index] : NULL;
    }
	void SetMeshes(CPUTMesh **meshes, int meshCount);
    void Render(CPUTRenderParameters &renderParams, int materialIndex);
	void RenderMaterialOverride(CPUTRenderParameters &renderParams, CPUTMaterial *material, int meshIndex =-1, bool culling = true );
	void RenderMeshOverride(CPUTRenderParameters &renderParams, CPUTMesh *mesh, int meshIndex, int matIndex);
	void RenderMeshCustom(CPUTRenderParameters &renderParams, CPUTMesh *mesh, CPUTMaterial *material);

	static			   void SetDrawModelCallBack(DrawModelCallBackFunc Func){mDrawModelCallBackFunc = Func;}

    virtual void       UpdateRecursive(float deltaSeconds);
    bool               IsRenderable() { return mIsRenderable; }
    void               SetRenderable(bool isRenderable) { mIsRenderable = isRenderable; }
    virtual bool       IsModel() { return true; }
    CPUT_NODE_TYPE     GetNodeType() { return CPUT_NODE_MODEL;};

    void               GetBoundsObjectSpace(float3 *pCenter, float3 *pHalf);
    void               GetBoundsWorldSpace(float3 *pCenter, float3 *pHalf);
    void               UpdateBoundsWorldSpace();
    int                GetMeshCount() const { return mMeshCount; }
    //CPUTMesh          *GetMesh( UINT ii ) { return mpMesh[ii]; }
    virtual CPUTResult LoadModel(CPUTConfigBlock *pBlock, int *pParentID, CPUTModel *pMasterModel=NULL, int numSystemMaterials=0, std::string *pSystemMaterialNames=NULL);
    CPUTResult         LoadModelPayload(const std::string &File);

	void CloneFrom(CPUTModel *srcModel) { assert(false); } // TODO: FACESCAN2

    virtual void       SetMaterial(UINT ii, CPUTMaterial **pMaterial, int numEffects);
    UINT GetMaterialIndex(int meshIndex, int index);
    CPUTMaterial* GetMaterial(int meshIndex, int materialIndex);
	UINT GetMaterialCount(int meshIndex) { return mpMaterialCount[meshIndex]; }
        
    void GetBoundingBoxRecursive( float3 *pCenter, float3 *pHalf)
    {
        if( *pHalf == float3(0.0f) )
        {
            *pCenter = mBoundingBoxCenterWorldSpace;
            *pHalf   = mBoundingBoxHalfWorldSpace;
        }
        else
        {
            float3 minExtent = *pCenter - *pHalf;
            float3 maxExtent = *pCenter + *pHalf;
            float3 minDistance = mBoundingBoxCenterWorldSpace - mBoundingBoxHalfWorldSpace;
            float3 maxDistance = mBoundingBoxCenterWorldSpace + mBoundingBoxHalfWorldSpace;
            minExtent = Min( minDistance, minExtent );
            maxExtent = Max( maxDistance, maxExtent );
            *pCenter = (minExtent + maxExtent) * 0.5f;
            *pHalf   = (maxExtent - minExtent) * 0.5f;
        }
        if(mpChild)   { mpChild->GetBoundingBoxRecursive(   pCenter, pHalf ); }
        if(mpSibling) { mpSibling->GetBoundingBoxRecursive( pCenter, pHalf ); }
    }
static bool DrawModelCallBack(CPUTModel*, CPUTRenderParameters &renderParams, CPUTMesh* pMesh, CPUTMaterial* pMaterial, CPUTMaterial* pOriginalMaterial, void* pInputLayout);
};
#endif // __CPUTMODEL_H__
