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
// Name        : CSatNotifySetUpCall.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Set Up call notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySetUpCallTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifySetUpCall.h"    // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifySetupCall::CSatNotifySetupCall
// Constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpCall* CSatNotifySetUpCall::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_NEWL_1, "CSAT: CSatNotifySetupCall::NewL");
    CSatNotifySetUpCall* const satNotifySetUpCall = 
        new ( ELeave ) CSatNotifySetUpCall( aNotificationsTsy );
    CleanupStack::PushL( satNotifySetUpCall );
    satNotifySetUpCall->ConstructL();
    CleanupStack::Pop( satNotifySetUpCall );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_NEWL_2, "CSAT: CSatNotifySetupCall::NewL, end of method");
    return satNotifySetUpCall;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetupCall::~CSatNotifySetupCall
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpCall::~CSatNotifySetUpCall
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_DTOR_1, "CSAT: CSatNotifySetupCall::~CSatNotifySetUpCall");
    }
        
// -----------------------------------------------------------------------------
// CSatNotifySetupCall::CSatNotifySetupCall
// Constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpCall::CSatNotifySetUpCall
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy ), iCCResult( 0 )
    {
    // None
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySetupCall::CSatNotifySetupCall
// Constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySetUpCall::ConstructL
        ( 
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_CONSTRUCTL_1, "CSAT: CSatNotifySetupCall::ConstructL");
    iCallConnectedEvent.Zero();
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySetupCall::Notify
// Save the request handle type for notification requested by ETel server
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySetUpCall::Notify
        ( 
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_NOTIFY_1, "CSAT: CSatNotifySetupCall::Notify");
    // Save data pointer to client side for completion
    iSetUpCallV1Pckg = reinterpret_cast<RSat::TSetUpCallV1Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySetUpCallPCmdReqType );   
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_NOTIFY_2, "CSAT: CSatNotifySetUpCall::Notify\t Handle:%d",aTsyReqHandle);
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSetUpCall );   

    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CSatNotifySetupCall::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySetUpCall request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpCall::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_CANCELNOTIFICATION_1, "CSAT: CSatNotifySetUpCall::CancelNotification");
    // Reset the request handle 
    iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifySetUpCallPCmdReqType );
    // Reset the data pointer
    iSetUpCallV1Pckg = NULL;    
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;      
    }

