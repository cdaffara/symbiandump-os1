/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Notify service class about connection state.
*
*/

#include "cautoaudioasypropertyobserver.h" 
#include "autoaudiopskeys.h"
#include "mautoaudioasypropertylistener.h"

#include "acc_debug.h"

// ---------------------------------------------------------------------------
// CAutoAudioAsyPropertyObserver::NewL
// ---------------------------------------------------------------------------
CAutoAudioAsyPropertyObserver* CAutoAudioAsyPropertyObserver::NewL ( TUid aCategory, TUint aKey,
        MAutoAudioAsyPropertyListener& aListener )
	{
    COM_TRACE_1 ( "[AccFW:AutoAudioAsy] CAutoAudioAsyPropertyObserver::NewL(), key- %d", aKey );
    return new ( ELeave ) CAutoAudioAsyPropertyObserver ( aCategory, aKey, aListener );
	}

// ---------------------------------------------------------------------------
// CAutoAudioAsyPropertyObserver::CAutoAudioAsyPropertyObserver
// ---------------------------------------------------------------------------
CAutoAudioAsyPropertyObserver::CAutoAudioAsyPropertyObserver ( TUid aCategory, TUint aKey,
        MAutoAudioAsyPropertyListener& aListener ):
    CActive ( EPriorityStandard ),
    iCategory ( aCategory ), iKey ( aKey ), iListener ( aListener )
    {
    CActiveScheduler::Add ( this );    
    iProperty.Attach ( iCategory, iKey );
    iProperty.Subscribe ( iStatus );
    SetActive ();
    }

// ---------------------------------------------------------------------------
// CAutoAudioAsyPropertyObserver::~CAutoAudioAsyPropertyObserver
// ---------------------------------------------------------------------------
CAutoAudioAsyPropertyObserver::~CAutoAudioAsyPropertyObserver ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyPropertyObserver::~CAutoAudioAsyPropertyObserver");
    Cancel ();
    iProperty.Close ();
    }

// ---------------------------------------------------------------------------
// CAutoAudioAsyPropertyObserver::RunL
// ---------------------------------------------------------------------------
void CAutoAudioAsyPropertyObserver::RunL ()
    {                      
    COM_TRACE_ ( "AccFW:AutoAudioAsy] CAutoAudioAsyPropertyObserver::RunL");

    TInt status = iStatus.Int ();
    COM_TRACE_3 ( "[AccFW:AutoAudioAsy] key 0x%08x/%u changed, status %d", iCategory.iUid, iKey, status );
    // Subscribe for the next change BEFORE invoking the listener to make sure
    // that we never miss any property changes.
    iProperty.Subscribe ( iStatus );
    SetActive ();

    // Notify the listener
    iListener.PropertyChange ( iProperty, iCategory, iKey, status );
    }

// ---------------------------------------------------------------------------
// CAutoAudioAsyPropertyObserver::DoCancel
// ---------------------------------------------------------------------------
void CAutoAudioAsyPropertyObserver::DoCancel ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyPropertyObserver::DoCancel" );
    iProperty.Cancel ();
    }    

// ---------------------------------------------------------------------------
// CAutoAudioAsyPropertyObserver::RunError
// ---------------------------------------------------------------------------
TInt CAutoAudioAsyPropertyObserver::RunError ( TInt /* aError */ )
	{
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyPropertyObserver::RunError" );
    return KErrNone;
	}
