/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CPUT_DX11.h"
#include "CPUTRenderStateBlockDX11.h"
#include "CPUTBufferDX11.h"
#include "CPUTTextureDX11.h"

// static initializers
ID3D11Device* CPUT_DX11::mpD3dDevice = NULL;
ID3D11DeviceContext* CPUT_DX11::mpContext = NULL;
CPUT_DX11 *gpSample;

void CPUT_DX11::ReleaseResources()
{
    CPUTGuiControllerDX11::DeleteController();
    SAFE_RELEASE(mpPerModelConstantBuffer);
    SAFE_RELEASE(mpPerFrameConstantBuffer);
    SAFE_RELEASE(mpSkinningDataConstantBuffer);

}

CPUT_DX11::~CPUT_DX11()
{
    // all previous shutdown tasks should have happened in CPUTShutdown()

    // We created the default renderstate block, we release it.

    CPUTRenderStateBlock *pRenderState = CPUTRenderStateBlock::GetDefaultRenderStateBlock();
    SAFE_RELEASE(pRenderState);
    SAFE_RELEASE(mpBackBufferSRV);
    SAFE_RELEASE(mpBackBufferUAV);
    SAFE_RELEASE(mpBackBuffer);
    SAFE_RELEASE(mpDepthBuffer);
    SAFE_RELEASE(mpBackBufferTexture);
    SAFE_RELEASE(mpDepthBufferTexture);


    SAFE_RELEASE(mpDepthStencilSRV);

    // destroy the window
    if(mpWindow)
    {
        delete mpWindow;
        mpWindow = NULL;
    }

    SAFE_DELETE(mpTimer);
    DestroyDXContext();
}

// Handle keyboard events
//-----------------------------------------------------------------------------
CPUTEventHandledCode CPUT_DX11::CPUTHandleKeyboardEvent(CPUTKey key, CPUTKeyState state)
{
    // dispatch event to GUI to handle GUI triggers (if any)
    CPUTEventHandledCode handleCode = CPUTGuiControllerDX11::GetController()->HandleKeyboardEvent(key, state);

    // dispatch event to users HandleMouseEvent() method
    HEAPCHECK;
    handleCode = HandleKeyboardEvent(key, state);
    HEAPCHECK;

    return handleCode;
}

// Handle mouse events
//-----------------------------------------------------------------------------
CPUTEventHandledCode CPUT_DX11::CPUTHandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)
{
    // dispatch event to GUI to handle GUI triggers (if any)
    CPUTEventHandledCode handleCode = CPUTGuiControllerDX11::GetController()->HandleMouseEvent(x,y,wheel,state, message);

    // dispatch event to users HandleMouseEvent() method if it wasn't consumed by the GUI
    if(CPUT_EVENT_HANDLED != handleCode)
    {
        HEAPCHECK;
        handleCode = HandleMouseEvent(x,y,wheel,state, message);
        HEAPCHECK;
    }

    return handleCode;
}


// Call appropriate OS create window call
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::MakeWindow(const std::string WindowTitle, CPUTWindowCreationParams windowParams)
{
    CPUTResult result;

    HEAPCHECK;

    // if we have a window, destroy it
    if(mpWindow)
    {
        delete mpWindow;
        mpWindow = NULL;
    }

    HEAPCHECK;

    // create the OS window
    mpWindow = new CPUTWindowWin();

    result = mpWindow->Create(WindowTitle, windowParams);
    
    mpWindow->RegisterLoopEvent([&]() { this->InnerExecutionLoop(); });
    mpWindow->RegisterCallbackKeyboardEvent([&](CPUTKey key, CPUTKeyState state) -> CPUTEventHandledCode { return this->CPUTHandleKeyboardEvent(key, state); });
    mpWindow->RegisterCallbackMouseEvent([&](int x, int y, int wheel, CPUTMouseState state, CPUTEventID message) -> CPUTEventHandledCode { return this->CPUTHandleMouseEvent(x, y, wheel, state, message); });
    mpWindow->RegisterCallbackResizeEvent([&](int width, int height) { this->ResizeWindow(width, height); });
 
    HEAPCHECK;

    return result;
}

// Return the current GUI controller
//-----------------------------------------------------------------------------
CPUTGuiControllerDX11* CPUT_DX11::CPUTGetGuiController()
{
    return CPUTGuiControllerDX11::GetController();
}


void CPUT_DX11::SetSyncInterval(unsigned int syncInterval)
{
    mSyncInterval = syncInterval;
}

