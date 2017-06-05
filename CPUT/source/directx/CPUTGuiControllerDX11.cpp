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

