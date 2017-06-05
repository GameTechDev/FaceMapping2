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

#include "CPUTTextureDX11.h"

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "CPUTSoftwareTexture.h"

// TODO: Would be nice to find a better place for this decl.  But, not another file just for this.
const std::string gDXGIFormatNames[] =
{
    "DXGI_FORMAT_UNKNOWN",
    "DXGI_FORMAT_R32G32B32A32_TYPELESS",
    "DXGI_FORMAT_R32G32B32A32_FLOAT",
    "DXGI_FORMAT_R32G32B32A32_UINT",
    "DXGI_FORMAT_R32G32B32A32_SINT",
    "DXGI_FORMAT_R32G32B32_TYPELESS",
    "DXGI_FORMAT_R32G32B32_FLOAT",
    "DXGI_FORMAT_R32G32B32_UINT",
    "DXGI_FORMAT_R32G32B32_SINT",
    "DXGI_FORMAT_R16G16B16A16_TYPELESS",
    "DXGI_FORMAT_R16G16B16A16_FLOAT",
    "DXGI_FORMAT_R16G16B16A16_UNORM",
    "DXGI_FORMAT_R16G16B16A16_UINT",
    "DXGI_FORMAT_R16G16B16A16_SNORM",
    "DXGI_FORMAT_R16G16B16A16_SINT",
    "DXGI_FORMAT_R32G32_TYPELESS",
    "DXGI_FORMAT_R32G32_FLOAT",
    "DXGI_FORMAT_R32G32_UINT",
    "DXGI_FORMAT_R32G32_SINT",
    "DXGI_FORMAT_R32G8X24_TYPELESS",
    "DXGI_FORMAT_D32_FLOAT_S8X24_UINT",
    "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",
    "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",
    "DXGI_FORMAT_R10G10B10A2_TYPELESS",
    "DXGI_FORMAT_R10G10B10A2_UNORM",
    "DXGI_FORMAT_R10G10B10A2_UINT",
    "DXGI_FORMAT_R11G11B10_FLOAT",
    "DXGI_FORMAT_R8G8B8A8_TYPELESS",
    "DXGI_FORMAT_R8G8B8A8_UNORM",
    "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",
    "DXGI_FORMAT_R8G8B8A8_UINT",
    "DXGI_FORMAT_R8G8B8A8_SNORM",
    "DXGI_FORMAT_R8G8B8A8_SINT",
    "DXGI_FORMAT_R16G16_TYPELESS",
    "DXGI_FORMAT_R16G16_FLOAT",
    "DXGI_FORMAT_R16G16_UNORM",
    "DXGI_FORMAT_R16G16_UINT",
    "DXGI_FORMAT_R16G16_SNORM",
    "DXGI_FORMAT_R16G16_SINT",
    "DXGI_FORMAT_R32_TYPELESS",
    "DXGI_FORMAT_D32_FLOAT",
    "DXGI_FORMAT_R32_FLOAT",
    "DXGI_FORMAT_R32_UINT",
    "DXGI_FORMAT_R32_SINT",
    "DXGI_FORMAT_R24G8_TYPELESS",
    "DXGI_FORMAT_D24_UNORM_S8_UINT",
    "DXGI_FORMAT_R24_UNORM_X8_TYPELESS",
    "DXGI_FORMAT_X24_TYPELESS_G8_UINT",
    "DXGI_FORMAT_R8G8_TYPELESS",
    "DXGI_FORMAT_R8G8_UNORM",
    "DXGI_FORMAT_R8G8_UINT",
    "DXGI_FORMAT_R8G8_SNORM",
    "DXGI_FORMAT_R8G8_SINT",
    "DXGI_FORMAT_R16_TYPELESS",
    "DXGI_FORMAT_R16_FLOAT",
    "DXGI_FORMAT_D16_UNORM",
    "DXGI_FORMAT_R16_UNORM",
    "DXGI_FORMAT_R16_UINT",
    "DXGI_FORMAT_R16_SNORM",
    "DXGI_FORMAT_R16_SINT",
    "DXGI_FORMAT_R8_TYPELESS",
    "DXGI_FORMAT_R8_UNORM",
    "DXGI_FORMAT_R8_UINT",
    "DXGI_FORMAT_R8_SNORM",
    "DXGI_FORMAT_R8_SINT",
    "DXGI_FORMAT_A8_UNORM",
    "DXGI_FORMAT_R1_UNORM",
    "DXGI_FORMAT_R9G9B9E5_SHAREDEXP",
    "DXGI_FORMAT_R8G8_B8G8_UNORM",
    "DXGI_FORMAT_G8R8_G8B8_UNORM",
    "DXGI_FORMAT_BC1_TYPELESS",
    "DXGI_FORMAT_BC1_UNORM",
    "DXGI_FORMAT_BC1_UNORM_SRGB",
    "DXGI_FORMAT_BC2_TYPELESS",
    "DXGI_FORMAT_BC2_UNORM",
    "DXGI_FORMAT_BC2_UNORM_SRGB",
    "DXGI_FORMAT_BC3_TYPELESS",
    "DXGI_FORMAT_BC3_UNORM",
    "DXGI_FORMAT_BC3_UNORM_SRGB",
    "DXGI_FORMAT_BC4_TYPELESS",
    "DXGI_FORMAT_BC4_UNORM",
    "DXGI_FORMAT_BC4_SNORM",
    "DXGI_FORMAT_BC5_TYPELESS",
    "DXGI_FORMAT_BC5_UNORM",
    "DXGI_FORMAT_BC5_SNORM",
    "DXGI_FORMAT_B5G6R5_UNORM",
    "DXGI_FORMAT_B5G5R5A1_UNORM",
    "DXGI_FORMAT_B8G8R8A8_UNORM",
    "DXGI_FORMAT_B8G8R8X8_UNORM",
    "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM",
    "DXGI_FORMAT_B8G8R8A8_TYPELESS",
    "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB",
    "DXGI_FORMAT_B8G8R8X8_TYPELESS",
    "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB",
    "DXGI_FORMAT_BC6H_TYPELESS",
    "DXGI_FORMAT_BC6H_UF16",
    "DXGI_FORMAT_BC6H_SF16",
    "DXGI_FORMAT_BC7_TYPELESS",
    "DXGI_FORMAT_BC7_UNORM",
    "DXGI_FORMAT_BC7_UNORM_SRGB"
};
const std::string *gpDXGIFormatNames = gDXGIFormatNames;

