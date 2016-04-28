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
#include "CPUTAnimation.h"
#include "CPUTRenderNode.h"
using namespace std;


//Generate Animation transform
//TODO:  Move this out of CPUTNodeAnimation to support blending of multiple animations
//-----------------------------------------------------------------------
float4x4 CPUTNodeAnimation::Interpolate( float sampleTime, bool isLoop )
{
    float3 translation(0.f);
    float3 rotation(0.f);
    float3 scale(1.f);

    if(isLoop && mDuration > 0)
    {
        sampleTime -= (floor(sampleTime/mDuration) * mDuration);
    }


    //Calculate animation offset for the given sample time
    for(UINT i = 0; i < mNumberOfLayers; ++i)
    {
        CPUTAnimationLayer *currLayer = &mpLayersList[i];
        for(UINT j = 0; j < currLayer->GetNumberOfCurves(); ++j)
        {
            CPUTAnimationCurve *currCurve = &currLayer->mpCurvesList[j];
            

            //Get interpolated value of current curve
            float curveValue = currCurve->Interpolate(sampleTime);

            
            switch (currCurve->GetTransformType())
            {
            case TRANSLATE_X:
                translation.x += currLayer->mWeight*curveValue;
                break;
            case TRANSLATE_Y:
                translation.y += currLayer->mWeight*curveValue;
                break;
            case TRANSLATE_Z:
                translation.z += currLayer->mWeight*curveValue;
                break;

            case ROTATE_X:
                rotation.x += currLayer->mWeight*curveValue;
                break;
            case ROTATE_Y:
                rotation.y += currLayer->mWeight*curveValue;
                break;
            case ROTATE_Z:
                rotation.z += currLayer->mWeight*curveValue;
                break;

            case SCALE_X:
                scale.x *= currLayer->mWeight*curveValue;
                break;
            case SCALE_Y:
                scale.y *= currLayer->mWeight*curveValue;
                break;
            case SCALE_Z:
                scale.z *= currLayer->mWeight*curveValue;
                break;
            default:
                break;
            }

        }
    }

    //Compute transformation due to animation
    float4x4 xform = 
        float4x4Scale(scale) * 
        float4x4RotationX(rotation.x) * 
        float4x4RotationY(rotation.y) * 
        float4x4RotationZ(rotation.z) * 
        float4x4Translation(translation);
    return xform;
}

//Interpolate function for Skeletal animations
//TODO:  Move this out of CPUTNodeAnimation to support blending of multiple animations
//-----------------------------------------------------------------------
float4x4 CPUTNodeAnimation::Interpolate( float sampleTime, CPUTJoint &joint, bool isLoop )
{
    float3 translation(0.f);
    float3 rotation(0.f);
    float3 scale(1.f);

    if(isLoop && mDuration > 0)
    {
        sampleTime -= (floor(sampleTime/mDuration) * mDuration);
    }


    //Calculate animation offset for the given sample time
    for(UINT i = 0; i < mNumberOfLayers; ++i)
    {
        CPUTAnimationLayer *currLayer = &mpLayersList[i];
        for(UINT j = 0; j < currLayer->GetNumberOfCurves(); ++j)
        {
            CPUTAnimationCurve *currCurve = &currLayer->mpCurvesList[j];


            //Get interpolated value of current curve
            float curveValue = currCurve->Interpolate(sampleTime);


            switch (currCurve->GetTransformType())
            {
            case TRANSLATE_X:
                translation.x += currLayer->mWeight*curveValue;
                break;
            case TRANSLATE_Y:
                translation.y += currLayer->mWeight*curveValue;
                break;
            case TRANSLATE_Z:
                translation.z += currLayer->mWeight*curveValue;
                break;

            case ROTATE_X:
                rotation.x += currLayer->mWeight*curveValue;
                break;
            case ROTATE_Y:
                rotation.y += currLayer->mWeight*curveValue;
                break;
            case ROTATE_Z:
                rotation.z += currLayer->mWeight*curveValue;
                break;

            case SCALE_X:
                scale.x *= currLayer->mWeight*curveValue;
                break;
            case SCALE_Y:
                scale.y *= currLayer->mWeight*curveValue;
                break;
            case SCALE_Z:
                scale.z *= currLayer->mWeight*curveValue;
                break;
            default:
                break;
            }

        }
    }

    //Compute transformation due to animation
    float4x4 xform = 
        float4x4RotationX(rotation.x) * 
        float4x4RotationY(rotation.y) * 
        float4x4RotationZ(rotation.z) * 
        joint.mPreRotationMatrix*
        float4x4Translation(translation);

    joint.mScaleMatrix =  float4x4Scale(scale);
    return xform;
}

