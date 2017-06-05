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
#include "CPUTGuiController.h"

//#include "CPUTBufferOGL.h"
//#include "CPUTTextureOGL.h"

#ifdef CPUT_OS_WINDOWS
#define OSSleep Sleep
#else
#include <unistd.h>
#define OSSleep sleep
#endif

#include <iostream>

#ifdef CPUT_OS_LINUX
CPUTResult CPUT_OGL::CreateOGLContext(CPUTContextCreation ContextParams )
{
    return CPUT_SUCCESS;
}

CPUTResult CPUT_OGL::DestroyOGLContext(void)
{
    return CPUT_SUCCESS;
}

#else
// WGL_ARB_create_context
PFNWGLCREATECONTEXTATTRIBSARBPROC   wglCreateContextAttribsARB   = nullptr;

// WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = nullptr;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB      = nullptr;

// WGL_EXT_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivEXT = nullptr;
PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvEXT = nullptr;
PFNWGLCHOOSEPIXELFORMATEXTPROC      wglChoosePixelFormatEXT      = nullptr;

// WGL_ARB_extensions_string 
PFNWGLGETEXTENSIONSSTRINGARBPROC    wglGetExtensionsStringARB    = nullptr;

// WGL_EXT_extensions_string 
PFNWGLGETEXTENSIONSSTRINGEXTPROC    wglGetExtensionsStringEXT    = nullptr;

PFNWGLSWAPINTERVALEXTPROC           wglSwapIntervalEXT           = nullptr;
PFNWGLGETSWAPINTERVALEXTPROC        wglGetSwapIntervalEXT        = nullptr;

// Advanced Pixel Format Descriptor
float  fAttributes[] = {0, 0};
uint32 iAttributes[] = { WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, //   - Hardware accelerated 
                         WGL_SUPPORT_OPENGL_ARB, TRUE,                    // _ - OpenGL supporting
                         WGL_DOUBLE_BUFFER_ARB, TRUE,                     //  |_ DOUBLE BUFFER - By exchange 
                         WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,      // _| 
                         WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,           //  |
                         WGL_COLOR_BITS_ARB, 32,                          //  +- COLOR - Minimum 32bit RGBA (R8G8B8A8)
                         WGL_RED_BITS_ARB,   8,                           //  |
                         WGL_GREEN_BITS_ARB, 8,                           //  |
                         WGL_BLUE_BITS_ARB,  8,                           //  |
                         WGL_ALPHA_BITS_ARB, 8,                           // _|
                         WGL_DEPTH_BITS_ARB, 24,                          //   - DEPTH   - Minimum 24bit
                         WGL_STENCIL_BITS_ARB, 8,                         //   - STANCIL - Minimum 8bit     
                         WGL_DRAW_TO_WINDOW_ARB, TRUE,                    //   - Can run in WINDOW mode
                         WGL_SAMPLES_ARB, 1,                              //   - MULTISAMPLING - 1,2,4,8,16 samples
                         WGL_EXT_framebuffer_sRGB, TRUE,
                         0, 0 };

// Legacy Pixel Format Descriptor 
const static PIXELFORMATDESCRIPTOR pfdLegacy = 
    { 
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW |            // Support window 
    PFD_SUPPORT_OPENGL |            // Support OpenGL double buffered RBGA surface
    PFD_DOUBLEBUFFER,               // _
    PFD_TYPE_RGBA,                  //  |  COLOR - Minimum 24bit RGB
    32,                             //  |  " Specifies the number of color bitplanes in each color buffer. 
    0, 0,                           //  +-   For RGBA pixel types, it is the size of the color buffer,
    0, 0,                           //  |    excluding the alpha bitplanes. " 
    0, 0,                           //  | ( http://msdn.microsoft.com/en-us/library/dd368826(v=vs.85).aspx )   
    0, 0,                           // -   So should be 24, but we set 32.
    0,                              // ACCUM     - off
    0, 0, 0, 0,                     // 
    24,                             // DEPTH     - 24bit Z-Buffer
    8,                              // STENCIL   - on
    0,                              // AUXILIARY - off
    PFD_MAIN_PLANE,                 // Main Drawing Layer - (obsolete iLayerType, equals 0)
    0,
    0, 0, 0 
    };


