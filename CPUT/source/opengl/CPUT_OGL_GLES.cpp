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

#ifdef CPUT_OS_WINDOWS
#define OSSleep Sleep
#else
#include <unistd.h>
#define OSSleep sleep
#endif
#ifdef CPUT_FOR_OGLES3_COMPAT
CPUTOglES3CompatFuncPtrs gOGLESCompatFPtrs;
#endif

PFNGLDISPATCHCOMPUTEPROC  glDispatchCompute  = NULL;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture = NULL;
PFNGLPATCHPARAMETERIPROC  glPatchParameteri  = NULL;
PFNGLPATCHPARAMETERFVPROC glPatchParameterfv = NULL;

CPUTResult CPUT_OGL::DestroyOGLContext(void)
{
     // Disabling and deleting all rendering contexts
    if(display!= EGL_NO_DISPLAY)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if(context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(display,context);
        }
        if(surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(display,surface);
        }
        eglTerminate(display);
    }
    return CPUT_SUCCESS;
}


//-----------------------------------------------------------------------------
CPUTResult CPUT_OGL::CreateOGLContext(CPUTContextCreation ContextParams )
{
    CPUTResult result = CPUT_ERROR;
    
    
    // Get a matching FB config
    const EGLint attribs[] =
    {
        EGL_SURFACE_TYPE,     EGL_WINDOW_BIT,
        EGL_BLUE_SIZE,        8,
        EGL_GREEN_SIZE,       8,
        EGL_RED_SIZE,         8,
        EGL_ALPHA_SIZE,       8,
        EGL_DEPTH_SIZE,       24,
        EGL_STENCIL_SIZE,     8,
        //EGL_SAMPLE_BUFFERS  , 1,
        //EGL_SAMPLES         , 4,

        EGL_NONE
    };

#ifndef EGL_CONTEXT_MINOR_VERSION_KHR
    #define EGL_CONTEXT_MINOR_VERSION_KHR 0x30FB
#endif

#ifndef EGL_CONTEXT_MAJOR_VERSION_KHR
    #define EGL_CONTEXT_MAJOR_VERSION_KHR EGL_CONTEXT_CLIENT_VERSION
#endif

    EGLint format;
    EGLint numConfigs;
    EGLConfig config;
    EGLBoolean success = EGL_TRUE;
    
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) 
    {
        return result;
    }
    
    EGLint EGLMajor, EGLMinor;
    success = eglInitialize(display, &EGLMajor, &EGLMinor);
    if (!success)
    {
        DEBUG_PRINT("Failed to initialise EGL\n");
        return result;
    }
    DEBUG_PRINT("\nEGL Version: %d.%d\n", EGLMajor, EGLMinor);
    
    //
    // Here, the application chooses the configuration it desires. In this
    // sample, we have a very simplified selection process, where we pick
    // the first EGLConfig that matches our criteria
    //
    success = eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    if (!success)
    {
        DEBUG_PRINT("Failed to choose config\n");
        return result;
    }

    //
    // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    // As soon as we picked a EGLConfig, we can safely reconfigure the
    // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
    //
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

#ifdef CPUT_ANDROID
    ANativeWindow_setBuffersGeometry(mpWindow->GetHWnd(), 0, 0, format);
