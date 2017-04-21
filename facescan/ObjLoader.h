/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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