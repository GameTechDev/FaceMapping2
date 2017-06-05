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
#ifndef DISABLE_RSSDK

#include <pxcversion.h>
#include "Menu_Scan.h"
#include "MenuController.h"
#include "CPUTGuiControllerDX11.h"
#include "CPUTRenderParams.h"
#include "MenuGlob.h"
#include <ctime>

#include "../GameFaceScan.h"

enum ScanMenuIds
{
	ScanMenuIds_StartScanning = 0,
	ScanMenuIds_GotoMapping,

	ScanMenuIds_StopScanning,
	ScanMenuIds_CancelScanning,
};

void Menu_Scan::Init()
{
	MenuBase::Init();

	D3D11_TEXTURE2D_DESC desc2D = {};
	desc2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc2D.Width = 640;
	desc2D.Height = 480;
	desc2D.Usage = D3D11_USAGE_DYNAMIC;
	desc2D.ArraySize = 1;
	desc2D.SampleDesc.Count = 1;
	desc2D.SampleDesc.Quality = 0;
	desc2D.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc2D.MipLevels = 1;

	memset(&CameraColorTexture, 0, sizeof(CameraColorTexture));
	memset(&ScanPreviewTexture, 0, sizeof(ScanPreviewTexture));

	TextureWrap_Init(&CameraColorTexture, &desc2D);
	TextureWrap_Init(&ScanPreviewTexture, &desc2D);

	mFullscreenSprite = CPUTSprite::Create(-0.9f, -0.9f, 1.8f, 1.8f, GetCodeSpriteMaterial());
}

void Menu_Scan::Shutdown()
{
	TextureWrap_Release(&CameraColorTexture);
	TextureWrap_Release(&ScanPreviewTexture);
}

void Menu_Scan::ActivationChanged(bool active)
{
	MenuBase::ActivationChanged(active);
	CPUTGuiController *pGUI = MenuGlob_GUI();
	if (active)
	{
		SetState(ScanState_ColorFeed );
		if (!FaceScan_Init())
		{
			::MessageBoxA(NULL, FaceScan_GetLastErrorMsg(), "RS Scan Initialization Failed", MB_OK);
		}
	}
	else
	{
		mHeadMessages.clear();
		mStatusMessages.clear();

		FaceScan_Shutdown();
	}
}

void Menu_Scan::AddMessageControls()
{
	CPUTGuiController *pGUI = MenuGlob_GUI();
	// Create status text buttons
	
	mStatusMessages.clear();
	for (int i = 0; i < 7; i++)
	{
		CPUTText *text = CPUTText::Create("", 0, MenuGlob_GUI()->GetFont());
		text->SetPosition(20, 20 + i * 40);
		pGUI->AddControl(text, MENU_CPUT_PANEL_ID);
		mStatusMessages.push_back(text);
	}

	mHeadMessages.clear();
	for (int i = 0; i < 7; i++)
	{
		CPUTText *text = CPUTText::Create("", 0, MenuGlob_GUI()->GetFont());
		text->SetPosition(-300, 20 + i * 40);
		pGUI->AddControl(text, MENU_CPUT_PANEL_ID);
		mHeadMessages.push_back(text);
	}
}

void Menu_Scan::HandleCPUTEvent(int eventID, int controlID, CPUTControl *control)
{
	if (eventID == CPUT_EVENT_DOWN)
	{
		switch (controlID)
		{
		case ScanMenuIds_StartScanning:
		{
			SetState(ScanState_Scanning);
			FaceScan_StartScan();
		} break;
		case ScanMenuIds_GotoMapping:
		{
			MenuController_PushMenu(gMenu_FaceMapping, false);
		} break;
		case ScanMenuIds_StopScanning:
		{
			if (mScanState == ScanState_Scanning)
			{
				std::string filename;
				CPUTFileSystem::CombinePath(GetUserDataDirectory(), "_temp.obj", &filename);
				
				// delete any existing temp data
				FaceScan_MoveScanData(filename .c_str(), NULL);
				FaceScan_SaveScan(filename.c_str());

				mScanState = ScanState_ScanComplete;
				
				gMenu_FaceScanPreview->LoadFaceObj(filename, true, true);
				gMenu_FaceScanPreview->SetFaceScanMode(FaceScanPreviewMode_ApproveScan);

				MenuController_PushMenu(gMenu_FaceScanPreview);
			}
		} break;
		case ScanMenuIds_CancelScanning:
		{
			FaceScan_CancelScan();
			SetState(ScanState_ColorFeed);
		} break;
		}
	}
}

