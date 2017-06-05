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
#include "OBJExporter.h"
#include <DirectX/CPUTMeshDX11.h>
#include <CPUTModel.h>
#include <array>
#include <memory>
#include <CPUTMaterial.h>
#include <CPUTMaterialDX11.h>
#include <CPUTTexture.h>
#include <CPUTTextureDX11.h>
#include <string>

#include <DirectXTex/DirectXTex/BC.h>
#include "../Extras/DirectXTex/SCreenGrab/ScreenGrab.h"
#include <DirectXTex/DirectXTex/DirectXTexP.h>
#include <ThrowHResult.h>

static HRESULT My_Convert(_In_ const Image& srcImage, _In_ DWORD filter, _In_ const Image& destImage, _In_ float threshold, _In_ size_t z);

static const XMVECTORF32 g_Grayscale = { 0.2125f, 0.7154f, 0.0721f, 0.0f };
static const XMVECTORF32 g_HalfMin = { -65504.f, -65504.f, -65504.f, -65504.f };
static const XMVECTORF32 g_HalfMax = { 65504.f, 65504.f, 65504.f, 65504.f };
static const XMVECTORF32 g_8BitBias = { 0.5f/255.f, 0.5f/255.f, 0.5f/255.f, 0.5f/255.f };

//-------------------------------------------------------------------------------------
// Convert scanline based on source/target formats
//-------------------------------------------------------------------------------------
struct ConvertData
{
    DXGI_FORMAT format;
    size_t datasize;
    DWORD flags;
};