// Create the d3d device
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::CreateDevice()
{
    CPUTResult result = CPUT_SUCCESS;
    HRESULT hr = S_OK;

    //
    // Create the DX device. If the platform does not support the 11.1 feature level then
    // CreateDevice will return E_INVALIDARG and we need to try and create an 11.0 device.
    //

    DWORD createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        numFeatureLevels,
        D3D11_SDK_VERSION,
        &mpD3dDevice,
        &mfeatureLevel,
        &mpContext
        );
    if (hr == E_INVALIDARG)
    {
        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            &featureLevels[1],    // skip D3D_FEATURE_LEVEL_11_1
            numFeatureLevels - 1, // skip D3D_FEATURE_LEVEL_11_1
            D3D11_SDK_VERSION,
            &mpD3dDevice,
            &mfeatureLevel,
            &mpContext
            );
    }
    if (FAILED(hr))
    {
        DEBUG_PRINT("Unable to create DX device.\n");
        result = CPUT_ERROR;
    }
    DEBUG_PRINT("11.0 device interface available\n");

    //
    // Here we check for support for the 11.2 or 11.1 device interfaces.
    //
    ID3D11Device1 *mpD3dDevice1 = nullptr;
	ID3D11DeviceContext1 *mpContext1 = nullptr;

    hr = mpD3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&mpD3dDevice1));
    if (SUCCEEDED(hr))
    {
        DEBUG_PRINT("11.1 device interface available\n");
        hr = mpContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mpContext1));
        if (SUCCEEDED(hr))
        {
            DEBUG_PRINT("11.1 context interface available\n");
        }
    }

#ifdef _DEBUG
	ID3D11Debug *pDebug;
	mpD3dDevice->QueryInterface(IID_ID3D11Debug, (VOID**)(&pDebug));
	if (pDebug)
	{
		ID3D11InfoQueue *d3dInfoQueue = nullptr;
		if (SUCCEEDED(pDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
			d3dInfoQueue->Release();
		}
		pDebug->Release();
	}
#endif

#if _MSC_VER >= 1800
    ID3D11Device2 *mpD3dDevice2 = nullptr;
	ID3D11DeviceContext2 *mpContext2 = nullptr;
    hr = mpD3dDevice->QueryInterface(__uuidof(ID3D11Device2), reinterpret_cast<void**>(&mpD3dDevice2));
    if (SUCCEEDED(hr))
    {
        DEBUG_PRINT("11.2 device interface available\n");
        hr = mpContext->QueryInterface(__uuidof(ID3D11DeviceContext2), reinterpret_cast<void**>(&mpContext2));
        if (SUCCEEDED(hr))
        {
            DEBUG_PRINT("11.2 context interface available\n");
        }
    }
    SAFE_RELEASE(mpD3dDevice2);
    SAFE_RELEASE(mpContext2);
#endif

    SAFE_RELEASE(mpD3dDevice1);
    SAFE_RELEASE(mpContext1);

    return result;
}

// Create the swap chain for the device and window stored in the class
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::CreateSwapChain(CPUTContextCreation ContextParams)
{
    HRESULT hr = S_OK;
    CPUTResult result = CPUT_SUCCESS;

    RECT rc;
    HWND hWnd = mpWindow->GetHWnd();
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = mpD3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
    {
        DEBUG_PRINT("Unable to create DXGI Factory.");
    }

    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
    if (SUCCEEDED(hr))
    {
        mSwapChainFormat = ContextParams.swapChainFormat;

        DXGI_SWAP_CHAIN_DESC1 sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Width       = width;
        sd.Height      = height;
        sd.Format      = ContextParams.swapChainFormat;
        sd.Stereo      = FALSE;
        sd.SampleDesc.Count   = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = ContextParams.swapChainUsage;
        sd.BufferCount = 1;
        sd.Scaling     = DXGI_SCALING_STRETCH;
        sd.SwapEffect  = DXGI_SWAP_EFFECT_DISCARD;
        sd.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Flags       = 0;

        IDXGISwapChain1 *pSwapChain1;
        hr = dxgiFactory2->CreateSwapChainForHwnd(mpD3dDevice, hWnd, &sd, nullptr, nullptr, &pSwapChain1);
        mpSwapChain = pSwapChain1;

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems 
        mSwapChainFormat = ContextParams.swapChainFormat;

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferDesc.Width   = width;
        sd.BufferDesc.Height  = height;
        sd.BufferDesc.RefreshRate.Numerator   = 0;
        sd.BufferDesc.RefreshRate.Denominator = 0;
        sd.BufferDesc.Format  = ContextParams.swapChainFormat;
        sd.SampleDesc.Count   = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage        = ContextParams.swapChainUsage;
        sd.BufferCount        = 1;
        sd.OutputWindow       = hWnd;
        sd.Windowed           = TRUE;
        sd.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags              = 0;

        hr = dxgiFactory->CreateSwapChain(mpD3dDevice, &sd, &mpSwapChain);
    }

    dxgiFactory->Release();

    return result;
}

