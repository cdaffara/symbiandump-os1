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
// Name        : CSatNotifyLanguageNotification.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Language notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyLanguageNotificationTraces.h"
#endif

#include <satcs.h>                  		// Etel SAT IPC definitions
#include "CSatTsy.h"                		// Tsy class header
#include "CSatNotifyLanguageNotification.h" // Tsy class header
#include "CSatNotificationsTsy.h"   	    // Class header
#include "CBerTlv.h"                		// Ber Tlv data handling
#include "TTlv.h"							// TTlv class
#include "CSatDataPackage.h"         		// Parameter packing 
#include "TSatUtility.h"            		// Utilities
#include "CSatTsyReqHandleStore.h"  		// Request handle class
#include "cmmmessagemanagerbase.h" 			// Message manager class for 
											// forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyLanguageNotification* CSatNotifyLanguageNotification::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_NEWL_1, "CSAT: CSatNotifyLanguageNotification::NewL");
   	CSatNotifyLanguageNotification* const satNotifyLanguageNotification = 
        new ( ELeave ) CSatNotifyLanguageNotification( aNotificationsTsy );
    CleanupStack::PushL( satNotifyLanguageNotification );
    satNotifyLanguageNotification->ConstructL();
    CleanupStack::Pop( satNotifyLanguageNotification );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_NEWL_2, "CSAT: CSatNotifyLanguageNotification::NewL, end of method");
    return satNotifyLanguageNotification;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::CSatNotifyLanguageNotification
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyLanguageNotification::~CSatNotifyLanguageNotification
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_DTOR_1, "CSAT: CSatNotifyLanguageNotification::~CSatNotifyLanguageNotification");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::CSatNotifyLanguageNotification
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyLanguageNotification::CSatNotifyLanguageNotification
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyLanguageNotification::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_CONSTRUCTL_1, "CSAT: CSatNotifyLanguageNotification::ConstructL");
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::Notify
// Save the request handle type for notification requested by ETel server
// -----------------------------------------------------------------------------
// 
TInt CSatNotifyLanguageNotification::Notify
        ( 
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_NOTIFY_1, "CSAT: CSatNotifyLanguageNotification::Notify");
    // Save data pointer to client side for completion
    iLanguageNotificationV2Pckg = reinterpret_cast<RSat::
    	TLanguageNotificationV2Pckg*>( aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyLanguageNotificationPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KLanguageNotification );   

    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyLanguageNotification request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLanguageNotification::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_CANCELNOTIFICATION_1, "CSAT: CSatNotifyLanguageNotification::CancelNotification");
    // Reset the request handle
    iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifyLanguageNotificationPCmdReqType );
    // Reset the data pointer
    iLanguageNotificationV2Pckg = NULL;
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;      
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::CompleteNotifyL
// This method completes an outstanding asynchronous 
// LanguageNotification request. 
// -----------------------------------------------------------------------------
// 
TInt CSatNotifyLanguageNotification::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage,
        TInt aErrorCode  
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_COMPLETENOTIFYL_1, "CSAT: CSatNotifyLanguageNotification::CompleteNotifyL");

    TInt returnValue( KErrNone );
    TInt ret( KErrNone );
	TBuf<1> additionalInfo;
	
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
		
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifyLanguageNotificationPCmdReqType );
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
            RSat::TLanguageNotificationV2& languageNotificationV2 = 
                                    ( *iLanguageNotificationV2Pckg )();
            // Transaction id
            languageNotificationV2.SetPCmdNumber( pCmdNumber );

            // Initialisate 
            languageNotificationV2.iNotificationType = 
                RSat::ENotificationTypeNotSet;
        
            // Get command qualifier
            TUint8 cmdQualifier(commandDetails.GetShortInfo( 
                ETLV_CommandQualifier ) );

            if ( !cmdQualifier )
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_COMPLETENOTIFYL_2, "CSAT: CSatNotifyLanguageNotification::CompleteNotifyL, No specific language used");
                // No specific language used
                languageNotificationV2.iNotificationType = 
                    RSat::ENonSpecificLangNotification;
                }
            else if ( KSpecificLanguage == cmdQualifier )
                {
                // Currently used language 
		        languageNotificationV2.iNotificationType = 
		            RSat::ESpecificLangNotification; 
                }
            else
            {
            // None.
            }

            CTlv language;
            returnValue = berTlv.TlvByTagValue( &language, KTlvLanguageTag );

            // If the Language tag is found
            if ( KErrNone == returnValue )
                {
                languageNotificationV2.iLanguage = language.GetLongInfo( 
                	ETLV_Language );
                }    
            }// End of if( KErrNone == aErrorCode )
        else
        	{
        	ret = aErrorCode;
        	}
            
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
        }// End of if( CSatTsy::ESatReqHandleUnknown != reqHandle )
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_COMPLETENOTIFYL_3, "CSAT: CSatNotifyLanguageNotification::CompleteNotifyL, Request not ongoing");
        additionalInfo.Zero();
        additionalInfo.Append( KNoCause );
        CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd,
            additionalInfo );       
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::TerminalResponseL
// Called by ETel server, passes terminal response to SIM card
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLanguageNotification::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_TERMINALRESPONSEL_1, "CSAT:CSatNotifyLanguageNotification::TerminalResponseL");
    TInt ret( KErrNone );
    TBuf<1> additionalInfo;
    additionalInfo.FillZ(1);

    RSat::TLanguageNotificationRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TLanguageNotificationRspV2Pckg*>( aRsp );
    RSat::TLanguageNotificationRspV2& rspV1 = ( *aRspPckg ) ();

    TUint8 pCmdNumber( rspV1.PCmdNumber() );

    CreateTerminalRespL( pCmdNumber, RSat::KSuccess, additionalInfo ); 
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLanguageNotification::CreateTerminalRespL
// Constructs LanguageNotification specific part of terminal response and calls 
// DOS to send the actual message. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLanguageNotification::CreateTerminalRespL
        ( 
        TUint8  aPCmdNumber,         
        TUint8  aGeneralResult,     
        TDesC16& aAdditionalInfo    
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLANGUAGENOTIFICATION_CREATETERMINALRESPL_1, "CSAT:CSatNotifyLanguageNotification::CreateTerminalRespL");
    TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    // General result
    tlvSpecificData.AddByte( aGeneralResult );
     
    if ( RSat::KSuccess != aGeneralResult ) 
        {
        tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] );
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
