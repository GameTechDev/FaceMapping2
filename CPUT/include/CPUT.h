/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __CPUTBASE_H__
#define __CPUTBASE_H__

// OpenGL
#ifdef CPUT_OS_ANDROID
#include <jni.h>
#include <errno.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <../middleware/ndk_helper/gestureDetector.h>
#if defined CPUT_FOR_OGLES3
#include "GLES3/gl3.h"                // OpenGL ES 3.0
#include "GLES3/gl3platform.h"        // OpenGL ES 3.0 Platform-Dependent Macros
#include <GLES2/gl2ext.h>             // OpenGL ES 2.0 Extensions 
#else
#error "No GLES version defined for Android"
#endif

#define CPUT_USE_ANDROID_ASSET_MANAGER // comment this out to use standard c/c++ file access in the external storage area

#endif


#if defined( CPUT_OS_WINDOWS ) && defined( CPUT_FOR_OGL )
#include <windows.h>  
#include <GL/glew.h>
#include <GL/gl.h>                    // OpenGL
#include "glext.h"                    // OpenGL Extensions 
#include "wglext.h"                   // OpenGL Windows Extensions 

extern PFNWGLSWAPINTERVALEXTPROC           wglSwapIntervalEXT;
extern PFNWGLGETSWAPINTERVALEXTPROC        wglGetSwapIntervalEXT;

#endif

#if defined( CPUT_OS_WINDOWS ) && defined( CPUT_FOR_OGLES3 )
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h> 
#endif

#ifdef CPUT_OS_LINUX
#include <GL/glew.h>
#include <GL/glx.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <assert.h>
#include <stdint.h>
#include <cstring>
#include "CPUTMath.h"
#include "CPUTEventHandler.h"
#include "CPUTCallbackHandler.h"
#include "CPUTTimer.h"

#ifdef CPUT_GPA_INSTRUMENTATION
// For D3DPERF_* calls, you also need d3d9.lib included
#include <d3d9.h>               // required for all the pix D3DPERF_BeginEvent()/etc calls
#include <ittnotify.h>
#include "CPUTITTTaskMarker.h"  // markup helper for GPA Platform Analyzer tags
#include "CPUTPerfTaskMarker.h" // markup helper for GPA Frame Analyzer tags

// GLOBAL instrumentation junk
enum CPUT_GPA_INSTRUMENTATION_STRINGS{
    GPA_HANDLE_CPUT_CREATE = 0,
    GPA_HANDLE_CONTEXT_CREATION,
    GPA_HANDLE_SYSTEM_INITIALIZATION,
    GPA_HANDLE_MAIN_MESSAGE_LOOP,
    GPA_HANDLE_EVENT_DISPATCH_AND_HANDLE,
    GPA_HANDLE_LOAD_SET,
    GPA_HANDLE_LOAD_MODEL,
    GPA_HANDLE_LOAD_MATERIAL,
    GPA_HANDLE_LOAD_TEXTURE,
    GPA_HANDLE_LOAD_CAMERAS,
    GPA_HANDLE_LOAD_LIGHTS,
    GPA_HANDLE_LOAD_VERTEX_SHADER,
    GPA_HANDLE_LOAD_GEOMETRY_SHADER,
    GPA_HANDLE_LOAD_PIXEL_SHADER,
    GPA_HANDLE_DRAW_GUI,
    GPA_HANDLE_STRING_ENUMS_SIZE,
};
#endif // CPUT_GPA_INSTRUMENTATION

#ifndef nullptr
#define nullptr NULL
#endif

// Integer types
typedef signed   char      sint8;
typedef unsigned char      uint8;
typedef signed   short     sint16;
typedef unsigned short     uint16;
typedef signed   int       sint32;
typedef unsigned int       uint32;
typedef signed   long long sint64;
typedef unsigned long long uint64;

#ifdef CPUT_FOR_DX11
//
// Include all DX11 headers needed
//
// Visual Studio 2012 only includes windows 8.0 sdk which is 11.1. If you want
// to develop an 11.2 application either update to 2013 or modify the paths to point to the 8.1 sdk.
//
#if _MSC_VER >= 1800
#include <d3d11_2.h>
#else
#include <d3d11_1.h>
#endif
#include <D3DCompiler.h>    // for D3DReflect() / D3DX11Refection - IMPORTANT NOTE: include directories MUST list DX SDK include path BEFORE
// Windows include paths or you'll get compile errors with D3DShader.h
#endif