static const ConvertData g_ConvertTable[] = {
    { DXGI_FORMAT_R32G32B32A32_FLOAT,           32, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R32G32B32A32_UINT,            32, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R32G32B32A32_SINT,            32, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R32G32B32_FLOAT,              32, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R32G32B32_UINT,               32, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R32G32B32_SINT,               32, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R16G16B16A16_FLOAT,           16, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R16G16B16A16_UNORM,           16, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R16G16B16A16_UINT,            16, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R16G16B16A16_SNORM,           16, CONVF_SNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R16G16B16A16_SINT,            16, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R32G32_FLOAT,                 32, CONVF_FLOAT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R32G32_UINT,                  32, CONVF_UINT | CONVF_R | CONVF_G  },
    { DXGI_FORMAT_R32G32_SINT,                  32, CONVF_SINT | CONVF_R | CONVF_G  },
    { DXGI_FORMAT_D32_FLOAT_S8X24_UINT,         32, CONVF_FLOAT | CONVF_DEPTH | CONVF_STENCIL },
    { DXGI_FORMAT_R10G10B10A2_UNORM,            10, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R10G10B10A2_UINT,             10, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R11G11B10_FLOAT,              10, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R8G8B8A8_UNORM,               8, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,          8, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R8G8B8A8_UINT,                8, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R8G8B8A8_SNORM,               8, CONVF_SNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R8G8B8A8_SINT,                8, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R16G16_FLOAT,                 16, CONVF_FLOAT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16G16_UNORM,                 16, CONVF_UNORM | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16G16_UINT,                  16, CONVF_UINT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16G16_SNORM,                 16, CONVF_SNORM | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16G16_SINT,                  16, CONVF_SINT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_D32_FLOAT,                    32, CONVF_FLOAT | CONVF_DEPTH },
    { DXGI_FORMAT_R32_FLOAT,                    32, CONVF_FLOAT | CONVF_R },
    { DXGI_FORMAT_R32_UINT,                     32, CONVF_UINT | CONVF_R },
    { DXGI_FORMAT_R32_SINT,                     32, CONVF_SINT | CONVF_R },
    { DXGI_FORMAT_D24_UNORM_S8_UINT,            32, CONVF_UNORM | CONVF_DEPTH | CONVF_STENCIL },
    { DXGI_FORMAT_R8G8_UNORM,                   8, CONVF_UNORM | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R8G8_UINT,                    8, CONVF_UINT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R8G8_SNORM,                   8, CONVF_SNORM | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R8G8_SINT,                    8, CONVF_SINT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16_FLOAT,                    16, CONVF_FLOAT | CONVF_R },
    { DXGI_FORMAT_D16_UNORM,                    16, CONVF_UNORM | CONVF_DEPTH },
    { DXGI_FORMAT_R16_UNORM,                    16, CONVF_UNORM | CONVF_R },
    { DXGI_FORMAT_R16_UINT,                     16, CONVF_UINT | CONVF_R },
    { DXGI_FORMAT_R16_SNORM,                    16, CONVF_SNORM | CONVF_R },
    { DXGI_FORMAT_R16_SINT,                     16, CONVF_SINT | CONVF_R },
    { DXGI_FORMAT_R8_UNORM,                     8, CONVF_UNORM | CONVF_R },
    { DXGI_FORMAT_R8_UINT,                      8, CONVF_UINT | CONVF_R },
    { DXGI_FORMAT_R8_SNORM,                     8, CONVF_SNORM | CONVF_R },
    { DXGI_FORMAT_R8_SINT,                      8, CONVF_SINT | CONVF_R },
    { DXGI_FORMAT_A8_UNORM,                     8, CONVF_UNORM | CONVF_A },
    { DXGI_FORMAT_R1_UNORM,                     1, CONVF_UNORM | CONVF_R },
    { DXGI_FORMAT_R9G9B9E5_SHAREDEXP,           9, CONVF_SHAREDEXP | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R8G8_B8G8_UNORM,              8, CONVF_UNORM | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_G8R8_G8B8_UNORM,              8, CONVF_UNORM | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_BC1_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC1_UNORM_SRGB,               8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC2_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC2_UNORM_SRGB,               8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC3_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC3_UNORM_SRGB,               8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC4_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R },
    { DXGI_FORMAT_BC4_SNORM,                    8, CONVF_SNORM | CONVF_BC | CONVF_R },
    { DXGI_FORMAT_BC5_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G },
    { DXGI_FORMAT_BC5_SNORM,                    8, CONVF_SNORM | CONVF_BC | CONVF_R | CONVF_G },
    { DXGI_FORMAT_B5G6R5_UNORM,                 5, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_B5G5R5A1_UNORM,               5, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_B8G8R8A8_UNORM,               8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_B8G8R8X8_UNORM,               8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,   10, CONVF_UNORM | CONVF_XR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,          8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,          8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_BC6H_UF16,                    16, CONVF_FLOAT | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC6H_SF16,                    16, CONVF_FLOAT | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC7_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC7_UNORM_SRGB,               8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_AYUV,                         8, CONVF_UNORM | CONVF_YUV | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_Y410,                         10, CONVF_UNORM | CONVF_YUV | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_Y416,                         16, CONVF_UNORM | CONVF_YUV | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_YUY2,                         8, CONVF_UNORM | CONVF_YUV | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_Y210,                         10, CONVF_UNORM | CONVF_YUV | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_Y216,                         16, CONVF_UNORM | CONVF_YUV | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_B4G4R4A4_UNORM,               4, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT,  10, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT,  10, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM,10, CONVF_SNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { XBOX_DXGI_FORMAT_R4G4_UNORM,               4, CONVF_UNORM | CONVF_R | CONVF_G },
};

struct RemapVertex
{
	float3 position;
	float3 normal;
	float2 uv;
	float2 bary;
};

bool CopyVerticesFromMesh(
	RemapVertex *pV1,
	RemapVertex *pV2,
	RemapVertex *pV3,
	const int index_0,
	const int index_1,
	const int index_2,
	const void *pVertData,
	const CPUTMesh *pMesh
	)
{
	// Cache offset data so we only need to look it up once
	// per mesh change
	static const CPUTMesh *s_pMesh;
	static int s_pos_offset = -1;
	static int s_uv_offset = -1;
	static int s_normal_offset = -1;

	static unsigned int s_stride;

	if (pMesh != s_pMesh)
	{
		s_pMesh = pMesh;

		s_pos_offset = -1;
		s_uv_offset = -1;
		s_normal_offset = -1;

		CPUTMeshDX11 *pDx11Mesh = (CPUTMeshDX11 *)pMesh;

		s_stride = pDx11Mesh->GetVertexStride();

		D3D11_INPUT_ELEMENT_DESC *pDesc = pDx11Mesh->GetLayoutDescription();

		int i = 0;

		while (true)
		{
			if (0 == pDesc[i].Format)
				break;

			if (-1 == s_pos_offset && 0 == strcmp(pDesc[i].SemanticName, "POSITION"))
				s_pos_offset = pDesc[i].AlignedByteOffset;
			else if (-1 == s_uv_offset && 0 == strcmp(pDesc[i].SemanticName, "TEXCOORD"))
				s_uv_offset = pDesc[i].AlignedByteOffset;
			else if (-1 == s_normal_offset && 0 == strcmp(pDesc[i].SemanticName, "NORMAL"))
				s_normal_offset = pDesc[i].AlignedByteOffset;

			if (-1 != s_uv_offset && -1 != s_pos_offset && -1 != s_normal_offset)
				break;

			++i;
		}
	}

	if (-1 == s_pos_offset || -1 == s_uv_offset)
		return false;

	const byte *pVerts = (const byte *)pVertData;

	if (NULL != pV1)
	{
		const byte *pVert1 = pVerts + index_0 * s_stride;

		pV1->position = *(float3 *)(pVert1 + s_pos_offset);
		pV1->normal = *(float3 *)(pVert1 + s_normal_offset);
		pV1->uv = *(float2 *)(pVert1 + s_uv_offset);
		pV1->bary = float2(1, 0);
	}

	if (NULL != pV2)
	{
		const byte *pVert2 = pVerts + index_1 * s_stride;

		pV2->position = *(float3 *)(pVert2 + s_pos_offset);
		pV2->normal = *(float3 *)(pVert2 + s_normal_offset);
		pV2->uv = *(float2 *)(pVert2 + s_uv_offset);
		pV2->bary = float2(0, 1);
	}

	if (NULL != pV3)
	{
		const byte *pVert3 = pVerts + index_2 * s_stride;

		pV3->position = *(float3 *)(pVert3 + s_pos_offset);
		pV3->normal = *(float3 *)(pVert3 + s_normal_offset);
		pV3->uv = *(float2 *)(pVert3 + s_uv_offset);
		pV3->bary = float2(0, 0);
	}

	return true;
}

#pragma prefast( suppress : 25004, "Signature must match bsearch_s" );
static int __cdecl _ConvertCompare( void *context, const void* ptr1, const void *ptr2 )
{
    UNREFERENCED_PARAMETER(context);
    const ConvertData *p1 = reinterpret_cast<const ConvertData*>(ptr1);
    const ConvertData *p2 = reinterpret_cast<const ConvertData*>(ptr2);
    if ( p1->format == p2->format ) return 0;
    else return (p1->format < p2->format ) ? -1 : 1;
}

void My_ConvertScanline( XMVECTOR* pBuffer, size_t count, DXGI_FORMAT outFormat, DXGI_FORMAT inFormat, DWORD flags)
{
    assert( pBuffer && count > 0 && (((uintptr_t)pBuffer & 0xF) == 0) );
    assert( IsValid(outFormat) && !IsTypeless(outFormat) && !IsPlanar(outFormat) && !IsPalettized(outFormat) );
    assert( IsValid(inFormat) && !IsTypeless(inFormat) && !IsPlanar(inFormat) && !IsPalettized(inFormat) );

    if ( !pBuffer )
        return;

    // Determine conversion details about source and dest formats
    ConvertData key = { inFormat, 0 };
    const ConvertData* in = (const ConvertData*) bsearch_s( &key, g_ConvertTable, _countof(g_ConvertTable), sizeof(ConvertData),
                                                            _ConvertCompare, 0 );

    key.format = outFormat;
    if ( !in)
    {
        assert(false);
        return;
    }

    assert( _GetConvertFlags( inFormat ) == in->flags );

    // Handle SRGB filtering modes
    switch ( inFormat )
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        flags |= TEX_FILTER_SRGB_IN;
        break;

    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        flags &= ~TEX_FILTER_SRGB_IN;
        break;
    }

    switch ( outFormat )
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        flags |= TEX_FILTER_SRGB_OUT;
        break;

    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        flags &= ~TEX_FILTER_SRGB_OUT;
        break;
    }

    if ( (flags & (TEX_FILTER_SRGB_IN|TEX_FILTER_SRGB_OUT)) == (TEX_FILTER_SRGB_IN|TEX_FILTER_SRGB_OUT) )
    {
        flags &= ~(TEX_FILTER_SRGB_IN|TEX_FILTER_SRGB_OUT);
    }

    // sRGB input processing (sRGB -> Linear RGB)
    if ( flags & TEX_FILTER_SRGB_IN )
    {
        if ( !(in->flags & CONVF_DEPTH) && ( (in->flags & CONVF_FLOAT) || (in->flags & CONVF_UNORM) ) )
        {
            XMVECTOR* ptr = pBuffer;
            for( size_t i=0; i < count; ++i, ++ptr )
            {
                *ptr = XMColorSRGBToRGB( *ptr );
            }
        }
    }

	XMVECTOR* ptr = pBuffer;
	for (size_t i = 0; i < count; ++i)
	{
		XMVECTOR v = *ptr;
		XMVECTOR v1 = XMVectorSplatW(v);
		*ptr++ = v1;
	}
}


