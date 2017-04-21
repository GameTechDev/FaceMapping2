/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CPUTGuiController.h"
#include "CPUTFont.h"

const unsigned int CPUT_GUI_BUFFER_MAX = 5000;         // size (in number of verticies) for all GUI control graphics

// Constructor
//----------------------`-------------------------------------------------------
CPUTGuiController::CPUTGuiController():
mRecalculate(false),
mUpdateBuffers(false),
    mControlPanelIDList(),
    mActiveControlPanelSlotID(CPUT_CONTROL_ID_INVALID),
    mpHandler(NULL),
    mpFocusControl(NULL),
    mpFont(NULL),
    mWidth(-1),
    mHeight(-1),
    mBufferSize(CPUT_GUI_BUFFER_MAX),
    mpMirrorBuffer(NULL),
    mUberBufferIndex(0)
{
    mpMirrorBuffer = new CPUTGUIVertex[CPUT_GUI_BUFFER_MAX];
    CPUTText::RegisterStaticResources();
    CPUTButton::RegisterStaticResources();
    CPUTCheckbox::RegisterStaticResources();
    CPUTSlider::RegisterStaticResources();
    CPUTDropdown::RegisterStaticResources();
}

// Destructor
//-----------------------------------------------------------------------------
CPUTGuiController::~CPUTGuiController()
{
    SAFE_RELEASE(mpFont);
    SAFE_DELETE_ARRAY(mpMirrorBuffer);
    DeleteAllControls();
}

//CPUTEventHandler members

// Handle mouse events
// The gui controller dispatches the event to each control until handled or
// it passes through unhandled
//-----------------------------------------------------------------------------
CPUTEventHandledCode CPUTGuiController::HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)
{
    // if there is no active panel, then we just return
    if(CPUT_CONTROL_ID_INVALID == mActiveControlPanelSlotID)
    {
        return CPUT_EVENT_PASSTHROUGH;
    }

    // walk the list of controls on the screen and see if they are to handle any of these events
    CPUTEventHandledCode EventResult = CPUT_EVENT_PASSTHROUGH;
    for(UINT i=0; i<mControlPanelIDList[mActiveControlPanelSlotID]->mControlList.size(); i++)
    {
        if(CPUT_EVENT_HANDLED == EventResult)
        {
            // walk the rest of the controls, updating them with the event, but with an 'invalid' location
            // this is a not-so-great but works bug fix for this problem:
            // If you click on another control besides a dropped-down dropdown, then you get a painting error
            // You need to send a closed event to any remaining dropdowns...

            mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[i]->HandleMouseEvent(-1,-1,wheel,state, message);
        }
        else
        {
            EventResult = mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[i]->HandleMouseEvent(x,y,wheel,state, message);

			if (i >= mControlPanelIDList[mActiveControlPanelSlotID]->mControlList.size())
				break;
            // if the control says it handled this event, do not pass it through to underlying controls
            // this is important for things like dropdowns that could dynamically overlap other controls
            if( CPUT_EVENT_HANDLED == EventResult)
            {
                mpFocusControl = mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[i];
                mUpdateBuffers = true;
            }
        }
        if(mpFocusControl && !mpFocusControl->HasFocus())
        {
            mpFocusControl = NULL;
            mUpdateBuffers = true;
        }
    }

    return EventResult; //CPUT_EVENT_PASSTHROUGH;
}

// Panels

// finds panel with matching ID and sets it as the active one
// if panelID is CPUT_CONTROL_ID_INVALID - it will disable all panels
//-----------------------------------------------------------------------------
CPUTResult CPUTGuiController::SetActivePanel(CPUTControlID panelID)
{
    CPUTControlID panelSlotID = FindPanelIDIndex(panelID);

    // if we found it, set it active
    if(CPUT_CONTROL_ID_INVALID == panelSlotID)
    {
        return CPUT_ERROR_NOT_FOUND;
    }

    // store previously active control
    mControlPanelIDList[mActiveControlPanelSlotID]->mpFocusControl = mpFocusControl;

    // change the active panel and refresh screen
    mActiveControlPanelSlotID = panelSlotID;
    mpFocusControl = mControlPanelIDList[mActiveControlPanelSlotID]->mpFocusControl;
    return CPUT_SUCCESS;
}

