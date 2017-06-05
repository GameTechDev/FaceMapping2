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
#include "CPUT_OGL.h"
#include "CPUTRenderStateBlockOGL.h"
#include "CPUTGuiControllerOGL.h"
#include "CPUTCamera.h"
#include "CPUTInputLayoutCache.h"
#include <map>

#ifdef CPUT_FOR_OGLES
#define CPUT_GLSL_VERSION "#version 300 es \n"
#else
#ifdef CPUT_SUPPORT_IMAGE_STORE
#define CPUT_GLSL_VERSION "#version 420 \n"
#else
#define CPUT_GLSL_VERSION "#version 400 \n"
#endif
#endif


// static initializers
CPUT_OGL *gpSample;

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    std::map<int, std::string> glErrorCodes;
    
    glErrorCodes[GL_NO_ERROR] = "GL_NO_ERROR";
    glErrorCodes[GL_INVALID_ENUM] = "GL_INVALID_ENUM";
    glErrorCodes[GL_INVALID_VALUE] = "GL_INVALID_VALUE";
    glErrorCodes[GL_INVALID_OPERATION] = "GL_INVALID_OPERATION";
    glErrorCodes[GL_INVALID_FRAMEBUFFER_OPERATION] = "GL_INVALID_FRAMEBUFFER_OPERATION";
    glErrorCodes[GL_OUT_OF_MEMORY] = "GL_OUT_OF_MEMORY";
    glErrorCodes[GL_STACK_UNDERFLOW] = "GL_STACK_UNDERFLOW";
    glErrorCodes[GL_STACK_OVERFLOW] = "GL_STACK_OVERFLOW";

    GLenum err = glGetError();

    if (err != GL_NO_ERROR)
    {
#ifdef CPUT_OS_ANDROID
        LOGW("OpenGL error %s(0x%08x), at %s:%i - %s\n", glErrorCodes[err].c_str(), err, fname, line, stmt);
#else
        DEBUG_PRINT("OpenGL error %s(0x%08x), at %s:%i - %s\n", glErrorCodes[err].c_str(), err, fname, line, stmt);
#endif
        abort();
    }
}

const char* CPUT_OGL::GLSL_VERSION = NULL;
CPUT_SHADER_MACRO* CPUT_OGL::DEFAULT_MACROS = NULL;

CPUT_OGL::CPUT_OGL() :
        mpWindow(NULL),
        mbShutdown(false),
        mpPerFrameConstantBuffer(NULL),
        mpPerModelConstantBuffer(NULL),
        mpSkinningDataConstantBuffer(NULL)
    {
        GLSL_VERSION = CPUT_GLSL_VERSION;
        DEFAULT_MACROS = NULL;

        gpSample = this;
#if defined CPUT_OS_LINUX
		mpTimer = CPUTTimerLinux::Create();
#elif defined CPUT_OS_ANDROID
		mpTimer = CPUTTimerLinux::Create();
#elif defined CPUT_OS_WINDOWS
		mpTimer = CPUTTimerWin::Create();
#else
#error "No OS defined"
#endif
    }
 


// Destructor
//-----------------------------------------------------------------------------
CPUT_OGL::~CPUT_OGL()
{
    // all previous shutdown tasks should have happened in CPUTShutdown()

	SAFE_DELETE(mpTimer);
    
    // destroy the window
    if(mpWindow)
    {
        delete mpWindow;
        mpWindow = NULL;
    }

}

// Handle keyboard events
//-----------------------------------------------------------------------------
CPUTEventHandledCode CPUT_OGL::CPUTHandleKeyboardEvent(CPUTKey key, CPUTKeyState state)
{
    CPUTEventHandledCode handleCode;
    
    // dispatch event to GUI to handle GUI triggers (if any) #### no GUI for now
    //CPUTEventHandledCode handleCode = CPUTGuiController::GetController()->HandleKeyboardEvent(key);

    // dispatch event to users HandleMouseEvent() method
    HEAPCHECK;
    handleCode = HandleKeyboardEvent(key, state);
    HEAPCHECK;

    return handleCode;
}