std::string CPUTNodeAnimation::GetTargetName() const
{
    return mTarget;
}

std::string CPUTNodeAnimation::GetName() const
{
    return mName;
}

void CPUTNodeAnimation::LoadNodeAnimation( int &parentIndex, CPUTFileSystem::CPUTOSifstream & file )
{
    std::string target,
        name;
    mNumberOfLayers	= 0;
    UINT stringLen	= 0;
    UINT nameLen	= 0;

    file.read((char*)&mDuration,		sizeof(float));
    file.read((char*)&mNumberOfLayers,	sizeof(UINT));
    file.read((char*)&stringLen,		sizeof(UINT));
    file.read((char*)&nameLen,			sizeof(UINT));

    assert(stringLen > 0);
    assert(nameLen > 0);

    target.resize(stringLen);
    name.resize(nameLen);


    mpLayersList = new CPUTAnimationLayer[mNumberOfLayers];

    file.read((char*)&target[0],		target.length() * sizeof(char));
    file.read((char*)&name[0],			name.length() * sizeof(char));

    mTarget = target;
    mName	= name;

    file.read((char*)&mId,		 sizeof(int));
    file.read((char*)&mParentId, sizeof(int));

    parentIndex = mParentId;

    for(UINT i = 0; i < mNumberOfLayers; ++i)
    {
        mpLayersList[i].LoadAnimationLayer(file);
    }
}

bool CPUTNodeAnimation::IsValidAnimation()
{
    return mNumberOfLayers > 0 && mDuration > 0.0f;
}

void CPUTNodeAnimation::CPUTAnimationCurve::LoadAnimationCurve( CPUTFileSystem::CPUTOSifstream& file )
{

    file.read((char*)&mNumberOfKeyFrames,	sizeof(UINT));
    file.read((char*)&mTransformType,		sizeof(UINT));

    mpKeyFramesList = new CPUTKeyFrame[mNumberOfKeyFrames];

    for(UINT i = 0; i < mNumberOfKeyFrames; ++i)
    {
        mpKeyFramesList[i].LoadKeyFrame(file);
    }
}

UINT CPUTNodeAnimation::CPUTAnimationCurve::GetTransformType() const
{
    return mTransformType;
}

//Interpolate KeyFrames in curve for sampleTime
float CPUTNodeAnimation::CPUTAnimationCurve::Interpolate( float sampleTime )
{
    CPUTKeyFrame	*left	= NULL, 
                    *right	= NULL;


    if(sampleTime <= mpKeyFramesList[0].mTime)
    {
        return mpKeyFramesList[0].mValue;
    }
    else if (sampleTime >= mpKeyFramesList[mNumberOfKeyFrames-1].mTime)
    {
        return mpKeyFramesList[mNumberOfKeyFrames-1].mValue;
    }


    
    //Locate keys to interpolate
    for(UINT i = 1 ; i < mNumberOfKeyFrames; ++i)
    {

        if(sampleTime > mpKeyFramesList[i - 1].mTime && sampleTime < mpKeyFramesList[i].mTime)
        {
            left = &mpKeyFramesList[i-1];
            right = &mpKeyFramesList[i];
            break;
        }
        else if(sampleTime == mpKeyFramesList[i].mTime)
        {
            return mpKeyFramesList[i].mValue;
        }

    }
    //When interpolating, always use the left keys coefficients
    float *coeff = left->mCubicCoefficients;
    
    //No need to interpolate value if interpolation set to constant
    //Always return the left key's value
    //TODO: What impact may this have on reverse playback?
    if(left->mInterpolationType == CPUT_CONSTANT_INTERPOLATION)
    {
        return left->mValue;
    }

    //Interpolate between Key frames cubicly
    float normalizeTime = (sampleTime - left->mTime);
    float timevec[4] = {pow(normalizeTime,3.0f),pow(normalizeTime,2.0f),normalizeTime,1.0};
    float val = timevec[0] * coeff[0] + timevec[1] * coeff[1] + 
        timevec[2] * coeff[2] + timevec[3] * coeff[3];
    return val;
}

void CPUTNodeAnimation::CPUTKeyFrame::LoadKeyFrame( CPUTFileSystem::CPUTOSifstream& file )
{
    file.read((char*)&mValue,					sizeof(float));
    file.read((char*)&mTime,					sizeof(float));
    file.read((char*)&mInterpolationType,		sizeof(CPUTInterpolationType));
    file.read((char*)&mCubicCoefficients[0],	4 * sizeof(float));
}

