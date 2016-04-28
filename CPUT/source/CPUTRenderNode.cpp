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
#include "CPUTRenderNode.h"

#include "CPUTOSServices.h" // for OutputDebugString();


// Constructor
//-----------------------------------------------------------------------------
CPUTRenderNode::CPUTRenderNode():
    mpParent(NULL),
    mpChild(NULL),
    mpSibling(NULL),
    mAnimationTime(0.0f),
    mPlaybackSpeed(1.0f),
    mpCurrentNodeAnimation(NULL),
    mpCurrentAnimation(NULL),
    mIsLoop(true),
    mRotation(float3(0.0f)),
    mPosition(float3(0.0f)),
    mScale(float(1.0f))
{
    // set transform to identity
    mWorldMatrix  = float4x4Identity();
    mParentMatrix = float4x4Identity();

    // always create with everything dirty
    mWorldMatrixDirty = true;
}

// Destructor
//-----------------------------------------------------------------------------
CPUTRenderNode::~CPUTRenderNode()
{
    SAFE_RELEASE(mpCurrentNodeAnimation);
    SAFE_RELEASE(mpCurrentAnimation);
    SAFE_RELEASE(mpParent);
    //TODO: should these be recusively release here?
    SAFE_RELEASE(mpChild);
    SAFE_RELEASE(mpSibling);
    for(unsigned int i = 0; i < mAnimationList.size(); i++)
    {
        SAFE_RELEASE(mAnimationList[i]);
    }
    mAnimationList.clear();
}

//-----------------------------------------------------------------------------
int CPUTRenderNode::ReleaseRecursive()
{
// #define OUTPUT_DEBUG_INFO
#ifdef OUTPUT_DEBUG_INFO
    char pRefCountString[64];
    sprintf_s( pRefCountString, 64, "(%d):", GetRefCount()-1 );
    DEBUG_PRINT( pRefCountString );
    std::string msg = GetName() + "\n";
    DEBUG_PRINT( msg.c_str() );

    if( mpParent )
    {
        DEBUG_PRINT( "Parent: " );
        sprintf_s( pRefCountString, 64, "(%d):", mpParent->GetRefCount()-1  );
        DEBUG_PRINT( pRefCountString );
        std::string msg =  (mpParent ? mpParent->GetName() : "NULL\n") + "\n";
        DEBUG_PRINT( msg.c_str() );
    }
#endif
    // Release the parent.  Note: we don't want to recursively release it, or it would release us = infinite loop.
    SAFE_RELEASE(mpParent);

    // Recursively release our children and siblings
    if( mpChild )
    {
#ifdef OUTPUT_DEBUG_INFO
        DEBUG_PRINT( "Child" ); 
#endif
        mpChild->ReleaseRecursive();
        mpChild = NULL;
    }
    if( mpSibling )
    {
#ifdef OUTPUT_DEBUG_INFO
        DEBUG_PRINT( "Sibling:" );
#endif
        mpSibling->ReleaseRecursive();
        mpSibling = NULL;
    }
    return CPUTRefCount::Release();
}

// parent/child/sibling
//-----------------------------------------------------------------------------
void CPUTRenderNode::SetParent(CPUTRenderNode *pParent)
{
    SAFE_RELEASE(mpParent);
    if(NULL!=pParent)
    {
        pParent->AddRef();
    }
    mpParent = pParent;
}

CPUTRenderNode* CPUTRenderNode::GetNext(CPUTRenderNode* pRoot)
{
    if(GetChild())
    {
        return GetChild();
    }
    CPUTRenderNode* pNode = this;
    while(pNode->GetSibling() == NULL)
    {
        pNode = pNode->GetParent();
        if(pNode == pRoot)
            return pNode;
    }
    pNode = pNode->GetSibling();
    return pNode;
}

//-----------------------------------------------------------------------------
void CPUTRenderNode::AddChild(CPUTRenderNode *pNode )
{
    ASSERT( NULL != pNode, "Can't add NULL node." );
    if( mpChild )
    {
        mpChild->AddSibling( pNode );
    }
    else
    {
        pNode->AddRef();
        mpChild = pNode;
    }
}

//-----------------------------------------------------------------------------
void CPUTRenderNode::AddSibling(CPUTRenderNode *pNode )
{
    ASSERT( NULL != pNode, "Can't add NULL node." );

    if( mpSibling )
    {
        mpSibling->AddSibling( pNode );
    }
    else
    {
        mpSibling = pNode;
        pNode->AddRef();
    }
}

// Return the model's cumulative transform
//-----------------------------------------------------------------------------
float4x4* CPUTRenderNode::GetWorldMatrix()
{
    if(mWorldMatrixDirty)
    {
        if(NULL!=mpParent)
        {
            float4x4 *pParentWorldMatrix = mpParent->GetWorldMatrix();
            mWorldMatrix = mParentMatrix * *pParentWorldMatrix;
        }
        else
        {
            mWorldMatrix = mParentMatrix;
        }
       mWorldMatrixDirty = false;
    }

    // copy it
    return &mWorldMatrix;
}

// Return the model's cumulative transform
//-----------------------------------------------------------------------------
float4x4 CPUTRenderNode::GetParentsWorldMatrix()
{
    float4x4 matrix(float4x4Identity());

    if(NULL!=mpParent)
    {
        matrix = *mpParent->GetWorldMatrix();

    }

    return matrix;
}