// context creation parameters
struct CPUTContextCreation
{
#ifdef CPUT_FOR_DX11
    int refreshRate;
    int swapChainBufferCount;
    DXGI_FORMAT swapChainFormat;
    DXGI_USAGE swapChainUsage;
	int swapChainSampleCount;
	CPUTContextCreation():  refreshRate(60), 
                            swapChainBufferCount(1), 
                            swapChainFormat(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB), 
                            swapChainUsage(DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT),
                            swapChainSampleCount(1){}
#endif
#if (defined(CPUT_FOR_OGL) || defined(CPUT_FOR_OGLES))
	int samples;
	CPUTContextCreation():samples(1){}
#endif
};

// window creation parameters
struct CPUTWindowCreationParams
{
    bool startFullscreen;
    int windowWidth;
    int windowHeight;
    int windowPositionX;
    int windowPositionY;
    int samples;
    CPUTContextCreation deviceParams;
    CPUTWindowCreationParams() : startFullscreen(false), windowWidth(1280), windowHeight(720), windowPositionX(0), windowPositionY(0), samples(1) {}
};

// TODO: Add here compile time preprocessor checks to validate sizes of variables!

// Heap corruption, ASSERT, and TRACE defines
//-----------------------------------------------------------------------------
void DEBUG_PRINT(const char *pData, ...);

#if _DEBUG
    #define TRACE(String)  {DEBUG_PRINT(String);}
    #define ASSERT(Condition, Message) { if( !(Condition) ) { std::string msg = std::string(__FUNCTION__) + ":  " + Message; DEBUG_PRINT(msg.c_str());} assert(Condition);} 
#ifdef CPUT_OS_WINDOWS
    #define HEAPCHECK     { int  heapstatus = _heapchk(); ASSERT(_HEAPOK == heapstatus, "Heap corruption" ); }
#define DEBUGMESSAGEBOX(Title, Text) { CPUTOSServices::OpenMessageBox(Title, Text);}
#else
#define DEBUGMESSAGEBOX(Title, Text) 
#define HEAPCHECK
#endif
#else
    #define ASSERT(Condition, Message)
    #define TRACE(String)
    #define DEBUGMESSAGEBOX(Title, Text){ }
    #define HEAPCHECK
#endif // _DEBUG

//
//class CPUTFuncPrint
//{
//public:
//    CPUTFuncPrint(const char * func, const char * file, const int line) : mpFunc(func), mpFile(file), mLine(line)
//    {
//        DEBUG_PRINT("Entering function %s (%s:%d)" + mpFunc + mpFile + mLine);
//        //DEBUG_PRINT("Entering function %s (%s:%d)", mpFunc, mpFile, mLine);
//    }
//    
//    virtual ~CPUTFuncPrint()
//    {
//        DEBUG_PRINT("Exiting function %s (%s:%d)", mpFunc, mpFile, mLine);
//    }
//          
//protected:
//    const char * mpFunc;
//    const char * mpFile;
//    const int    mLine;
//};
//
//#define FUNC_ENTRY CPUTFuncPrint CPUTFP(__FUNCTION__, __FILE__, __LINE__)

