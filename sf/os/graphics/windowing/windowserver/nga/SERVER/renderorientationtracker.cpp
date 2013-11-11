// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This material, including documentation and any related
// computer programs, is protected by copyright controlled by
// Nokia. All rights are reserved. Copying, including
// reproducing, storing, adapting or translating, any
// or all of this material requires the prior written consent of
// Nokia. This material also contains confidential
// information which may not be disclosed to others without the
// prior written consent of Nokia.
//
// Description:
// Render Orientation Tracking and Publication
// 

#include <hal.h>
#include <e32std.h>
#include "renderorientationtracker.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "wstop.h"
#include "..\debuglog\DEBUGLOG.H"

extern CDebugLogBase* wsDebugLog;

/** Convert a TRenderOrientation value into a TDigitiserOrientation.
Note: The algorithm used makes use of the ordering of the values of the respective enums, 
thus this is checked for (at compile time) at the start of the function.
@param aWservOrientation A value from the TRenderOrientation enums.
@return The equivalent value from the TDigitiserOrientation enums.
*/
inline HALData::TDigitiserOrientation WservToDigitiser(TRenderOrientation aWservOrientation)
	{
	__ASSERT_COMPILE(EDisplayOrientationNormal+1 == EDisplayOrientation90CW);
	__ASSERT_COMPILE(EDisplayOrientationNormal+2 == EDisplayOrientation180);
	__ASSERT_COMPILE(EDisplayOrientationNormal+3 == EDisplayOrientation270CW);
	__ASSERT_COMPILE(HALData::EDigitiserOrientation_000+1 == HALData::EDigitiserOrientation_090);
	__ASSERT_COMPILE(HALData::EDigitiserOrientation_000+2 == HALData::EDigitiserOrientation_180);
	__ASSERT_COMPILE(HALData::EDigitiserOrientation_000+3 == HALData::EDigitiserOrientation_270);
	HALData::TDigitiserOrientation ret=static_cast<HALData::TDigitiserOrientation>
			(HALData::EDigitiserOrientation_000 + (aWservOrientation - EDisplayOrientationNormal));
	return ret;
	}

// Todo remove/undefine this for release
#define TECHVIEW_TESTMODE

CWsRenderOrienationTracker* CWsRenderOrienationTracker::NewL()
    {
    CWsRenderOrienationTracker* self = new(ELeave)CWsRenderOrienationTracker();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CWsRenderOrienationTracker::CWsRenderOrienationTracker()
    : CActive(CActive::EPriorityStandard),
      iRenderOrientationTrackingType(EDisplayOrientationNormal),
      iPublishedRenderOrientation(EDisplayOrientationNormal)
    {
    CActiveScheduler::Add(this);    
    }

void CWsRenderOrienationTracker::ConstructL()
    {    
    const TSecurityPolicy   KRenderOrientationReadSecurityPolicy(ECapability_None);
    const TSecurityPolicy   KRenderOrientationWriteSecurityPolicy(ECapabilityWriteDeviceData);
    
    // Define P&S Property to publish to
    TInt error = RProperty::Define( KRenderOrientationCategory,
                                    KRenderOrientationKey,
                                    RProperty::EInt,
                                    KRenderOrientationReadSecurityPolicy,
                                    KRenderOrientationWriteSecurityPolicy);

    // Attach the publisher for real-time publishing
    if(KErrNone == error)
        error = iRenderOrientationPublisher.Attach( KRenderOrientationCategory,
                                                    KRenderOrientationKey);

    // Publish the initial value
    if(KErrNone == error)    
        error = DoPublishOrientation(EDisplayOrientationNormal);
    
    //Set the initial value to HAL
    if(KErrNone == error)
        SetHALOrientation(EDisplayOrientationNormal);
    
    if (wsDebugLog && KErrNone!=error)
        {
        _LIT(logText,"Orientation Tracker: failed to initialise with error %d");
        wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,logText,error);
        }    
    User::LeaveIfError(error);
    }