CPUTTextureDX11::~CPUTTextureDX11() {
	SAFE_RELEASE(mpShaderResourceView);
	SAFE_RELEASE(mpTexture);
	SAFE_RELEASE(mpTextureStaging);
	SAFE_DELETE(mpSoftwareTexture);
}

//-----------------------------------------------------------------------------
CPUTTexture *CPUTTextureDX11::Create( const std::string &name, const std::string &absolutePathAndFilename, bool loadAsSRGB )
{
    // TODO:  Delegate to derived class.  We don't currently have CPUTTextureDX11
    ID3D11ShaderResourceView *pShaderResourceView = NULL;
    ID3D11Resource *pTexture = NULL;
    ID3D11Device *pD3dDevice= CPUT_DX11::GetDevice();
    CPUTResult result = CreateNativeTexture( pD3dDevice, absolutePathAndFilename, &pShaderResourceView, &pTexture, loadAsSRGB );
    ASSERT( CPUTSUCCESS(result), "Error loading texture: '"+absolutePathAndFilename );
    UNREFERENCED_PARAMETER(result);

    CPUTTextureDX11 *pNewTexture = new CPUTTextureDX11();
    pNewTexture->mName = name;
    pNewTexture->SetTextureAndShaderResourceView( pTexture, pShaderResourceView );
    pTexture->Release();
    pShaderResourceView->Release();


    return pNewTexture;
}

// TODO: FACESCAN2
CPUTTexture* CPUTTextureDX11::CreateFromRenderTarget(std::string &name, CPUTRenderTargetColor *rt)
{
	ID3D11ShaderResourceView *pShaderResourceView = NULL;
	ID3D11Device *pD3dDevice = CPUT_DX11::GetDevice();
	ID3D11DeviceContext *context= CPUT_DX11::GetContext();
	ID3D11Texture2D *pTexture = NULL;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = rt->GetWidth();
	desc.Height = rt->GetHeight();
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.MipLevels = 1;

	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Format = rt->GetColorFormat();
	desc.Usage = D3D11_USAGE_DEFAULT;
	HRESULT hr = pD3dDevice->CreateTexture2D(&desc, NULL, &pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	pD3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &pShaderResourceView);

	context->CopyResource(pTexture, rt->GetColorTextureDX());

	CPUTTextureDX11 *pNewTexture = new CPUTTextureDX11();
	pNewTexture->mName = name;
	pNewTexture->SetTextureAndShaderResourceView(pTexture, pShaderResourceView);
	pTexture->Release();
	pShaderResourceView->Release();

	return pNewTexture;
}

void CPUTTextureDX11::CopyFromRenderTarget(CPUTRenderTargetColor *rt)
{
	// This is sketchy yes. Should be making sure the resource is the correct size
	ID3D11DeviceContext *context = CPUT_DX11::GetContext();
	context->CopyResource(mpTexture, rt->GetColorTextureDX());
}

