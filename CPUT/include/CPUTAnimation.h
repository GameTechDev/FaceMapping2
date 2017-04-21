/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CPUTANIMATION_H__
#define __CPUTANIMATION_H__

#include <string>
#include <fstream>
#include <exception>

#include "CPUTMath.h"
#include "CPUTRefCount.h"
#include "CPUTAssetLibrary.h"
#include <vector>
#include "CPUTSkeleton.h"


enum CPUTInterpolationType
{
    CPUT_CONSTANT_INTERPOLATION	    = 0x02,
    CPUT_LINEAR_INTERPOLATION		= 0x04,
    CPUT_CUBIC_INTERPOLATION		= 0x08
};

//Supported Animated Channels
enum CPUTAnimatedProperty
{
    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,

    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,

    SCALE_X,
    SCALE_Y,
    SCALE_Z
};

//Nesting CPUTKeyFrame, CPUTAnimationCurve and CPUTAnimationLayer as their
//implementation details do not not to be known outside of CPUTNodeAnimation
class CPUTNodeAnimation: public CPUTRefCount
{

    //Keyframe: contains value for a given state of a single Animated property
    //-----------------------------------------------------------------------
    class CPUTKeyFrame
    {
    public:
        float					mValue;                 //Value of the Keyframe
        float					mTime;                  //Keyframe's sample time
        CPUTInterpolationType	mInterpolationType;     //Interpolation used with Keyframe
        float					mCubicCoefficients[4];  //Coefficients used with cubic interpolation

        void					LoadKeyFrame(CPUTFileSystem::CPUTOSifstream& file);
    };

    //AnimationCurve: Collection of KeyFrames for any given Animated property
    //-----------------------------------------------------------------------
    class CPUTAnimationCurve
    {
        CPUTKeyFrame*			mpKeyFramesList;
        CPUTAnimatedProperty	mTransformType;
        UINT					mNumberOfKeyFrames;

    public:
        ~CPUTAnimationCurve()
        {
            if(mpKeyFramesList != NULL)
            {
                delete [] mpKeyFramesList;
            }
        }
        CPUTAnimationCurve():mpKeyFramesList(NULL),
            mTransformType(TRANSLATE_X),mNumberOfKeyFrames(0){}

        UINT	GetTransformType() const;
        void	LoadAnimationCurve(CPUTFileSystem::CPUTOSifstream& file);
        float	Interpolate(float sampleTime);

    private:
        CPUTAnimationCurve(const CPUTAnimationCurve &);
        CPUTAnimationCurve & operator=(const CPUTAnimationCurve &);
    };

    //AnimationLayer: Contains collection of Animated Curves.  Multiple Layers can be
    //blended together
    //-----------------------------------------------------------------------
    class CPUTAnimationLayer
    {
    public:
        float					mWeight;	//Weight of the layer, used for blending layers
        std::string					mName;
        CPUTAnimationCurve*		mpCurvesList;
        UINT					mNumberOfCurves;

        ~CPUTAnimationLayer()
        {
            if(mpCurvesList != NULL)
            {
                delete [] mpCurvesList;
            }
        }
        CPUTAnimationLayer():mWeight(1.0),mName(""),
            mpCurvesList(NULL),mNumberOfCurves(0){}

        UINT					GetNumberOfCurves() const { return mNumberOfCurves; }
        void					LoadAnimationLayer(CPUTFileSystem::CPUTOSifstream& file);

    private:
        CPUTAnimationLayer(const CPUTAnimationLayer &);
        CPUTAnimationLayer & operator=(const CPUTAnimationLayer &);
    };

    //NodeAnimation: Per-node Animations, composed of a collection of Animation Layers
    //-----------------------------------------------------------------------
    std::string						mTarget;	//Name of the node associated with animation
    std::string						mName;
    CPUTAnimationLayer*			mpLayersList;
    UINT						mNumberOfLayers;
    float						mDuration;	//Duration of the entire Node Animation
protected:
    ~CPUTNodeAnimation()
    {
        if(mpLayersList != NULL)
        {
            delete [] mpLayersList;
        }
    }