void CPUTNodeAnimation::CPUTAnimationLayer::LoadAnimationLayer( CPUTFileSystem::CPUTOSifstream& file )
{
    mNumberOfCurves = 0;
    UINT AnimationLayerNameLength	= 0;
    std::string name;

    file.read((char*)&mNumberOfCurves,	sizeof(UINT));
    file.read((char*)&AnimationLayerNameLength,		sizeof(UINT));

    //This should never be 0
    assert(AnimationLayerNameLength > 0 );

    name.resize(AnimationLayerNameLength);

    file.read((char*)&name[0],	sizeof(char)*name.length());
    file.read((char*)&mWeight,	sizeof(float));

    mName = name;

    mpCurvesList = new CPUTAnimationCurve[mNumberOfCurves];

    for(UINT i = 0; i < mNumberOfCurves; ++i)
    {
        mpCurvesList[i].LoadAnimationCurve(file);
    }
}

CPUTAnimation * CPUTAnimation::Create( const std::string &file )
{
    std::string name;
    //TODO: ws2s causing leak
    CPUTFileSystem::CPUTOSifstream stream(std::string(file.begin(), file.end()),ios::binary);

    UINT numberOfAnimations = 0;
    int animationSetStringLength = 0;

    CPUTAnimation *pAnimationSet;
	//FIXME: check is_open can be replaced by good()
    if(!stream.good())
    {
        return NULL;
    }
    CPUTNodeAnimation **mppAnimationList = NULL;

    stream.read((char*)&numberOfAnimations,			sizeof(UINT));
    stream.read((char*)&animationSetStringLength,	sizeof(UINT));

    assert(animationSetStringLength > 0 );
    assert(numberOfAnimations > 0 );

    name.resize(animationSetStringLength);
    
    stream.read((char*)&name[0], name.length() * sizeof(char));


    pAnimationSet = new CPUTAnimation;
    pAnimationSet->mName = name;

    mppAnimationList = new CPUTNodeAnimation *[numberOfAnimations];

    std::vector<CPUTNodeAnimation *> jointAnimations;
    std::vector<UINT>jointRootLocations;

    for(UINT i = 0; i < numberOfAnimations; ++i)
    {
        CPUTNodeAnimation *currentAnimation = new CPUTNodeAnimation;
        int parentID = 0;
        currentAnimation->LoadNodeAnimation(parentID, stream);

        if(parentID > -1)
        {
            CPUTNodeAnimation * parentAnimation = mppAnimationList[parentID];
            currentAnimation->SetParent(parentAnimation);
            parentAnimation->AddChild(currentAnimation);
        }
        else if(parentID == -1 && i > 0)
        {
            jointRootLocations.push_back(i);
            currentAnimation->AddRef();
        }

        mppAnimationList[i] = currentAnimation;
    }

    pAnimationSet->mpRootAnimation = mppAnimationList[0];
    mppAnimationList[0]->AddRef();

    for(UINT jointIndicies = 0; jointIndicies < jointRootLocations.size(); ++ jointIndicies)
    {
        UINT range = 0;
        if(jointIndicies + 1 < jointRootLocations.size())
        {
            range = jointRootLocations[jointIndicies+1] - jointRootLocations[jointIndicies];
        }
        else
        {
            range = numberOfAnimations - jointRootLocations[jointIndicies];
        }

        for(UINT i = 0; i < range; ++i)
        {
            jointAnimations.push_back(mppAnimationList[jointRootLocations[jointIndicies]+i]);
            mppAnimationList[jointRootLocations[jointIndicies]+i]->AddRef();
        }
        pAnimationSet->mJointAnimationList.push_back(jointAnimations);
        jointAnimations.clear();
    }

    if(mppAnimationList != NULL)
    {
        for(UINT i = 0 ; i < numberOfAnimations; ++i)
        {
            SAFE_RELEASE(mppAnimationList[i]);
        }
        SAFE_DELETE_ARRAY(mppAnimationList);
    }

    //Recursively release joint animations as hierarchy need not be retained
    //TODO:  Find a better way to load this data
    for(UINT i = 0; i < pAnimationSet->mJointAnimationList.size(); ++i)
    {
        pAnimationSet->mJointAnimationList[i][0]->ReleaseRecursive();
    }

    return pAnimationSet;
}
