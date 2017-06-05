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
#ifndef __CPUTMESHOGL_H__
#define __CPUTMESHOGL_H__

#pragma once

#include "CPUTMesh.h"
#include "CPUT.h"
#include "CPUTOSServices.h"

class CPUTVertexArrayOGL;
class CPUTBuffer;

//-----------------------------------------------------------------------------
class CPUTMeshOGL : public CPUTMesh
{
protected:

    CPUTMeshOGL();

    CPUTVertexArrayOGL * mpVertexArray;
    
    CPUTBuffer    *mpVertexBuffer;
    UINT           mVertexCount;
    UINT           mVertexStride;

    CPUTBuffer    *mpIndexBuffer;
    UINT           mIndexCount;
    
    // This is a handle to the shader program
    GLuint shaderprogram;

    GLuint vao,vbo[2]; /* Create handles for our Vertex Array Object and two Vertex Buffer Objects */
    eCPUTMapType              mIndexBufferMappedType;

public:
	static CPUTMeshOGL* Create();
    virtual ~CPUTMeshOGL();
    
    CPUTResult CreateNativeResources(
                                  CPUTModel             *pModel,
                                  UINT                   meshIdx,
                                  int                    vertexElementCount,
                                  CPUTBufferElementInfo *pVertexInfo,
                                  UINT                   vertexCount,
                                  void                  *pVertexData,
                                  CPUTBufferElementInfo *pIndexInfo,
                                  UINT                   indexCount,
                                  void                  *pIndex
                              );
    void Draw();
    void DrawPatches();

    void SetNumVertices(int numVertices) {this->mVertexCount = numVertices;};
    void SetNumIndices(int numIndices) {this->mIndexCount = numIndices;};

	// FIXME force get of buffer and map there
    void SetIndexSubData( UINT offset, UINT size, void* pData);
    void SetVertexSubData( UINT offset, UINT size, void* pData);
	void* MapVertices(   CPUTRenderParameters &params, eCPUTMapType type, bool wait=true ) {ASSERT(0, "unsupported"); return NULL;};
    void* MapIndices(    CPUTRenderParameters &params, eCPUTMapType type, bool wait=true ){ASSERT(0, "unsupported");return NULL;};
    void  UnmapVertices( CPUTRenderParameters &params ){ASSERT(0, "unsupported");};
    void  UnmapIndices(  CPUTRenderParameters &params ){ASSERT(0, "unsupported");};
};

#endif // __CPUTMESHOGL_H__
