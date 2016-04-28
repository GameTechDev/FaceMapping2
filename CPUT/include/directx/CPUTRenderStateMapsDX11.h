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

#ifndef _CPUTRENDERSTATEMAPSDX11_H
#define _CPUTRENDERSTATEMAPSDX11_H

#define MAX_LENGTH_RENDERSTATE_STRING 25

// TODO: Should this be in a cpp file instead of an h file?
//       We just put it here to get it out of the way
//       in the implementation file (CPUTRenderStateBlockDX11.cpp)

//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pBlendDescMap[] =
{
    { "alphatocoverageenable",  ePARAM_TYPE_BOOL,  (UINT)offsetof(D3D11_BLEND_DESC, AlphaToCoverageEnable)},
    { "independentblendenable", ePARAM_TYPE_BOOL,  (UINT)offsetof(D3D11_BLEND_DESC, IndependentBlendEnable)},
    { "blendfactor1",           ePARAM_TYPE_FLOAT, (UINT)offsetof(CPUTRenderStateDX11, BlendFactor[0] )},
    { "blendfactor2",           ePARAM_TYPE_FLOAT, (UINT)offsetof(CPUTRenderStateDX11, BlendFactor[1] )},
    { "blendfactor3",           ePARAM_TYPE_FLOAT, (UINT)offsetof(CPUTRenderStateDX11, BlendFactor[2] )},
    { "blendfactor4",           ePARAM_TYPE_FLOAT, (UINT)offsetof(CPUTRenderStateDX11, BlendFactor[3] )},
    { "samplemask",             ePARAM_TYPE_UINT,  (UINT)offsetof(CPUTRenderStateDX11, SampleMask )},
    { "", ePARAM_TYPE_TYPELESS, 0}
};

//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pRenderTargetBlendDescMap[] =
{
    { "blendenable",           ePARAM_TYPE_BOOL,           (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, BlendEnable )},
    { "srcblend",              ePARAM_TYPE_D3D11_BLEND,    (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, SrcBlend )},
    { "destblend",             ePARAM_TYPE_D3D11_BLEND,    (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, DestBlend )},
    { "blendop",               ePARAM_TYPE_D3D11_BLEND_OP, (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, BlendOp )},
    { "srcblendalpha",         ePARAM_TYPE_D3D11_BLEND,    (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, SrcBlendAlpha )},
    { "destblendalpha",        ePARAM_TYPE_D3D11_BLEND,    (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, DestBlendAlpha )},
    { "blendopalpha",          ePARAM_TYPE_D3D11_BLEND_OP, (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, BlendOpAlpha )},
    { "rendertargetwritemask", ePARAM_TYPE_UCHAR,          (UINT)offsetof(D3D11_RENDER_TARGET_BLEND_DESC, RenderTargetWriteMask )},
    { "", ePARAM_TYPE_TYPELESS, 0}
};

//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pDepthStencilDescMap[] =
{
    { "depthenable",                 ePARAM_TYPE_BOOL,                  (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, DepthEnable)},
    { "depthwritemask",              ePARAM_TYPE_DEPTH_WRITE_MASK,      (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, DepthWriteMask)},
    { "depthfunc",                   ePARAM_TYPE_D3D11_COMPARISON_FUNC, (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, DepthFunc)},
    { "StencilEnable",               ePARAM_TYPE_BOOL,                  (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, StencilEnable)},
    { "StencilReadMask",             ePARAM_TYPE_UCHAR,                 (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, StencilWriteMask)},
    { "StencilWriteMask",            ePARAM_TYPE_UCHAR,                 (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, DepthWriteMask)},
    { "FrontFaceStencilFailOp",      ePARAM_TYPE_D3D11_STENCIL_OP,      (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, FrontFace.StencilFailOp )},
    { "FrontFaceStencilDepthFailOp", ePARAM_TYPE_D3D11_STENCIL_OP,      (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, FrontFace.StencilDepthFailOp)},
    { "FrontFaceStencilPassOp",      ePARAM_TYPE_D3D11_STENCIL_OP,      (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, FrontFace.StencilPassOp)},
    { "FrontFaceStencilFunc",        ePARAM_TYPE_D3D11_COMPARISON_FUNC, (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, FrontFace.StencilFunc)},
    { "BackFaceStencilFailOp",       ePARAM_TYPE_D3D11_STENCIL_OP,      (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, BackFace.StencilDepthFailOp)},
    { "BackFaceStencilDepthFailOp",  ePARAM_TYPE_D3D11_STENCIL_OP,      (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, BackFace.StencilDepthFailOp)},
    { "BackFaceStencilPassOp",       ePARAM_TYPE_D3D11_STENCIL_OP,      (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, BackFace.StencilPassOp)},
    { "BackFaceStencilFunc",         ePARAM_TYPE_D3D11_COMPARISON_FUNC, (UINT)offsetof(D3D11_DEPTH_STENCIL_DESC, BackFace.StencilFunc)},
    { "", ePARAM_TYPE_TYPELESS, 0}
};

