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
#include "ObjLoader.h"

#include <stdio.h>
#include <windows.h>

#include <iostream>
#include <fstream>
using namespace std;


#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))

enum
{
    UNKNOWN,
    VERTEX,
    NORMAL,
    TEXCOORD,
    FACE,
    GROUP,
    USEMATERIAL,
    MATERIALLIB,
    COMMENT
};

struct tPair
{
    int value;
    const char *string;
};

static const tPair objTokens[] = 
{
    {   UNKNOWN,    ""              },
    {   VERTEX,     "v"             },
    {   NORMAL,     "vn"            },
    {   TEXCOORD,   "vt"            },
    {   FACE,       "f"             },
    {   GROUP,      "g"             },
    {   USEMATERIAL,"usemtl"        },
    {   MATERIALLIB,"mtllib"        },
    {   COMMENT,    "#"             },

};

int getToken(const char *value)
{
    int token = UNKNOWN;

    for ( int i=0; i<ARRAY_SIZE(objTokens); ++i)
    {
        if (!strcmp(objTokens[i].string,value))
        {
            token = objTokens[i].value;
            break;
        }
    }

    return token;
}

void skipLine(FILE *fp)
{
    // read characters till the end of the line
    while ((!feof(fp)) && (fgetc(fp)!='\n'))
    {
    }
}

struct tVector
{
    float m[3];
};

struct tVectorI
{
    int m[3];
};

float min(float a, float b)
{
    return (a<b) ? a:b;
}

tVector min( const tVector &a, const tVector &b)
{
    tVector ret;

    ret.m[0] = min(a.m[0], b.m[0]);
    ret.m[1] = min(a.m[1], b.m[1]);
    ret.m[2] = min(a.m[2], b.m[2]);

    return ret;
}

float max(float a, float b)
{
    return (a>b) ? a:b;
}

tVector max( const tVector &a, const tVector &b)
{
    tVector ret;

    ret.m[0] = max(a.m[0], b.m[0]);
    ret.m[1] = max(a.m[1], b.m[1]);
    ret.m[2] = max(a.m[2], b.m[2]);

    return ret;
}

void pushVector(const char **ptr, std::vector<tVector> &vecs)
{
    tVector tmp;

    sscanf( *ptr,"%f %f %f", &tmp.m[0], &tmp.m[1], &tmp.m[2]);
	// for consistency it'd be nice if this incremented ptr, but nothing after pushing vector is used
    vecs.push_back(tmp);
}

bool operator == ( const tVectorI &v0, const tVectorI &v1)
{
    return ((v0.m[0] == v1.m[0]) && (v0.m[1] == v1.m[1]) && (v0.m[2] == v1.m[2]));
}

void skipWhiteSpace(const char **buf)
{
	while (**buf != 0 && **buf == ' ')
		(*buf)++;
}

void getDelimitedToken(const char **buf, char delimiter, char *outBuf, int size)
{
	skipWhiteSpace(buf);
	const char *ptr = *buf;
	int index = 0;
	while (index < size)
	{
		if (ptr[index] == delimiter || ptr[index] == 0)
		{
			memcpy(outBuf, ptr, index);
			outBuf[index] = 0;
			int inc = index + ((ptr[index] == delimiter) ? 1 : 0);
			*buf += inc; // increment pointer and move past delimieter
			return;
		}
		index++;
	}
	outBuf[0] = 0;
}


bool getVectorI(const char **ptr, int *outX, int *outY, int *outZ)
{
	char buf[256];
	getDelimitedToken(ptr, ' ', buf, sizeof(buf));
	int result = sscanf(buf, "%d/%d/%d", outX, outY, outZ);
	return result == 3;
}

void pushFaces(const char **ptr, std::vector<tVectorI> &vecs, std::vector<ObjIndexInt> &uniqueIndices)
{
	const int maxIdx = 16;
	tVectorI tmp[maxIdx];
	int idxCount = 0;
	for (int i = 0; i < maxIdx; i++)
	{
		if (!getVectorI(ptr, &tmp[i].m[0], &tmp[i].m[1], &tmp[i].m[2]))
		{
			break;
		}
		// obj files are indexed from 1
		tmp[i].m[0] -= 1;
		tmp[i].m[1] -= 1;
		tmp[i].m[2] -= 1;
		idxCount++;
	}

	if (idxCount < 3)
		return;

	for (int i = 2; i < idxCount; i++)
	{
		// root of fan
		uniqueIndices.push_back((ObjIndexInt)vecs.size());
		vecs.push_back(tmp[0]);

		for (int j = 0; j < 2; j++)
		{
			uniqueIndices.push_back((ObjIndexInt)vecs.size());
			vecs.push_back(tmp[i - 1 + j ]);
		}
	}
}