CWsRenderOrienationTracker::~CWsRenderOrienationTracker()
    {
    Cancel();
    iRenderOrientationPublisher.Delete(KRenderOrientationCategory, KRenderOrientationKey);
    iRenderOrientationPublisher.Close();
    }

/**
If the orientation of the given window group is useable updates aOrientationTrackingType with the orientation

@param Input: the window group to check
@param Output: the window group's orientation if usable ( otherwise unchanged )
@return KErrNone if the orienation is usable, KErrNotFound if the orientation is not useable, KErrNotSupported if the orientation is unknown
*/
TInt CWsRenderOrienationTracker::CheckWindowGroupOrientation(const CWsWindowGroup& aWinGroup, TRenderOrientationTrackingType& aOrientationTrackingType)
    {
    TInt error = KErrNone;
    TRenderOrientationTrackingType tempOrientationTrackingType = static_cast<TRenderOrientationTrackingType>(aWinGroup.WsOwner()->GetIndicatedAppOrientation());
    switch(tempOrientationTrackingType)
        {
        case EDisplayOrientationNormal:
        case EDisplayOrientation90CW:                
        case EDisplayOrientation180:
        case EDisplayOrientation270CW:            
        case EDisplayOrientationAuto:
            aOrientationTrackingType = tempOrientationTrackingType;
            break;

        case EDisplayOrientationIgnore:
            error = KErrNotFound;
            if (wsDebugLog)
                {
                _LIT(logText,"Orientation Tracker: winGroup %08x orientation is set to be ignored");
                wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,logText,reinterpret_cast<TInt>(&aWinGroup));
                }            
            break;

        default:
            error = KErrNotSupported;
            if (wsDebugLog)
                {
                _LIT(logText,"Orientation Tracker: winGroup %08x has undefined orientation, Error %d");                
                TBuf<LogTBufSize> buf;
                buf.Format(logText, &aWinGroup, error);                
                wsDebugLog->MiscMessage(CDebugLogBase::ELogIntermediate,buf);
                }                          
            break;             
        }
    
    return error;
    }

/**
Checks that the given group window is appropriate for dictating the render orientation

@param Input:  The group window to check
@return ETrue is the group window is usable, else EFalse  
*/
TBool CWsRenderOrienationTracker::UseableGroupWindow(const CWsWindowGroup& aWinGroup) const
    {
#ifdef TECHVIEW_TESTMODE
    // for some reason IsFocusable seems to return 0 and 2, not 0 and 1
    return NULL!=aWinGroup.Child() &&
            (aWinGroup.IsFocusable() ? ETrue : EFalse);
#else    
    return (NULL!=aWinGroup.Child());     
#endif
    }

/**
Finds the topmost usable windowgroup which has a usable orientation, and outputs that orientation

@param Output: The current render orientation
@return KErrNone if successful, KErrNotFound if the focus window group is not usable, KErrNotSupported if an invalid orientation is found
*/
TInt CWsRenderOrienationTracker::GetFocusWindowOrientation(TRenderOrientationTrackingType& aOrientationTrackingType)
    {
    TInt error = KErrNone;
    CWsWindowGroup* focusWinGroup = CWsTop::FocusWindowGroup();    
    if(!focusWinGroup)
        {
        if(wsDebugLog)
            {
            _LIT(logText,"Orientation Tracker: focusWinGroup not found");
            wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,logText);
            }
        error = KErrNotFound;
        }
    else
        {
        error = CheckWindowGroupOrientation(*focusWinGroup, aOrientationTrackingType);
        }
    return error;
    }

