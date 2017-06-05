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
#include "CPUTRenderStateMapsOGL.h"
#include "CPUTRenderParams.h"
//-----------------------------------------------------------------------------
void CPUTRenderStateBlockOGL::ReadValue( CPUTConfigEntry *pValue, const CPUTRenderStateMapEntry *pRenderStateList, void *pDest )
{
    std::string lowerCaseName = pValue->NameAsString();

    bool found = false;
    // Find it in the map.  TODO: could use a real map.  Maybe with a binary search, lexical storage, etc.
    for( CPUTRenderStateMapEntry const *pCur = pRenderStateList; pCur->name.compare(""); pCur++ )
    {
#ifndef CPUT_OS_WINDOWS
            found = 0 == strcasecmp( lowerCaseName.data(), pCur->name.data() );
#else
            found = 0 == _stricmp( lowerCaseName.data(), pCur->name.data() );
#endif

        if( found )
        {
            // We found it.  Now convert it from the text file's string to its internal representation

            // There must be a more-generic way to do the following.  write( void*, void*, type ).
            // Use function pointer array to ValueAsInt() and similar, so we can call them without the switch?
            // Might require they all have same signature ==> use void pointers, and cast internally?
            switch( pCur->type )
            {
            case ePARAM_TYPE_TYPELESS: ASSERT(0,""); break; // Should not get here.
            case ePARAM_TYPE_INT:     *(int*)&((char*)pDest)[pCur->offset]  = pValue->ValueAsInt();   break;
            case ePARAM_TYPE_UINT:   *(UINT*)&((char*)pDest)[pCur->offset]  = pValue->ValueAsUint();  break;
            case ePARAM_TYPE_BOOL:   *(bool*)&((char*)pDest)[pCur->offset]  = pValue->ValueAsBool();  break;
            case ePARAM_TYPE_FLOAT: *(float*)&((char*)pDest)[pCur->offset]  = pValue->ValueAsFloat(); break;
            case ePARAM_TYPE_SHORT: *(short*)&((char*)pDest)[pCur->offset]  = pValue->ValueAsInt();   break;
            case ePARAM_TYPE_CHAR:            ((char*)pDest)[pCur->offset]  = pValue->ValueAsInt();   break;
            case ePARAM_TYPE_UCHAR:           ((unsigned char*)pDest)[pCur->offset] = pValue->ValueAsUint();  break;
            case ePARAM_TYPE_OGL_BOOL: *(GLboolean*)&((char*)pDest)[pCur->offset] = pValue->ValueAsBool();  break;
            case ePARAM_TYPE_OGL_INT:      *(GLint*)&((char*)pDest)[pCur->offset] = pValue->ValueAsInt();  break;
            case ePARAM_TYPE_OGL_UINT:    *(GLuint*)&((char*)pDest)[pCur->offset] = pValue->ValueAsUint();  break;
            case ePARAM_TYPE_OGL_HEX32:   *(GLuint*)&((char*)pDest)[pCur->offset] = pValue->ValueAsHex32(); break;
            case ePARAM_TYPE_STRING:       strncpy(&((char*)pDest)[pCur->offset], pValue->ValueAsString().c_str(), MAX_LENGTH_RENDERSTATE_STRING); break;
                // The following types must be converted from string to enum.  They achieve this with a translation map
            case ePARAM_TYPE_GL_BLEND:                  found = pOGLBlendMap->FindMapEntryByName(         (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_GL_BLEND_OP:               found = pOGLBlendOpMap->FindMapEntryByName(       (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_DEPTH_WRITE_MASK:          found = pDepthWriteMaskMap->FindMapEntryByName(   (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_STENCIL_OP:            found = pOGLStencilOpMap->FindMapEntryByName(     (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_FILL_MODE:             found = pFillModeMap->FindMapEntryByName(         (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_CULL_ENABLE:           found = pCullEnableMap->FindMapEntryByName(       (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_CULL_MODE:             found = pCullModeMap->FindMapEntryByName(         (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_FILTER:                found = pOGLFilterMap->FindMapEntryByName(        (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_COMPARISON_MODE:       found = pOGLComparisonModeMap->FindMapEntryByName((int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_COMPARISON_FUNC:       found = pOGLComparisonMap->FindMapEntryByName(    (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_TEXTURE_ADDRESS_MODE:  found = pOGLTextureAddressMap->FindMapEntryByName((int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            case ePARAM_TYPE_OGL_ENUM:                  found = pOGLMap->FindMapEntryByName(              (int*)&((char*)pDest)[pCur->offset], pValue->ValueAsString() ); break;
            }
            break; // From for.  We found it, so we're done.
        }
    }
    ASSERT( found, "Unkown render state: '" + pValue->NameAsString() + "'." );
} // CPUTRenderStateBlockOGL::ReadValue()


//-----------------------------------------------------------------------------
CPUTResult CPUTRenderStateBlockOGL::ReadProperties(
    CPUTConfigFile                &file,
    const std::string                 &blockName,
    const CPUTRenderStateMapEntry *pMap,
    void                          *pDest
)
{
  //  assert(false);
    CPUTConfigBlock *pProperties = file.GetBlockByName(blockName);
    if( !pProperties )
    {
        // Note: We choose not to assert here.  The nature of the parameter block is that
        // only the values that deviate from default need to be present.  It is very
        // common that blocks will be missing
        return CPUT_ERROR_PARAMETER_BLOCK_NOT_FOUND;
    }

    UINT count = pProperties->ValueCount();
    for( UINT ii=0; ii<count; ii++ )
    {
        // Get the next property
        CPUTConfigEntry *pValue = pProperties->GetValue(ii);
        ASSERT( pValue->IsValid(), "Invalid Value: '"+pValue->NameAsString()+"'." );
        ReadValue( pValue, pMap, pDest );
    }
    return CPUT_SUCCESS;
} // CPUTRenderStateBlockOGL::ReadProperties()

//-----------------------------------------------------------------------------
void CPUTRenderStateOGL::SetDefaults()
{
    DepthStencilDesc.DepthEnable                  = true;
    DepthStencilDesc.DepthWriteMask               = GL_TRUE;
    DepthStencilDesc.DepthFunc                    = GL_GREATER;
    DepthStencilDesc.StencilEnable                = false;
    DepthStencilDesc.StencilWriteMask             = 0xFFFFFFFF;
    DepthStencilDesc.FrontFaceStencilFunc         = GL_ALWAYS;
    DepthStencilDesc.FrontFaceStencilFuncRef      = 0;
    DepthStencilDesc.FrontFaceStencilFuncMask     = 0xFFFFFFFF;
    DepthStencilDesc.FrontFaceStencilDepthFailOp  = GL_KEEP;
    DepthStencilDesc.FrontFaceStencilPassOp       = GL_KEEP;
    DepthStencilDesc.FrontFaceStencilFailOp       = GL_KEEP;
    DepthStencilDesc.BackFaceStencilFunc          = GL_ALWAYS;
    DepthStencilDesc.BackFaceStencilFuncRef       = 0;
    DepthStencilDesc.BackFaceStencilFuncMask      = 0xFFFFFFFF;
    DepthStencilDesc.BackFaceStencilDepthFailOp   = GL_KEEP;
    DepthStencilDesc.BackFaceStencilPassOp        = GL_KEEP;
    DepthStencilDesc.BackFaceStencilFailOp        = GL_KEEP;
    
#ifndef CPUT_FOR_OGLES
    RasterizerDesc.FillMode                       = GL_FILL;
#endif
    RasterizerDesc.CullMode                       = GL_BACK;
    RasterizerDesc.FrontCounterClockwise          = GL_CW;
    RasterizerDesc.CullingEnabled                 = true;
    RasterizerDesc.DepthClipEnable                = true;
    RasterizerDesc.ScissorEnable                  = false;
    RasterizerDesc.MultisampleEnable              = true;
    RasterizerDesc.AntialiasedLineEnable          = false;

	RenderTargetBlendDesc.BlendEnable			= false;
	RenderTargetBlendDesc.BlendOp				 = GL_FUNC_ADD;
	RenderTargetBlendDesc.BlendOpAlpha			 = GL_FUNC_ADD;
	RenderTargetBlendDesc.SrcBlend				 = GL_ONE;
	RenderTargetBlendDesc.DestBlend				 = GL_ZERO;
	RenderTargetBlendDesc.SrcBlendAlpha			 = GL_ONE;
	RenderTargetBlendDesc.DestBlendAlpha		 = GL_ZERO;


    DefaultSamplerDesc.MinFilter                 = GL_LINEAR_MIPMAP_LINEAR;
    DefaultSamplerDesc.MagFilter                 = GL_LINEAR;
    DefaultSamplerDesc.AddressU                  = GL_REPEAT; // Note that these are different from DX default (*CLAMP)
    DefaultSamplerDesc.AddressV                  = GL_REPEAT;
    DefaultSamplerDesc.AddressW                  = GL_REPEAT;
    DefaultSamplerDesc.MipLODBias                = 0.0f;

	DefaultSamplerDesc.ComparisonMode            = GL_NONE;
	DefaultSamplerDesc.ComparisonFunc            = GL_NEVER;
    DefaultSamplerDesc.BorderColor[0]            = 0.0f;
    DefaultSamplerDesc.BorderColor[1]            = 0.0f;
    DefaultSamplerDesc.BorderColor[2]            = 0.0f;
    DefaultSamplerDesc.BorderColor[3]            = 0.0f;
    DefaultSamplerDesc.MinLOD                    = -1000.0f;
    DefaultSamplerDesc.MaxLOD                    = 1000.0f;
        
    for( int ii=0; ii<NUM_SAMPLERS_PER_RENDERSTATE; ii++ )
    {
        SamplerDesc[ii].MinFilter                 = GL_LINEAR_MIPMAP_LINEAR;
        SamplerDesc[ii].MagFilter                 = GL_LINEAR;
        SamplerDesc[ii].AddressU                  = GL_REPEAT; // Note that these are different from DX default (*CLAMP)
        SamplerDesc[ii].AddressV                  = GL_REPEAT;
        SamplerDesc[ii].AddressW                  = GL_REPEAT;
        SamplerDesc[ii].MipLODBias                = 0.0f;

		SamplerDesc[ii].ComparisonMode            = GL_NONE;
		SamplerDesc[ii].ComparisonFunc            = GL_NEVER;
        SamplerDesc[ii].BorderColor[0]            = 0.0f;
        SamplerDesc[ii].BorderColor[1]            = 0.0f;
        SamplerDesc[ii].BorderColor[2]            = 0.0f;
        SamplerDesc[ii].BorderColor[3]            = 0.0f;
        SamplerDesc[ii].MinLOD                    = -1000;
        SamplerDesc[ii].MaxLOD                    = 1000;
    }
    
    
    
} // CPUTRenderStateOGL::SetDefaults()

//-----------------------------------------------------------------------------
CPUTResult CPUTRenderStateBlockOGL::LoadRenderStateBlock(const std::string &fileName)
{
    // TODO: If already loaded, then Release() all the old members

    // use the fileName for now, maybe we'll add names later?
    mMaterialName = fileName;

    // Open/parse the file
    CPUTConfigFile file;
    CPUTResult result = file.LoadFile(fileName);
    ASSERT( !FAILED(result), "Failed loading file: '" + fileName + "'." );

    // Note: We ignore "not found error" results for ReadProperties() calls.
    // These blocks are optional.
//    for( UINT ii=0; ii<8; ii++ )
//    {
//        char pBlockName[64];
//#ifndef CPUT_OS_WINDOWS
//        sprintf( pBlockName, "RenderTargetBlendStateOGL_%d", ii+1 );
//#else
//        sprintf( pBlockName, ("RenderTargetBlendStateOGL_%d"), ii+1 );
//#endif
//    }

//#ifndef CPUT_OS_WINDOWS
//    ReadProperties( file, "DepthStencilStateOGL", pDepthStencilDescMap, &mStateDesc.DepthStencilDesc);
//    ReadProperties( file, "RasterizerStateOGL",   pRasterizerDescMap,   &mStateDesc.RasterizerDesc);
//    ReadProperties( file, "RenderTargetBlendStateOGL",   pRenderTargetBlendDescMap,   &mStateDesc.RenderTargetBlendDesc);
//#else
    ReadProperties( file, "DepthStencilStateOGL", pDepthStencilDescMap, &mStateDesc.DepthStencilDesc);
    ReadProperties( file, "RasterizerStateOGL",   pRasterizerDescMap,   &mStateDesc.RasterizerDesc);
    ReadProperties( file, "RenderTargetBlendStateOGL",   pRenderTargetBlendDescMap,   &mStateDesc.RenderTargetBlendDesc);
//#endif

    //
    // For each sampler we read, need to create an OGL sampler object
    //
    GL_CHECK(glGenSamplers(1, &mStateDesc.DefaultSamplerID));
    GL_CHECK(glSamplerParameteri( mStateDesc.DefaultSamplerID, GL_TEXTURE_MIN_FILTER,   mStateDesc.DefaultSamplerDesc.MinFilter));
    GL_CHECK(glSamplerParameteri( mStateDesc.DefaultSamplerID, GL_TEXTURE_MAG_FILTER,   mStateDesc.DefaultSamplerDesc.MagFilter));
    GL_CHECK(glSamplerParameteri( mStateDesc.DefaultSamplerID, GL_TEXTURE_WRAP_S,       mStateDesc.DefaultSamplerDesc.AddressU));
    GL_CHECK(glSamplerParameteri( mStateDesc.DefaultSamplerID, GL_TEXTURE_WRAP_T,       mStateDesc.DefaultSamplerDesc.AddressV));
    GL_CHECK(glSamplerParameteri( mStateDesc.DefaultSamplerID, GL_TEXTURE_WRAP_R,       mStateDesc.DefaultSamplerDesc.AddressW));
    GL_CHECK(glSamplerParameteri( mStateDesc.DefaultSamplerID, GL_TEXTURE_COMPARE_MODE, mStateDesc.DefaultSamplerDesc.ComparisonMode));
    GL_CHECK(glSamplerParameteri( mStateDesc.DefaultSamplerID, GL_TEXTURE_COMPARE_FUNC, mStateDesc.DefaultSamplerDesc.ComparisonFunc));
    GL_CHECK(glSamplerParameterf( mStateDesc.DefaultSamplerID, GL_TEXTURE_MIN_LOD,      mStateDesc.DefaultSamplerDesc.MinLOD));
    GL_CHECK(glSamplerParameterf( mStateDesc.DefaultSamplerID, GL_TEXTURE_MAX_LOD,      mStateDesc.DefaultSamplerDesc.MaxLOD));
#ifndef CPUT_FOR_OGLES
    GL_CHECK(glSamplerParameterf( mStateDesc.DefaultSamplerID, GL_TEXTURE_LOD_BIAS,     mStateDesc.DefaultSamplerDesc.MipLODBias));
    GL_CHECK(glSamplerParameterfv(mStateDesc.DefaultSamplerID, GL_TEXTURE_BORDER_COLOR, mStateDesc.DefaultSamplerDesc.BorderColor));
#endif
        
    mNumSamplers = 0;
    for( UINT ii=0; ii<NUM_SAMPLERS_PER_RENDERSTATE; ii++ )
    {
        // TODO: Use sampler names from .fx file.  Already did this for texture names.
        // The challenge is that the renderstate file is independent from the material (and the shaders).
        // Another feature is that the artists don't name the samplers (in the CPUTSL source).  Though, arbitrary .fx files can.
        // TODO: Add sampler-state properties to CPUTSL source (e.g., filter modes).  Then, have ShaderGenerator output a .rs file.
        char pBlockName[64];
        sprintf( pBlockName, "SamplerOGL_%d", ii+1 );
        CPUTResult result = ReadProperties( file, std::string(pBlockName), pSamplerDescMap, &mStateDesc.SamplerDesc[ii] );

        if( CPUT_SUCCESS != result )
        {
            break; // Reached last sampler spec
        }

        //
        // For each sampler we read, need to create an OGL sampler object
        //
        GL_CHECK(glGenSamplers(1, &mStateDesc.SamplerIDs[ii]));
        GL_CHECK(glSamplerParameteri( mStateDesc.SamplerIDs[ii], GL_TEXTURE_MIN_FILTER,   mStateDesc.SamplerDesc[ii].MinFilter));
        GL_CHECK(glSamplerParameteri( mStateDesc.SamplerIDs[ii], GL_TEXTURE_MAG_FILTER,   mStateDesc.SamplerDesc[ii].MagFilter));
        GL_CHECK(glSamplerParameteri( mStateDesc.SamplerIDs[ii], GL_TEXTURE_WRAP_S,       mStateDesc.SamplerDesc[ii].AddressU));
        GL_CHECK(glSamplerParameteri( mStateDesc.SamplerIDs[ii], GL_TEXTURE_WRAP_T,       mStateDesc.SamplerDesc[ii].AddressV));
        GL_CHECK(glSamplerParameteri( mStateDesc.SamplerIDs[ii], GL_TEXTURE_WRAP_R,       mStateDesc.SamplerDesc[ii].AddressW));
#ifndef CPUT_FOR_OGLES
        GL_CHECK(glSamplerParameterf( mStateDesc.SamplerIDs[ii], GL_TEXTURE_LOD_BIAS,     mStateDesc.SamplerDesc[ii].MipLODBias));
        GL_CHECK(glSamplerParameterfv(mStateDesc.SamplerIDs[ii], GL_TEXTURE_BORDER_COLOR, mStateDesc.SamplerDesc[ii].BorderColor));
#endif
        GL_CHECK(glSamplerParameteri( mStateDesc.SamplerIDs[ii], GL_TEXTURE_COMPARE_MODE, mStateDesc.SamplerDesc[ii].ComparisonMode));
        GL_CHECK(glSamplerParameteri( mStateDesc.SamplerIDs[ii], GL_TEXTURE_COMPARE_FUNC, mStateDesc.SamplerDesc[ii].ComparisonFunc));
        GL_CHECK(glSamplerParameterf( mStateDesc.SamplerIDs[ii], GL_TEXTURE_MIN_LOD,      mStateDesc.SamplerDesc[ii].MinLOD));
        GL_CHECK(glSamplerParameterf( mStateDesc.SamplerIDs[ii], GL_TEXTURE_MAX_LOD,      mStateDesc.SamplerDesc[ii].MaxLOD));

        ++mNumSamplers;
    }
 //   CreateNativeResources();

    return CPUT_SUCCESS;
} // CPUTRenderStateBlockOGL::LoadRenderStateBlock()

//-----------------------------------------------------------------------------
void CPUTRenderStateBlockOGL::CreateNativeResources()
{
    assert(false);
} // CPUTRenderStateBlockOGL::CreateDXResources()

//-----------------------------------------------------------------------------
void CPUTRenderStateBlockOGL::SetRenderStates()
{
    //
    // Set Depth and Stencil render states
    //
    mStateDesc.DepthStencilDesc.DepthEnable   ? glEnable(GL_DEPTH_TEST)   : glDisable(GL_DEPTH_TEST);
    mStateDesc.DepthStencilDesc.StencilEnable ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    GL_CHECK("Set Depth Stencil States");

    GL_CHECK(glDepthMask(mStateDesc.DepthStencilDesc.DepthWriteMask));
    GL_CHECK(glDepthFunc(mStateDesc.DepthStencilDesc.DepthFunc));
    
    GL_CHECK(glStencilMaskSeparate(GL_FRONT_AND_BACK, mStateDesc.DepthStencilDesc.StencilWriteMask));
    GL_CHECK(glStencilOpSeparate(GL_FRONT, mStateDesc.DepthStencilDesc.FrontFaceStencilFailOp, mStateDesc.DepthStencilDesc.FrontFaceStencilDepthFailOp, mStateDesc.DepthStencilDesc.FrontFaceStencilPassOp));
    GL_CHECK(glStencilOpSeparate(GL_BACK, mStateDesc.DepthStencilDesc.BackFaceStencilFailOp, mStateDesc.DepthStencilDesc.BackFaceStencilDepthFailOp, mStateDesc.DepthStencilDesc.BackFaceStencilPassOp));
    GL_CHECK(glStencilFuncSeparate(GL_FRONT, mStateDesc.DepthStencilDesc.FrontFaceStencilFunc, mStateDesc.DepthStencilDesc.FrontFaceStencilFuncRef, mStateDesc.DepthStencilDesc.FrontFaceStencilFuncMask));
    GL_CHECK(glStencilFuncSeparate(GL_BACK, mStateDesc.DepthStencilDesc.BackFaceStencilFunc, mStateDesc.DepthStencilDesc.BackFaceStencilFuncRef, mStateDesc.DepthStencilDesc.BackFaceStencilFuncMask));
    
    //
    // Set Rasterization states
    //
#ifndef CPUT_FOR_OGLES
    GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, mStateDesc.RasterizerDesc.FillMode));
#endif
    GL_CHECK(glCullFace(mStateDesc.RasterizerDesc.CullMode));
    GL_CHECK(glFrontFace(mStateDesc.RasterizerDesc.FrontCounterClockwise));
    //GL_CHECK(glPolygonOffset()); // used for depth bias
    mStateDesc.RasterizerDesc.CullingEnabled        ? glEnable(GL_CULL_FACE)    : glDisable(GL_CULL_FACE);
    mStateDesc.RasterizerDesc.ScissorEnable         ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST); // is glScissor needed here as well?
#ifndef CPUT_FOR_OGLES
    mStateDesc.RasterizerDesc.DepthClipEnable       ? glDisable(GL_DEPTH_CLAMP) : glEnable(GL_DEPTH_CLAMP);
    mStateDesc.RasterizerDesc.MultisampleEnable     ? glEnable(GL_MULTISAMPLE)  : glDisable(GL_MULTISAMPLE);  // is glSampleCoverage() needed here as well?
    mStateDesc.RasterizerDesc.AntialiasedLineEnable ? glEnable(GL_LINE_SMOOTH)  : glDisable(GL_LINE_SMOOTH);
    
    //NOTE: Could not find the rendertarget being set to SRGB anywhere else; this might not be the best place
    GL_CHECK(glEnable(GL_FRAMEBUFFER_SRGB));
#endif
    GL_CHECK("Set Rasterization States");
    //glDisable(GL_CULL_FACE);

    //
    // Set Blend states
    //

	mStateDesc.RenderTargetBlendDesc.BlendEnable ? glEnable(GL_BLEND): glDisable(GL_BLEND);
    GL_CHECK(glBlendFuncSeparate(mStateDesc.RenderTargetBlendDesc.SrcBlend,mStateDesc.RenderTargetBlendDesc.DestBlend,mStateDesc.RenderTargetBlendDesc.SrcBlendAlpha,mStateDesc.RenderTargetBlendDesc.DestBlendAlpha));
	GL_CHECK(glBlendEquationSeparate(mStateDesc.RenderTargetBlendDesc.BlendOp,mStateDesc.RenderTargetBlendDesc.BlendOpAlpha));
    GL_CHECK("Set Blend States");


 } // CPUTRenderStateBlockOGL::SetRenderState()

CPUTRenderStateBlockOGL* CPUTRenderStateBlockOGL::Create()
{
	return new CPUTRenderStateBlockOGL();
}
void SetRenderStateBlock(CPUTRenderStateBlock* pNewBase, CPUTRenderStateBlock* pCurrentBase)
{
    if (pNewBase != pCurrentBase)
    {
        pNewBase->SetRenderStates();
    }
}
