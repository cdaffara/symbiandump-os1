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
// Name        : CSatNotifyPlayTone.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// PlayTone notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyPlayToneTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyPlayTone.h"     // Tsy class header
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
CSatNotifyPlayTone* CSatNotifyPlayTone::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_NEWL_1, "CSAT: CSatNotifyPlayTone::NewL");
   	CSatNotifyPlayTone* const satNotifyPlayTone = 
        new ( ELeave ) CSatNotifyPlayTone( aNotificationsTsy );
    CleanupStack::PushL( satNotifyPlayTone );
    satNotifyPlayTone->ConstructL();
    CleanupStack::Pop( satNotifyPlayTone );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_NEWL_2, "CSAT: CSatNotifyPlayTone::NewL, end of method");
    return satNotifyPlayTone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::~CSatNotifyPlayTone
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyPlayTone::~CSatNotifyPlayTone
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_DTOR_1, "CSAT: CSatNotifyPlayTone::~CSatNotifyPlayTone");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::CSatNotifyPlayTone
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyPlayTone::CSatNotifyPlayTone
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyPlayTone::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_CONSTRUCTL_1, "CSAT: CSatNotifyPlayTone::ConstructL, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::Notify
// Save the request handle type for notification requested by ETel server
// -----------------------------------------------------------------------------
//
TInt CSatNotifyPlayTone::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_NOTIFY_1, "CSAT: CSatNotifyPlayTone::Notify");
    // Save data pointer to client side for completion
    iPlayToneV2Pckg = reinterpret_cast<RSat::TPlayToneV2Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyPlayTonePCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KPlayTone );   

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyPlayTone request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyPlayTone::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_CANCELNOTIFICATION_1, "CSAT: CSatNotifyPlayTone::CancelNotification");
    // Reset the request handle
    iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyPlayTonePCmdReqType );
	iPlayToneV2Pckg = NULL;
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }




// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::CompleteNotifyL
// Complete Sim Session End notification to the client
// -----------------------------------------------------------------------------
// 
TInt CSatNotifyPlayTone::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage, 
		TInt aErrorCode  
        ) 
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_COMPLETENOTIFYL_1, "CSAT: CSatNotifyPlayTone::CompleteNotifyL");
	TInt returnValue( KErrNone );
    TInt ret( KErrNone );
    
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
	TBuf<1>additionalInfo;
    
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifyPlayTonePCmdReqType );
    
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iNotificationsTsy->iTerminalRespData.iCommandDetails.
        Copy( commandDetails.Data() );
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );	
	
	// In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode )
            {
            // Fill the Play Tone structure
            RSat::TPlayToneV2& playToneV2 = ( *iPlayToneV2Pckg )();

            playToneV2.SetPCmdNumber( pCmdNumber );
    
  			// Alpha ID string (optional)
			CTlv alphaIdentifier;
			returnValue = berTlv.TlvByTagValue( &alphaIdentifier, 
            KTlvAlphaIdentifierTag );
            
			if ( KErrNotFound != returnValue )
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_COMPLETENOTIFYL_2, "CSAT: CSatNotifyPlayTone::CompleteNotifyL, Alpha ID present");
				TUint16 alphaIdLength = alphaIdentifier.GetLength();
				if ( alphaIdLength )
					{
					// Get the alpha id
					TPtrC8 sourceString;
					sourceString.Set( 
					alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
					// Convert and set the alpha id
					TSatUtility::SetAlphaId( sourceString , 
					playToneV2.iAlphaId.iAlphaId ); 
					}
    
				// Set Alpha ID status
				if ( playToneV2.iAlphaId.iAlphaId.Length() )
					{
					playToneV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_COMPLETENOTIFYL_3, "CSAT: CSatNotifyPlayTone::CompleteNotifyL, Alpha ID is NULL");
					playToneV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
					}
				}

			// Tone selection (optional)
			playToneV2.iTone = RSat::KToneNotSet;
			CTlv tone;
			returnValue = berTlv.TlvByTagValue( &tone, KTlvToneTag );
			if ( KErrNotFound != returnValue )
				{
				playToneV2.iTone = ( RSat::TTone ) tone.GetShortInfo( 
				    ETLV_Tone );
			    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_COMPLETENOTIFYL_4, "CSAT: CSatNotifyPlayTone::CompleteNotifyL, Tone Selection: %d", playToneV2.iTone);
				}
        
			// Duration of the tone (optional)
			TSatUtility::FillDurationStructure( berTlv, 
			    playToneV2.iDuration );

			// Icond Id (optional)
			TSatUtility::FillIconStructure( berTlv,
			    playToneV2.iIconId );
				
			}// End of if ( KErrNone == aErrorCode )
        else
        	{
        	ret = aErrorCode;
        	}

		iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
		}// End of if ( CSatTsy::ESatReqHandleUnknown != reqHandle )			
	else 
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_COMPLETENOTIFYL_5, "CSAT: CSatNotifyPlayTone::CompleteNotifyL, Request not ongoing");
		// Request not on, returning response immediately
		additionalInfo.Zero();
		additionalInfo.Append( KNoCause );
		CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd,
			additionalInfo );					
    	}

    return ret;
	
	}


// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifyPlayTone::TerminalResponseL
        ( 
        TDes8* aRsp
        )
	{    
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_TERMINALRESPONSEL_1,  "CSAT: CSatNotifyPlayTone::TerminalResponseL" );

    TInt ret( KErrNone );
    TBuf<1>additionalInfo;
    additionalInfo.Append( 0 );
    RSat::TPlayToneRspV1Pckg* aRspPckg = 
        reinterpret_cast<RSat::TPlayToneRspV1Pckg*>( aRsp );
    RSat::TPlayToneRspV1& rspV1 = ( *aRspPckg ) ();
    
    // Get Proactive command number
    TUint8 pCmdNumber( rspV1.PCmdNumber() );
	
    // Check that general result value is valid
    // Note: When the phone is in silent mode, KMeUnableToProcessCmd
    // is returned as a result, with no additional info.
	if ( ( RSat::KSuccess != rspV1.iGeneralResult ) && 
		 ( RSat::KPSessionTerminatedByUser  != rspV1.iGeneralResult ) && 
		 ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult ) && 
		 ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult ) && 
		 ( RSat::KCmdDataNotUnderstood != rspV1.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_TERMINALRESPONSEL_2,  "CSAT: CSatNotifyPlayTone::TerminalResponseL, Invalid General Result" );
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is ME (Mobile Equipment) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( 0 != rspV1.iAdditionalInfo.Length() )
            {
            additionalInfo.Zero();
            additionalInfo.Append( rspV1.iAdditionalInfo[0] ); 
			}
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_TERMINALRESPONSEL_3,  "CSAT: CSatNotifyPlayTone::TerminalResponseL, Invalid Additional Info" );
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_TERMINALRESPONSEL_4,  "CSAT: CSatNotifyPlayTone::TerminalResponseL: AdditionalInfo: %S", additionalInfo );

    CreateTerminalRespL( pCmdNumber, static_cast< TUint8 >( 
    	rspV1.iGeneralResult ), additionalInfo );                            

    return ret;
	}	

// -----------------------------------------------------------------------------
// CSatNotifyPlayTone::CreateTerminalRespL
// Constructs PlayTone specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyPlayTone::CreateTerminalRespL
	    ( 
        TUint8	 aPCmdNumber,		    
        TUint8	 aGeneralResult,	    
        TDesC16&  aAdditionalInfo            
	    )    	
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYPLAYTONE_CREATETERMINALRESPL_1, "CSAT: CSatNotifyPlayTone::CreateTerminalRespL");
    TTlv tlvSpecificData;
    tlvSpecificData.AddTag( KTlvResultTag );
	//General result
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
            case RSat::KCmdDataNotUnderstood:
                {
                break;
                }
            default:
                {    
                tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] );
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
    
// End of File