// Create a DX11 context
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::CreateDXContext(CPUTContextCreation ContextParams)
{

    HRESULT hr = S_OK;
    CPUTResult result = CPUT_SUCCESS;
    // call the DeviceCreated callback/backbuffer/etc creation
    result = CreateContext();
    return result;
}

void CPUT_DX11::CreateResources()
{
    HRESULT hr = S_OK;
    CPUTResult result = CPUT_SUCCESS;
    CPUTRenderStateBlock *pBlock = CPUTRenderStateBlockDX11::Create();
    pBlock->CreateNativeResources();
    CPUTRenderStateBlock::SetDefaultRenderStateBlock( pBlock );

    CPUTBufferDesc desc;
    desc.cpuAccess = BUFFER_CPU_WRITE;
    desc.memory = BUFFER_DYNAMIC;
    desc.target = BUFFER_UNIFORM;
    desc.pData = NULL;
    desc.sizeBytes = sizeof(CPUTFrameConstantBuffer);

    std::string name = "$cbPerFrameValues";
    mpPerFrameConstantBuffer = CPUTBuffer::Create(name, &desc);
    CPUTAssetLibrary::GetAssetLibrary()->AddConstantBuffer("", name, "", mpPerFrameConstantBuffer);

    name = "$cbPerModelValues";
    desc.sizeBytes = sizeof(CPUTModelConstantBuffer);
    mpPerModelConstantBuffer = CPUTBuffer::Create(name, &desc);
    CPUTAssetLibrary::GetAssetLibrary()->AddConstantBuffer("", name, "", mpPerModelConstantBuffer);

    name = "$cbSkinningValues";
    desc.sizeBytes = sizeof(CPUTAnimationConstantBuffer);
    mpSkinningDataConstantBuffer = CPUTBuffer::Create(name, &desc);
    CPUTAssetLibrary::GetAssetLibrary()->AddConstantBuffer("", name, "", mpSkinningDataConstantBuffer);
}

// This function tests a created DirectX context for specific features required for
// the framework, and possibly sample.  If your sample has specific hw features
// you wish to check for at creation time, you can add them here and have them
// tested at startup time.  If no contexts support your desired features, then
// the system will revert to the DX reference rasterizer, or barring that, 
// pop up a dialog and exit.
//-----------------------------------------------------------------------------
bool CPUT_DX11::TestContextForRequiredFeatures()
{
    // D3D11_RESOURCE_MISC_BUFFER_STRUCTURED check
    // attempt to create a 
    // create the buffer for the shader resource view
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    desc.Usage = D3D11_USAGE_DEFAULT;
    // set the stride for one 'element' block of verts
    UINT m_VertexStride      = 4*sizeof(float);                 // size in bytes of a single element - this test case we'll use 4 floats 
    desc.ByteWidth           = 1 * m_VertexStride;              // size in bytes of entire buffer - this test case uses just one element
    desc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags      = 0;
    desc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = m_VertexStride;

    ID3D11Buffer *pVertexBufferForSRV=NULL;
    D3D11_SUBRESOURCE_DATA resourceData;
    float pData[4] ={ 0.0f, 0.0f, 0.0f, 0.0f };

    ZeroMemory( &resourceData, sizeof(resourceData) );
    resourceData.pSysMem = pData;
    HRESULT hr = mpD3dDevice->CreateBuffer( &desc, &resourceData, &pVertexBufferForSRV );
    SAFE_RELEASE(pVertexBufferForSRV);
    if(!SUCCEEDED(hr))
    {
        // failed the feature test
        return false;
    }

    // add other required features here

    return true;
}

// Return the active D3D device used to create the context
//-----------------------------------------------------------------------------
ID3D11Device* CPUT_DX11::GetDevice()
{
    return mpD3dDevice;
}

