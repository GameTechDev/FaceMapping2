/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUTTEXTURE_H
#define _CPUTTEXTURE_H

#include "CPUT.h"
#include "CPUTRefCount.h"

class CPUTTexture : public CPUTRefCount
{
protected:
	std::string mName;
	eCPUTMapType mMappedType;

	~CPUTTexture() {} // Destructor is not public.  Must release instead of delete.

	CPUTTexture() : mMappedType(CPUT_MAP_UNDEFINED) {}

	CPUTTexture(std::string& name) : mMappedType(CPUT_MAP_UNDEFINED), mName(name) {}

public:
	static CPUTTexture* Create(const std::string& name, const std::string absolutePathAndFilename, bool loadAsSRGB);
	// ##### what is the OpenGL equivalent of mapping a texture? Really all we are returning here is a void* to memory and row pitch and depth.
	// no reason to be DX specific.
#ifdef CPUT_FOR_DX11
	virtual D3D11_MAPPED_SUBRESOURCE MapTexture(CPUTRenderParameters& params, eCPUTMapType type, bool wait = true) = 0;
#elif defined(CPUT_FOR_OGL)
    virtual void *MapTexture(   CPUTRenderParameters &params, eCPUTMapType type, bool wait = true ) = 0;
#endif
	virtual void UnmapTexture(CPUTRenderParameters& params) = 0; // TODO: Store params on Map() and don't require here.

	std::string GetName() const { return mName; }
};

#endif //_CPUTTEXTURE_H