// create a unique index set for the data
bool pushVectorI(FILE *fp, std::vector<tVectorI> &vecs, std::vector<ObjIndexInt> &uniqueIndices)
{
    tVectorI tmp;

    int result = fscanf(fp,"%d/%d/%d", &tmp.m[0], &tmp.m[1], &tmp.m[2]);
	if (result != 3)
		return false;

    // obj files are indexed from 1
    tmp.m[0]-=1;
    tmp.m[1]-=1;
    tmp.m[2]-=1;

	uniqueIndices.push_back((ObjIndexInt)vecs.size());
    vecs.push_back(tmp);
	return true;

}

void objLoader(const char *filename, tObjModel &model)
{
	ifstream fs;
	fs.open(filename);
	/*if (fp == NULL)
	{
		Sleep(1000);
		int errnoresult = errno;
		printf( "  err %d \n", errnoresult );
		fp = fopen(filename, "r"); 
	}*/

    // structures for loading the data
    std::vector<tVector>    tempVertices;
    std::vector<tVector>    tempNormals;
    std::vector<tVector>    tempUVs;

    std::vector<tVectorI>   tempFaces;
    
    model.m_segments.clear();
    model.m_indices.clear();
    model.m_vertices.clear();

	std::string line;
    if ( fs.is_open() )
    {
		char line[1024];
		char token[1024];
		while (fs.getline(line, sizeof(line)))
        {
			const char *linePtr = line;
			
			getDelimitedToken(&linePtr, ' ', token, sizeof(token));

			int tokenInt = getToken(token);

			switch (tokenInt)
            {
                case   UNKNOWN:     break;
				case   VERTEX:      pushVector(&linePtr, tempVertices);    break;
				case   NORMAL:      pushVector(&linePtr, tempNormals);     break;
				case   TEXCOORD:    pushVector(&linePtr, tempUVs);         break;
                case   FACE:
					pushFaces(&linePtr, tempFaces, model.m_indices);
                    break;    
                case   GROUP:
                    {
                        char name[256];
                        sscanf(linePtr, "%s",name);

                        if ( model.m_segments.size() > 0 )
                        {
							model.m_segments.back().m_end = (int)model.m_indices.size();
                        }


                        tSegment newSegment;

                        newSegment.m_name.append(name);
						newSegment.m_start = (int)model.m_indices.size();
                        newSegment.m_end   = 0;

                        model.m_segments.push_back(newSegment);
    
                    }
                // all these cases fall through and we skip the line
                case   USEMATERIAL:
                case   MATERIALLIB:
                case   COMMENT:    
                default:    break;
            };

            if ( model.m_segments.size() > 0 )
            {
                model.m_segments.back().m_end = (int)model.m_indices.size();
            }
        }

        // find a bounding box?
        if ( tempVertices.size() > 0 )
        {
            tVector vmin = tempVertices[0];
            tVector vmax = tempVertices[0];

            for ( std::vector<tVector>::const_iterator it = tempVertices.begin(); it != tempVertices.end(); ++it )
            {
                vmin = min(vmin,*it);
                vmax = max(vmax,*it);
            }

            tVector bsize;

            bsize.m[0] = vmax.m[0] - vmin.m[0];
            bsize.m[1] = vmax.m[1] - vmin.m[1];
            bsize.m[2] = vmax.m[2] - vmin.m[2];
        }

        // to complete the model
        // generate the interleaved vertex data from the temporary face indices
        for ( std::vector<tVectorI>::iterator it = tempFaces.begin(); it != tempFaces.end(); ++it )
        {
            tVertex tmp;

            tVector &v  = tempVertices[it->m[0]];
            tVector &vt = tempUVs[it->m[1]];
            tVector &vn = tempNormals[it->m[2]];

            
            tmp.x = v.m[0];
            tmp.y = v.m[1];
            tmp.z = v.m[2];

            // normals are inverted on the model!
            tmp.nx = vn.m[0];
            tmp.ny = vn.m[1];
            tmp.nz = vn.m[2];

            tmp.u = vt.m[0];
            tmp.v = vt.m[1];

            model.m_vertices.push_back(tmp);
        }
		fs.close();
    }
}