//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pRasterizerDescMap[] =
{
    { "FillMode",              ePARAM_TYPE_D3D11_FILL_MODE, (UINT)offsetof(D3D11_RASTERIZER_DESC, FillMode)},
    { "CullMode",              ePARAM_TYPE_D3D11_CULL_MODE, (UINT)offsetof(D3D11_RASTERIZER_DESC, CullMode)},
    { "FrontCounterClockwise", ePARAM_TYPE_BOOL,            (UINT)offsetof(D3D11_RASTERIZER_DESC, FrontCounterClockwise)},
    { "DepthBias",             ePARAM_TYPE_INT,             (UINT)offsetof(D3D11_RASTERIZER_DESC, DepthBias)},
    { "DepthBiasClamp",        ePARAM_TYPE_FLOAT,           (UINT)offsetof(D3D11_RASTERIZER_DESC, DepthBiasClamp)},
    { "SlopeScaledDepthBias",  ePARAM_TYPE_FLOAT,           (UINT)offsetof(D3D11_RASTERIZER_DESC, SlopeScaledDepthBias)},
    { "DepthClipEnable",       ePARAM_TYPE_BOOL,            (UINT)offsetof(D3D11_RASTERIZER_DESC, DepthClipEnable)},
    { "ScissorEnable",         ePARAM_TYPE_BOOL,            (UINT)offsetof(D3D11_RASTERIZER_DESC, ScissorEnable)},
    { "MultisampleEnable",     ePARAM_TYPE_BOOL,            (UINT)offsetof(D3D11_RASTERIZER_DESC, MultisampleEnable)},
    { "AntialiasedLineEnable", ePARAM_TYPE_BOOL,            (UINT)offsetof(D3D11_RASTERIZER_DESC, AntialiasedLineEnable)},
    { "", ePARAM_TYPE_TYPELESS, 0}
};

//-----------------------------------------------------------------------------
static const CPUTRenderStateMapEntry pSamplerDescMap[] =
{
    { "Filter",         ePARAM_TYPE_D3D11_FILTER,               (UINT)offsetof(D3D11_SAMPLER_DESC, Filter)},
    { "AddressU",       ePARAM_TYPE_D3D11_TEXTURE_ADDRESS_MODE, (UINT)offsetof(D3D11_SAMPLER_DESC, AddressU)},
    { "AddressV",       ePARAM_TYPE_D3D11_TEXTURE_ADDRESS_MODE, (UINT)offsetof(D3D11_SAMPLER_DESC, AddressV)},
    { "AddressW",       ePARAM_TYPE_D3D11_TEXTURE_ADDRESS_MODE, (UINT)offsetof(D3D11_SAMPLER_DESC, AddressW)},
    { "MipLODBias",     ePARAM_TYPE_FLOAT,                      (UINT)offsetof(D3D11_SAMPLER_DESC, MipLODBias)},
    { "MaxAnisotropy",  ePARAM_TYPE_UINT,                       (UINT)offsetof(D3D11_SAMPLER_DESC, MaxAnisotropy)},
    { "ComparisonFunc", ePARAM_TYPE_D3D11_COMPARISON_FUNC,      (UINT)offsetof(D3D11_SAMPLER_DESC, ComparisonFunc)},
    { "BorderColor0",   ePARAM_TYPE_FLOAT,                      (UINT)offsetof(D3D11_SAMPLER_DESC, BorderColor[0])},
    { "BorderColor1",   ePARAM_TYPE_FLOAT,                      (UINT)offsetof(D3D11_SAMPLER_DESC, BorderColor[1])},
    { "BorderColor2",   ePARAM_TYPE_FLOAT,                      (UINT)offsetof(D3D11_SAMPLER_DESC, BorderColor[2])},
    { "BorderColor3",   ePARAM_TYPE_FLOAT,                      (UINT)offsetof(D3D11_SAMPLER_DESC, BorderColor[3])},
    { "MinLOD",         ePARAM_TYPE_FLOAT,                      (UINT)offsetof(D3D11_SAMPLER_DESC, MinLOD)},
    { "MaxLOD",         ePARAM_TYPE_FLOAT,                      (UINT)offsetof(D3D11_SAMPLER_DESC, MaxLOD)},
    { "", ePARAM_TYPE_TYPELESS, 0}
};