// Default creation routine for making the back/stencil buffers
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::CreateContext()
{
    HRESULT hr;
    CPUTResult result;
    RECT rc;
    HWND hWnd = mpWindow->GetHWnd();

    GetClientRect( hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    // Create a render target view
    ID3D11Texture2D *pBackBuffer = NULL;
    hr = mpSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    ASSERT( SUCCEEDED(hr), "Failed getting back buffer." );

    hr = mpD3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &mpBackBufferRTV );
    pBackBuffer->Release();
    ASSERT( SUCCEEDED(hr), "Failed creating render target view." );
    CPUTSetDebugName( mpBackBufferRTV, "BackBufferView" );

    // create depth/stencil buffer
    result = CreateAndBindDepthBuffer(width, height);
    ASSERT( SUCCEEDED(hr), "Failed creating and binding depth buffer." );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    mpContext->RSSetViewports( 1, &vp );

    return CPUT_SUCCESS;
}

// destroy the DX context and release all resources
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::DestroyDXContext()
{
    if (mpContext) {
        mpContext->ClearState();
        mpContext->Flush();
    }

    SAFE_RELEASE( mpBackBufferRTV );
    SAFE_RELEASE( mpDepthStencilBuffer );
    SAFE_RELEASE( mpDepthStencilState );
    SAFE_RELEASE( mpDepthStencilView );
    SAFE_RELEASE( mpContext );
    SAFE_RELEASE( mpD3dDevice );
    SAFE_RELEASE( mpSwapChain );

    return CPUT_SUCCESS;
}

// Toggle fullscreen mode
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::CPUTToggleFullScreenMode()
{    
    // Get the current fullscreen state
    bool bIsFullscreen = CPUTGetFullscreenState();

    // Toggle the state
    bIsFullscreen = !bIsFullscreen;

    // Set the fullscreen state


    HRESULT hr = mpSwapChain->SetFullscreenState(bIsFullscreen, NULL);
    // Resize our window and any render targets, etc that are related to the window dimensions
    int x,y,width,height;
    mpWindow->GetClientDimensions(&x, &y, &width, &height);
    ResizeWindow(width,height);

    ASSERT( SUCCEEDED(hr), "Failed toggling full screen mode." );
    UNREFERENCED_PARAMETER(hr);

    // Set the fullscreen state
    FullscreenModeChange( bIsFullscreen );
    return CPUT_SUCCESS;
}

//-----------------------------------------------------------------------------
void CPUT_DX11::CPUTSetFullscreenState(bool bIsFullscreen)
{
    // get the current fullscreen state
    bool bCurrentFullscreenState = CPUTGetFullscreenState();
    if((bool)bCurrentFullscreenState == bIsFullscreen)
    {
        // No need to call expensive state change, full screen state is already in desired state
        return;
    }

    // Set the fullscreen state
    HRESULT hr = mpSwapChain->SetFullscreenState(bIsFullscreen, NULL);
    ASSERT( SUCCEEDED(hr), "Failed toggling full screen mode." );
    UNREFERENCED_PARAMETER(hr);

    // Resize our window and any render targets, etc that are related to the window dimensions
    int x,y,width,height;
    mpWindow->GetClientDimensions(&x, &y, &width, &height);
    ResizeWindow(width,height);

    // trigger a fullscreen mode change call if the sample has decided to handle the mode change
    FullscreenModeChange( bIsFullscreen );
}

//-----------------------------------------------------------------------------
bool CPUT_DX11::CPUTGetFullscreenState()
{
    // get the current fullscreen state
    BOOL bCurrentlyFullscreen;
    IDXGIOutput *pSwapTarget=NULL;
    mpSwapChain->GetFullscreenState(&bCurrentlyFullscreen, &pSwapTarget);
    SAFE_RELEASE(pSwapTarget);
    if(bCurrentlyFullscreen )
    {
        return true;
    }
    return false;
}