HRESULT _Convert( const Image& srcImage, DXGI_FORMAT format, DWORD filter, float threshold, ScratchImage& image)
{
    if ( (srcImage.format == format) || !IsValid( format ) )
        return E_INVALIDARG;

    if ( !srcImage.pixels )
        return E_POINTER;

    if ( IsCompressed(srcImage.format) || IsCompressed(format)
         || IsPlanar(srcImage.format) || IsPlanar(format)
         || IsPalettized(srcImage.format) || IsPalettized(format)
         || IsTypeless(srcImage.format) || IsTypeless(format) )
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );

#ifdef _M_X64
    if ( (srcImage.width > 0xFFFFFFFF) || (srcImage.height > 0xFFFFFFFF) )
        return E_INVALIDARG;
#endif

    HRESULT hr = image.Initialize2D( format, srcImage.width, srcImage.height, 1, 1 );
    if ( FAILED(hr) )
        return hr;

    const Image *rimage = image.GetImage( 0, 0, 0 );
    if ( !rimage )
    {
        image.Release();
        return E_POINTER;
    }

	hr = My_Convert(srcImage, filter, *rimage, threshold, 0);

    if ( FAILED(hr) )
    {
        image.Release();
        return hr;
    }

    return S_OK;
}

static HRESULT My_Convert( _In_ const Image& srcImage, _In_ DWORD filter, _In_ const Image& destImage, _In_ float threshold, _In_ size_t z)
{
    assert( srcImage.width == destImage.width );
    assert( srcImage.height == destImage.height );

    const uint8_t *pSrc = srcImage.pixels;
    uint8_t *pDest = destImage.pixels;
    if ( !pSrc || !pDest )
        return E_POINTER;

    size_t width = srcImage.width;

	ScopedAlignedArrayXMVECTOR scanline(reinterpret_cast<XMVECTOR*>(_aligned_malloc((sizeof(XMVECTOR)*width), 16)));
	if (!scanline)
		return E_OUTOFMEMORY;

	// No dithering
	for (size_t h = 0; h < srcImage.height; ++h)
	{
		if (!_LoadScanline(scanline.get(), width, pSrc, srcImage.rowPitch, srcImage.format))
			return E_FAIL;

		My_ConvertScanline(scanline.get(), width, destImage.format, srcImage.format, filter);

		if (!_StoreScanline(pDest, destImage.rowPitch, destImage.format, scanline.get(), width, threshold))
			return E_FAIL;

		pSrc += srcImage.rowPitch;
		pDest += destImage.rowPitch;
    }

    return S_OK;
}