#endif

    //
    // Create window surface
    //
    const EGLint surfaceAttribs[] =
    {
        0x3087, 0x3089, // ask for sRGB surface
        EGL_NONE
    };

    surface = eglCreateWindowSurface(display, config, mpWindow->GetHWnd(), surfaceAttribs);
    if (surface == EGL_NO_SURFACE)
    {
        DEBUG_PRINT("Failed to create EGLSurface\n");
        return result;
    }

    //
    // Retrieve information about the newly created surface
    //
    {
        DEBUG_PRINT("\nEGL Surface Information\n");

        EGLint eglContextValue;
        std::string eglContextValueText;
        eglQuerySurface(display, surface, EGL_CONFIG_ID, &eglContextValue);
        DEBUG_PRINT("\tEGL_CONFIG_ID: %d\n", eglContextValue);

        eglQuerySurface(display, surface, EGL_HEIGHT, &eglContextValue);
        DEBUG_PRINT("\tEGL_HEIGHT: %d\n", eglContextValue);

        eglQuerySurface(display, surface, EGL_HORIZONTAL_RESOLUTION, &eglContextValue);
        DEBUG_PRINT("\tEGL_HORIZONTAL_RESOLUTION: %d\n", eglContextValue);

        eglQuerySurface(display, surface, EGL_LARGEST_PBUFFER, &eglContextValue);
        DEBUG_PRINT("\tEGL_LARGEST_PBUFFER: %d\n", eglContextValue);

        eglQuerySurface(display, surface, EGL_MIPMAP_LEVEL, &eglContextValue);
        DEBUG_PRINT("\tEGL_MIPMAP_LEVEL: %d\n", eglContextValue);

        eglQuerySurface(display, surface, EGL_MIPMAP_TEXTURE, &eglContextValue);
        switch(eglContextValue) {
        case EGL_TRUE:
            eglContextValueText.assign("EGL_TRUE");
            break;
        case EGL_FALSE:
            eglContextValueText.assign("EGL_FALSE");
            break;
        default:
            eglContextValue = 0;
            eglContextValueText.assign("ERROR");
            break;
        }
        DEBUG_PRINT("\tEGL_MIPMAP_TEXTURE: %s\n", eglContextValueText.c_str());

        eglQuerySurface(display, surface, EGL_MULTISAMPLE_RESOLVE, &eglContextValue);
        switch(eglContextValue) {
        case EGL_MULTISAMPLE_RESOLVE_DEFAULT:
            eglContextValueText.assign("EGL_MULTISAMPLE_RESOLVE_DEFAULT");
            break;
        case EGL_MULTISAMPLE_RESOLVE_BOX:
            eglContextValueText.assign("EGL_MULTISAMPLE_RESOLVE_BOX");
            break;
        default:
            eglContextValue = 0;
            eglContextValueText.assign("ERROR");
            break;
        }
        DEBUG_PRINT("\tEGL_MULTISAMPLE_RESOLVE: %s(%#x)\n", eglContextValueText.c_str(), eglContextValue);

        eglQuerySurface(display, surface, EGL_PIXEL_ASPECT_RATIO, &eglContextValue);
        DEBUG_PRINT("\tEGL_PIXEL_ASPECT_RATIO: %d\n", eglContextValue);

        eglQuerySurface(display, surface, EGL_RENDER_BUFFER, &eglContextValue);
        DEBUG_PRINT("\tEGL_RENDER_BUFFER: %d\n", eglContextValue);

        eglQuerySurface(display, surface, EGL_SWAP_BEHAVIOR, &eglContextValue);
        switch(eglContextValue) {
        case EGL_BUFFER_PRESERVED:
            eglContextValueText.assign("EGL_MULTISAMPLE_RESOLVE_DEFAULT");
            break;
        case EGL_BUFFER_DESTROYED:
            eglContextValueText.assign("EGL_MULTISAMPLE_RESOLVE_BOX");
            break;
        default:
            eglContextValue = 0;
            eglContextValueText.assign("ERROR");
            break;
        }
        DEBUG_PRINT("\tEGL_SWAP_BEHAVIOR: %s(%#x)\n", eglContextValueText.c_str(), eglContextValue);

        eglQuerySurface(display, surface, EGL_TEXTURE_FORMAT, &eglContextValue);
        switch(eglContextValue) {
        case EGL_NO_TEXTURE:
            eglContextValueText.assign("EGL_NO_TEXTURE");
            break;
        case EGL_TEXTURE_RGB:
            eglContextValueText.assign("EGL_TEXTURE_RGB");
            break;
        case EGL_TEXTURE_RGBA:
            eglContextValueText.assign("EGL_TEXTURE_RGBA");
            break;
        default:
            eglContextValue = 0;
            eglContextValueText.assign("ERROR");
            break;
        }
        DEBUG_PRINT("\tEGL_TEXTURE_FORMAT: %s(%#x)\n", eglContextValueText.c_str(), eglContextValue);

        eglQuerySurface(display, surface, EGL_TEXTURE_TARGET, &eglContextValue);
        switch(eglContextValue) {
        case EGL_NO_TEXTURE:
            eglContextValueText.assign("EGL_NO_TEXTURE");
            break;
        case EGL_TEXTURE_2D:
            eglContextValueText.assign("EGL_TEXTURE_2D");
            break;
        default:
            eglContextValue = 0;
            eglContextValueText.assign("ERROR");
            break;
        }
        DEBUG_PRINT("\tEGL_TEXTURE_TARGET: %s(%#x)\n", eglContextValueText.c_str(), eglContextValue);

        eglQuerySurface(display, surface, EGL_VERTICAL_RESOLUTION, &eglContextValue);
        DEBUG_PRINT("\tEGL_VERTICAL_RESOLUTION: %d\n", eglContextValue);

        eglQuerySurface(display, surface, EGL_WIDTH, &eglContextValue);
        DEBUG_PRINT("\tEGL_WIDTH: %d\n", eglContextValue);
    }

    //
    // Create OpenGL context
    //
    const EGLint contextAttribs[] =
    {
        #if defined CPUT_FOR_OGLES3
            #if defined CPUT_FOR_OGLES3_1
                EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
                EGL_CONTEXT_MINOR_VERSION_KHR, 1,
            #else
            EGL_CONTEXT_CLIENT_VERSION, 3,
        #endif
        #else
            #error "No ES version specified"
        #endif
        EGL_NONE
    };

    context = eglCreateContext(display, config, NULL, contextAttribs);
    if (context == EGL_NO_CONTEXT)
    {
        DEBUG_PRINT("Failed to create EGLContext %d.%d\n", contextAttribs[1], contextAttribs[3]);
        return result;
    }

    //
    // Retrieve information about the newly created context
    //
    {
        DEBUG_PRINT("\nEGL Context Information");

        EGLint eglContextValue;
        std::string eglContextValueText;
        eglQueryContext(display, context, EGL_CONFIG_ID, &eglContextValue);
        DEBUG_PRINT("\tEGL_CONFIG_ID: %d\n", eglContextValue);

        eglQueryContext(display, context, EGL_CONTEXT_CLIENT_TYPE, &eglContextValue);
        switch(eglContextValue) {
        case EGL_OPENGL_API:
            eglContextValueText.assign("EGL_OPENGL_API");
            break;
        case EGL_OPENGL_ES_API:
            eglContextValueText.assign("EGL_OPENGL_ES_API");
            break;
        case EGL_OPENVG_API:
            eglContextValueText.assign("EGL_OPENVG_API");
            break;
        default:
            eglContextValue = 0;
            eglContextValueText.assign("ERROR");
            break;
        }
        DEBUG_PRINT("\tEGL_CONTEXT_CLIENT_TYPE: %s(%#x)\n", eglContextValueText.c_str(), eglContextValue);

        eglQueryContext(display, context, EGL_CONTEXT_CLIENT_VERSION, &eglContextValue);
        DEBUG_PRINT("\tEGL_CONTEXT_CLIENT_VERSION: %d\n", eglContextValue);

        eglQueryContext(display, context, EGL_RENDER_BUFFER, &eglContextValue);
        switch(eglContextValue) {
        case EGL_SINGLE_BUFFER:
            eglContextValueText.assign("EGL_SINGLE_BUFFER");
            break;
        case EGL_BACK_BUFFER:
            eglContextValueText.assign("EGL_BACK_BUFFER");
            break;
        case EGL_NONE:
            eglContextValueText.assign("EGL_NONE");
            break;
        default:
            eglContextValue = 0;
            eglContextValueText.assign("ERROR");
            break;
        }
        DEBUG_PRINT("\tEGL_RENDER_BUFFER: %s(%#x)\n", eglContextValueText.c_str(), eglContextValue);
    }

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) 
    {
        DEBUG_PRINT("Failed to eglMakeCurrent\n");
        return result;
    }

    DEBUG_PRINT("\nGL Vendor: %s\n", glGetString(GL_VENDOR));
    DEBUG_PRINT("GL Renderer: %s\n", glGetString(GL_RENDERER));
    DEBUG_PRINT("GL Version: %s\n", glGetString(GL_VERSION));
    DEBUG_PRINT("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
   // Creates table of supported extensions strings
    extensions.clear();
    string tmp;
    sint32 begin, end;
    tmp   = string( (char*)glGetString( GL_EXTENSIONS ) );

    begin = 0;
    end   = tmp.find( ' ', 0 );

    DEBUG_PRINT("\nGL_EXTENSIONS\n");
    
    while( end != string::npos )
    {
        DEBUG_PRINT("\t%s", tmp.substr( begin, end-begin ).c_str());
        extensions.insert( extensions.end(), tmp.substr( begin, end-begin ) );
        begin = end + 1;
        end   = tmp.find( ' ', begin );

    }

    if(supportExtension("GL_INTEL_tessellation"))
    {
        glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)eglGetProcAddress("glPatchParameteri");
        DEBUG_PRINT("%s = %p", "glPatchParameteri",(void*)glPatchParameteri);
        glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)eglGetProcAddress("glPatchParameterfv");
        DEBUG_PRINT("%s = %p", "glPatchParameterfv",(void*)glPatchParameterfv);
    }
    if(supportExtension("GL_INTEL_compute_shader"))
    {
        glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)eglGetProcAddress("glDispatchCompute");
        DEBUG_PRINT("%s = %p", "glDispatchCompute",(void*)glDispatchCompute);
        glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)eglGetProcAddress("glBindImageTexture");
        DEBUG_PRINT("%s = %p", "glBindImageTexture",(void*)glBindImageTexture);
    }

//  if(supportExtension("GL_INTEL_shader_image_load_store"))
//  {
//      glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)eglGetProcAddress("glBindImageTexture");
//      DEBUG_PRINT("%s = %p", "glBindImageTexture",(void*)glBindImageTexture);
//  }

    EGLint width, height;
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
    mpWindow->SetClientDimensions(width, height);

    return CPUT_SUCCESS;
}
