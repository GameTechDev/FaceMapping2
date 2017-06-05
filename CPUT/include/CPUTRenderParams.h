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
#ifndef __CPUTRENDERPARAMS_H__
#define __CPUTRENDERPARAMS_H__

#include "CPUTMath.h"

// TODO:  Change name to CPUTRenderContext?
class CPUTCamera;
class CPUTBuffer;
class CPUTRenderParameters
{
public:
    bool         mShowBoundingBoxes;
    bool         mDrawModels;
    bool         mRenderOnlyVisibleModels;
    int          mWidth;
    int          mHeight;
    CPUTCamera  *mpCamera;
    CPUTCamera  *mpShadowCamera;
    CPUTBuffer  *mpPerModelConstants;
    CPUTBuffer  *mpPerFrameConstants;
    CPUTBuffer  *mpSkinningData;
	float4x4 mLightMatrixOverride;

    CPUTRenderParameters() :
        mShowBoundingBoxes(false),
        mDrawModels(true),
        mRenderOnlyVisibleModels(true),
        mpCamera(0),
        mpShadowCamera(0),
        mpPerModelConstants(0),
        mpPerFrameConstants(0),
        mpSkinningData(0)
    {}
    ~CPUTRenderParameters(){}
private:
};

#endif // __CPUTRENDERPARAMS_H__