// Handle mouse events
//-----------------------------------------------------------------------------
CPUTEventHandledCode CPUT_OGL::CPUTHandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)
{
//    assert(false);
    CPUTEventHandledCode handleCode = CPUT_EVENT_UNHANDLED;
    
    // dispatch event to GUI to handle GUI triggers (if any)
    handleCode = CPUTGuiControllerOGL::GetController()->HandleMouseEvent(x,y,wheel,state, message);

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
CPUTResult CPUT_OGL::MakeWindow(const std::string WindowTitle, CPUTWindowCreationParams windowParams)
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

	// As OpenGL is multiplatform be sure to call proper function on proper OS to create window for now.
#ifdef CPUT_OS_WINDOWS
	mpWindow = new CPUTWindowWin();
#elif defined CPUT_OS_LINUX
    mpWindow = new CPUTWindowX();
#elif defined CPUT_OS_ANDROID
    mpWindow = new CPUTWindowAndroid();
#else
#error "Need OS Support"
#endif

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
CPUTGuiController* CPUT_OGL::CPUTGetGuiController()
{
    return CPUTGuiControllerOGL::GetController();
}

bool CPUT_OGL::supportExtension(const string name)
{
	vector<string>::iterator it;

	for(it=extensions.begin() ; it < extensions.end(); it++) {

		if(name == (*it))
		{
	        DEBUG_PRINT("%s == %s\n", name.c_str(),(*it).c_str());
			return true;
		}
    }
    return false;
}

// This function tests a created DirectX context for specific features required for
// the framework, and possibly sample.  If your sample has specific hw features
// you wish to check for at creation time, you can add them here and have them
// tested at startup time.  If no contexts support your desired features, then
// the system will revert to the DX reference rasterizer, or barring that, 
// pop up a dialog and exit.
//-----------------------------------------------------------------------------
bool CPUT_OGL::TestContextForRequiredFeatures()
{

    return true;
}

// Default creation routine for making the back/stencil buffers
//-----------------------------------------------------------------------------
CPUTResult CPUT_OGL::CreateContext()
{
    int width,height;
    mpWindow->GetClientDimensions(&width, &height);

    // Set default viewport
    glViewport( 0, 0, width, height );
    return CPUT_SUCCESS;
}



// Toggle the fullscreen mode
// This routine keeps the current desktop resolution.  DougB suggested allowing
// one to go fullscreen in a different resolution
//-----------------------------------------------------------------------------
CPUTResult CPUT_OGL::CPUTToggleFullScreenMode()
{    
	CPUTSetFullscreenState(!CPUTGetFullscreenState());
    return CPUT_SUCCESS;
}

// Set the fullscreen mode to a desired state
//-----------------------------------------------------------------------------
void CPUT_OGL::CPUTSetFullscreenState(bool bIsFullscreen)
{
	mpWindow->SetFullscreenState(bIsFullscreen);
	int width, height;
	mpWindow->GetClientDimensions(&width, &height);
	ResizeWindow(width, height);
    FullscreenModeChange( bIsFullscreen );
}

// Get a bool indicating whether the system is in full screen mode or not
//-----------------------------------------------------------------------------
bool CPUT_OGL::CPUTGetFullscreenState()
{
	return mpWindow->GetFullscreenState();
}


// incoming resize event to be handled and translated
//-----------------------------------------------------------------------------
void CPUT_OGL::ResizeWindow(UINT width, UINT height)
{
    DEBUG_PRINT( "ResizeWindow\n" );

	DEBUG_PRINT("Width %d Height %d\n",width, height);
	
	CPUTGuiControllerOGL::GetController()->Resize(width, height);
	
    // set the viewport
    glViewport( 0, 0, width, height );

}

// 'soft' resize - just stretch-blit
//-----------------------------------------------------------------------------
void CPUT_OGL::ResizeWindowSoft(UINT width, UINT height)
{
    UNREFERENCED_PARAMETER(width);
    UNREFERENCED_PARAMETER(height);
    // trigger the GUI manager to resize
 //   CPUTGuiControllerDX11::GetController()->Resize();

    InnerExecutionLoop();
}

//-----------------------------------------------------------------------------
void CPUT_OGL::UpdatePerFrameConstantBuffer( CPUTRenderParameters &renderParams, double totalSeconds )
{
    //NOTE: Issue with using the value of the resultant Uniform Block in shader
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

        CPUTFrameConstantBuffer cb;

        cb.View           = view;
        cb.InverseView    = inverseView;
        cb.Projection     = projection;
        cb.ViewProjection = viewProjection;
        cb.AmbientColor   = float4(mAmbientColor, 0.0f);
        cb.LightColor     = float4(mLightColor, 0.0f);
        cb.LightDirection = lightDir;
        cb.EyePosition    = eyePosition;
        cb.TotalSeconds   = float4((float)totalSeconds);

        mpPerFrameConstantBuffer->SetData(0, sizeof(CPUTFrameConstantBuffer), &cb);
    }
}

