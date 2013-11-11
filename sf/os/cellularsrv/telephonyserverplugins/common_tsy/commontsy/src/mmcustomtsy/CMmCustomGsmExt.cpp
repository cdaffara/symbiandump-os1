// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



//  INCLUDE FILES
#include "CMmCustomGsmExt.h"
#include "cmmcalltsy.h"
#include "cmmcalllist.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmCustomGsmExt::CMmCustomGsmExt( 
    CMmPhoneTsy* aMmPhoneTsy )
    {
    iMmPhoneTsy = aMmPhoneTsy;
    }

void CMmCustomGsmExt::ConstructL(
    CMmCustomTsy* aMmCustomTsy )
    {
    iMmCustomTsy = aMmCustomTsy;
    iRemoteAlertingToneStatus = RMmCustomAPI::EUiNoTone;
    }

CMmCustomGsmExt* CMmCustomGsmExt::NewL(
    CMmPhoneTsy* aMmPhoneTsy, 
    CMmCustomTsy* aMmCustomTsy )
    {
    CMmCustomGsmExt* customGsmExt = 
        new ( ELeave ) CMmCustomGsmExt( aMmPhoneTsy );

    CleanupStack::PushL( customGsmExt );
    customGsmExt->ConstructL( aMmCustomTsy );
    CleanupStack::Pop();

    return customGsmExt;
    }

CMmCustomGsmExt::~CMmCustomGsmExt()
    {
    }

// ---------------------------------------------------------------------------
// CMmCustomGsmExt::SetLocalRemoteAlertToneRequired
// Sets if the remote alerting tone should be played locally or not. EFalse 
// means that NW is playing the remote alert tone, ETrue means that remote 
// alert tone should be generated locally.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomGsmExt::SetLocalRemoteAlertToneRequired( 
    TBool aAlertingToneStatus )
    {
    RMmCustomAPI::TRemoteAlertingToneStatus remoteAlertingToneStatusTemp;
    remoteAlertingToneStatusTemp = iRemoteAlertingToneStatus;
    
    if ( !aAlertingToneStatus )
        {
        iRemoteAlertingToneStatus = RMmCustomAPI::EUiStopTone;
        }
    else
        {
        iRemoteAlertingToneStatus = RMmCustomAPI::EUiRbtTone;
        }
    
    if ( remoteAlertingToneStatusTemp != iRemoteAlertingToneStatus )
	    {
	    iMmCustomTsy->CompleteNotifyRemoteAlertingToneStatusChange( 
	        iRemoteAlertingToneStatus, KErrNone );
	    }
    
    }

// ---------------------------------------------------------------------------
// CMmCustomGsmExt::GetRemoteAlertingToneStatus
// Returns the status of the remote alerting tone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomGsmExt::GetRemoteAlertingToneStatus( 
    RMmCustomAPI::TRemoteAlertingToneStatus* aStatus )
    {
    *aStatus = iRemoteAlertingToneStatus;

    return KErrNone;
    }

// End of File
