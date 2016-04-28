//--------------------------------------------------------------------------------------
// Copyright 2011 Intel Corporation
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
#ifndef _CPUTSprite_H
#define _CPUTSprite_H

#include "CPUT.h"

enum SpriteCoordType
{
	SpriteCoordType_Device,
	SpriteCoordType_Screen,
};

class CPUTRenderParameters;
class CPUTMesh;
class CPUTMaterial;
class CPUTSprite
{
protected:
    class SpriteVertex
    {
    public:
        float mpPos[3];
        float mpUV[2];
    };

    CPUTMaterial      *mpMaterial;
    CPUTMesh          *mpMesh;
    CPUTSprite(); 

	bool mDirty;
	int mPrevScreenWidth;
	int mPrevScreenHeight;
	float2 mPosition;
	float2 mDimensions;
	float2 mTexCoord1;
	float2 mTexCoord2;
	SpriteCoordType mCoordType;

	void UpdateVertData(SpriteVertex *verts, CPUTRenderParameters *params = NULL);
	void UpdateMesh(CPUTRenderParameters &params);

public:
    
    ~CPUTSprite();
    void DrawSprite(CPUTRenderParameters &renderParams);
    void DrawSprite(CPUTRenderParameters &renderParams, CPUTMaterial &material);

	static CPUTSprite *Create(
		float          spriteX,
        float          spriteY,
        float          spriteWidth,
        float          spriteHeight,
        CPUTMaterial*  pMaterial
		);

	void SetC(float x, float y, float width, float height);
	void SetTL(float x, float y, float width, float height);
	void SetCoordType(SpriteCoordType type);
	void SetUV(float u1, float v1, float u2, float v2);



};

#endif 
