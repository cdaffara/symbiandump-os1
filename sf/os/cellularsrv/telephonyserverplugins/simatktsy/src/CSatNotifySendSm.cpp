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
// Name        : CSatNotifySendSm.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// SendSm notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySendSmTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifySendSm.h"       // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifySendSm::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySendSm* CSatNotifySendSm::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_NEWL_1, "CSAT: CSatNotifySendSm::NewL");
   	CSatNotifySendSm* const satNotifySendSm = 
        new ( ELeave ) CSatNotifySendSm( aNotificationsTsy );
    CleanupStack::PushL( satNotifySendSm );
    satNotifySendSm->ConstructL();
    CleanupStack::Pop( satNotifySendSm );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_NEWL_2, "CSAT: CSatNotifySendSm::NewL, end of method");
    return satNotifySendSm;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::~CSatNotifySendSm
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendSm::~CSatNotifySendSm
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_DTOR_1, "CSAT: CSatNotifySendSm::~CSatNotifySendSm");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySendSm::CSatNotifySendSm
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendSm::CSatNotifySendSm
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySendSm::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CONSTRUCTL_1, "CSAT: CSatNotifySendSm::ConstructL, does nothing");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySendSm::Notify
// This request allows a client to be notified of a Send Sm proactive command.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_NOTIFY_1, "CSAT: CSatNotifySendSm::Notify");
    // Save data pointer to client side for completion
    iSendSmV1Pckg = reinterpret_cast<RSat::TSendSmV1Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySendSmPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSendShortMessage );     
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::CancelNotification
// This method cancels an outstanding asynchronous NotifySendSm request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CANCELNOTIFICATION_1, "CSAT: CSatNotifySendSm::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifySendSmPCmdReqType );
	// Reset the data pointer
	iSendSmV1Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::CompleteNotifyL
// This method completes an outstanding asynchronous NotifySendSm request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,  
        TInt aErrorCode                 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_COMPLETENOTIFYL_1, "CSAT: CSatNotifySendSm::CompleteNotifyL");
    TInt ret( KErrNone );
	TBuf<1> additionalInfo;
	// Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
	// Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
    	ResetTsyReqHandle( CSatTsy::ESatNotifySendSmPCmdReqType );
	
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
			// Fill the send sm structure             
			RSat::TSendSmV1& sendSmV1 = ( *iSendSmV1Pckg )();
			// Command number
			sendSmV1.SetPCmdNumber( pCmdNumber  );
			sendSmV1.iSmsTpdu.Zero();
			CTlv smsTpduTlv;
			// Get TPdu tlv
			TInt returnValue( berTlv.TlvByTagValue( &smsTpduTlv, 
			    KTlvSmsTpduTag ) );
			    
			if ( KErrNone == returnValue )
				{
				// Tpdu TLV length
				TUint16 smsTpduLength( smsTpduTlv.GetLength() );
				// Set pointer to Tpdu
				TPtrC8 smsTpdu = smsTpduTlv.GetData( ETLV_SmsTPdu );
				// Check that TP-UDL is valid
				ret = CheckTpdu( smsTpdu );
				
				if ( KErrCorrupt == ret )
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_COMPLETENOTIFYL_2, "CSAT: CSatNotifySendSm::CompleteNotifyL, Invalid data");
					additionalInfo.Zero();
                    additionalInfo.Append( KNoCause );
					CreateTerminalRespL( pCmdNumber, 
					    RSat::KCmdDataNotUnderstood, additionalInfo );                            
                    }
				else if ( KErrNone == ret && smsTpduLength )
					{
					// Copying the TTpdu
					sendSmV1.iSmsTpdu.Append( smsTpdu );
					}

				// Checking if packing is required for the SMS message or not
				// Packing: see ETSI 3.38 and 3.40
				if ( ( KSmsPackingRequiredMask & iNotificationsTsy->
                    iTerminalRespData.iCommandDetails[KCommandQualifier] )
					&& ( KErrNone == ret ) )
					{
					// Packing required
					if ( smsTpduLength )
						{
						// Call method to pack sms
						ret = PackSms( smsTpdu, sendSmV1.iSmsTpdu );
						if ( KErrNone != ret )
							{
							OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_COMPLETENOTIFYL_3, "CSAT: CSatNotifySendSm::CompleteNotifyL, Invalid TPDU");
							// TPDU is invalid or packing cannot be requested 
							// if tpdu is something else than SMS-SUBMIT
							additionalInfo.Zero();
                            additionalInfo.Append( KNoCause );
							CreateTerminalRespL( pCmdNumber,                  
								RSat::KCmdDataNotUnderstood, additionalInfo );         
							ret = KErrCorrupt;
							}
						else
							{
							// Do nothing
							}					
						}
					else
						{
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_COMPLETENOTIFYL_4, "CSAT: CSatNotifySendSm::CompleteNotifyL, Packing not required");
						}
					}
				else
					{
					// Do nothing
					}
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_COMPLETENOTIFYL_5, "CSAT: CSatNotifySendSm::CompleteNotifyL, Missing TPDU");
				// TPdu is missing, returning response immediately
				additionalInfo.Zero();
                additionalInfo.Append( KNoCause );
				CreateTerminalRespL( pCmdNumber, 
				    RSat::KErrorRequiredValuesMissing, additionalInfo );
				ret = KErrCorrupt;
				}
				
			if ( KErrNone == ret )
				{
				SetAlphaIdAndAddressData( &berTlv, sendSmV1 );
				// Iconid 
				TSatUtility::FillIconStructure( berTlv, sendSmV1.iIconId );
				} 
			
			} // End of if ( KErrNone == aErrorCode )
        else
        	{
        		ret = aErrorCode;
        	}
        
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );   
        	
        } // End of if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_COMPLETENOTIFYL_6, "CSAT: CSatNotifySendSm::CompleteNotifyL, Request not ongoing");
		// Request not on, returning response immediately
		additionalInfo.Zero();
        additionalInfo.Append( KNoCause );       
		CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd, 
			additionalInfo );					 
		}
	return ret;
	}

