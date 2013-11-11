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
// Name        : CSatNotifySendSs.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Send Ss notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySendSsTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatCCTsy.h"              // Call Control Tsy class header
#include "CSatNotifySendSs.h"       // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.
#include "msattsy_ipcdefs.h"		// Sat ipc values

// -----------------------------------------------------------------------------
// CSatNotifySendSs::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySendSs* CSatNotifySendSs::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_NEWL_1, "CSAT: CSatNotifySendSs::NewL");
   	CSatNotifySendSs* const satNotifySendSs = 
        new ( ELeave ) CSatNotifySendSs( aNotificationsTsy );
    CleanupStack::PushL( satNotifySendSs );
    satNotifySendSs->ConstructL();
    CleanupStack::Pop( satNotifySendSs );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_NEWL_2, "CSAT: CSatNotifySendSs::NewL, end of method");
    return satNotifySendSs;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::~CSatNotifySendSs
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendSs::~CSatNotifySendSs
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_DTOR_1, "CSAT: CSatNotifySendSs::~CSatNotifySendSs");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySendSs::CSatNotifySendSs
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendSs::CSatNotifySendSs
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySendSs::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_CONSTRUCTL_1, "CSAT: CSatNotifySendSs::ConstructL");
    }
   
// -----------------------------------------------------------------------------
// CSatNotifySendSs::Notify
// This request allows a client to be notified of a Send SS proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSs::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_NOTIFY_1, "CSAT: CSatNotifySendSs::Notify");
    // Save data pointer to client side for completion
    iSendSsV1Pckg = reinterpret_cast<RSat::TSendSsV1Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySendSsPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSendSs );   

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySendSs request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSs::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_CANCELNOTIFICATION_1, "CSAT: CSatNotifySendSs::CancelNotification");
    
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifySendSsPCmdReqType );
	// Reset the data pointer
	iSendSsV1Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }



