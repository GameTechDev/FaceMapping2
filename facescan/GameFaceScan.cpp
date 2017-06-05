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
#include <wchar.h>
#include "CPUTOSServices.h"
#include "windows.h"
#include "assert.h"
#include "sampleutil.h"
#include "GameFaceScan.h"

#ifndef DISABLE_RSSDK

#include "pxcsensemanager.h"


struct SFaceScanData
{
	PXCSenseManager *SenseManager;
	PXC3DScan *FaceScan;
	
	bool FrameAcquired;

	PXCImage *colorImage;
	PXCImage::ImageData colorImageData;

	PXCImage *AcquiredPreviewImage;

	PXC3DScan::Configuration ScanConfig;

	bool ScanningInProgress;
};

const char *alertMap[] =
{
	"ALERT_IN_RANGE",
	"ALERT_TOO_CLOSE",
	"ALERT_TOO_FAR",
	"ALERT_TRACKING",
	"ALERT_LOST_TRACKING",
	"ALERT_SUFFICIENT_STRUCTURE",
	"ALERT_INSUFFICIENT_STRUCTURE",
	"ALERT_FACE_DETECTED",
	"ALERT_FACE_NOT_DETECTED",
	"ALERT_FACE_X_IN_RANGE",
	"ALERT_FACE_X_TOO_FAR_LEFT",
	"ALERT_FACE_X_TOO_FAR_RIGHT",
	"ALERT_FACE_Y_IN_RANGE",
	"ALERT_FACE_Y_TOO_FAR_UP",
	"ALERT_FACE_Y_TOO_FAR_DOWN",
	"ALERT_FACE_Z_IN_RANGE",
	"ALERT_FACE_Z_TOO_CLOSE",
	"ALERT_FACE_Z_TOO_FAR",
	"ALERT_FACE_YAW_IN_RANGE",
	"ALERT_FACE_YAW_TOO_FAR_LEFT",
	"ALERT_FACE_YAW_TOO_FAR_RIGHT",
	"ALERT_FACE_PITCH_IN_RANGE",
	"ALERT_FACE_PITCH_TOO_FAR_UP",
	"ALERT_FACE_PITCH_TOO_FAR_DOWN"
};