// -----------------------------------------------------------------------------
// CSatNotifySendSm::TerminalResponseL
// Called by ETel server, passes terminal response to NAA
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::TerminalResponseL
        (
        TDes8* aRsp 
        )
    {  
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_TERMINALRESPONSEL_1,  "CSAT::CSatNotifySendSm::TerminalResponseL" );
    
    TInt ret( KErrNone );
    TBuf<1> additionalInfo;
    RSat::TSendSmRspV1Pckg* aRspPckg = 
        reinterpret_cast<RSat::TSendSmRspV1Pckg*>( aRsp );
    RSat::TSendSmRspV1& rspV1 = ( *aRspPckg ) ();
    // Get Proactive command number
    TUint8 pCmdNumber( rspV1.PCmdNumber() );
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_TERMINALRESPONSEL_2,  "CSAT::CSatNotifySendSm::TerminalResponseL: GeneralResult: %d", static_cast<TUint8>( rspV1.iGeneralResult ) );

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult ) && 
         ( RSat::KMeUnableToProcessCmd  != rspV1.iGeneralResult ) && 
         ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult )&& 
         ( RSat::KSmsRpError != rspV1.iGeneralResult ) && 
         ( RSat::KInteractionWithCCPermanentError != rspV1.iGeneralResult ) && 
         ( RSat::KModifiedByCallControl != rspV1.iGeneralResult ) && 
         ( RSat::KSuccessRequestedIconNotDisplayed != rspV1.iGeneralResult ) )        
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_TERMINALRESPONSEL_3,  "CSAT::CSatNotifySendSm::TerminalResponseL, Invalid General result" );
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is ME (Mobile Entity) error or network error, additional info
    // is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) || 
         ( RSat::KSatNetworkErrorInfo == rspV1.iInfoType ) || 
         ( RSat::KControlInteraction == rspV1.iInfoType ) )        
        {
        // Check the length of additional info
        if ( 0 == rspV1.iAdditionalInfo.Length() )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_TERMINALRESPONSEL_4,  "CSAT::CSatNotifySendSm::TerminalResponseL, Invalid Additional Info" );
            // No info
            ret = KErrCorrupt;
            }
        else
            {
            additionalInfo.Append( rspV1.iAdditionalInfo[0] );
			}
        }
	else
		{
		// Do nothing
		}
		
    // Creating the info message
	TInt response = CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
	    rspV1.iGeneralResult ), additionalInfo );

	if(KErrNone == ret)
		ret = response;
	return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::CreateTerminalRespL
