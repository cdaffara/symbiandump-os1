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
// Name        : CSatNotifySendDtmf.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Send dtmf notification functionality of Sat Tsy
// Version     : 1.0
//


 
//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySendDtmfTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifySendDtmf.h"     // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySendDtmf* CSatNotifySendDtmf::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_NEWL_1, "CSAT: CSatNotifySendDtmf::NewL");
   	CSatNotifySendDtmf* const satNotifySendDtmf = 
        new ( ELeave ) CSatNotifySendDtmf( aNotificationsTsy );
    CleanupStack::PushL( satNotifySendDtmf );
    satNotifySendDtmf->ConstructL();
    CleanupStack::Pop( satNotifySendDtmf );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_NEWL_2, "CSAT: CSatNotifySendDtmf::NewL, end of method");
    return satNotifySendDtmf;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::~CSatNotifySendDtmf
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendDtmf::~CSatNotifySendDtmf
        ( 
		// None
        )
    { 
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_DTOR_1, "CSAT: CSatNotifySendDtmf::~CSatNotifySendDtmf");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::CSatNotifySendDtmf
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySendDtmf::CSatNotifySendDtmf
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySendDtmf::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_CONSTRUCTL_1, "CSAT: CSatNotifySendDtmf::ConstructL, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::Notify
// Save the request handle type for notification requested by ETel server
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySendDtmf::Notify
        ( 
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_NOTIFY_1, "CSAT: CSatNotifySendDtmf::Notify");
    // Save data pointer to client side for completion
    iSendDtmfV1Pckg = reinterpret_cast<RSat::TSendDtmfV1Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySendDtmfPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSendDtmf );   

    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySetUpCall request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendDtmf::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_CANCELNOTIFICATION_1, "CSAT: CSatNotifySendDtmf::CancelNotification");
    // Reset the request handle
    iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifySendDtmfPCmdReqType );
    // Reset the data pointer
    iSendDtmfV1Pckg = NULL;
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;      
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::CompleteNotifyL
// This method completes an outstanding asynchronous 
// SetUpCall request. 
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySendDtmf::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage,
        TInt aErrorCode  
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_COMPLETENOTIFYL_1, "CSAT: CSatNotifySendDtmf::CompleteNotifyL");
    TInt returnValue( KErrNone );
    TInt ret( KErrNone );
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
	TBuf<1> additionalInfo;
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifySendDtmfPCmdReqType );

    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    
    // Store command details tlv
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( commandDetails.Data() );
    
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
                
    // In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode )
            {
            RSat::TSendDtmfV1& sendDtmfV1 = ( *iSendDtmfV1Pckg )();

            // Command number
            sendDtmfV1.SetPCmdNumber( pCmdNumber );
            // Alpha id string (optional)
            sendDtmfV1.iAlphaId.iAlphaId.Zero();
            CTlv alphaIdentifier;
            returnValue = berTlv.TlvByTagValue( &alphaIdentifier, 
                                    KTlvAlphaIdentifierTag );
            // If alpha id string exist
            if( KErrNone == returnValue ) 
                {
                TUint16 alphaIdLength = alphaIdentifier.GetLength();
    
                if ( RSat::KAlphaIdMaxSize < alphaIdLength )
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_COMPLETENOTIFYL_2, "CSAT: CSatNotifySendDtmf::CompleteNotifyL, Alpha ID length exceeded");
                    // String too long
                    additionalInfo.Zero();
                    additionalInfo.Append( KNoCause );                  
                    CreateTerminalRespL( pCmdNumber, 
                        RSat::KCmdBeyondMeCapabilities, additionalInfo );                    
                    ret = KErrCorrupt;
                    }
                else if ( alphaIdLength )
                    {
                    TPtrC8 temp;
                    // Get the alpha id
                    temp.Set( alphaIdentifier.GetData( 
                        ETLV_AlphaIdentifier ) );
                    // Convert and set the alpha id
                    TSatUtility::SetAlphaId( temp, 
                        sendDtmfV1.iAlphaId.iAlphaId );
                    sendDtmfV1.iAlphaId.iStatus = RSat::EAlphaIdProvided;
                    }
                else
                	{
                	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_COMPLETENOTIFYL_3, "CSAT: CSatNotifySendDtmf::CompleteNotifyL, Alpha ID is NULL");
                    sendDtmfV1.iAlphaId.iStatus = RSat::EAlphaIdNull;                     
                    }
                }
            // Alpha id not present
            else 
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_COMPLETENOTIFYL_4, "CSAT: CSatNotifySendDtmf::CompleteNotifyL, Alpha ID not present");
                sendDtmfV1.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
                }

            // Dtmf string length, 8-bit string, mandatory
            sendDtmfV1.iDtmfString.Zero();
            CTlv sendDtmf;
            returnValue = berTlv.TlvByTagValue( &sendDtmf, 
            KTlvDtmfStringTag );
            
            if ( KErrNone == returnValue )     
                {
	            TUint8 generalResult( RSat::KSuccess );
	            // length - 1 for string
	            TUint16 dtmfStringLength = (TUint16) ( sendDtmf.GetLength() );
	            
	            // If first byte of the dtmf string is 0xFF it means that dtmf string
	            // is empty and we have to return general result 
	            if ( ( dtmfStringLength && ( sendDtmf.Data()[2] == 0xFF ) ) ||
	                !dtmfStringLength )
	                {
	                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_COMPLETENOTIFYL_5, "CSAT: CSatNotifySendDtmf::CompleteNotifyL, Data not understood");
	                generalResult = RSat::KCmdDataNotUnderstood; 
	                ret = KErrCorrupt;
	                }
	            else if( RSat::KDtmfStringMaxSize < dtmfStringLength )
	                {
	                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_COMPLETENOTIFYL_6, "CSAT: CSatNotifySendDtmf::CompleteNotifyL, Cmd beyond ME capabilities");
	                generalResult = RSat::KCmdBeyondMeCapabilities;
	                ret = KErrCorrupt;
	                }
	            else
	                {
	                sendDtmfV1.iDtmfString.Append( sendDtmf.GetData( ETLV_DtmfString ) );
	                }

	            if( KErrCorrupt == ret )
	                {
	                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_COMPLETENOTIFYL_7, "CSAT: CSatNotifySendDtmf::CompleteNotifyL, DTMF length exceeded");
                    // String too long
                    additionalInfo.Zero();
                    additionalInfo.Append( KNoCause );
                    CreateTerminalRespL( pCmdNumber, generalResult, 
                        additionalInfo );
	                }
                }
            else
                {
                additionalInfo.Zero();
                additionalInfo.Append( KNoCause );
                CreateTerminalRespL( pCmdNumber, RSat::KErrorRequiredValuesMissing, 
                    additionalInfo );								
                ret = KErrCorrupt;
                }

            // Iconid 
            if ( KErrNone == ret )
                {
                TSatUtility::FillIconStructure( berTlv, sendDtmfV1.iIconId );
                }
            }    
        else
        	{
        	ret = aErrorCode;
        	}
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
        }
    else
        {
		additionalInfo.Zero();
		additionalInfo.Append( KNoCause );
		CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd, 
		    additionalInfo );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::TerminalResponseL