class MyAlertHandler : public PXC3DScan::AlertHandler
{
public:
	MyAlertHandler()
	{
		Reset();
	}
	void Reset()
	{
		ZeroMemory(&Data, sizeof(Data));
		mStatusDetected = (FaceStatusDetected)0;
		mStatusXAxis = (FaceStatusXAxis)0;
		mStatusYAxis = (FaceStatusYAxis)0;
		mStatusZAxis = (FaceStatusZAxis)0;
		mStatusPitch = (FaceStatusPitch)0;
		mStatusYaw = (FaceStatusYaw)0;

	}
	virtual void PXCAPI OnAlert(const PXC3DScan::AlertData& data)
	{
		Data = data;
		int labelIndex = data.label;
		if (labelIndex >= 0 && labelIndex < ARRAYSIZE(alertMap))
		{
			DebugPrintf("Alert %s\n", alertMap[labelIndex]);
		}
		switch (data.label)
		{
			case PXC3DScan::ALERT_FACE_DETECTED: { mStatusDetected = FaceStatusDetected_Detected; } break;
			case PXC3DScan::ALERT_FACE_NOT_DETECTED: { mStatusDetected = FaceStatusDetected_NotDetected; } break;

			case PXC3DScan::ALERT_FACE_X_IN_RANGE: { mStatusXAxis = FaceStatusXAxis_InRange; } break;
			case PXC3DScan::ALERT_FACE_X_TOO_FAR_LEFT: { mStatusXAxis = FaceStatusXAxis_TooFarLeft; } break;
			case PXC3DScan::ALERT_FACE_X_TOO_FAR_RIGHT: { mStatusXAxis = FaceStatusXAxis_TooFarRight; } break;
				
			case PXC3DScan::ALERT_FACE_Y_IN_RANGE: { mStatusYAxis = FaceStatusYAxis_InRange; } break;
			case PXC3DScan::ALERT_FACE_Y_TOO_FAR_UP: { mStatusYAxis = FaceStatusYAxis_TooFarUp; } break;
			case PXC3DScan::ALERT_FACE_Y_TOO_FAR_DOWN: {mStatusYAxis = FaceStatusYAxis_TooFarDown; } break;
				
			case PXC3DScan::ALERT_FACE_Z_IN_RANGE: { mStatusZAxis = FaceStatusZAxis_InRange; } break;
			case PXC3DScan::ALERT_FACE_Z_TOO_CLOSE: { mStatusZAxis = FaceStatusZAxis_TooClose; } break;
			case PXC3DScan::ALERT_FACE_Z_TOO_FAR: { mStatusZAxis = FaceStatusZAxis_TooFar; } break;
				
			case PXC3DScan::ALERT_FACE_YAW_IN_RANGE: { mStatusYaw = FaceStatusYaw_InRange; } break;
			case PXC3DScan::ALERT_FACE_YAW_TOO_FAR_LEFT: { mStatusYaw = FaceStatusYaw_TooFarLeft; } break;
			case PXC3DScan::ALERT_FACE_YAW_TOO_FAR_RIGHT: { mStatusYaw = FaceStatusYaw_TooFarRight; } break;
				
			case PXC3DScan::ALERT_FACE_PITCH_IN_RANGE: { mStatusPitch = FaceStatusPitch_InRange; } break;
			case PXC3DScan::ALERT_FACE_PITCH_TOO_FAR_UP: { mStatusPitch = FaceStatusPitch_TooFarUp; } break;
			case PXC3DScan::ALERT_FACE_PITCH_TOO_FAR_DOWN: { mStatusPitch = FaceStatusPitch_TooFarDown; } break;
		}
	}
	PXC3DScan::AlertData Data;

	FaceStatusDetected GetStatusDetected() { return mStatusDetected; }
	FaceStatusXAxis GetStatusXAxis() { return mStatusXAxis; }
	FaceStatusYAxis GetStatusYAxis() { return mStatusYAxis; }
	FaceStatusZAxis GetStatusZAxis() { return mStatusZAxis; }
	FaceStatusYaw GetStatusYaw() { return mStatusYaw; }
	FaceStatusPitch GetStatusPitch() { return mStatusPitch; }

private:
	
	FaceStatusDetected mStatusDetected;
	FaceStatusXAxis mStatusXAxis;
	FaceStatusYAxis mStatusYAxis;
	FaceStatusZAxis mStatusZAxis;
	FaceStatusYaw mStatusYaw;
	FaceStatusPitch mStatusPitch;
};

MyAlertHandler gAlertHandler;

FaceStatusDetected FaceScan_StatusDetected() { return gAlertHandler.GetStatusDetected(); }
FaceStatusXAxis FaceScan_StatusXAxis() { return gAlertHandler.GetStatusXAxis(); }
FaceStatusYAxis FaceScan_StatusYAxis() { return gAlertHandler.GetStatusYAxis(); }
FaceStatusZAxis FaceScan_StatusZAxis() { return gAlertHandler.GetStatusZAxis(); }
FaceStatusYaw FaceScan_StatusYaw() { return gAlertHandler.GetStatusYaw(); }
FaceStatusPitch FaceScan_StatusPitch() { return gAlertHandler.GetStatusPitch(); }

static SFaceScanData gFaceGlob = {};

static char gFaceScanErrorMsg[4096] = { 0 };

const char *FaceScan_GetLastErrorMsg()
{
	return gFaceScanErrorMsg;
}

static void SetFaceScanError(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf_s(gFaceScanErrorMsg, fmt, args);
	va_end(args);
}

