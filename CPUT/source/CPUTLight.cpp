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