/**
Finds the topmost usable windowgroup which has a usable orientation, and outputs that orientation

@param Output: The current render orientation
@return KErrNone if successful, KErrNotSupported if an invalid orientation is found
*/
TInt CWsRenderOrienationTracker::FindOrientationFromWindowTree(TRenderOrientationTrackingType& aOrientationTrackingType)
    {
    TInt error = KErrNone;
    TRenderOrientationTrackingType tempOrientationTrackingType = iRenderOrientationTrackingType;
    CWsRootWindow* rootWin = CWsTop::CurrentFocusScreen()->RootWindow();
    TBool finished = EFalse;
    for(CWsWindowGroup* winGroup = rootWin->Child(); !finished && NULL != winGroup; winGroup = winGroup->NextSibling())
        {
        if (wsDebugLog)
            {
            _LIT(logText,"Orientation Tracker: winGroup %08x has priority %d, Orientation %d, Focusable %d, Child %08x");
            TBuf<LogTBufSize> buf;
            buf.Format(logText, winGroup, winGroup->OrdinalPriority(), winGroup->WsOwner()->GetIndicatedAppOrientation(),
                    winGroup->IsFocusable()?ETrue:EFalse, winGroup->Child());                                
            wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,buf);
            }               
        // winGroup is a higher priority ordinal, so see if it has an orientation that can be used
        // although we're only interested in window groups with child windows otherwise nothing is visible anyway        
        if(UseableGroupWindow(*winGroup))
            {
            error = CheckWindowGroupOrientation(*winGroup, tempOrientationTrackingType);
            switch(error)
                {
                case KErrNone:
                    {
                    // List is in order, so just find the first one
                    if (wsDebugLog)
                        {
                        _LIT(logText,"Orientation Tracker: Found winGroup %08x with Orientation %d");
                        TBuf<LogTBufSize> buf;
                        buf.Format(logText, winGroup, winGroup->WsOwner()->GetIndicatedAppOrientation());                    
                        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,buf);
                        }
                    finished = ETrue;
                    break;
                    }

                case KErrNotFound:
                    // so keep searching
                    break;                    
                    
                case KErrNotSupported:
                default:
                    finished = ETrue;
                    break;
                    
                }
            
            }
        }
    // Safe even in error code as won't have been changed by CheckWindowGroupOrientation
    aOrientationTrackingType = tempOrientationTrackingType;
    
    return error;
    }

/**
First checks to see if the focus window group has a usable orientation, if so that is output.
Otherwise, finds the topmost usable windowgroup which has a usable orientation, and outputs that

@param Output: The current render orientation
@return KErrNone if successful, KErrNotSupported if an invalid orientation is found 
 */
TInt CWsRenderOrienationTracker::GetIndicatedOrientation(TRenderOrientationTrackingType& aOrientationTrackingType)
    {
    // First check the focus window group
    TInt error = GetFocusWindowOrientation(aOrientationTrackingType);

    // Don't look for another window if the focus window is usable
    // or if an error has occured, then don't change current orientation
    switch(error)
        {
        case KErrNone:
            {
            if(wsDebugLog)
                {
                _LIT(logText,"Orientation Tracker: Using focus window %08x for orientation");
                wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,logText,reinterpret_cast<TInt>(CWsTop::FocusWindowGroup()));
                }
            break;
            }
        
        case KErrNotFound:
            {
            // Can't use focus window group, so find the topmost windowgroup with a valid orientation
            error = FindOrientationFromWindowTree(aOrientationTrackingType);
            break;
            }
            
        default:
            // Unrecoverable error, abort and leave published orientation unchanged
            break;
        }
    
    return error;
    }