bool FaceScan_Init()
{

	// Make sure the runtime is installed
#if _WIN64
	const wchar_t *versionKey = RSSDK_REG_RUNTIME TEXT("\\Components\\scan3d");
#else
	const wchar_t *versionKey = RSSDK_REG_RUNTIME32 TEXT("\\Components\\scan3d");
#endif
	HKEY key = NULL;
	if (RegOpenKey(HKEY_LOCAL_MACHINE, versionKey, &key) != ERROR_SUCCESS)
	{
		SetFaceScanError("The RealSense SDK Runtime V%d must be installed (Compiled with %d.%d.%d.%d)", PXC_VERSION_MAJOR, PXC_VERSION_MAJOR, PXC_VERSION_MINOR, PXC_VERSION_BUILD, PXC_VERSION_REVISION);
		return false;
	}
	RegCloseKey(key);

	gAlertHandler.Reset();
	PXCSenseManager *sm = PXCSenseManager::CreateInstance();
	pxcStatus status;
	if (sm == NULL)
	{
		SetFaceScanError("Error PXCSenseManager::CreatingInstance");
		return false;
	}
	gFaceGlob.SenseManager = sm;
	
	status = sm->Enable3DScan();
	if (status != PXC_STATUS_NO_ERROR)
	{
		SetFaceScanError("Error Enable3DScan %d", status);
		FaceScan_Shutdown();
		return false;
	}
	
	PXC3DScan *fs = sm->Query3DScan();

	status = sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 1280, 720, 30.0f);
	if (status != PXC_STATUS_NO_ERROR)
	{
		SetFaceScanError("Error EnableStream Color %d", status);
		FaceScan_Shutdown();
		return false;
	}

	status = sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480, 30.0f);
	if (status != PXC_STATUS_NO_ERROR)
	{
		SetFaceScanError("Error EnableStream Depth %d", status);
		FaceScan_Shutdown();
		return false;
	}

	gFaceGlob.ScanConfig.startScan = false;
	gFaceGlob.ScanConfig.mode = PXC3DScan::ScanningMode::FACE;
	gFaceGlob.ScanConfig.options = PXC3DScan::ReconstructionOption::TEXTURE;
	gFaceGlob.ScanConfig.options = gFaceGlob.ScanConfig.options | PXC3DScan::ReconstructionOption::LANDMARKS;
	gFaceGlob.ScanConfig.maxTriangles = 0;

	status = fs->SetConfiguration(gFaceGlob.ScanConfig);
	if (status != PXC_STATUS_NO_ERROR)
	{
		SetFaceScanError("Error SetConfiguration %d", status);
		FaceScan_Shutdown();
		return false;
	}

	fs->Subscribe(&gAlertHandler);
	gFaceGlob.FaceScan = fs;
	
	status = sm->Init();
	if (status != PXC_STATUS_NO_ERROR)
	{
		SetFaceScanError("SenseManager::Init %d", status);
		FaceScan_Shutdown();
		return false;
	}
	return true;
}

void FaceScan_Shutdown()
{

	if (gFaceGlob.SenseManager != NULL)
	{
		gFaceGlob.SenseManager->Release();
		gFaceGlob.SenseManager = NULL;
	}
}

bool FaceScan_FrameAcquire()
{
	if (gFaceGlob.SenseManager == NULL)
		return false;
	assert(!gFaceGlob.FrameAcquired);
	pxcStatus acquireStatus = gFaceGlob.SenseManager->AcquireFrame(false);

	gFaceGlob.ScanningInProgress = gFaceGlob.FaceScan->IsScanning() != 0;
	
	gFaceGlob.FrameAcquired = acquireStatus >= pxcStatus::PXC_STATUS_NO_ERROR;
	return gFaceGlob.FrameAcquired;
}

PXC3DScan::AlertData FaceScan_GetAlertData()
{
	return gAlertHandler.Data;
}