// returns the ID of the active panel
//-----------------------------------------------------------------------------
CPUTControlID CPUTGuiController::GetActivePanelID()
{
    if(CPUT_CONTROL_ID_INVALID == mActiveControlPanelSlotID)
        return CPUT_CONTROL_ID_INVALID;

    return mControlPanelIDList[mActiveControlPanelSlotID]->mpanelID;
}

// Get the list of controls currently being displayed
//-----------------------------------------------------------------------------
CPUTResult CPUTGuiController::GetActiveControlList(std::vector<CPUTControl*> *ppControlList)
{
	ASSERT(ppControlList != NULL, "CPUTGuiController::GetActiveControlList - pControlList is NULL");

    if(CPUT_CONTROL_ID_INVALID == mActiveControlPanelSlotID)
    {
        // return CPUT_GUI_INVALID_CONTROL_ID;
    }

    if(NULL==ppControlList)
    {
        return CPUT_ERROR_INVALID_PARAMETER;
    }

    // todo: make a copy instead to avoid deletion problems?
    *ppControlList = mControlPanelIDList[mActiveControlPanelSlotID]->mControlList;

    return CPUT_SUCCESS;
}


// removes specified control from the panel (does not delete the control)
//-----------------------------------------------------------------------------
CPUTResult CPUTGuiController::RemoveControlFromPanel(CPUTControlID controlID, CPUTControlID panelID)
{
    CPUTControlID panelSlotID;
    if(CPUT_CONTROL_ID_INVALID==panelID)
    {
        // use the currently active panel if none specified
        panelSlotID = mActiveControlPanelSlotID;
    }
    else
    {
        panelSlotID = FindPanelIDIndex(panelID);
    }

    // walk list of controls in the panel and see if control is there
    for(UINT i=0; i<mControlPanelIDList[panelSlotID]->mControlList.size(); i++)
    {
        if( controlID == mControlPanelIDList[panelSlotID]->mControlList[i]->GetControlID() )
        {
            mControlPanelIDList[panelSlotID]->mControlList.erase( (mControlPanelIDList[panelSlotID]->mControlList.begin() + i) );
            return CPUT_SUCCESS;
        }
    }
    return CPUT_WARNING_NOT_FOUND;
}

CPUTResult CPUTGuiController::RemoveControlFromPanel(CPUTControl *control, CPUTControlID panelID )  // removes specified control from the panel (does not delete the control)
{
	CPUTControlID panelSlotID;
	if (CPUT_CONTROL_ID_INVALID == panelID)
	{
		// use the currently active panel if none specified
		panelSlotID = mActiveControlPanelSlotID;
	}
	else
	{
		panelSlotID = FindPanelIDIndex(panelID);
	}

	// walk list of controls in the panel and see if control is there
	for (UINT i = 0; i < mControlPanelIDList[panelSlotID]->mControlList.size(); i++)
	{
		if (control == mControlPanelIDList[panelSlotID]->mControlList[i])
		{
			mControlPanelIDList[panelSlotID]->mControlList.erase((mControlPanelIDList[panelSlotID]->mControlList.begin() + i));
			return CPUT_SUCCESS;
		}
	}
	return CPUT_WARNING_NOT_FOUND;
}

void CPUTGuiController::DeleteControlsFromPanel(CPUTControlID panelID)
{
	// find the panel they specified
	CPUTControlID panelSlotID = FindPanelIDIndex(panelID);

	if (panelSlotID == -1)
	{
		return;
	}

	// walk the panel and delete all the controls in it
	for (UINT i = 0; i < mControlPanelIDList[panelSlotID]->mControlList.size(); i++)
	{
		// delete each control
		CPUTControl *control = mControlPanelIDList[panelSlotID]->mControlList[i];
		if (mpFocusControl == control)
			mpFocusControl = NULL;
		SAFE_DELETE_ARRAY(mControlPanelIDList[panelSlotID]->mControlList[i]);
	}
	mControlPanelIDList[panelSlotID]->mControlList.clear();
}

