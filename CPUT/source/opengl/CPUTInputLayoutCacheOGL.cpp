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
#include "CPUTInputLayoutCacheOGL.h"

CPUTInputLayoutCacheOGL* CPUTInputLayoutCacheOGL::mpInputLayoutCache = NULL;

CPUTInputLayoutCache* CPUTInputLayoutCache::GetInputLayoutCache()
{
    return CPUTInputLayoutCacheOGL::GetInputLayoutCache();
}
void CPUTInputLayoutCache::DeleteInputLayoutCache()
{
    CPUTInputLayoutCacheOGL::DeleteInputLayoutCache();
}

//-----------------------------------------------------------------------------
void CPUTInputLayoutCacheOGL::ClearLayoutCache()
{
}

CPUTInputLayoutCacheOGL* CPUTInputLayoutCacheOGL::GetInputLayoutCache()
{
    if (NULL == mpInputLayoutCache)
    {
        mpInputLayoutCache = new CPUTInputLayoutCacheOGL();
    }
    return mpInputLayoutCache;
}

// singleton destroy routine
//-----------------------------------------------------------------------------
CPUTResult CPUTInputLayoutCacheOGL::DeleteInputLayoutCache()
{
    SAFE_DELETE(mpInputLayoutCache);
    return CPUT_SUCCESS;
}

void GetInputLayoutCache()
{

}
void CPUTInputLayoutCacheOGL::Apply(CPUTMesh* pMesh, CPUTMaterial* pMaterial)
{
 /*   D3D11_INPUT_ELEMENT_DESC *pDXLayout = ((CPUTMeshDX11*)pMesh)->GetLayoutDescription();
    CPUTVertexShaderDX11 *pVertexShader = ((CPUTMaterialDX11*)pMaterial)->GetVertexShader();
    ID3D11InputLayout *pInputLayout;

    if (CPUT_SUCCESS == GetLayout(pDevice, pDXLayout, pVertexShader, &pInputLayout))
    {
        CPUT_DX11::GetContext()->IASetInputLayout(pInputLayout);
    }*/
}
//-----------------------------------------------------------------------------
