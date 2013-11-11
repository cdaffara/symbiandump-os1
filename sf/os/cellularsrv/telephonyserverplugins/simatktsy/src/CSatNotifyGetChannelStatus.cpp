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
// Name        : CSatNotifyGetChannelStatus.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Get channel status notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyGetChannelStatusTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyGetChannelStatus.h"  // Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyGetChannelStatus* CSatNotifyGetChannelStatus::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_NEWL_1, "CSAT: CSatNotifyGetChannelStatus::NewL");
   	CSatNotifyGetChannelStatus* const satNotifyGetChannelStatus = 
        new ( ELeave ) CSatNotifyGetChannelStatus( aNotificationsTsy );
    CleanupStack::PushL( satNotifyGetChannelStatus );
    satNotifyGetChannelStatus->ConstructL();
    CleanupStack::Pop( satNotifyGetChannelStatus );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_NEWL_2, "CSAT: CSatNotifyGetChannelStatus::NewL, end of method");
    return satNotifyGetChannelStatus;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::~CSatNotifyGetChannelStatus
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyGetChannelStatus::~CSatNotifyGetChannelStatus
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_DTOR_1, "CSAT: CSatNotifyGetChannelStatus::~CSatNotifyGetChannelStatus");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::CSatNotifyGetChannelStatus
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyGetChannelStatus::CSatNotifyGetChannelStatus
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyGetChannelStatus::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_CONSTRUCTL_1, "CSAT: CSatNotifyGetChannelStatus::ConstructL, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::Notify
// This request allows a client to be notified of a GET CHANNEL STATUS proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetChannelStatus::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_NOTIFY_1, "CSAT: CSatNotifyGetChannelStatus::Notify");
    // Save data pointers to client side for completion
    iGetChannelStatusRspV2Pckg = reinterpret_cast<RSat::TGetChannelStatusV2Pckg*>( 
        aPackage.Des1n() );         
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyGetChannelStatusPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KGetChannelStatus );   
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyGetChannelStatus request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetChannelStatus::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_CANCELNOTIFICATION_1, "CSAT: CSatNotifyGetChannelStatus::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyGetChannelStatusPCmdReqType );
	// Reset the data pointers
	iGetChannelStatusRspV2Pckg = NULL;	
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifyGetChannelStatus request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetChannelStatus::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_COMPLETENOTIFYL_1, "CSAT: CSatNotifyGetChannelStatus::CompleteNotifyL");

    TInt ret( KErrNone );	
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyGetChannelStatusPCmdReqType );
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
        commandDetails.Data() );
    
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
    
    // In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode )
            {
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_COMPLETENOTIFYL_2, "CSAT: CSatNotifyGetChannelStatus::CompleteNotifyL aErrorCode: %d", aErrorCode );
	        // Fill the Get Channel status structure             
	        RSat::TGetChannelStatusV2& channelStatusV2 = 
	            ( *iGetChannelStatusRspV2Pckg )();
	        
	        // Store transaction ID
	        channelStatusV2.SetPCmdNumber( pCmdNumber );			
			}
        else
        	{
        	ret = aErrorCode;
        	}
		iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );	
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_COMPLETENOTIFYL_3, "CSAT: CSatNotifyGetChannelStatus::CompleteNotifyL Request not ongoing");
   		// Request not on, returning response immediately
		TBuf16<1> additionalInfo;
		additionalInfo.Append( RSat::KNoSpecificMeProblem );
    	CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd,
    	    additionalInfo );				
		ret = KErrCorrupt;
		}
		
	return ret;
	
	}
// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetChannelStatus::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_TERMINALRESPONSEL_1, "CSAT: CSatNotifyGetChannelStatus::TerminalResponseL");
    TInt   ret( KErrNone );
    
    TBuf16<RSat::KAdditionalInfoMaxSize> additionalInfo;

    RSat::TGetChannelStatusRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TGetChannelStatusRspV2Pckg*>( aRsp );
    RSat::TGetChannelStatusRspV2& rspV2 = ( *aRspPckg ) ();

    TUint8 pCmdNumber( rspV2.PCmdNumber() );

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV2.iGeneralResult ) 
        && ( RSat::KPartialComprehension != rspV2.iGeneralResult )
        && ( RSat::KMissingInformation != rspV2.iGeneralResult )
        && ( RSat::KSuccessRequestedIconNotDisplayed != rspV2.iGeneralResult )
        && ( RSat::KPSessionTerminatedByUser != rspV2.iGeneralResult )
        && ( RSat::KMeUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV2.iGeneralResult )
        && ( RSat::KCmdTypeNotUnderstood != rspV2.iGeneralResult )
        && ( RSat::KCmdDataNotUnderstood != rspV2.iGeneralResult )
        && ( RSat::KCmdNumberNotKnown != rspV2.iGeneralResult )
        && ( RSat::KErrorRequiredValuesMissing != rspV2.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_TERMINALRESPONSEL_2, "CSAT: CSatNotifyGetChannelStatus::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    if ( ( RSat::KChannelStatusInfo == rspV2.iInfoType )
        || ( RSat::KMeProblem == rspV2.iInfoType ) )
        {
        if ( !rspV2.iAdditionalInfo.Length() )
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_TERMINALRESPONSEL_3, "CSAT: CSatNotifyGetChannelStatus::TerminalResponseL, Invalid Additional Info");
	        ret = KErrCorrupt;
	        }
	    else
        	{     	
        	additionalInfo.Copy(rspV2.iAdditionalInfo);
        	}
       	}   
       	
    TInt response =CreateTerminalRespL( pCmdNumber, ( TUint8 ) rspV2.iGeneralResult,
                       additionalInfo );

    if ( KErrNone == ret )
    	{
    	ret = response;
    	}

    return ret;

    }

// -----------------------------------------------------------------------------
// CSatNotifyGetChannelStatus::CreateTerminalRespL
// Constructs GetChannelStatus specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetChannelStatus::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,      
        const TDesC16& aAdditionalInfo
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_CREATETERMINALRESPL_1, "CSAT: CSatMessHandler::CreateTerminalRespL");
    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        || ( RSat::KNetworkUnableToProcessCmd == aGeneralResult ) 
        || ( RSat::KInteractionWithCCPermanentError == aGeneralResult )
        || ( RSat::KErrorRequiredValuesMissing == aGeneralResult )
        || ( RSat::KBearerIndepProtocolError == aGeneralResult ) )
        {
        // Unsuccessful result requires an additional info byte
        for ( TInt j = 0; j < aAdditionalInfo.Length(); j++ )
        	{
        	tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[j] ) );
        	}
        }
    else if ( RSat::KSuccess == aGeneralResult )
        {
        // Length check is carried out in
        // CSatNotifyGetChannelStatus::TerminalResponseL       
        for ( TInt i = 1; i < aAdditionalInfo.Length(); i += 2 )
            {
	        tlvSpecificData.AddTag( KTlvChannelStatusTag );
            tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[i - 1] ) );
            tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[i] ) );
            }                 
        }
   	else
   		{
    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETCHANNELSTATUS_CREATETERMINALRESPL_2, "CSAT: CSatMessHandler::CreateTerminalRespL - mandatory channel status missing");
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


