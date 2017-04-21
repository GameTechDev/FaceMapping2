/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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