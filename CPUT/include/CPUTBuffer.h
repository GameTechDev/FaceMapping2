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
#ifndef _CPUTBUFFER_H
#define _CPUTBUFFER_H

#include "CPUT.h"
#include "CPUTRefCount.h"

enum BufferTarget
{
    BUFFER_VERTEX  = 0x1,
    BUFFER_INDEX   = 0x2,
    BUFFER_UNIFORM = 0x4,
};

enum BufferMemory
{
    BUFFER_IMMUTABLE,
    BUFFER_DYNAMIC,
    BUFFER_STREAMING,
    BUFFER_STAGING,
};

enum BufferCPUAccess
{
    BUFFER_CPU_NO_ACCESS = 0x0,
    BUFFER_CPU_READ      = 0x1,
    BUFFER_CPU_WRITE     = 0x2,
};

struct CPUTBufferDesc
{
    UINT sizeBytes;
    void* pData;
    enum BufferTarget target;
    enum BufferMemory memory;
    enum BufferCPUAccess cpuAccess;
};

class CPUTBuffer : public CPUTRefCount
{
public:
    const std::string &GetName() { return mName; }
    virtual void SetData(UINT offset, UINT sizeBytes, void* pData) = 0;
    virtual void GetData(void* pData)=0;
    static CPUTBuffer* Create(std::string &name, CPUTBufferDesc *pDesc);

    UINT GetSize();

protected:
    std::string mName;
    BufferTarget mTarget;
    BufferMemory mMemory;
    BufferCPUAccess mCPUAccess;
    UINT mSizeBytes;

    CPUTBuffer(std::string &name, CPUTBufferDesc* pDesc) 
        :mName(name), mSizeBytes(0)
    {
        if (pDesc)
        {
            mTarget = pDesc->target;
            mMemory = pDesc->memory;
            mCPUAccess = pDesc->cpuAccess;
            mSizeBytes = pDesc->sizeBytes;
        }
    };
    ~CPUTBuffer(){}; // Destructor is not public.  Must release instead of delete.
};

#endif //_CPUTBUFFER_H
