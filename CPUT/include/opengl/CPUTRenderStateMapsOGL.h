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

#ifndef _CPUTRENDERSTATEMAPSOGL_H
#define _CPUTRENDERSTATEMAPSOGL_H

// TODO: Should this be in a cpp file instead of an h file?
//       We just put it here to get it out of the way
//       in the implementation file (CPUTRenderStateBlockDX11.cpp)

#define CONVERTTYPE

#define MAX_LENGTH_RENDERSTATE_STRING 25



//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pBlendDescMap[] =
{
    /*
    { CONVERTTYPE("alphatocoverageenable"),  ePARAM_TYPE_BOOL,  (UINT)offsetof(D3D11_BLEND_DESC, AlphaToCoverageEnable)},
    { CONVERTTYPE("independentblendenable"), ePARAM_TYPE_BOOL,  (UINT)offsetof(D3D11_BLEND_DESC, IndependentBlendEnable)},
    { CONVERTTYPE("blendfactor1"),           ePARAM_TYPE_FLOAT, (UINT)offsetof(CPUTRenderStateDX11, BlendFactor[0] )},
    { CONVERTTYPE("blendfactor2"),           ePARAM_TYPE_FLOAT, (UINT)offsetof(CPUTRenderStateDX11, BlendFactor[1] )},
    { CONVERTTYPE("blendfactor3"),           ePARAM_TYPE_FLOAT, (UINT)offsetof(CPUTRenderStateDX11, BlendFactor[2] )},
    { CONVERTTYPE("blendfactor4"),           ePARAM_TYPE_FLOAT, (UINT)offsetof(CPUTRenderStateDX11, BlendFactor[3] )},
    { CONVERTTYPE("samplemask"),             ePARAM_TYPE_UINT,  (UINT)offsetof(CPUTRenderStateDX11, SampleMask )},
     */
    { CONVERTTYPE(""), ePARAM_TYPE_TYPELESS, 0}
     
};

//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pRenderTargetBlendDescMap[] =
{
    { CONVERTTYPE("blendenable"),           ePARAM_TYPE_BOOL,            (UINT)offsetof(CPUT_BLEND_DESC, BlendEnable )},
    { CONVERTTYPE("srcblend"),              ePARAM_TYPE_GL_BLEND,		(UINT)offsetof(CPUT_BLEND_DESC, SrcBlend )},
    { CONVERTTYPE("destblend"),             ePARAM_TYPE_GL_BLEND,		(UINT)offsetof(CPUT_BLEND_DESC, DestBlend )},
    { CONVERTTYPE("blendop"),               ePARAM_TYPE_GL_BLEND_OP,		(UINT)offsetof(CPUT_BLEND_DESC, BlendOp )},
    { CONVERTTYPE("srcblendalpha"),         ePARAM_TYPE_GL_BLEND,		(UINT)offsetof(CPUT_BLEND_DESC, SrcBlendAlpha )},
    { CONVERTTYPE("destblendalpha"),        ePARAM_TYPE_GL_BLEND,		(UINT)offsetof(CPUT_BLEND_DESC, DestBlendAlpha )},
    { CONVERTTYPE("blendopalpha"),          ePARAM_TYPE_GL_BLEND_OP,		(UINT)offsetof(CPUT_BLEND_DESC, BlendOpAlpha )},
 //   { CONVERTTYPE("rendertargetwritemask"), ePARAM_TYPE_UCHAR,          (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, RenderTargetWriteMask )},
    { CONVERTTYPE(""), ePARAM_TYPE_TYPELESS, 0}
};

