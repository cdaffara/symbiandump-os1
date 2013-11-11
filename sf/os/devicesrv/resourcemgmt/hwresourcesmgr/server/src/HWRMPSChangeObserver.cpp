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
#include "HWRMtrace.h"
#include "HWRMPSChangeObserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMPSChangeObserver::CHWRMPSChangeObserver
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMPSChangeObserver::CHWRMPSChangeObserver(MHWRMPSChangeObserver& aCallback,
                                             const TUid& aUid,
                                             TUint32 aKey)
    : CActive(EPriorityStandard), 
      iCallback(aCallback),
      iUid(aUid),
      iKey(aKey)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPSChangeObserver::CHWRMPSChangeObserver(0x%x)" ), &aCallback);

    CActiveScheduler::Add(this);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::CHWRMPSChangeObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPSChangeObserver::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CHWRMPSChangeObserver* CHWRMPSChangeObserver::NewL(MHWRMPSChangeObserver& aCallback,
                                                   const TUid& aUid,
                                                   TUint32 aKey)
    {

    CHWRMPSChangeObserver* newInstance = NewLC(aCallback, aUid, aKey);
	CleanupStack::Pop();

	return newInstance;
    }


// -----------------------------------------------------------------------------
// CHWRMPSChangeObserver::NewLC
// Two-phased constructor. Leaves instance to cleanup stack
// -----------------------------------------------------------------------------
//
CHWRMPSChangeObserver* CHWRMPSChangeObserver::NewLC(MHWRMPSChangeObserver& aCallback,
                                                    const TUid& aUid,
                                                    TUint32 aKey)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPSChangeObserver::NewLC(0x%x)" ), &aCallback);

	CHWRMPSChangeObserver* newInstance = new (ELeave) CHWRMPSChangeObserver(aCallback, aUid, aKey);
	CleanupStack::PushL( newInstance );
	
	newInstance->ConstructL();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPSChangeObserver::NewLC - return 0x%x" ), newInstance );

	return newInstance;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMPSChangeObserver::~CHWRMPSChangeObserver()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::~CHWRMPSChangeObserver()" ));

    // Cancel property separately, as the following Cancel() call doesn't seem to 
    // cause DoCancel to fire.
    iProperty.Cancel();

    Cancel();
    
    iProperty.Close();

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::~CHWRMPSChangeObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPSChangeObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMPSChangeObserver::ConstructL()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::ConstructL()" ));

    User::LeaveIfError(iProperty.Attach(iUid, iKey));
    
    // Call RunL to order notification and do the initial notifying of the client
    RunL();

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::ConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPSChangeObserver::RunL
// Handle notification from pubsub session.
// -----------------------------------------------------------------------------
//
void CHWRMPSChangeObserver::RunL()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::RunL()" ));

    // Reorder notification
    OrderNotification();

    // get value from pubsub
    TInt value(0);
    
    TInt err = iProperty.Get(value);
    
    // if no error or not found then no problem.
    // Otherwise leave as something went wrong
    if ( err == KErrNotFound || err == KErrNone )
        {
        iCallback.PSValueChanged(iUid, iKey, value);    
        }
    else
        {
        COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPSChangeObserver::RunL(), err %d" ), err);
        User::Leave(err);
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::RunL - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMPSChangeObserver::RunError
// Handle error in RunL
// -----------------------------------------------------------------------------
//
#if defined(_DEBUG) && defined(COMPONENT_TRACE_FLAG)
TInt CHWRMPSChangeObserver::RunError(TInt aError)
#else
TInt CHWRMPSChangeObserver::RunError(TInt /*aError*/)
#endif
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPSChangeObserver::RunError(%d)" ), aError);

    // Error occurred either while reordering notification or getting property value.
    // We can't really do anything about it, so ignore.
    // (possible errors are KErrPermissionDenied and KErrArgument, which indicate fundamental
    //  problem with property, and will be caught on the first RunL execution by ConstructL)

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPSChangeObserver::RunError - return %d" ), KErrNone );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHWRMPSChangeObserver::DoCancel
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CHWRMPSChangeObserver::DoCancel()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::DoCancel()" ));

    iProperty.Cancel();

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::DoCancel - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPSChangeObserver::OrderNotification
// Order new notification from CenRep.
// -----------------------------------------------------------------------------
//
void CHWRMPSChangeObserver::OrderNotification()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::OrderNotification()" ));

	if (IsActive())
		{
		COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::OrderNotification - returning early as already active" ) );	
		return;
		}
		
    iProperty.Subscribe(iStatus);
    SetActive();

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPSChangeObserver::OrderNotification - return" ) );
    }



// End of File
