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
#ifndef __CPUTRENDERNODE_H__
#define __CPUTRENDERNODE_H__

#include "CPUTRenderParams.h"
#include "CPUTRefCount.h"
#include "CPUTMath.h"
#include "CPUTConfigBlock.h"
#include "CPUTAnimation.h"
#include "CPUTAssetLibrary.h"
// forward declarations
class CPUTCamera;

class CPUTRenderNode : public CPUTRefCount
{
protected:

    CPUTRenderNode();
	
    std::string             mName;
    CPUTRenderNode     *mpParent;
    CPUTRenderNode     *mpChild;
    CPUTRenderNode     *mpSibling;
    bool                mWorldMatrixDirty;
    float4x4            mWorldMatrix; // transform of this object combined with it's parent(s) transform(s)
    float4x4            mParentMatrix;   // transform of this object relative to it's parent
    std::string             mPrefix;
    ~CPUTRenderNode(); // Destructor is not public.  Must release instead of delete.

    
    //Animation related variables
    CPUTNodeAnimation  *mpCurrentNodeAnimation;
    CPUTAnimation      *mpCurrentAnimation;
    float			    mAnimationTime;
	float				mAnimationStartTime;
    float				mPlaybackSpeed;
	bool				mIsLoop;
    void			 SetAnimation(CPUTAnimation *pAnimation,CPUTNodeAnimation *pNodeAnimation, float speed = 1.0f,bool enableLoop = true, float startTime = 0.0f);

	

public:

    //Animation related variables, use for computing animation offsets
    float3 mRotation;
    float3 mScale;
    float3 mPosition;
    std::vector<CPUTAnimation *> mAnimationList; // AnimationSets that are loaded as part of this node

    float4x4            mPreRotation;
    
    

    void Scale(float xx, float yy, float zz)
    {
        float4x4 scale(
              xx, 0.0f, 0.0f, 0.0f,
            0.0f,   yy, 0.0f, 0.0f,
            0.0f, 0.0f,   zz, 0.0f,
            0.0f, 0.0f, 0.0f,    1
        );
        mParentMatrix = mParentMatrix * scale;
        MarkDirty();
    }
    void Scale(float xx)
    {
        float4x4 scale(
           xx, 0.0f, 0.0f, 0.0f,
         0.0f,   xx, 0.0f, 0.0f,
         0.0f, 0.0f,   xx, 0.0f,
         0.0f, 0.0f, 0.0f,    1
        );
        mParentMatrix = mParentMatrix * scale;
        MarkDirty();
    }
    void SetPosition(float x, float y, float z)
    {
        mParentMatrix.r3.x = x;
        mParentMatrix.r3.y = y;
        mParentMatrix.r3.z = z;
        MarkDirty();
    }
    void SetPosition(const float3 &position)
    {
        mParentMatrix.r3.x = position.x;
        mParentMatrix.r3.y = position.y;
        mParentMatrix.r3.z = position.z;
        MarkDirty();
    }
    void GetPosition(float *pX, float *pY, float *pZ)
    {
        *pX = mParentMatrix.r3.x;
        *pY = mParentMatrix.r3.y;
        *pZ = mParentMatrix.r3.z;
    }
    void GetPosition(float3 *pPosition)
    {
        pPosition->x = mParentMatrix.r3.x;
        pPosition->y = mParentMatrix.r3.y;
        pPosition->z = mParentMatrix.r3.z;
    }
    float3 GetPosition()
    {
        float3 ret = float3( mParentMatrix.r3.x, mParentMatrix.r3.y, mParentMatrix.r3.z);
        return ret;
    }
    float3 GetPositionWS()
    {
        float4x4 worldMat = *GetWorldMatrix();
        return worldMat.getPosition();
    }
    float3 GetLook()
    {
        return mParentMatrix.getZAxis();
    }
    float3 GetLookWS()
    {
        float4x4 worldMat = *GetWorldMatrix();
        return worldMat.getZAxis();
    }
    float3 GetUp()
    {
        return mParentMatrix.getYAxis();
    }
    float3 GetUpWS()
    {
        float4x4 worldMat = *GetWorldMatrix();
        return worldMat.getYAxis();
    }
    float3 GetLook( float *pX, float *pY, float *pZ )
    {
        float3 look = mParentMatrix.getZAxis();
        *pX = look.x;
        *pY = look.y;
        *pZ = look.z;
    }
    float3 GetLookWS( float *pX, float *pY, float *pZ )
    {
        float4x4 worldMat = *GetWorldMatrix();
        float3 look = worldMat.getZAxis();
        *pX = look.x;
        *pY = look.y;
        *pZ = look.z;
    }

    enum CPUT_NODE_TYPE
    {
        CPUT_NODE_NULL,
        CPUT_NODE_LIGHT,
        CPUT_NODE_CAMERA,
        CPUT_NODE_MODEL,
        CPUT_NODE_GUI_ELEMENT,
        CPUT_NODE_OTHER
    };

