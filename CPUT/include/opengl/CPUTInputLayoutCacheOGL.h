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
#ifndef __CPUTINPUTLAYOUTCACHEROGL_H__
#define __CPUTINPUTLAYOUTCACHEROGL_H__

#include "CPUTInputLayoutCache.h"

//struct D3D11_INPUT_ELEMENT_DESC;

class CPUTInputLayoutCacheOGL :public CPUTInputLayoutCache
{
public:
    ~CPUTInputLayoutCacheOGL()
    {
    }
    static CPUTInputLayoutCacheOGL *GetInputLayoutCache();
    static CPUTResult DeleteInputLayoutCache();
    void ClearLayoutCache();
    void Apply(CPUTMesh* pMesh, CPUTMaterial* pMaterial);
private:
    CPUTInputLayoutCacheOGL() { };
    static CPUTInputLayoutCacheOGL *mpInputLayoutCache;    
};

#endif //#define __CPUTINPUTLAYOUTCACHEROGL_H__