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
// Name        : CSatNotifySendData.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Send data notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySendDataTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifySendData.h" 	// Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifySendData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySendData* CSatNotifySendData::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_NEWL_1, "CSAT: CSatNotifySendData::NewL");
   	CSatNotifySendData* const satNotifySendData = 
        new ( ELeave ) CSatNotifySendData( aNotificationsTsy );
    CleanupStack::PushL( satNotifySendData );
    satNotifySendData->ConstructL();
    CleanupStack::Pop( satNotifySendData );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_NEWL_2, "CSAT: CSatNotifySendData::NewL, end of method");
    return satNotifySendData;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::~CSatNotifySendData
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendData::~CSatNotifySendData
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_DTOR_1, "CSAT: CSatNotifySendData::~CSatNotifySendData");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySendData::CSatNotifySendData
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendData::CSatNotifySendData
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySendData::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_CONSTRUCTL_1, "CSAT: CSatNotifySendData::ConstructL, does nothing");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySendData::Notify
// This request allows a client to be notified of a SEND DATA proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendData::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage    
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_NOTIFY_1, "CSAT: CSatNotifySendData::Notify");
    // Save data pointer to client side for completion
    iSendDataRspV2Pckg = reinterpret_cast<RSat::TSendDataV2Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySendDataPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSendData );   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySendData request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendData::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_CANCELNOTIFICATION_1, "CSAT: CSatNotifySendData::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifySendDataPCmdReqType );
	// Reset the data pointers
	iSendDataRspV2Pckg = NULL;	
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;       
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifySendData request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendData::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_COMPLETENOTIFYL_1, "CSAT: CSatNotifySendData::CompleteNotifyL");
    TInt ret( KErrNone );
	TBuf<1> noAdditionalInfo( 0 );
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifySendDataPCmdReqType );    
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
	        // Fill the Send Data structure             
	        RSat::TSendDataV2& sendDataV2 = 
	            ( *iSendDataRspV2Pckg )();
	        
	        // Store transaction ID
	        sendDataV2.SetPCmdNumber( pCmdNumber );
	        
	        // Channel Id (Mandatory)
	        CTlv deviceIds;
	        // Channel data (Mandatory)
	        CTlv channelData;
	        if ( ( KErrNone == berTlv.TlvByTagValue( &channelData,
	                                      KTlvChannelDataTag ) ) &&
	             ( KErrNone == berTlv.TlvByTagValue( &deviceIds, 
	             	                      KTlvDeviceIdentityTag ) ) )
	            {
		        // Set channel ID
	            sendDataV2.iDestination = ( RSat::TDeviceId ) deviceIds.
	            	GetShortInfo( ETLV_DestinationDeviceIdentity );

	            // Set channel data
	            sendDataV2.iChannelData.Copy( 
	                channelData.GetData( ETLV_ChannelDataString ) );

	            //Set Data mode using command qualifier
	            TUint8 cmdQualifier( commandDetails.GetShortInfo( 
	            	ETLV_CommandQualifier ) );
	            if ( KBipSendDataImmediately & cmdQualifier )
	                {
	                // bit 1 = 1
	                sendDataV2.iMode = RSat::ESendDataImmediately;
	                }
	            else
	                {
	                // bit 1 = 0
	                sendDataV2.iMode = RSat::EStoreDataInTxBuffer;
	                }
	            
	            // Alpha Id (Optional)
	            sendDataV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
	            CTlv alphaIdentifier;
	            TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier, 
	                KTlvAlphaIdentifierTag ) );
	                
	            if ( KErrNotFound != returnValue )
	                {
	                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_COMPLETENOTIFYL_2, "CSAT: CSatNotifySendData::CompleteNotifyL Alpha ID found");
	                TUint16 alphaIdLength = alphaIdentifier.GetLength();
	                if ( alphaIdLength )
	                    {
	                    // get the alpha id
	                    TPtrC8 sourceString;
	                    sourceString.Set( 
	                        alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
	                    // convert and set the alpha id
	                    TSatUtility::SetAlphaId( sourceString , 
	                        sendDataV2.iAlphaId.iAlphaId ); 
	                    sendDataV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
	                    }
	                else
	                    {
	                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_COMPLETENOTIFYL_3, "CSAT: CSatNotifySendData::CompleteNotifyL Alpha id is NULL");
	                    sendDataV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
	                    }
	                }

	            // Icon Id (Optional)
	            TSatUtility::FillIconStructure( berTlv,
	                sendDataV2.iIconId );

	            }
	        else
	            {
	            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_COMPLETENOTIFYL_4, "CSAT: CSatNotifySendData::CompleteNotifyL Required values missing");
	            // Required values missing
	            TUint8 channelDataLength( 0 );
	            CreateTerminalRespL( 
	                 pCmdNumber, RSat::KErrorRequiredValuesMissing,
	                 KNullDesC16, channelDataLength );
	            ret = KErrCorrupt;
	            }
	            
	        }   
        else // if ( KErrNone == aErrorCode )
        	{
        	ret = aErrorCode; 
        	}
        // Complete request
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );  
        }
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_COMPLETENOTIFYL_5, "CSAT: CSatNotifySendData::CompleteNotifyL Request not ongoing");
		// Request not on, returning response immediately       
        TUint8 channelDataLength( 0 );
		TBuf16<1> additionalInfo;	
        additionalInfo.Append ( RSat::KNoSpecificMeProblem );
        CreateTerminalRespL( 
           pCmdNumber, RSat::KMeUnableToProcessCmd,
           additionalInfo, channelDataLength );
        ret = KErrCorrupt;   	
		}        
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendData::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_TERMINALRESPONSEL_1, "CSAT: CSatNotifySendData::TerminalResponseL");
    TInt   ret( KErrNone );
    
    TBuf16<RSat::KAdditionalInfoMaxSize> additionalInfo;

    RSat::TSendDataRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSendDataRspV2Pckg*>( aRsp );
    RSat::TSendDataRspV2& rspV2 = ( *aRspPckg ) ();

    TUint8 pCmdNumber( rspV2.PCmdNumber() );

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV2.iGeneralResult ) 
        && ( RSat::KPartialComprehension != rspV2.iGeneralResult )
        && ( RSat::KMissingInformation != rspV2.iGeneralResult )
        && ( RSat::KSuccessRequestedIconNotDisplayed != rspV2.iGeneralResult )
        && ( RSat::KPSessionTerminatedByUser != rspV2.iGeneralResult )
        && ( RSat::KMeUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KNetworkUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV2.iGeneralResult )
        && ( RSat::KCmdTypeNotUnderstood != rspV2.iGeneralResult )
        && ( RSat::KCmdDataNotUnderstood != rspV2.iGeneralResult )
        && ( RSat::KCmdNumberNotKnown != rspV2.iGeneralResult )
        && ( RSat::KErrorRequiredValuesMissing != rspV2.iGeneralResult )
        && ( RSat::KBearerIndepProtocolError != rspV2.iGeneralResult ) )
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_TERMINALRESPONSEL_2, "CSAT: CSatNotifySendData::TerminalResponseL, Invalid general result: %d", rspV2.iGeneralResult);
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
	        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_TERMINALRESPONSEL_3, "CSAT: CSatNotifySendData::TerminalResponseL, Additional Info corrupted ");
	        ret = KErrCorrupt;
	        }
		}     
		  
    CreateTerminalRespL( pCmdNumber, ( TUint8 ) rspV2.iGeneralResult,
        additionalInfo, rspV2.iChannelDataLength );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendData::CreateTerminalRespL