void CPUT_OGL::SetSyncInterval(unsigned int syncInterval)
{
#ifdef CPUT_FOR_OGLES3
    eglSwapInterval(display, syncInterval);
#else
    wglSwapIntervalEXT(syncInterval);
#endif
}

// draw all the GUI controls
//-----------------------------------------------------------------------------
void CPUT_OGL::CPUTDrawGUI()
{
#ifdef CPUT_GPA_INSTRUMENTATION
    D3DPERF_BeginEvent(D3DCOLOR(0xff0000), L"CPUT Draw GUI");
#endif

    // draw all the Gui controls
    HEAPCHECK;
        CPUTGuiControllerOGL::GetController()->Draw();
    HEAPCHECK;

#ifdef CPUT_GPA_INSTRUMENTATION
        D3DPERF_EndEvent();
#endif
}

// Create a window context
//-----------------------------------------------------------------------------
CPUTResult CPUT_OGL::CPUTCreateWindowAndContext(const std::string WindowTitle, CPUTWindowCreationParams windowParams)
{
    CPUTResult result = CPUT_SUCCESS;

    HEAPCHECK;

    // We shouldn't destroy old window if it already exist, 
    // Framework user should do this by himself to be aware
    // of what he is doing.
    if( mpWindow )
    {
        return CPUT_ERROR_WINDOW_ALREADY_EXISTS;
    }

    result = MakeWindow(WindowTitle, windowParams);
    if(CPUTFAILED(result))
    {
        return result;
    }


    HEAPCHECK;

    // create the GL context
    result = CreateOGLContext(windowParams.deviceParams);
    if(CPUTFAILED(result))
    {
        return result;
    }


    HEAPCHECK;

    result = CreateContext();

    if (windowParams.startFullscreen) {
        CPUTSetFullscreenState(true);
    }
    //
    // Start the timer after everything is initialized and assets have been loaded
    //
    mpTimer->StartTimer();

    int x, y, width, height;
    mpWindow->GetClientDimensions(&x, &y, &width, &height);

    ResizeWindow(width, height);

    return result;
}

void CPUT_OGL::CreateResources()
{
    CPUTRenderStateBlock *pBlock = CPUTRenderStateBlockOGL::Create();
    CPUTRenderStateBlock::SetDefaultRenderStateBlock( pBlock );
    CPUTBufferDesc desc;
    desc.cpuAccess = BUFFER_CPU_WRITE;
    desc.memory = BUFFER_DYNAMIC;
    desc.target = BUFFER_UNIFORM;
    desc.pData = NULL;

    std::string name = "$cbPerFrameValues";
    desc.sizeBytes = sizeof(CPUTFrameConstantBuffer);
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
    HEAPCHECK;
}


