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
// Name        : CSatNotifySetupEventList.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Set Up event list notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySetUpEventListTraces.h"
#endif

#include <satcs.h>                   // Etel SAT IPC definitions
#include "CSatTsy.h"                 // Tsy class header
#include "CSatNotifySetUpEventList.h"// Tsy class header
#include "CSatNotificationsTsy.h"    // Class header
#include "CBerTlv.h"                 // Ber Tlv data handling
#include "TTlv.h"					 // TTlv class 
#include "CSatDataPackage.h"         // Parameter packing 
#include "TSatUtility.h"             // Utilities
#include "CSatTsyReqHandleStore.h"   // Request handle class
#include "cmmmessagemanagerbase.h"   // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpEventList* CSatNotifySetUpEventList::NewL
		(
		CSatNotificationsTsy* aNotificationsTsy
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_NEWL_1, "CSAT: CSatNotifySetUpEventList::NewL");
   	CSatNotifySetUpEventList* const satNotifySetUpEventList = 
        new ( ELeave ) CSatNotifySetUpEventList( aNotificationsTsy );
    CleanupStack::PushL( satNotifySetUpEventList );
    satNotifySetUpEventList->ConstructL();
    CleanupStack::Pop( satNotifySetUpEventList );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_NEWL_2, "CSAT: CSatNotifySetUpEventList::NewL, end of method");
    return satNotifySetUpEventList;	
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::~CSatNotifySetUpEventList
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpEventList::~CSatNotifySetUpEventList
		(
		// None
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_DTOR_1, "CSAT: CSatNotifySetUpEventList::~CSatNotifySetUpEventList");
    }    
// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::CSatNotifySetUpEventList
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpEventList::CSatNotifySetUpEventList
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpEventList::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySetUpEventList::ConstructL
		(
		// None
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_CONSTRUCTL_1, "CSAT: CSatNotifySetUpEventList::ConstructL, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifySetupEventList::Notify
// Save the request handle type for notification requested by ETel server
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySetUpEventList::Notify
        ( 
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_NOTIFY_1, "CSAT: CSatNotifySetUpEventList::Notify");
    // Save data pointer to client side for completion
    iSetUpEventListV1Pckg = reinterpret_cast<RSat::TSetUpEventListV1Pckg*>( 
        aPackage.Des1n() );

    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySetUpEventListPCmdReqType );

    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSetUpEventList );   

    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CSatNotifySetupEventList::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySetUpEventList request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpEventList::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_CANCELNOTIFICATION_1, "CSAT: CSatNotifySetUpEventList::CancelNotification");
    
    // Reset the request handle
    iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifySetUpEventListPCmdReqType );

    // Reset the data pointer
    iSetUpEventListV1Pckg = NULL;
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
           
    return KErrNone;      
    }

