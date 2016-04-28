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
#ifndef __CPUTSKELETON_H__
#define __CPUTSKELETON_H__

#include "CPUTMath.h"
#include "CPUTAssetLibrary.h"
#include <vector>


class CPUTJoint
{
public:
    std::string  mName;         //Joint name
    UINT     mParentIndex;  //Array Index of Parent Joint

    float4x4 mInverseBindPoseMatrix;  //This represents the inverse of the joint transforms at time of binding
    float4x4 mRTMatrix;              //Rotation and Translation matrix; propagated to children
    float4x4 mPreRotationMatrix;     //Orientation of joint, prior to binding
    float4x4 mScaleMatrix;           //Scale matrix; not propagated to children

    CPUTJoint();
    bool LoadJoint(std::ifstream& file);
};

struct CPUTSkeleton
{
    void LoadSkeleton(std::ifstream& file);
public:
    std::string mName;         //Name of Mesh associated with Skeleton
    UINT mNumberOfJoints;  //Total number of joints contained in skeleton
    std::vector<CPUTJoint> mJointsList; //List of Joints in Skeleton

    bool LoadSkeleton(std::string path);    //Uses fullpath

	static CPUTSkeleton * Create();

private:

    CPUTSkeleton();
};

#endif  //__CPUTSKELETON_H__