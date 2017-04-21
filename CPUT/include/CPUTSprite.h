/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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
