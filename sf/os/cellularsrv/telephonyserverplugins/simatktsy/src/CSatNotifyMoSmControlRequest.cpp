// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CSatNotifyMoSmControlRequest.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Display text notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyMoSmControlRequestTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyMoSmControlRequest.h"  // Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "BerTlv_defs.h"            // Ber Tlv specific definitions
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h" 	// Message manager class for forwarding req.
#include "MSatTsy_IPCDefs.h"        // Sat Tsy internal request types

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyMoSmControlRequest* CSatNotifyMoSmControlRequest::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_NEWL_1, "CSAT: CSatNotifyMoSmControlRequest::NewL");
   	CSatNotifyMoSmControlRequest* const satNotifyMoSmControlRequest = 
        new ( ELeave ) CSatNotifyMoSmControlRequest( aNotificationsTsy );
    CleanupStack::PushL( satNotifyMoSmControlRequest );
    satNotifyMoSmControlRequest->ConstructL();
    CleanupStack::Pop( satNotifyMoSmControlRequest );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_NEWL_2, "CSAT: CSatNotifyMoSmControlRequest::NewL, end of method");
    return satNotifyMoSmControlRequest;
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::~CSatNotifyMoSmControlRequest
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyMoSmControlRequest::~CSatNotifyMoSmControlRequest
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_DTOR_1, "CSAT: CSatNotifyMoSmControlRequest::~CSatNotifyMoSmControlRequest");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::CSatNotifyMoSmControlRequest
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyMoSmControlRequest::CSatNotifyMoSmControlRequest
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyMoSmControlRequest::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_CONSTRUCTL_1, "CSAT: CSatNotifyMoSmControlRequest::ConstructL");
    // Checks if MO-SMS control should be activated
    iNotificationsTsy->iSatTsy->MessageManager()->HandleRequestL( 
    		ESatTsyMoSmsControlActivation );	
    // Initialize MO-SMS Control to deactivated
    iIsMoSmsCtrlActivated = EFalse; 
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_CONSTRUCTL_2, "CSAT: CSatNotifyMoSmControlRequest::ConstructL, end of method");
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::Notify
// This request allows a client to be notified of a MO-SMS Control proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifyMoSmControlRequest::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_NOTIFY_1, "CSAT: CSatNotifyMoSmControlRequest::Notify");

    // Save data pointer to client side for completion      
    iMoSmControlV1Pckg = reinterpret_cast<RSat::TMoSmControlV1Pckg*>(
    	aPackage.Des1n() );
    
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyMoSmControlRequestPCmdReqType );  

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::CancelNotification
// This method cancels an outstanding asynchronous 
// MoSmControl request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyMoSmControlRequest::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_CANCELNOTIFICATION_1, "CSAT: CSatNotifyMoSmControlRequest::CancelNotification");
    
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyMoSmControlRequestPCmdReqType );
	// Reset the data pointer
	iMoSmControlV1Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::CompleteNotifyL