// Recursively visit all sub-nodes in breadth-first mode and mark their
// cumulative transforms as dirty
//-----------------------------------------------------------------------------
void CPUTRenderNode::MarkDirty()
{
    mWorldMatrixDirty = true;

    if(mpSibling)
    {
        mpSibling->MarkDirty();
    }

    if(mpChild)
    {
        mpChild->MarkDirty();
    }
}

// Update - recursively visit all sub-nodes in breadth-first mode
// Likely used for animation with a frame# or timestamp passed in
// so that the update routine would calculate the new transforms
// and called before Render() function
//-----------------------------------------------------------------------------
void CPUTRenderNode::UpdateRecursive( float deltaSeconds )
{
    if(mpCurrentNodeAnimation != NULL && mpCurrentNodeAnimation->IsValidAnimation())
    {
        SetParentMatrix(mpCurrentNodeAnimation->Interpolate(mAnimationTime,mIsLoop));
        mAnimationTime += deltaSeconds * mPlaybackSpeed;
    }

    Update(deltaSeconds);

    if(mpSibling)
    {
        mpSibling->UpdateRecursive(deltaSeconds);
    }
    if(mpChild)
    {
        mpChild->UpdateRecursive(deltaSeconds);
    }
}

// RenderRecursive - recursively visit all sub-nodes in breadth-first mode
//-----------------------------------------------------------------------------
void CPUTRenderNode::RenderRecursive(CPUTRenderParameters &renderParams, int materialIndex)
{
    Render(renderParams, materialIndex);

    if(mpChild)
    {
        mpChild->RenderRecursive(renderParams, materialIndex);
        CPUTRenderNode *pNode = mpChild->GetSibling();
        while(pNode)
        {
            pNode->RenderRecursive(renderParams, materialIndex);
            pNode = pNode->GetSibling();
        }
    }
}

//ToggleAnimationLoop - recursively Enable/Disable Animation Loop
//-----------------------------------------------------------------------------
void CPUTRenderNode::ToggleAnimationLoop()
{
    mIsLoop = !mIsLoop;

    if(mpSibling)
    {
        mpSibling->ToggleAnimationLoop();
    }
    if(mpChild)
    {
        mpChild->ToggleAnimationLoop();
    }
}

// SetAnimationSpeed - recursively set playback speed
// Values can be negative for reverse playback
//-----------------------------------------------------------------------------
void CPUTRenderNode::SetAnimationSpeed( float speed )
{
    mPlaybackSpeed = speed;

    if(mpSibling)
    {
        mpSibling->SetAnimationSpeed(speed);
    }
    if(mpChild)
    {
        mpChild->SetAnimationSpeed(speed);
    }
}

// SetAnimation - recursively apply NodeAnimations contains in pAnimation
// pAnimation:      CPUTAnimation that we want to set
// playbackSpeed:   Speed at which to play the animation; values can be negative if reverse playback is desired
// enableLoop:      Should the animation loop indefinitely
// startTime:       At what time should we start sampling the animation
//-----------------------------------------------------------------------------
void CPUTRenderNode::SetAnimation( CPUTAnimation *pAnimation, float playbackSpeed ,bool enableLoop , float startTime )
{
    if(pAnimation)
    {
        SAFE_RELEASE(mpCurrentNodeAnimation);

        mpCurrentNodeAnimation = pAnimation->GetRootAnimation();
        mpCurrentNodeAnimation->AddRef();

        SAFE_RELEASE(mpCurrentAnimation);
        mpCurrentAnimation = pAnimation;
        mpCurrentAnimation->AddRef();

		mAnimationTime = startTime;
		mIsLoop = enableLoop;
		mPlaybackSpeed = playbackSpeed;

        if(mpSibling)
        {
            mpSibling->SetAnimation(pAnimation,mpCurrentNodeAnimation->GetSibling(),playbackSpeed,enableLoop,startTime);
        }
        if(mpChild)
        {
            mpChild->SetAnimation(pAnimation,mpCurrentNodeAnimation->GetChild(),playbackSpeed,enableLoop,startTime);
        }
    }
}

// SetAnimation - recursively set NodeAnimation
// pAnimationNode:  CPUTNodeAnimation that we want to set
// playbackSpeed:   Speed at which to play the animation; values can be negative if reverse playback is desired
// enableLoop:      Should the animation loop indefinitely
// startTime:       At what time should we start sampling the animation
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPUTRenderNode::SetAnimation(CPUTAnimation *pAnimation, CPUTNodeAnimation *pAnimationNode, float playbackSpeed ,bool enableLoop , float startTime )
{
    if(pAnimationNode)
    {
        SAFE_RELEASE(mpCurrentNodeAnimation);

        mpCurrentNodeAnimation = pAnimationNode;
        mpCurrentNodeAnimation->AddRef();

        SAFE_RELEASE(mpCurrentAnimation);
        mpCurrentAnimation = pAnimation;
        mpCurrentAnimation->AddRef();

		mAnimationTime = startTime;
		mIsLoop = enableLoop;
		mPlaybackSpeed = playbackSpeed;

        if(mpSibling)
        {
            mpSibling->SetAnimation(pAnimation,pAnimationNode->GetSibling(),playbackSpeed,enableLoop,startTime);
        }
        if(mpChild)
        {
            mpChild->SetAnimation(pAnimation,pAnimationNode->GetChild(),playbackSpeed,enableLoop,startTime);
        }
    }
}

