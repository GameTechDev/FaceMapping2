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
#ifndef __CPUTGUICONTROLLER_H__
#define __CPUTGUICONTROLLER_H__

#include <vector>

#include "CPUTEventHandler.h"

#include "CPUTButton.h"
#include "CPUTText.h"
#include "CPUTCheckbox.h"
#include "CPUTSlider.h"
#include "CPUTDropdown.h"

class CPUTFont;

class CPUTGuiController:public CPUTEventHandler
{
 
public:
    struct GUIConstantBufferVS
    {
        float4x4 Projection;
        float4x4 Model;
    };

    CPUTGuiController();
    virtual ~CPUTGuiController();

    virtual CPUTResult Initialize(const std::string& materialName, const std::string& fontName) = 0;
    virtual void Draw()=0;

    // get GUI controller base class
    static CPUTGuiController *GetController();

    //CPUTEventHandler members
    virtual CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state) {UNREFERENCED_PARAMETER(key); return CPUT_EVENT_UNHANDLED;}
    virtual CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);
    void SetCallback(CPUTCallbackHandler *pHandler, bool ForceAll=false);       // sets the event handler callback on all registered controls
    
    // Panels
    CPUTResult      AddControl(CPUTControl *pControl, CPUTControlID panelID); 
    CPUTResult      DeleteControl(CPUTControlID controlID);

    CPUTResult      FindControl(CPUTControlID controlID, CPUTControl **ppControl, CPUTControlID *pPanelID);  // search all panels to find a control and its panelID
    CPUTControl    *GetControl(CPUTControlID controlID, CPUTResult *pResult = NULL);            // search all panels to find a control
    
    CPUTResult      SetActivePanel(CPUTControlID panelID);                                      // sets the actively displayed panel
    CPUTControlID   GetActivePanelID();                                                         // returns the ID of the active panel
    
    CPUTResult      GetActiveControlList(std::vector<CPUTControl*> *ppControlList);
    CPUTResult      RemoveControlFromPanel(CPUTControlID controlID, CPUTControlID panelID=-1);  // removes specified control from the panel (does not delete the control)
	CPUTResult      RemoveControlFromPanel(CPUTControl *control, CPUTControlID panelID = -1);  // removes specified control from the panel (does not delete the control)
    CPUTResult      DeletePanel(CPUTControlID panelID);                                         // removes panel and deletes all controls associated with it
	void            DeleteAllControls();                                // deletes all controls and all panels
	void			DeleteControlsFromPanel(CPUTControlID panelID);
    int             GetNumberOfControlsInPanel(CPUTControlID panelID=-1);                       // returns the number of controls in a specific panel
    bool            IsControlInPanel(CPUTControlID controlID, CPUTControlID panelID=-1);        // Is specified control in panel?
    
    void            SetFont(CPUTFont *pFont) { mpFont = pFont; }
	CPUTFont		*GetFont() { return mpFont; }

	// drawing/callbacks
    void Resize(int width, int height);
    void Update();
    //use "ControlModified()" when modifying a control outside of the normal event handing
    //e.g. when updating text controls.
    void ControlModified();
    void RecalculateLayout();

    CPUTResult CreateButton(const std::string pButtonText, CPUTControlID controlID, CPUTControlID panelID, CPUTButton **ppButton=NULL);
    CPUTResult CreateSlider(const std::string pSliderText, CPUTControlID controlID, CPUTControlID panelID, CPUTSlider **ppSlider=NULL, float scale = 1.0f);
    CPUTResult CreateCheckbox(const std::string pCheckboxText, CPUTControlID controlID, CPUTControlID panelID, CPUTCheckbox **ppCheckbox=NULL, float scale = 1.0f);
    CPUTResult CreateDropdown(const std::string pSelectionText, CPUTControlID controlID, CPUTControlID panelID, CPUTDropdown **ppDropdown=NULL);
    CPUTResult CreateText(const std::string Text,  CPUTControlID controlID, CPUTControlID panelID, CPUTText **ppStatic=NULL);    

protected:
    virtual void UpdateConstantBuffer()=0;
    virtual CPUTResult UpdateUberBuffers() = 0;
    
    bool mRecalculate;
    bool mUpdateBuffers;
    
    struct Panel
    {
        CPUTControlID mpanelID;
        CPUTControl *mpFocusControl;
        std::vector<CPUTControl*> mControlList;
    };

    // list of panels which have lists of controls associated with it
    std::vector<Panel*>  mControlPanelIDList;

    // the active panel list
    CPUTControlID  mActiveControlPanelSlotID;
    CPUTCallbackHandler *mpHandler;

    // active control
    CPUTControl *mpFocusControl;

    CPUTFont                *mpFont;
    int mWidth, mHeight;

    const UINT                  mBufferSize;

    CPUTGUIVertex              *mpMirrorBuffer;
    UINT                        mUberBufferIndex;   
private:
    CPUTGuiController(const CPUTGuiController &);
    CPUTGuiController & operator=(const CPUTGuiController &);

    UINT FindPanelIDIndex(CPUTControlID panelID);

};
#endif //#ifndef __CPUTGUICONTROLLER_H__
