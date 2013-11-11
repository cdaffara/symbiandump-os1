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
// Name        : CSatNotifyGetInput.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Get Input notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyGetInputTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyGetInput.h"     // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyGetInput* CSatNotifyGetInput::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_NEWL_1, "CSAT: CSatNotifyGetInput::NewL");
   	CSatNotifyGetInput* const satNotifyGetInput = 
        new ( ELeave ) CSatNotifyGetInput( aNotificationsTsy );
    CleanupStack::PushL( satNotifyGetInput );
    satNotifyGetInput->ConstructL();
    CleanupStack::Pop( satNotifyGetInput );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_NEWL_2, "CSAT: CSatNotifyGetInput::NewL, end of method");
    return satNotifyGetInput;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInput::~CSatNotifyGetInput
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyGetInput::~CSatNotifyGetInput
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_DTOR_1, "CSAT: CSatNotifyGetInput::~CSatNotifyGetInput");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyGetInput::CSatNotifyGetInput
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyGetInput::CSatNotifyGetInput
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyGetInput::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_CONSTRUCTL_1, "CSAT: CSatNotifyGetInput::ConstructL");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyGetInput::Notify
// Save the request handle type for notification requested by ETel server
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInput::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_NOTIFY_1, "CSAT: CSatNotifyGetInput::Notify");
    // Save data pointer to client side for completion
    iGetInputV1Pckg = reinterpret_cast<RSat::TGetInputV1Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyGetInputPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KGetInput );   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInput::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyNotifyGetInput request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInput::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {   
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_CANCELNOTIFICATION_1, "CSAT: CSatNotifyGetInput::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
		ResetTsyReqHandle( 
			CSatTsy::ESatNotifyGetInputPCmdReqType );
	// Reset the data pointer
	iGetInputV1Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyGetInput::CompleteNotifyL
// Complete Sim Session End notification to the client
// -----------------------------------------------------------------------------
// 
TInt CSatNotifyGetInput::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage, 
        TInt aErrorCode  
        ) 
    {   
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_COMPLETENOTIFYL_1, "CSAT: CSatNotifyGetInput::CompleteNotifyL");
    TInt returnValue( KErrNone );
    TInt ret( KErrNone );
    // Unpack parameters
    TPtrC8* data;  	
	aDataPackage->UnPackData( &data );    
    TBuf<1> additionalInfo;
	TBuf<1> emptyText;
	// Reset req handle. Returns the deleted req handle
	TTsyReqHandle reqHandle = 
		iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle(
			CSatTsy::ESatNotifyGetInputPCmdReqType );
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );

    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( commandDetails.Data() );

    // Get command qualifier
    TUint8 cmdQualifier( commandDetails.GetShortInfo( 
        ETLV_CommandQualifier ) );
    
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
            
    // In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode )
            {
            // Fill the get input structure
            RSat::TGetInputV1& getInputV1 = ( *iGetInputV1Pckg )();

            getInputV1.SetPCmdNumber( pCmdNumber );
    
            if ( KUserInputMask & cmdQualifier )
                {
				// User input hidden
				getInputV1.iInputDisplayOption = RSat::EHideUserInput;
				}
            else
				{
				// User input shown
				getInputV1.iInputDisplayOption = RSat::EDisplayUserInput;
				}    
			
				
            // Character set selected
            if ( KUCS2AlphabetMask & cmdQualifier )
				{
                // UCS2 alphabet selected
                getInputV1.iRspFormat = RSat::EUcs2Alphabet;
                }
            else if ( KAlphabetMask & cmdQualifier )
				{
                if ( KGetInputSmsPackedMask & cmdQualifier )
					{
					// SMS default alphabet
					getInputV1.iRspFormat = RSat::ESmsDefaultAlphabetPacked;
					}
				else
					{
					getInputV1.iRspFormat = RSat::ESmsDefaultAlphabetUnpacked;
					}
				}
            
			else
				{
				if  ( KGetInputSmsPackedMask & cmdQualifier )
					{
					// Digit only
					getInputV1.iRspFormat = RSat::EDigitOnlyPacked;
					}
				else
					{
					getInputV1.iRspFormat = RSat::EDigitOnlyUnpacked;
					}
				}
				

            // Help information.
            if ( KHelpAvailabilityMask & cmdQualifier )
                {
                // Help information available
                getInputV1.iHelp = RSat::EHelpAvailable;
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_COMPLETENOTIFYL_2, "CSAT: CSatNotifyGetInput::CompleteNotifyL, No Help available");
                // No help
                getInputV1.iHelp = RSat::ENoHelpAvailable;
                }

            
            // Get Input text
            getInputV1.iText.Zero();
			CTlv textString;
            returnValue = berTlv.TlvByTagValue( &textString, KTlvTextStringTag );
    
            if ( KErrNone == returnValue )
                {
                // Convert and set text
                TSatUtility::SetText( textString, getInputV1.iText );
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_COMPLETENOTIFYL_3, "CSAT: CSatNotifyGetInput::CompleteNotifyL, Input text missing");
                additionalInfo.Zero();
                additionalInfo.Append( KNoCause );
                CreateTerminalRespL( pCmdNumber, 
					RSat::KErrorRequiredValuesMissing, additionalInfo,
					emptyText, RSat::ESmsDefaultAlphabetUnpacked );
                ret = KErrCorrupt;
                }

            OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_COMPLETENOTIFYL_4, "CSAT: GetInput, text: %S", getInputV1.iText );

			// Response length expected                                     
			CTlv responseLength;
			returnValue = berTlv.TlvByTagValue( &responseLength, 
			    KTlvResponseLengthTag );
			
			if ( KErrNone == returnValue )
				{
				// Set minimum response length
				getInputV1.iRspLength.iMinRspLength = 
                responseLength.GetShortInfo( ETLV_MinimumLengthOfResponse );
				
				if ( RSat::KGetInputTextMaxSize < 
				    getInputV1.iRspLength.iMinRspLength )
					{	
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_COMPLETENOTIFYL_5, "CSAT: CSatNotifyGetInput::CompleteNotifyL, Input text length exceeded");
					additionalInfo.Zero();
                    additionalInfo.Append( KNoCause );
					CreateTerminalRespL( pCmdNumber, 
						RSat::KCmdDataNotUnderstood, additionalInfo,
						emptyText, RSat::ESmsDefaultAlphabetUnpacked );
					ret = KErrCorrupt;
					}
					
				// Set maximum response length
				getInputV1.iRspLength.iMaxRspLength = 
                responseLength.GetShortInfo( ETLV_MaximumLengthOfResponse );
				}				
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_COMPLETENOTIFYL_6, "CSAT: CSatNotifyGetInput::CompleteNotifyL, Response length TLV missing");
				// Mandatory reponse length tlv is missing
				additionalInfo.Zero();
                additionalInfo.Append( KNoCause );
				CreateTerminalRespL( pCmdNumber,
					RSat::KErrorRequiredValuesMissing, additionalInfo,
					emptyText, RSat::ESmsDefaultAlphabetUnpacked );
				ret = KErrCorrupt;
				}					
									
			// Default text to display (optional)
			getInputV1.iDefaultText.Zero();
			returnValue = berTlv.TlvByTagValue( &textString, KTlvDefaultTextTag );
    
			// If default text exist
			if ( KErrNone == returnValue )
				{
				TSatUtility::SetText( textString, getInputV1.iDefaultText );
				}

			// Iconid ( optional )
			TSatUtility::FillIconStructure( berTlv, getInputV1.iIconId );

			} // if ( KErrNone == aErrorCode )
        else
        	{
        	ret = aErrorCode;
        	}
		iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
		}
	
	else 
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_COMPLETENOTIFYL_7, "CSAT: CSatNotifyGetInput::CompleteNotifyL, Request not ongoing");
		// Request not on, returning response immediately
		additionalInfo.Zero();
        additionalInfo.Append( KNoCause );		
		CreateTerminalRespL( pCmdNumber,
			RSat::KMeUnableToProcessCmd, additionalInfo,
			emptyText, RSat::ESmsDefaultAlphabetUnpacked );					
    	}
		
	return ret;
	
	}