/**
Checks to see if the render orientation has changed, and publishes any new orientaion
via publish and subscribe

@see KRenderOrientationCategory
@see KRenderOrientationKey 
*/
void CWsRenderOrienationTracker::CheckRenderOrientation()
    {
    TRenderOrientationTrackingType newOrientationTrackingType = iRenderOrientationTrackingType;    
    TInt error = GetIndicatedOrientation(newOrientationTrackingType);

    // if the tracking type has changed...
    if(KErrNone == error && iRenderOrientationTrackingType != newOrientationTrackingType)
        {
        if(EDisplayOrientationAuto == iRenderOrientationTrackingType)
            {
            // change from auto type, so we need to cancel request for updates from the theme server        
            Cancel();
            }    
        iRenderOrientationTrackingType = newOrientationTrackingType;
        if(EDisplayOrientationAuto == iRenderOrientationTrackingType)
            {
            // Change to auto type, so we need to request updates from the theme server            
            // Attach to the Theme server to get orientation change updates
            error = iThemeOrientationProperty.Attach( KThemeOrientationCategory, KThemeOrientationKey );
            if (wsDebugLog)
                {
                if(KErrNone == error)
                    {
                    // Information Log
                    _LIT(logText,"Orientation Tracker: Attached to theme orientation property");
                    wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,logText);
                    }
                else
                    {
                    // Error Log
                    _LIT(logText,"Orientation Tracker: Error %d attaching to theme orientation property");
                    wsDebugLog->MiscMessage(CDebugLogBase::ELogIntermediate,logText, error);                
                    }
                }              
            
            RequestDeviceOrientationNotification();
            }
        // See if the  has changed, and publish if it has        
        error = DoOrientationTracking();
        }

    if (wsDebugLog && KErrNone != error)
        {
        _LIT(logText,"Orientation Tracker: Error %d Checking Render Orientation");
        wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,logText, error);
        }  
    }

/**
Requests notification of change of the theme server orientation

@Pre iThemeOrientationProperty has had Attach called on it
*/
void CWsRenderOrienationTracker::RequestDeviceOrientationNotification()
    {
    if(!IsActive())
        {
        if (wsDebugLog)
            {
            _LIT(logText,"Orientation Tracker: Subscribing to theme orientation property");
            wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,logText);
            }          
        // Request for Theme Server Orientation P&S  
        iThemeOrientationProperty.Subscribe(iStatus);        
        SetActive();
        }
    }

/**
Cancels and closes (detaches) from the theme orientation publish and subscribe
*/
void CWsRenderOrienationTracker::CancelDeviceOrientationNotification()
    {
    // Cancel Request for Theme Server Orientation P&S  
    iThemeOrientationProperty.Cancel();
    iThemeOrientationProperty.Close();
    
    if (wsDebugLog)
        {
        _LIT(logText,"Orientation Tracker: Cancelled/closed theme orientation property");
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,logText);
        }      
    }

/**
Called when the theme servers orientation has changed.
Re-requests unless cancelled
*/
void CWsRenderOrienationTracker::RunL()
    {
    TInt error = iStatus.Int();
    if(KErrNone == error)
        {
        // Re-request
        RequestDeviceOrientationNotification();
    
        TInt error = DoOrientationTracking();
        if (wsDebugLog && KErrNone != error)
            {
            _LIT(logText,"Orientation Tracker: Error %d processing theme orientation property");
            wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,logText, error);
            }         
        }
    else if (wsDebugLog && KErrCancel != error)
        {
        _LIT(logText,"Orientation Tracker: Error %d from theme orientation property, not resubscribed");
        wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,logText, error);     
        }
    }

/**
Cancels the request for notification for changes to theme orientation
*/
void CWsRenderOrienationTracker::DoCancel()
    {
    CancelDeviceOrientationNotification();
    }

/**
Gets the orientation published from theme server

@param Output: the theme server orientation
@return KErrNone if successful, KErrNotSupported if the theme server returns an unknown orientation, else any of the system wide error codes 
*/
TInt CWsRenderOrienationTracker::GetThemeOrientation(TRenderOrientation& aThemeOrientation)
    { 
    TInt themeOrientation=EDisplayOrientationNormal;
    TInt error = iThemeOrientationProperty.Get(themeOrientation);
    if(wsDebugLog && KErrNone != error)
        {
        _LIT(logText,"Orientation Tracker: Error %d getting theme orientation property");
        wsDebugLog->MiscMessage(CDebugLogBase::ELogIntermediate,logText, error);     
        }
    
    if(KErrNone == error)
        {
        // Translate the received orientation    
        switch(themeOrientation)
            {           
            case EDisplayOrientationNormal:
            case EDisplayOrientation90CW:
            case EDisplayOrientation180:
            case EDisplayOrientation270CW:
                // only update if orientation is supported
                aThemeOrientation = static_cast<TRenderOrientation>(themeOrientation);
                break;
            
            default:
                error = KErrNotSupported;
                if (wsDebugLog)
                    {
                    _LIT(logText,"Orientation Tracker: Unsupported orientation %d from theme orientation property, Error %d");
                    TBuf<LogTBufSize> buf;
                    buf.Format(logText, themeOrientation, error);
                    wsDebugLog->MiscMessage(CDebugLogBase::ELogIntermediate,buf);     
                    }                
                break;
            }
        }
    return error;  
    }