    virtual int      ReleaseRecursive();
    void             SetName(const std::string &name) { mName = name;};
    void             SetParent(CPUTRenderNode *pParent);
    CPUTRenderNode  *GetParent()       { return mpParent; }
    CPUTRenderNode  *GetChild()        { return mpChild; }
    CPUTRenderNode  *GetSibling()      { return mpSibling; }
    CPUTRenderNode  *GetNext(CPUTRenderNode* pRoot);
    std::string         &GetName()         { return mName;};
    std::string         &GetPrefix()       { return mPrefix; }
    void             SetPrefix( std::string &prefix ) { mPrefix = prefix; }
    virtual bool     IsModel()         { return false; }
    virtual CPUT_NODE_TYPE GetNodeType()=0;
    float4x4        *GetParentMatrix() { return &mParentMatrix; }
    float4x4        *GetWorldMatrix();
    float4x4         GetParentsWorldMatrix();
    void             MarkDirty();
    void             AddChild(CPUTRenderNode *pNode);
    void             AddSibling(CPUTRenderNode *pNode);
    virtual void     Update( float deltaSeconds = 0.0f ){}
    virtual void     UpdateRecursive( float deltaSeconds );
    virtual void     Render(CPUTRenderParameters &renderParams, int materialIndex=0){}
    virtual void     RenderRecursive(CPUTRenderParameters &renderParams, int materialIndex=0);

    //Animation related functions
    void             ToggleAnimationLoop();
    void			 SetAnimationSpeed(float speed);
    void			 SetAnimation(CPUTAnimation *pAnimation, float speed = 1.0f,bool enableLoop = true, float startTime = 0.0f);

    void             SetParentMatrix(const float4x4 &parentMatrix)
    {
        mParentMatrix = parentMatrix;
        MarkDirty();
    }
    void LoadParentMatrixFromParameterBlock( CPUTConfigBlock *pBlock )
    {
        // get and set the transform
        float pMatrix[4][4];
        CPUTConfigEntry *pColumn = pBlock->GetValueByName("matrixColumn0");
        CPUTConfigEntry *pRow    = pBlock->GetValueByName("matrixRow0");
        float4x4 parentMatrix;
        if( pColumn->IsValid() )
        {
            pBlock->GetValueByName("matrixColumn0")->ValueAsFloatArray(&pMatrix[0][0], 4);
            pBlock->GetValueByName("matrixColumn1")->ValueAsFloatArray(&pMatrix[1][0], 4);
            pBlock->GetValueByName("matrixColumn2")->ValueAsFloatArray(&pMatrix[2][0], 4);
            pBlock->GetValueByName("matrixColumn3")->ValueAsFloatArray(&pMatrix[3][0], 4);
            parentMatrix = float4x4((float*)&pMatrix[0][0]);
            parentMatrix.transpose(); // Matrices are specified column-major, but consumed row-major.
        } else if( pRow->IsValid() )
        {
            pBlock->GetValueByName("matrixRow0")->ValueAsFloatArray(&pMatrix[0][0], 4);
            pBlock->GetValueByName("matrixRow1")->ValueAsFloatArray(&pMatrix[1][0], 4);
            pBlock->GetValueByName("matrixRow2")->ValueAsFloatArray(&pMatrix[2][0], 4);
            pBlock->GetValueByName("matrixRow3")->ValueAsFloatArray(&pMatrix[3][0], 4);
            parentMatrix = float4x4((float*)&pMatrix[0][0]);
        } else
        {
            float identity[16] = { 1.f, 0.f, 0.f, 0.f,   0.f, 1.f, 0.f, 0.f,   0.f, 0.f, 1.f, 0.f,    0.f, 0.f, 0.f, 1.f };
            parentMatrix = float4x4(identity);
        }

        //Initial Transform necessary for computing animation offset
        pBlock->GetValueByName("scale")->ValueAsFloatArray(&mScale.f[0], 3);
        pBlock->GetValueByName("rotation")->ValueAsFloatArray(&mRotation.f[0], 3);
        pBlock->GetValueByName("position")->ValueAsFloatArray(&mPosition.f[0], 3);

        float3 preRotation(0.0f);
        pBlock->GetValueByName("prerotation")->ValueAsFloatArray(&preRotation.f[0], 3);

        mPreRotation = float4x4RotationX(preRotation.x) * 
            float4x4RotationY(preRotation.y) * 
            float4x4RotationZ(preRotation.z);

        SetParentMatrix(parentMatrix);   // set the relative transform, marking child transform's dirty
    }
    virtual void GetBoundingBoxRecursive( float3 *pCenter, float3 *pHalf)
    {
        if(mpChild)   { mpChild->GetBoundingBoxRecursive(   pCenter, pHalf ); }
        if(mpSibling) { mpSibling->GetBoundingBoxRecursive( pCenter, pHalf ); }
    }
};

#endif // #ifndef __CPUTRENDERNODE_H__