std::wstring string2wstring(std::string& str)
{
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.from_bytes(str);
}


void SaveAlpha( const Image& srcImage, DWORD filter, const Image& destImage, float threshold, size_t z ) {

	assert(srcImage.width == destImage.width);
	assert(srcImage.height == destImage.height);

	const uint8_t *pSrc = srcImage.pixels;
	uint8_t *pDest = destImage.pixels;

	size_t width = srcImage.width;

	ScopedAlignedArrayXMVECTOR scanline(reinterpret_cast<XMVECTOR*>(_aligned_malloc((sizeof(XMVECTOR)*width), 16)));

	if (!scanline)
		throw std::exception("Out of memory");

	for (size_t h = 0; h < srcImage.height; ++h)
	{
		if (!_LoadScanline(scanline.get(), width, pSrc, srcImage.rowPitch, srcImage.format))
			throw std::exception("Failed to load scanline");

		_ConvertScanline(scanline.get(), width, destImage.format, srcImage.format, filter);

		if (!_StoreScanline(pDest, destImage.rowPitch, destImage.format, scanline.get(), width, threshold))
			throw std::exception("Failed to store scanline");

		pSrc += srcImage.rowPitch;
		pDest += destImage.rowPitch;
	}
}

bool OBJExporter::ExportModel( CPUTModel* cputModel, CPUTRenderParameters const& renderParams, int materialPass ) {

	if(cputModel == nullptr) {
		return false;
	}

	renderParameters = renderParams;

	int numMeshes = cputModel->GetMeshCount();
	std::vector<std::string> materialNames;

	int currentSize = (int)objMaterials.size();
	objMaterials.resize(currentSize + numMeshes);

	// Extract the materials
	for (int i = 0; i < numMeshes; ++i) {
		auto mesh = cputModel->GetMesh(i);
		modelMeshes.push_back(std::make_pair(mesh, currentSize+i));

		int vertex_count = static_cast<CPUTMeshDX11 *>(mesh)->GetVertexCount();

		CPUTMaterialDX11* material = static_cast<CPUTMaterialDX11*>(cputModel->GetMaterial(i, materialPass));
		auto tt = *material->GetMaterialName();
		materialNames.push_back(tt);

		objMaterials[currentSize + i].materialName = CPUTFileSystem::basename(*material->GetMaterialName(), true);

		std::vector<std::string> TexNames;

		// Extract texture names

		std::array<std::pair<std::string, OBJTextureType>, 3> validTextures = {
			std::make_pair("BaseTexture", OBJTextureType::Diffuse)
			, std::make_pair("NormalTexture", OBJTextureType::Bump)
			, std::make_pair("AmbientTexture", OBJTextureType::Ambient)
		};

		CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
		auto mediaPath = pAssetLibrary->GetMediaDirectoryName();
		auto texPath = pAssetLibrary->GetTextureDirectoryName();

		auto pp = pAssetLibrary->GetTextureDirectoryName();

		for (int j = 0; j < material->mPixelShaderParameters.mTextureCount; ++j) {

			std::string TexBindName = material->mPixelShaderParameters.mpTextureName[j];

			if (material->mPixelShaderParameters.mpTexture[j]) {
				auto iter = std::find_if(std::begin(validTextures), std::end(validTextures), [TexBindName](std::pair<std::string, OBJTextureType> const& element) { return element.first == TexBindName;});
				if (iter != std::end(validTextures)) {
					std::string theName = material->mPixelShaderParameters.mpTexture[j]->GetName();

					auto texData = static_cast<CPUTTextureDX11*> (material->mPixelShaderParameters.mpTexture[j]);
					auto nativeTexture = texData->GetTexture();

					auto aa = CPUTFileSystem::dirname(mFilePath) + "\\" + CPUTFileSystem::basename(theName, true);

					// Try and save the texture directly
					HRESULT hr = SaveWICTextureToFile(
						CPUT_DX11::GetContext(),
						nativeTexture,
						GUID_ContainerFormatPng,
						string2wstring(aa + ".png").c_str(),
						&GUID_WICPixelFormat32bppBGR);

					ScratchImage CapturedImage;
					CaptureTexture(CPUT_DX11::GetDevice(), CPUT_DX11::GetContext(), nativeTexture, CapturedImage);

					if(!SUCCEEDED(hr)) {
						// The native format was not convertiable VIA WIC(Probably compressed)

						TexMetadata info = CapturedImage.GetMetadata();

						// Decompress
						auto img = CapturedImage.GetImage(0, 0, 0);
						size_t nimg = CapturedImage.GetImageCount();

						std::unique_ptr<ScratchImage> ConvertedImage(new (std::nothrow) ScratchImage);
						std::unique_ptr<ScratchImage> DecompressedImage(new ScratchImage);

						ThrowHResult(Decompress(img, nimg, info, DXGI_FORMAT_UNKNOWN /* picks good default */, *DecompressedImage));
						ThrowHResult(Convert(DecompressedImage->GetImages()[0], DXGI_FORMAT_R8G8B8A8_UNORM, TEX_FILTER_SRGB, 0.5f, *ConvertedImage));
						ThrowHResult(SaveToWICFile(*ConvertedImage->GetImage(0, 0, 0), (DWORD)0, GUID_ContainerFormatPng, string2wstring(aa + ".png").c_str(), &GUID_WICPixelFormat32bppBGRA));


						TexNames.push_back(theName);
						objMaterials[currentSize + i].AssignTexture(CPUTFileSystem::basename(theName, true) + ".png", iter->second);

						// Add a dissolve texture to handle alpha
						objMaterials[currentSize + i].AssignTexture(CPUTFileSystem::basename(theName, true) + "_alpha.png", OBJTextureType::Dissolve);

						std::unique_ptr<ScratchImage> timage(new ScratchImage);


						ThrowHResult(_Convert(DecompressedImage->GetImages()[0],    DXGI_FORMAT_R32G32B32_FLOAT, TEX_FILTER_SRGB, 0.5f, *timage));
						ThrowHResult(SaveToWICFile(*timage->GetImage(0, 0, 0), (DWORD)0, GUID_ContainerFormatPng, string2wstring(aa + "_alpha.png").c_str(), &GUID_WICPixelFormat32bppBGRA));
					} else {
						TexNames.push_back(theName);
						objMaterials[currentSize + i].AssignTexture(CPUTFileSystem::basename(theName, true) + ".png", iter->second);

						// Add a dissolve texture to handle alpha
						objMaterials[currentSize + i].AssignTexture(CPUTFileSystem::basename(theName, true) + "_alpha.png", OBJTextureType::Dissolve);

						std::unique_ptr<ScratchImage> timage(new ScratchImage);

						ThrowHResult(_Convert(CapturedImage.GetImages()[0],   DXGI_FORMAT_R32G32B32_FLOAT,  TEX_FILTER_SRGB, 0.5f, *timage));
						ThrowHResult(SaveToWICFile(*timage->GetImage(0, 0, 0), (DWORD)0, GUID_ContainerFormatPng, string2wstring(aa + "_alpha.png").c_str(), &GUID_WICPixelFormat32bppBGRA));
					}


				}
			}
		}
	}


	std::string MaterialPath = CPUTFileSystem::dirname(mFilePath) + "\\" + CPUTFileSystem::basename(mFilePath, true) + ".mtl";
	std::string MaterialName = CPUTFileSystem::basename(MaterialPath, true) + ".mtl";


	std::stringstream objFile;

	int j = 0, i = 0, numVerts = 0;
	for(auto item : modelMeshes) {

		item.first->FreeStagingIndexBuffer();
		item.first->FreeStagingVertexBuffer();
		D3D11_MAPPED_SUBRESOURCE verts = item.first->MapVertices(renderParameters, CPUT_MAP_READ);
		D3D11_MAPPED_SUBRESOURCE indices = item.first->MapIndices(renderParameters, CPUT_MAP_READ);

		int index_count = static_cast<CPUTMeshDX11 *>(item.first)->GetIndexCount();
		int vertex_count = static_cast<CPUTMeshDX11 *>(item.first)->GetVertexCount();

		RemapVertex v1, v2, v3;

		objFile << "o " << "mesh" << j++ << std::endl;

		for (i = 0; i < vertex_count; i++) {
			if (false == CopyVerticesFromMesh(&v1, NULL, NULL, i, -1, -1, verts.pData, item.first))
				break;

			objFile << "v " << std::setprecision(4) << std::fixed << -v1.position.x << " " << v1.position.y << " " << v1.position.z << std::endl;
		}

		for (i = 0; i < vertex_count; i++)
		{
			if (false == CopyVerticesFromMesh(&v1, NULL, NULL, i, -1, -1, verts.pData, item.first))
				break;

			objFile << "vt " << std::setprecision(4) << std::fixed << (v1.uv.x) << " " << (1.0f - v1.uv.y) << std::endl;
		}

		for (i = 0; i < vertex_count; i++)
		{
			if (false == CopyVerticesFromMesh(&v1, NULL, NULL, i, -1, -1, verts.pData, item.first))
				break;

			objFile << "vn " << std::setprecision(4) << std::fixed << -v1.normal.x << " " << v1.normal.y << " " << v1.normal.z << std::endl;
		}

		objFile << "s off" << std::endl;

		OBJMaterial* mat = &objMaterials[item.second];
		objFile << "usemtl " << mat->materialName << std::endl;

		uint16 *pIndices = (uint16 *)indices.pData;

		for (i = 0; i < index_count; i += 3)
		{
			int vert_id_1 = currentNumVerts + numVerts + pIndices[i + 0];
			int vert_id_2 = currentNumVerts + numVerts + pIndices[i + 2];
			int vert_id_3 = currentNumVerts + numVerts + pIndices[i + 1];

			objFile << "f " <<
				(vert_id_1 + 1) << "/" << (vert_id_1 + 1) << "/" << (vert_id_1 + 1) << " " <<
				(vert_id_2 + 1) << "/" << (vert_id_2 + 1) << "/" << (vert_id_2 + 1) << " " <<
				(vert_id_3 + 1) << "/" << (vert_id_3 + 1) << "/" << (vert_id_3 + 1) << " "
				<< std::endl;
		}

		// -----

		item.first->UnmapVertices(renderParameters);
		item.first->UnmapIndices(renderParameters);
		item.first->FreeStagingIndexBuffer();
		item.first->FreeStagingVertexBuffer();

		numVerts += vertex_count;
	}
	currentNumVerts += numVerts;


	RawObjects.push_back(objFile.str());


	return true;
}

