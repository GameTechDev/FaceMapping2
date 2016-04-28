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