// Error codes
//-----------------------------------------------------------------------------
typedef enum CPUTResult
{
    // success
    CPUT_SUCCESS = 0x00000000,

    // warnings
//    CPUT_WARNING_OUT_OF_RANGE,
    CPUT_WARNING_NOT_FOUND,
//    CPUT_WARNING_ALREADY_EXISTS,
//    CPUT_WARNING_FILE_IN_SEARCH_PATH_BUT_NOT_WHERE_SPECIFIED,
//    CPUT_WARNING_PHONG_SHADER_MISSING_TEXTURE,
    CPUT_WARNING_CANCELED,
//    CPUT_WARNING_NO_SUITABLE_FORMAT_FOUND,
//
    CPUT_WARNING_SHADER_INPUT_SLOT_NOT_MATCHED,
    CPUT_WARNING_NO_ASSETS_LOADED,
//
// just an error
//
    CPUT_ERROR = 0xF0000000,
//
//    // file errors
    CPUT_ERROR_FILE_NOT_FOUND = 0xF0000001,
    CPUT_ERROR_FILE_READ_ERROR = CPUT_ERROR_FILE_NOT_FOUND+1,
    CPUT_ERROR_FILE_CLOSE_ERROR = CPUT_ERROR_FILE_NOT_FOUND+2,
    CPUT_ERROR_FILE_IO_ERROR = CPUT_ERROR_FILE_NOT_FOUND+3,
    CPUT_ERROR_FILE_NO_SUCH_DEVICE_OR_ADDRESS = CPUT_ERROR_FILE_NOT_FOUND+4,
    CPUT_ERROR_FILE_BAD_FILE_NUMBER = CPUT_ERROR_FILE_NOT_FOUND+5,
    CPUT_ERROR_FILE_NOT_ENOUGH_MEMORY = CPUT_ERROR_FILE_NOT_FOUND+6,
    CPUT_ERROR_FILE_PERMISSION_DENIED = CPUT_ERROR_FILE_NOT_FOUND+7,
    CPUT_ERROR_FILE_DEVICE_OR_RESOURCE_BUSY = CPUT_ERROR_FILE_NOT_FOUND+8,
    CPUT_ERROR_FILE_EXISTS = CPUT_ERROR_FILE_NOT_FOUND+9,
    CPUT_ERROR_FILE_IS_A_DIRECTORY = CPUT_ERROR_FILE_NOT_FOUND+10,
    CPUT_ERROR_FILE_TOO_MANY_OPEN_FILES = CPUT_ERROR_FILE_NOT_FOUND+11,
    CPUT_ERROR_FILE_TOO_LARGE = CPUT_ERROR_FILE_NOT_FOUND+12,
    CPUT_ERROR_FILE_DEVICE_FULL = CPUT_ERROR_FILE_NOT_FOUND+13,
    CPUT_ERROR_FILE_FILENAME_TOO_LONG = CPUT_ERROR_FILE_NOT_FOUND+14,
    CPUT_ERROR_FILE_PATH_ERROR = CPUT_ERROR_FILE_NOT_FOUND+15,
    CPUT_ERROR_FILE_ERROR = CPUT_ERROR_FILE_NOT_FOUND+16,
//
//    CPUT_ERROR_DIRECTORY_NOT_FOUND = CPUT_ERROR_FILE_NOT_FOUND+21,
//
//    // subsystem errors
    CPUT_ERROR_INVALID_PARAMETER = 0xF0000100,
    CPUT_ERROR_NOT_FOUND = CPUT_ERROR_INVALID_PARAMETER+1,
    CPUT_ERROR_NOT_IMPLEMENTED = CPUT_ERROR_INVALID_PARAMETER + 2,
//    CPUT_ERROR_COMPONENT_NOT_INITIALIZED = CPUT_ERROR_INVALID_PARAMETER+2,
//    CPUT_ERROR_SUBSYSTEM_OUT_OF_MEMORY = CPUT_ERROR_INVALID_PARAMETER+3,
//    CPUT_ERROR_OUT_OF_BOUNDS = CPUT_ERROR_INVALID_PARAMETER+4,
//    CPUT_ERROR_HEAP_CORRUPTION = CPUT_ERROR_INVALID_PARAMETER+5,
//
//    // image format errors
    CPUT_ERROR_UNSUPPORTED_IMAGE_FORMAT = 0xF0000200,
//    CPUT_ERROR_ERROR_LOADING_IMAGE = CPUT_ERROR_UNSUPPORTED_IMAGE_FORMAT+1,
    CPUT_ERROR_UNSUPPORTED_SRGB_IMAGE_FORMAT,
//
//    // shader loading errors
//    CPUT_SHADER_LOAD_ERROR = 0xF0000300,
//    CPUT_SHADER_COMPILE_ERROR = CPUT_SHADER_LOAD_ERROR+1,
//    CPUT_SHADER_LINK_ERROR = CPUT_SHADER_LOAD_ERROR+2,
//    CPUT_SHADER_REGISTRATION_ERROR = CPUT_SHADER_LOAD_ERROR+3,
//    CPUT_SHADER_CONSTANT_BUFFER_ERROR = CPUT_SHADER_LOAD_ERROR+4,
//    CPUT_SHADER_REFLECTION_ERROR = CPUT_SHADER_LOAD_ERROR+5,
//
//    // texture loading errors
    CPUT_TEXTURE_LOAD_ERROR = 0xF0000400,
    CPUT_ERROR_TEXTURE_FILE_NOT_FOUND = CPUT_TEXTURE_LOAD_ERROR+1,
//
//    // GUI errors
    CPUT_GUI_GEOMETRY_CREATION_ERROR = 0xF0000500,
//    CPUT_GUI_SAMPLER_CREATION_ERROR = CPUT_GUI_GEOMETRY_CREATION_ERROR+1,
//    CPUT_GUI_TEXTURE_CREATION_ERROR = CPUT_GUI_GEOMETRY_CREATION_ERROR+2,
//    CPUT_GUI_CANNOT_CREATE_CONTROL = CPUT_GUI_GEOMETRY_CREATION_ERROR+3,
    CPUT_GUI_INVALID_CONTROL_ID = CPUT_GUI_GEOMETRY_CREATION_ERROR+4,
//
//    // Texture loading errors
//    CPUT_FONT_TEXTURE_TYPE_ERROR = 0xF0000600,
//    CPUT_FONT_TEXTURE_LOAD_ERROR = CPUT_FONT_TEXTURE_TYPE_ERROR+1,
//
//    // Model loading errors
//    CPUT_ERROR_MODEL_LOAD_ERROR = 0xF0000650,
//    CPUT_ERROR_MODEL_FILE_NOT_FOUND = CPUT_ERROR_MODEL_LOAD_ERROR+1,
//
//    // Shader errors
    CPUT_ERROR_VERTEX_LAYOUT_PROBLEM = 0xF0000700,
//    CPUT_ERROR_VERTEX_BUFFER_CREATION_PROBLEM = CPUT_ERROR_VERTEX_LAYOUT_PROBLEM+1,
//    CPUT_ERROR_INDEX_BUFFER_CREATION_PROBLEM = CPUT_ERROR_VERTEX_LAYOUT_PROBLEM+2,
//    CPUT_ERROR_UNSUPPORTED_VERTEX_ELEMENT_TYPE = CPUT_ERROR_VERTEX_LAYOUT_PROBLEM+3,
//    CPUT_ERROR_INDEX_BUFFER_LAYOUT_PROBLEM = CPUT_ERROR_VERTEX_LAYOUT_PROBLEM+4,
    CPUT_ERROR_SHADER_INPUT_SLOT_NOT_MATCHED = CPUT_ERROR_VERTEX_LAYOUT_PROBLEM+5,
//
//
//    // Context creation errors
//    CPUT_ERROR_CONTEXT_CREATION_FAILURE = 0xF0000C00,
//    CPUT_ERROR_SWAP_CHAIN_CREATION_FAILURE = CPUT_ERROR_CONTEXT_CREATION_FAILURE+1,
//    CPUT_ERROR_RENDER_TARGET_VIEW_CREATION_FAILURE = CPUT_ERROR_CONTEXT_CREATION_FAILURE+2,
//
//    // Depth buffer errors
//    CPUT_ERROR_DEPTH_BUFFER_CREATION_ERROR = 0xF0000800,
//    CPUT_ERROR_DEPTH_STENCIL_BUFFER_CREATION_ERROR = CPUT_ERROR_DEPTH_BUFFER_CREATION_ERROR+1,
//    CPUT_ERROR_RASTER_STATE_CREATION_ERROR = CPUT_ERROR_DEPTH_BUFFER_CREATION_ERROR+2,
//
//    // GUI shaders
    CPUT_ERROR_INITIALIZATION_GUI_VERTEX_SHADER_NOT_FOUND = 0xF0000130,
    CPUT_ERROR_INITIALIZATION_GUI_PIXEL_SHADER_NOT_FOUND = CPUT_ERROR_INITIALIZATION_GUI_VERTEX_SHADER_NOT_FOUND+1,
    CPUT_ERROR_INITIALIZATION_GUI_CONTROL_TEXTURES_NOT_FOUND = CPUT_ERROR_INITIALIZATION_GUI_VERTEX_SHADER_NOT_FOUND+2,
//
//    // gfx system errors
//    CPUT_ERROR_GFX_SUBSYSTEM_BUSY = 0xF0000B00,
//    CPUT_ERROR_GFX_SUBSYSTEM_TO_MANY_OBJECTS = CPUT_ERROR_GFX_SUBSYSTEM_BUSY+1,
//
//    // window layer errors
    CPUT_ERROR_WINDOW_CANNOT_REGISTER_APP = 0xF0000D00,
    CPUT_ERROR_WINDOW_ALREADY_EXISTS = CPUT_ERROR_WINDOW_CANNOT_REGISTER_APP+1,
//    CPUT_ERROR_CANNOT_GET_WINDOW_CLASS = CPUT_ERROR_WINDOW_CANNOT_REGISTER_APP+3,
    CPUT_ERROR_CANNOT_GET_WINDOW_INSTANCE = CPUT_ERROR_WINDOW_CANNOT_REGISTER_APP+4,
//    CPUT_ERROR_WINDOW_OS_PROPERTY_GET_ERROR = CPUT_ERROR_WINDOW_CANNOT_REGISTER_APP+5,
//
//    // AssetLibrary/AssetSet errors
    CPUT_ERROR_ASSET_LIBRARY_INVALID_LIBRARY = 0xF0000E00,
//    CPUT_ERROR_ASSET_SET_INVALID_TYPE = CPUT_ERROR_ASSET_LIBRARY_INVALID_LIBRARY+1,
//    CPUT_ERROR_ASSET_LIBRARY_OBJECT_NOT_FOUND,
//    CPUT_ERROR_ASSET_ALREADY_EXISTS = CPUT_ERROR_ASSET_LIBRARY_INVALID_LIBRARY+3,
//
//    // Paramter block errors.
    CPUT_ERROR_PARAMETER_BLOCK_NOT_FOUND = 0xF0000F00,
//
//    // misc errors
//    CPUT_ERROR_FULLSCREEN_SWITCH_ERROR = 0xF0000F00,
} CPUTResult;

