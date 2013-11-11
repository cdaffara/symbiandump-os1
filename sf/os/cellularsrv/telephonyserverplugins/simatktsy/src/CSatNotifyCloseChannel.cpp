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
// Name        : CSatNotifyCloseChannel.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Close channel notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyCloseChannelTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyCloseChannel.h" // Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyCloseChannel* CSatNotifyCloseChannel::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_NEWL_1, "CSAT: CSatNotifyCloseChannel::NewL");
   	CSatNotifyCloseChannel* const satNotifyCloseChannel = 
        new ( ELeave ) CSatNotifyCloseChannel( aNotificationsTsy );
    CleanupStack::PushL( satNotifyCloseChannel );
    satNotifyCloseChannel->ConstructL();
    CleanupStack::Pop( satNotifyCloseChannel );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_NEWL_2, "CSAT: CSatNotifyCloseChannel::NewL, end of method");
    return satNotifyCloseChannel;
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::~CSatNotifyCloseChannel
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyCloseChannel::~CSatNotifyCloseChannel
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_DTOR_1, "CSAT: CSatNotifyCloseChannel::~CSatNotifyCloseChannel");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::CSatNotifyCloseChannel
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyCloseChannel::CSatNotifyCloseChannel
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyCloseChannel::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_CONSTRUCTL_1, "CSAT: CSatNotifyCloseChannel::~CSatNotifyCloseChannel, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::Notify
// This request allows a client to be notified of a CLOSE CHANNEL proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifyCloseChannel::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage    
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_NOTIFY_1, "CSAT: CSatNotifyCloseChannel::Notify");

    // Save data pointer to client side for completion
    iCloseChannelRspV2Pckg = reinterpret_cast<RSat::TCloseChannelV2Pckg*>( 
        aPackage.Des1n() );     
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyCloseChannelPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KCloseChannel );   

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyCloseChannel request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyCloseChannel::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_CANCELNOTIFICATION_1, "CSAT: CSatNotifyCloseChannel::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyCloseChannelPCmdReqType );
	// Reset the data pointers
	iCloseChannelRspV2Pckg = NULL;	
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;       
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifyCloseChannel request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyCloseChannel::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,  
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_COMPLETENOTIFYL_1, "CSAT: CSatNotifyCloseChannel::CompleteNotifyL");
    TInt ret( KErrNone );
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyCloseChannelPCmdReqType );
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
	        // Fill the Close Channel structure             
	        RSat::TCloseChannelV2& closeChannelV2 = 
	            ( *iCloseChannelRspV2Pckg )();
	        
	        // Store transaction ID
	        closeChannelV2.SetPCmdNumber( pCmdNumber );
	        
	        // Store Channel Id
	        CTlv deviceIds;
	        TInt returnValue( berTlv.TlvByTagValue( &deviceIds, 
	            KTlvDeviceIdentityTag ) );
	        if ( KErrNone == returnValue )
	            {
	            closeChannelV2.iDestination = (RSat::TDeviceId) deviceIds.
	            	GetShortInfo( ETLV_DestinationDeviceIdentity );
	        
		        // Alpha Id (Optional)
		        CTlv alphaIdentifier;
		        returnValue = berTlv.TlvByTagValue( &alphaIdentifier, 
		            KTlvAlphaIdentifierTag ) ;
	            closeChannelV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;            
		        if ( KErrNotFound != returnValue )
		            {
		            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_COMPLETENOTIFYL_2, "CSAT: CSatNotifyCloseChannel::CompleteNotifyL Alpha ID present");
		            TUint16 alphaIdLength = alphaIdentifier.GetLength();
		            if ( alphaIdLength )
		                {
		                // Get the alpha id
		                TPtrC8 sourceString;
		                sourceString.Set( 
		                    alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
		                // convert and set the alpha id
		                TSatUtility::SetAlphaId( sourceString , 
		                    closeChannelV2.iAlphaId.iAlphaId ); 
		                }
		    
		            // Set Alpha ID status
		            if ( closeChannelV2.iAlphaId.iAlphaId.Length() )
		                {
		                closeChannelV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
		                }
		            else
		                {
		                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_COMPLETENOTIFYL_3, "CSAT: CSatNotifyCloseChannel::CompleteNotifyL Alpha ID is NULL");
		                closeChannelV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
		                }
		            }
	
		        // Icon Id (Optional)
		        TSatUtility::FillIconStructure( berTlv,
		            closeChannelV2.iIconId );
            	} // if ( KErrNone == returnValue )
        	else
        		{
            	// Required values missing
            	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_COMPLETENOTIFYL_4, "CSAT: CSatNotifyCloseChannel::CompleteNotifyL, required values missing (Device Identities)");
            	ret = KErrCorrupt;
	            CreateTerminalRespL( pCmdNumber, RSat::KErrorRequiredValuesMissing, KNullDesC16 );
        		}
	        }  // if ( KErrNone == aErrorCode )
        else
        	{
        	ret = aErrorCode;
        	}
        // Complete request
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
        }
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_COMPLETENOTIFYL_5, "CSAT: CSatNotifyCloseChannel::CompleteNotifyL Request not ongoing");
		// Request not on, returning response immediately
		TBuf16<1> additionalInfo;	
        additionalInfo.Append ( RSat::KNoSpecificMeProblem );
        CreateTerminalRespL( pCmdNumber,RSat::KMeUnableToProcessCmd, 
        					additionalInfo );
		ret = KErrCorrupt;        						
		}        
    return ret;    
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifyCloseChannel::TerminalResponseL
        ( 
        TDes8* aRsp 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_TERMINALRESPONSEL_1, "CSAT: CSatNotifyCloseChannel::TerminalResponseL");
    TInt   ret( KErrNone );
    
    TBuf16<1> additionalInfo;
    RSat::TCloseChannelRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TCloseChannelRspV2Pckg*>( aRsp );
    RSat::TCloseChannelRspV2& rspV2 = ( *aRspPckg ) ();

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
        && ( RSat::KErrorRequiredValuesMissing != rspV2.iGeneralResult )
        && ( RSat::KBearerIndepProtocolError != rspV2.iGeneralResult ) 
        && ( RSat::KFramesError != rspV2.iGeneralResult ))
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_TERMINALRESPONSEL_2, "CSAT: CSatNotifyCloseChannel::TerminalResponseL Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    if ( RSat::KMeProblem == rspV2.iInfoType )
        {
        if ( rspV2.iAdditionalInfo.Length() )
        	{
        	additionalInfo.Append( rspV2.iAdditionalInfo[0] );
        	}
    	else
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_TERMINALRESPONSEL_3, "CSAT: CSatNotifyCloseChannel::TerminalResponseL Invalid Additional Info");
        	ret = KErrCorrupt;
        	}
        }
    else if (RSat::KNoAdditionalInfo == rspV2.iInfoType )
    	{
    	// do nothing
    	}
    else
    	{
    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_TERMINALRESPONSEL_4, "CSAT: CSatNotifyCloseChannel::TerminalResponseL Invalid Additional Info type");
    	ret = KErrCorrupt;
    	}
            
    TInt response = CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
    	rspV2.iGeneralResult ),	additionalInfo );
    
	if ( KErrNone == ret )
		ret = response;

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyCloseChannel::CreateTerminalRespL
// Constructs Close channel specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyCloseChannel::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,     
        const TDesC16& aAdditionalInfo	
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_CREATETERMINALRESPL_1, "CSAT: CSatNotifyCloseChannel::CreateTerminalRespL");
    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        || ( RSat::KNetworkUnableToProcessCmd == aGeneralResult ) 
        || ( RSat::KInteractionWithCCPermanentError == aGeneralResult )
        || ( RSat::KErrorRequiredValuesMissing == aGeneralResult )
        || ( RSat::KBearerIndepProtocolError == aGeneralResult )
        || ( RSat::KFramesError == aGeneralResult ))
        {
        // Unsuccessful result requires an additional info byte
        for ( TInt i = 0; i < aAdditionalInfo.Length(); i++ )
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYCLOSECHANNEL_CREATETERMINALRESPL_2, "CSAT: CSatNotifyCloseChannel::CreateTerminalRespL, Unsuccessful result");
        	tlvSpecificData.AddByte( static_cast<TUint8>( 
        	    aAdditionalInfo[i] ) );
        	}
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

