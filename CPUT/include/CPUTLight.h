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
#ifndef __CPUTLight_H__
#define __CPUTLight_H__

#include "CPUT.h"
#include "CPUTRenderNode.h"
#include "CPUTConfigBlock.h"

enum LightType
{
    CPUT_LIGHT_DIRECTIONAL,
    CPUT_LIGHT_POINT,
    CPUT_LIGHT_SPOT,
    CPUT_LIGHT_AMBIENT,
};

struct CPUTLightParams
{
    LightType   nLightType;
    float       pColor[3];
    float       fIntensity;
    float       fInnerAngle;
    float       fOuterAngle;
    float       fDecayStart;
    bool        bEnableNearAttenuation;
    bool        bEnableFarAttenuation;
    float       fNearAttenuationStart;
    float       fNearAttenuationEnd;
    float       fFarAttenuationStart;
    float       fFarAttenuationEnd;
	bool		bCastShadows;
};

class CPUTLight:public CPUTRenderNode
{
protected:
    CPUTLightParams mLightParams;

    CPUTLight() {}
public:
	static CPUTLight * Create();

    virtual ~CPUTLight() {}

    CPUT_NODE_TYPE GetNodeType() { return CPUT_NODE_LIGHT;};

    void             SetLightParameters(CPUTLightParams &lightParams);
    CPUTLightParams *GetLightParameters() {return &mLightParams; }
    CPUTResult       LoadLight(CPUTConfigBlock *pBlock, int *pParentID);
};

#endif //#ifndef __CPUTLight_H__