PXCImage *FaceScan_GetColorImage()
{
	assert(gFaceGlob.FrameAcquired);
	PXCCapture::Sample *sample = gFaceGlob.SenseManager->QuerySample();
	return sample->color;
}

bool FaceScan_IsScanning()
{
	return gFaceGlob.ScanningInProgress;
}

PXCImage *FaceScan_GetScanPreviewImage()
{
	assert(gFaceGlob.FrameAcquired);
	if (!gFaceGlob.AcquiredPreviewImage)
	{
		gFaceGlob.AcquiredPreviewImage = gFaceGlob.FaceScan->AcquirePreviewImage();
	}
	return gFaceGlob.AcquiredPreviewImage;
}

void FaceScan_FrameRelease()
{
	assert(gFaceGlob.FrameAcquired);
	if (gFaceGlob.AcquiredPreviewImage)
	{
		gFaceGlob.AcquiredPreviewImage->Release();
		gFaceGlob.AcquiredPreviewImage = NULL;
	}

	gFaceGlob.SenseManager->ReleaseFrame();
	gFaceGlob.FrameAcquired = false;
}

void FaceScan_SaveScan(const char *filename)
{
	wchar_t  ws[256];
	swprintf(ws, 256, L"%hs", filename);
	gFaceGlob.FaceScan->Reconstruct(PXC3DScan::FileFormat::OBJ, ws);
}

void FaceScan_StartScan()
{
	gFaceGlob.ScanConfig.startScan = true;
	gFaceGlob.ScanConfig.mode = PXC3DScan::ScanningMode::FACE;
	gFaceGlob.ScanConfig.options = PXC3DScan::ReconstructionOption::TEXTURE;
	gFaceGlob.ScanConfig.options = gFaceGlob.ScanConfig.options | PXC3DScan::ReconstructionOption::LANDMARKS;
	gFaceGlob.ScanConfig.maxTriangles = 0;
	gFaceGlob.FaceScan->SetConfiguration(gFaceGlob.ScanConfig);
}

void FaceScan_CancelScan()
{
	gFaceGlob.ScanConfig.startScan = false;
	gFaceGlob.FaceScan->SetConfiguration(gFaceGlob.ScanConfig);
}

#endif

void FaceScan_MoveScanData(const char *srcObjFilename, const char *dstObjFilename)
{
	std::string tmpObjName = std::string(srcObjFilename);
	std::string drive;
	std::string dir;
	std::string filename;
	std::string ext;
	CPUTFileSystem::SplitPathAndFilename(srcObjFilename, &drive, &dir, &filename, &ext);

	std::string tmpMtlName = drive + dir + filename + ".mtl";
	std::string tmpImageName = drive + dir + filename + "image1.png";
	std::string tmpJsonName = drive + dir + filename + ".json";

	if (dstObjFilename == NULL)
	{
		DeleteFileA(tmpMtlName.c_str());
		DeleteFileA(tmpImageName.c_str());
		DeleteFileA(tmpJsonName.c_str());
		DeleteFileA(tmpObjName.c_str());
	}
	else
	{
		CPUTFileSystem::SplitPathAndFilename(dstObjFilename, &drive, &dir, &filename, &ext);
		std::string dstMtlName = drive + dir + filename + ".mtl";
		std::string dstImageName = drive + dir + filename + "image1.png";
		std::string dstJsonName = drive + dir + filename + ".json";
		std::string dstObjName = drive + dir + filename + ".obj";
		MoveFileExA(tmpMtlName.c_str(), dstMtlName.c_str(), MOVEFILE_REPLACE_EXISTING);
		MoveFileExA(tmpImageName.c_str(), dstImageName.c_str(), MOVEFILE_REPLACE_EXISTING);
		MoveFileExA(tmpJsonName.c_str(), dstJsonName.c_str(), MOVEFILE_REPLACE_EXISTING);
		MoveFileExA(tmpObjName.c_str(), dstObjName.c_str(), MOVEFILE_REPLACE_EXISTING);
	}
}

