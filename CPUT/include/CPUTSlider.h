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
#ifndef __CPUTSLIDER_H__
#define __CPUTSLIDER_H__

#include "CPUTControl.h"

struct CPUTGUIVertex;
class CPUTFont;
class CPUTText;

typedef enum CPUTSliderState
{
    CPUT_SLIDER_NIB_UNPRESSED,
    CPUT_SLIDER_NIB_PRESSED,
}CPUTSliderState;

#define CPUT_NUM_IMAGES_IN_SLIDER_ARRAY 6
#define CPUT_NUM_QUADS_IN_SLIDER_ARRAY 6
#define CPUT_DEFAULT_TRAY_WIDTH 198

// Button base - common functionality for all the controls
//-----------------------------------------------------------------------------
class CPUTSlider:public CPUTControl
{
protected:
    // control position/location/dimensionss
    CPUT_RECT mControlDimensions;

    // slider state/range
    float     mSliderStartValue;
    float     mSliderEndValue;
    int       mSliderNumberOfSteps;
    int       mSliderNumberOfTicks;
    float     mCurrentSliderValue;
    float     mScale;
    // others
    CPUTSliderState mSliderState;

    void   InitialStateSet();

public:

    virtual ~CPUTSlider();

	static CPUTSlider* Create(std::string const& SliderText, CPUTControlID controlID, CPUTFont * mpFont, float scale);

    //CPUTEventHandler
    virtual CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state){UNREFERENCED_PARAMETER(key); return CPUT_EVENT_UNHANDLED;}
    virtual CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);

    // CPUTControl  &graphical manipulation
    void GetPosition(int &x, int &y);
    void GetDimensions(int &width, int &height);
    void SetPosition(int x, int y);
    virtual bool ContainsPoint(int x, int y);
    void SetText(const std::string ControlText);
    virtual void SetEnable(bool in_bEnabled);
    virtual unsigned int GetOutputVertexCount();

    //CPUTSlider
    CPUTResult SetScale(float StartValue, float EndValue, int NumberOfSteps);
    CPUTResult SetNumberOfTicks(int NumberOfTicks);
    float GetValue();
    CPUTResult SetValue(float fValue);
    void SetTickDrawing(bool DrawTicks);

    // Register assets
    static CPUTResult RegisterStaticResources();
    static CPUTResult UnRegisterStaticResources();

    CPUTResult RegisterInstanceResources();
    CPUTResult UnRegisterInstanceResources();
    
    // draw
    virtual void Draw(CPUTGUIVertex *pVertexBufferMirror, UINT *pInsertIndex, UINT pMaxBufferSize);

protected:
        struct LocationGuides
    {
        float TickIndent;
        float TextDownIndent;
        float GripDownIndent;
        float TrayDownIndent;
        float TickSpacing;
        float StepSpacing;
        float TotalWidth;
        float TotalHeight;
    };

    CPUTText *mpControlText;
    CPUTFont *mpFont;
    int mSliderNubTickLocation;
    float mSliderNubLocation;
    bool mbDrawTicks;

    // events
    bool mbMouseInside;
    bool mbStartedClickInside;
    


    // uberbuffer    
    static CPUT_SIZE mpActiveImageSizeList[CPUT_NUM_IMAGES_IN_SLIDER_ARRAY];
    static CPUT_SIZE mpPressedImageSizeList[CPUT_NUM_IMAGES_IN_SLIDER_ARRAY];
    static CPUT_SIZE mpDisabledImageSizeList[CPUT_NUM_IMAGES_IN_SLIDER_ARRAY];

    CPUTGUIVertex *mpMirrorBufferActive;
    CPUTGUIVertex *mpMirrorBufferPressed;
    CPUTGUIVertex *mpMirrorBufferDisabled;
    void Recalculate();
    void RecalculateSizes();

    // helper functions
    bool PointingAtNub(int x, int y);
    bool PointingAtSlider(int x, int y);
    void CalculateLocationGuides(LocationGuides &guides);
    void SnapToNearestTick(); 
    void ClampTicks();

private:

    // button should self-register with the GuiController on create
    CPUTSlider(const std::string ControlText, CPUTControlID id, CPUTFont *pFont, float scale = 1.0f);
};

#endif //#ifndef __CPUTSLIDER_H__