static int gRefCount = 0;
//handy defines
//-----------------------------------------------------------------------------
#define SAFE_RELEASE(p)     {if((p)){HEAPCHECK; gRefCount = (p)->Release(); (p)=NULL; HEAPCHECK;} }
#define SAFE_DELETE(p)      {if((p)){HEAPCHECK; delete (p);     (p)=NULL;HEAPCHECK; }}
#define SAFE_DELETE_ARRAY(p){if((p)){HEAPCHECK; delete[](p);    (p)=NULL;HEAPCHECK; }}
#define SAFE_FREE(p)      {if((p)){HEAPCHECK; free(p);     (p)=NULL;HEAPCHECK; }}
#define UNREFERENCED_PARAMETER(P) (P)

// CPUT data types
//-----------------------------------------------------------------------------
#define CPUTSUCCESS(returnCode) ((returnCode) < 0xF0000000)
#define CPUTFAILED(returnCode) ((returnCode) >= 0xF0000000)


//typedef UINT CPUTResult;
typedef unsigned int UINT;
//typedef unsigned long DWORD;

struct CPUTHSLColor
{
	float h;
	float s;
	float l;
	CPUTHSLColor()
	{
	}
	CPUTHSLColor( float hue, float saturation, float lightness)
	{
		h = hue;
		s = saturation;
		l = lightness;
	}
	float3 ToFloat3()
	{
		return float3(h,s,l);
	}
	bool operator == (const CPUTHSLColor& rhs) const
	{
		return((rhs.h == h) &&
			(rhs.s == s) &&
			(rhs.l == l));
	}
	bool operator != (const CPUTHSLColor& rhs) const
	{
		return (rhs != *this);
	}
	void GetUserFriendlyValues(int *hue, int *saturation, int *lightness)
	{
		*hue = (int)(h * 360.0f);
		*saturation = (int)(s * 100);
		*lightness = (int)(l * 100);
	}
	void SetUserFriendlyValues(int hue, int saturation, int lightness)
	{
		h = (float)hue / 360.0f;
		s = (float)saturation / 100.0f;
		l = (float)lightness / 100.0f;
	}
};