// Pop up a message box with specified title/text
//-----------------------------------------------------------------------------
void CPUT_OGL::DrawLoadingFrame()
{
    // fill first frame with clear values so render order later is ok
    const float srgbClearColor[] = { 0.0993f, 0.0993f, 0.0993f, 1.0f };
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);


    CPUTGuiControllerOGL *pGUIController = CPUTGuiControllerOGL::GetController();
    CPUTText *pText = NULL;
    pGUIController->CreateText("Just a moment, now loading...", 999, 0, &pText);
    pText->SetAutoArranged(false);

    int textWidth, textHeight;
    pText->GetDimensions(textWidth, textHeight);
    int width,height;
    mpWindow->GetClientDimensions(&width, &height);
    pText->SetPosition(width/2-textWidth/2, height/2);

    pGUIController->Draw();
    pGUIController->DeleteAllControls();
    
    // present loading screen
    Present();
    
}

// Pop up a message box with specified title/text
//-----------------------------------------------------------------------------
void CPUT_OGL::CPUTMessageBox(const std::string DialogBoxTitle, const std::string DialogMessage)
{
    CPUTOSServices::OpenMessageBox(DialogBoxTitle.c_str(), DialogMessage.c_str());
}

// start main message loop
//-----------------------------------------------------------------------------
int CPUT_OGL::CPUTMessageLoop()
{
    return mpWindow->StartMessageLoop();
}

// Window is closing. Shut the system to shut down now, not later.
//-----------------------------------------------------------------------------
void CPUT_OGL::DeviceShutdown()
{
	/*
    if(mpSwapChain)
    {
        // DX requires setting fullscreenstate to false before exit.
        mpSwapChain->SetFullscreenState(false, NULL);
    }
     */
    if (mbShutdown == false) {
        mbShutdown = true;
        ShutdownAndDestroy();
    }
}

// Shutdown the CPUT system
// Destroy all 'global' resource handling objects, all asset handlers,
// the DX context, and everything EXCEPT the window
//-----------------------------------------------------------------------------
void CPUT_OGL::Shutdown()
{
    // release the lock on the mouse (if there was one)
//    CPUTOSServices::GetOSServices()->ReleaseMouse();
    mbShutdown = true;
}

void CPUT_OGL::ReleaseResources()
{
    CPUTGuiControllerOGL::DeleteController();
    SAFE_RELEASE(mpPerFrameConstantBuffer);
    SAFE_RELEASE(mpPerModelConstantBuffer);
    SAFE_RELEASE(mpSkinningDataConstantBuffer);

}
// Actually destroy all 'global' resource handling objects, all asset handlers,
// the DX context, and everything EXCEPT the window
//-----------------------------------------------------------------------------
void CPUT_OGL::ShutdownAndDestroy()
{
    // make sure no more rendering can happen
    mbShutdown = true;

    // call the user's OnShutdown code
    Shutdown();
	
    CPUTAssetLibrary::DeleteAssetLibrary();
	CPUTRenderStateBlock::SetDefaultRenderStateBlock( NULL );
    CPUTInputLayoutCache::DeleteInputLayoutCache();

    DestroyOGLContext();
    HEAPCHECK;
}

//-----------------------------------------------------------------------------
void CPUTSetDebugName( void *pResource, std::string name )
{
#ifdef _DEBUG
    //char pCharString[CPUT_MAX_STRING_LENGTH];
    const char *pWideString = name.c_str();
    //UINT ii;
	/*
    UINT length = min( (UINT)name.length(), (CPUT_MAX_STRING_LENGTH-1));
    for(ii=0; ii<length; ii++)
    {
        pCharString[ii] = (char)pWideString[ii];
    }
    pCharString[ii] = 0; // Force NULL termination
    ((ID3D11DeviceChild*)pResource)->SetPrivateData( WKPDID_D3DDebugObjectName, (UINT)name.length(), pCharString );
	 * */
#endif // _DEBUG
}