//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pDepthStencilDescMap[] =
{
    { CONVERTTYPE("depthenable"),                 ePARAM_TYPE_BOOL,                  (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, DepthEnable)},
    { CONVERTTYPE("depthwritemask"),              ePARAM_TYPE_BOOL,                  (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, DepthWriteMask)},
    { CONVERTTYPE("depthfunc"),                   ePARAM_TYPE_OGL_COMPARISON_FUNC,   (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, DepthFunc)},
    { CONVERTTYPE("StencilEnable"),               ePARAM_TYPE_BOOL,                  (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, StencilEnable)},
    { CONVERTTYPE("StencilWriteMask"),            ePARAM_TYPE_OGL_HEX32,             (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, StencilWriteMask)},
    { CONVERTTYPE("FrontFaceStencilFailOp"),      ePARAM_TYPE_OGL_ENUM,              (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, FrontFaceStencilFailOp )},
    { CONVERTTYPE("FrontFaceStencilDepthFailOp"), ePARAM_TYPE_OGL_ENUM,              (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, FrontFaceStencilDepthFailOp)},
    { CONVERTTYPE("FrontFaceStencilPassOp"),      ePARAM_TYPE_OGL_ENUM,              (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, FrontFaceStencilPassOp)},
    { CONVERTTYPE("FrontFaceStencilFunc"),        ePARAM_TYPE_OGL_ENUM,              (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, FrontFaceStencilFunc)},
    { CONVERTTYPE("FrontFaceStencilFuncRef"),     ePARAM_TYPE_OGL_INT,               (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, FrontFaceStencilFuncRef)},
    { CONVERTTYPE("FrontFaceStencilFuncMask"),    ePARAM_TYPE_OGL_HEX32,             (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, FrontFaceStencilFuncMask)},
    { CONVERTTYPE("BackFaceStencilFailOp"),       ePARAM_TYPE_OGL_ENUM,              (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, BackFaceStencilDepthFailOp)},
    { CONVERTTYPE("BackFaceStencilDepthFailOp"),  ePARAM_TYPE_OGL_ENUM,              (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, BackFaceStencilDepthFailOp)},
    { CONVERTTYPE("BackFaceStencilPassOp"),       ePARAM_TYPE_OGL_ENUM,              (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, BackFaceStencilPassOp)},
    { CONVERTTYPE("BackFaceStencilFunc"),         ePARAM_TYPE_OGL_ENUM,              (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, BackFaceStencilFunc)},
    { CONVERTTYPE("BackFaceStencilFuncRef"),      ePARAM_TYPE_OGL_INT,               (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, BackFaceStencilFuncRef)},
    { CONVERTTYPE("BackFaceStencilFuncMask"),     ePARAM_TYPE_OGL_HEX32,             (UINT)offsetof(CPUT_DEPTH_STENCIL_DESC, BackFaceStencilFuncMask)},
    { CONVERTTYPE(""), ePARAM_TYPE_TYPELESS, 0}

};

//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pRasterizerDescMap[] =
{
    { CONVERTTYPE("FillMode"),              ePARAM_TYPE_OGL_ENUM,        (UINT)offsetof(CPUT_RASTERIZER_DESC, FillMode)},
    { CONVERTTYPE("CullMode"),              ePARAM_TYPE_OGL_ENUM,        (UINT)offsetof(CPUT_RASTERIZER_DESC, CullMode)},
    { CONVERTTYPE("CullingEnabled"),        ePARAM_TYPE_OGL_ENUM,        (UINT)offsetof(CPUT_RASTERIZER_DESC, CullingEnabled)},
    { CONVERTTYPE("FrontCounterClockwise"), ePARAM_TYPE_OGL_ENUM,        (UINT)offsetof(CPUT_RASTERIZER_DESC, FrontCounterClockwise)},
//    { CONVERTTYPE("DepthBias"),             ePARAM_TYPE_INT,             (UINT)offsetof(CPUT_RASTERIZER_DESC, DepthBias)}, // not sure how the opengl options compare to dx options
//    { CONVERTTYPE("DepthBiasClamp"),        ePARAM_TYPE_FLOAT,           (UINT)offsetof(CPUT_RASTERIZER_DESC, DepthBiasClamp)},
//    { CONVERTTYPE("SlopeScaledDepthBias"),  ePARAM_TYPE_FLOAT,           (UINT)offsetof(CPUT_RASTERIZER_DESC, SlopeScaledDepthBias)},
    { CONVERTTYPE("DepthClipEnable"),       ePARAM_TYPE_BOOL,            (UINT)offsetof(CPUT_RASTERIZER_DESC, DepthClipEnable)},
    { CONVERTTYPE("ScissorEnable"),         ePARAM_TYPE_BOOL,            (UINT)offsetof(CPUT_RASTERIZER_DESC, ScissorEnable)},
    { CONVERTTYPE("MultisampleEnable"),     ePARAM_TYPE_BOOL,            (UINT)offsetof(CPUT_RASTERIZER_DESC, MultisampleEnable)},
    { CONVERTTYPE("AntialiasedLineEnable"), ePARAM_TYPE_BOOL,            (UINT)offsetof(CPUT_RASTERIZER_DESC, AntialiasedLineEnable)},
    { CONVERTTYPE(""), ePARAM_TYPE_TYPELESS, 0}
};