// -----------------------------------------------------------------------------
// CSatNotifySendSs::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifySendSs request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSs::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_COMPLETENOTIFYL_1, "CSAT: CSatNotifySendSs::CompleteNotifyL");
	TInt ret( KErrNone );

    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );

    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
		iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
		CSatTsy::ESatNotifySendSsPCmdReqType );
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
			// Get ber tlv
			CBerTlv berTlv;
			berTlv.SetData( *data );

			// Get command details tlv
			CTlv commandDetails;
			berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    
			// Store command details tlv
			iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
				commandDetails.Data() );

		    // Fill the send ss structure             
			RSat::TSendSsV1& sendSsV1 = ( *iSendSsV1Pckg )();

			// Store command number
            sendSsV1.SetPCmdNumber( pCmdNumber );

			TPtrC8 sourceString; // Used with conversions

			 // Alpha id string (optional)
			sendSsV1.iAlphaId.iAlphaId.Zero();
			CTlv alphaIdentifier;
			// Get alpha id tlv from berTlv
			TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier, 
                                                KTlvAlphaIdentifierTag ) );
			// Check if alpha id string exist
			if ( KErrNone == returnValue ) 
				{
				if ( alphaIdentifier.GetLength() )
					{
					// 8-bit string to 16-bit string
					sourceString.Set( alphaIdentifier.GetData( 
					    ETLV_AlphaIdentifier ) );

					// Convert and set the alpha id
					TSatUtility::SetAlphaId(
						sourceString ,sendSsV1.iAlphaId.iAlphaId ); 
					// Set alpha id status
					sendSsV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_COMPLETENOTIFYL_2, "CSAT: CSatNotifySendSs::CompleteNotifyL, Alpha ID is NULL");
					sendSsV1.iAlphaId.iStatus = RSat::EAlphaIdNull;
					}
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_COMPLETENOTIFYL_3, "CSAT: CSatNotifySendSs::CompleteNotifyL, Alpha ID not present");
				sendSsV1.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
				}

			// SS string Tlv
			sendSsV1.iSsString.iSsString.Zero(); 
			CTlv ssString;
			// Get SS string tlv
			returnValue = berTlv.TlvByTagValue( &ssString, KTlvSsStringTag );
			
			if ( KErrNone == returnValue )
				{
				// Initialize ton and npi
				RSat::TTypeOfNumber ton;
				RSat::TNumberingPlan npi;

				iNotificationsTsy->iSatTsy->SetTonNpi( 
				    ssString.GetShortInfo( ETLV_TonAndNpi ) );
				            
				// Call utility function that maps received TON and NPI 
				// to RSat values
				TSatUtility::TonAndNpi( 
					ssString.GetShortInfo( ETLV_TonAndNpi ), &ton, &npi );

				// Set TON and NPI
				sendSsV1.iSsString.iTypeOfNumber = ton;
				sendSsV1.iSsString.iNumberPlan = npi;
				// Check length of SS string.
				// BCD format = two digits "packed" in one characher
				if ( RSat::KStringMaxSize >= ( ssString.GetLength() * 2 )  )
					{
					TBuf8<RSat::KStringMaxSize> tempString;
					// Set pointer to the BCD SS string
					sourceString.Set( ssString.GetData( 
					    ETLV_SsOrUssdString ) );
					// Semi-octet presentation used
					// Converting back to ASCII format
					TSatUtility::BCDToAscii( sourceString, tempString );
					// Set pointer to the ASCII string.
					sourceString.Set( tempString );
					sendSsV1.iSsString.iSsString.Append( tempString );

					if ( RSat::EInternationalNumber == 
						sendSsV1.iSsString.iTypeOfNumber )
						{
						// Append '+' character if the string is for call forwarding
						CheckCallForwarding( sourceString, sendSsV1.iSsString );
						}

					if ( KErrNone == CheckSsStringValidity( sourceString ) )
						{
						// Valid SS, report to call control
						iNotificationsTsy->iSatTsy->StoreProactiveAddress(&sourceString );
						}
					else
						{
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_COMPLETENOTIFYL_4, "CSAT: CSatNotifySendSs::CompleteNotifyL, Data corrupted");
						CreateTerminalRespL( pCmdNumber, RSat::KCmdDataNotUnderstood,
								KNullDesC16 );
						ret = KErrCorrupt;
						}

					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_COMPLETENOTIFYL_5, "CSAT: CSatNotifySendSs::CompleteNotifyL, SS String length exceeded");
					// SS string is too long
					CreateTerminalRespL( pCmdNumber, RSat::KCmdDataNotUnderstood,
							KNullDesC16 );
					ret = KErrCorrupt;
					}
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_COMPLETENOTIFYL_6, "CSAT: CSatNotifySendSs::CompleteNotifyL, SS String missing");
				// Mandatory tlv is missing, returning response immediately
				CreateTerminalRespL(
					pCmdNumber, RSat::KErrorRequiredValuesMissing, 
					KNullDesC16 );
				ret = KErrCorrupt;
				}
				

			// Icon (optional)
			TSatUtility::FillIconStructure( berTlv, sendSsV1.iIconId );

			} // End of if (KErrNone == aErrorCode)
		else
			{
			ret = aErrorCode;
			}

        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );

        } // End of if ( reqHandle != CSatTsy::ESatReqHandleUnknown )
    else 
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_COMPLETENOTIFYL_7, "CSAT: CSatNotifySendSs::CompleteNotifyL, Request not ongoing");
        // Request not on, returning response immediately
		TBuf<1> additionalInfo;
		additionalInfo.Append( RSat::KNoSpecificMeProblem );
		CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd,  
			additionalInfo );					
    	}

    return ret;
	}


