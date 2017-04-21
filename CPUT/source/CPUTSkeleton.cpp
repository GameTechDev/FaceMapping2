/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CPUTSkeleton.h"
#include <fstream>
#include "CPUT.h"


bool CPUTJoint::LoadJoint( std::ifstream& file )
{
    UINT stringLength(0);
    std::string jointName("");
    float3 preRotation(0.0f);

    if(file.eof())
    {
        return false;
    }

    file.read((char*)&stringLength, sizeof(UINT));

    //TODO: Used unique ID instead of name
    //Load Joint Name
    jointName.resize(stringLength);
    file.read((char*)&jointName[0], jointName.length() * sizeof(char));

    mName = jointName;

    file.read((char*)&mParentIndex, sizeof(UINT));

    file.read((char*)&mInverseBindPoseMatrix, sizeof(float) * 16);
    file.read((char*)&preRotation, sizeof(float) * 3);

    mPreRotationMatrix = float4x4RotationX(preRotation.x) * 
        float4x4RotationY(preRotation.y) * 
        float4x4RotationZ(preRotation.z);

    return true;
}

CPUTJoint::CPUTJoint() :mName(""),mParentIndex(0xff)
{
    mInverseBindPoseMatrix = float4x4Identity();
    mRTMatrix = float4x4Identity();
    mPreRotationMatrix = float4x4Identity();
    mScaleMatrix = float4x4Identity();
}


CPUTSkeleton::CPUTSkeleton() :mName(""),mNumberOfJoints(0)
{

}

void CPUTSkeleton::LoadSkeleton( std::ifstream& file )
{
    int stringLength(0);
    std::string tempName("");

    file.read((char*)&stringLength, sizeof(UINT));
    file.read((char*)&mNumberOfJoints,sizeof(UINT));

    assert(stringLength > 0);
    assert(mNumberOfJoints >= 0);

    //TODO: Used unique ID instead of name
    //Load Skeleton Name
    tempName.resize(stringLength);
    file.read((char*)&tempName[0], tempName.length() * sizeof(char));

    mName = tempName;

    mJointsList.resize(mNumberOfJoints);
    for(UINT i = 0; i < mNumberOfJoints; ++i)
    {
        bool result = mJointsList[i].LoadJoint(file);
        ASSERT(result, "Number of joints reported do not match number of joints found");
    }
}

bool CPUTSkeleton::LoadSkeleton( std::string path )
{
    std::string filename(path.begin(),path.end());
    std::ifstream binaryStream(filename, std::ios::binary);

    if(binaryStream.is_open())
    {
        LoadSkeleton(binaryStream);
        binaryStream.close();
        return true;
    }
    return false;
}

CPUTSkeleton * CPUTSkeleton::Create()
{
	return new CPUTSkeleton();
}