//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pSamplerDescMap[] =
{
 //   { CONVERTTYPE("TextureName"),    ePARAM_TYPE_STRING,					(UINT)offsetof(CPUT_SAMPLER_DESC, TextureName)},
    { CONVERTTYPE("MinFilter"),      ePARAM_TYPE_OGL_FILTER,                (UINT)offsetof(CPUT_SAMPLER_DESC, MinFilter) },
    { CONVERTTYPE("MagFilter"),      ePARAM_TYPE_OGL_FILTER,				(UINT)offsetof(CPUT_SAMPLER_DESC, MagFilter)},
    { CONVERTTYPE("AddressU"),       ePARAM_TYPE_OGL_TEXTURE_ADDRESS_MODE,	(UINT)offsetof(CPUT_SAMPLER_DESC, AddressU)},
    { CONVERTTYPE("AddressV"),       ePARAM_TYPE_OGL_TEXTURE_ADDRESS_MODE,  (UINT)offsetof(CPUT_SAMPLER_DESC, AddressV)},
    { CONVERTTYPE("AddressW"),       ePARAM_TYPE_OGL_TEXTURE_ADDRESS_MODE,  (UINT)offsetof(CPUT_SAMPLER_DESC, AddressW)},
    { CONVERTTYPE("MipLODBias"),     ePARAM_TYPE_FLOAT,						(UINT)offsetof(CPUT_SAMPLER_DESC, MipLODBias)},
 //   { CONVERTTYPE("MaxAnisotropy"),  ePARAM_TYPE_UINT,                       (UINT)offsetof(CPUT_SAMPLER_DESC, MaxAnisotropy)},
    { CONVERTTYPE("ComparisonMode"), ePARAM_TYPE_OGL_COMPARISON_MODE,		(UINT)offsetof(CPUT_SAMPLER_DESC, ComparisonMode)},
    { CONVERTTYPE("ComparisonFunc"), ePARAM_TYPE_OGL_COMPARISON_FUNC,		(UINT)offsetof(CPUT_SAMPLER_DESC, ComparisonFunc)},
    { CONVERTTYPE("BorderColor0"),   ePARAM_TYPE_FLOAT,						(UINT)offsetof(CPUT_SAMPLER_DESC, BorderColor[0])},
    { CONVERTTYPE("BorderColor1"),   ePARAM_TYPE_FLOAT,						(UINT)offsetof(CPUT_SAMPLER_DESC, BorderColor[1])},
    { CONVERTTYPE("BorderColor2"),   ePARAM_TYPE_FLOAT,						(UINT)offsetof(CPUT_SAMPLER_DESC, BorderColor[2])},
    { CONVERTTYPE("BorderColor3"),   ePARAM_TYPE_FLOAT,						(UINT)offsetof(CPUT_SAMPLER_DESC, BorderColor[3])},
    { CONVERTTYPE("MinLOD"),         ePARAM_TYPE_FLOAT,						(UINT)offsetof(CPUT_SAMPLER_DESC, MinLOD)},
    { CONVERTTYPE("MaxLOD"),         ePARAM_TYPE_FLOAT,						(UINT)offsetof(CPUT_SAMPLER_DESC, MaxLOD)},

    { CONVERTTYPE(""), ePARAM_TYPE_TYPELESS, 0}
};

