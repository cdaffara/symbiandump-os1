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
#include "HWRMPrivatePSKeys.h"
#include "HWRMtrace.h"
#include "HWRMVibra.h"
#include "HWRMVibraStatusObserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMVibraStatusObserver::CHWRMVibraStatusObserver
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMVibraStatusObserver::CHWRMVibraStatusObserver(MHWRMVibraObserver* aCallback)
    : CActive(EPriorityStandard), iCallback(aCallback)
    {
    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::CHWRMVibraStatusObserver(0x%x)" ), aCallback);

    CActiveScheduler::Add(this);

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::CHWRMVibraStatusObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraStatusObserver::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CHWRMVibraStatusObserver* CHWRMVibraStatusObserver::NewL(MHWRMVibraObserver* aCallback)
    {

    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::NewLC(0x%x)" ), aCallback);

	CHWRMVibraStatusObserver* newInstance = new (ELeave) CHWRMVibraStatusObserver(aCallback);

	CleanupStack::PushL( newInstance );
	
	newInstance->ConstructL();

	CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::NewLC - return 0x%x" ), newInstance);

	return newInstance;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMVibraStatusObserver::~CHWRMVibraStatusObserver()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::~CHWRMVibraStatusObserver()" ));

    // Cancel property separately, as the following Cancel() call doesn't seem to 
    // cause DoCancel to fire.
    iProperty.Cancel();

    Cancel();
    
    iProperty.Close();
    
    iCallback = NULL; // PCLint demands

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::~CHWRMVibraStatusObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraStatusObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMVibraStatusObserver::ConstructL()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::ConstructL()" ));

    User::LeaveIfError(iProperty.Attach(KPSUidHWResourceNotification, KHWRMVibraStatus));
    
    // Call RunL to order notification and do the initial notifying of the client
    RunL();

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::ConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraStatusObserver::RunL
// Handle notification from pubsub session.
// -----------------------------------------------------------------------------
//
void CHWRMVibraStatusObserver::RunL()
    {
    // NOTE: RunL cannot currently leave, so it's not necessary to implement RunError. 
    // Remember to implement it if any change to RunL make it a leaving function.
    
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::RunL()" ));

	User::LeaveIfError(KErrNone); // avoid leavescan error
	
    // Reorder notification
    OrderNotification();

    // get value from pubsub
    TInt value(CHWRMVibra::EVibraStatusUnknown);
    
    TInt err = iProperty.Get(value);
    
    // if no error or not found then no problem.
    // Otherwise leave as something went wrong
    if ( err == KErrNotFound || err == KErrNone )
        {
        CHWRMVibra::TVibraStatus status = static_cast<CHWRMVibra::TVibraStatus>(value);
        
        // Only notify if value actually changed
        if ( iCallback && iVibraStatus != status)
            {            
            iCallback->VibraStatusChanged(status);    
            }
        iVibraStatus = status;
        }
    else
        {
        COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::RunL. Error (%d) in iProperty.Get()" ), err);
	    if ( iCallback && iVibraStatus != CHWRMVibra::EVibraStatusUnknown)
	        {        
	        iCallback->VibraStatusChanged(CHWRMVibra::EVibraStatusUnknown);
	        }
	    
	    iVibraStatus = CHWRMVibra::EVibraStatusUnknown;
        }

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::RunL - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraStatusObserver::DoCancel
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CHWRMVibraStatusObserver::DoCancel()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::DoCancel()" ));

    iProperty.Cancel();

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::DoCancel - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraStatusObserver::OrderNotification
// Order new notification from CenRep.
// -----------------------------------------------------------------------------
//
void CHWRMVibraStatusObserver::OrderNotification()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::OrderNotification()" ));

    iProperty.Subscribe(iStatus);
    SetActive();

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::OrderNotification - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMVibraStatusObserver::CurrentStatus
// Return vibra status
// -----------------------------------------------------------------------------
//
CHWRMVibra::TVibraStatus CHWRMVibraStatusObserver::CurrentStatus() const
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::CurrentStatus()" ));
    
    TInt value(CHWRMVibra::EVibraStatusUnknown);
    CHWRMVibra::TVibraStatus retval(CHWRMVibra::EVibraStatusUnknown);
    
    TInt err = RProperty::Get(KPSUidHWResourceNotification, KHWRMVibraStatus, value);
    
    if ( err != KErrNone )
        {
        // Perhaps a different status value other than the current one 
        // should be considered
        retval = iVibraStatus;
        }
    else
        {
        retval = static_cast<CHWRMVibra::TVibraStatus>(value);
        }
    
    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraStatusObserver::CurrentStatus - return 0x%x" ), retval );
    
    return retval;
    }
    

// End of File