// removes panel and deletes all controls associated with it
//-----------------------------------------------------------------------------
CPUTResult CPUTGuiController::DeletePanel(CPUTControlID panelID)
{

    // find the panel they specified
    CPUTControlID panelSlotID = FindPanelIDIndex(panelID);

	if (panelSlotID == -1)
	{
		return CPUT_ERROR_INVALID_PARAMETER;
	}

    // walk the panel and delete all the controls in it
    for(UINT i=0; i<mControlPanelIDList[panelSlotID]->mControlList.size(); i++)
    {
        // delete each control
        SAFE_DELETE_ARRAY(mControlPanelIDList[panelSlotID]->mControlList[i]);        
    }

    // remove this panel from the control list
    mControlPanelIDList.erase(mControlPanelIDList.begin()+panelSlotID);

    // if the panel you delete is the active one, set the active panel to first
    // or invalid if none are left
    if(mActiveControlPanelSlotID == panelSlotID)
    {
        // set panel to the first panel
        if(0 == mControlPanelIDList.size())
            mActiveControlPanelSlotID = CPUT_CONTROL_ID_INVALID;
        else
            mActiveControlPanelSlotID = 0;
    }
    return CPUT_SUCCESS;
}

// private: Finds the index of the specified panel ID code in mControlPanelIDList[]
//-----------------------------------------------------------------------------
UINT CPUTGuiController::FindPanelIDIndex(CPUTControlID panelID)
{
    CPUTControlID foundID = CPUT_CONTROL_ID_INVALID;

    for(UINT i=0; i<mControlPanelIDList.size(); i++)
    {
        if(panelID == mControlPanelIDList[i]->mpanelID)
            return i;
    }

    return foundID;
}

// Returns the number of controls in the currently ACTIVE panel
//-----------------------------------------------------------------------------
int CPUTGuiController::GetNumberOfControlsInPanel(CPUTControlID panelID)
{
    // if not specified, returns count of currently active pane
    if(-1 == panelID)
    {
        if(CPUT_CONTROL_ID_INVALID == mActiveControlPanelSlotID)
            return 0;
        return (int) mControlPanelIDList[mActiveControlPanelSlotID]->mControlList.size();
    }

    // if panelID specified, return that number, or 0 if not found
    UINT foundID = FindPanelIDIndex(panelID);
    if(CPUT_CONTROL_ID_INVALID != foundID)
        return (int) mControlPanelIDList[foundID]->mControlList.size();

    return CPUT_CONTROL_ID_INVALID;
}

// is the control in the panel?
//-----------------------------------------------------------------------------
bool CPUTGuiController::IsControlInPanel(CPUTControlID controlID, CPUTControlID panelID)
{
    CPUTControlID panelSlotID;
    if(-1==panelID)
    {
        // use the currently active panel if none specified
        panelSlotID = mActiveControlPanelSlotID;
    }
    else
    {
        panelSlotID = FindPanelIDIndex(panelID);
    }

    // invalid panel
    if(CPUT_CONTROL_ID_INVALID == panelSlotID)
        return false;

    // walk list of controls in the panel and see if control is there
    for(UINT i=0; i<mControlPanelIDList[panelSlotID]->mControlList.size(); i++)
    {
        if( controlID == mControlPanelIDList[panelSlotID]->mControlList[i]->GetControlID() )
            return true;
    }
    return false;
}

// Control management

// Add a control (to a panel)
//-----------------------------------------------------------------------------
CPUTResult CPUTGuiController::AddControl(CPUTControl *pControl, CPUTControlID panelID)
{
    mRecalculate = true;
    // set the global callback handler for this object
    pControl->SetControlCallback(mpHandler);

    CPUTControlID panelSlotID = FindPanelIDIndex(panelID);

    // if the panel wasn't found, add a new one
    if(CPUT_CONTROL_ID_INVALID == panelSlotID)
    {
        Panel *pNewControlPanel = new Panel();
        pNewControlPanel->mpanelID = panelID;
        pNewControlPanel->mControlList.clear();
        pNewControlPanel->mpFocusControl = NULL;

        mControlPanelIDList.push_back( pNewControlPanel );
        panelSlotID = (int)mControlPanelIDList.size()-1;

        // make the newly added panel active if none was
        // active before
        if(CPUT_CONTROL_ID_INVALID == mActiveControlPanelSlotID)
            mActiveControlPanelSlotID = panelSlotID;
    }

    // store the control in the list
    mControlPanelIDList[panelSlotID]->mControlList.push_back(pControl);

    return CPUT_SUCCESS;
}