static const StringToIntMapEntry pOGLBlendMap[] = {
    { CONVERTTYPE("gl_one"),		GL_ONE },
    { CONVERTTYPE("gl_zero"),		GL_ZERO },
	{ CONVERTTYPE("gl_src_color"),  GL_SRC_COLOR },
	{ CONVERTTYPE("gl_one_minus_src_color"),    GL_ONE_MINUS_SRC_COLOR },
	{ CONVERTTYPE("gl_dst_color"),    GL_DST_COLOR },
	{ CONVERTTYPE("gl_one_minus_dst_color"),    GL_ONE_MINUS_DST_COLOR },
	{ CONVERTTYPE("gl_src_alpha"),    GL_SRC_ALPHA },
	{ CONVERTTYPE("gl_one_minus_src_alpha"),    GL_ONE_MINUS_SRC_ALPHA },
	{ CONVERTTYPE("gl_dst_alpha"),    GL_DST_ALPHA },
	{ CONVERTTYPE("gl_one_minus_dst_alpha"),    GL_ONE_MINUS_DST_ALPHA },
	{ CONVERTTYPE("gl_constant_color"),    GL_CONSTANT_COLOR },
	{ CONVERTTYPE("gl_one_minus_constant_color"),    GL_ONE_MINUS_CONSTANT_COLOR },
	{ CONVERTTYPE("gl_constant_alpha"),    GL_CONSTANT_ALPHA },
	{ CONVERTTYPE("gl_one_minus_constant_alpha"),    GL_ONE_MINUS_CONSTANT_ALPHA },
	{ CONVERTTYPE("gl_src_alpha_saturate"),    GL_SRC_ALPHA_SATURATE },
    { CONVERTTYPE(""), -1 }
};



static const StringToIntMapEntry pOGLMap[] = {
    { CONVERTTYPE("gl_true"),     GL_TRUE },
    { CONVERTTYPE("gl_false"),    GL_FALSE },
 #ifndef CPUT_FOR_OGLES  
    { CONVERTTYPE("gl_point"),    GL_POINT },
    { CONVERTTYPE("gl_line"),     GL_LINE },
    { CONVERTTYPE("gl_fill"),     GL_FILL },
#endif    
    { CONVERTTYPE("gl_cw"),       GL_CW },
    { CONVERTTYPE("gl_ccw"),      GL_CCW },
    
    { CONVERTTYPE("gl_front"),    GL_FRONT },
    { CONVERTTYPE("gl_back"),     GL_BACK },
    { CONVERTTYPE("gl_front_and_back"), GL_FRONT_AND_BACK },
    { CONVERTTYPE("gl_none"), GL_NONE },
    { CONVERTTYPE(""), -1 }
};