// Create the depth buffer
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::CreateAndBindDepthBuffer(int width, int height)
{
    HRESULT hr;

    // Clamp to minimum size of 1x1 pixel
    width  = std::max( width, 1 );
    height = std::max( height, 1 );

    // ---- DEPTH BUFFER ---
    // 1. Initialize the description of the depth buffer.
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    hr = mpD3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &mpDepthStencilBuffer);
    ASSERT( SUCCEEDED(hr), "Failed to create texture." );
    CPUTSetDebugName( mpDepthStencilBuffer, "DepthBufferTexture" );

    // 2. Initialize the description of the stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;    
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable    = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_GREATER_EQUAL;

    depthStencilDesc.StencilEnable    = true;
    depthStencilDesc.StencilReadMask  = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    hr = mpD3dDevice->CreateDepthStencilState(&depthStencilDesc, &mpDepthStencilState);
    ASSERT( SUCCEEDED(hr), "Failed to create depth-stencil state." );
    mpContext->OMSetDepthStencilState(mpDepthStencilState, 1);

    // Create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Format              = DXGI_FORMAT_R32_FLOAT;
    hr = mpD3dDevice->CreateShaderResourceView(mpDepthStencilBuffer, &srvDesc, &mpDepthStencilSRV);
    ASSERT( SUCCEEDED(hr), "Failed to create depth-stencil SRV." );
    CPUTSetDebugName( mpDepthStencilSRV, "DepthStencilSRV" );

    // Create the depth stencil view.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format             = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    hr = mpD3dDevice->CreateDepthStencilView(mpDepthStencilBuffer, &depthStencilViewDesc, &mpDepthStencilView);
    ASSERT( SUCCEEDED(hr), "Failed to create depth-stencil view." );
    CPUTSetDebugName( mpDepthStencilView, "DepthStencilView" );

    // Bind the render target view and depth stencil buffer to the output render pipeline.
    mpContext->OMSetRenderTargets(1, &mpBackBufferRTV, mpDepthStencilView);

    CPUTRenderTargetColor::SetActiveRenderTargetView( mpBackBufferRTV );
    CPUTRenderTargetDepth::SetActiveDepthStencilView( mpDepthStencilView );

    return CPUT_SUCCESS;
}

// incoming resize event to be handled and translated
//-----------------------------------------------------------------------------
void CPUT_DX11::ResizeWindow(UINT width, UINT height)
{
    HRESULT hr;
    CPUTResult result;
    CPUTAssetLibraryDX11 *pAssetLibrary = (CPUTAssetLibraryDX11*)CPUTAssetLibraryDX11::GetAssetLibrary();

    SAFE_RELEASE(mpBackBufferRTV);	
    SAFE_RELEASE(mpBackBufferSRV);
    SAFE_RELEASE(mpBackBufferUAV);
    SAFE_RELEASE(mpDepthStencilSRV);
	if (mpBackBuffer)
	{
		CPUTBufferDX11* pBackBuffer = (CPUTBufferDX11*)mpBackBuffer;
		pBackBuffer->ReleaseNativeBuffer();
		pBackBuffer->ReleaseShaderResourceView();
		pBackBuffer->ReleaseUnorderedAccessView();
	}
    if (mpBackBufferTexture)
    {
        CPUTTextureDX11* pBackBuffer = (CPUTTextureDX11*)mpBackBufferTexture;
        pBackBuffer->ReleaseTexture();
    }
	if (mpDepthBuffer)
	{
		CPUTBufferDX11* pDepthBuffer = (CPUTBufferDX11*)mpDepthBuffer;
		pDepthBuffer->ReleaseNativeBuffer();
		pDepthBuffer->ReleaseShaderResourceView();
		pDepthBuffer->ReleaseUnorderedAccessView();
	}
    if (mpBackBufferTexture)
    {
        CPUTTextureDX11* pTexture = (CPUTTextureDX11*)mpDepthBufferTexture;
        pTexture->ReleaseTexture();
    }
    mpContext->ClearState();
    Present();
    mpContext->Flush();
    
    CPUT::ResizeWindow(width, height);

    // Call the sample's clean up code if present.
    ReleaseSwapChain();

    // handle the internals of a resize
    int windowWidth, windowHeight;
    mpWindow->GetClientDimensions( &windowWidth, &windowHeight);

    // resize the swap chain
    hr = mpSwapChain->ResizeBuffers(mSwapChainBufferCount, windowWidth, windowHeight, mSwapChainFormat, 0);

    ASSERT( SUCCEEDED(hr), "Error resizing swap chain" );

    // re-create the render-target view
    ID3D11Texture2D *pSwapChainBuffer = NULL;
    hr = mpSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D), (LPVOID*) (&pSwapChainBuffer));
    ASSERT(SUCCEEDED(hr), "");
    hr = mpD3dDevice->CreateRenderTargetView( pSwapChainBuffer, NULL, &mpBackBufferRTV);
    ASSERT(SUCCEEDED(hr), "");
    hr = mpD3dDevice->CreateShaderResourceView( pSwapChainBuffer, NULL, &mpBackBufferSRV);
    ASSERT(SUCCEEDED(hr), "");