// Constructs send data specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendData::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,      
        const TDesC16& aAdditionalInfo,
        TUint8 aChannelDataLength	
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_CREATETERMINALRESPL_1, "CSAT: CSatNotifySendData::CreateTerminalRespL");
    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );
    
    switch ( aGeneralResult )
    	{
    	case RSat::KMeUnableToProcessCmd:
    	case RSat::KNetworkUnableToProcessCmd:
    	case RSat::KInteractionWithCCPermanentError:
    	case RSat::KErrorRequiredValuesMissing:
    	case RSat::KBearerIndepProtocolError:
	        { 
	        for ( TInt i = 0; i < aAdditionalInfo.Length(); i++ )
	        	{
	        	// Unsuccessful result neccessitating additional info byte
	        	tlvSpecificData.AddByte( static_cast<TUint8>( 
	        		aAdditionalInfo[i] ) );
	        	}
	        break;
	        }
	    case RSat::KSuccess:
	    case RSat::KSuccessRequestedIconNotDisplayed:
	    case RSat::KPartialComprehension:
	    case RSat::KMissingInformation:
	        {
	        // Successful result: append Channel Status
	        tlvSpecificData.AddTag( KTlvChannelDataLengthTag );
	        tlvSpecificData.AddByte( aChannelDataLength );
	        break;
	        }
	    default:
	    	{
	    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDATA_CREATETERMINALRESPL_2, "CSAT: CSatNotifySendData::CreateTerminalRespL Unknown General result");
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

