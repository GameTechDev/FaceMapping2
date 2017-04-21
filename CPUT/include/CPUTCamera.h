/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __CPUTCamera_H__
#define __CPUTCamera_H__

#include <memory.h>
#include "CPUT.h"
#include "CPUTRenderNode.h"
#include "CPUTConfigBlock.h"
#include "CPUTFrustum.h"
#include "CPUTOSServices.h"

enum CPUT_PROJECTION_MODE {
    CPUT_PERSPECTIVE,
    CPUT_ORTHOGRAPHIC
};

#define ASSERT_ORTHOGRAPHIC ASSERT(mMode == CPUT_ORTHOGRAPHIC, "Camera not orthographic.");
#define ASSERT_PERSPECTIVE  ASSERT(mMode == CPUT_PERSPECTIVE,  "Camera not perspective.");

//-----------------------------------------------------------------------------
class CPUTCamera:public CPUTRenderNode
{
protected:
    CPUT_PROJECTION_MODE mMode;
    float            mFov;                // the field of view in degrees
    float            mNearPlaneDistance;
    float            mFarPlaneDistance;
    float            mAspectRatio;        // width/height.  TODO: Support separate pixel and viewport aspect ratios
    float            mWidth;              // width and height for orthographic projection
    float            mHeight;
    float4x4         mView;
    float4x4         mProjection;

public:
    CPUTFrustum mFrustum;

	static CPUTCamera* Create(CPUT_PROJECTION_MODE mode);
    ~CPUTCamera() {}

    CPUT_NODE_TYPE GetNodeType() { return CPUT_NODE_CAMERA;};

    // We can't afford to Update() every time we're asked for the view matrix.
    // Caller needs to make sure to Update() before entering render loop.
    const float4x4  *GetViewMatrix(void)       const { return &mView; }
    const float4x4  *GetProjectionMatrix(void) const { return &mProjection; }
    CPUT_PROJECTION_MODE GetProjectionMode()       const { return mMode; }
    float            GetNearPlaneDistance()    const { return mNearPlaneDistance; }
    float            GetFarPlaneDistance()     const { return mFarPlaneDistance; }
    float            GetWidth()                const { ASSERT_ORTHOGRAPHIC; return mWidth; }
    float            GetHeight()               const { ASSERT(mMode == CPUT_ORTHOGRAPHIC, "Camera not orthographic."); return mHeight; }
    float            GetAspectRatio()          const { return mAspectRatio; }
    float            GetFov()                  const { ASSERT_PERSPECTIVE; return mFov; }

    void             SetProjectionMatrix(const float4x4 &projection) { mProjection = projection; }
    void             SetNearPlaneDistance( float nearPlaneDistance ) { mNearPlaneDistance = nearPlaneDistance; }
    void             SetFarPlaneDistance(  float farPlaneDistance )  { mFarPlaneDistance  = farPlaneDistance; }
    void             SetWidth( float width)                          { ASSERT_ORTHOGRAPHIC; mWidth  = width;}
    void             SetHeight(float height)                         { ASSERT_ORTHOGRAPHIC; mHeight = height;};
    void             SetProjectionMode(CPUT_PROJECTION_MODE mode)    { mMode = mode; mFov = 0.0f; }
    void             SetAspectRatio(const float aspectRatio)         { mAspectRatio = aspectRatio; }
    void             SetFov(float fov)                               { ASSERT_PERSPECTIVE; mFov = fov; }

    void             LookAt( float xx, float yy, float zz );
    void             LookAt( const float3 &pp ) { LookAt( pp.x, pp.y, pp.z ); }
    void             Update( float deltaSeconds=0.0f );
    CPUTResult       LoadCamera(CPUTConfigBlock *pBlock, int *pParentID);
private:

