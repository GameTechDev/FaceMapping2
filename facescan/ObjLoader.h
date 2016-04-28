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
#ifndef OBJLOADER_INCLUDED_
#define OBJLOADER_INCLUDED_

#include <vector>
#include <string>
#include <stdint.h>

// structure for reading the obj model data
struct tVertex
{
    float   x,y,z;
    float   u,v;
	float   nx, ny, nz;
};

typedef int32_t ObjIndexInt;

struct tSegment
{
    std::string m_name;
    int m_start;
    int m_end;
};

struct tObjModel
{
    std::vector<tSegment>   m_segments;
    std::vector<tVertex>    m_vertices;
	std::vector<ObjIndexInt>     m_indices;
};

void objLoader(const char *filename, tObjModel &model);

#endif