#ifdef CREATE_SWAP_CHAIN_UAV
    // Not every DXGI format supports UAV.  So, create UAV only if sample chooses to do so.
    hr = mpD3dDevice->CreateUnorderedAccessView( pSwapChainBuffer, NULL, &mpBackBufferUAV);
    ASSERT(SUCCEEDED(hr), "");
#endif
    // Add the back buffer to the asset library.  Create CPUTBuffer and a CPUTTexture forms and add them.
    if (!mpBackBuffer)
    {
        std::string name = "$BackBuffer";
        mpBackBuffer = CPUTBuffer::Create(name, NULL);
        pAssetLibrary->AddBuffer(name, "", "", mpBackBuffer);
    }
    CPUTBufferDX11* pBackBufferDX11 = (CPUTBufferDX11*)mpBackBuffer;
    pBackBufferDX11->SetShaderResourceView(mpBackBufferSRV);
    //pBackBufferDX11->SetUnorderedAccessView(mpBackBufferUAV);

    if( mpBackBufferTexture )
    {
        ((CPUTTextureDX11*)mpBackBufferTexture)->SetTextureAndShaderResourceView( NULL, mpBackBufferSRV );
    }
    else
    {
        std::string backBufferName = "$BackBuffer"; 
        mpBackBufferTexture  = CPUTTextureDX11::Create( backBufferName, NULL, mpBackBufferSRV );
        pAssetLibrary->AddTexture( backBufferName, "", "", mpBackBufferTexture );
    }

    // release the old depth buffer objects
    // release the temporary swap chain buffer
    SAFE_RELEASE(pSwapChainBuffer);
    SAFE_RELEASE(mpDepthStencilBuffer);
    SAFE_RELEASE(mpDepthStencilState);
    SAFE_RELEASE(mpDepthStencilView);

    result = CreateAndBindDepthBuffer(windowWidth, windowHeight);
    if(CPUTFAILED(result))
    {
        // depth buffer creation error
        ASSERT(0,"");
    }

    if( !mpDepthBuffer )
    {
        std::string name = "$DepthBuffer";
        mpDepthBuffer = CPUTBuffer::Create(name, NULL);
        pAssetLibrary->AddBuffer(name, "", "", mpDepthBuffer);
    }
    CPUTBufferDX11* pDepthBufferDX11 = (CPUTBufferDX11*)mpDepthBuffer;
    pDepthBufferDX11->SetShaderResourceView(mpDepthStencilSRV);
    
    if( mpDepthBufferTexture )
    {
        ((CPUTTextureDX11*)mpDepthBufferTexture)->SetTextureAndShaderResourceView( NULL, mpDepthStencilSRV );
    }
    else
    {
        std::string DepthBufferName = "$DepthBuffer"; 
        mpDepthBufferTexture  = CPUTTextureDX11::Create( DepthBufferName, NULL, mpDepthStencilSRV );
        pAssetLibrary->AddTexture( DepthBufferName, "", "", mpDepthBufferTexture );
    }

    // set the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT) windowWidth;
    vp.Height = (FLOAT)windowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    mpContext->RSSetViewports( 1, &vp );

    // trigger the GUI manager to resize
    CPUTGuiControllerDX11::GetController()->Resize(width, height);
}

// 'soft' resize - just stretch-blit
//-----------------------------------------------------------------------------
void CPUT_DX11::ResizeWindowSoft(UINT width, UINT height)
{
    UNREFERENCED_PARAMETER(width); UNREFERENCED_PARAMETER(height);
    InnerExecutionLoop();
}

//-----------------------------------------------------------------------------
void CPUT_DX11::UpdatePerFrameConstantBuffer( CPUTRenderParameters &renderParams, double totalSeconds )
{
    if( mpPerFrameConstantBuffer )
    {
        float4x4 view, projection, inverseView, viewProjection;
        float4 eyePosition, lightDir;
        if( renderParams.mpCamera )
        {
            view = *renderParams.mpCamera->GetViewMatrix();
            projection = *renderParams.mpCamera->GetProjectionMatrix();
            inverseView = inverse(*renderParams.mpCamera->GetViewMatrix());
            eyePosition = float4(renderParams.mpCamera->GetPosition(), 0.0f);        
            viewProjection = view * projection;
         
        }
        if( renderParams.mpShadowCamera )
        {
            lightDir = float4(normalize(renderParams.mpShadowCamera->GetLook()), 0);
        }
        CPUTFrameConstantBuffer perframeconstant;
        perframeconstant.View = view;
        perframeconstant.InverseView = inverseView;
        perframeconstant.Projection = projection;
        perframeconstant.ViewProjection = viewProjection;
        perframeconstant.AmbientColor = mAmbientColor;
        perframeconstant.LightColor = mLightColor;
        perframeconstant.LightDirection = lightDir;
        perframeconstant.EyePosition = eyePosition;
        perframeconstant.TotalSeconds = float4((float)totalSeconds);

        mpPerFrameConstantBuffer->SetData(0, sizeof(CPUTFrameConstantBuffer), &perframeconstant);        
    }
}