// color
struct CPUTColor4
{
    float r;
    float g;
    float b;
    float a;

	CPUTColor4()
	{
	}

	CPUTColor4(float r, float g, float b, float a){
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

    bool operator == (const CPUTColor4& rhs) const
    {
        return((rhs.r == r) && 
               (rhs.g == g) &&
               (rhs.b == b) &&
               (rhs.a == a));
    }
    bool operator != (const CPUTColor4& rhs) const
    {
        return((rhs.r != r) || 
               (rhs.g != g) ||
               (rhs.b != b) ||
               (rhs.a != a));
    }

	float4 ToFloat4()
	{
		return float4(r, g, b, a);
	}
};

inline CPUTColor4 CPUTColorFromBytes(byte r, byte g, byte b, byte a)
{
	return CPUTColor4(((float)r / 255.0f), ((float)g / 255.0f), ((float)b / 255.0f), ((float)a / 255.0f));
}

// where the loader should start looking from to locate files
enum CPUT_PATH_SEARCH_MODE
{
    CPUT_PATH_SEARCH_RESOURCE_DIRECTORY,
    CPUT_PATH_SEARCH_NONE,
};

// string size limitations
const UINT CPUT_MAX_PATH = 2048;
const UINT CPUT_MAX_STRING_LENGTH = 1024;
const UINT CPUT_MAX_SHADER_ERROR_STRING_LENGTH = 8192;
const UINT CPUT_MAX_DIGIT_STRING_LENGTH = 5;



// Data format types used in interpreting mesh data
enum CPUT_DATA_FORMAT_TYPE
{
    CPUT_UNKNOWN=0,

