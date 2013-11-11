/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Vibra control API implementation that uses HWRM vibra API
*                instead of DOS.
*
*/


// INCLUDE FILES
#include "vibractrlwrapperimpl.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVibraControlImpl::CVibraControlImpl
// Constructor with notify handling.
// -----------------------------------------------------------------------------
//
CVibraControlImpl::CVibraControlImpl(MVibraControlObserver* aCallback)
    :iCallback(aCallback),
    iVibra(NULL)
    {
    //...
    }

// -----------------------------------------------------------------------------
// CVibraControlImpl::~CVibraControlImpl
// Destructor
// -----------------------------------------------------------------------------
//
CVibraControlImpl::~CVibraControlImpl()
    {
    delete iVibra;
    iVibra = NULL;
    
    // PCLint demands
    iCallback = NULL;
    }

// -----------------------------------------------------------------------------
// CVibraControlImpl::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CVibraControlImpl::ConstructL()
    {
    // Ignore status change notifications caused by initialization
    // to make functionality similar to old vibra.
    iVibraRouteEvents = EFalse;

    // To get access vibra thru DOS
    iVibra = CHWRMVibra::NewL(this);
    
    iVibraRouteEvents = ETrue;
    }

    
// -----------------------------------------------------------------------------
// CVibraControlImpl::HandleErrorL
// Handles error from HWRM Vibra API
// -----------------------------------------------------------------------------
void CVibraControlImpl::HandleErrorL(TInt aError)
    {
    if ( aError != KErrNone )
        {
        if ( aError == KErrAccessDenied )
            {
            NotifyRequestStatus(EVibraRequestNotAllowed);
            }
        else 
            {
            NotifyRequestStatus(EVibraRequestFail);
            if ( aError == KErrArgument || aError == KErrNotSupported )
                {
                User::Leave(aError);
                }
            else
                {
                User::Leave(KErrGeneral);
                }
            }        
        }
    else
        {
        NotifyRequestStatus(EVibraRequestOK);
        iExpectingStop = ETrue;  // Expecting vibra to be stopped by HWRM timer soon.
        }
    }

// -----------------------------------------------------------------------------
// CVibraControlImpl::StartVibraL:
// Route call to HWRM Vibra API
// -----------------------------------------------------------------------------
EXPORT_C void CVibraControlImpl::StartVibraL(TUint16 aDuration, TInt aIntensity)
    {
    if ( aIntensity != 0 )
        {        
        TRAPD(err, iVibra->StartVibraL(aDuration, aIntensity));
        HandleErrorL(err);
        }
    else
        {
        StopVibraL();
        }    
    }

// -----------------------------------------------------------------------------
// CVibraControlImpl::StartVibraL:
// Route call to HWRM Vibra API
// -----------------------------------------------------------------------------
EXPORT_C void CVibraControlImpl::StartVibraL(TUint16 aDuration)
    {
    TRAPD(err, iVibra->StartVibraL(aDuration));
    
    HandleErrorL(err);
    }

// -----------------------------------------------------------------------------
// CVibraControlImpl::StopVibra
// Vibra is stopped and timer supervising vibra is cancelled.
// -----------------------------------------------------------------------------
EXPORT_C void CVibraControlImpl::StopVibraL()
    {
    TRAPD(err, iVibra->StopVibraL());

    if ( err != KErrNone )
        {
        NotifyRequestStatus(EVibraRequestUnableToStop);
        User::Leave(err);
        }
    else
        {
        NotifyRequestStatus(EVibraRequestStopped);
        }   
    }

// -----------------------------------------------------------------------------
// CVibraControlImpl::VibraSettings
// Mode of the vibra is returned (from user profiles)
// -----------------------------------------------------------------------------
EXPORT_C CVibraControl::TVibraModeState CVibraControlImpl::VibraSettings() const
    {
    CHWRMVibra::TVibraModeState hwrmState = iVibra->VibraSettings();
    
    CVibraControl::TVibraModeState retval;
    
    switch (hwrmState)
        {
        case CHWRMVibra::EVibraModeON:
            {
            retval = CVibraControl::EVibraModeON;
            break;
            }
        case CHWRMVibra::EVibraModeOFF:
            {
            retval = CVibraControl::EVibraModeOFF;
            break;
            }
        default:
            {
            retval = CVibraControl::EVibraModeUnknown;
            break;
            }
        }
 
    return retval;
    }

// -----------------------------------------------------------------------------
// CVibraControlImpl::NotifyRequestStatus
// VibraRequestStatus callback method is called if there is observer who is
// listening it.
// -----------------------------------------------------------------------------
void CVibraControlImpl::NotifyRequestStatus(TVibraRequestStatus aStatus)
    {
    if (iCallback && iVibraRouteEvents  )
        {
        iCallback->VibraRequestStatus(aStatus);
        }
    }

// -----------------------------------------------------------------------------
// CVibraControlImpl::VibraModeChanged
// Handler for events from HWRM Vibra API
// -----------------------------------------------------------------------------
void CVibraControlImpl::VibraModeChanged(CHWRMVibra::TVibraModeState aStatus)
    {
    if ( iCallback && iVibraRouteEvents )
        {        
        if ( aStatus == CHWRMVibra::EVibraModeON )
            {
            iCallback->VibraModeStatus(CVibraControl::EVibraModeON);
            }
        else if ( aStatus == CHWRMVibra::EVibraModeOFF )
            {
            iCallback->VibraModeStatus(CVibraControl::EVibraModeOFF);
            }
        else
            {
            iCallback->VibraModeStatus(CVibraControl::EVibraModeUnknown);
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CVibraControlImpl::VibraStatusChanged
// Handler for events from HWRM Vibra API
// -----------------------------------------------------------------------------
void CVibraControlImpl::VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus)
    {
    // Only notify stop if expecting one. This is required to handle
    // stop generated automatically by HWRM vibra timer. All other
    // status notifications are either already handled in initial call or 
    // relate to some other vibra client.
    if ( iExpectingStop && aStatus == CHWRMVibra::EVibraStatusStopped )
        {
        iExpectingStop = EFalse;
        NotifyRequestStatus(EVibraRequestStopped);
        }
    }

//  End of File  
