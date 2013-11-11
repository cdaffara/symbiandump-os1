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
// Name        : CSatNotifyGetInkey.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Get inkey notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyGetInkeyTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyGetInkey.h"     // Tsy class header
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
CSatNotifyGetInkey* CSatNotifyGetInkey::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_NEWL_1, "CSAT: CSatNotifyGetInkey::NewL");
   	CSatNotifyGetInkey* const satNotifyGetInkey = 
        new ( ELeave ) CSatNotifyGetInkey( aNotificationsTsy );
    CleanupStack::PushL( satNotifyGetInkey );
    satNotifyGetInkey->ConstructL();
    CleanupStack::Pop( satNotifyGetInkey );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_NEWL_2, "CSAT: CSatNotifyGetInkey::NewL, end of method");
    return satNotifyGetInkey;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::~CSatNotifyGetInkey
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyGetInkey::~CSatNotifyGetInkey
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_DTOR_1, "CSAT: CSatNotifyGetInkey::~CSatNotifyGetInkey");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::CSatNotifyGetInkey
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyGetInkey::CSatNotifyGetInkey
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
void CSatNotifyGetInkey::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_CONSTRUCTL_1, "CSAT: CSatNotifyGetInkey::ConstructL");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::Notify
// Save the request handle type for notification requested by ETel server
// -----------------------------------------------------------------------------
// 
TInt CSatNotifyGetInkey::Notify
        ( 
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_NOTIFY_1, "CSAT: CSatNotifyGetInkey::Notify");
    // Save data pointer to client side for completion
    iGetInkeyV2Pckg = reinterpret_cast<RSat::TGetInkeyV2Pckg*>( 
        aPackage.Des1n() );    
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyGetInkeyPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KGetInkey );   
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySimSessionEnd request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInkey::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_CANCELNOTIFICATION_1, "CSAT: CSatNotifyGetInkey::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyGetInkeyPCmdReqType );
    // Reset the data pointer
    iGetInkeyV2Pckg = NULL;
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::CompleteNotifyL
// Complete Sim Session End notification to the client
// -----------------------------------------------------------------------------
// 
TInt CSatNotifyGetInkey::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage, 
        TInt aErrorCode  
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_COMPLETENOTIFYL_1, "CSAT: CSatNotifyGetInkey::CompleteNotifyL");
    TInt ret( KErrNone );
    TInt returnValue( KErrNone );

    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
	TBuf<1> additionalInfo;

    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifyGetInkeyPCmdReqType );
        
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
        commandDetails.Data() );
    // Get command qualifier
    TUint8 cmdQualifier( commandDetails.GetShortInfo( 
        ETLV_CommandQualifier ) );
    
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
    
    // In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode  )
            {
            // Fill the get inkey structure
            RSat::TGetInkeyV2& getInkeyV2 = ( *iGetInkeyV2Pckg )();

            getInkeyV2.SetPCmdNumber( pCmdNumber );
    
            if ( KYesNoMask & cmdQualifier )
                {
                // The character set bits are disabled and yes/no response
                // is requested
                getInkeyV2.iRspFormat = RSat::EYesNo;
                iRequestedRspFormat = K8BitDCS;
                }
            else
                {
                // The character set bits are enabled
                if ( KAlphabetMask & cmdQualifier )
                    {
                    // Character set selected
                    if ( KUCS2AlphabetMask & cmdQualifier )
                        {
                        // UCS2 alphabet selected
                        getInkeyV2.iRspFormat = RSat::ECharUcs2Alphabet;
                        iRequestedRspFormat = KUCS2DCS;
                        }
                    else
                        {
                        // SMS default alphabet selected
                        getInkeyV2.iRspFormat = RSat::ECharSmsDefaultAlphabet;
                        iRequestedRspFormat = K8BitDCS;
                        }
                    }
                else
                    {
                    // Digits only selected
                    getInkeyV2.iRspFormat = RSat::EDigitOnly;
                    iRequestedRspFormat = K8BitDCS;
                    }   
                }

            // Help information.
            if ( KHelpAvailabilityMask & cmdQualifier )
                {
                // Help information available
                getInkeyV2.iHelp = RSat::EHelpAvailable;
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_COMPLETENOTIFYL_2, "CSAT: CSatNotifyGetInkey::CompleteNotifyL, No Help available");
                // No help
                getInkeyV2.iHelp = RSat::ENoHelpAvailable;
                }

            // Response Mode
            getInkeyV2.iMode = RSat::EGetInkeyRspModeNotSet;
            if ( KImmediateDigitResponse & cmdQualifier )
                {
                // An immediate digit response (0-9, * and #) is requested.
                getInkeyV2.iMode = RSat::EImmediateDigitRsp;
                }
            else
                {
                getInkeyV2.iMode = RSat::EUserRspDisplayed;
                }

            // Get Inkey text
            CTlv textString;
            returnValue = berTlv.TlvByTagValue( &textString, KTlvTextStringTag );
    
            if ( KErrNone == returnValue )
                {
                // Convert and set text
                TSatUtility::SetText( textString, getInkeyV2.iText );
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_COMPLETENOTIFYL_3, "CSAT: CSatNotifyGetInkey::CompleteNotifyL, Required values missing");
                additionalInfo.Zero();
                additionalInfo.Append( KNoCause );
                CreateTerminalRespL( pCmdNumber, 
                    RSat::KErrorRequiredValuesMissing, additionalInfo );

                ret = KErrCorrupt;
                }

            // IconId Optional
            TSatUtility::FillIconStructure( berTlv, getInkeyV2.iIconId );
            // Duration 
            TSatUtility::FillDurationStructure( berTlv, getInkeyV2.iDuration );
            }
        else
        	{
        	ret = aErrorCode;
        	}
        
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
        }
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_COMPLETENOTIFYL_4, "CSAT: CSatNotifyGetInkey::CompleteNotifyL, Request not ongoing");
		additionalInfo.Zero();
        additionalInfo.Append( KNoCause );
        CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd, 
            additionalInfo ); 
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::TerminalResponseL
// Called by ETel server, passes terminal response to SIM card
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInkey::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {   
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_TERMINALRESPONSEL_1, "CSAT: CSatNotifyGetInkey::TerminalResponseL");
    
	TInt ret( KErrNone );

    RSat::TGetInkeyRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TGetInkeyRspV2Pckg*>( aRsp );
    RSat::TGetInkeyRspV2& rspV2 = ( *aRspPckg ) ();

    TUint8 pCmdNumber( rspV2.PCmdNumber() );

    // Checking that infotype is valid
    if ( ( RSat::KMeProblem != rspV2.iInfoType ) && 
         ( RSat::KTextString != rspV2.iInfoType ) && 
         ( RSat::KNoAdditionalInfo != rspV2.iInfoType ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_TERMINALRESPONSEL_2, "CSAT: CSatNotifyGetInkey::TerminalResponseL, Invalid Infotype");
        ret = KErrCorrupt;
        }

    // Checking that the response type is valid
    if ( ( RSat::EDigitOnly != rspV2.iRspFormat )&& 
         ( RSat::ECharSmsDefaultAlphabet != rspV2.iRspFormat ) && 
         ( RSat::ECharUcs2Alphabet  != rspV2.iRspFormat ) && 
         ( RSat::EYesNo != rspV2.iRspFormat ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_TERMINALRESPONSEL_3, "CSAT: CSatNotifyGetInkey::TerminalResponseL, Invalid Response Format");
        ret = KErrCorrupt;
        }
    
    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV2.iGeneralResult ) && 
         ( RSat::KPSessionTerminatedByUser  != rspV2.iGeneralResult ) && 
         ( RSat::KBackwardModeRequestedByUser != rspV2.iGeneralResult ) && 
         ( RSat::KNoResponseFromUser != rspV2.iGeneralResult ) && 
         ( RSat::KHelpRequestedByUser != rspV2.iGeneralResult ) && 
         ( RSat::KMeUnableToProcessCmd != rspV2.iGeneralResult ) && 
         ( RSat::KCmdBeyondMeCapabilities != rspV2.iGeneralResult ) && 
         ( RSat::KErrorRequiredValuesMissing != rspV2.iGeneralResult ) &&
         ( RSat::KSuccessRequestedIconNotDisplayed != rspV2.iGeneralResult ))
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_TERMINALRESPONSEL_4, "CSAT: CSatNotifyGetInkey::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }
    
    // If there is Me (Mobile Entity) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV2.iInfoType ) &&  
         ( NULL == rspV2.iAdditionalInfo.Length() ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_TERMINALRESPONSEL_5, "CSAT: CSatNotifyGetInkey::TerminalResponseL, Invalid Additional Info, ME Problem");
        // Invalid additional info field
        ret = KErrCorrupt;
        }

    if ( ( RSat::KTextString == rspV2.iInfoType ) && 
         ( NULL == rspV2.iAdditionalInfo.Length() ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_TERMINALRESPONSEL_6, "CSAT: CSatNotifyGetInkey::TerminalResponseL, Invalid Additional Info");
        ret = KErrCorrupt;
        }
	
	else
		{
		// Do nothing
		}
		
    CreateTerminalRespL( pCmdNumber, ( TUint8 ) rspV2.iGeneralResult ,       
        rspV2.iAdditionalInfo, iRequestedRspFormat );                 

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyGetInkey::CreateTerminalRespL
// Constructs GetInkey specific part of terminal response and calls 
// DOS to send the actual message. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyGetInkey::CreateTerminalRespL
        ( 
        TUint8  aPCmdNumber,         
        TUint8  aGeneralResult,      
        TDesC16& aAdditionalInfo,  
        TUint8  aDcs          
		)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_CREATETERMINALRESPL_1, "CSAT: CSatNotifyGetInkey::CreateTerminalRespL");

	TTlv tlvSpecificData;
    tlvSpecificData.AddTag( KTlvResultTag );  
	// General result
    tlvSpecificData.AddByte( aGeneralResult );
    
    if ( !( iNotificationsTsy->CommandPerformedSuccessfully( 
        aGeneralResult ) ) )
        {
        switch ( aGeneralResult )
            {
            // Cases in which additional info is not required
            case RSat::KPSessionTerminatedByUser:
            case RSat::KBackwardModeRequestedByUser:
            case RSat::KNoResponseFromUser:
            case RSat::KErrorRequiredValuesMissing:
            case RSat::KHelpRequestedByUser:
                {
                break;
                }
            default:
                {
                if ( NULL != aAdditionalInfo.Length() )
                    {
                    tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] );
					}
                break;
                }
            }
        }
    else
        {
        tlvSpecificData.AddTag( KTlvTextStringTag );
        tlvSpecificData.AddByte( aDcs );
        TBuf8<2> character;

        if ( aDcs & KUCS2DCS )
            {
            TSatUtility::Copy16to8LE( aAdditionalInfo, character);
            tlvSpecificData.AddData( character );
            OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYGETINKEY_CREATETERMINALRESPL_2, "CSAT:CSatNotifyGetInkey::CreateTerminalRespL, %s character", character );
            }
        else
            {
            if ( NULL != aAdditionalInfo.Length())
                {
                TBuf<1> addInfo;
                addInfo.Append( aAdditionalInfo );
                TSatUtility::ConvertUnicode16To7Bit( addInfo, character );
                }
            else
                {
                character.Append( 0x00 );
                }
            tlvSpecificData.AddData( character );
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

