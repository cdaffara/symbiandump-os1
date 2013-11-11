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
// Name        : CSatNotifyDisplayText.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Display text notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyDisplayTextTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyDisplayText.h"  // Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.


// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::CSatNotifyDisplayText
// Constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyDisplayText* CSatNotifyDisplayText::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )  
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_NEWL_1, "CSAT: CSatNotifyDisplayText::NewL");
   	CSatNotifyDisplayText* const satNotifyDisplayText = 
        new ( ELeave ) CSatNotifyDisplayText( aNotificationsTsy );
    CleanupStack::PushL( satNotifyDisplayText );
    satNotifyDisplayText->ConstructL();
    CleanupStack::Pop( satNotifyDisplayText );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_NEWL_2, "CSAT: CSatNotifyDisplayText::NewL, end of method");
    return satNotifyDisplayText;
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::~CSatNotifyDisplayText
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyDisplayText::~CSatNotifyDisplayText()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_DTOR_1, "CSAT: CSatNotifyDisplayText::~CSatNotifyDisplayText");
    }
        
// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::CSatNotifyDisplayText
// Constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyDisplayText::CSatNotifyDisplayText
        ( 
		CSatNotificationsTsy* aNotificationsTsy
        ):iNotificationsTsy( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CSatNotifyDisplayText::ConstructL
        (    
        void
        )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_CONSTRUCTL_1, "CSAT: CSatNotifyDisplayText::ConstructL");
	}
       	
// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::Notify
// This request allows a client to be notified of a DISPLAY TEXT proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifyDisplayText::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_NOTIFY_1, "CSAT: CSatNotifyDisplayText::Notify");
    // Save data pointer to client side for completion
    iDisplayTextV2Pckg = reinterpret_cast<RSat::TDisplayTextV2Pckg*>( 
        aPackage.Des1n() );    
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyDisplayTextPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KDisplayText );   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyDisplayText request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyDisplayText::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_CANCELNOTIFICATION_1, "CSAT: CSatNotifyDisplayText::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyDisplayTextPCmdReqType );
	// Reset the data pointer
	iDisplayTextV2Pckg = NULL;
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifyDisplayText request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyDisplayText::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_COMPLETENOTIFYL_1, "CSAT: CSatNotifyDisplayText::CompleteNotifyL");
    TInt ret( KErrNone );

    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_COMPLETENOTIFYL_2, "CSAT: CSatNotifyDisplayText::CompleteNotifyL, dataLength: %d", data->Length());
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( 
		CSatTsy::ESatNotifyDisplayTextPCmdReqType );
		
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
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
        if ( KErrNone == aErrorCode )
            {  
            // Let's fill the display text structure
            RSat::TDisplayTextV2& displayTextV2 = ( *iDisplayTextV2Pckg )();

            // Store command number
            displayTextV2.SetPCmdNumber( pCmdNumber );

            // Priority scheme
            if ( KPriorityMask & cmdQualifier )
                {
                // Display priority high
                displayTextV2.iPriority = RSat::EHighPriority;
                }
            else
                {
                // Display priority normal
                displayTextV2.iPriority = RSat::ENormalPriority;
                }

            // Clearing scheme
            if ( KMessageClearMask & cmdQualifier)
                {
                // Wait for user to clear the message
                displayTextV2.iClearScreenTrigger = RSat::EUserClear;
                }
            else
                {
                // Clear message after a delay
                displayTextV2.iClearScreenTrigger = RSat::EClearAfterDelay;
                }

            CTlv immediateResponse;
            TInt returnValue( berTlv.TlvByTagValue( 
                &immediateResponse, KTlvImmediateResponseTag ) );

            // Immediate response
            if ( KErrNotFound != returnValue )
                {
                // Respond immediately
                displayTextV2.iImmediateRsp = RSat::EImmediateRsp;
                }
            else
                {
                // Do not respond immediately
                displayTextV2.iImmediateRsp = RSat::ENoImmediateRsp;
                }

            // Text to display
            displayTextV2.iText.Zero();

            CTlv textString;
            returnValue = berTlv.TlvByTagValue( &textString, 
                KTlvTextStringTag );

            if ( KErrNone == returnValue )
                {
                // Text string tag is found, now
                // check if the text string is null.
                if ( textString.GetLength() )
                    {
                    // Convert and set text
                    TSatUtility::SetText( textString, displayTextV2.iText );
                    }
                // Text string is a null data object.
                // Return terminal response immediately.
                else
                    {            
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_COMPLETENOTIFYL_3, "CSAT: CSatNotifyDisplayText::CompleteNotifyL, Text string empty");
					CreateTerminalRespL( pCmdNumber, 
					    RSat::KCmdDataNotUnderstood, KNullDesC16 );
					// Indicate Client that response was corrupted
                    ret = KErrCorrupt;
                    }
                }    
			else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_COMPLETENOTIFYL_4, "CSAT: CSatNotifyDisplayText::CompleteNotifyL, Test string missing");
	            CreateTerminalRespL( 
					pCmdNumber, RSat::KErrorRequiredValuesMissing,  
					KNullDesC16 );
				// Indicate Client that response was corrupted
                ret = KErrCorrupt;
                }

            // Duration,( optional ) see 3GPP TS 31.111
            TSatUtility::FillDurationStructure( 
                berTlv, displayTextV2.iDuration );

            // Iconid (optional)
            TSatUtility::FillIconStructure( 
                berTlv, displayTextV2.iIconId );
    
            } // End of if ( KErrNone == aErrorCode )
        else
        	{
        	ret = aErrorCode;
        	}

        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );

        } // End of if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
    else 
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_COMPLETENOTIFYL_5, "CSAT: CSatNotifyDisplayText::CompleteNotifyL, Request not ongoing");
    	TBuf16<1> additionalInfo;
        // Request not on, returning response immediately
        additionalInfo.Append( RSat::KNoSpecificMeProblem );
		CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd, 
		    additionalInfo );					
    	}

    return ret;
    }
// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifyDisplayText::TerminalResponseL
        ( 
        TDes8* aRsp  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_TERMINALRESPONSEL_1, "CSAT: CSatNotifyDisplayText::TerminalResponseL");

    TInt ret( KErrNone );
    TBuf16<1> additionalInfo;

    RSat::TDisplayTextRspV1Pckg* aRspPckg = 
        reinterpret_cast<RSat::TDisplayTextRspV1Pckg*>( aRsp );
    RSat::TDisplayTextRspV1& rspV1 = ( *aRspPckg ) ();
    
    // Get Proactive command number
    TUint8 pCmdNumber( rspV1.PCmdNumber() );
  
    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult ) 
    	&& ( RSat::KSuccessRequestedIconNotDisplayed != rspV1.iGeneralResult )
        && ( RSat::KPSessionTerminatedByUser  != rspV1.iGeneralResult )
        && ( RSat::KBackwardModeRequestedByUser != rspV1.iGeneralResult )
        && ( RSat::KNoResponseFromUser != rspV1.iGeneralResult )
        && ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult )
        )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_TERMINALRESPONSEL_2, "CSAT: CSatNotifyDisplayText::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is ME (Mobile Equipment) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo.Append( rspV1.iAdditionalInfo[0] );
			}
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_TERMINALRESPONSEL_3, "CSAT: CSatNotifyDisplayText::TerminalResponseL, Invalid Additional Info");
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
	
    CreateTerminalRespL( pCmdNumber, static_cast< TUint8 >( 
    	rspV1.iGeneralResult ), additionalInfo );                            

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyDisplayText::CreateTerminalRespL
// Constructs DisplayText specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyDisplayText::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,      
        const TDesC16& aAdditionalInfo
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYDISPLAYTEXT_CREATETERMINALRESPL_1, "CSAT: CSatNotifyDisplayText::CreateTerminalRespL");
    TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    // General result
    tlvSpecificData.AddByte( aGeneralResult );
     
    if ( KMeCurrentlyUnableToProcessCommand == aGeneralResult )
        {
        if ( aAdditionalInfo.Length() > 0 )
            {
            tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[0] ) );
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

