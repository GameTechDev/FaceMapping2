#include "CPUTSoftwareTexture.h"

CPUTSoftwareTexture::CPUTSoftwareTexture() :
mData(NULL),
mWidth(0),
mHeight(0)
{
}

CPUTSoftwareTexture::~CPUTSoftwareTexture()
{
	if (mData)
	{ 
		free(mData);
		mData = NULL;
	}
}

int GetBPP(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
		return 4;
			
	default:

		assert(false); // Add your texture format
	}
	return 0;
}



void CPUTSoftwareTexture::SampleRGBAFromUV(float u, float v, CPUTColor4 *outColor)
{
	uint32 x = (uint32)(u * (float)mWidth) % (uint32)mWidth;
	uint32 y = (uint32)(v * (float)mHeight) % (uint32)mHeight;
	int bpp = GetBPP(mFormat);
	byte* byteData = &((byte*)mData)[bpp * (x + y * mWidth)];
	if (mFormat == DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		outColor->r = (float)byteData[0] / 255.0f;
		outColor->g = (float)byteData[1] / 255.0f;
		outColor->b = (float)byteData[2] / 255.0f;
		outColor->a = (float)byteData[3] / 255.0f;
	}
	else if (mFormat == DXGI_FORMAT_R32_FLOAT)
	{
		outColor->r = *((float*)byteData);
	}
}

void CPUTSoftwareTexture::SetData(void *data, int width, int height, int pitch, DXGI_FORMAT textureFormat)
{
	int bpp = GetBPP(textureFormat);

	int size = bpp * width * height;

	if (width != mWidth || height != mHeight || textureFormat != mFormat || mData == NULL)
	{
		if (mData)
			free(mData);
		
		mData = malloc(bpp * width * height);
		mWidth = width;
		mHeight = height;
		mFormat = textureFormat;
	}
	
	// if this is not the case, copy row by row
	assert((pitch / bpp) == width);
	memcpy(mData, data, size);
}