    //Hierarchy related variables
    int mId;
    int mParentId;
    CPUTNodeAnimation *mpParent;
    CPUTNodeAnimation *mpChild;
    CPUTNodeAnimation *mpSibling;
public:
    
    CPUTNodeAnimation():mTarget(""),mName(""),mpLayersList(NULL),
        mNumberOfLayers(0),mDuration(0.0f),mpParent(NULL),mpChild(NULL),mpSibling(NULL),
    mId(0),mParentId(0){}

    std::string						GetTargetName() const;
    std::string						GetName() const;
    void						LoadNodeAnimation(int &parentIndex,CPUTFileSystem::CPUTOSifstream& file);
    float4x4                    Interpolate(float sampleTime, bool isLoop = true);
    float4x4                    Interpolate(float sampleTime, CPUTJoint &joint, bool isLoop = true );
    bool						IsValidAnimation();

    void SetParent(CPUTNodeAnimation *pParent)
    {
        SAFE_RELEASE(mpParent);
        if(NULL!=pParent)
        {
            pParent->AddRef();
        }
        mpParent = pParent;
    }
    void AddChild(CPUTNodeAnimation *pChild)
    {
        ASSERT( NULL != pChild, "Can't add NULL NodeAnimation node." );
        if( mpChild )
        {
            mpChild->AddSibling( pChild );
        }
        else
        {
            pChild->AddRef();
            mpChild = pChild;
        }
    }
    void AddSibling(CPUTNodeAnimation *pSibling)
    {
        ASSERT( NULL != pSibling, "Can't add NULL NodeAnimation node." );

        if( mpSibling )
        {
            mpSibling->AddSibling( pSibling );
        }
        else
        {
            mpSibling = pSibling;
            pSibling->AddRef();
        }
    }
    
    CPUTNodeAnimation *GetChild()
    {
        return mpChild;
    }
    CPUTNodeAnimation *GetSibling()
    {
        return mpSibling;
    }
    int ReleaseRecursive()
    {
        // Release the parent
        SAFE_RELEASE(mpParent);

        int refCount;

        // Recursively release our children and siblings
        if( mpChild )
        {
            refCount = mpChild->ReleaseRecursive();
            if( !refCount )
            {
                mpChild = NULL;
            }
        }
        if( mpSibling )
        {
            refCount = mpSibling->ReleaseRecursive();
            if( !refCount )
            {
                mpSibling = NULL;
            }
        }
        return CPUTRefCount::Release();
    }

private:
        CPUTNodeAnimation(const CPUTNodeAnimation &);
        CPUTNodeAnimation & operator=(const CPUTNodeAnimation &);
};

//AnimationSet: Contains all per-node animation for any given scene
class CPUTAnimation:public CPUTRefCount
{
    std::string mName;
    CPUTNodeAnimation *mpRootAnimation; 
    std::vector<std::vector<CPUTNodeAnimation *> > mJointAnimationList;

    ~CPUTAnimation()
    {
        if(mpRootAnimation != NULL)
        {
            mpRootAnimation->ReleaseRecursive();
        }
        for(UINT i = 0; i < mJointAnimationList.size(); ++i)
        {
            for(UINT j = 0; j < mJointAnimationList[i].size(); ++j)
            {
                mJointAnimationList[i][j]->Release();
            }
        }
    }

    CPUTAnimation(): mName(""),mpRootAnimation(NULL){}

public:

    CPUTNodeAnimation	 *GetRootAnimation() {return mpRootAnimation;}
    std::vector<CPUTNodeAnimation  *> *FindJointNodeAnimation(const std::string &name)
    {
        for(UINT i = 0; i < mJointAnimationList.size();++i)
        {
            if(mJointAnimationList[i][0]->GetTargetName() == name)
                return &mJointAnimationList[i];
        }
        return NULL;
    }
    static CPUTAnimation *Create(const std::string &file);

};

#endif // __CPUTANIMATION_H__