// This method completes an outstanding asynchronous NotifyMoSmControlRequest
// request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyMoSmControlRequest::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_COMPLETENOTIFYL_1, "CSAT: CSatNotifyMoSmControlRequest::CompleteNotifyL" );
    
    CSatNotificationsTsy::TMoSmCtrlData moSmCtrlData;

    // Initialize parameters
    moSmCtrlData.iResult = KMoSmCtrlResultAllowed;
    moSmCtrlData.iAlphaId = NULL;
    
    // The request is completed to the client only if Alpha ID is present,
    // otherwise just handle the message just internally
    if ( KErrNone == aErrorCode )
    	{ 	
    	TBuf8<KAddrMaxLength> addr1;
        TBuf8<KAddrMaxLength> addr2;
        RSat::TAlphaId alphaId;
        alphaId.iAlphaId.Zero();
        alphaId.iStatus = RSat::EAlphaIdNotPresent;
        TBool simRespDataConsistent( ETrue );        
    	TBool isDataOk( ETrue );
    	
    	// Unpack parameters
		TDesC8* atkData = NULL;
		aDataPackage->UnPackData( &atkData );
    	
    	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_COMPLETENOTIFYL_2, "CSAT: CSatNotifyMoSmControlRequest::CompleteNotifyL Data length: %d", ( *atkData ).Length() );
    	
	    if ( KTlvMaxSize < ( *atkData ).Length() )
	    	{
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_COMPLETENOTIFYL_3, "CSAT: CSatNotifyMoSmControlRequest::CompleteNotifyL, Data length exceeded" );
            // Data not consistent, reject the call
            moSmCtrlData.iResult = KMoSmCtrlResultNotAllowed;
	        isDataOk = EFalse;
            }
        else if ( ( *atkData ).Length() )
            {
            // Note: The alpha id can be provided by the NAA even if the addresses
            // have not been modified.                
            ParseNotification( *atkData, addr1, addr2, alphaId  ); 

            if ( KMoSmCtrlResultAllowedWithModifications == ( *atkData )[0] )
                {
                OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_COMPLETENOTIFYL_4, "CSAT: CSatNotifyMoSmControlRequest::CompleteNotifyL Addr1: %s, Addr2: %s", addr1, addr2 );
                simRespDataConsistent = VerifyNotificationData( addr1, addr2 );      
                }
                
            if ( simRespDataConsistent )
                {
            	// atkData[0] --> result
            	moSmCtrlData.iResult = ( *atkData )[0];            
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_COMPLETENOTIFYL_5, "CSAT: CSatNotifyMoSmControlRequest::CompleteNotifyL, NAA response data not consistent" );
                // Data not consistent, reject the sms
                moSmCtrlData.iResult = KMoSmCtrlResultNotAllowed;
	        	isDataOk = EFalse;
                }
            }
             
        // Prepare rest of the data for response        
        moSmCtrlData.iAddressData.iRPDestinationAddr.Set( addr1 );
        moSmCtrlData.iAddressData.iTPDestinationAddr.Set( addr2 );
        moSmCtrlData.iAlphaId = &alphaId;
        
        // Pack data and send response to NAA
    	CSatDataPackage dataPackage;
    	dataPackage.PackData( &moSmCtrlData );
        iNotificationsTsy->iSatTsy->MessageManager()->HandleRequestL( 
        	ESatNotifyMoSmControlRequest, &dataPackage );
	    
	    // Then start checking whether the client nees to be notified as well..
	    // Check if the MO-SMS is activated and the data is verified ok
	    if ( iIsMoSmsCtrlActivated && isDataOk )
	        {          
		    // Get req handle. Can not be deleted yet, since completion
		    // depends on the presence of Alpha ID
		    TTsyReqHandle reqHandle = 
		        iNotificationsTsy->iSatReqHandleStore->TsyReqHandle( 
		            CSatTsy::ESatNotifyMoSmControlRequestPCmdReqType );
		    
		    // In case the request was ongoing, continue.. But note that
		    // the client is notified only if AlphaID is present, in order 
		    // to inform that the ETel API MO-SMS Control structure 
		    // TMoSmControlV1 has been modified.
	        if ( CSatTsy::ESatReqHandleUnknown != reqHandle &&
	             ( moSmCtrlData.iAlphaId->iAlphaId.Length() || 
	             RSat::EAlphaIdNull == moSmCtrlData.iAlphaId->iStatus ) )
	            {
	            // Now its safe to reset the request handle
		        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
		            CSatTsy::ESatNotifyMoSmControlRequestPCmdReqType );
	            // Fill the call control structure
		        RSat::TMoSmControlV1& moSmControlV1 = 
		        	( *iMoSmControlV1Pckg )();
		        
	            // Control result must be mapped to ETel API values.
	            switch ( ( *atkData )[0] )
	                {
	                case KMoSmCtrlResultAllowed:
	                    {
	                    moSmControlV1.iResult = 
	                    	RSat::EAllowedNoModification;
	                    break;
	                    }
	                case KMoSmCtrlResultAllowedWithModifications:
	                    {
	                    moSmControlV1.iResult = 
	                    	RSat::EAllowedWithModifications;
	                    break;
	                    }
	                case KMoSmCtrlResultNotAllowed:
	                    {
	                    moSmControlV1.iResult = RSat::ENotAllowed;
	                    break;
	                    }
	                default:
	                    {
	                    moSmControlV1.iResult = RSat::EControlResultNotSet;
	                    break;
	                    }
	                }

		        // Set alpha id and status
		        moSmControlV1.iAlphaId.iAlphaId.Append( alphaId.iAlphaId );
		        moSmControlV1.iAlphaId.iStatus = alphaId.iStatus;
		        
		        // Complete notification to the client
	    		iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, 
	    		    KErrNone );
	            } // If Alpha ID or request handle

	        } // If MO SM Ctrl Activated and data ok
	    else
	        {
	        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_COMPLETENOTIFYL_6, "LSAT: CSatMoSmsCtrlMessHandler::CompleteNotifyL, iIsMoSmsCtrlActivated: %d, isDataOk: %d ", iIsMoSmsCtrlActivated, isDataOk);
	        }
    	} // if KErrNone
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_COMPLETENOTIFYL_7, "LSAT: CSatMoSmsCtrlMessHandler::CompleteNotifyL, Error in notification ");
        moSmCtrlData.iResult = KMoSmCtrlResultNotAllowed;
        // Pack data and send response right away to NAA
    	CSatDataPackage dataPackage;
    	dataPackage.PackData( &moSmCtrlData );
        iNotificationsTsy->iSatTsy->MessageManager()->HandleRequestL( 
        	ESatNotifyMoSmControlRequest, &dataPackage );
        }

    return KErrNone;
    }
     
// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::CreateMoSmsCtrlEnvelopeL
// Send an envelope to SIM server in order to check wheter the SMS can be sent 
// or not. The response will be used to send a response to SMS server
// -----------------------------------------------------------------------------
//
void CSatNotifyMoSmControlRequest::CreateMoSmsCtrlEnvelopeL
	    (
	    CSatDataPackage* aDataPackage,
	    TInt /*aResult*/
	    )   
    {   
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_CREATEMOSMSCTRLENVELOPEL_1, "CSAT: CSatNotifyMoSmControlRequest::CreateMoSmsCtrlEnvelopeL");
    // Check first if the MO-SMS is activated
    if ( iIsMoSmsCtrlActivated )
        {  
    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_CREATEMOSMSCTRLENVELOPEL_2, "CSAT: CSatNotifyMoSmControlRequest::CreateMoSmsCtrlEnvelopeL, MO SM Ctrl Activated");
    	
	    CSatNotificationsTsy::TAddressData* addressData;
	    
	    // Unpack data
	    aDataPackage->UnPackData( &addressData );
	    
	    TTlv envelope;

	    envelope.Begin( KBerTlvMoShortMessageControlTag );
	    //device identities
	    envelope.AddTag( KTlvDeviceIdentityTag );
	    envelope.AddByte( KMe );
	    envelope.AddByte( KSim );
	    // Address Data object 1
	    // Ton and Npi is in the first byte of address data
	    envelope.AddTag( KTlvAddressTag );
	    envelope.AddData( addressData->iRPDestinationAddr );
	    // Address Data object 2
	    // Ton and Npi is in the first byte of address data
	    envelope.AddTag( KTlvAddressTag );
	    envelope.AddData( addressData->iTPDestinationAddr );

	    // !!! This part assumes that location info has been received
	    // Mobile Country & Network Codes (MCC & MNC)
	    iNotificationsTsy->AddLocationInformationToTlv( envelope );
	        	
	   	// Prepare data
	    TPtrC8 data = envelope.End();
	    // Pack data
	    CSatDataPackage dataPackage;
	    dataPackage.PackData( &data );
	    
	    // Forward request to the DOS. Send envelope only if its length is less 
	    // than the maximum allowed envelope length.
	    iNotificationsTsy->iSatTsy->MessageManager()->HandleRequestL( 
	        ESatTsySendEnvelope, &dataPackage );
        }
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::Activate
// Set iIsMoSmsCtrlActivated to ETrue if MO-SMS Control has to be activated
// -----------------------------------------------------------------------------
//
void CSatNotifyMoSmControlRequest::SetActivationStatus
		(
		CSatDataPackage* aDataPackage
		)
    {
    // Unpack data
    aDataPackage->UnPackData( iIsMoSmsCtrlActivated );
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_SETACTIVATIONSTATUS_1, "CSAT: CSatNotifyMoSmControlRequest::SetActivationStatus, %d",iIsMoSmsCtrlActivated );
    }

// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::ActivationStatus
// Returns ETrue if MO-SMS Control is activated
// -----------------------------------------------------------------------------
//
TBool CSatNotifyMoSmControlRequest::ActivationStatus
		(
		void
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_ACTIVATIONSTATUS_1, "CSAT: CSatNotifyMoSmControlRequest::ActivationStatus");
    return iIsMoSmsCtrlActivated;
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::CleanAddressData
// This method cleans the input address data, so that all the bytes found
// after a 0xXF or a 0xFX are removed.
// -----------------------------------------------------------------------------
//
void CSatNotifyMoSmControlRequest::CleanAddressData
		( 
		TDes8& aAddr 
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_CLEANADDRESSDATA_1, "CSAT: CSatNotifyMoSmControlRequest::CleanAddressData");
    // Starts from index 1, since index 0 is for TON/NPI    
    for ( TInt i=1; i < aAddr.Length(); i++ )
        {
        if ( 0xF0 == ( aAddr[i] & 0xF0 ) )
            {
            if ( ( i + 1 ) < aAddr.Length() )
                {
                aAddr.Delete( i + 1, aAddr.Length() - i - 1 );
                }
            break;
            }
        else if ( 0x0F == ( aAddr[i] & 0x0F ) )
            {
            aAddr.Delete( i, aAddr.Length() - i );
            break;
            }        
        }
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::ParseNotification
// Extracts data from proactive command data
// -----------------------------------------------------------------------------
//
void CSatNotifyMoSmControlRequest::ParseNotification
		(
	    const TDesC8& aAtkData,
	    TDes8& aAddr1,
	    TDes8& aAddr2,
	    RSat::TAlphaId& aAlphaId
	    )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_PARSENOTIFICATION_1, "CSAT: CSatNotifyMoSmControlRequest::ParseNotification");
    
    // Input data:
    // aAtkData[0] : result
    // aAtkData[1][2] : length of BER-TLV, set n=1 if aAtkData[1]=0x81
    // aAtkData[2+n] : tag of first TLV
    // aAtkData[3+n][4+n] : length of first TLV, set nn=1 if aAtkData[3+n]=0x81
    // etc..
    
    // Comments drawn from SAT TS lab test cases:
    // - The NAA may insert a wild '0xD' character in addresses and therefore 
    // the phone sw should prevent the sending of the SMS. At the time this 
    // comment is being written, it is not clear which part prevents it, if 
    // prevents ? And this is not yet handled in SIM ATK TSY.
    // - The NAA may insert a 0xF in addresses, and the phone sw should discard
    // all characters placed after the 0xF. At the time when this
    // comment is being written, it is not clear which part should do that,
    // and this is not yet handled in SIM ATK TSY.
    
    // Check if the BER-TLV length is coded with 2 bytes.
    TInt indexInBerTlv( 1 );
    TUint8 tag( aAtkData[indexInBerTlv] );
    TUint8 lengthBerTlv( 0 );
    TInt n( ( KTwoByteLengthCoding == tag ) ? 1:0 );    
    lengthBerTlv = aAtkData[1 + n];
    indexInBerTlv = 2 + n;
    
    // Initialize Alpha ID
    aAlphaId.iAlphaId.Zero();
    
    // Extract TLVs and fill in output variables
    TBool address1_got( EFalse );
    while( lengthBerTlv )
        {
        // Get tag value without 'comprehension required' bit
        tag = aAtkData[indexInBerTlv] & 0x7F;
        // Length of TLV with 2 bytes ?
        TInt nn( ( KTwoByteLengthCoding == aAtkData[indexInBerTlv +1 ] )?1:0 );
        // Length of TLV
        TInt lengthTlv( aAtkData[indexInBerTlv + nn + 1] );
        
        // Check which TLV is received
        if ( ( KTlvAddressTag == tag ) && ( !address1_got ) )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_PARSENOTIFICATION_2, "CSAT: CSatNotifyMoSmControlRequest::ParseNotification Address 1");
            // RP address first
            address1_got = ETrue;
            // Copy data to output variable
            aAddr1.Copy( aAtkData.Mid( indexInBerTlv + nn + 2, lengthTlv ) );
            if ( KReservedTonNpi1 != aAddr1[0] && 
            	 KReservedTonNpi2 != aAddr1[0] && ( 1 < lengthTlv ) )
                {
                CleanAddressData( aAddr1 );
                }            
            }
        else if ( KTlvAddressTag == tag )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_PARSENOTIFICATION_3, "CSAT: CSatNotifyMoSmControlRequest::ParseNotification Address 2");
            // TP address. Copy data to output variable
            aAddr2.Copy( aAtkData.Mid( indexInBerTlv + nn + 2, lengthTlv ) );         
            if ( KReservedTonNpi1 != aAddr2[0] && 
            	 KReservedTonNpi2 != aAddr2[0] && ( 1 < lengthTlv ) )
                {
                CleanAddressData( aAddr2 );
                }    
            }
        else if ( KTlvAlphaIdentifierTag == tag )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_PARSENOTIFICATION_4, "CSAT: CSatNotifyMoSmControlRequest::ParseNotification Alpha ID");
            // Alpha id must be converted to TBuf16/unicode
            if ( lengthTlv )
                {
                // Set Alpha id status
			        aAlphaId.iStatus = RSat::EAlphaIdProvided;
                TSatUtility::SetAlphaId( aAtkData.Mid( indexInBerTlv + nn + 2, 
                    lengthTlv ), aAlphaId.iAlphaId );                                
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_PARSENOTIFICATION_5, "CSAT: CSatNotifyMoSmControlRequest:: ParseNotification. Alpha ID is NULL");
                aAlphaId.iStatus = RSat::EAlphaIdNull;
                }  
            }
 
        // Update length for the while loop    
        lengthBerTlv -= ( lengthTlv + 2 + nn );
        // Updata index for the while loop
        indexInBerTlv += ( lengthTlv + 2 + nn );
        }    
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyMoSmControlRequest::VerifyNotificationData
// This method checks the consistency of the data sent by the NAA to the ME, 
// when the call is allowed and modified.
// -----------------------------------------------------------------------------
//
TBool CSatNotifyMoSmControlRequest::VerifyNotificationData
		( 
		TDes8& aRPAddr, 
		TDes8& aTPAddr 
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYMOSMCONTROLREQUEST_VERIFYNOTIFICATIONDATA_1, "CSAT: CSatNotifyMoSmControlRequest::VerifyNotificationData");
    TBool ret( ETrue );    
    if ( ( 0 == aRPAddr.Length() && 0 == aTPAddr.Length() ) || 
         ( KMoSmCtrlMaxLengthAddr1Addr2 <= ( 
         	aRPAddr.Length() + aTPAddr.Length() ) ) ) 
        {
        ret = EFalse;
        }
    else
        {
        // Detect the presence of 'wild' values in the digits of the addresses       
        for ( TInt i = 0; i < aRPAddr.Length(); i++)
            {
            if ( ( 0x0A < ( 0x0F & aRPAddr[i] ) && 
                   0x0F > ( 0x0F & aRPAddr[i] ) ) ||
                 ( 0xA0 < ( 0xF0 & aRPAddr[i] ) && 
                   0xF0 > ( 0xF0 & aRPAddr[i] ) ) )
                {
                ret = EFalse;
                break;
                }            
            }
        for ( TInt i = 0; i < aTPAddr.Length(); i++)
            {
            if ( ( 0x09 < ( 0x0F & aTPAddr[i] ) && 
                   0x0F > ( 0x0F & aTPAddr[i] ) ) ||
                 ( 0x90 < ( 0xF0 & aTPAddr[i] ) && 
                   0xF0 > ( 0xF0 & aTPAddr[i] ) ) )
                {
                ret = EFalse;
                break;
                }            
            }        
        }    
    return ret;
    }
    
// End of File

