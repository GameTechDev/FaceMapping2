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

#include "CPUT.h"
#include "CPUTSprite.h"
#include "CPUTMesh.h"
#include "CPUTMaterial.h"
#include "CPUTInputLayoutCache.h"
#include "CPUTRenderParams.h"

CPUTSprite::CPUTSprite() :
mpMaterial(NULL),
mpMesh(NULL),
mDirty(false),
mCoordType(SpriteCoordType_Device)
{
}

CPUTSprite::~CPUTSprite()
{
    SAFE_RELEASE(mpMaterial);
    SAFE_RELEASE(mpMesh);
}

//-----------------------------------------------
CPUTSprite* CPUTSprite::Create(
    float          spriteX,
    float          spriteY,
    float          spriteWidth,
    float          spriteHeight,
    CPUTMaterial*  pMaterial
    )
{
    CPUTSprite* pCPUTSprite = new CPUTSprite();

    pCPUTSprite->mpMaterial = pMaterial;
	if(pMaterial != NULL)
		pMaterial->AddRef();

    CPUTBufferElementInfo pVertexInfo[3] = {
            { "POSITION", 0, 0, CPUT_F32, 3, 3 * sizeof(float), 0 },
            { "TEXCOORD", 0, 1, CPUT_F32, 2, 2 * sizeof(float), 3 * sizeof(float) }
    };
	pCPUTSprite->mTexCoord1 = float2(0, 0);
	pCPUTSprite->mTexCoord2 = float2(1, 1);
	SpriteVertex pVertices[6];
	pCPUTSprite->mDimensions.x = spriteWidth;
	pCPUTSprite->mDimensions.y = spriteHeight;
	pCPUTSprite->mPosition.x = spriteX;
	pCPUTSprite->mPosition.y = spriteY;
	pCPUTSprite->UpdateVertData(pVertices);
    pCPUTSprite->mpMesh = CPUTMesh::Create();
    pCPUTSprite->mpMesh->CreateNativeResources(NULL, 0, 2, pVertexInfo, 6, pVertices, NULL, 0, NULL);
    pCPUTSprite->mpMesh->SetMeshTopology(eCPUT_MESH_TOPOLOGY::CPUT_TOPOLOGY_INDEXED_TRIANGLE_LIST);
    return pCPUTSprite;
} 

void CPUTSprite::DrawSprite(CPUTRenderParameters &renderParams) 
{ 
    DrawSprite(renderParams, *mpMaterial); 
}

//-----------------------------------------
void CPUTSprite::DrawSprite(
	CPUTRenderParameters &renderParams,
	CPUTMaterial         &material
	)
{

	if ( ( mCoordType == SpriteCoordType_Screen ) && (renderParams.mWidth != mPrevScreenWidth || renderParams.mHeight != mPrevScreenHeight) )
	{
		mPrevScreenWidth = renderParams.mWidth;
		mPrevScreenHeight = renderParams.mHeight;
		mDirty = true;
	}

	if (mDirty)
	{
		UpdateMesh(renderParams);
		mDirty = false;
	}

	

    material.SetRenderStates();
    CPUTInputLayoutCache::GetInputLayoutCache()->Apply(mpMesh, &material);
    mpMesh->Draw();

}

void CPUTSprite::UpdateVertData(SpriteVertex *verts, CPUTRenderParameters *params)
{
	float2 pos = mPosition;
	float2 dim = mDimensions;
	if (params && mCoordType == SpriteCoordType_Screen)
	{
		pos.x = (pos.x * 2 / params->mWidth) - 1.0f;
		pos.y = ((pos.y * 2 / params->mHeight) - 1.0f);
		dim.x = 2.0f * dim.x / params->mWidth;
		dim.y = 2.0f * dim.y / params->mHeight;
	}

	const float top = -pos.y;
	const float bottom = -pos.y - dim.y;
	const float left = pos.x;
	const float right = pos.x + dim.x;
	SpriteVertex pVertices[] = {
			{ left, top, 1.0f, mTexCoord1.x, mTexCoord1.y },
			{ right, top, 1.0f, mTexCoord2.x, mTexCoord1.y },
			{ left, bottom, 1.0f, mTexCoord1.x, mTexCoord2.y },

			{ right, top, 1.0f, mTexCoord2.x, mTexCoord1.y },
			{ right, bottom, 1.0f, mTexCoord2.x, mTexCoord2.y },
			{ left, bottom, 1.0f, mTexCoord1.x, mTexCoord2.y }
	};
	memcpy(verts, pVertices, sizeof(SpriteVertex) * 6);
}

void CPUTSprite::UpdateMesh(CPUTRenderParameters &params)
{
	SpriteVertex verts[6];
	UpdateVertData(verts, &params);
	mpMesh->UpdateVerts(verts, sizeof(SpriteVertex) * 6);
}

void CPUTSprite::SetC(float x, float y, float width, float height)
{
	float2 pos = float2(x, y);
	float2 dim = float2(width, height);
	mDirty |= (pos != mPosition) || (dim != mDimensions);
	mPosition = float2(x, y);
	mDimensions = float2(width, height);
}

void CPUTSprite::SetTL(float x, float y, float width, float height)
{
	float2 pos = float2(x - width / 2, y - height / 2.0f);
	float2 dim = float2(width, height);
	mDirty |= (pos != mPosition) || (dim != mDimensions);
	mPosition = float2(x, y);
	mDimensions = float2(width, height);
}

void CPUTSprite::SetCoordType(SpriteCoordType type)
{
	mDirty |= (mCoordType != type);
	mCoordType = type;
	
}

void CPUTSprite::SetUV(float u1, float v1, float u2, float v2)
{
	float2 coord1 = float2(u1, v1);
	float2 coord2 = float2(u2, v2);
	mDirty |= (coord1 != mTexCoord1 || coord2 != mTexCoord2);
	mTexCoord1 = coord1;
	mTexCoord2 = coord2;
}
