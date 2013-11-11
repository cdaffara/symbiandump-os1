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
// Name        : CSatNotifySendUssd.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// SendUssd notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySendUssdTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifySendUssd.h"     // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifySendUssd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySendUssd* CSatNotifySendUssd::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_NEWL_1, "CSAT: CSatNotifySendUssd::NewL");
   	CSatNotifySendUssd* const satNotifySendUssd = 
        new ( ELeave ) CSatNotifySendUssd( aNotificationsTsy );
    CleanupStack::PushL( satNotifySendUssd );
    satNotifySendUssd->ConstructL();
    CleanupStack::Pop( satNotifySendUssd );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_NEWL_2, "CSAT: CSatNotifySendUssd::NewL, end of method");
    return satNotifySendUssd;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendUssd::~CSatNotifySendUssd
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendUssd::~CSatNotifySendUssd
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_DTOR_1, "CSAT: CSatNotifySendUssd::~CSatNotifySendUssd");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySendUssd::CSatNotifySendUssd
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendUssd::CSatNotifySendUssd
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendUssd::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySendUssd::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_CONSTRUCTL_1, "CSAT: CSatNotifySendUssd::ConstructL");
    }
   
// -----------------------------------------------------------------------------
// CSatNotifySendUssd::Notify
// This request allows a client to be notified of a SEND USSD proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendUssd::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_NOTIFY_1, "CSAT: CSatNotifySendUssd::Notify");

    // Save data pointer to client side for completion
    iSendUssdV1Pckg = reinterpret_cast<RSat::TSendUssdV1Pckg*>( 
        aPackage.Des1n() );
    
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySendUssdPCmdReqType );

    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSendUssd );   

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendUssd::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySendUssd request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendUssd::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_CANCELNOTIFICATION_1, "CSAT: CSatNotifySendUssd::CancelNotification");
    
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifySendUssdPCmdReqType );
	// Reset the data pointer
	iSendUssdV1Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySendUssd::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifySendUssd request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendUssd::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_COMPLETENOTIFYL_1, "CSAT: CSatNotifySendUssd::CompleteNotifyL");
	TInt ret( KErrNone );
	TInt returnValue( KErrNone );
	TBuf<1> additionalInfo;
	TBuf<1> emptyText;	
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
		iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifySendUssdPCmdReqType );
        	
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
			// Fill the Send Ussd structure             
			RSat::TSendUssdV1& sendUssdV1 = ( *iSendUssdV1Pckg )();			
			// Store command number
			sendUssdV1.SetPCmdNumber( pCmdNumber );
			                   
			TPtrC8 sourceStr; // Used with Copy8to16 function

			// Alpha id string (optional)
			sendUssdV1.iAlphaId.iAlphaId.Zero();
			CTlv alphaIdentifier;
			returnValue = berTlv.TlvByTagValue( &alphaIdentifier, 
				KTlvAlphaIdentifierTag );
				
			if ( KErrNotFound != returnValue )
				{
				TUint16 alphaIdLength = alphaIdentifier.GetLength() ;
				if ( RSat::KAlphaIdMaxSize < alphaIdLength )
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_COMPLETENOTIFYL_2, "CSAT: CSatNotifySendUssd::CompleteNotifyL, Alpha ID length exceeded");
					// String too long
					additionalInfo.Zero();
                    additionalInfo.Append( KNoCause );
					CreateTerminalRespL( pCmdNumber,
						RSat::KMeUnableToProcessCmd, additionalInfo, 
						emptyText );
					ret = KErrCorrupt;
					}
				else if ( alphaIdLength )
					{
					// get the alpha id
					sourceStr.Set( alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
					// convert and set the alpha id
					TSatUtility::SetAlphaId( sourceStr , 
						sendUssdV1.iAlphaId.iAlphaId ); 
					}

				// Check alpha id status
				// Alpha Tag present
				if ( sendUssdV1.iAlphaId.iAlphaId.Length() )
					{
					sendUssdV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_COMPLETENOTIFYL_3, "CSAT: CSatNotifySendUssd::CompleteNotifyL, Alpha ID is NULL");
					sendUssdV1.iAlphaId.iStatus = RSat::EAlphaIdNull;
					}  
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_COMPLETENOTIFYL_4, "CSAT: CSatNotifySendUssd::CompleteNotifyL, Alpha ID not present");
				sendUssdV1.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
				}


			// Ussd string (mandatory)
			CTlv ussdTlv;
			sendUssdV1.iUssdString.iUssdString.Zero();
			returnValue = berTlv.TlvByTagValue( &ussdTlv, 
				KTlvUssdStringTag );

			if ( KErrNone == returnValue )
				{
	            // Get the data coding scheme from the ISI msg
	            // and set the corresponding ETel Sat data field.
	            // The DCS is coded as for Cell Broadcast.
	            sendUssdV1.iUssdString.iDcs = 
	                ussdTlv.GetShortInfo( ETLV_DataCodingScheme );
	                
	            // Decode DCS
	            TSmsDcs decodedDcs( ESmsUnknownOrReservedDcs );
	            decodedDcs = TSatUtility::DecodeCbsDcs( 
	            	sendUssdV1.iUssdString.iDcs );
	            
	            TPtrC8 ussdString = ussdTlv.GetData( ETLV_UssdString ); 
	            TUint16 ussdStringLengthInBytes = (TUint16) ( 
	                ussdString.Length() );
	            if ( (  ( ESms16BitDcs==decodedDcs ) 
	                   && 2*RSat::KStringMaxSize<ussdStringLengthInBytes)
	               || ( ( ESms8BitDcs==decodedDcs ) 
	                  && RSat::KStringMaxSize<ussdStringLengthInBytes)
	               || ( ( ESms7BitDcs==decodedDcs ) 
	                  && RSat::KStringMaxSize<8*ussdStringLengthInBytes/7))
                	{
                	// The Ussd text string is too long.
                	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_COMPLETENOTIFYL_5, "CSAT:CSatNotifySendUssd::CompleteNotifyL, USSD String too long");
					// Text string too long
					ret = KErrCorrupt;
					additionalInfo.Zero();
					CreateTerminalRespL( pCmdNumber, 
					    RSat::KCmdDataNotUnderstood, additionalInfo, 
						emptyText);   
					}
				else
					{
					
	                // Conversion to 16-bit following the DCS
	                switch ( decodedDcs )
	                    {
	                    case ESms7BitDcs:
	                        {
	                        TBuf8<RSat::KStringMaxSize> ussdString8;
	                        TSatUtility::Packed7to8Unpacked( ussdString, 
	                            ussdString8 );
	                        TSatUtility::Convert7BitToUnicode16( ussdString8,
	                            sendUssdV1.iUssdString.iUssdString );
	                        break;
	                        }
	                    case ESms8BitDcs:
	                        {
	                        TSatUtility::Convert7BitToUnicode16( ussdString,
	                            sendUssdV1.iUssdString.iUssdString );	                        
	                        break;
	                        }
	                    case ESms16BitDcs:
	                        {
	                        TSatUtility::Copy8to16LE( ussdString , 
	                        	sendUssdV1.iUssdString.iUssdString );
	                        break;
	                        }
	                    default:
	                        {
	                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_COMPLETENOTIFYL_6, "CSAT:CSatNotifySendUssd::CompleteNotifyL, USSD DCS has a reserved value");
	                        // The DCS has a reserved value
	                        ret = KErrCorrupt;
							additionalInfo.Zero();
							CreateTerminalRespL( pCmdNumber,
								RSat::KCmdDataNotUnderstood, additionalInfo,
								emptyText );  
	                        break;
	                        }     
	                    }
					}
				}
			else
				{
				// Mandatory field missing
				ret = KErrCorrupt;
				additionalInfo.Zero();
				CreateTerminalRespL(
					pCmdNumber, RSat::KErrorRequiredValuesMissing,
					additionalInfo, emptyText );  
				}

        	if ( KErrNone == ret )
            	{
				// Iconid 
				TSatUtility::FillIconStructure( berTlv, 
					sendUssdV1.iIconId );
            	}
            	
		    } // End of if (KErrNone == aErrorCode)
        else
        	{
        	ret = aErrorCode;
        	}

        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );

        } // End of if ( reqHandle != CSatTsy::ESatReqHandleUnknown )	
	else 
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_COMPLETENOTIFYL_7, "CSAT: CSatNotifySendUssd::CompleteNotifyL, Request not ongoing");
        // Request not on, returning response immediately
        additionalInfo.Zero();
        additionalInfo.Append( KNoCause );
		CreateTerminalRespL(
			pCmdNumber, RSat::KMeUnableToProcessCmd,
			additionalInfo, emptyText);  
		}

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifySendUssd::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendUssd::TerminalResponseL
        ( 
        TDes8* aRsp 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_TERMINALRESPONSEL_1, "CSAT: CSatNotifySendUssd::TerminalResponseL");
	
    TInt ret( KErrNone );
    TBuf<RSat::KAdditionalInfoMaxSize> additionalInfo;
    RSat::TSendUssdRspV1Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSendUssdRspV1Pckg*>( aRsp );
    RSat::TSendUssdRspV1& rspV1 = ( *aRspPckg ) ();
    // Get Proactive command number
    TUint8 pCmdNumber( rspV1.PCmdNumber() );
    // Check that general result values are valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult )
        && ( RSat::KPartialComprehension != rspV1.iGeneralResult )
        && ( RSat::KMissingInformation != rspV1.iGeneralResult )
        && ( RSat::KSuccessRequestedIconNotDisplayed != rspV1.iGeneralResult )
        && ( RSat::KModifiedByCallControl != rspV1.iGeneralResult )
        && ( RSat::KUssdTransactionTerminatedByUser != rspV1.iGeneralResult )
        && ( RSat::KInteractionWithCCTemporaryError != rspV1.iGeneralResult )
        && ( RSat::KCmdTypeNotUnderstood != rspV1.iGeneralResult )
        && ( RSat::KCmdDataNotUnderstood != rspV1.iGeneralResult )
        && ( RSat::KCmdNumberNotKnown != rspV1.iGeneralResult )
        && ( RSat::KErrorRequiredValuesMissing != rspV1.iGeneralResult )
        && ( RSat::KInteractionWithCCPermanentError != rspV1.iGeneralResult )     
        && ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult )
        && ( RSat::KNetworkUnableToProcessCmd != rspV1.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult )
        && ( RSat::KUssdReturnError != rspV1.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_TERMINALRESPONSEL_2, "CSAT: CSatNotifySendUssd::CompleteNotifyL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Entity) error, network error or text string,
    // additional info is needed
    additionalInfo.Zero();
    if ( ( RSat::KMeProblem == rspV1.iInfoType )
        || ( RSat::KCallControlRequestedAction == rspV1.iInfoType )
        || ( RSat::KSatNetworkErrorInfo == rspV1.iInfoType )
        || ( RSat::KTextString == rspV1.iInfoType ) )
        {
        // Check the length of additional info:
        if ( 0 == rspV1.iAdditionalInfo.Length() )
            {
            // No info
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_TERMINALRESPONSEL_3, "CSAT:CSatNotifySendUssd::TerminalResponseL, AdditionalInfoType set, but no additional info available");
            ret = KErrCorrupt;
            }
        else if ( RSat::KTextString == rspV1.iInfoType )
            {
            // Text string - additional info for a 
            // successful GET INKEY, GET INPUT or SEND USSD.
            // --> Not used by SAT Server when the command has been
            // performed successfully, SAT Server uses the 
            // rspV1.iUssdString.iUssdString to return the USSD string
            // sent by the network.
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_TERMINALRESPONSEL_4, "CSAT:CSatNotifySendUssd::TerminalResponseL, AdditionalInfoType set to TextString.");
            }            
        else
            {
            additionalInfo.Zero();          
            additionalInfo.Append( rspV1.iAdditionalInfo[0] );
            }
        }
    
    TInt response = CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( rspV1.iGeneralResult ), 
        additionalInfo, rspV1.iUssdString.iUssdString,                            
        rspV1.iUssdString.iDcs );                    

    if( KErrNone == ret)
    	{
    	ret = response;
    	}
    
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifySendUssd::CreateTerminalRespL
// Constructs SendUssd specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendUssd::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,      
        TDesC16& aAdditionalInfo,	
        TDesC16& aTextString,       
		TUint8 aDcs		
		)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_CREATETERMINALRESPL_1, "CSAT: CSatNotifySendUssd::CreateTerminalRespL");
    TTlv tlvSpecificData;
    TBuf8<RSat::KStringMaxSize> string;

    tlvSpecificData.AddTag( KTlvResultTag );
    tlvSpecificData.AddByte( aGeneralResult );

    if( !iNotificationsTsy->CommandPerformedSuccessfully( aGeneralResult ) )
    	{
	    // For the general results '20', '21', '37', '38' it is mandatory for the 
	    // ME to provide a specific cause value as additional info
	    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult ) || 
	         ( RSat::KNetworkUnableToProcessCmd == aGeneralResult ) || 
	         ( RSat::KUssdReturnError == aGeneralResult) || 
	         ( RSat::KMultipleCardCmdsError == aGeneralResult) )
	        {
	        for ( TInt i = 0; i < aAdditionalInfo.Length(); i++ )
	            {
	            tlvSpecificData.AddByte( ( TUint8 ) ( 
	            	aAdditionalInfo[i] & 0x00FF ) );
	            }
	        }
        }
    else
        {
        // Append received Ussd text string, with a Text string tag
        tlvSpecificData.AddTag( KTlvTextStringTag );  
                              
        // Decode DCS
        // Remains to know if SatServer sets the general result to
        // KUssdReturnError when the DCS has a reserved value. 
        TSmsDcs decodedDcs( ESmsUnknownOrReservedDcs );
        decodedDcs = TSatUtility::DecodeCbsDcs( aDcs ); 
        //Data Coding Scheme for Text String
        tlvSpecificData.AddByte( (TUint8)decodedDcs );        
        
        switch ( decodedDcs )
            {
            case ESms7BitDcs:
                {
                // Convert the Response string (which comes in the form of 
                // a TBuf (unicode)) to 7-bit gsm format.                      
                TSatUtility::UCSToPacked7( aTextString, string );                                
                break;
                }
            case ESms8BitDcs:            
                {
                // Convert the Response string (which comes in the form of 
                // a TBuf (unicode)) to 7-bit gsm format.      
                TSatUtility::ConvertUnicode16To7Bit( aTextString, string );
                break;
                }
            case ESms16BitDcs:
                {
                TSatUtility::Copy16to8LE( aTextString, string );
                break;
                }
            default:
                {
                // Reserved
                // The general result should in fact prevent reaching this branch of
                // the switch.
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDUSSD_CREATETERMINALRESPL_2, "TSY:CSatMessHandler::SendUssdTerminalRespL, The DCS sent by the network has a reserved value. The general result should have been set to UssdReturnError");
                break;
                }                        
            }                   
        // Text
        tlvSpecificData.AddData( string );       
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
    
// End of File