// returns a pointer to the specified control
//-----------------------------------------------------------------------------
CPUTControl* CPUTGuiController::GetControl(CPUTControlID controlID, CPUTResult *pResult)
{
    if (pResult)
    {
        *pResult = CPUT_SUCCESS;
    }

    for(UINT i=0; i<mControlPanelIDList.size(); i++)
    {
        for(UINT j=0; j<mControlPanelIDList[i]->mControlList.size(); j++)
        {
            if(controlID == mControlPanelIDList[i]->mControlList[j]->GetControlID())
            {
                return mControlPanelIDList[i]->mControlList[j];
            }
        }
    }
    
    if (pResult)
    {
        *pResult = CPUT_GUI_INVALID_CONTROL_ID;
    }
    return NULL;
}

// Find control and return pointer and panel id for it
//-----------------------------------------------------------------------------
CPUTResult CPUTGuiController::FindControl(CPUTControlID controlID, CPUTControl **ppControl, CPUTControlID *pPanelID)
{
    for(UINT i=0; i<mControlPanelIDList.size(); i++)
    {
        for(UINT j=0; j<mControlPanelIDList[i]->mControlList.size(); j++)
        {
            if(controlID == mControlPanelIDList[i]->mControlList[j]->GetControlID())
            {
                // found it!
                *pPanelID = mControlPanelIDList[i]->mpanelID;
                *ppControl = mControlPanelIDList[i]->mControlList[j];
                return CPUT_SUCCESS;
            }
        }
    }
    return CPUT_ERROR_NOT_FOUND;
}

// Delete all the controls in the list
//-----------------------------------------------------------------------------
void CPUTGuiController::DeleteAllControls()
{
    // set active panel to invalid
    mActiveControlPanelSlotID = CPUT_CONTROL_ID_INVALID;

    // walk list of panels deleting each list of controls
    int panelCount = (int) mControlPanelIDList.size();
    for(int i=0; i<panelCount; i++)
    {
        int controlListCount = (int)mControlPanelIDList[i]->mControlList.size();
        for(int j=0; j<controlListCount; j++)
        {
            SAFE_DELETE( mControlPanelIDList[i]->mControlList[j] );            
        }

        // erase this panel's control list
        mControlPanelIDList[i]->mControlList.clear();

        // delete the panel object
        SAFE_DELETE( mControlPanelIDList[i] );
        mControlPanelIDList[i] = NULL;
    }

    // clear the panel list
    mControlPanelIDList.clear();

    mRecalculate = true;
    mUpdateBuffers = true;
}


// Sets the object to call back for all newly created objects
// if ForceAll=true, then it walks the list of all the registered controls
// in all the panels and resets their callbacks
//-----------------------------------------------------------------------------
void CPUTGuiController::SetCallback(CPUTCallbackHandler *pHandler, bool ForceAll)
{
    if(true == ForceAll)
    {
        // walk list of ALL the controls and reset the callback pointer
        int panelCount = (int) mControlPanelIDList.size();
        for(int i=0; i<panelCount; i++)
        {
            int controlListCount = (int) mControlPanelIDList[i]->mControlList.size();
            for(int j=0; j<controlListCount; j++)
            {
                mControlPanelIDList[i]->mControlList[j]->SetControlCallback(pHandler);
            }
        }
    }
    else
    {
        // set the callback handler to be used on any NEW controls added
        mpHandler = pHandler;
    }
}


// Create a button control and add it to the GUI layout controller
//--------------------------------------------------------------------------------
CPUTResult CPUTGuiController::CreateButton(const std::string ButtonText, CPUTControlID controlID, CPUTControlID panelID, CPUTButton **ppButton)
{
    // create the control
    CPUTButton *pButton = CPUTButton::Create(ButtonText, controlID, mpFont);
    ASSERT(NULL != pButton, "Failed to create control." );

    // return control if requested
    if(NULL!=ppButton)
    {
        *ppButton = pButton;
    }

    // add control to the gui manager
    return this->AddControl(pButton, panelID);

}