    CPUTCamera( CPUT_PROJECTION_MODE mode = CPUT_PERSPECTIVE) : 
        mFov(75.0f * kPi/180.0f),
        mNearPlaneDistance(1.0f),
        mFarPlaneDistance(100.0f),
        mAspectRatio(16.0f/9.0f),
        mWidth(64),
        mHeight(64),
        mMode(mode)
    {
        // default maya position (roughly)
        SetPosition( 1.0f, 0.8f, 1.0f );
    }
};

//-----------------------------------------------------------------------------
class CPUTCameraController : public CPUTEventHandler
{
protected:
    CPUTRenderNode *mpCamera;
    float           mfMoveSpeed;
    float           mfLookSpeed;
    int             mnPrevFrameX;
    int             mnPrevFrameY;
    CPUTMouseState  mPrevFrameState;
    CPUTKeyState    keyPressed[KEY_NUM_KEYS];

    CPUTCameraController()
        : mpCamera(NULL)
        , mnPrevFrameX(0)
        , mnPrevFrameY(0)
        , mfMoveSpeed(1.0f)
        , mfLookSpeed(1.0f)
    {
        for (unsigned int i = 0; i < 100; i++) {
            keyPressed[i] = CPUT_KEY_UP;
        }
    }

public:
    virtual         ~CPUTCameraController(){ SAFE_RELEASE(mpCamera);}
    void            SetCamera(CPUTRenderNode *pCamera)  { SAFE_RELEASE(mpCamera); mpCamera = pCamera; if(pCamera){pCamera->AddRef();} }
    CPUTRenderNode *GetCamera(void) const               { return mpCamera; }
    void            SetMoveSpeed(float speed)           { mfMoveSpeed = speed; }
    void            SetLookSpeed(float speed)           { mfLookSpeed = speed; }
    virtual void    Update(float deltaSeconds=0.0f) = 0;
};

//-----------------------------------------------------------------------------
class CPUTCameraControllerFPS : public CPUTCameraController
{

public:
	static CPUTCameraControllerFPS * Create();

    unsigned int keyPressedIndex;
    void Update( float deltaSeconds=0.0f);
    CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state);
    CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);

protected:
	CPUTCameraControllerFPS() : CPUTCameraController() {}
};

//-----------------------------------------------------------------------------
class CPUTCameraControllerArcBall : public CPUTCameraController
{
public:
    void Update( float deltaSeconds=0.0f ) {}
    CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state) { return CPUT_EVENT_UNHANDLED; }
    CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID  message);
};

//-----------------------------------------------------------------------------
class CPUTCameraModelViewer : public CPUTCameraController
{
public:

	CPUTCameraModelViewer();
	~CPUTCameraModelViewer();

	void Update(float deltaSeconds = 0.0f);
	CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state);
	CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID  message);

	void SetTarget(float3 target);
	void SetDistance(float start, float min, float max);
	void SetViewAngles(float xAxisRotation, float yAxisRotation);

	float3 mTarget;
	float mDistance;
	float mDstDistance;
	float3 mViewAngles;

	float mDistanceMin;
	float mDistanceMax;
};

//-----------------------------------------------------------------------------
class CPUTCameraControllerOrthographic : public CPUTCameraController
{
public:
	CPUTCameraControllerOrthographic();
	~CPUTCameraControllerOrthographic();
	
	void SetViewportSize(float width, float height);

	void Update(float deltaSeconds = 0.0f);
	CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state);
	CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID  message);

	void SetLook(float3 target);
	void SetPosition(float3 position);

	void SetVolume(float3 minDim, float3 maxDim);
	void SetZoomRange(float minWidth, float minHeight, float maxWidth, float maxHeight);

private:

	float3 ClampPosition(float3 position);

	CPUTCamera *mCamera;
	float3 mTarget;
	float3 mMinDim;
	float3 mMaxDim;
	float mMinWidth;
	float mMaxWidth;
	float mMinHeight;
	float mMaxHeight;

	float mZoomRatio;
	float3 mPosition;
	float mCurWidth;
	float mCurHeight;

	float2 mViewportDim;

};

#endif //#ifndef __CPUTCamera_H__
