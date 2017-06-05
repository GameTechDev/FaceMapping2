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
#ifndef __SAMPLE_UTIL__
#define __SAMPLE_UTIL__

#include <D3D11.h>
#include <string>
#include "cput.h"
#include "CPUT.h"

struct tObjModel;
class CPUTSoftwareMesh;

#ifndef DISABLE_RSSDK
#include "pxcimage.h"
#endif // #ifndef DISABLE_RSSDK

struct SCodeTextureWrap
{
	ID3D11Texture2D *Texture;
	ID3D11Resource *Resource;
	ID3D11ShaderResourceView *SRV;
	D3D11_TEXTURE2D_DESC Desc;
};

bool TextureWrap_Init(SCodeTextureWrap* wrap, D3D11_TEXTURE2D_DESC *desc);
bool TextureWrap_AdjustSize(SCodeTextureWrap* wrap, int width, int height);
void TextureWrap_Release(SCodeTextureWrap* wrap);

#ifndef DISABLE_RSSDK
void CopyPXCImageToTexture(PXCImage *srcImage, SCodeTextureWrap *dstTexture);
#endif // #ifndef DISABLE_RSSDK

const std::string &GetUserDataDirectory();
const std::string &GetMyAssetsDirectory();

void DebugPrintf(char *fmt, ...);

void DebugPrintBeginFileCapture( const char *filename);
void DebugPrintEndFileCapture();

class ProfileBlockScoped
{
public:
	ProfileBlockScoped( std::string blockName );
	~ProfileBlockScoped();
private:
	uint64 mStartTime;
	std::string mBlockName;
};

void SampleUtil_Init();
void SampleUtil_Shutdown();


void SetCodeTexture(int index, ID3D11ShaderResourceView *srv);
void SetCodeTexture(int index, SCodeTextureWrap *texture);
void SetCodeTexture(int index, CPUTTexture *texture);

class CPUTMaterial;
CPUTMaterial *GetCodeSpriteMaterial();
CPUTMaterial *GetMeshPreviewMaterial();

class CPUTModel;
CPUTModel *LoadObjAsCPUTModel(const char *objFilename);


void DrawCube(CPUTRenderParameters &renderParams, float3 position, float size, CPUTColor4 color);
void DrawBox(CPUTRenderParameters &renderParams, float3 position, float3 size, CPUTColor4 color);

void DrawQuadSC(CPUTRenderParameters &renderParams, float2 position, float size, CPUTColor4 color);

class ViewportScoped
{
public:
	ViewportScoped(ID3D11DeviceContext *context, D3D11_VIEWPORT *viewports, int count, CPUTRenderParameters *params = NULL);
	~ViewportScoped();

private:
	D3D11_VIEWPORT mStoredViewports[8];
	UINT mStoredViewportCount;
	ID3D11DeviceContext *mContext;

	CPUTRenderParameters *mParams;
	int mStoredParamWidth;
	int mStoredParamHeight;
};

#define COMBINE1(X,Y) X##Y  // helper macro
#define PUSH_VIEWPORT( context, viewports, count, params ) ViewportScoped COMBINE1(viewportBlock,__LINE__)(context, viewports, count, params);


void CopyOBJDataToSoftwareMesh(tObjModel *objModel, CPUTSoftwareMesh *softwareMesh);

#endif