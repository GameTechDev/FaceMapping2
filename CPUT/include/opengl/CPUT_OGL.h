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
#ifndef __CPUT_OGL_H__
#define __CPUT_OGL_H__

#include <stdio.h>
// include base headers we'll need
#include "CPUT.h"
#include "CPUTMath.h"
#include "CPUTEventHandler.h"
#include "CPUTGuiController.h"
#include "CPUTBufferOGL.h"

#include <vector>
#include <string>
using namespace std;

//#define CPUT_SUPPORT_TESSELLATION
//#define CPUT_SUPPORT_IMAGE_STORE
#ifdef CPUT_SUPPORT_IMAGE_STORE
#define CPUT_OPENGL_MAJOR 4
#define CPUT_OPENGL_MINOR 2
#else
#define CPUT_OPENGL_MAJOR 4
#define CPUT_OPENGL_MINOR 0
#endif 
#ifdef CPUT_OS_LINUX
#include <GL/glew.h>
#endif

// CPUT objects
#ifdef CPUT_OS_WINDOWS
#include "CPUTWindowWin.h"
#include "CPUTTimerWin.h"
#elif defined CPUT_OS_LINUX
#include "CPUTWindowX.h"
#include "CPUTTimerLinux.h"
#elif defined CPUT_OS_ANDROID
#include "CPUTWindowAndroid.h"
#include "CPUTTimerLinux.h"
#else
#error "OS not defined"
#endif

#define CPUT_STRINGIFY2(a) #a
#define CPUT_STRINGIFY(a) CPUT_STRINGIFY2(a)
#define CPUT_PASTE2(a, b) a##b
#define CPUT_PASTE(a, b) CPUT_PASTE2(a, b)

void CheckOpenGLError(const char* stmt, const char* fname, int line);
#ifdef DEBUG
    #define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (false)
#else
    #define GL_CHECK(stmt) stmt
#endif

#if  defined( CPUT_FOR_OGLES )

#ifndef EGL_CONTEXT_MINOR_VERSION_KHR
#define EGL_CONTEXT_MINOR_VERSION_KHR 0x30FB
#endif

#ifndef EGL_CONTEXT_MAJOR_VERSION_KHR
#define EGL_CONTEXT_MAJOR_VERSION_KHR EGL_CONTEXT_CLIENT_VERSION
#endif