// -----------------------------------------------------------------------------
// CSatNotifySendSs::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSs::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_TERMINALRESPONSEL_1, "CSAT:: CSatNotifySendSs::TerminalResponseL");
    TInt ret( KErrNone );
    // Buffer for additional information
    TBuf<RSat::KAdditionalInfoMaxSize> additionalInfo;
    RSat::TSendSsRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSendSsRspV2Pckg*>( aRsp );
    RSat::TSendSsRspV2& rspV2 = ( *aRspPckg ) ();
    // Get Proactive command number
    TUint8 pCmdNumber( rspV2.PCmdNumber() );

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV2.iGeneralResult ) 
        && ( RSat::KPartialComprehension != rspV2.iGeneralResult )
        && ( RSat::KMissingInformation != rspV2.iGeneralResult )
        && ( RSat::KSuccessRequestedIconNotDisplayed != rspV2.iGeneralResult )
        && ( RSat::KUssdTransactionTerminatedByUser != rspV2.iGeneralResult )
        && ( RSat::KMeUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KNetworkUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KInteractionWithCCTemporaryError != rspV2.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV2.iGeneralResult )
        && ( RSat::KCmdTypeNotUnderstood != rspV2.iGeneralResult )
        && ( RSat::KCmdDataNotUnderstood != rspV2.iGeneralResult )
        && ( RSat::KCmdNumberNotKnown != rspV2.iGeneralResult )
        && ( RSat::KSsReturnError != rspV2.iGeneralResult )
        && ( RSat::KErrorRequiredValuesMissing != rspV2.iGeneralResult )
        && ( RSat::KInteractionWithCCPermanentError != rspV2.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_TERMINALRESPONSEL_2, "CSAT:: CSatNotifySendSs::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    TBool isValidAddInfo(EFalse); // assume worse
    // check that additional info is actually present
    switch (rspV2.iGeneralResult)
    	{
    	case RSat::KSsReturnError:
    	case RSat::KNetworkUnableToProcessCmd:
        	if ( ( RSat::KSatNetworkErrorInfo == rspV2.iInfoType ) &&
        		 ( rspV2.iAdditionalInfo.Length() > 0 ) )
        		{
    			isValidAddInfo = ETrue;
                additionalInfo.Copy( rspV2.iAdditionalInfo );
        		}
    		break;
    	case RSat::KMeUnableToProcessCmd:
    	case RSat::KInteractionWithCCPermanentError:
        	if ( ( RSat::KMeProblem == rspV2.iInfoType ) &&
       		     ( rspV2.iAdditionalInfo.Length() > 0 ) )
        		{
    			isValidAddInfo = ETrue;
                additionalInfo.Copy( rspV2.iAdditionalInfo );
        		}
    		break;
    	case RSat::KSuccess:
    	case RSat::KPartialComprehension:
    	case RSat::KMissingInformation:
    	case RSat::KSuccessRequestedIconNotDisplayed:
		    if ( ( RSat::KSendSsInfo == rspV2.iInfoType ) &&
  		         ( rspV2.iAdditionalInfo.Length() > 0 ) )
	    		{
				isValidAddInfo = ETrue;
	            additionalInfo.Copy( rspV2.iAdditionalInfo );
	            // Remove operation code.
	            // SATUI appends iOperationCode to additional info.
	            // RMmCustomAPI::TSsAdditionalInfo iAddtionalInfo includes
	            // correct SS Return Result Operation code, so fisrt byte of
	            // additionalInfo is not needed.
	            additionalInfo.Delete( 0, 1 );
	    		}
		    break;
    	default:
    	    if ( RSat::KNoAdditionalInfo == rspV2.iInfoType )
    	    	{
    			isValidAddInfo = ETrue;
    	    	}
    	    else if ( rspV2.iAdditionalInfo.Length() > 0 )
	    		{
    			isValidAddInfo = ETrue;
	    		additionalInfo.Copy( rspV2.iAdditionalInfo );
	    		}
    		break;
    	}
    
	if ( !isValidAddInfo )
		{
    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_TERMINALRESPONSEL_3, "CSAT: CSatNotifyCloseChannel::TerminalResponseL Invalid Additional Info");
    	ret = KErrCorrupt;
		}
        
    // Report to call control that proactive session is over
    iNotificationsTsy->iSatTsy->StoreProactiveAddress( NULL );


	TInt response = CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
										rspV2.iGeneralResult ), additionalInfo );
	if ( KErrNone == ret )
	{
		ret = response;
	}

    return ret;
    }