// Constructs SendSm specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::CreateTerminalRespL
		( 
        TUint8 aPCmdNumber,        
        TUint8 aGeneralResult,      
        TDesC16& aAdditionalInfo		
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CREATETERMINALRESPL_1,  "CSAT::CSatNotifySendSm::CreateTerminalRespL" );
    // Create and append response data
    TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    // General result
    tlvSpecificData.AddByte( aGeneralResult );
    
    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult ) ||
         ( RSat::KNetworkUnableToProcessCmd == aGeneralResult ) ||
         ( RSat::KSmsRpError == aGeneralResult ) ||
         ( RSat::KInteractionWithCCPermanentError == aGeneralResult ) )
        {
		if( aAdditionalInfo.Length() > 0 )
	        {
	        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CREATETERMINALRESPL_2,  "CSAT::CSatNotifySendSm::CreateTerminalRespL, Unsuccessful result" );
	        // If there is ME, Network or SMS error, append additional info
	        tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[0] ) );
	        }
		}
    else
    	{
    	// Do nothing
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
    
// -----------------------------------------------------------------------------
// CSatNotifySendSm::SetAlphaIdAndAddressData
// Set Alpha id and Address into send sm structure
// -----------------------------------------------------------------------------
//
void CSatNotifySendSm::SetAlphaIdAndAddressData
		(
		CBerTlv* aBerTlv,
		RSat::TSendSmV1& aSendSmV1
		)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_SETALPHAIDANDADDRESSDATA_1,  "CSAT::CSatNotifySendSm::SetAlphaIdAndAddressData" );
	TInt returnValue;
	TPtrC8 sourceString; // Used in unicode conversions
	// Alpha id string (optional)
	aSendSmV1.iAlphaId.iAlphaId.Zero();
	CTlv alphaIdTlv;
	returnValue = aBerTlv->TlvByTagValue( &alphaIdTlv, 
	    KTlvAlphaIdentifierTag ) ;
	    
	if ( KErrNone == returnValue )
		{
		// Alpha id tlv found
		if ( alphaIdTlv.GetLength() )
			{
			// Get alpha id text
			sourceString.Set( alphaIdTlv.GetData( ETLV_AlphaIdentifier ) );
			// Convert and set alpha id
			TSatUtility::SetAlphaId( sourceString, 
			    aSendSmV1.iAlphaId.iAlphaId );
			}
			
		// Alpha Tag present
		if ( aSendSmV1.iAlphaId.iAlphaId.Length() )
			{
			aSendSmV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_SETALPHAIDANDADDRESSDATA_2,  "CSAT::CSatNotifySendSm::SetAlphaIdAndAddressData, Alpha ID is NULL" );
			aSendSmV1.iAlphaId.iStatus = RSat::EAlphaIdNull;
			}
		}
	else 
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_SETALPHAIDANDADDRESSDATA_3,  "CSAT::CSatNotifySendSm::SetAlphaIdAndAddressData, Alpha ID not present" );
		aSendSmV1.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
		}
	// The address data object holds the RP_Destination_Address of the Service 
	// Centre. If no RP_Destination_Address is transferred, then the ME shall 
	// insert the default Service Centre address (below).
	aSendSmV1.iAddress.iTelNumber.Zero();
	CTlv addressTlv;
	// Get address tlv. Includes SCA number. (optional)
	returnValue = aBerTlv->TlvByTagValue( &addressTlv, KTlvAddressTag );
	
	if ( KErrNone == returnValue )
		{
		if ( 0 < addressTlv.GetLength() )
		    {		
			// Initialize ton and npi
			RSat::TTypeOfNumber ton;
			RSat::TNumberingPlan npi;
			// Call utility function that maps received TON and NPI to 
			// RSat values
			TSatUtility::TonAndNpi( 
			addressTlv.GetShortInfo( ETLV_TonAndNpi ), &ton, &npi );
			// Set TON and NPI
			aSendSmV1.iAddress.iTypeOfNumber = ton;
			aSendSmV1.iAddress.iNumberPlan = npi;    
			TBuf8<RSat::KMaxMobileTelNumberSize> tempScaNumber;
			// set pointer to address tlv
			sourceString.Set( addressTlv.GetData( 
			    ETLV_DiallingNumberString ) ); 
	
			if ( sourceString.Length() )
				{
				// Semi-octet presentation used
				// Converting back to ASCII format
				TSatUtility::BCDToAscii( sourceString, tempScaNumber );
				aSendSmV1.iAddress.iTelNumber.Copy( tempScaNumber );
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_SETALPHAIDANDADDRESSDATA_4, "CSAT: SendSm, SCA number: %S", aSendSmV1.iAddress.iTelNumber );
				}
       		else
      			{
      			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_SETALPHAIDANDADDRESSDATA_5,  "CSAT::CSatNotifySendSm::SetAlphaIdAndAddressData Address TLV found, TON/NPI present, but number epmpty" );
        		// Address TLV found, TON/NPI present, but number epmpty.
       			aSendSmV1.iAddress.iTypeOfNumber = RSat::EUnknownNumber;
        		aSendSmV1.iAddress.iNumberPlan = RSat::EUnknownNumberingPlan;
      			}
		    }
		else
		    {
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_SETALPHAIDANDADDRESSDATA_6,  "CSAT::CSatNotifySendSm::SetAlphaIdAndAddressData Address TLV found, but the Value part doesn't exist");
		    // Address TLV found, but the Value part doesn't exist
		    aSendSmV1.iAddress.iTypeOfNumber = RSat::EUnknownNumber;
		    aSendSmV1.iAddress.iNumberPlan = RSat::EUnknownNumberingPlan;
		    }
		}
	else
	    {
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_SETALPHAIDANDADDRESSDATA_7,  "CSAT::CSatNotifySendSm::SetAlphaIdAndAddressData Address TLV not found");
	    // Address TLV not found
	    aSendSmV1.iAddress.iTypeOfNumber = RSat::ETypeOfNumberNotSet;
	    aSendSmV1.iAddress.iNumberPlan = RSat::ENumberingPlanNotSet;  
	    }	
	}
	