// -----------------------------------------------------------------------------
// CSatNotifySetupEventList::CompleteNotifyL
// This method completes an outstanding asynchronous 
// SetUpEventList request. 
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySetUpEventList::CompleteNotifyL
        ( 
        CSatDataPackage* aDataPackage,
        TInt aErrorCode  
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_COMPLETENOTIFYL_1, "CSAT: CSatNotifySetUpEventList::CompleteNotifyL");
    TInt ret( KErrNone );
    TUint8 generalResult( RSat::KSuccess );

    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
	TBuf<1> additionalInfo;
    // Reset req handle. Returns the req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        TsyReqHandle( CSatTsy::ESatNotifySetUpEventListPCmdReqType );
	        
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
        commandDetails.Data() );
        
	TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
	
    // In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {    
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode )
            {
            // Let's fill the set up event list structure
            RSat::TSetUpEventListV1& setUpEventListV1 = 
                ( *iSetUpEventListV1Pckg )();

            setUpEventListV1.SetPCmdNumber( pCmdNumber );

            CTlv eventListTlv;
            ret = berTlv.TlvByTagValue( &eventListTlv, KTlvEventListTag );

            //Initialize Type to Not set
            setUpEventListV1.iType = RSat::ESetUpEventListTypeNotSet;
        
            // If the returnValue is KErrNone
            if ( KErrNone == ret )
                {
                iEvents = 0;
                TPtrC8 setUpEventList = eventListTlv.GetData( ETLV_EventList );
                TUint16 eventListLength = eventListTlv.GetLength();
                if ( !eventListLength )
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_COMPLETENOTIFYL_2, "CSAT: CSatNotifySetUpEventList::CompleteNotifyL, Event list empty");
                    setUpEventListV1.iType = RSat::ERemoveExistingEventList;
                    // Remove events from TSY:s list, No events = 0
                    iNotificationsTsy->iSatTsy->SetUpEventList( 0 );
                    }
                else
                    {
                    setUpEventListV1.iType = RSat::EUpdateEventList;
                    TInt8 i( 0 );
                    TBuf8<14> eventListBuf;
                    eventListBuf.FillZ( 14 );
                    for ( i = 0; i < eventListLength; i++ )
                        {
                        if(  KLocalConnection < setUpEventList[i] )
                            {
                            // Invalid event code
                            generalResult = RSat::KCmdBeyondMeCapabilities;
                            }
                        else if( !eventListBuf[setUpEventList[i]] )
                            {
                            // Each event type shall not appear more than ones
                            eventListBuf[ setUpEventList[i] ] = 0x01;

                            switch( setUpEventList[i] )
                                {
                                case KMTCall:
                                	{
                                    iEvents += RSat::KMTCall;
                                    break;
                                	}
                                case KCallConnected:
                                    {
                                    iEvents += RSat::KCallConnected;
                                    break;
                                    }
                                case KCallDisconnected:
                                	{
                                    iEvents += RSat::KCallDisconnected;
                                    break;
                                	}
                                case KLocationStatus:
                                    {
                                    iEvents += RSat::KLocationStatus;
                                    break;
                                	}
                                case KUserActivity:
                                	{
                                    setUpEventListV1.iEvents 
                                        += RSat::KUserActivity;
                                    iEvents += RSat::KUserActivity;
                                    break;
                                	}
                                case KIdleScreenAvailable:
                                    {
                                    setUpEventListV1.iEvents 
                                        += RSat::KIdleScreenAvailable;
                                    iEvents += RSat::KIdleScreenAvailable;
                                    break;
                                    }
                                case KCardReaderStatus:
                                    {
                                    setUpEventListV1.iEvents 
                                       += RSat::KCardReaderStatus;
                                    iEvents += RSat::KCardReaderStatus;
                                    break;
                                    }
                                case KLanguageSelection:
                                	{
                                    setUpEventListV1.iEvents 
                                        += RSat::KLanguageSelection;
                                    iEvents += RSat::KLanguageSelection;
                                    break;
                                	}
                                case KBrowserTermination:
                                    {
                                    setUpEventListV1.iEvents 
                                        += RSat::KBrowserTermination;
                                    iEvents += RSat::KBrowserTermination; 
                                    break;
                                    }
                                case KDataAvailable:
                                    {
                                    setUpEventListV1.iEvents 
                                        += RSat::KDataAvailable;
                                    iEvents += RSat::KDataAvailable; 
                                    break;
                                    }
                                case KChannelStatus:
                                    {
                                    setUpEventListV1.iEvents 
                                        += RSat::KChannelStatus;
                                    iEvents += RSat::KChannelStatus; 
                                    break;
                                    }
                                case KAccessTechnologyChange:
                                    {
                                    iEvents += RSat::KAccessTechnologyChange;
                                    break;
                                    }
                                case KDisplayParamsChanges:
                                    {
                                    setUpEventListV1.iEvents 
                                        += RSat::KDisplayParamsChanges;
                                    iEvents += RSat::KDisplayParamsChanges;
                                    break;
                                    }
                                case KLocalConnection:
                                    {
                                    setUpEventListV1.iEvents 
                                        += RSat::KLocalConnection;
                                    iEvents += RSat::KLocalConnection;
                                    break;
                                    }
                                default:
                                	{
                                    break;
                                	}
                                } // switch ( setUpEventList[i] ) 
                            } // else if( !eventListBuf[setUpEventList[i]] )
                        } // for ( i = 0; i < eventListLength; i++ )
                    } // else ( if ( !eventListLength ) )
                }// if( KErrNone == ret )
            else
            	{
            	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_COMPLETENOTIFYL_3, "CSAT: CSatNotifySetUpEventList::CompleteNotifyL, Event list missing");
           		// Event list is missing
            	generalResult = RSat::KErrorRequiredValuesMissing;
            	}    
            }// if ( KErrNone == aErrorCode )        	    
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_COMPLETENOTIFYL_4, "CSAT: CSatNotifySetUpEventList::CompleteNotifyL, Error occurred in LSAT");
            // Error code received from LicenceeSimAtkTsy
            return ret;
            }
        
        } // if ( CSatTsy::ESatReqHandleUnknown != reqHandle )    
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_COMPLETENOTIFYL_5, "CSAT: CSatNotifySetUpEventList::CompleteNotifyL, Request not ongoing");
        generalResult = RSat::KMeUnableToProcessCmd;
        }

    if ( RSat::KSuccess != generalResult )
        {
        additionalInfo.Zero();
        additionalInfo.Append( KNoCause );
        CreateTerminalRespL( pCmdNumber, generalResult, additionalInfo ); 
        }
    else
        {
        // Reset req handle. Returns the deleted req handle
	    TTsyReqHandle reqHandle = 
	        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
	        CSatTsy::ESatNotifySetUpEventListPCmdReqType );
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetupEventList::TerminalResponseL
// Called by ETel server, passes terminal response to SIM card
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpEventList::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_TERMINALRESPONSEL_1, "CSAT:CSatNotifySetUpEventList::TerminalResponseL");

    TInt ret( KErrNone );
    TBuf<1> additionalInfo;

    RSat::TSetUpEventListRspV1Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSetUpEventListRspV1Pckg*>( aRsp );
    RSat::TSetUpEventListRspV1& rspV1 = ( *aRspPckg ) ();

    // Checking first that transaction ID is the same as in proactive message
    TUint8 pCmdNumber( rspV1.PCmdNumber() );

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult ) 
        && ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult )
		&& ( RSat::KPartialComprehension != rspV1.iGeneralResult )
		&& ( RSat::KMissingInformation != rspV1.iGeneralResult )
		&& ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult )
		&& ( RSat::KCmdTypeNotUnderstood != rspV1.iGeneralResult )
		&& ( RSat::KCmdDataNotUnderstood != rspV1.iGeneralResult )
		&& ( RSat::KCmdNumberNotKnown != rspV1.iGeneralResult )
        && ( RSat::KErrorRequiredValuesMissing != rspV1.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_TERMINALRESPONSEL_2, "CSAT:CSatNotifySetUpEventList::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

	if ((RSat::KMeUnableToProcessCmd == rspV1.iGeneralResult) && (RSat::KMeProblem != rspV1.iInfoType))
	{
	// it's obligatory to provide additional info and KMeProblem flag for KMeUnableToProcessCmd error code
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_TERMINALRESPONSEL_3, "CSAT:CSatNotifySetUpEventList::TerminalResponseL, KMeProblem flag is not specified");
    ret = KErrCorrupt;   
	}
    
    // If there is Me (Mobile Entity) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType ) )
        {
        // Check the length of additional info
        if ( 0 != rspV1.iAdditionalInfo.Length() )
            {            
            additionalInfo.Append( ( TUint8 )rspV1.iAdditionalInfo[0] );  
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_TERMINALRESPONSEL_4, "CSAT:CSatNotifySetUpEventList::TerminalResponseL, Invalid Additional Info");
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
    
    if( RSat::KSuccess == rspV1.iGeneralResult )
        {
        // Set events to TSY:s list
        iNotificationsTsy->iSatTsy->SetUpEventList( iEvents );
        }
        
    // Creating the terminal response message
    TInt response = CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
        rspV1.iGeneralResult ), additionalInfo ); 
    
    if(KErrNone == ret)
    	ret = response;
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetupEventList::CreateTerminalRespL
// Constructs SetUpEventList specific part of terminal response and calls 
// DOS to send the actual message. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpEventList::CreateTerminalRespL
        ( 
        TUint8  aPCmdNumber,        
        TUint8  aGeneralResult,      
        TDesC16& aAdditionalInfo       
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPEVENTLIST_CREATETERMINALRESPL_1, "CSAT: CSatNotifySetUpEventList::CreateTerminalRespL");
    // Create and append response data
    TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    // General result
    tlvSpecificData.AddByte( aGeneralResult );
    
    if ( ( RSat::KSuccess != aGeneralResult ) &&
         ( RSat::KCmdBeyondMeCapabilities != aGeneralResult ) &&
         ( RSat::KErrorRequiredValuesMissing != aGeneralResult ) )
    	{
    	if ( aAdditionalInfo.Length()>0 )
    		{
    		tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] );
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

