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