static const StringToIntMapEntry pOGLBlendOpMap[] = {
  
    { CONVERTTYPE("gl_func_add"),          GL_FUNC_ADD },
    { CONVERTTYPE("gl_func_subtract"),      GL_FUNC_SUBTRACT },
    { CONVERTTYPE("gl_func_reverse_subtract"),      GL_FUNC_REVERSE_SUBTRACT },
    { CONVERTTYPE(""), -1 }
};
//-----------------------------------------------------------------------------
static const StringToIntMapEntry pDepthWriteMaskMap[] = {
    /*
    { CONVERTTYPE("D3D11_DEPTH_WRITE_MASK_ZERO"), D3D11_DEPTH_WRITE_MASK_ZERO },
    { CONVERTTYPE("D3D11_DEPTH_WRITE_MASK_ALL"),  D3D11_DEPTH_WRITE_MASK_ALL },
     */
    { CONVERTTYPE(""), -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pOGLComparisonModeMap[] = {
    
    { CONVERTTYPE("gl_none"), GL_NONE },
	{ CONVERTTYPE("gl_compare_ref_to_texture"),  GL_COMPARE_REF_TO_TEXTURE },
     
    { CONVERTTYPE(""), -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pOGLComparisonMap[] = {
    { CONVERTTYPE("gl_never"),    GL_NEVER },
    { CONVERTTYPE("gl_less"),     GL_LESS },
    { CONVERTTYPE("gl_equal"),    GL_EQUAL },
    { CONVERTTYPE("gl_lequal"),   GL_LEQUAL },
    { CONVERTTYPE("gl_greater"),  GL_GREATER },
    { CONVERTTYPE("gl_notequal"), GL_NOTEQUAL },
    { CONVERTTYPE("gl_gequal"),   GL_GEQUAL },
    { CONVERTTYPE("gl_always"),   GL_ALWAYS },
    { CONVERTTYPE(""), -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pOGLStencilOpMap[] = {
    { CONVERTTYPE("gl_keep"),     GL_KEEP },
    { CONVERTTYPE("gl_zero"),     GL_ZERO },
    { CONVERTTYPE("gl_replace"),  GL_REPLACE },
    { CONVERTTYPE("gl_incr"),     GL_INCR },
    { CONVERTTYPE("gl_incr_wrap"),GL_INCR_WRAP },
    { CONVERTTYPE("gl_decr"),     GL_DECR },
    { CONVERTTYPE("gl_decr_wrap"),GL_DECR_WRAP },
    { CONVERTTYPE("gl_invert"),   GL_INVERT },
    { CONVERTTYPE(""), -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pFillModeMap[] = {
    /*
    { CONVERTTYPE("D3D11_FILL_WIREFRAME"),      D3D11_FILL_WIREFRAME },
    { CONVERTTYPE("D3D11_FILL_SOLID"),          D3D11_FILL_SOLID },
     */
    { CONVERTTYPE(""), -1 }
};

static const StringToIntMapEntry pCullEnableMap[] = {
    { CONVERTTYPE("gl_true"), GL_TRUE },
    { CONVERTTYPE("gl_false"), GL_FALSE}
};
//-----------------------------------------------------------------------------
static const StringToIntMapEntry pCullModeMap[] = {
    { CONVERTTYPE("gl_front"),          GL_FRONT },
    { CONVERTTYPE("gl_back"),           GL_BACK },
    { CONVERTTYPE("gl_front_and_back"), GL_FRONT_AND_BACK },
};


//-----------------------------------------------------------------------------
static const StringToIntMapEntry pOGLFilterMap[] = {
    { CONVERTTYPE("gl_nearest"),                GL_NEAREST },
    { CONVERTTYPE("gl_linear"),                 GL_LINEAR },
    { CONVERTTYPE("gl_nearest_mipmap_nearest"), GL_NEAREST_MIPMAP_NEAREST },
    { CONVERTTYPE("gl_linear_mipmap_nearest"),  GL_LINEAR_MIPMAP_NEAREST },
    { CONVERTTYPE("gl_nearest_mipmap_linear"),  GL_NEAREST_MIPMAP_LINEAR },
    { CONVERTTYPE("gl_linear_mipmap_linear"),   GL_LINEAR_MIPMAP_LINEAR },
    { CONVERTTYPE(""), -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pOGLTextureAddressMap[] = {
 #ifndef CPUT_FOR_OGLES
	{ CONVERTTYPE("gl_clamp_to_border"),        GL_CLAMP_TO_BORDER },
#endif
    { CONVERTTYPE("gl_clamp_to_edge"),          GL_CLAMP_TO_EDGE },
    { CONVERTTYPE("gl_mirrored_repeat"),        GL_MIRRORED_REPEAT },
    { CONVERTTYPE("gl_repeat"),                 GL_REPEAT },
#ifndef CPUT_FOR_OGLES
	{ CONVERTTYPE("gl_clamp"),                  GL_CLAMP },
#endif
    { CONVERTTYPE(""), -1 }
};

#endif //_CPUTRENDERSTATEMAPSOGL_H