// Called by ETel server, passes terminal response to SIM card
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendDtmf::TerminalResponseL
        ( 
        TDes8* aRsp 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_TERMINALRESPONSEL_1, "CSAT:CSatNotifySendDtmf::TerminalResponseL");
    
    TInt ret( KErrNone );
    TBuf<1> additionalInfo;

    RSat::TSendDtmfRspV1Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSendDtmfRspV1Pckg*>( aRsp );
    RSat::TSendDtmfRspV1& rspV1 = ( *aRspPckg ) ();

    TUint8 pCmdNumber( rspV1.PCmdNumber() );

    // Check that general result values are valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult ) && 
         ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult ) && 
         ( RSat::KCmdTypeNotUnderstood != rspV1.iGeneralResult ) && 
         ( RSat::KCmdDataNotUnderstood != rspV1.iGeneralResult ) && 
         ( RSat::KCmdNumberNotKnown != rspV1.iGeneralResult ) && 
         ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult ) && 
         ( RSat::KPartialComprehension != rspV1.iGeneralResult ) && 
         ( RSat::KMissingInformation != rspV1.iGeneralResult ) && 
         ( RSat::KSuccessRequestedIconNotDisplayed != rspV1.iGeneralResult ) && 
         ( RSat::KPSessionTerminatedByUser != rspV1.iGeneralResult ) && 
         ( RSat::KErrorRequiredValuesMissing != rspV1.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_TERMINALRESPONSEL_2, "CSAT:CSatNotifySendDtmf::TerminalResponseL, Invalid General result");
        ret = KErrCorrupt;
        }
        
    if( RSat::KMeProblem == rspV1.iInfoType )
        {
        if ( rspV1.iAdditionalInfo.Length() )
            {
            additionalInfo.Zero();
        	additionalInfo.Append( rspV1.iAdditionalInfo[0] );
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_TERMINALRESPONSEL_3, "CSAT:CSatNotifySendDtmf::TerminalResponseL, Invalid Info Type");
            ret = KErrCorrupt;
            }
        }

    // Creating the terminal response message
    TInt response = CreateTerminalRespL( pCmdNumber, ( TUint8 )rspV1.iGeneralResult, 
        additionalInfo );
    
    if( KErrNone == ret )
    	{
    	ret = response;
    	}
                         
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySendDtmf::CreateTerminalRespL
// Constructs SendDtmf specific part of terminal response and calls 
// DOS to send the actual message. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySendDtmf::CreateTerminalRespL
        ( 
        TUint8  aPCmdNumber,         
        TUint8  aGeneralResult,      
        TDesC16& aAdditionalInfo          
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSENDDTMF_CREATETERMINALRESPL_1, "CSAT: CSatNotifySendDtmf::CreateTerminalRespL");
    
    TTlv tlvSpecificData;
    // Append general result tag
    tlvSpecificData.AddTag( KTlvResultTag );
    // Append general result
    tlvSpecificData.AddByte( aGeneralResult );
  
    if ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        {
        // UnSuccessful result neccessitating additional info byte
        if ( aAdditionalInfo.Length() )
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