typedef void (GL_APIENTRY * PFNGLDISPATCHCOMPUTEPROC) (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void (GL_APIENTRY * PFNGLBINDIMAGETEXTUREPROC) (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
typedef void (GL_APIENTRY * PFNGLPATCHPARAMETERFVPROC) (GLenum pname, const GLfloat* values);
typedef void (GL_APIENTRY * PFNGLPATCHPARAMETERIPROC) (GLenum pname, GLint value);
typedef void (GL_APIENTRY * PFNGLQUERYCOUNTERPROC) (GLuint id, GLenum target);
typedef void (GL_APIENTRY * PFNGLGETQUERYOBJECTIVPROC) (GLuint id, GLenum pname, GLint* params);
typedef void (GL_APIENTRY * PFNGLGETQUERYOBJECTUI64VPROC) (GLuint id, GLenum pname, GLuint64* params);
typedef GLuint(GL_APIENTRY * PFNGLGETPROGRAMRESOURCEINDEXPROC) (GLuint program, GLenum programInterface, const GLchar* name);
typedef void (GL_APIENTRY * PFNGLGETPROGRAMRESOURCENAMEPROC) (GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar *name);
typedef void (GL_APIENTRY * PFNGLGETPROGRAMINTERFACEIVPROC) (GLuint program, GLenum programInterface, GLenum pname, GLint* params);
typedef void (GL_APIENTRY *  PFNGLSHADERSTORAGEBLOCKBINDINGPROC) (GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);

extern PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
extern PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
extern PFNGLPATCHPARAMETERIPROC glPatchParameteri;
extern PFNGLPATCHPARAMETERFVPROC glPatchParameterfv;
extern PFNGLQUERYCOUNTERPROC glQueryCounter;
extern PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;
extern PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex;
extern PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName;
extern PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv;
extern PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding;

#define GL_ACTIVE_RESOURCES				  0x92F5
#define GL_IMAGE_2D                       0x904D
#define GL_READ_ONLY                      0x88B8
#define GL_READ_WRITE                     0x88BA
#define GL_WRITE_ONLY					  0x88B9
#define GL_COMPUTE_SHADER				  0x91B9
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_PATCHES                        0x000E
#define GL_PATCH_VERTICES                 0x8E72
#define GL_SHADER_STORAGE_BUFFER		  0x90D2
#define GL_SHADER_STORAGE_BLOCK			  0x92E6
#define GL_TIMESTAMP					  0x8E28
#endif

// OpenGL CPUT layer
//-----------------------------------------------------------------------------
class CPUT_OGL;
extern CPUT_OGL *gpSample;

class CPUT_OGL : public CPUT
{
public:
    static const char* GLSL_VERSION;
    static CPUT_SHADER_MACRO *DEFAULT_MACROS; 

protected:

#ifdef CPUT_OS_WINDOWS
    CPUTWindowWin             *mpWindow;
#elif defined  CPUT_OS_LINUX
    CPUTWindowX               *mpWindow;
#elif defined  CPUT_OS_ANDROID
    CPUTWindowAndroid         *mpWindow;

#else
#error "No OS"
#endif

    bool           mbShutdown;
    CPUTBuffer    *mpPerFrameConstantBuffer;
    CPUTBuffer    *mpPerModelConstantBuffer;
    CPUTBuffer    *mpSkinningDataConstantBuffer;
    vector<string> extensions;                     // Table of supported extensions

#ifdef CPUT_FOR_OGL
	HDC                 mhDC;                      // Application Device Context
    HGLRC               mhRC;                      // OpenGL Rendering Contexts per worker thread
#endif
#ifdef CPUT_FOR_OGLES
    EGLDisplay          display;                   // Display device
    EGLSurface          surface;                   // Drawing surface
    EGLContext          context;                   // OpenGL Rendering Contexts
    int              width;
    int              height;  
#endif
public:
    CPUT_OGL(); 
    virtual ~CPUT_OGL();

    virtual void CreateResources();
    virtual void ReleaseResources();
    // context creation/destruction routines
#ifdef CPUT_OS_ANDROID
    static int32_t cput_handle_input(struct android_app* app, AInputEvent* event);
    ndk_helper::DoubletapDetector mDoubletapDetector;
    ndk_helper::PinchDetector     mPinchDetector;
    ndk_helper::DragDetector      mDragDetector;
#else
    HWND CreateDummyWindow(HINSTANCE hInst);
#endif
    bool supportExtension(const string name);

    int CPUTMessageLoop();
    CPUTResult CPUTCreateWindowAndContext(const std::string WindowTitle, CPUTWindowCreationParams windowParams);

    // CPUT interfaces
    virtual void ResizeWindow(UINT width, UINT height);
    virtual void ResizeWindowSoft(UINT width, UINT height);
    void DeviceShutdown();

    void UpdatePerFrameConstantBuffer( CPUTRenderParameters &renderParams, double totalSeconds );

    // events
    virtual void Update(double deltaSeconds) {}
    virtual void Present() 
    { 
// This can be also divided into separate functions, 
// see proposition of interface design in CPUTWindowWin.h
#if   defined( CPUT_OS_LINUX )
        mpWindow->Present();
    // linux stuff here from CPUTWindowLin.cpp 
#elif defined( CPUT_OS_WINDOWS )
#if   defined( CPUT_FOR_OGL )
    SwapBuffers( mhDC );		 
#elif defined( CPUT_FOR_DX11 )
    // DX11 display
#elif defined( CPUT_FOR_OGLES3 )
    // DX11 display
	 eglSwapBuffers( display, surface);
#endif
#else
#if   defined( CPUT_FOR_OGL )
    SwapBuffers( mhDC );		 
#elif defined( CPUT_FOR_OGLES3 )
    // DX11 display
	 eglSwapBuffers( display, surface);
#else
#error "Need to add something here"
#endif
#endif
    }
    
    bool HasWindow()
    {
        return (mpWindow != NULL);
    }

    virtual void Render(double deltaSeconds) = 0;
    virtual void Create()=0;
    virtual void Shutdown();
    virtual void FullscreenModeChange(bool bFullscreen) {UNREFERENCED_PARAMETER(bFullscreen);} // fires when CPUT changes to/from fullscreen mode
    virtual void ReleaseSwapChain() {}
    // virtual void ResizeWindow(UINT width, UINT height){UNREFERENCED_PARAMETER(width);UNREFERENCED_PARAMETER(height);}
    virtual CPUTResult CreateContext();
    virtual void SetSyncInterval(unsigned int syncInterval);

    // GUI
    void CPUTDrawGUI();

    // Event Handling
    CPUTEventHandledCode CPUTHandleKeyboardEvent(CPUTKey key, CPUTKeyState state);
    CPUTEventHandledCode CPUTHandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);

    // Utility functions for the sample developer
    CPUTResult CPUTToggleFullScreenMode();
    void CPUTSetFullscreenState(bool bIsFullscreen);
    bool CPUTGetFullscreenState();
    CPUTGuiController* CPUTGetGuiController();

    // Message boxes
    void CPUTMessageBox(const std::string DialogBoxTitle, const std::string DialogMessage);

protected:
    // private helper functions
    bool TestContextForRequiredFeatures();
    void ShutdownAndDestroy();
    virtual CPUTResult CreateOGLContext(CPUTContextCreation ContextParams);   // allow user to override DirectX context creation
    virtual CPUTResult DestroyOGLContext();  // allow user to override DirectX context destruction
    CPUTResult         CreateWindowAndContext(const std::string WindowTitle, CPUTWindowCreationParams windowParams);
    CPUTResult         MakeWindow(const std::string WindowTitle, CPUTWindowCreationParams windowParams);
    void               DrawLoadingFrame();

    // TODO: Put this somewhere else
//    bool               FindMatchingInputSlot(const char *pInputSlotName, const ID3DBlob *pVertexShaderBlob);
};

#endif //#ifndef __CPUT_DX11_H__
