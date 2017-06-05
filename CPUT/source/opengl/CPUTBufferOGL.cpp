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

#include "CPUTBufferOGL.h"

CPUTBuffer* CPUTBuffer::Create(std::string &name, CPUTBufferDesc* pDesc)
{
    return CPUTBufferOGL::Create(name, pDesc);
}

CPUTBufferOGL::~CPUTBufferOGL()
{
    glDeleteBuffers(1, &mBufferID);
}
void CPUTBufferOGL::SetData(UINT offset, UINT size, void* pData)
{
    if (offset + size <= mSizeBytes)
    {
        GL_CHECK(glBindBuffer(mGLTarget, mBufferID));
        GL_CHECK(glBufferSubData(mGLTarget, offset, size, pData));
        GL_CHECK(glBindBuffer(mGLTarget, 0));
    }
    else
    {
        DEBUG_PRINT("Error attempting to write past end of buffer: %s", mName.c_str());
    }
}

void CPUTBufferOGL::GetData(void* pData)
{
    GL_CHECK(glBindBuffer(mGLTarget, mBufferID));
    void* pMappedData = glMapBufferRange(mTarget, 0, mSizeBytes, GL_MAP_READ_BIT);
    memcpy(pData, pMappedData, mSizeBytes);
    glUnmapBuffer(mGLTarget);
    GL_CHECK(glBindBuffer(mGLTarget, 0));
}

CPUTBuffer* CPUTBufferOGL::Create(std::string &name, CPUTBufferDesc *pDesc)
{
    return new CPUTBufferOGL(name, pDesc);
}

CPUTBufferOGL::CPUTBufferOGL(std::string &name, CPUTBufferDesc *pDesc)
    : CPUTBuffer(name, pDesc)
{
    glGenBuffers(1, &mBufferID);
    mGLTarget = pDesc->target == BUFFER_VERTEX ? GL_ARRAY_BUFFER
            : pDesc->target == BUFFER_INDEX ? GL_ELEMENT_ARRAY_BUFFER
            : pDesc->target == BUFFER_UNIFORM ? GL_UNIFORM_BUFFER
            : NULL;
    if (pDesc != NULL)
    {
        switch (pDesc->memory)
        {
        case BUFFER_IMMUTABLE:
            mGLUsage = GL_STATIC_DRAW;
            break;
        case BUFFER_DYNAMIC:
            mGLUsage = GL_DYNAMIC_DRAW;
            break;
        case BUFFER_STREAMING:
            mGLUsage = GL_STREAM_DRAW;
            break;
        default:
            GL_STATIC_DRAW;
        }
        if (pDesc->memory == BUFFER_STAGING || pDesc->cpuAccess == BUFFER_CPU_WRITE)
            mGLUsage += 2; //copy
        else if (pDesc->cpuAccess = BUFFER_CPU_READ)
            mGLUsage += 1;//read

        GL_CHECK(glBindBuffer(mGLTarget, mBufferID));
        GL_CHECK(glBufferData(mGLTarget, mSizeBytes, pDesc->pData, mGLUsage));
        GL_CHECK(glBindBuffer(mGLTarget, 0));

    }
}