// -----------------------------------------------------------------------------
// CSatNotifyGetInput::TerminalResponseL
// Called by ETel server, passes terminal response to SIM card
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInput::TerminalResponseL
        ( 
        TDes8* aRsp 
        )
    {    	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_TERMINALRESPONSEL_1, "CSAT: CSatNotifyGetInput::TerminalResponseL");
    
	TInt returnValue( KErrNone );
	TBuf<1>additionalInfo;

    RSat::TGetInputRspV1Pckg* aRspPckg = 
            reinterpret_cast<RSat::TGetInputRspV1Pckg*>( aRsp );
    RSat::TGetInputRspV1& rspV1 = ( *aRspPckg ) ();

    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_TERMINALRESPONSEL_2, "CSAT: CSatNotifyGetInput::TerminalResponseL, iAdditionalInfo: %S, iInfoType: %u", rspV1.iAdditionalInfo, rspV1.iInfoType);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_TERMINALRESPONSEL_3, "CSAT:CSatNotifyGetInput::TerminalResponseL, iGeneralResult: 0x%02x", (TUint)rspV1.iGeneralResult);
	
	// Get Proactive command number
    TUint8 pCmdNumber( rspV1.PCmdNumber() );
	
    // Checking that the response type is valid
    if ( RSat::EGetInputRspFormatNotSet == rspV1.iRspFormat )
        {
        returnValue = KErrCorrupt;
        }

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult ) 
	    && ( RSat::KPSessionTerminatedByUser  != rspV1.iGeneralResult )
        && ( RSat::KBackwardModeRequestedByUser != rspV1.iGeneralResult )
        && ( RSat::KNoResponseFromUser != rspV1.iGeneralResult )
        && ( RSat::KHelpRequestedByUser != rspV1.iGeneralResult )
        && ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult ))
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_TERMINALRESPONSEL_4, "CSAT: CSatNotifyGetInput::TerminalResponseL, Invalid General Result");
        // Invalid general result
        returnValue = KErrCorrupt;
        }
    
    // If there is Me (Mobile Entity) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo.Zero();
            additionalInfo.Append( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_TERMINALRESPONSEL_5, "CSAT: CSatNotifyGetInput::TerminalResponseL, Invalid Additional Info");
            // Invalid additional info field
            returnValue = KErrCorrupt;
            }
        }
        
    CreateTerminalRespL( pCmdNumber, ( TUint8 ) rspV1.iGeneralResult ,       
        additionalInfo, rspV1.iAdditionalInfo, ( TUint8 )rspV1.iRspFormat );						
        
    return returnValue;

    }
    
