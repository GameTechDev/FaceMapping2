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

#include "CPUTTexture.h"
#ifdef CPUT_FOR_DX11
#include "CPUTTextureDX11.h"
#elif (defined(CPUT_FOR_OGL) || defined(CPUT_FOR_OGLES))
#include "CPUTTextureOGL.h"
#else
    #error You must supply a target graphics API (ex: #define CPUT_FOR_DX11), or implement the target API for this file.
#endif


//--------------------------------------------------------------------------------------
CPUTTexture *CPUTTexture::Create( const std::string &name, const std::string absolutePathAndFilename, bool loadAsSRGB )
{
    // TODO: accept DX11/OGL param to control which platform we generate.
    // TODO: be sure to support the case where we want to support only one of them
#ifdef CPUT_FOR_DX11
    return CPUTTextureDX11::Create( name, absolutePathAndFilename, loadAsSRGB );
#elif (defined(CPUT_FOR_OGL) || defined(CPUT_FOR_OGLES))
    return CPUTTextureOGL::Create( name, absolutePathAndFilename, loadAsSRGB );
#else    
    #error You must supply a target graphics API (ex: #define CPUT_FOR_DX11), or implement the target API for this file.
#endif
    
}