// -----------------------------------------------------------------------------
// CSatNotifySendSm::CheckTpdu
// This method Checks TPDU validity
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::CheckTpdu
	    (
	    TPtrC8 aTpdu    
	    )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CHECKTPDU_1,  "CSAT::CSatNotifySendSm::CheckTpdu");
    TInt ret( KErrNone );

    if ( KSATSmsMTISubmitOrSubmitReport == ( aTpdu[0] & KMask3 ) ) 
        {
        // SMS-SUBMIT
        TUint8 tpUdl( GetTpUdl( aTpdu ) ); // TP-User-Data-Length
        // Get data coding scheme, leave only Alphabet bits
        TUint8 dcs( GetTpDcs( aTpdu ) );
        dcs &= KDCSAlphabetMask;
        
        if ( !dcs && KSmsMaxSize < tpUdl )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CHECKTPDU_2,  "CSAT::CSatNotifySendSm::CheckTpdu, False DCS Length");
            // DCS is 7-bit and message is over 160 bytes, set ret to corrupt
            ret = KErrCorrupt;
            }
        else if ( ( !( KSmsPackingRequiredMask & iNotificationsTsy->
        	iTerminalRespData.iCommandDetails[KCommandQualifier] ) )
            && ( KSmsMaxSizeWithoutPacking < tpUdl && dcs ) )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CHECKTPDU_3,  "CSAT::CSatNotifySendSm::CheckTpdu, TP-UD Too long without packing");
            // TP-UD is too long without packing
            ret = KErrCorrupt;
            }
        else if ( ( KSmsMaxSize < tpUdl ) && dcs )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CHECKTPDU_4,  "CSAT::CSatNotifySendSm::CheckTpdu, TP-UD is too long even with packing");
            // TP-UD is too long even with packing
            ret = KErrCorrupt;
            }
        else
        	{
        	// Do nothing
        	}
        }
    else
        {
        // SMS-COMMAND
        if ( KSMSCommandMaxSize < GetTpUdl( aTpdu ) )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_CHECKTPDU_5,  "CSAT::CSatNotifySendSm::CheckTpdu, False SMS Command length");
            ret = KErrCorrupt;
            }
		else
			{
			// Do nothing
			}
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::GetTpUdl
// This method Returns TP-UDL or TP-CDL depending on is sms type SMS-SUBMIT or
// SMS-COMMAND.
// -----------------------------------------------------------------------------
//
TUint8 CSatNotifySendSm::GetTpUdl
	    ( 
	    TPtrC8 aTpdu  
	    )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_GETTPUDL_1,  "CSAT::CSatNotifySendSm::GetTpUdl");
    TUint8 ret( 0 );
    if ( KSATSmsMTISubmitOrSubmitReport == ( aTpdu[0] & KMask3 ) )
        {
        // SMS-SUBMIT
        // Calculate the startpos of TP-UD field,
        // 5 = number of mandatory fields
        TUint8 startPos( KTPDUFieldStart );

        // Checking the TP-VP field existence
        // bit4    bit3
        // 0       0   TP-VP field not present
        // 1       0   TP-VP field present - relative format 
        //             KSmsVPFRelative:0x10:000 10 000
        // 0       1   TP-VP field present - enhanced format 
        //             KSmsVPFEnhanced:0x08:000 01 000
        // 1       1   TP-VP field present - absolute format 
        //             KSmsVPFAbsolute:0x18:000 11 000
        //
        if ( KSmsVPFRelative == ( aTpdu[0] & KSmsVPFAbsolute ) )
            {
            startPos++;
            }
        if ( ( KSmsVPFEnhanced == ( aTpdu[0] & KSmsVPFAbsolute ) ) 
            || ( KSmsVPFAbsolute == ( aTpdu[0] & KSmsVPFAbsolute ) ) )
            {
            startPos += 7;
            }

        // Then the address field length.
        // Address field length is number of BCD charachters
        // Two BCD string chars are coded in one byte.
        // Divide by two to get number of bytes and plus two is
        // length tag + TON&NPI byte.
        startPos = TUint8( startPos + aTpdu[2] / 2 + 2 );
        if ( aTpdu[2] % 2 )
            {
            // If odd number of BCD characters
            // increase position by one
            startPos++;
            }
        
        else
        	{
        	// Do nothing
        	}

        ret = aTpdu[startPos]; // Return TP-UDL
        }
    else    // SMS-COMMAND
        {
        TUint8 startPos( 0x05 );
        startPos = static_cast<TUint8>( startPos + aTpdu[5] / 2 + 2 );
        if ( aTpdu[5] % 2 )
            {
            // If odd number of characters
            // increase position by one
            startPos++;
            }
        ret = aTpdu[startPos]; // Return TP-CDL
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::GetTpDcs
// This method Returns data coding scheme of the SMS. In the case SMS is 
// SMS-COMMAND then return null.
// -----------------------------------------------------------------------------
//
TUint8 CSatNotifySendSm::GetTpDcs
	    ( 
	    TPtrC8 aTpdu 
	    )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_GETTPDCS_1,  "CSAT::CSatNotifySendSm::GetTpDcs");
    TUint8 retValue( NULL );
    if ( KSATSmsMTISubmitOrSubmitReport == ( aTpdu[0] & KMask3 ) )
        {
        // SMS-SUBMIT
        TUint8 posDCS( 0 );
        posDCS = static_cast<TUint8>( aTpdu[2] / 2 + 2 );
    
        if ( aTpdu[2] % 2 )
            {
            // If odd number of characters
            // increase position by one
            posDCS++;
            }

        // Add 3 for first octet, message reference and protocol identifier
        // mandatory fields
        posDCS += 3;

        retValue = aTpdu[posDCS];
        }
    // SMS-COMMAND
    return retValue;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSm::PackSms