void Menu_Scan::SetState( ScanState state )
{
	CPUTGuiController *pGUI = (CPUTGuiController*)CPUTGuiControllerDX11::GetController();
	pGUI->DeleteControlsFromPanel(MENU_CPUT_PANEL_ID);

	switch (state)
	{
		case ScanState_ColorFeed:
		{
			pGUI->CreateButton("Begin Scanning", ScanMenuIds_StartScanning, MENU_CPUT_PANEL_ID, &mBeginScanButton);
			pGUI->CreateButton("Skip Scanning", ScanMenuIds_GotoMapping, MENU_CPUT_PANEL_ID);
			mBeginScanButton->SetEnable(false);
		} break;
		case ScanState_Scanning:
		{
			pGUI->CreateButton("Stop Scanning", ScanMenuIds_StopScanning, MENU_CPUT_PANEL_ID);
			pGUI->CreateButton("Cancel Scanning", ScanMenuIds_CancelScanning, MENU_CPUT_PANEL_ID);
		} break;
	}

	AddMessageControls();

	mScanState = state;
}

void Menu_Scan::Update(float dt)
{
	if (FaceScan_FrameAcquire())
	{
		// update messages
		if (mStatusMessages.size() != 0 )
		{
			CPUTText *tcon;
			int msgIdx = 0;
			{
				tcon = mStatusMessages[msgIdx++];
				FaceStatusDetected status = FaceScan_StatusDetected();
				tcon->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
				tcon->SetText(status == FaceStatusDetected_Detected ? "" : "Face Status: Not Detected");
			}
			{
				tcon = mStatusMessages[msgIdx++];
				FaceStatusZAxis status = FaceScan_StatusZAxis();
				if (status == FaceStatusZAxis_InRange)
				{
					tcon->SetText("");
				}
				else
				{
					tcon->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
					tcon->SetText((status == FaceStatusZAxis_TooClose) ? "Move Back" : "Move Closer");
				}
			}
			{
				tcon = mStatusMessages[msgIdx++];
				FaceStatusXAxis status = FaceScan_StatusXAxis();
				if (status == FaceStatusXAxis_InRange)
				{
					tcon->SetText("");
				}
				else
				{
					tcon->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
					tcon->SetText((status == FaceStatusXAxis_TooFarLeft) ? "Move Right" : "Move Left");
				}
			}
			{
				tcon = mStatusMessages[msgIdx++];
				FaceStatusYAxis status = FaceScan_StatusYAxis();
				if (status == FaceStatusYAxis_InRange)
				{
					tcon->SetText("");
				}
				else
				{
					tcon->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
					tcon->SetText((status == FaceStatusYAxis_TooFarUp) ? "Move Lower" : "Move Higher");
				}
			}

			{
				tcon = mStatusMessages[msgIdx++];
				FaceStatusYaw status = FaceScan_StatusYaw();
				if (status == FaceStatusYaw_InRange)
				{
					tcon->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
					tcon->SetText("");
				}
				else
				{
					tcon->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
					tcon->SetText((status == FaceStatusYaw_TooFarLeft) ? "Turn Right" : "Turn Left");
				}
			}
			{
				tcon = mStatusMessages[msgIdx++];
				FaceStatusPitch status = FaceScan_StatusPitch();
				if (status == FaceStatusPitch_InRange)
				{
					tcon->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
					tcon->SetText("");
				}
				else
				{
					tcon->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
					tcon->SetText((status == FaceStatusPitch_TooFarUp) ? "Tilt Down" : "Tilt Up");
				}
			}
		}
		
		float2 uValues = (mScanState == ScanState_ColorFeed) ? float2(1.0f, 0.0f) : float2(0.0f, 1.0f);
		mFullscreenSprite->SetUV(uValues.x, 0.0f, uValues.y, 1.0f); // flip image

		if (mScanState == ScanState_ColorFeed)
		{
			bool readyToScan = true;
			readyToScan &= FaceScan_StatusYaw() == FaceStatusYaw_InRange;
			readyToScan &= FaceScan_StatusPitch() == FaceStatusPitch_InRange;
			readyToScan &= FaceScan_StatusXAxis() == FaceStatusXAxis_InRange;
			readyToScan &= FaceScan_StatusYAxis() == FaceStatusYAxis_InRange;
			readyToScan &= FaceScan_StatusZAxis() == FaceStatusZAxis_InRange;
			readyToScan &= FaceScan_StatusDetected() == FaceStatusDetected_Detected;

			mBeginScanButton->SetEnable(readyToScan);
			CopyPXCImageToTexture(FaceScan_GetColorImage(), &CameraColorTexture);
		}
		if (mScanState == ScanState_Scanning)
		{
			CopyPXCImageToTexture(FaceScan_GetScanPreviewImage(), &CameraColorTexture);
		}
		FaceScan_FrameRelease();
	}
}

void Menu_Scan::Render(CPUTRenderParameters &renderParams )
{
	SetCodeTexture(0, &CameraColorTexture);
	mFullscreenSprite->DrawSprite(renderParams);
}

#endif // #ifndef DISABLE_RSSDK