CPUTResult CPUT_OGL::DestroyOGLContext(void)
{
	HWND hWnd = mpWindow->GetHWnd();

       // Disabling and deleting all rendering contexts
    wglMakeCurrent( NULL, NULL ); 
    wglDeleteContext( mhRC );
    mhRC = NULL;
   
    // Disabling device context
    if ( mhDC )
    {
        ReleaseDC( hWnd, mhDC ); 
        mhDC = 0;
    }
	return CPUT_SUCCESS;
}
CPUTResult CPUT_OGL::CreateOGLContext(CPUTContextCreation ContextParams )
{
	HWND hWnd;

	hWnd = CreateDummyWindow( GetModuleHandle(NULL));
    // Create dummy window to gather information about OpenGL
    if ( ! hWnd) 
    {
        return CPUT_ERROR;
    }

    // To get list of supported extensions, this 
    // code should be used since OpenGL 3.0 Core:
    //
    // uint32 count = 0;
    // glGetIntegerv(GL_NUM_EXTENSIONS, (GLint*)&count);
    // for(uint16 i=0; i<count; i++)
    //    support.m_extensions.insert(support.m_extensions.end(),string((char*)glGetStringi(GL_EXTENSIONS,i));
    //
    // But data about gpu, are gathered in dummy
    // OpenGL context which works in deprecaded
    // mode. This forces implementation in old
    // deprecated way:
    
    // Creates table of supported extensions strings
    extensions.clear();
    string tmp;
    size_t begin, end;
    tmp   = string( (char*)glGetString( GL_EXTENSIONS ) );
    begin = 0;
    end   = tmp.find( ' ', 0 );
    while( end != string::npos )
    {
        extensions.insert( extensions.end(), tmp.substr( begin, end-begin ) );
        begin = end + 1;
        end   = tmp.find( ' ', begin );
    }
    
    // This extension together with WGL_EXT_swap_interval
    // should be returned in list of supported extensions
    // after calling glGetString(GL_EXTENSIONS);
    // Because NVidia drivers don't follow this rule it's
    // specification says as follow:
    // 
    // "Applications should call wglGetProcAddress to see 
    // whether or not wglGetExtensionsStringARB is supported."
    // ( http://www.opengl.org/registry/specs/ARB/wgl_extensions_string.txt )
    //
    bool wglexts = false;
    wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress( "wglGetExtensionsStringARB" );  
    if ( wglGetExtensionsStringARB != NULL )
    {
        tmp = string( (char*)wglGetExtensionsStringARB( mhDC ) );
        wglexts = true;
    }
    else
    {
        wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress( "wglGetExtensionsStringEXT" );
        if ( wglGetExtensionsStringEXT != NULL )
        {
            tmp = string( (char*)wglGetExtensionsStringEXT() ); 
            wglexts = true; 
        }
    }

    // If it is possible to obtain WGL extensions list add
    // them to the rest of supported extensions.
    if ( wglexts )
    {
        begin = 0;
        end   = tmp.find( ' ', 0 );
        while( end != string::npos )
        {
            extensions.insert( extensions.end(), tmp.substr( begin, end-begin ) );
            begin = end + 1;
            end   = tmp.find( ' ', begin );
        }
	}

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	// Get pointers to WGL specific functions that are required during window creation
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress( "wglCreateContextAttribsARB" );
    wglChoosePixelFormatARB    = (PFNWGLCHOOSEPIXELFORMATARBPROC)    wglGetProcAddress( "wglChoosePixelFormatARB" );
    wglChoosePixelFormatEXT    = (PFNWGLCHOOSEPIXELFORMATEXTPROC)    wglGetProcAddress( "wglChoosePixelFormatEXT" );
 
	// Set Samples count in Advanced Pixel Format
	iAttributes[27] = ContextParams.samples;

    // Choosing advanced Pixel Format in modern way 
    uint32 PixelFormat = 0;
    bool choosedPixelFormat = false;   
    if ( supportExtension( "WGL_ARB_pixel_format" ) )
    {
        uint32 numFormats;
        if ( wglChoosePixelFormatARB( mhDC, (const int*)iAttributes, fAttributes, 1, (int*)&PixelFormat, (unsigned int*)&numFormats ) )
            if ( numFormats >= 1 )
                choosedPixelFormat = true;
    }
    
    // Old way for choosing advanced Pixel Format
    if ( !choosedPixelFormat &&
         supportExtension( "WGL_EXT_pixel_format" ) )
    {
        uint32 numFormats;
        if ( wglChoosePixelFormatEXT( mhDC, (const int*)iAttributes, fAttributes, 1, (int*)&PixelFormat, (unsigned int*)&numFormats ) )
            if ( numFormats >= 1 )
                choosedPixelFormat = true;
    }

    // Basic Pixel Format
    if (!choosedPixelFormat) {
        PixelFormat = ChoosePixelFormat(mhDC, &pfdLegacy);
    }

    // If couldn't find Pixel Format report error
    if ( PixelFormat == NULL )
    {
        cerr << "Error! Cannot find aproprieate pixel format." << endl;	
        DestroyOGLContext();	
        return CPUT_ERROR;
    } 

    // Clear message queue
    MSG msg = { 0 };
    
    while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    
    // Data about OpenGL are gathered, proper Pixel Format is chosen.
    // Destroy Dummy Window and proceed to creation of proper window.
    // Disabling and deleting all rendering contexts
    wglMakeCurrent( NULL, NULL ); 
    wglDeleteContext( mhRC );
    mhRC = NULL;
    // Disabling device context
    ReleaseDC( hWnd, mhDC ); 
    mhDC = 0;
   
	// Deleting window
    DestroyWindow( hWnd );  

    // Unregistering window class
    if(TRUE != UnregisterClass( L"DummyWindowClass", GetModuleHandle(NULL) ))
	{
		return CPUT_ERROR;
	}

	hWnd = mpWindow->GetHWnd();

   
    // Acquiring Device Context
    mhDC = GetDC( hWnd );
    if ( mhDC == NULL )
    {
        cerr << "Error! Cannot create device context." << endl;		
        DestroyOGLContext();	
        return CPUT_ERROR;
    }  

    // Activating Pixel Format
    if ( !SetPixelFormat( mhDC, PixelFormat, &pfdLegacy ) )
    {	
        cerr << "Error! Cannot init pixel format." << endl;
        DestroyOGLContext();	
        return CPUT_ERROR;
	} 

    // OpenGL 4.0 Core profile settings
    uint32 glAttributes[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                              WGL_CONTEXT_MINOR_VERSION_ARB, 0,
                              WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                              GL_CONTEXT_PROFILE_MASK, GL_CONTEXT_CORE_PROFILE_BIT,
                              0, 0 };
   //         GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
     //   GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    // Debug version supports better debugging
#ifdef _DEBUG
    glAttributes[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif 

    // Try to create OpenGL context in new way
    mhRC = wglCreateContextAttribsARB( mhDC, 0, (const int *) &glAttributes );
    if ( mhRC == NULL )
    {
        cerr << "Error! Cannot create window rendering context." << endl;
        DestroyOGLContext();				
        return CPUT_ERROR;
    }  

    // Activating rendering context
    if( !wglMakeCurrent( mhDC, mhRC ) )
    {	
        std::cerr << "Error! Cannot activate rendering context." << endl;
        DestroyOGLContext();	
        return CPUT_ERROR;
    } 
    
    // Setting created window as current 
    ShowWindow( hWnd, SW_SHOW );
    SetForegroundWindow( hWnd );
    SetFocus( hWnd );
    
    // Link all OpenGL function pointers
    glewExperimental = TRUE;
    if ( glewInit() != GLEW_OK )
    {
        cerr << "Error! Cannot activate GLEW." << endl;
        DestroyOGLContext();	
        return CPUT_ERROR;
    }

    // GLEW's problem is that it calls glGetString(GL_EXTENSIONS) which causes 
    // GL_INVALID_ENUM on GL 3.2 core context as soon as glewInit() is called.
    // Clear errors indicator 
    glGetError();


    return CPUT_SUCCESS;
}



//
// WndProcDummy
// Handles the main message loop's events/messages for dummy window
//--------------------------------------------------------------------------
LRESULT CALLBACK WndProcDummy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Returning 0 is not enough because compiler will optimize this function
    // and remove it as function that doesn't have code. This instead will 
    // result in tun-time error because we cannot register window without its  
    // event handling function. So instead we do nothing for all important 
    // messages and pass rest of them to parent.
    switch (message)
    {
    case WM_COMMAND:
    case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_PAINT:
    case WM_SIZING:
    case WM_MOVING:
    case WM_ERASEBKGND:
    case WM_SIZE:
    case WM_EXITSIZEMOVE:
    case WM_DESTROY:
        break;

    default:
        // we don't handle it - pass it on thru to parent 
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// Creates dummy window with legacy OpenGL context just to gather info.
//-----------------------------------------------------------------------------

HWND CPUT_OGL::CreateDummyWindow(HINSTANCE hInst)
{
    WNDCLASS  Window;      // Window class
    uint32    PixelFormat; // Format of pixel
    
    // Window settings
    Window.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  // Have its own Device Context and also cannot be resized    
    Window.lpfnWndProc   = (WNDPROC) WndProcDummy;              // Procedure that handles OS evets
    Window.cbClsExtra    = 0;                                   // No extra window data
    Window.cbWndExtra    = 0;                                   //
    Window.hInstance     = hInst;                               // Handle to instance of program that this window belongs to
    Window.hIcon         = LoadIcon(NULL, IDI_WINLOGO);         // Load default icon
    Window.hCursor       = LoadCursor(NULL, IDC_ARROW);         // Load arrow pointer
    Window.hbrBackground = NULL;                                // No background (OpenGL will handle this)
    Window.lpszMenuName  = NULL;                                // No menu
    Window.lpszClassName = L"DummyWindowClass";                 // Class name
    
    // Registering Window Class
    if ( !RegisterClass( &Window ) )	
    {					
        cerr << "Error! Cannot register window class." << endl;
        return NULL;
    }

    // Create Dummy Window with OpenGL RC to gather required
    // information together with wgl functions pointers and
    // final Pixel Format number.
    HWND hWnd = CreateWindow(
        L"DummyWindowClass",           // Window class name
        L"CPUTFramework...",           // Title of window            
        WS_CLIPSIBLINGS |             // \_Prevents from overdrawing
        WS_CLIPCHILDREN |             // / by other windows
        WS_OVERLAPPEDWINDOW,          // Additional styles
        0, 0,                         // Position
        128,                          // True window widht
        128,                          // True window height
        NULL,                         // No parent window
        NULL,                         // No menu
        hInst,                        // Handle to this instance of program
        NULL );                       // Won't pass anything
    
    // Check if dummy window was created
    if ( hWnd == NULL )
    {
        cerr << "Error! Cannot create window." << endl;
        DestroyOGLContext();	
        return NULL;
    }  
    
    // Acquiring Device Context
    mhDC = GetDC( hWnd );
    if ( mhDC == NULL )
    {
        cerr << "Error! Cannot create device context." << endl;		
        DestroyOGLContext();	
        return NULL;
    }  
    
    // Choosing Pixel Format the old way
    PixelFormat = ChoosePixelFormat( mhDC, &pfdLegacy );
    if ( PixelFormat == NULL )
    {
        cerr << "Error! Cannot find aproprieate pixel format." << endl;	
        DestroyOGLContext();	
        return NULL;
    }  
    
    // Activating Pixel Format
    if ( !SetPixelFormat( mhDC, PixelFormat, &pfdLegacy ) )
    {	
        cerr << "Error! Cannot init pixel format." << endl;
        DestroyOGLContext();		
        return NULL;
    }  
    
    // Creating OpenGL Rendering Context
    mhRC = wglCreateContext( mhDC );
    if ( mhRC == NULL )
    {
        cerr << "Error! Cannot create window rendering context." << endl;
        DestroyOGLContext();						
        return NULL;
    }  
    
    // Activating rendering context
    if ( !wglMakeCurrent( mhDC, mhRC ) )
    {	
        cerr << "Error! Cannot activate rendering context." << endl;
        DestroyOGLContext();	
        return NULL;
    } 
    
    return hWnd;
}
#endif
