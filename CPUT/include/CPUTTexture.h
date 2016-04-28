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