// Call the user's Render() callback (if it exists)
//-----------------------------------------------------------------------------
void CPUT_DX11::InnerExecutionLoop()
{
#ifdef CPUT_GPA_INSTRUMENTATION
    D3DPERF_BeginEvent(D3DCOLOR(0xff0000), L"CPUT User's Render() ");
#endif
    if(!mbShutdown)
    {
        double deltaSeconds = mpTimer->GetElapsedTime();
        Update(deltaSeconds);
        Present(); // Note: Presenting immediately before Rendering minimizes CPU stalls (i.e., execute Update() before Present() stalls)

        double totalSeconds = mpTimer->GetTotalTime();
        
        Render(deltaSeconds);
        if(!mpWindow->DoesWindowHaveFocus() && !mpWindow->IsTopmost())
        {
            Sleep(100);
        }
    }
    else
    {
#ifndef _DEBUG
        exit(0);
#endif
        Present(); // Need to present, or will leak all references held by previous Render()!
        ShutdownAndDestroy();
    }

#ifdef CPUT_GPA_INSTRUMENTATION
    D3DPERF_EndEvent();
#endif
}

// draw all the GUI controls
//-----------------------------------------------------------------------------
void CPUT_DX11::CPUTDrawGUI()
{
#ifdef CPUT_GPA_INSTRUMENTATION
    D3DPERF_BeginEvent(D3DCOLOR(0xff0000), L"CPUT Draw GUI");
#endif

    // draw all the Gui controls
    HEAPCHECK;
    CPUTGuiControllerDX11::GetController()->Draw();
    HEAPCHECK;

#ifdef CPUT_GPA_INSTRUMENTATION
    D3DPERF_EndEvent();
#endif
}

// Create a window context
//-----------------------------------------------------------------------------
CPUTResult CPUT_DX11::CPUTCreateWindowAndContext(const std::string WindowTitle, CPUTWindowCreationParams windowParams)
{
    CPUTResult result = CPUT_SUCCESS;

    HEAPCHECK;

    // create the window
    result = MakeWindow(WindowTitle, windowParams);
    if(CPUTFAILED(result))
    {
        return result;
    }
    HEAPCHECK;

    CreateDevice();
    CreateSwapChain(windowParams.deviceParams);

    // create the DX context
    result = CreateDXContext(windowParams.deviceParams);
    if(CPUTFAILED(result))
    {
        return result;
    }
    HEAPCHECK;

    // If the WARP or Reference rasterizer is being used, the performance will suffer.  Display message box with a warning.
    if((D3D_DRIVER_TYPE_REFERENCE == mdriverType) || (D3D_DRIVER_TYPE_WARP == mdriverType))
    {
        CPUTOSServices::OpenMessageBox("Performance warning", "Your graphics hardware does not support the DirectX features required by this sample. The sample is now running using the DirectX software rasterizer.");
    }
    HEAPCHECK;
    //
    // Start the timer after everything is initialized and assets have been loaded
    //
    mpTimer->StartTimer();

    // Simply return if ShutdownAndDestroy() during creation process.
    if(mbShutdown)
    {
        return result;
    }

    // does user want to start in fullscreen mode?
    if(windowParams.startFullscreen)
    {
        result = CPUTToggleFullScreenMode();
        if(CPUTFAILED(result))
        {
            return result;
        }
    }

    // fill first frame with clear values so render order later is ok
    const float srgbClearColor[] = { 0.0993f, 0.0993f, 0.0993f, 1.0f };
    mpContext->ClearRenderTargetView( mpBackBufferRTV, srgbClearColor );
    mpContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

    // trigger a 'resize' event
    int x,y,width,height;
    mpWindow->GetClientDimensions(&x, &y, &width, &height);
    ResizeWindow(width,height);

    return result;
}

