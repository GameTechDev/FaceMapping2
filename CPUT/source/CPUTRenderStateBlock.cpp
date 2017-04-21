/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CPUTRenderStateBlock.h"
#ifdef CPUT_FOR_DX11
#include "CPUTRenderStateBlockDX11.h"
#elif (defined(CPUT_FOR_OGL) || defined (CPUT_FOR_OGLES))
    #include "CPUTRenderStateBlockOGL.h"
#else
    #error You must supply a target graphics API (ex: #define CPUT_FOR_DX11), or implement the target API for this file.
#endif


CPUTRenderStateBlock *CPUTRenderStateBlock::mpDefaultRenderStateBlock = NULL;

CPUTRenderStateBlock *CPUTRenderStateBlock::Create( const std::string &name, const std::string &absolutePathAndFilename )
{
#ifdef CPUT_FOR_DX11
    CPUTRenderStateBlock *pRenderStateBlock = CPUTRenderStateBlockDX11::Create();
#elif (defined(CPUT_FOR_OGL) || defined (CPUT_FOR_OGLES))
    CPUTRenderStateBlock *pRenderStateBlock = CPUTRenderStateBlockOGL::Create();
#else
    #error You must supply a target graphics API (ex: #define CPUT_FOR_DX11), or implement the target API for this file.
#endif
    pRenderStateBlock->LoadRenderStateBlock( absolutePathAndFilename );

    return pRenderStateBlock;
}