/**
Processes the indicated orientation into an actual orientation

@return KErrNone for success, KErrNotSupported if the orientation is unknown, else any of the system wide error codes
*/
TInt CWsRenderOrienationTracker::DoOrientationTracking()
    {
    TInt error = KErrNone;
    TRenderOrientation newDeviceOrientation;
    switch(iRenderOrientationTrackingType)
        {
        case EDisplayOrientationNormal:
        case EDisplayOrientation90CW:                
        case EDisplayOrientation180:
        case EDisplayOrientation270CW:            
            newDeviceOrientation = iRenderOrientationTrackingType;
            break;
            
        case EDisplayOrientationAuto:
            error = GetThemeOrientation(newDeviceOrientation);
            break;
                      
        default:
            error = KErrNotSupported;
            if (wsDebugLog)
                {
                _LIT(logText,"Orientation Tracker: Unsupported orientation tracking type %d, error %d");
                TBuf<LogTBufSize> buf;
                buf.Format(logText, iRenderOrientationTrackingType, error);
                wsDebugLog->MiscMessage(CDebugLogBase::ELogIntermediate,buf);     
                }              
            break;            
        }    

    if(KErrNone == error)
        {
        error = PublishOrientation(newDeviceOrientation);
        }
    
    return error;
    }

/**
Publishes the given value

@param The render orientation to publish
@return KErrNone for success, else any of the system wide erro codes
*/
TInt CWsRenderOrienationTracker::DoPublishOrientation(const TRenderOrientation aRenderOrientation)
    {
    TInt error = iRenderOrientationPublisher.Set(aRenderOrientation);
         
    // if it's published OK, then remember the newly published value
    if(KErrNone == error)
        {
        iPublishedRenderOrientation = aRenderOrientation;
        if(wsDebugLog)
            {
            _LIT(logText,"Orientation Tracker: Published render orientation %d");
            wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, logText, aRenderOrientation);
            }
        }
    else if(wsDebugLog)
        {
        _LIT(logText,"Orientation Tracker: Error %d setting render orientation property");
        wsDebugLog->MiscMessage(CDebugLogBase::ELogIntermediate, logText, error);                   
        }
    return error;
    }

void CWsRenderOrienationTracker::SetHALOrientation(const TRenderOrientation aRenderOrientation)
    {
    // If the render orientation is EDisplayOrientationAuto then don't update HAL
    // The application and HAL should always have the same state for the orientation.
    if(EDisplayOrientationAuto != aRenderOrientation)
        {
        TInt error = HAL::Set(CWsTop::CurrentFocusScreen()->ScreenNumber(), HALData::EDigitiserOrientation, WservToDigitiser(iPublishedRenderOrientation));
        //Just log the error if there is one.
        if(wsDebugLog && error != KErrNone)
            {
            _LIT(logText,"Orientation Tracker: Error %d setting digitiser orientation");
            wsDebugLog->MiscMessage(CDebugLogBase::ELogIntermediate, logText, error);           
            } 
        }
    }

/**
If the current orientation differs from the previously published value then publishes the current value

@param The render orientation to check and publish
@return KErrNone for success, else any of the system wide erro codes
*/
TInt CWsRenderOrienationTracker::PublishOrientation(const TRenderOrientation aRenderOrientation)
    {
    TInt error = KErrNone;
  
    if(aRenderOrientation != iPublishedRenderOrientation)
        {
        // If the device Orientation has changed, publish it
        error = DoPublishOrientation(aRenderOrientation);
        if(KErrNone == error)
            SetHALOrientation(aRenderOrientation);
        }
    return error;
    }