    CPUT_DOUBLE=1,
    CPUT_F32=2,

    CPUT_U64=3,
    CPUT_I64=4,

    CPUT_U32=5,
    CPUT_I32=6,

    CPUT_U16=7,
    CPUT_I16=8,

    CPUT_U8=9,
    CPUT_I8=10,

    CPUT_CHAR=11,
    CPUT_BOOL=12,
};

// Corresponding sizes (in bytes) that match CPUT_DATA_FORMAT_TYPE
const int CPUT_DATA_FORMAT_SIZE[] =
{
        0, //CPUT_UNKNOWN=0,
        
        8, //CPUT_DOUBLE,
        4, //CPUT_F32,
        
        8, //CPUT_U64,
        8, //CPUT_I64,
        
        4, //CPUT_U32,
        4, //CPUT_I32,
        
        2, //CPUT_U16,
        2, //CPUT_I16,
        
        1, //CPUT_U8,
        1, //CPUT_I8,

        1, //CPUT_CHAR
        1, //CPUT_BOOL
};

//-----------------------------------------------------------------------------
enum eCPUTMapType
{
    CPUT_MAP_UNDEFINED = 0,
    CPUT_MAP_READ = 1,
    CPUT_MAP_WRITE = 2,
    CPUT_MAP_READ_WRITE = 3,
    CPUT_MAP_WRITE_DISCARD = 4,
    CPUT_MAP_NO_OVERWRITE = 5
};

// C++ std::to_string is currently not supported in the NDK toolchain
#ifdef CPUT_OS_ANDROID
inline std::string cput_to_string(int integer) 
{
    std::stringstream sstream;
    sstream << integer;
    return sstream.str();
}
#else
#define cput_to_string std::to_string
#endif

// convert pointer to string
//-----------------------------------------------------------------------------
inline std::string ptoc(const void *pPointer)
{
    std::ostringstream stream;

    stream << pPointer;

    std::string address;
    address = stream.str();

    return address;
}


typedef struct CPUT_SHADER_MACRO
{
    char *Name;
    char *Definition;
} CPUT_SHADER_MACRO;

char* ConvertShaderMacroToChar(CPUT_SHADER_MACRO *pShaderMacros);

#include "CPUTRenderTarget.h"
#ifdef CPUT_FOR_DX11
#include "CPUTRenderTarget.h"
#elif defined(CPUT_FOR_OGL)
//#include "CPUTRenderTargetOGL.h"
#else    
//#error You must supply a target graphics API (ex: #define CPUT_FOR_DX11), or implement the target API for this file.
//this should cause an error
#endif

class CPUTCamera;
class CPUTRenderStateBlock;

// CPUT class
//-----------------------------------------------------------------------------
class CPUT:public CPUTEventHandler, public CPUTCallbackHandler
{
protected:
    CPUTCamera  *mpCamera;
    CPUTCamera  *mpShadowCamera;
    CPUTTimer   *mpTimer;
    
