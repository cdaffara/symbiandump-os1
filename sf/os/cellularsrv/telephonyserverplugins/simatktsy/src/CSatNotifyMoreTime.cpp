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
// Name        : CSatNotifyMoreTime.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// More time notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyMoreTimeTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyMoreTime.h"  	// Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyMoreTime* CSatNotifyMoreTime::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMORETIME_NEWL_1, "CSAT: CSatNotifyMoreTime::NewL");
   	CSatNotifyMoreTime* const satNotifyMoreTime = 
        new ( ELeave ) CSatNotifyMoreTime( aNotificationsTsy );
    CleanupStack::PushL( satNotifyMoreTime );
    satNotifyMoreTime->ConstructL();
    CleanupStack::Pop( satNotifyMoreTime );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMORETIME_NEWL_2, "CSAT: CSatNotifyMoreTime::NewL, end of method");
    return satNotifyMoreTime;
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::~CSatNotifyMoreTime
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyMoreTime::~CSatNotifyMoreTime
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMORETIME_DTOR_1, "CSAT: CSatNotifyMoreTime::~CSatNotifyMoreTime");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::CSatNotifyMoreTime
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyMoreTime::CSatNotifyMoreTime
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyMoreTime::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMORETIME_CONSTRUCTL_1, "CSAT: CSatNotifyMoreTime::ConstructL, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::CompleteNotifyL
// This method completes an outstanding asynchronous NotifyMoreTime request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyMoreTime::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,
        TInt /*aErrorCode*/              
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMORETIME_COMPLETENOTIFYL_1, "CSAT: CSatNotifyMoreTime::CompleteNotifyL");
    TInt ret( KErrNone );
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );    
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
    	commandDetails.Data() );
	// Get command number
	TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
	
    CreateTerminalRespL( pCmdNumber, RSat::KSuccess );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoreTime::CreateTerminalRespL
// Constructs more time specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyMoreTime::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult			 
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMORETIME_CREATETERMINALRESPL_1, "CSAT: CSatNotifyMoreTime::CreateTerminalRespL");
    // Create and append response data
    TTlv tlvSpecificData;
	// General result
    tlvSpecificData.AddTag( KTlvResultTag );   
    tlvSpecificData.AddByte( aGeneralResult );	
    // Prepare data
    iNotificationsTsy->iTerminalRespData.iPCmdNumber = aPCmdNumber;
    TPtrC8 data = tlvSpecificData.GetDataWithoutTopLevelTag();
    // Pack data
    CSatDataPackage dataPackage;
	dataPackage.PackData( &iNotificationsTsy->iTerminalRespData, &data );
    // Forward request to the DOS
    return iNotificationsTsy->iSatTsy->MessageManager()->HandleRequestL( 
		ESatTerminalRsp, &dataPackage );		
    }
    
// End of file

