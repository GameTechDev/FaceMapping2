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
#ifndef __GAME_FACE_SCAN__
#define __GAME_FACE_SCAN__

#ifndef DISABLE_RSSDK

enum FaceStatusDetected
{
	FaceStatusDetected_Detected,
	FaceStatusDetected_NotDetected
};

enum FaceStatusXAxis
{
	FaceStatusXAxis_InRange,
	FaceStatusXAxis_TooFarLeft,
	FaceStatusXAxis_TooFarRight
};

enum FaceStatusYAxis
{
	FaceStatusYAxis_InRange,
	FaceStatusYAxis_TooFarUp,
	FaceStatusYAxis_TooFarDown
};

enum FaceStatusZAxis
{
	FaceStatusZAxis_InRange,
	FaceStatusZAxis_TooClose,
	FaceStatusZAxis_TooFar
};

enum FaceStatusYaw
{
	FaceStatusYaw_InRange,
	FaceStatusYaw_TooFarLeft,
	FaceStatusYaw_TooFarRight,
};

enum FaceStatusPitch
{
	FaceStatusPitch_InRange,
	FaceStatusPitch_TooFarUp,
	FaceStatusPitch_TooFarDown,
};


#include "pxcimage.h"
#include "pxc3dscan.h"

bool FaceScan_Init();
void FaceScan_Shutdown();

const char *FaceScan_GetLastErrorMsg();

bool FaceScan_FrameAcquire();
void FaceScan_FrameRelease();

PXCImage *FaceScan_GetColorImage();
PXCImage *FaceScan_GetScanPreviewImage();

void FaceScan_StartScan();
void FaceScan_CancelScan();
void FaceScan_SaveScan(const char *filename);

bool FaceScan_IsScanning();


FaceStatusDetected FaceScan_StatusDetected();
FaceStatusXAxis FaceScan_StatusXAxis();
FaceStatusYAxis FaceScan_StatusYAxis();
FaceStatusZAxis FaceScan_StatusZAxis();
FaceStatusYaw FaceScan_StatusYaw();
FaceStatusPitch FaceScan_StatusPitch();

PXC3DScan::AlertData FaceScan_GetAlertData();

#endif

void FaceScan_MoveScanData(const char *srcObjFilename, const char *dstObjFilename);

#endif // #ifndef __GAME_FACE_SCAN__