    CPUTBuffer  *mpBackBuffer;
    CPUTBuffer  *mpDepthBuffer;
    CPUTTexture *mpBackBufferTexture;
    CPUTTexture *mpDepthBufferTexture;

public:

	float4       mLightColor; // TODO: Get from light(s)
	float4       mAmbientColor;

    CPUT() :
        mpCamera(NULL),
        mpShadowCamera(NULL),
        mAmbientColor(0.3f, 0.3f, 0.3f, 1.0f),
        mLightColor(0.7f, 0.7f, 0.7f, 1.0f),
        mpBackBuffer(NULL),
        mpDepthBuffer(NULL),
        mpBackBufferTexture(NULL),
        mpDepthBufferTexture(NULL)

	{}
    virtual ~CPUT() {};
    virtual void CreateResources() =0;
    virtual void ReleaseResources() =0;
    virtual void Update(double deltaSeconds) = 0;
    virtual void Present() = 0; 
    virtual void Render(double deltaSeconds) = 0;
    void InnerExecutionLoop() {
#ifdef CPUT_GPA_INSTRUMENTATION
        D3DPERF_BeginEvent(D3DCOLOR(0xff0000), L"CPUT User's Render() ");
#endif
        {
            double deltaSeconds = mpTimer->GetElapsedTime();
            Update(deltaSeconds);
            Present(); // Note: Presenting immediately before Rendering minimizes CPU stalls (i.e., execute Update() before Present() stalls)

            double totalSeconds = mpTimer->GetTotalTime();
            //CPUTMaterialDX11::ResetStateTracking();
            Render(deltaSeconds);
        }
#ifdef CPUT_GPA_INSTRUMENTATION
        D3DPERF_BeginEvent(D3DCOLOR(0xff0000), L"CPUT User's Render() ");
#endif
        {
            double deltaSeconds = mpTimer->GetElapsedTime();
            Update(deltaSeconds);
            Present(); // Note: Presenting immediately before Rendering minimizes CPU stalls (i.e., execute Update() before Present() stalls)

            double totalSeconds = mpTimer->GetTotalTime();
            Render(deltaSeconds);
        }

#ifdef CPUT_GPA_INSTRUMENTATION
        D3DPERF_EndEvent();
#endif
        ;}
    virtual void ResizeWindowSoft(UINT width, UINT height) {UNREFERENCED_PARAMETER(width);UNREFERENCED_PARAMETER(height);}
    virtual void ResizeWindow(UINT width, UINT height) {
#ifdef CPUT_FOR_DX11
        CPUTRenderTargetColor::SetActiveWidthHeight( width, height );
        CPUTRenderTargetDepth::SetActiveWidthHeight( width, height );
#endif
    }
    virtual void DeviceShutdown(){}

    virtual CPUTEventHandledCode CPUTHandleKeyboardEvent(CPUTKey key, CPUTKeyState state) {UNREFERENCED_PARAMETER(key);return CPUT_EVENT_UNHANDLED;}
    virtual CPUTEventHandledCode CPUTHandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message) {UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);UNREFERENCED_PARAMETER(wheel);UNREFERENCED_PARAMETER(state);return CPUT_EVENT_UNHANDLED;}

    virtual void SetSyncInterval(unsigned int syncInterval) = 0;
};


void CPUTSetDebugName( void *pResource, std::string name );

struct CPUTModelConstantBuffer
{
    float4x4 World;
    float4x4 NormalMatrix;
    float4x4 WorldViewProjection;
    float4x4 InverseWorld;
    float4x4 LightWorldViewProjection;
    float4  BoundingBoxCenterWorldSpace;
    float4  BoundingBoxHalfWorldSpace;
    float4  BoundingBoxCenterObjectSpace;
    float4  BoundingBoxHalfObjectSpace;
	float4  UserData1;
	float4  UserData2;
	float4  UserData3;
};

struct CPUTAnimationConstantBuffer
{
    float4x4    SkinMatrix[255];
    float4x4    SkinNormalMatrix[255];     
};

struct CPUTFrameConstantBuffer
{
    float4x4  View;
    float4x4  InverseView;
    float4x4  Projection;
    float4x4  ViewProjection;
    float4    AmbientColor;
    float4    LightColor;
    float4    LightDirection;
    float4    EyePosition;
    float4    TotalSeconds;
};

#endif // #ifndef __CPUTBASE_H__