//-----------------------------------------------------------------------------
CPUTResult CPUTTextureDX11::CreateNativeTexture(
    ID3D11Device *pD3dDevice,
    const std::string &fileName,
    ID3D11ShaderResourceView **ppShaderResourceView,
    ID3D11Resource **ppTexture,
    bool ForceLoadAsSRGB)
{
    HRESULT hr;
    uint32_t numWChars = MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[numWChars];
    MultiByteToWideChar( CP_UTF8 , 0 , fileName.c_str() , -1, wstr , numWChars );

    hr = DirectX::CreateDDSTextureFromFileEx(
        pD3dDevice,
        wstr,
        0,//maxsize
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE,
        0,
        0,
        ForceLoadAsSRGB,
        ppTexture,
        ppShaderResourceView);
    

	if (FAILED(hr))
	{
		ID3D11DeviceContext *pContext = CPUT_DX11::GetContext();

		hr = DirectX::CreateWICTextureFromFileEx(
			pD3dDevice,
			pContext,
			wstr,
			0,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			ForceLoadAsSRGB,
			ppTexture,
			ppShaderResourceView
			);
	}
	
	delete[] wstr;

	if( FAILED( hr ) )
	{
        if (hr == ERROR_FILE_NOT_FOUND)
        {
            DEBUGMESSAGEBOX("File load error", "File not found: " + fileName + ".");
        }
        else
        {
            DEBUGMESSAGEBOX("Error Texture Creation", "Could not create texture: " + fileName + ".");
        }
        //ASSERT(false, "Failed to load texture: " + fileName)
		return CPUT_TEXTURE_LOAD_ERROR;
	}

    CPUTSetDebugName( *ppTexture, fileName );
    CPUTSetDebugName( *ppShaderResourceView, fileName );

    return CPUT_SUCCESS;
}
// This function returns the DXGI string equivalent of the DXGI format for
// error reporting/display purposes
//-----------------------------------------------------------------------------
const std::string &CPUTTextureDX11::GetDXGIFormatString(DXGI_FORMAT format)
{
    ASSERT( (format>=0) && (format<=DXGI_FORMAT_BC7_UNORM_SRGB), "Invalid DXGI Format." );
    return gpDXGIFormatNames[format];
}

