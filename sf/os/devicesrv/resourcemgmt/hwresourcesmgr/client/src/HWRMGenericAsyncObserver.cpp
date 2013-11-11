// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "HWRMGenericAsyncObserver.h"
#include "HWRMTrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMGenericAsyncObserver::CHWRMGenericAsyncObserver
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMGenericAsyncObserver::CHWRMGenericAsyncObserver(RHWRMClient& aClient, 
                                                     TInt aCommand,
                                                     MHWRMGenericAsyncObserver* aCallback)
    : CActive(EPriorityStandard), 
      iCallback(aCallback),
      iCommand(aCommand),
      iClient(aClient)
    {
    COMPONENT_TRACE3(_L( "HWRM Client - CHWRMGenericAsyncObserver::CHWRMGenericAsyncObserver(0x%x, 0x%x, 0x%x)" ), &aClient, aCommand, aCallback);

    CActiveScheduler::Add(this);

    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::CHWRMGenericAsyncObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMGenericAsyncObserver::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMGenericAsyncObserver* CHWRMGenericAsyncObserver::NewL(RHWRMClient& aClient, 
                                                           TInt aCommand,
                                                           TInt aInitialCommand,
                                                           MHWRMGenericAsyncObserver* aCallback)
    {

    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::NewLC()" ) );

    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }

	CHWRMGenericAsyncObserver* newInstance = new (ELeave) CHWRMGenericAsyncObserver(aClient, aCommand, aCallback);

	CleanupStack::PushL( newInstance );
	
	newInstance->ConstructL(aInitialCommand);

	CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Client - CHWRMGenericAsyncObserver::NewLC - return 0x%x" ), newInstance );

	return newInstance;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMGenericAsyncObserver::~CHWRMGenericAsyncObserver()
    {
    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::~CHWRMGenericAsyncObserver()" ));

    Cancel();
    
    iCallback = NULL; // PCLint demands

    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::~CHWRMGenericAsyncObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMGenericAsyncObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMGenericAsyncObserver::ConstructL(TInt aInitialCommand)
    {
    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::ConstructL()" ));

    // Order notification
    OrderNotification();

    // Get initial value
    TPtr8 ptr( reinterpret_cast<TUint8*>(&iLatestStatus), sizeof(iLatestStatus), sizeof(iLatestStatus) );
    User::LeaveIfError(iClient.ExecuteOperation(aInitialCommand, TIpcArgs(&ptr)));

    if ( iCallback )
        {
        iCallback->AsyncRequestCompleted(iCommand, iLatestStatus);    
        }
    
    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::ConstructL - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMGenericAsyncObserver::RunL
// Handle notification from server.
// No need for RunError, as RunL will never leave
// -----------------------------------------------------------------------------
//
void CHWRMGenericAsyncObserver::RunL()
    {
    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::RunL()" ));
    
    
    // Store value
    iLatestStatus = iStatus.Int();

    // Reorder notification
    OrderNotification();

    if ( iCallback )
        {
        iCallback->AsyncRequestCompleted(iCommand, iLatestStatus);    
        }

    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::RunL - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMGenericAsyncObserver::DoCancel
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CHWRMGenericAsyncObserver::DoCancel()
    {
    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::DoCancel()" ));
    
    // Nothing to do

    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::DoCancel - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMGenericAsyncObserver::OrderNotification
// Order new notification from CenRep.
// -----------------------------------------------------------------------------
//
void CHWRMGenericAsyncObserver::OrderNotification()
    {
    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::OrderNotification()" ));

    iClient.ExecuteAsyncOperation( iCommand, TIpcArgs(), iStatus );

    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::OrderNotification - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMGenericAsyncObserver::LatestStatus
// Return vibra status
// -----------------------------------------------------------------------------
//
TInt CHWRMGenericAsyncObserver::LatestStatus() const
    {
    COMPONENT_TRACE1(_L( "HWRM Client - CHWRMGenericAsyncObserver::LatestStatus()" ));
    
    COMPONENT_TRACE2(_L( "HWRM Client - CHWRMGenericAsyncObserver::LatestStatus - return 0x%x" ), iLatestStatus );
    
    return iLatestStatus;
    }
    

// End of File
