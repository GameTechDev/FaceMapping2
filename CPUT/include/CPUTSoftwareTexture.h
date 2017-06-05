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