// Create a slider control and add it to the GUI layout controller
//--------------------------------------------------------------------------------
CPUTResult CPUTGuiController::CreateSlider(const std::string SliderText, CPUTControlID controlID, CPUTControlID panelID, CPUTSlider **ppSlider, float scale)
{
    // create the control
    CPUTSlider *pSlider = CPUTSlider::Create(SliderText, controlID, mpFont, scale);
    ASSERT(NULL!=pSlider, "Failed creating slider" );

    // return control if requested
    if(NULL!=ppSlider)
    {
        *ppSlider = pSlider;
    }
    
    // add control to the gui manager
    return this->AddControl(pSlider, panelID);
}

// Create a checkbox control and add it to the GUI layout controller
//--------------------------------------------------------------------------------
CPUTResult CPUTGuiController::CreateCheckbox(const std::string CheckboxText, CPUTControlID controlID, CPUTControlID panelID, CPUTCheckbox **ppCheckbox, float scale)
{
    // create the control
    CPUTCheckbox *pCheckbox = CPUTCheckbox::Create(CheckboxText, controlID, mpFont, scale);
    ASSERT(NULL!=pCheckbox, "Failed creating checkbox" );

    // return control if requested
    if(NULL!=ppCheckbox)
    {
        *ppCheckbox = pCheckbox;
    }

    // add control to the gui manager
    return this->AddControl(pCheckbox, panelID);
}

// Create a dropdown control and add it to the GUI layout controller
//--------------------------------------------------------------------------------
CPUTResult CPUTGuiController::CreateDropdown(const std::string SelectionText, CPUTControlID controlID, CPUTControlID panelID, CPUTDropdown **ppDropdown)
{
    // create the control
    CPUTDropdown *pDropdown = CPUTDropdown::Create(SelectionText, controlID, mpFont);
    ASSERT(NULL!=pDropdown, "Failed creating control" );

    // return control if requested
    if(NULL!=ppDropdown)
    {
        *ppDropdown = pDropdown;
    }

    // add control to the gui manager
    CPUTResult result;
    result = this->AddControl(pDropdown, panelID);

    return result;
}

// Create a text item (static text)
//--------------------------------------------------------------------------------
CPUTResult CPUTGuiController::CreateText(const std::string Text, CPUTControlID controlID, CPUTControlID panelID, CPUTText **ppStatic)
{
    // create the control
    CPUTText *pStatic=NULL;
    pStatic = CPUTText::Create(Text, controlID, mpFont);
    ASSERT(NULL!=pStatic, "Failed creating static" );
    if(NULL != ppStatic)
    {
        *ppStatic = pStatic;
    }

    // add control to the gui manager
    return this->AddControl(pStatic, panelID);
}


