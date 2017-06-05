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

#include "CPUT_DX11.h"
#include "CPUTPostProcess.h"
#include "CPUTRenderTarget.h"
#include "CPUTAssetLibrary.h"
#include "CPUTMaterial.h"
#include "CPUTSprite.h"

//-----------------------------------------
CPUTPostProcess::~CPUTPostProcess() {
    SAFE_DELETE( mpFullScreenSprite );

    SAFE_RELEASE( mpMaterialComposite );
    SAFE_RELEASE( mpMaterialBlurVertical );
    SAFE_RELEASE( mpMaterialBlurHorizontal );
    SAFE_RELEASE( mpMaterialDownSampleLogLum );
    SAFE_RELEASE( mpMaterialDownSample4x4Alpha );
    SAFE_RELEASE( mpMaterialDownSample4x4 );
    SAFE_RELEASE( mpMaterialDownSampleBackBuffer4x4 );

    SAFE_DELETE(mpRT1x1 );
    SAFE_DELETE(mpRT4x4 );
    SAFE_DELETE(mpRT64x64 );
    SAFE_DELETE(mpRTDownSample4x4PingPong );
    SAFE_DELETE(mpRTDownSample4x4 );
    // SAFE_DELETE(mpRTSourceRenderTarget ); // We don't allocate this.  Don't delete it.
}

//-----------------------------------------
void CPUTPostProcess::CreatePostProcess(
    CPUTRenderTargetColor *pSourceRenderTarget
){
    mpRTSourceRenderTarget    = pSourceRenderTarget;

    DXGI_FORMAT sourceFormat  = mpRTSourceRenderTarget->GetColorFormat();
    UINT sourceWidth          = mpRTSourceRenderTarget->GetWidth();
    UINT sourceHeight         = mpRTSourceRenderTarget->GetHeight();

    mpRTDownSample4x4         = CPUTRenderTargetColor::Create();
    mpRTDownSample4x4PingPong = CPUTRenderTargetColor::Create();
    mpRT64x64                 = CPUTRenderTargetColor::Create();
    mpRT4x4                   = CPUTRenderTargetColor::Create();
    mpRT1x1                   = CPUTRenderTargetColor::Create();

    mpRTDownSample4x4->CreateRenderTarget(         "$PostProcessDownsample4x4",         sourceWidth/4, sourceHeight/4,          sourceFormat );
    mpRTDownSample4x4PingPong->CreateRenderTarget( "$PostProcessDownsample4x4PingPong", sourceWidth/4, sourceHeight/4,          sourceFormat );
    mpRT64x64->CreateRenderTarget(                 "$PostProcessRT64x64",                          64,             64, DXGI_FORMAT_R32_FLOAT ); 
    mpRT4x4->CreateRenderTarget(                   "$PostProcessRT4x4",                             8,              8, DXGI_FORMAT_R32_FLOAT ); 
    mpRT1x1->CreateRenderTarget(                   "$PostProcessRT1x1",                             1,              1, DXGI_FORMAT_R32_FLOAT );

    CPUTAssetLibrary *pLibrary = CPUTAssetLibrary::GetAssetLibrary();
    mpMaterialDownSampleBackBuffer4x4 = pLibrary->GetMaterial("PostProcess/DownSampleBackBuffer4x4");
    mpMaterialDownSample4x4           = pLibrary->GetMaterial("PostProcess/DownSample4x4");
    mpMaterialDownSample4x4Alpha      = pLibrary->GetMaterial("PostProcess/DownSample4x4Alpha");
    mpMaterialDownSampleLogLum        = pLibrary->GetMaterial("PostProcess/DownSampleLogLum");
    mpMaterialBlurHorizontal          = pLibrary->GetMaterial("PostProcess/BlurHorizontal");
    mpMaterialBlurVertical            = pLibrary->GetMaterial("PostProcess/BlurVertical");
    mpMaterialComposite               = pLibrary->GetMaterial("PostProcess/Composite");

    CPUTMaterial* pSpriteMaterial = pLibrary->GetMaterial("Sprite");
    mpFullScreenSprite = CPUTSprite::Create( -1.0f, -1.0f, 2.0f, 2.0f, pSpriteMaterial );
    SAFE_RELEASE(pSpriteMaterial);
}

//-----------------------------------------
void CPUTPostProcess::RecreatePostProcess()
{
    UINT sourceWidth          = mpRTSourceRenderTarget->GetWidth();
    UINT sourceHeight         = mpRTSourceRenderTarget->GetHeight();

    mpRTDownSample4x4->RecreateRenderTarget(         sourceWidth/4, sourceHeight/4 );
    mpRTDownSample4x4PingPong->RecreateRenderTarget( sourceWidth/4, sourceHeight/4 );
}

//-----------------------------------------
void CPUTPostProcess::PerformPostProcess( CPUTRenderParameters &renderParams )
{
    mpRTDownSample4x4->SetRenderTarget( renderParams);
    mpFullScreenSprite->DrawSprite( renderParams, *mpMaterialDownSampleBackBuffer4x4 );
    mpRTDownSample4x4->RestoreRenderTarget( renderParams );

    // Compute average of log of luminance by downsampling log to 64x64, then 4x4, then 1x1
    mpRT64x64->SetRenderTarget(renderParams);
    mpFullScreenSprite->DrawSprite(renderParams, *mpMaterialDownSampleLogLum);
    mpRT64x64->RestoreRenderTarget( renderParams );

    mpRT4x4->SetRenderTarget(renderParams);
    mpFullScreenSprite->DrawSprite(renderParams, *mpMaterialDownSample4x4);
    mpRT4x4->RestoreRenderTarget( renderParams );

    static bool firstTime = true;

    float logLum = -2.0f;
    float pLumClearColor[] = { logLum, logLum, logLum, 1.0f };
    mpRT1x1->SetRenderTarget( renderParams, 0, 0, pLumClearColor, firstTime );
    mpFullScreenSprite->DrawSprite( renderParams, *mpMaterialDownSample4x4Alpha ); // Partially blend with previous to smooth result over time
    mpRT1x1->RestoreRenderTarget( renderParams );
    firstTime = false;

    // Better blur for bloom
    UINT ii;
    UINT numBlurs = 1; // TODO: expose as a config param
    for( ii=0; ii<numBlurs; ii++ )
    {
        mpRTDownSample4x4PingPong->SetRenderTarget(renderParams);
        mpFullScreenSprite->DrawSprite( renderParams, *mpMaterialBlurHorizontal );
        mpRTDownSample4x4PingPong->RestoreRenderTarget( renderParams );

        mpRTDownSample4x4->SetRenderTarget( renderParams);
        mpFullScreenSprite->DrawSprite( renderParams, *mpMaterialBlurVertical );
        mpRTDownSample4x4->RestoreRenderTarget( renderParams );
    }
    mpFullScreenSprite->DrawSprite(renderParams, *mpMaterialComposite);
}
