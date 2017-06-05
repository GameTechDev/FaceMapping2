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
#include "CPUT.h"
#include "CPUTLight.h"

// Read light properties from .set file
//-----------------------------------------------------------------------------
CPUTResult CPUTLight::LoadLight(CPUTConfigBlock *pBlock, int *pParentID)
{
    ASSERT( (NULL!=pBlock), "Invalid NULL parameter." );

    CPUTResult result = CPUT_SUCCESS;

    // set the null/group node name
    mName = pBlock->GetValueByName("name")->ValueAsString();

    // get the parent ID
    *pParentID = pBlock->GetValueByName("parent")->ValueAsInt();

    LoadParentMatrixFromParameterBlock( pBlock );

    std::string lightType = pBlock->GetValueByName("lighttype")->ValueAsString();
    if(lightType.compare("spot") == 0)
    {
        mLightParams.nLightType = CPUT_LIGHT_SPOT;
    }
    else if(lightType.compare("directional") == 0)
    {
        mLightParams.nLightType = CPUT_LIGHT_DIRECTIONAL;
    }
    else if(lightType.compare("point") == 0)
    {
        mLightParams.nLightType = CPUT_LIGHT_POINT;
    }
    else if (lightType.compare("ambient") == 0)
    {
        mLightParams.nLightType = CPUT_LIGHT_AMBIENT;
    }
    else
    {
        std::string warning = "Warning: Light: " + mName + ", has invalid light type: " + lightType;
        DEBUG_PRINT(warning.c_str());
    }
	lightType.clear();
    pBlock->GetValueByName("Color")->ValueAsFloatArray(mLightParams.pColor, 3);
    mLightParams.fIntensity    = pBlock->GetValueByName("Intensity")->ValueAsFloat();
    mLightParams.fInnerAngle      = pBlock->GetValueByName("InnerAngle")->ValueAsFloat();
    mLightParams.fOuterAngle    = pBlock->GetValueByName("ConeAngle")->ValueAsFloat();
    mLightParams.fDecayStart   = pBlock->GetValueByName("DecayStart")->ValueAsFloat();
    mLightParams.bEnableFarAttenuation = pBlock->GetValueByName("EnableNearAttenuation")->ValueAsBool();
    mLightParams.bEnableFarAttenuation = pBlock->GetValueByName("EnableFarAttenuation")->ValueAsBool();
    mLightParams.fNearAttenuationStart = pBlock->GetValueByName("NearAttenuationStart")->ValueAsFloat();
    mLightParams.fNearAttenuationEnd   = pBlock->GetValueByName("NearAttenuationEnd")->ValueAsFloat();
    mLightParams.fFarAttenuationStart  = pBlock->GetValueByName("FarAttenuationStart")->ValueAsFloat();
    mLightParams.fFarAttenuationEnd    = pBlock->GetValueByName("FarAttenuationEnd")->ValueAsFloat();
	mLightParams.bCastShadows = pBlock->GetValueByName("CastShadows")->ValueAsBool();
    return result;
}

CPUTLight * CPUTLight::Create()
{
	return new CPUTLight();
}
