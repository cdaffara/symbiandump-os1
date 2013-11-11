// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CSatNotifySimSessonEnd.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Sim session end notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySimSessionEndTraces.h"
#endif

#include "CSatTsy.h"                    // Main tsy class header
#include "CSatNotifySimSessionEnd.h"    // Class header
#include "CSatNotificationsTsy.h"       // Tsy class header
#include "CSatDataPackage.h"            // Parameter packing 
#include "CSatTsyReqHandleStore.h"      // Request handle class
#include "CBerTlv.h"                    // Ber Tlv data handling
#include "TSatUtility.h"                // (U)SIM Status constants

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySimSessionEnd* CSatNotifySimSessionEnd::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSIMSESSIONEND_NEWL_1, "CSAT: CSatNotifySimSessionEnd::NewL");
   	CSatNotifySimSessionEnd* const satNotifySimSessionEnd = 
        new ( ELeave ) CSatNotifySimSessionEnd( aNotificationsTsy );
    CleanupStack::PushL( satNotifySimSessionEnd );
    satNotifySimSessionEnd->ConstructL();
    CleanupStack::Pop( satNotifySimSessionEnd );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSIMSESSIONEND_NEWL_2, "CSAT: CSatNotifySimSessionEnd::NewL, end of method");
    return satNotifySimSessionEnd;
    }

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::~CSatNotifySimSessionEnd
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySimSessionEnd::~CSatNotifySimSessionEnd
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSIMSESSIONEND_DTOR_1, "CSAT: CSatNotifySimSessionEnd::~CSatNotifySimSessionEnd");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::CSatNotifySimSessionEnd
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySimSessionEnd::CSatNotifySimSessionEnd
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySimSessionEnd::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSIMSESSIONEND_CONSTRUCTL_1, "CSAT: CSatNotifySimSessionEnd::ConstructL, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::Notify
// Save the request handle type for notification requested by ETel server
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySimSessionEnd::Notify
        ( 
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& /*aPackage*/   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSIMSESSIONEND_NOTIFY_1, "CSAT: CSatNotifySimSessionEnd::Notify");
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyProactiveSimSessionEndReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( 
        KEndOfTheProactiveSession );   

    return KErrNone; 
    }

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySimSessionEnd request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySimSessionEnd::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSIMSESSIONEND_CANCELNOTIFICATION_1, "CSAT: CSatNotifySimSessionEnd::CancelNotification");
    
    // Reset the request handle
    TTsyReqHandle reqHandle = 
        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifyProactiveSimSessionEndReqType );
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifySimSessionEnd::CompleteNotifyL
// Complete Sim Session End notification to the client
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySimSessionEnd::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage, 
        TInt aErrorCode  
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSIMSESSIONEND_COMPLETENOTIFYL_1, "CSAT: CSatNotifySimSessionEnd::CompleteNotifyL");
    
    // Unpack data
    TUint8 sw1;
    TUint8 sw2;
    aDataPackage->UnPackData( sw1, sw2 );
    if ( KSw1NormalEnding == sw1 && KSw2NormalEnding == sw2 )
        {
	    // Reset req handle. Returns the deleted req handle
	    TTsyReqHandle reqHandle = 
	        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
	        CSatTsy::ESatNotifyProactiveSimSessionEndReqType );
	           
	    // In case the request was ongoing, complete the request to the client.
	    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
	        {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSIMSESSIONEND_COMPLETENOTIFYL_2, "CSAT: CSatNotifySimSessionEnd::CompleteNotifyL, SIM SESSION END");
	        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, aErrorCode );
	        }
        }

    return KErrNone;
    }

// End of file
