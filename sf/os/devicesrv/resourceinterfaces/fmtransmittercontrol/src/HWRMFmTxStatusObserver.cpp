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



// INCLUDE FILES
#include <hwrm/hwrmfmtx.h>
#include "HWRMFmTxInternalPSKeys.h"
#include "HWRMFmTxStatusObserver.h"
#include "HWRMtrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmTxStatusObserver::CHWRMFmTxStatusObserver
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMFmTxStatusObserver::CHWRMFmTxStatusObserver(MHWRMFmTxObserver* aCallback)
    : CActive(EPriorityStandard), iCallback(aCallback)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::CHWRMFmTxStatusObserver(0x%x)" ), aCallback);

    CActiveScheduler::Add(this);

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::CHWRMFmTxStatusObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStatusObserver::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxStatusObserver* CHWRMFmTxStatusObserver::NewL(MHWRMFmTxObserver* aCallback)
    {

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::NewLC(0x%x)" ), aCallback);

	CHWRMFmTxStatusObserver* newInstance = new (ELeave) CHWRMFmTxStatusObserver(aCallback);

	CleanupStack::PushL( newInstance );
	
	newInstance->ConstructL();

	CleanupStack::Pop( newInstance );

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::NewLC - return 0x%x" ), newInstance );

	return newInstance;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxStatusObserver::~CHWRMFmTxStatusObserver()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::~CHWRMFmTxStatusObserver()" ));

    // Cancel property separately, as the following Cancel() call doesn't seem to 
    // cause DoCancel to fire.
    iProperty.Cancel();

    Cancel();
    
    iProperty.Close();
    
    iCallback = NULL; // PCLint demands

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::~CHWRMFmTxStatusObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStatusObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxStatusObserver::ConstructL()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::ConstructL()" ));

    User::LeaveIfError(iProperty.Attach(KPSUidHWRMFmTx, KHWRMFmTxStatus));
    
    // Call RunL to order notification and do the initial notifying of the client
    RunL();

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::ConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStatusObserver::RunL
// Handle notification from P&S session.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxStatusObserver::RunL()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::RunL()" ));

    // Reorder notification
    OrderNotification();

    // get value from P&S
    TInt value(EFmTxStateUnknown);
    
    TInt err = iProperty.Get(value);
    
    // if no error or not found then no problem.
    // Otherwise leave as something went wrong
    if ( err == KErrNotFound || err == KErrNone )
        {
        TFmTxState state = static_cast<TFmTxState>(value);

        // Only notify if value actually changed
        if ( iCallback && iFmTxState != state)
            {            
            iCallback->FmTxStatusChanged(state);    
            }
        iFmTxState = state;
        }
    else
        {
        User::Leave(err);
        }

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::RunL - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxStatusObserver::RunError
// Handle error in RunL
// -----------------------------------------------------------------------------
//
#if defined(_DEBUG) && defined(COMPONENT_TRACE_FLAG)
TInt CHWRMFmTxStatusObserver::RunError(TInt aError)
#else
TInt CHWRMFmTxStatusObserver::RunError(TInt /*aError*/)
#endif
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::RunError(%d)" ), aError);

    // Error occurred either while reordering notification or getting 
    // property value. Set status accordingly
    
    if ( iCallback && iFmTxState != EFmTxStateUnknown)
        {        
        iCallback->FmTxStatusChanged(EFmTxStateUnknown);
        }
    
    iFmTxState = EFmTxStateUnknown;
    
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::RunError - return %d" ), KErrNone );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStatusObserver::DoCancel
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxStatusObserver::DoCancel()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::DoCancel()" ));

    iProperty.Cancel();

    COMPONENT_TRACE1( _L( "HWRM FmTxClient - CHWRMFmTxStatusObserver::DoCancel - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStatusObserver::OrderNotification
// Order new notification from CenRep.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxStatusObserver::OrderNotification()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::OrderNotification()" ));

    iProperty.Subscribe(iStatus);
    SetActive();

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::OrderNotification - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMFmTxStatusObserver::Status
// Return FM Tx status
// -----------------------------------------------------------------------------
//
TFmTxState CHWRMFmTxStatusObserver::Status()
    {    
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxStatusObserver::Status - return 0x%x" ), iFmTxState );
    
    return iFmTxState;
    }
    

// End of File
