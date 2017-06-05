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