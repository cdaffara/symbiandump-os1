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
// Name        : CSatNotifyReceiveData.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Receive data notification functionality of Sat Tsy
// Version     : 1.0
//



// INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyReceiveDataTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyReceiveData.h" 	// Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyReceiveData* CSatNotifyReceiveData::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_NEWL_1, "CSAT: CSatNotifyMoreTime::NewL");
   	CSatNotifyReceiveData* const satNotifyReceiveData = 
        new ( ELeave ) CSatNotifyReceiveData( aNotificationsTsy );
    CleanupStack::PushL( satNotifyReceiveData );
    satNotifyReceiveData->ConstructL();
    CleanupStack::Pop( satNotifyReceiveData );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_NEWL_2, "CSAT: CSatNotifyMoreTime::NewL, end of method");
    return satNotifyReceiveData;
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::~CSatNotifyReceiveData
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyReceiveData::~CSatNotifyReceiveData
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_DTOR_1, "CSAT: CSatNotifyMoreTime::~CSatNotifyReceiveData");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::CSatNotifyReceiveData
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyReceiveData::CSatNotifyReceiveData
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyReceiveData::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_CONSTRUCTL_1, "CSAT: CSatNotifyMoreTime::ConstructL, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::Notify
// This request allows a client to be notified of a RECEIVE DATA proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifyReceiveData::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage    
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_NOTIFY_1, "CSAT: CSatNotifyReceiveData::Notify");
    // Save data pointer to client side for completion
    iReceiveDataRspV2Pckg = reinterpret_cast<RSat::TReceiveDataV2Pckg*>( 
        aPackage.Des1n() );     
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyReceiveDataPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KReceiveData );   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyReceiveData request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyReceiveData::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_CANCELNOTIFICATION_1, "CSAT: CSatNotifyReceiveData::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyReceiveDataPCmdReqType );
	// Reset the data pointers
	iReceiveDataRspV2Pckg = NULL;	
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;       
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifyReceiveData request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyReceiveData::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_COMPLETENOTIFYL_1, "CSAT: CSatNotifyReceiveData::CompleteNotifyL");
    TInt ret( KErrNone );
	TBuf<1> noAdditionalInfo( 0 );
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyReceiveDataPCmdReqType );
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
	        // Fill the Receive Data structure             
	        RSat::TReceiveDataV2& receiveDataV2 = 
	            ( *iReceiveDataRspV2Pckg )();
	        
	        // Store transaction ID
	        receiveDataV2.SetPCmdNumber( pCmdNumber );
	        
	        // Channel Id (Mandatory)
	        CTlv deviceIds;
	        // Channel data length (Mandatory)
	        CTlv channelDataLengthTlv;
	        if ( ( KErrNone == berTlv.TlvByTagValue( &channelDataLengthTlv,
	                                      KTlvChannelDataLengthTag ) ) &&
	             ( KErrNone == berTlv.TlvByTagValue( &deviceIds, 	             	                      
	            		 KTlvDeviceIdentityTag ) ) )
	            {
	            // Set channel ID
	            receiveDataV2.iDestination = ( RSat::TDeviceId ) 
	            deviceIds.GetShortInfo( ETLV_DestinationDeviceIdentity );
                // Set data length
	            receiveDataV2.iChannelDataLength = 
	            	channelDataLengthTlv.GetShortInfo( 
	            	ETLV_ChannelDataLength );

	            // Alpha Id (Optional)
	            receiveDataV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
	            CTlv alphaIdentifier;
	            TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier, 
	                KTlvAlphaIdentifierTag ) );
	                
	            if ( KErrNotFound != returnValue )
	                {
	                TUint16 alphaIdLength = alphaIdentifier.GetLength();
	                if ( alphaIdLength )
	                    {
	                    // get the alpha id
	                    TPtrC8 sourceString;
	                    sourceString.Set( 
	                        alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
	                    // convert and set the alpha id
	                    TSatUtility::SetAlphaId( sourceString , 
	                        receiveDataV2.iAlphaId.iAlphaId ); 
	                    receiveDataV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
	                    }
	                else
	                    {
	                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_COMPLETENOTIFYL_2, "CSAT: CSatNotifyReceiveData::CompleteNotifyL, Alpha ID is NULL");
	                    receiveDataV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
	                    }
	                }

	            // Icon Id (Optional)
	            TSatUtility::FillIconStructure( berTlv,
	                receiveDataV2.iIconId );

	            }
	        else
	            {
	            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_COMPLETENOTIFYL_3, "CSAT: CSatNotifyReceiveData::CompleteNotifyL, Required values missing");
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
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_COMPLETENOTIFYL_4, "CSAT: CSatNotifyReceiveData::CompleteNotifyL, Request not ongoing");
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
// CSatNotifyReceiveData::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifyReceiveData::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_TERMINALRESPONSEL_1, "CSAT: CSatNotifyReceiveData::TerminalResponseL");

    TInt ret( KErrNone );
    
    TBuf<RSat::KAdditionalInfoMaxSize> additionalInfo;
    additionalInfo.Zero();

    RSat::TReceiveDataRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TReceiveDataRspV2Pckg*>( aRsp );
    RSat::TReceiveDataRspV2& rspV2 = ( *aRspPckg ) ();

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
        && ( RSat::KBearerIndepProtocolError != rspV2.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_TERMINALRESPONSEL_2, "CSAT: CSatNotifyReceiveData::TerminalResponseL, Invalid general result");
        ret = KErrCorrupt;
        }

    if ( ( RSat::KMeProblem == rspV2.iInfoType )
        || ( RSat::KChannelData == rspV2.iInfoType ) )
        {
        if ( rspV2.iAdditionalInfo.Length() )
        	{
        	// Channel Data available and sent using Additional info Data are 
        	// received in unicode format: 0x00XX where XX is meaningful.
        	additionalInfo.Copy( rspV2.iAdditionalInfo );
        	}
    	else
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_TERMINALRESPONSEL_3, "CSAT: CSatNotifyReceiveData::TerminalResponseL, Additional Info is Corrupted");
        	ret = KErrCorrupt;
        	}
        }
        
    CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
    	rspV2.iGeneralResult ), additionalInfo, rspV2.iChannelDataLength );
        
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyReceiveData::CreateTerminalRespL
// Constructs receive data specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyReceiveData::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,          
        TUint8 aGeneralResult,      
        const TDesC16& aAdditionalInfo, 
        TUint8 aChannelDataLength
		)
	{   	
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYRECEIVEDATA_CREATETERMINALRESPL_1, "CSAT: CSatNotifyReceiveData::CreateTerminalRespL");
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
    	case RSat::KMissingInformation:
    	case RSat::KPartialComprehension:
	        { 
	        tlvSpecificData.AddTag( KTlvChannelDataTag );
	        for ( TInt i = 0; i < aAdditionalInfo.Length(); i++ )
	        	{
	        	// Successful result: Append Channel Data using additional info
	        	tlvSpecificData.AddByte( static_cast<TUint8>( 
	        		aAdditionalInfo[i] ) );
	        	}
	        // Channel data length
	        tlvSpecificData.AddTag( KTlvChannelDataLengthTag );
	        tlvSpecificData.AddByte( aChannelDataLength );
	        break;
	        }
	    default:
	    	{
	    	// None
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

