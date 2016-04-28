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
#ifndef _CPUTBUFFERDX11_H
#define _CPUTBUFFERDX11_H

#include "CPUTBuffer.h"
#include "CPUT_DX11.h"

//--------------------------------------------------------------------------------------
class CPUTBufferDX11 : public CPUTBuffer
{
private:
    // resource view pointer
    ID3D11ShaderResourceView  *mpShaderResourceView;
    ID3D11UnorderedAccessView *mpUnorderedAccessView;
    ID3D11Buffer              *mpBuffer;

    // Destructor is not public.  Must release instead of delete.
    ~CPUTBufferDX11() {
        SAFE_RELEASE(mpShaderResourceView);
        SAFE_RELEASE(mpUnorderedAccessView);
        SAFE_RELEASE(mpBuffer);
    }

public:
	void ReleaseShaderResourceView()
	{
		SAFE_RELEASE(mpShaderResourceView);
	}
	void ReleaseUnorderedAccessView()
	{
		SAFE_RELEASE(mpUnorderedAccessView);
	}
	void ReleaseNativeBuffer()
	{
		SAFE_RELEASE(mpBuffer);
	}
    ID3D11ShaderResourceView *GetShaderResourceView()
    {
        return mpShaderResourceView;
    }

    ID3D11UnorderedAccessView *GetUnorderedAccessView()
    {
        return mpUnorderedAccessView;
    }

    void SetShaderResourceView(ID3D11ShaderResourceView *pShaderResourceView)
    {
        SAFE_RELEASE(mpShaderResourceView);
        mpShaderResourceView = pShaderResourceView;
        mpShaderResourceView->AddRef();
    }
    void SetUnorderedAccessView(ID3D11UnorderedAccessView *pUnorderedAccessView)
    {
        SAFE_RELEASE(mpUnorderedAccessView);
        mpUnorderedAccessView = pUnorderedAccessView;
		if ( mpUnorderedAccessView != NULL )
			mpUnorderedAccessView->AddRef();
    }
    void SetBuffer(ID3D11Buffer *pBuffer)
    {
        SAFE_RELEASE(mpBuffer);
        mpBuffer = pBuffer;
        if (mpBuffer) 
            mpBuffer->AddRef();
    }

    ID3D11Buffer *GetNativeBuffer() { return mpBuffer; };

    void SetData(UINT offset, UINT sizeBytes, void* pData);
    void GetData(void* pData);

    static CPUTBuffer* Create(std::string &name, CPUTBufferDesc* pDesc);

private:
    CPUTBufferDX11(std::string &name, CPUTBufferDesc* pDesc);
};
#endif //_CPUTBUFFERDX11_H

