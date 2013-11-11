/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CTVOutStubEventListener class.
*
*/

#include "tvoutconfigstatuscontrol.h"
#include "tvoutstubeventlistener.h"
#include "tvoutstubeventsender.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CTVOutStubEventListener::CTVOutStubEventListener( MTVOutStubEventSender& aSender )
  : CActive( EPriorityStandard ),
    iSender( aSender ), iPreviousValue(-1)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CTVOutStubEventListener::ConstructL( TUid aCategory, TUint aKey )
    {
    //RProperty::Define( KTVOutStatusControlCategory, KTVOutStatusControlCableConnect, RProperty::EInt );
    RProperty::Define( aCategory, aKey, RProperty::EInt );
    User::LeaveIfError( iProperty.Attach( aCategory, aKey ) );
    iCategory = aCategory;
    iKey = aKey;
    iProperty.Set( 0 );
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CTVOutStubEventListener* CTVOutStubEventListener::NewL(
        MTVOutStubEventSender& aSender,
        TUid aCategory,
        TUint aKey )
    {
    CTVOutStubEventListener* self = new( ELeave ) CTVOutStubEventListener( aSender );
    CleanupStack::PushL( self );
    self->ConstructL( aCategory, aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CTVOutStubEventListener::~CTVOutStubEventListener()
    {
    Cancel();

    iProperty.Close();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// ---------------------------------------------------------------------------
//
void CTVOutStubEventListener::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// ---------------------------------------------------------------------------
//
void CTVOutStubEventListener::RunL()
    {
    if ( iStatus == KErrNone )
        {
        TInt value;
        TInt errorCode = iProperty.Get( value );

        if ( errorCode == KErrNone )
            {
            	RProcess process;
            	
            	INFO_3("**EVENT received for 0x%x PREV=%d CURR=%d", (TUint32)process.SecureId(), iPreviousValue, value );
            if( (iKey == KTVOutStatusControlCableConnect) || (iKey == KTVOutStatusControlAnalogCableConnect) )
            	  {
            	  	if( iPreviousValue != value )
            	  		{
            				iSender.SendEvent( iCategory, iKey, value );
            			  }
            	  }
            else
            	{
            		iSender.SendEvent( iCategory, iKey, value );
            	}
            iPreviousValue = value;
            }
        }

    if ( iStatus != KErrCancel )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }
