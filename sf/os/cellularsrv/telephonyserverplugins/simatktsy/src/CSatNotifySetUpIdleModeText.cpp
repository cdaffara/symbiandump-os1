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
// Name        : CSatNotifySetUpIdleModeText.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// SetUpIdleModeText notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySetUpIdleModeTextTraces.h"
#endif

#include <satcs.h>						// Etel SAT IPC definitions
#include "CSatTsy.h"					// Tsy class header
#include "CSatNotifySetUpIdleModeText.h"// Tsy class header
#include "CSatNotificationsTsy.h"		// Class header
#include "CBerTlv.h"					// Ber Tlv data handling
#include "TTlv.h"						// TTlv class
#include "CSatDataPackage.h"			// Parameter packing 
#include "TSatUtility.h"				// Utilities
#include "CSatTsyReqHandleStore.h"		// Request handle class
#include "cmmmessagemanagerbase.h"		// Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpIdleModeText* CSatNotifySetUpIdleModeText::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_NEWL_1, "CSAT: CSatNotifySetUpIdleModeText::NewL");
   	CSatNotifySetUpIdleModeText* const satNotifySetUpIdleModeText = 
        new ( ELeave ) CSatNotifySetUpIdleModeText( aNotificationsTsy );
    CleanupStack::PushL( satNotifySetUpIdleModeText );
    satNotifySetUpIdleModeText->ConstructL();
    CleanupStack::Pop( satNotifySetUpIdleModeText );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_NEWL_2, "CSAT: CSatNotifySetUpIdleModeText::NewL, end of method");
    return satNotifySetUpIdleModeText;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::~CSatNotifySetUpIdleModeText
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpIdleModeText::~CSatNotifySetUpIdleModeText
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_DTOR_1, "CSAT: CSatNotifySetUpIdleModeText::~CSatNotifySetUpIdleModeText");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::CSatNotifySetUpIdleModeText
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpIdleModeText::CSatNotifySetUpIdleModeText
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySetUpIdleModeText::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_CONSTRUCTL_1, "CSAT: CSatNotifySetUpIdleModeText::ConstructL");
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::Notify
// This request allows a client to be notified of a Set Up Idle Mode Text 
// proactive command
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpIdleModeText::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage    
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_NOTIFY_1, "CSAT: CSatNotifySetUpIdleModeText::Notify");
    // Save data pointer to client side for completion
    iSetUpIdleModeTextV1Pckg = reinterpret_cast<RSat::
    	TSetUpIdleModeTextV1Pckg*>( aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySetUpIdleModeTextPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSetUpIdleModeText );   

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySetUpIdleModeText request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpIdleModeText::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_CANCELNOTIFICATION_1, "CSAT: CSatNotifySetUpIdleModeText::CancelNotification");
    
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifySetUpIdleModeTextPCmdReqType );
	// Reset the data pointer
	iSetUpIdleModeTextV1Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifySetUpIdleModeText request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpIdleModeText::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,  
        TInt aErrorCode                  
        )
    {   
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_COMPLETENOTIFYL_1, "CSAT: CSatNotifySetUpIdleModeText::CompleteNotifyL");
    TInt ret( KErrNone );
	TInt returnValue( KErrNone );
	TBuf<1> additionalInfo;	
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
		iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifySetUpIdleModeTextPCmdReqType );
	// Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
	//get command details tlv
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
			// Fill the set up idle mode text structure
			RSat::TSetUpIdleModeTextV1& setUpIdleModeTextV1 
				= ( *iSetUpIdleModeTextV1Pckg )();

			// Store command number
            setUpIdleModeTextV1.SetPCmdNumber( pCmdNumber );
			
			//Initialize Type to notSet and text to zero
			setUpIdleModeTextV1.iType = RSat::ESetUpIdleModeTextTypeNotSet;
			setUpIdleModeTextV1.iText.Zero();

			CTlv textString;
			returnValue = berTlv.TlvByTagValue( &textString, 
			    KTlvTextStringTag );

			// If the returnValue is KErrNone
			if ( KErrNone == returnValue )
				{
				if ( textString.GetLength() )
					{
					// Convert and set text
					TSatUtility::SetText( textString, 
					    setUpIdleModeTextV1.iText );
					//set type
					setUpIdleModeTextV1.iType = RSat::EUpdateIdleModeText;
					}
				else
					{
					//set type
					setUpIdleModeTextV1.iType = 
					    RSat::ERemoveExistingIdleModeText;
					}
				}

			// Iconid (optional)
			TSatUtility::FillIconStructure( berTlv, 
				setUpIdleModeTextV1.iIconId );
			
			if( (KErrNone != returnValue) ||
				( ( RSat::ERemoveExistingIdleModeText == setUpIdleModeTextV1.iType) && 
				  ( RSat::ENotSelfExplanatory==setUpIdleModeTextV1.iIconId.iQualifier)) )
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_COMPLETENOTIFYL_2, "CSAT: CSatNotifySetUpIdleModeText::CompleteNotifyL, Invalid Data");
				additionalInfo.Zero();
                additionalInfo.Append( KNoCause );
				CreateTerminalRespL( pCmdNumber, RSat::KCmdDataNotUnderstood, 
					additionalInfo );                         				
				ret = KErrCorrupt;
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
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_COMPLETENOTIFYL_3, "CSAT: CSatNotifySetUpIdleModeText::CompleteNotifyL, Request not ongoing");
        // Request not on, returning response immediately
		additionalInfo.Zero();
        additionalInfo.Append( KNoCause );
		CreateTerminalRespL(
			pCmdNumber, RSat::KMeUnableToProcessCmd,       
			additionalInfo );					
		}
		
    return ret;
	}


// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpIdleModeText::TerminalResponseL
        ( 
        TDes8* aRsp 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_TERMINALRESPONSEL_1, "CSAT: CSatNotifySetUpIdleModeText::TerminalResponseL");

	TInt   ret( KErrNone );
    TBuf<1> additionalInfo;
    additionalInfo.Zero();
    RSat::TSetUpIdleModeTextRspV1Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSetUpIdleModeTextRspV1Pckg*>( aRsp );
    RSat::TSetUpIdleModeTextRspV1& rspV1 = ( *aRspPckg ) ();
    // Get Proactive command number
    TUint8 pCmdNumber( rspV1.PCmdNumber() );

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult ) && 
         ( RSat::KPSessionTerminatedByUser  != rspV1.iGeneralResult ) && 
         ( RSat::KBackwardModeRequestedByUser != rspV1.iGeneralResult ) && 
         ( RSat::KNoResponseFromUser != rspV1.iGeneralResult ) && 
         ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult ) && 
         ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult ) && 
         ( RSat::KCmdDataNotUnderstood != rspV1.iGeneralResult ) &&
         ( RSat::KSuccessRequestedIconNotDisplayed != rspV1.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_TERMINALRESPONSEL_2, "CSAT: CSatNotifySetUpIdleModeText::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Equipment) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( 0 != rspV1.iAdditionalInfo.Length()  )
            {
            additionalInfo.Append( ( TUint8 ) rspV1.iAdditionalInfo[0] );
			}
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_TERMINALRESPONSEL_3, "CSAT: CSatNotifySetUpIdleModeText::TerminalResponseL, Invalid Additional Info");
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
        
    CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
        rspV1.iGeneralResult ), additionalInfo );                            

	return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpIdleModeText::CreateTerminalRespL
// Constructs SetUpIdleModeText specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpIdleModeText::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,       
        TUint8 aGeneralResult,      
        TDesC16& aAdditionalInfo			
		)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPIDLEMODETEXT_CREATETERMINALRESPL_1, "CSAT: CSatNotifySetUpIdleModeText::CreateTerminalRespL");
	TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    // General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( !( iNotificationsTsy->CommandPerformedSuccessfully( aGeneralResult ) ) )
        {
        switch ( aGeneralResult )
            {
            // Cases in which additional info is not required
            case RSat::KPSessionTerminatedByUser:
            case RSat::KBackwardModeRequestedByUser:
            case RSat::KNoResponseFromUser:
            case RSat::KErrorRequiredValuesMissing:
            case RSat::KHelpRequestedByUser:
            case RSat::KCmdDataNotUnderstood:
                {
                break;
                }
            default:
                {
                if ( aAdditionalInfo.Length() )
                    {
                    tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] );
                    }
				break;
                }
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

