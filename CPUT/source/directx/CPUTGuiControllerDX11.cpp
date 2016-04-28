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
#include "CPUTGuiControllerDX11.h"
#include "CPUTBuffer.h"
#include "CPUT_DX11.h"


CPUTGuiControllerDX11* CPUTGuiControllerDX11::mguiController = NULL;

// chained constructor
//--------------------------------------------------------------------------------
CPUTGuiControllerDX11::CPUTGuiControllerDX11():
    mpMaterial(NULL),
    mpConstantBufferVS(NULL),
    mpUberBuffer(NULL)
{
}

// destructor
//--------------------------------------------------------------------------------
CPUTGuiControllerDX11::~CPUTGuiControllerDX11()
{
    SAFE_RELEASE(mpMaterial);
    SAFE_RELEASE(mpConstantBufferVS);
    SAFE_RELEASE(mpUberBuffer);
    DeleteAllControls();
}

// static getter
//--------------------------------------------------------------------------------
CPUTGuiControllerDX11* CPUTGuiControllerDX11::GetController()
{
    if(NULL==mguiController)
    {
        mguiController = new CPUTGuiControllerDX11();
    }
    return mguiController;
}


// Delete the controller
//--------------------------------------------------------------------------------
CPUTResult CPUTGuiControllerDX11::DeleteController()
{
    SAFE_DELETE(mguiController);

    return CPUT_SUCCESS;
}

// Load and register all the resources needed by the GUI system
//-----------------------------------------------------------------------------
CPUTResult CPUTGuiControllerDX11::Initialize(const std::string& material, const std::string& font)
{
    ID3D11DeviceContext* pImmediateContext = CPUT_DX11::GetContext();
    ID3D11Device *pDevice = CPUT_DX11::GetDevice();
    CPUTAssetLibrary* pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
    HRESULT hr = S_OK;
    std::string ErrorMessage;

    CPUTResult result = CPUT_SUCCESS;
    {
        std::string name = "$cbGUIValues";
        CPUTBufferDesc desc;
        desc.cpuAccess = BUFFER_CPU_WRITE;
        desc.memory = BUFFER_DYNAMIC;
        desc.target = BUFFER_UNIFORM;
        desc.pData = NULL;
        desc.sizeBytes = sizeof(GUIConstantBufferVS);
        mpConstantBufferVS = CPUTBuffer::Create(name, &desc);
        pAssetLibrary->AddConstantBuffer(name, "", "", mpConstantBufferVS);
    }

    mpMaterial = pAssetLibrary->GetMaterial(material);
    mpFont = pAssetLibrary->GetFont(font);
    
    CPUTBufferElementInfo pGUIVertex[3] = {
        { "POSITION", 0, 0, CPUT_F32, 3, 3*sizeof(float), 0 },            
        { "TEXCOORD", 0, 1, CPUT_F32, 2, 2*sizeof(float), 3*sizeof(float)},
        { "COLOR",    0, 2, CPUT_F32, 4, 4*sizeof(float), 5*sizeof(float)},
    };
    
    mpUberBuffer = CPUTMeshDX11::Create();
    mpUberBuffer->SetMeshTopology(CPUT_TOPOLOGY_INDEXED_TRIANGLE_LIST);
    mpUberBuffer->CreateNativeResources(NULL, 0, 3, pGUIVertex, mBufferSize, mpMirrorBuffer, NULL, 0, NULL);
    return CPUT_SUCCESS;
}


// Draw - must be positioned after all the controls are defined
//--------------------------------------------------------------------------------
void CPUTGuiControllerDX11::Draw()
{    
    mpMaterial->SetRenderStates();
    CPUTInputLayoutCacheDX11::GetInputLayoutCache()->Apply(mpUberBuffer, mpMaterial);

    mpUberBuffer->Draw();
}

void CPUTGuiControllerDX11::UpdateConstantBuffer()
{
    if(mpConstantBufferVS )
    {
		// set up orthographic display
		float znear = 0.1f;
		float zfar = 100.0f;
		float4x4 m;

		m = float4x4OrthographicOffCenterLH(0, (float)mWidth, (float)mHeight, 0, znear, zfar);
    
	    GUIConstantBufferVS ConstantBufferMatrices;
	    ConstantBufferMatrices.Projection = m;
        m = float4x4Identity();
	    ConstantBufferMatrices.Model = m; 

        mpConstantBufferVS->SetData(0, sizeof(ConstantBufferMatrices), &ConstantBufferMatrices);
    }
}

//
//------------------------------------------------------------------------
CPUTResult CPUTGuiControllerDX11::UpdateUberBuffers()
{
    ID3D11DeviceContext* pImmediateContext = CPUT_DX11::GetContext();
    ASSERT(pImmediateContext, "CPUTGuiControllerDX11::UpdateUberBuffers - Context pointer is NULL");
    ID3D11Buffer* pVB = mpUberBuffer->GetVertexBuffer();
    pImmediateContext->UpdateSubresource(pVB, 0, NULL, (void *)mpMirrorBuffer, sizeof(CPUTGUIVertex)*mUberBufferIndex+1, 0);
    mpUberBuffer->SetNumVertices(mUberBufferIndex);
    return CPUT_SUCCESS;
}

