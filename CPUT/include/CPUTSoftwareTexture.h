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
#ifndef _CPUTSOFTWARETEXTURE_H
#define _CPUTSOFTWARETEXTURE_H

#include <d3d11.h>
#include "CPUT.h"

class CPUTSoftwareTexture
{
public:
	CPUTSoftwareTexture();
	~CPUTSoftwareTexture();

	void SetData(void *data, int width, int height, int pitch, DXGI_FORMAT textureFormat);

	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }
	void *GetData() { return mData; }

	void SampleRGBAFromUV(float u, float v, CPUTColor4 *outColor);
	float SampleR32FromUV(float u, float v);

private:

	void *mData;
	DXGI_FORMAT mFormat;

	int mWidth;
	int mHeight;

};

#endif //_CPUTSOFTWARETEXTURE_H