void OBJExporter::ExportObject( D3D11_MAPPED_SUBRESOURCE verts, D3D11_MAPPED_SUBRESOURCE indices, int numVerts, int numInd, OBJMaterial objMaterial, const std::string& name, CPUTMesh* mesh ) {

	// Copy the material definition
	objMaterials.push_back(objMaterial);

	std::stringstream objFile;

	int j = 0, i = 0;

	RemapVertex v1, v2, v3;

	objFile << "o " << name << j++ << std::endl;

	for (i = 0; i < numVerts; i++) {
		if (false == CopyVerticesFromMesh(&v1, NULL, NULL, i, -1, -1, verts.pData, mesh))
			break;

		objFile << "v " << std::setprecision(4) << std::fixed << v1.position.x << " " << v1.position.y << " " << v1.position.z << std::endl;
	}

	for (i = 0; i < numVerts; i++)
	{
		if (false == CopyVerticesFromMesh(&v1, NULL, NULL, i, -1, -1, verts.pData, mesh))
			break;

		objFile << "vt " << std::setprecision(4) << std::fixed << v1.uv.x << " " << v1.uv.y << std::endl;
	}

	for (i = 0; i < numVerts; i++)
	{
		if (false == CopyVerticesFromMesh(&v1, NULL, NULL, i, -1, -1, verts.pData, mesh))
			break;

		objFile << "vn " << std::setprecision(4) << std::fixed << v1.normal.x << " " << v1.normal.y << " " << v1.normal.z << std::endl;
	}

	objFile << "s off" << std::endl;

	OBJMaterial* mat = &objMaterial;
	objFile << "usemtl " << mat->materialName << std::endl;

	int *pIndices = (int *)indices.pData;

	for (i = 0; i < numInd; i += 3)
	{
		int vert_id_1 = currentNumVerts + pIndices[i + 0];
		int vert_id_2 = currentNumVerts + pIndices[i + 1];
		int vert_id_3 = currentNumVerts + pIndices[i + 2];

		objFile << "f " <<
			(vert_id_1 + 1) << "/" << (vert_id_1 + 1) << "/" << (vert_id_1 + 1) << " " <<
			(vert_id_2 + 1) << "/" << (vert_id_2 + 1) << "/" << (vert_id_2 + 1) << " " <<
			(vert_id_3 + 1) << "/" << (vert_id_3 + 1) << "/" << (vert_id_3 + 1) << " "
			<< std::endl;
	}

	RawObjects.push_back(objFile.str());

	currentNumVerts += numVerts;
}