// This method Pack SMS if it is required by the NAA 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSm::PackSms
	    (
	    TPtrC8 aTpdu,   
	    TTpdu& aSendSm  
	    )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_PACKSMS_1,  "CSAT::CSatNotifySendSm::PackSms");
    // See 3GPP TS 23.040 and TS 23.038: 
    // Packing of Basic elements of the SMS SUBMIT type
    TInt ret( KErrNone );
    
    // Message is a SMS-SUBMIT message
    if ( KSATSmsMTISubmitOrSubmitReport == ( aTpdu[0] & KMask3 ) )
        {
        TUint8 startPos( KTPDUFieldStart );

        if ( KSmsVPFRelative == ( aTpdu[0] & KSmsVPFAbsolute ) )
            {
            startPos++;
            }
            
        if ( ( KSmsVPFEnhanced == ( aTpdu[0] & KSmsVPFAbsolute ) ) || 
             ( KSmsVPFAbsolute == ( aTpdu[0] & KSmsVPFAbsolute ) ) )
            {
            startPos += 7;
            }
            
        // Then the address field length, Address field length in semi-octets
        startPos = static_cast<TUint8>( startPos + aTpdu[2] / 2 + 2 );
        
        if ( aTpdu[2] % 2 )
            {
            // if odd number of characters, increase position by one
            startPos++;
            }
        else
        	{
        	// Do nothing
        	}
        	
        TInt i( 0 );
        TInt x = startPos + 1;
        
        // Checking of there is a header field(s) in the message
        if ( KSmsHeaderIndicatorMask & aTpdu[0] )
            {
            // Has headers, Pack all the characters in SMS message
            TUint8 move( 0 );
            TBool endOfTPDU( EFalse );
            x++;    
            
            for ( i = startPos; i < ( aTpdu[startPos] - aTpdu [startPos + 1] ) 
                + startPos; i++ )
                {
                TUint8 char1( static_cast<TUint8> ( 
                	aTpdu[aTpdu[startPos + 1] + 2 + i] >> move ) );
                TUint8 char2( 0 );
                
                if ( ( i + 2 ) <= ( ( aTpdu[startPos] - aTpdu [startPos + 1] ) 
                    + startPos ) )
                    {
                    char2 = static_cast<TUint8>( 
                    	aTpdu[aTpdu[startPos + 1] + 2 + i + 1] << ( 7 - move ) );
                    }
                else
                    {
                    char2 = 0;
                    endOfTPDU = ETrue;                    
                    }
                    
                aSendSm[aTpdu[startPos + 1] + x] = static_cast<TUint8>( 
                    char1 | char2 );
                x++;    
                
                if ( ( 6 == move ) && !endOfTPDU )
                    {
                    i++;
                    move = 0;
                    }
                else
                    {
                    move++;
                    }
                }
            // Calculate the new length (in septets)
            // Old length was in octects
            aSendSm[startPos] = 
            static_cast<TUint8>( ( aTpdu[startPos + 1] * 8 / 7 ) 
                + ( aTpdu[startPos] - aTpdu[startPos + 1] ) );
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_PACKSMS_2,  "CSAT::CSatNotifySendSm::PackSms, No headers");
            // No headers, lets pack all the characters in SMS message
            TBool endOfTPDU( EFalse );
            TUint8 move( 0 );

            for ( i = startPos; i < aTpdu[startPos] + startPos; i++ )
                {
                if ( ( i + 1 ) < aTpdu.Length() )
                    {
                    TUint8 char1( static_cast<TUint8> ( aTpdu[i + 1] >> move ) );
                    TUint8 char2;
                    
                    if ( ( i + 2 ) <= ( aTpdu[startPos] + startPos ) )
                        {
                        if ( ( i + 2 ) < aTpdu.Length() )
                            {
                            char2 = static_cast<TUint8>( aTpdu[i + 2] << 
                            	( 7 - move ) );
                            }
                        else
                            {
                            ret = KErrCorrupt;
                            break;
                            }
                        }
                    else
                        {
                        endOfTPDU = ETrue;
                        char2 = 0;
                        }
                        
                    // Append packed character
                    aSendSm[x++] = static_cast<TUint8>( char1 | char2 );
            
                    if ( ( 6 == move ) && !endOfTPDU )
                        {
                        i++;
                        move = 0;
                        }
                    else
                        {
                        move++;
                        }
                    }
                else
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSM_PACKSMS_3,  "CSAT::CSatNotifySendSm::PackSms, Corrupted");
                    ret = KErrCorrupt;
                    break;
                    }    
                }
            }
            
        if ( KErrCorrupt != ret )
            {
            // New string length (number of characters)
            if ( i - ( x - 1 ) )
                {
                aSendSm.SetLength( aSendSm.Length() - ( i - ( x - 1 ) ) );
                }
                
            // Address field length
            // Address field length in semi-octets (BCD)
            TUint8 posDCS( 0 );
            posDCS = static_cast<TUint8>( aTpdu[2] / 2 + 2 );
            
            if ( aTpdu[2] % 2 )
                {
                // If odd number of characters, increase position by one
                posDCS++;
                }
                
            // Add 3 for first octet, message reference and protocol identifier
            // mandatory fields
            posDCS += 3;
            
            // Change the DCS to default alphabet, Coding: see ETSI 3.38
            if ( ( aTpdu[posDCS] & KMaskC0 ) == 0 )
                {
                aSendSm[posDCS] = static_cast<TUint8>( aSendSm[posDCS] & KMaskF3 );
                }
            if ( ( KMaskF0 & aTpdu[posDCS] ) == KMaskF0 )
                {
                aSendSm[posDCS] = static_cast<TUint8>( aSendSm[posDCS] & KMaskFB );
                }
            else
            	{
            	// Do nothing
            	}
            }
        }
    else
        {    
        // Packing cannot be requested if tpdu is something else than SMS submit
        ret = KErrCorrupt;
        }
    return ret;
    }

// End of file