//-----------------------------------------------------------------------------
static const StringToIntMapEntry pBlendMap[] = {
    { "d3d11_blend_zero",             D3D11_BLEND_ZERO },
    { "d3d11_blend_one",              D3D11_BLEND_ONE },
    { "d3d11_blend_src_color",        D3D11_BLEND_SRC_COLOR },
    { "d3d11_blend_inv_src_color",    D3D11_BLEND_INV_SRC_COLOR },
    { "d3d11_blend_src_alpha",        D3D11_BLEND_SRC_ALPHA },
    { "d3d11_blend_inv_src_alpha",    D3D11_BLEND_INV_SRC_ALPHA },
    { "d3d11_blend_dest_alpha",       D3D11_BLEND_DEST_ALPHA },
    { "d3d11_blend_inv_dest_alpha",   D3D11_BLEND_INV_DEST_ALPHA },
    { "d3d11_blend_dest_color",       D3D11_BLEND_DEST_COLOR },
    { "d3d11_blend_inv_dest_color",   D3D11_BLEND_INV_DEST_COLOR },
    { "d3d11_blend_src_alpha_sat",    D3D11_BLEND_SRC_ALPHA_SAT },
    { "d3d11_blend_blend_factor",     D3D11_BLEND_BLEND_FACTOR },
    { "d3d11_blend_inv_blend_factor", D3D11_BLEND_INV_BLEND_FACTOR },
    { "d3d11_blend_src1_color",       D3D11_BLEND_SRC1_COLOR },
    { "d3d11_blend_inv_src1_color",   D3D11_BLEND_INV_SRC1_COLOR },
    { "d3d11_blend_src1_alpha",       D3D11_BLEND_SRC1_ALPHA },
    { "d3d11_blend_inv_src1_alpha",   D3D11_BLEND_INV_SRC1_ALPHA },
    { "", -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pBlendOpMap[] = {
    { "d3d11_blend_op_add",          D3D11_BLEND_OP_ADD },
    { "d3d11_blend_op_subtract",     D3D11_BLEND_OP_SUBTRACT },
    { "d3d11_blend_op_rev_subtract", D3D11_BLEND_OP_REV_SUBTRACT },
    { "d3d11_blend_op_min",          D3D11_BLEND_OP_MIN },
    { "d3d11_blend_op_max",          D3D11_BLEND_OP_MAX },
    { "", -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pDepthWriteMaskMap[] = {
    { "D3D11_DEPTH_WRITE_MASK_ZERO", D3D11_DEPTH_WRITE_MASK_ZERO },
    { "D3D11_DEPTH_WRITE_MASK_ALL",  D3D11_DEPTH_WRITE_MASK_ALL },
    { "", -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pComparisonMap[] = {
    { "D3D11_COMPARISON_NEVER",         D3D11_COMPARISON_NEVER },
    { "D3D11_COMPARISON_LESS",          D3D11_COMPARISON_LESS },
    { "D3D11_COMPARISON_EQUAL",         D3D11_COMPARISON_EQUAL },
    { "D3D11_COMPARISON_LESS_EQUAL",    D3D11_COMPARISON_LESS_EQUAL },
    { "D3D11_COMPARISON_GREATER",       D3D11_COMPARISON_GREATER },
    { "D3D11_COMPARISON_NOT_EQUAL",     D3D11_COMPARISON_NOT_EQUAL},
    { "D3D11_COMPARISON_GREATER_EQUAL", D3D11_COMPARISON_GREATER_EQUAL},
    { "D3D11_COMPARISON_ALWAYS",        D3D11_COMPARISON_ALWAYS},
    { "", -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pStencilOpMap[] = {
    { "D3D11_STENCIL_OP_KEEP",     D3D11_STENCIL_OP_KEEP },
    { "D3D11_STENCIL_OP_ZERO",     D3D11_STENCIL_OP_ZERO },
    { "D3D11_STENCIL_OP_REPLACE",  D3D11_STENCIL_OP_REPLACE },
    { "D3D11_STENCIL_OP_INCR_SAT", D3D11_STENCIL_OP_INCR_SAT },
    { "D3D11_STENCIL_OP_DECR_SAT", D3D11_STENCIL_OP_DECR_SAT },
    { "D3D11_STENCIL_OP_INVERT",   D3D11_STENCIL_OP_INVERT },
    { "D3D11_STENCIL_OP_INCR",     D3D11_STENCIL_OP_INCR },
    { "D3D11_STENCIL_OP_DECR",     D3D11_STENCIL_OP_DECR },
    { "", -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pFillModeMap[] = {
    { "D3D11_FILL_WIREFRAME",      D3D11_FILL_WIREFRAME },
    { "D3D11_FILL_SOLID",          D3D11_FILL_SOLID },
    { "", -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pCullModeMap[] = {
    { "D3D11_CULL_NONE",           D3D11_CULL_NONE },
    { "D3D11_CULL_FRONT",          D3D11_CULL_FRONT },
    { "D3D11_CULL_BACK",           D3D11_CULL_BACK },
    { "", -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pFilterMap[] = {
    { "D3D11_FILTER_MIN_MAG_MIP_POINT",                          D3D11_FILTER_MIN_MAG_MIP_POINT },
    { "D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR",                   D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR },
    { "D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT",             D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT },
    { "D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR",                   D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR },
    { "D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT",                   D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT },
    { "D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR",            D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR },
    { "D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT",                   D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT },
    { "D3D11_FILTER_MIN_MAG_MIP_LINEAR",                         D3D11_FILTER_MIN_MAG_MIP_LINEAR },
    { "D3D11_FILTER_ANISOTROPIC",                                D3D11_FILTER_ANISOTROPIC },
    { "D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT",               D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT },
    { "D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR",        D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR },
    { "D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT",  D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT },
    { "D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR",        D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR },
    { "D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT",        D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT },
    { "D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR", D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR },
    { "D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT",        D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT },
    { "D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR",              D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR },
    { "D3D11_FILTER_COMPARISON_ANISOTROPIC",                     D3D11_FILTER_COMPARISON_ANISOTROPIC },
    // { "D3D11_FILTER_TEXT_1BIT",                                  D3D11_FILTER_TEXT_1BIT }, // DX docs list this, but not in actual structure
    { "", -1 }
};

//-----------------------------------------------------------------------------
static const StringToIntMapEntry pTextureAddressMap[] = {
    { "D3D11_TEXTURE_ADDRESS_WRAP",        D3D11_TEXTURE_ADDRESS_WRAP },
    { "D3D11_TEXTURE_ADDRESS_MIRROR",      D3D11_TEXTURE_ADDRESS_MIRROR },
    { "D3D11_TEXTURE_ADDRESS_CLAMP",       D3D11_TEXTURE_ADDRESS_CLAMP },
    { "D3D11_TEXTURE_ADDRESS_BORDER",      D3D11_TEXTURE_ADDRESS_BORDER },
    { "D3D11_TEXTURE_ADDRESS_MIRROR_ONCE", D3D11_TEXTURE_ADDRESS_MIRROR_ONCE },
    { "", -1 }
};

#endif //_CPUTRENDERSTATEMAPSDX11_H