std::ostream& operator<<(std::ostream& os, const OBJMaterial& mat) {

    os << "newmtl " << mat.materialName << std::endl;

    os << "Ka " << std::fixed << std::setprecision(3) << mat.mAmbient.x << " " << mat.mAmbient.y << " " << mat.mAmbient.z << std::endl;
    os << "Kd " << std::fixed << std::setprecision(3) << mat.mDiffuse.x << " " << mat.mDiffuse.y << " " << mat.mDiffuse.z << std::endl;
    os << "Ks " << std::fixed << std::setprecision(3) << mat.mSpectral.x << " " << mat.mSpectral.y << " " << mat.mSpectral.z << std::endl;

    os << "illum " << static_cast<int>(mat.mIllumination) << std::endl;

    os << "d " << std::fixed << std::setprecision(3) << mat.mDissolve << std::endl;


    // TODO: Ka Spectral
    // TODO: Ka xyz
    // TODO: Kd Spectral
    // TODO: Kd xyz
    // TODO: Ks Spectral
    // TODO: Ks xyz
    // TODO: Transmission filter (Tf)
    // TODO: Specular exponent (Ns)
    // TODO: sharpness
    // TODO: Optical Density(Ni)
    // TODO: Dissolve with halo
    // TODO: Texturemap options
    // TODO: Reflection map

    os << mat.Ambient;
    os << mat.Diffuse;
    os << mat.Reflectivity;
    os << mat.Specular;
    os << mat.Dissolve;
    os << mat.Displacement;
    os << mat.Decal;
    os << mat.Bump;

    return os;
}

std::ostream& operator<<(std::ostream& os, const OBJTexture& mat) {

    if (mat.mFileName == "")
        return os;

    switch(mat.mType) {
        case OBJTextureType::Ambient:
            os << "map_Ka ";
            break;
        case OBJTextureType::Diffuse:
            os << "map_Kd ";
            break;
        case OBJTextureType::Specular:
            os << "map_Ks ";
            break;
        case OBJTextureType::Reflectivity:
            os << "map_Ns ";
            break;
        case OBJTextureType::Dissolve:
            os << "map_d ";
            break;
        case OBJTextureType::Displacement:
            os << "disp ";
            break;
        case OBJTextureType::Decal:
            os << "decal ";
            break;
        case OBJTextureType::Bump:
            os << "bump ";
            break;
        default: break;
    }
    os << mat.mFileName << std::endl;
    return os;
}