// -----------------------------------------------------------------------------
// CSatNotifySetupCall::CompleteNotifyL
// This method completes an outstanding asynchronous 
// SetUpCall request. 
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySetUpCall::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage,
        TInt aErrorCode  
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_1, "CSAT: CSatNotifySetupCall::CompleteNotifyL");
    TInt returnValue( KErrNone );
    TInt ret( KErrNone );
    iCCResult = 0;
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
	TBuf<1> additionalInfo;
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifySetUpCallPCmdReqType );
    
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
                
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_2, "CSAT: CSatNotifySetUpCall::CompleteNotifyL\t Handle:%d",reqHandle);
    // In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown  != reqHandle )
        {
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode )
            {
            // Fill the Set Up Call structure
            RSat::TSetUpCallV1& setUpCallV1 = ( *iSetUpCallV1Pckg )();

            // Transaction ID
            setUpCallV1.SetPCmdNumber( pCmdNumber );
            
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_3, "CSAT: CSatNotifySetupCall::CompleteNotifyL CommandQualifier %d", iNotificationsTsy->iTerminalRespData.iCommandDetails[ KCommandQualifier ]);

            switch ( iNotificationsTsy->iTerminalRespData.
            		iCommandDetails[ KCommandQualifier ] )
                {
                case KOnlyIfNotBusy:
                    {
                    // Set up call, if not busy on another call
                    setUpCallV1.iType = RSat::EOnlyIfNotBusy;
                    break;
                    }
                case KOnlyIfNotBusyWithRedial:
                    {
                    // Set up call, if not busy on another call, with redial
                    setUpCallV1.iType = RSat::EOnlyIfNotBusyWithRedial;
                    break;
                    }
                case KHoldOtherCalls:
                    {
                    // Set up call, put other calls (if any) on hold
                    setUpCallV1.iType = RSat::EHoldOtherCalls;
                    break;
                    }
                case KHoldOtherCallsWithRedial:
                    {
                    // Set up call, put other calls (if any) on hold, with 
                    // redial
                    setUpCallV1.iType = RSat::EHoldOtherCallsWithRedial;
                    break;
                    }
                case KDisconnectOtherCalls:
                    {
                    // Set up call, disconnect other calls (if any)
                    setUpCallV1.iType = RSat::EDisconnectOtherCalls;
                    break;
                    }
                case KDisconnectOtherCallsWithRedial:
                    {
                    // Set up call, disconn. other calls (if any), with redial
                    setUpCallV1.iType = RSat::EDisconnectOtherCallsWithRedial;
                    break;
                    }
                default:
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_4, "CSAT: CSatNotifySetupCall::CompleteNotifyL,Call type not set");
                    // Call type not set
                    setUpCallV1.iType = RSat::ESetUpCallTypeNotSet;
                    break;
                    }
                }    

            TPtrC8 sourceString; // Used with conversions

            // Alpha id string (optional)
            setUpCallV1.iAlphaIdConfirmationPhase.iAlphaId.Zero();

             // Call setup phase alphaid (optional)
            setUpCallV1.iAlphaIdCallSetUpPhase.iAlphaId.Zero();

            CTlv alphaIdentifier;
            // Check alpha identifiers (for user confirmation phase and call set 
            // up phase )
            returnValue = berTlv.TlvByTagValue( &alphaIdentifier, 
                                                KTlvAlphaIdentifierTag );
            // If alpha id string exist
            if ( KErrNone == returnValue ) 
                {
                // First alpha identifier is for user confirmation phase
                if ( alphaIdentifier.GetLength() )
                    {
                    // Set status
                    setUpCallV1.iAlphaIdConfirmationPhase.iStatus =
                        RSat::EAlphaIdProvided;

                     // Get alpha id string
                    sourceString.Set( alphaIdentifier.GetData( 
                        ETLV_AlphaIdentifier ) );
                    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_5, "CSAT: CSatNotifySetupCall::CompleteNotifyL User confirmation phase: sourceString %s", sourceString );
                    // Convert and set the alpha id
                    TSatUtility::SetAlphaId( sourceString ,
                        setUpCallV1.iAlphaIdConfirmationPhase.iAlphaId ); 
                
                    }
                // Alpha id found for user confirmation phase, but length 
                // is zero
                else
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_6, "CSAT: CSatNotifySetupCall::CompleteNotifyL User confirmation phase: Alpha Id missing");
                    setUpCallV1.iAlphaIdConfirmationPhase.iStatus = 
                        RSat::EAlphaIdNull;
                    }
                }
            // No alpha id for user confirmation phase
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_7, "CSAT: CSatNotifySetupCall::CompleteNotifyL, User confirmation phase: Alpha ID not present");
                setUpCallV1.iAlphaIdConfirmationPhase.iStatus = 
                    RSat::EAlphaIdNotPresent;
                }  

            // Get alpha identifier for call set up phase
            returnValue = berTlv.TlvByTagValue( &alphaIdentifier, 
                                                KTlvAlphaIdentifierTag, 1 );
            // If alpha id string exist
            if ( KErrNone == returnValue )
                {
                // Second alpha identifier is for call set up phase 
                if ( alphaIdentifier.GetLength() )
                    {
                    // Set status
                    setUpCallV1.iAlphaIdCallSetUpPhase.iStatus = 
                        RSat::EAlphaIdProvided;
                     // Get alpha id string
                    sourceString.Set( alphaIdentifier.GetData( 
                        ETLV_AlphaIdentifier ) );
                    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_8, "CSAT: CSatNotifySetupCall::CompleteNotifyL Call Set Up phase: sourceString %s",sourceString );
                    // Convert and set the alpha id
                    TSatUtility::SetAlphaId( sourceString,
                        setUpCallV1.iAlphaIdCallSetUpPhase.iAlphaId ); 

                    }
                // Alpha id found for set up call phase,  but length is zero
                else
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_9, "CSAT: CSatNotifySetupCall::CompleteNotifyL Call Set Up phase: Alpha Id is NULL");
                    setUpCallV1.iAlphaIdCallSetUpPhase.iStatus = 
                        RSat::EAlphaIdNull;
                    }
                }
   
            // No alpha id set up call phase
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_10, "CSAT: CSatNotifySetupCall::CompleteNotifyL, Alpha ID not present");
                setUpCallV1.iAlphaIdCallSetUpPhase.iStatus = 
                    RSat::EAlphaIdNotPresent;
                }  

            // First icon for user confirmation phase
            TSatUtility::FillIconStructure( 
                berTlv, setUpCallV1.iIconIdConfirmationPhase, 0 );

            // Second icon for set up call phase
            TSatUtility::FillIconStructure( 
                berTlv, setUpCallV1.iIconIdCallSetUpPhase, 1 );

            // Expecting address
            CTlv addressTlv;
            returnValue = berTlv.TlvByTagValue( &addressTlv, 
                                                KTlvAddressTag );
            if ( KErrNone == returnValue && addressTlv.GetLength() )
                {
                // Initialize ton and npi
                RSat::TTypeOfNumber ton( RSat::EUnknownNumber );
                RSat::TNumberingPlan npi( RSat::EUnknownNumberingPlan );

                // Call utility function that maps received TON and NPI to 
                // RSat values
                TSatUtility::TonAndNpi( 
                    addressTlv.GetShortInfo( ETLV_TonAndNpi ) , &ton, &npi );
            
                // Set TON and NPI
                setUpCallV1.iAddress.iTypeOfNumber = ton;
                setUpCallV1.iAddress.iNumberPlan = npi;
            
                // Address
                setUpCallV1.iAddress.iTelNumber.Zero();
                // Length is number of BCD characters, 
                // multiply by two to get real length and - 2 for ton&npi
                TUint8 dialNumberLength( (TUint8) ( ( 
                    addressTlv.GetLength() * 2 ) - 2 ) );
            
                if ( dialNumberLength )
                    {   
                    // Semi-octet presentation used
                    sourceString.Set( addressTlv.GetData( 
                        ETLV_DiallingNumberString ) );

                    TBuf8<RSat::KMaxMobileTelNumberSize> tempNumber;
                    // Convert BCD string to ASCII        
                    TSatUtility::BCDToAscii( sourceString, tempNumber );
            
                    sourceString.Set( tempNumber );

                    // Add '+' character to the preceding of international number
                    if ( ( RSat::EInternationalNumber == 
                    	setUpCallV1.iAddress.iTypeOfNumber )
                            && (KPlusMarkCharacterCode != sourceString[0] )
                            && ( RSat::KMaxMobileTelNumberSize > dialNumberLength ) )
                        {
                        _LIT8( KPlusMark, "+");
                        tempNumber.Insert( 0, KPlusMark );
                        sourceString.Set( tempNumber );
                        }

                    // The command may also include DTMF digits, which the ME shall
                    // send to the network after the call has connected. 
                    // Remove wild and expansion digits.
                    TSatUtility::RemoveWildAndExpansionDigit( sourceString, 
                        tempNumber );
                    // Set actual number
                    setUpCallV1.iAddress.iTelNumber.Copy( tempNumber );

                    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_11, "CSAT: CSatNotifySetUpCall::CompleteNotifyL, TelNumber: %S", setUpCallV1.iAddress.iTelNumber );
                    	
                    iNotificationsTsy->iSatTsy->StoreProactiveAddress( 
                        &tempNumber );
                    }
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_12, "CSAT: CSatNotifySetupCall::CompleteNotifyL, Request not ongoing");
                additionalInfo.Zero();
                additionalInfo.Append( KNoCause );
                // Request not on, returning response immediately
                CreateTerminalRespL( pCmdNumber, RSat::KCmdDataNotUnderstood,
                    additionalInfo ); 
                ret = KErrCorrupt;
                }

            // Bearer capability
            setUpCallV1.iCapabilityConfigParams.Zero();
            CTlv capabilityConfig;
            returnValue = berTlv.TlvByTagValue( &capabilityConfig, 
                KTlvCapabilityConfigurationParametersTag );

            if ( ( KErrNone == returnValue )
                && ( capabilityConfig.GetLength() ) )
                {
                // iCapabilityConfigParams is 8-bit string
                setUpCallV1.iCapabilityConfigParams.Append( capabilityConfig.
                    GetData( ETLV_CapabilityConfigurationParameters ) );

                OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_13, "CSAT: CSatNotifySetUpCall::CompleteNotifyL, CapabilityParams: %s", setUpCallV1.iCapabilityConfigParams );
                }

            // Called Party SubAddress
            setUpCallV1.iSubAddress.Zero();
            CTlv subAddress;
            returnValue = berTlv.TlvByTagValue( &subAddress, KTlvSubaddressTag );
        
            if ( ( KErrNone == returnValue )
                && ( subAddress.GetLength() ) )
                {   
                // Source is 8-bit string, converting to 16-bit
                sourceString.Set( subAddress.GetData( ETLV_SubAddress) );
                // The Authority and Format Identifier code 50 (in BCD) 
                // indicates that the subaddress consists of IA5 characters
                if ( KAuthorityFormatIdentifier == sourceString[1] )
                    {
                    TInt i( 0 );
                    for ( i = 2; i < sourceString.Length(); i++ )
                        {
                        // IA5 character as defined in ITU-T Recommendation 
                        // T.50/ISO 646 and then encoded into two semi-octets 
                        // according to the "preferred binary encoding" 
                        // defined in X.213/ISO 8348 AD2.
                        setUpCallV1.iSubAddress.Append( KZeroCharacterCode + 
                            ( ( sourceString[i] & 0xF0  ) >> 4 ) );
                        setUpCallV1.iSubAddress.Append( KZeroCharacterCode + 
                            ( sourceString[i] & 0x0F ) );
                        }
                    }
                else
                    {

                    TSatUtility::Copy8to16LE( sourceString, 
                        setUpCallV1.iSubAddress );
                    }
                
                OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_14, "CSAT: CSatNotifySetUpCall, SubAddress: %S", setUpCallV1.iSubAddress );
                }

            // Duration ( optional, maximum duration for the redial mechanism )
            CTlv callDuration;
            returnValue = berTlv.TlvByTagValue( &callDuration, 
                                                KTlvDurationTag );
            if ( KErrNone == returnValue )
                {
                TInt durationTimeUnit( callDuration.GetShortInfo( 
                    ETLV_TimeUnit ) );
                switch ( durationTimeUnit )
                    {
                    case KMinutes:
                        {
                        setUpCallV1.iDuration.iTimeUnit = RSat::EMinutes;
                        break;
                        }
                    case KSeconds:
                        {
                        setUpCallV1.iDuration.iTimeUnit = RSat::ESeconds;
                        break;
                        }
                    case KTenthsOfSeconds:
                        {
                        setUpCallV1.iDuration.iTimeUnit = 
                            RSat::ETenthsOfSeconds;
                        break;
                        }
                    default:
                        {
                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_15, "CSAT: CSatNotifySetupCall::CompleteNotifyL, time unit not set");
                        setUpCallV1.iDuration.iTimeUnit = 
                            RSat::ETimeUnitNotSet;
                        break;
                        }
                    }
            
                // Time interval, valid range 1-255 (0x01 - 0xFF)
                setUpCallV1.iDuration.iNumOfUnits = 
                    callDuration.GetShortInfo( ETLV_TimeInterval );

                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_16, "CSAT: CSatNotifySetUpCall::CompleteNotifyL Max Duration for the Redial Mechanism" );

                // Check that Time interval value is not invalid
                if ( NULL == setUpCallV1.iDuration.iNumOfUnits )
                    {
                    // Set value to valid range
                    setUpCallV1.iDuration.iNumOfUnits = 0x1;    
                    }
                }
            // Set SetUpCall flag on for Call connected event. To avoid situation
            // that event is send before terminal response
            if ( KErrNone == ret )
                {
                iNotificationsTsy->iSatTsy->SetSetUpCallStatus( ETrue );
                }
            
            } // if ( KErrNone == aErrorCode )
        else
        	{
        	ret = aErrorCode;
        	}
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
        }        
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_COMPLETENOTIFYL_17, "CSAT: CSatNotifySetupCall::CompleteNotifyL, Request not ongoing");
        additionalInfo.Zero();
        additionalInfo.Append( KNoCause );
        // Request not on, returning response immediately
        CreateTerminalRespL( pCmdNumber, 
                RSat::KMeUnableToProcessCmd,
                additionalInfo ); 
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetupCall::TerminalResponseL
// Called by ETel server, passes terminal response to SIM card
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpCall::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_TERMINALRESPONSEL_1, "CSAT:CSatNotifySetUpCall::TerminalResponseL");

    TInt    ret( KErrNone );
    TBuf<1> additionalInfo;

    RSat::TSetUpCallRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSetUpCallRspV2Pckg*>( aRsp );
    RSat::TSetUpCallRspV2& rspV2 = ( *aRspPckg ) ();

    // Proactive command number
    TUint8 pCmdNumber( rspV2.PCmdNumber() );

    // Check that general result values are valid
    if ( ( RSat::KSuccess != rspV2.iGeneralResult ) 
        && ( RSat::KSuccessRequestedIconNotDisplayed != rspV2.iGeneralResult )
        && ( RSat::KPSessionTerminatedByUser != rspV2.iGeneralResult )
        && ( RSat::KMeUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KNetworkUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KPCmdNotAcceptedByUser != rspV2.iGeneralResult )
        && ( RSat::KCallClearedBeforeConnectionOrReleased 
            != rspV2.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities  != rspV2.iGeneralResult )
        && ( RSat::KSsReturnError != rspV2.iGeneralResult )
        && ( RSat::KErrorRequiredValuesMissing != rspV2.iGeneralResult ) 
        && ( RSat::KInteractionWithCCPermanentError != rspV2.iGeneralResult) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_TERMINALRESPONSEL_2, "CSAT:CSatNotifySetUpCall::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Entity) error or network error,
    // additional info is needed
    if ( ( RSat::KSatNetworkErrorInfo == rspV2.iInfoType )
        || ( RSat::KMeProblem == rspV2.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV2.iAdditionalInfo.Length() )
            {
            additionalInfo.Zero();
            additionalInfo.Append( static_cast<TUint8>( 
                rspV2.iAdditionalInfo[0] ) );
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_TERMINALRESPONSEL_3, "CSAT:CSatNotifySetUpCall::TerminalResponseL, Invalid Additional Info");
            ret = KErrCorrupt;
            }
        }
        
    iNotificationsTsy->iSatTsy->StoreProactiveAddress( NULL );

    // Creating the terminal response message
    CreateTerminalRespL( pCmdNumber, 
    	static_cast<TUint8>( rspV2.iGeneralResult ), additionalInfo );  

    // Clear flag
    iNotificationsTsy->iSatTsy->SetSetUpCallStatus( EFalse );
    
    // In the case of a call initiated through a SET UP CALL proactive command
    // while the call connected event is part of the current event list, the 
    // ME shall send both the TERMINAL RESPONSE related to the proactive 
    // command, and the EVENT DOWNLOAD command, in the order TERMINAL RESPONSE 
    // first, ENVELOPE(EVENT DOWNLOAD - call connected) second. 

    // Check if there is stored envelope in the buffer
    if ( iCallConnectedEvent.Length() )
        {
        // Pack data
        CSatDataPackage dataPackage;
	    dataPackage.PackData( &iCallConnectedEvent);
        // Send envelope..
        iNotificationsTsy->iSatTsy->MessageManager()->
            HandleRequestL(ESatNotifySetUpCallPCmd, &dataPackage );
        // Clear the buffer
        iCallConnectedEvent.Zero();
        }
        
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetupCall::CreateTerminalRespL
// Constructs SetUpCall specific part of terminal response and calls 
// DOS to send the actual message. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpCall::CreateTerminalRespL
        ( 
        TUint8  aPCmdNumber,         
        TUint8  aGeneralResult,      
        TDesC16& aAdditionalInfo            
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_CREATETERMINALRESPL_1, "CSAT: CSatNotifySetUpCall::CreateTerminalRespL");
    // Create and append response data
    TTlv tlvSpecificData;
    tlvSpecificData.AddTag( KTlvResultTag );
    // Result
    tlvSpecificData.AddByte( aGeneralResult );
    
    if ( ( RSat::KMeUnableToProcessCmd == aGeneralResult ) ||
         ( RSat::KSuccessRequestedIconNotDisplayed == aGeneralResult ) || 
         ( RSat::KNetworkUnableToProcessCmd == aGeneralResult ) || 
         ( RSat::KInteractionWithCCPermanentError == aGeneralResult ) )
        {
        if ( aAdditionalInfo.Length() )
            {
            tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[0] ) );
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_CREATETERMINALRESPL_2,  "CSAT: CSatNotifySetUpCall::CreateTerminalRespL aAdditionalInfo.Length() == 0" );
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


// -----------------------------------------------------------------------------
// CSatNotifySetupCall::StoreCallConnectedEvent
// Store a Call connected envelope to buffer
// -----------------------------------------------------------------------------
//
void CSatNotifySetUpCall::StoreCallConnectedEvent
        ( 
        const TDesC8& aEnvelope
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPCALL_STORECALLCONNECTEDEVENT_1, "CSAT: CSatNotifySetUpCall::StoreCallConnectedEvent");
    // Store it to the buffer
    iCallConnectedEvent = aEnvelope;
    } 

// End of file
