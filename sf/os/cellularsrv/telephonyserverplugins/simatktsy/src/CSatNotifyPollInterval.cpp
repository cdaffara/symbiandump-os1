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
// Name        : CSatNotifyPollInterval.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Polling off notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyPollIntervalTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyPollInterval.h" // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h" 	// Message manager class for forwarding req.
#include "MSatTsy_IPCDefs.h"        // Sat Tsy internal request types


// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyPollInterval* CSatNotifyPollInterval::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPOLLINTERVAL_NEWL_1, "CSAT: CSatNotifyPollInterval::NewL");
   	CSatNotifyPollInterval* const satNotifyPollInterval = 
        new ( ELeave ) CSatNotifyPollInterval( aNotificationsTsy );
    CleanupStack::PushL( satNotifyPollInterval );
    satNotifyPollInterval->ConstructL();
    CleanupStack::Pop( satNotifyPollInterval );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPOLLINTERVAL_NEWL_2, "CSAT: CSatNotifyPollInterval::NewL, end of method");
    return satNotifyPollInterval;
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::~CSatNotifyPollInterval
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyPollInterval::~CSatNotifyPollInterval
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPOLLINTERVAL_DTOR_1, "CSAT: CSatNotifyPollInterval::~CSatNotifyPollInterval");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::CSatNotifyPollInterval
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyPollInterval::CSatNotifyPollInterval
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyPollInterval::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPOLLINTERVAL_CONSTRUCTL_1, "CSAT: CSatNotifyPollInterval::ConstructL");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::CompleteNotifyL
// This method completes an outstanding asynchronous 
// PollInterval request. 
// -----------------------------------------------------------------------------
// 
TInt CSatNotifyPollInterval::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage,
        TInt /*aErrorCode*/  
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPOLLINTERVAL_COMPLETENOTIFYL_1, "CSAT: CSatNotifyPollInterval::CompleteNotifyL");
    TInt returnValue( KErrNone );
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
        commandDetails.Data() );
    // Duration of the interval
    CTlv duration;
    returnValue = berTlv.TlvByTagValue( &duration, KTlvDurationTag );
    
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
    
    TUint16 anIntervalInSeconds( KDefaultPollInterval );
    
    if ( KErrNone == returnValue )
        {
        TUint8 timeIntv( duration.GetShortInfo( ETLV_TimeInterval ) );
        // Time Interval 0 is default value (25 seconds)
		if ( 0 != timeIntv )
			{
            iPollingIntervalRspV3.iGeneralResult = RSat::KSuccess;
            iPollingIntervalRspV3.iInfoType =  RSat::KPollingIntv;

            TUint8 durationTimeUnit( duration.GetShortInfo( ETLV_TimeUnit ) );

            switch ( durationTimeUnit )
                {
                case KMinutes:
                    {
                    anIntervalInSeconds = timeIntv * 60;
                    break;
                    }
                case KSeconds:
                    {
                    anIntervalInSeconds = timeIntv;
                    break;
                    }
                case KTenthsOfSeconds:
                    {
                    // Rounding
	                if ( 5 >= ( timeIntv % 10 ) )
	                    {
	                    // If the intervals the ME can offer are equidistant
	                    // (higher and lower) from the NAA's request, the ME 
	                    // shall respond with the lower interval of the two.
	                    anIntervalInSeconds = static_cast<TUint8>( 
	                        timeIntv / 10 );
	                    }
	                else
	                    {
	                    anIntervalInSeconds = static_cast<TUint8> ( ( 
	                        timeIntv / 10 ) + 1);
	                    }
                    break;
                    }
                default:
                	{
                 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPOLLINTERVAL_COMPLETENOTIFYL_2, "CSAT: CSatNotifyPollInterval::CompleteNotifyL, Time unit unknown.");
	        		returnValue = KErrNotFound;
                 	// Command data not understood
                 	CreateTerminalRespL( pCmdNumber, 
                 	    RSat::KCmdDataNotUnderstood, 
                 	    RSat::KNoSpecificMeProblem, 0 );
                	}
                } // End of switch
        	
        	// Check interval limits
        	if ( KMinPollInterval > anIntervalInSeconds )
        	    {
        	    anIntervalInSeconds = KMinPollInterval;
        	    }
        	else if ( KMaxPollInterval < anIntervalInSeconds )
        	    {
        	    anIntervalInSeconds = KMaxPollInterval;
        	    }
			}
        }
	else
		{
		// Required values are missing
        CreateTerminalRespL( pCmdNumber, RSat::KErrorRequiredValuesMissing, 
        	RSat::KNoSpecificMeProblem, 0 );
		}

    if ( KErrNone == returnValue )
	    {
    	// A PCmd PollingOff may be executed. 	
        iNotificationsTsy->SetPollingOffStatus( EFalse );
        // Send SetPollInterval request to ISA CellMo side. The terminal response
        // will be sent to ISA CellMo side after SetPollInterval response.
        iNotificationsTsy->SetPollingIntervalL( static_cast<TUint8>( 
            anIntervalInSeconds ) );
	    }
 
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CSatNotifyPollInterval::CreateTerminalRespL
// Constructs PollInterval specific part of terminal response and calls 
// DOS to send the actual message. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyPollInterval::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,     
        TUint8 aAdditionalInfo,   
        TUint8 aDcs               	
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPOLLINTERVAL_CREATETERMINALRESPL_1, "CSAT: CSatNotifyPollInterval::CreateTerminalRespL");
    TTlv tlvSpecificData;
    tlvSpecificData.AddTag( KTlvResultTag );     
    tlvSpecificData.AddByte( aGeneralResult );
    
    if ( !(iNotificationsTsy->CommandPerformedSuccessfully( 
            aGeneralResult ) ) )
        {
        switch ( aGeneralResult )
            {
            // Cases in which additional info is not required
            case RSat::KPSessionTerminatedByUser:
            case RSat::KBackwardModeRequestedByUser:
            case RSat::KNoResponseFromUser:
            case RSat::KErrorRequiredValuesMissing:
            case RSat::KHelpRequestedByUser:
                {
                break;
                }
            default:
                {
                OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPOLLINTERVAL_CREATETERMINALRESPL_2, "CSAT: CSatNotifyPollInterval::CreateTerminalRespL, Additional Info: %d", aAdditionalInfo);
                tlvSpecificData.AddByte( aAdditionalInfo );
                break;
                }
            }
        }
    else 
        {
        tlvSpecificData.AddTag( KTlvDurationTag );
        tlvSpecificData.AddByte( KSeconds );
        tlvSpecificData.AddByte( aDcs );
        }

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