// Deletes a control from the GUI manager
// Will delete all instances of the control no matter which panel(s) it is in and then
// deallocates the memory for the control
//--------------------------------------------------------------------------------
CPUTResult CPUTGuiController::DeleteControl(CPUTControlID controlID)
{
    mRecalculate = true;
    // look thruogh all the panels and delete the item with this controlID
    // for each panel
    std::vector <CPUTControl*> pDeleteList;

    for(UINT i=0; i<mControlPanelIDList.size(); i++)
    {
        // walk list of controls
        for(UINT j=0; j<mControlPanelIDList[i]->mControlList.size(); j++)
        {
            if(controlID == mControlPanelIDList[i]->mControlList[j]->GetControlID())
            {
                // found an instance of the control we wish to delete
                // see if it's in the list already
                bool bFound = false;
                for(UINT x=0; x<pDeleteList.size(); x++)
                {
                    if( mControlPanelIDList[i]->mControlList[j] ==  pDeleteList[x] )
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                {
                    // store for deleting
                    pDeleteList.push_back( mControlPanelIDList[i]->mControlList[j] );
                }

                // remove the control from the container list
                mControlPanelIDList[i]->mControlList.erase( mControlPanelIDList[i]->mControlList.begin() + j );
            }
        }
    }

    // delete the control(s) we found with this id
    for(UINT i=0; i<pDeleteList.size(); i++)
    {
        SAFE_DELETE( pDeleteList[i] );
    }

    mRecalculate = true;
    mUpdateBuffers = true;

    return CPUT_SUCCESS;
}


//--------------------------------------------------------------------------------
void CPUTGuiController::Update()
{
    if (mActiveControlPanelSlotID == CPUT_CONTROL_ID_INVALID)
        return;
    if(mRecalculate)
    {
        RecalculateLayout();
        mRecalculate = false;
    }
    if(!mUpdateBuffers)
        return;
    mUpdateBuffers = false;

    HEAPCHECK;
    static double timeSinceLastFPSUpdate = 0;
    static int    framesSinceLastFPSUpdate = 0;
    static double avgFPS = 0;
    static CPUT_RECT windowRect = {0, 0, 0, 0};
	static int lastControlsInPanel = 0;

	int controlsInPanel = GetNumberOfControlsInPanel();
    if( 0 == controlsInPanel && lastControlsInPanel == controlsInPanel)
    {
		lastControlsInPanel = controlsInPanel;
        return;
    }
	lastControlsInPanel = controlsInPanel;


    // check and see if any of the controls resized themselves
    int ControlCount=GetNumberOfControlsInPanel();

    // 'clear' the buffer by resetting the pointer to the head
    mUberBufferIndex = 0;

    int ii=0;
    while(ii<ControlCount)
    {
        CPUTControl *pControl = mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[ii];

        // don't draw the focus control - draw it last so it stays on 'top'
        if(mpFocusControl != pControl)
        {
            pControl->Draw(mpMirrorBuffer, &mUberBufferIndex, mBufferSize);                    
        }
        ii++;
        HEAPCHECK
    }

    // do the 'focused' control last so it stays on top (i.e. dropdowns)
    if(mpFocusControl)
    {
        mpFocusControl->Draw(mpMirrorBuffer, &mUberBufferIndex, mBufferSize);
    }
        
    //API Specific
    UpdateUberBuffers();
    HEAPCHECK
    return;
}

void CPUTGuiController::ControlModified()
{
    mRecalculate = true;
}

void CPUTGuiController::RecalculateLayout()
{
    if (mActiveControlPanelSlotID == CPUT_CONTROL_ID_INVALID)
        return;

    mUpdateBuffers = true;
	// Build columns of controls right to left
    int x,y;
    x=0; y=0;

    // walk list of controls, counting up their *heights*, until the
    // column is full.  While counting, keep track of the *widest*
    int width, height;
    const int GUI_WINDOW_PADDING = 5;

    int numberOfControls = (int) mControlPanelIDList[mActiveControlPanelSlotID]->mControlList.size();
    int indexStart=0;
    int indexEnd=0;
    int columnX = 0;
    int columnNumber = 1;
    while(indexEnd < numberOfControls)
    {
        int columnWidth=0;
        y=0;
        // figure out which controls belong in this column + column width
        while( indexEnd < numberOfControls )
        {
            if(mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[indexEnd]->IsVisible() &&
                mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[indexEnd]->IsAutoArranged())
            {
                mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[indexEnd]->GetDimensions(width, height);
                if( y + height + GUI_WINDOW_PADDING < (mHeight-2*GUI_WINDOW_PADDING))
                {
                    y = y + height + GUI_WINDOW_PADDING;
                    if(columnWidth < width)
                    {
                        columnWidth = width;
                    }
                    indexEnd++;
                }
                else
                {
                    // if the window is now so small it won't fit a whole control, just
                    // draw one anyway and it'll just have to be clipped
                    if(indexEnd == indexStart)
                    {
                        columnWidth = width;
                        indexEnd++;
                    }
                    break;
                }
            }
            else
            {
                indexEnd++;
            }
        }
        

        // ok, now re-position each control with x at widest, and y at proper height
        y=GUI_WINDOW_PADDING;
        for(int i=indexStart; i<indexEnd; i++)
        {
            if(mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[i]->IsVisible() &&
                mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[i]->IsAutoArranged())
            {
                mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[i]->GetDimensions(width, height);
                x = mWidth - columnX - columnWidth - (columnNumber*GUI_WINDOW_PADDING);
                mControlPanelIDList[mActiveControlPanelSlotID]->mControlList[i]->SetPosition(x,y);
				y = y + height + GUI_WINDOW_PADDING;
            }
        }
        indexStart = indexEnd;
        columnX+=columnWidth;
        columnNumber++;
    }
}

void CPUTGuiController::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
   
    mRecalculate = true;
    mUpdateBuffers = true;
    UpdateConstantBuffer();
}