// Pop up a message box with specified title/text
//-----------------------------------------------------------------------------
void CPUT_DX11::DrawLoadingFrame()
{
    // fill first frame with clear values so render order later is ok
    const float srgbClearColor[] = { 0.0993f, 0.0993f, 0.0993f, 1.0f };
    mpContext->ClearRenderTargetView( mpBackBufferRTV, srgbClearColor );
    mpContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

    // get center
    int x,y,width,height;
    mpWindow->GetClientDimensions(&x, &y, &width, &height);

    // draw "loading..." text
    CPUTGuiControllerDX11 *pGUIController = CPUTGuiControllerDX11::GetController();
    CPUTText *pText = NULL;
    pGUIController->CreateText("Just a moment, now loading...", 999, 0, &pText);
    pText->SetAutoArranged(false);
    int textWidth, textHeight;
    pText->GetDimensions(textWidth, textHeight);
    pText->SetPosition(width/2-textWidth/2, height/2);

    pGUIController->Draw();
    pGUIController->DeleteAllControls();

    // present loading screen
    mpSwapChain->Present( mSyncInterval, 0 );
}

// Pop up a message box with specified title/text
//-----------------------------------------------------------------------------
void CPUT_DX11::CPUTMessageBox(const std::string DialogBoxTitle, const std::string DialogMessage)
{
    CPUTOSServices::OpenMessageBox(DialogBoxTitle.c_str(), DialogMessage.c_str());
}

// start main message loop
//-----------------------------------------------------------------------------
int CPUT_DX11::CPUTMessageLoop()
{
#ifdef CPUT_GPA_INSTRUMENTATION
    D3DPERF_BeginEvent(D3DCOLOR(0xff0000), L"CPUTMessageLoop");
#endif

    return mpWindow->StartMessageLoop();

#ifdef CPUT_GPA_INSTRUMENTATION
    D3DPERF_EndEvent();
#endif
}

// Window is closing. Shut the system to shut down now, not later.
//-----------------------------------------------------------------------------
void CPUT_DX11::DeviceShutdown()
{
    if(mpSwapChain)
    {
        // DX requires setting fullscreenstate to false before exit.
        mpSwapChain->SetFullscreenState(false, NULL);
    }
    if(!mbShutdown)
    {
        mbShutdown = true;
        ShutdownAndDestroy();
    }
}

// Shutdown the CPUT system
// Destroy all 'global' resource handling objects, all asset handlers,
// the DX context, and everything EXCEPT the window
//-----------------------------------------------------------------------------
void CPUT_DX11::Shutdown()
{
    // release the lock on the mouse (if there was one)
    mpWindow->ReleaseMouse();
    mbShutdown = true;
}


// Actually destroy all 'global' resource handling objects, all asset handlers,
// the DX context, and everything EXCEPT the window
//-----------------------------------------------------------------------------
void CPUT_DX11::ShutdownAndDestroy()
{
    // make sure no more rendering can happen
    mbShutdown = true;

    // call the user's OnShutdown code
    Shutdown();
    CPUTInputLayoutCacheDX11::DeleteInputLayoutCache();
    CPUTAssetLibraryDX11::DeleteAssetLibrary();

    // #ifdef _DEBUG
#if 0
    ID3D11Debug *pDebug;
    mpD3dDevice->QueryInterface(IID_ID3D11Debug, (VOID**)(&pDebug));
    if( pDebug )
    {
        pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        pDebug->Release();
    }
#endif

    HEAPCHECK;
}

//-----------------------------------------------------------------------------
void CPUTSetDebugName( void *pResource, std::string name )
{
#ifdef _DEBUG

    char pCharString[CPUT_MAX_STRING_LENGTH];
    UINT checkLength = sizeof(char) * CPUT_MAX_STRING_LENGTH;
    ((ID3D11DeviceChild*)pResource)->GetPrivateData( WKPDID_D3DDebugObjectName, &checkLength, pCharString);
    if(checkLength == 0)
    {
        const char *pWideString = name.c_str();
        UINT ii;
        UINT length = std::min( (UINT)name.length(), (CPUT_MAX_STRING_LENGTH-1));
        for(ii=0; ii<length; ii++)
        {
            pCharString[ii] = (char)pWideString[ii];
        }
        pCharString[ii] = 0; // Force NULL termination

        ((ID3D11DeviceChild*)pResource)->SetPrivateData( WKPDID_D3DDebugObjectName, (UINT)name.length(), pCharString );
    }
#endif // _DEBUG
}
