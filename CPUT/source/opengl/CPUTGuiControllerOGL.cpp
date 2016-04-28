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
#include "CPUTGuiControllerOGL.h"
#include "CPUTAssetLibraryOGL.h"
#include "CPUTBufferOGL.h"
#include "CPUTMaterial.h"
#include "CPUTMeshOGL.h"
#include "CPUTRenderParams.h"

CPUTGuiControllerOGL* CPUTGuiControllerOGL::mguiController = NULL;

// chained constructor
//--------------------------------------------------------------------------------
CPUTGuiControllerOGL::CPUTGuiControllerOGL():
    mpMaterial(NULL),
    mpConstantBufferVS(NULL),
    mpUberBuffer(NULL)
{
}

// destructor
//--------------------------------------------------------------------------------
CPUTGuiControllerOGL::~CPUTGuiControllerOGL()
{
    SAFE_RELEASE(mpMaterial);
    SAFE_RELEASE(mpConstantBufferVS);
    SAFE_RELEASE(mpUberBuffer);
    DeleteAllControls();
}

// static getter
//--------------------------------------------------------------------------------
CPUTGuiControllerOGL* CPUTGuiControllerOGL::GetController()
{
    if(NULL==mguiController)
    {
        mguiController = new CPUTGuiControllerOGL();
    }
    return mguiController;
}


// Delete the controller
//--------------------------------------------------------------------------------
CPUTResult CPUTGuiControllerOGL::DeleteController()
{
    SAFE_DELETE(mguiController);

    return CPUT_SUCCESS;
}

// Load and register all the resources needed by the GUI system
//-----------------------------------------------------------------------------
CPUTResult CPUTGuiControllerOGL::Initialize(const std::string& materialName, const std::string& fontName)
{
    CPUTResult result = CPUT_SUCCESS;

	std::string name = "$cbGUIValues";
    CPUTBufferDesc desc;
    desc.cpuAccess = BUFFER_CPU_WRITE;
    desc.memory = BUFFER_DYNAMIC;
    desc.target = BUFFER_UNIFORM;
    desc.pData = NULL;
    desc.sizeBytes = sizeof(GUIConstantBufferVS);
    mpConstantBufferVS = CPUTBuffer::Create(name, &desc);
   
    CPUTAssetLibraryOGL *pAssetLibrary = NULL;
    pAssetLibrary = (CPUTAssetLibraryOGL*)CPUTAssetLibraryOGL::GetAssetLibrary();
    pAssetLibrary->AddConstantBuffer("", name, "", mpConstantBufferVS);

    mpMaterial = pAssetLibrary->GetMaterial(materialName);
    mpFont = pAssetLibrary->GetFont(fontName);

    CPUTBufferElementInfo pGUIVertex[3] = {
        { "POSITION", 0, 0, CPUT_F32, 3, 3*sizeof(float), 0 },            
        { "TEXCOORD", 0, 1, CPUT_F32, 2, 2*sizeof(float), 3*sizeof(float)},
        { "COLOR",    0, 2, CPUT_F32, 4, 4*sizeof(float), 5*sizeof(float)},
    };

    mpUberBuffer = CPUTMeshOGL::Create();
    mpUberBuffer->CreateNativeResources(NULL, 0, 3, pGUIVertex, mBufferSize, mpMirrorBuffer, NULL, 0, NULL);

    return CPUT_SUCCESS;
}


void CPUTGuiControllerOGL::Draw()
{   
    CPUTRenderParameters params;

    mpMaterial->SetRenderStates();
    mpUberBuffer->Draw();
}

void CPUTGuiControllerOGL::UpdateConstantBuffer()
{
    if(mpConstantBufferVS )
    {
		float znear = 0.1f;
		float zfar = 100.0f;
		float4x4 m;

		m = float4x4OrthographicOffCenterLH(0, (float)mWidth, (float)mHeight, 0, znear, zfar);
    
	    GUIConstantBufferVS ConstantBufferMatrices;
	    ConstantBufferMatrices.Projection = m;
        m = float4x4Identity();
	    ConstantBufferMatrices.Model = m; 
        mpConstantBufferVS->SetData(0, sizeof(GUIConstantBufferVS), &ConstantBufferMatrices);
	}
}

//------------------------------------------------------------------------
CPUTResult CPUTGuiControllerOGL::UpdateUberBuffers()
{
    mpUberBuffer->SetVertexSubData(0, sizeof(CPUTGUIVertex)*mUberBufferIndex+1, mpMirrorBuffer); 
    mpUberBuffer->SetNumVertices(mUberBufferIndex);
    return CPUT_SUCCESS;
}