// -----------------------------------------------------------------------------
// CSatNotifySendSs::CreateTerminalRespL
// Constructs SendSs specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendSs::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,        
        TUint8 aGeneralResult,      
        const TDesC16& aAdditionalInfo
		)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_CREATETERMINALRESPL_1, "CSAT:: CSatNotifySendSs::CreateTerminalRespL");
    // Create and append response data
    TTlv tlvSpecificData;
    tlvSpecificData.AddTag( KTlvResultTag );    
	// Append general result
    tlvSpecificData.AddByte( aGeneralResult );
	for (TInt i = 0; i < aAdditionalInfo.Length(); i++ )
		{
		tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[i] ) );
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
// CSatNotifySendSs::CheckCallForwarding
// This method Checks SS string if it is for call forwarding. If it is 
// append '+' character to pretending international number.
// -----------------------------------------------------------------------------
//

void CSatNotifySendSs::CheckCallForwarding
		( 
    	TPtrC8 aSource,               
    	RSat::TSsString& aSsString 
    	)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_CHECKCALLFORWARDING_1, "CSAT:: CSatNotifySendSs::CheckCallForwarding");
    _LIT8(KCFU,"*21*");    // Call forwarding unconditional (CFU)
    _LIT8(KCFB,"*67*");    // Call forwarding on Mobile Subscriber Busy(CFB)
    _LIT8(KCFNRy, "*61*"); // Call forwarding on No Reply (CFNRy)
    _LIT8(KCFNRc, "*62*"); // Call forwarding on Mobile Subscriber Not 
                           // Reachable (CFNRc)    
    // Try to find call forwarding string
    TInt pos( aSource.Find( KCFU ) );
    if ( KErrNotFound == pos )
        {
        pos = aSource.Find( KCFB );
        }
    if ( KErrNotFound == pos )
        {
        pos = aSource.Find( KCFNRy );
        }
    if ( KErrNotFound == pos )
        {
        pos = aSource.Find( KCFNRc );
        }
    if ( KErrNotFound != pos )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_CHECKCALLFORWARDING_2, "CSAT:: CSatNotifySendSs::CheckCallForwarding, String is for call forwarding");
        aSsString.iSsString.Zero();
        aSsString.iSsString.Append( aSource.Left( pos + 4 ) );
        aSsString.iSsString.Append( KPlusMarkCharacterCode );
        aSsString.iSsString.Append( 
            aSource.Right( aSource.Length() - ( pos + 4) ) );
        }
    }

// -----------------------------------------------------------------------------
// CSatNotifySendSs::CheckSsStringValidity
// This method Checks validity of SS string.If string includes undefined SS 
// characters or length is zero then return KErrCorrupt.
// -----------------------------------------------------------------------------
//	

TInt CSatNotifySendSs::CheckSsStringValidity
		(
    	TPtrC8 aSsString 
    	)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_CHECKSSSTRINGVALIDITY_1, "CSAT:: CSatNotifySendSs::CheckSsStringValidity");
    TInt ret( KErrNone );
    
    if ( !aSsString.Length() )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_CHECKSSSTRINGVALIDITY_2, "CSAT:: CSatNotifySendSs::CheckSsStringValidity, String length is NULL");
        // If length is zero
        ret = KErrCorrupt;
        }
    
    _LIT8( KDtmf,            "p");
    _LIT8( KWild,            "w");
    _LIT8( KExpansionDigit,  ".");

    // SS string can contain only digits, star '*' and dash '#' characters.
    if ( ( KErrNotFound != aSsString.Find( KDtmf ) )
        || ( KErrNotFound != aSsString.Find( KWild ) )
        || ( KErrNotFound != aSsString.Find( KExpansionDigit ) ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDSS_CHECKSSSTRINGVALIDITY_3, "CSAT:: CSatNotifySendSs::CheckSsStringValidity, Incompatible character found");
        ret = KErrCorrupt;
        }
    return ret;
    }
    
// End of file

