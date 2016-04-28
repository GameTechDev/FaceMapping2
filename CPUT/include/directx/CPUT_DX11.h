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
#ifndef __CPUT_DX11_H__
#define __CPUT_DX11_H__

#include <stdio.h>
#include "DXGIDebug.h"

// include base headers we'll need
#include "CPUTWindowWin.h"
#include "CPUT.h"
#include "CPUTMath.h"
#include "CPUTEventHandler.h"
#include "CPUTGuiControllerDX11.h"

// CPUT objects
#include "CPUTMeshDX11.h"
#include "CPUTAssetLibraryDX11.h"
#include "CPUTCamera.h"
#include "CPUTLight.h"
#include "CPUTMaterialDX11.h"
#include "CPUTTimerWin.h"

// DirectX 11 CPUT layer
//-----------------------------------------------------------------------------
class CPUT_DX11;
extern CPUT_DX11 *gpSample;

class CPUT_DX11:public CPUT
{
protected:
    static ID3D11Device *mpD3dDevice;
    static ID3D11DeviceContext *mpContext;

public:
    static ID3D11Device *GetDevice();
    static ID3D11DeviceContext *GetContext() { return mpContext; }
	HWND GetHWnd() { return mpWindow->GetHWnd(); }
	//HINSTANCE GetHInstance() { return mpWindow->GetHInstance(); }
    virtual void CreateResources();
    virtual void ReleaseResources();

protected:
    bool                       mbShutdown;
    CPUTWindowWin             *mpWindow;
    D3D_DRIVER_TYPE            mdriverType;
    D3D_FEATURE_LEVEL          mfeatureLevel; // potentially not accurate. 
    IDXGISwapChain            *mpSwapChain;
    DXGI_FORMAT                mSwapChainFormat;
    UINT                       mSwapChainBufferCount;
    
    ID3D11RenderTargetView    *mpBackBufferRTV;
    ID3D11ShaderResourceView  *mpBackBufferSRV;
    ID3D11UnorderedAccessView *mpBackBufferUAV;

    ID3D11Texture2D           *mpDepthStencilBuffer;
    ID3D11DepthStencilView    *mpDepthStencilView; // was in protected
    ID3D11ShaderResourceView  *mpDepthStencilSRV;

    ID3D11DepthStencilState   *mpDepthStencilState;

    UINT                       mSyncInterval; // used for vsync

    //move to sample
    CPUTBuffer            *mpPerFrameConstantBuffer;
    CPUTBuffer            *mpPerModelConstantBuffer;
    CPUTBuffer            *mpSkinningDataConstantBuffer;

public:
    CPUT_DX11():
        mpWindow(NULL),
        mpSwapChain(NULL),
        mSwapChainBufferCount(1),
        mpBackBufferRTV(NULL),
        mpBackBufferSRV(NULL),
        mpBackBufferUAV(NULL),
        mpDepthStencilBuffer(NULL),
        mpDepthStencilState(NULL),
        mpDepthStencilView(NULL),
        mpDepthStencilSRV(NULL),
        mSwapChainFormat(DXGI_FORMAT_UNKNOWN),
        mbShutdown(false),
        mSyncInterval(1),    // start with vsync on
        mpPerFrameConstantBuffer(NULL)
    {
		mpTimer = CPUTTimerWin::Create();
        gpSample = this;
    }
    virtual ~CPUT_DX11();

    D3D_FEATURE_LEVEL GetFeatureLevel() { return mfeatureLevel; }

    int CPUTMessageLoop();
    CPUTResult CPUTCreateWindowAndContext(const std::string WindowTitle, CPUTWindowCreationParams windowParams);

    // CPUT interfaces
    virtual void ResizeWindow(UINT width, UINT height);
    virtual void ResizeWindowSoft(UINT width, UINT height);
    void DeviceShutdown();
    
    void UpdatePerFrameConstantBuffer( CPUTRenderParameters &renderParams, double totalSeconds );
    void InnerExecutionLoop();

    // events
    virtual void Update(double deltaSeconds) {}
    virtual void Present() { if(mpSwapChain) { mpSwapChain->Present( mSyncInterval, 0 );} }
    virtual void Render(double deltaSeconds) = 0;
    virtual void Create()=0;
    virtual void Shutdown();
    virtual void FullscreenModeChange(bool bFullscreen) {UNREFERENCED_PARAMETER(bFullscreen);}
    virtual void ReleaseSwapChain() {}
    virtual CPUTResult CreateContext();
    virtual void SetSyncInterval(unsigned int syncInterval);
    

    // GUI
    void CPUTDrawGUI();

    // Event Handling
    CPUTEventHandledCode CPUTHandleKeyboardEvent(CPUTKey key, CPUTKeyState state);
    CPUTEventHandledCode CPUTHandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message );

    // Utility functions for the sample developer
    CPUTResult CPUTToggleFullScreenMode();
    void CPUTSetFullscreenState(bool bIsFullscreen);
    bool CPUTGetFullscreenState();
    CPUTGuiControllerDX11* CPUTGetGuiController();

    // Message boxes
    void CPUTMessageBox(const std::string DialogBoxTitle, const std::string DialogMessage);

protected:
    // private helper functions
    bool TestContextForRequiredFeatures();
    void ShutdownAndDestroy();
    virtual CPUTResult CreateDevice();
    virtual CPUTResult CreateSwapChain(CPUTContextCreation ContextParams);
    virtual CPUTResult CreateDXContext(CPUTContextCreation ContextParams);   // allow user to override DirectX context creation
    virtual CPUTResult DestroyDXContext();  // allow user to override DirectX context destruction
    CPUTResult         MakeWindow(const std::string WindowTitle, CPUTWindowCreationParams windowParams);
    CPUTResult         CreateAndBindDepthBuffer(int width, int height);
    void               DrawLoadingFrame();

    // TODO: Put this somewhere else
    bool               FindMatchingInputSlot(const char *pInputSlotName, const ID3DBlob *pVertexShaderBlob);
};

#endif //#ifndef __CPUT_DX11_H__