//-----------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE CPUTTextureDX11::MapTexture( CPUTRenderParameters &params, eCPUTMapType type, bool wait )
{
    // Mapping for DISCARD requires dynamic buffer.  Create dynamic copy?
    // Could easily provide input flag.  But, where would we specify? Don't like specifying in the .set file
    // Because mapping is something the application wants to do - it isn't inherent in the data.
    // Could do Clone() and pass dynamic flag to that.
    // But, then we have two.  Could always delete the other.
    // Could support programatic flag - apply to all loaded models in the .set
    // Could support programatic flag on model.  Load model first, then load set.
    // For now, simply support CopyResource mechanism.
    HRESULT hr;
    ID3D11Device *pD3dDevice = CPUT_DX11::GetDevice();
    ID3D11DeviceContext *pContext = CPUT_DX11::GetContext();

    if( !mpTextureStaging )
    {
        // We need to create the texture differently, based on dimension.
        D3D11_RESOURCE_DIMENSION dimension;
        mpTexture->GetType(&dimension);
        switch( dimension )
        {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            {
                D3D11_TEXTURE1D_DESC desc;
                ((ID3D11Texture1D*)mpTexture)->GetDesc( &desc );
                desc.Usage = D3D11_USAGE_STAGING;
                switch( type )
                {
                case CPUT_MAP_READ:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                    desc.BindFlags = 0;
                    break;
                case CPUT_MAP_READ_WRITE:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
                    desc.BindFlags = 0;
                    break;
                case CPUT_MAP_WRITE:
                case CPUT_MAP_WRITE_DISCARD:
                case CPUT_MAP_NO_OVERWRITE:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                    desc.BindFlags = 0;
                    break;
                };
                hr = pD3dDevice->CreateTexture1D( &desc, NULL, (ID3D11Texture1D**)&mpTextureStaging );
                ASSERT( SUCCEEDED(hr), "Failed to create staging texture" );
                break;
            }
        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            {
                D3D11_TEXTURE2D_DESC desc;
                ((ID3D11Texture2D*)mpTexture)->GetDesc( &desc );
                desc.Usage = D3D11_USAGE_STAGING;
                switch( type )
                {
                case CPUT_MAP_READ:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                    desc.BindFlags = 0;
                    break;
                case CPUT_MAP_READ_WRITE:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
                    desc.BindFlags = 0;
                    break;
                case CPUT_MAP_WRITE:
                case CPUT_MAP_WRITE_DISCARD:
                case CPUT_MAP_NO_OVERWRITE:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                    desc.BindFlags = 0;
                    break;
                };
				desc.MiscFlags = (desc.MiscFlags & ~D3D11_RESOURCE_MISC_GENERATE_MIPS);
                hr = pD3dDevice->CreateTexture2D( &desc, NULL, (ID3D11Texture2D**)&mpTextureStaging );
                ASSERT( SUCCEEDED(hr), "Failed to create staging texture" );
                break;
            }
        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            {
                D3D11_TEXTURE3D_DESC desc;
                ((ID3D11Texture3D*)mpTexture)->GetDesc( &desc );
                desc.Usage = D3D11_USAGE_STAGING;
                switch( type )
                {
                case CPUT_MAP_READ:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                    desc.BindFlags = 0;
                    break;
                case CPUT_MAP_READ_WRITE:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
                    desc.BindFlags = 0;
                    break;
                case CPUT_MAP_WRITE:
                case CPUT_MAP_WRITE_DISCARD:
                case CPUT_MAP_NO_OVERWRITE:
                    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                    desc.BindFlags = 0;
                    break;
                };
                hr = pD3dDevice->CreateTexture3D( &desc, NULL, (ID3D11Texture3D**)&mpTextureStaging );
                ASSERT( SUCCEEDED(hr), "Failed to create staging texture" );
                break;
            }
        default:
            ASSERT(0, "Unkown texture dimension" );
            break;
        }
    }
    else
    {
        ASSERT( mMappedType == type, "Mapping with a different CPU access than creation parameter." );
    }
    D3D11_MAPPED_SUBRESOURCE info;
    switch( type )
    {
    case CPUT_MAP_READ:
    case CPUT_MAP_READ_WRITE:
        // TODO: Copying and immediately mapping probably introduces a stall.
        // Expose the copy externally?
        // TODO: copy only if changed?
        // Copy only first time?
        // Copy the GPU version before we read from it.
        pContext->CopyResource( mpTextureStaging, mpTexture );
        break;
    };
    hr = pContext->Map( mpTextureStaging, wait ? 0 : D3D11_MAP_FLAG_DO_NOT_WAIT, (D3D11_MAP)type, 0, &info );
    mMappedType = type;
    return info;
} // CPUTTextureDX11::Map()

//-----------------------------------------------------------------------------
void CPUTTextureDX11::UnmapTexture( CPUTRenderParameters &params )
{
    ASSERT( mMappedType != CPUT_MAP_UNDEFINED, "Can't unmap a render target that isn't mapped." );

    ID3D11DeviceContext *pContext = CPUT_DX11::GetContext();

    pContext->Unmap( mpTextureStaging, 0 );

    // If we were mapped for write, then copy staging buffer to GPU
    switch( mMappedType )
    {
    case CPUT_MAP_READ:
        break;
    case CPUT_MAP_READ_WRITE:
    case CPUT_MAP_WRITE:
    case CPUT_MAP_WRITE_DISCARD:
    case CPUT_MAP_NO_OVERWRITE:
        pContext->CopyResource( mpTexture, mpTextureStaging );
        break;
    };
} // CPUTTextureDX11::Unmap()

CPUTSoftwareTexture *CPUTTextureDX11::GetSoftwareTexture(bool forceUpdate, bool updateIfDirty)
{
	if (mpSoftwareTexture == NULL || forceUpdate || (updateIfDirty && mSoftwareTextureDirty))
	{
		ID3D11Texture2D *tex2d;
		HRESULT hr = mpTexture->QueryInterface(IID_ID3D11Texture2D, (void **)&tex2d);
		if (FAILED(hr)) {
			return NULL;
		}

		if (mpSoftwareTexture == NULL)
		{
			mpSoftwareTexture = new CPUTSoftwareTexture();
		}

		CPUTRenderParameters params;
		CD3D11_TEXTURE2D_DESC textureDesc;
		tex2d->GetDesc(&textureDesc);
		D3D11_MAPPED_SUBRESOURCE mapped = MapTexture(params, CPUT_MAP_READ, true);
		mpSoftwareTexture->SetData(mapped.pData, textureDesc.Width, textureDesc.Height, mapped.RowPitch, textureDesc.Format);
		UnmapTexture(params);
		mSoftwareTextureDirty = false;
		
		tex2d->Release();
	}
	return mpSoftwareTexture;
}