// -----------------------------------------------------------------------------
// CSatNotifyGetInput::CreateTerminalRespL
// Constructs GetInput specific part of terminal response and calls 
// DOS to send the actual message. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInput::CreateTerminalRespL
		(
	    TUint8	 aPCmdNumber,	
	    TUint8	 aGeneralResult,	
	    TDesC16&  aAdditionalInfo,  
	    TDesC16& aTextString,       
	    TUint8   aDcs              
	    )
	{	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_CREATETERMINALRESPL_1, "CSAT: CSatNotifyGetInput::CreateTerminalRespL");
    TTlv tlvSpecificData;
	TBuf8<KGetInputStringMaxSize> string;

    tlvSpecificData.AddTag( KTlvResultTag );
    //General result
	tlvSpecificData.AddByte( aGeneralResult );
    
	if ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        {
        if ( aAdditionalInfo.Length() > 0 )
        	{
	        tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] );
        	}
		}

    if ( iNotificationsTsy->CommandPerformedSuccessfully( aGeneralResult ) )
        {
        // Create TextString TLV
        tlvSpecificData.AddTag( KTlvTextStringTag );
        

        if ( aTextString.Length() )
    		{
            OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_CREATETERMINALRESPL_2, "CSAT: CSatNotifyGetInput::CreateTerminalRespL aTextString: %S", aTextString);
            switch ( aDcs )
                {
                case RSat::EUcs2Alphabet:
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_CREATETERMINALRESPL_3, "CSAT: CSatNotifyGetInput::CreateTerminalRespL EUcs2Alphabet");
                     // Data coding scheme UCS2 (16-bit)
                    tlvSpecificData.AddByte( KUCS2DCS );   
                    
                    if ( aTextString.Length() )
                        {
						TSatUtility::Copy16to8LE( aTextString, string );   
                        tlvSpecificData.AddData( string );
                        }
                    break;
                    }

                case RSat::EDigitOnlyPacked:
                case RSat::ESmsDefaultAlphabetPacked:
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_CREATETERMINALRESPL_4, "CSAT: CSatNotifyGetInput::CreateTerminalRespL EDigitOnlyPacked/ESmsDefaultAlphabetPacked");
                    // Data coding scheme 7-bit default sms
                    tlvSpecificData.AddByte( K7BitDefaultSmsDCS );  
                    TSatUtility::UCSToPacked7( aTextString, string );
                    tlvSpecificData.AddData( string );
                    break;
                    }

                case RSat::EDigitOnlyUnpacked:
                case RSat::ESmsDefaultAlphabetUnpacked:
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_CREATETERMINALRESPL_5, "CSAT: CSatNotifyGetInput::CreateTerminalRespL EDigitOnlyUnpacked/ESmsDefaultAlphabetUnpacked");
                    // Data coding scheme 8-bit
                    tlvSpecificData.AddByte( K8BitDCS );
                    TSatUtility::ConvertUnicode16To7Bit( aTextString, string );
                    tlvSpecificData.AddData( string );
                    break;
                    }
                default:
                    // do nothing
                    break;
                }

            OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINPUT_CREATETERMINALRESPL_6, "CSAT: CSatNotifyGetInput::CreateTerminalRespL string: %s", string);
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


