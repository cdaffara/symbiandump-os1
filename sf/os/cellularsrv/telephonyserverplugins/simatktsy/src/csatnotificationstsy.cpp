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
// Name        : CSatNotificationsTsy.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Notifications-related functionality of Sat Tsy
// Version     : 1.0
//




//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csatnotificationstsyTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatDataPackage.h"        // Parameter packing 
#include "CSatTsy.h"                // Sat Tsy class
#include "CSatCCTsy.h"              // Sat Call Contol Tsy class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req
#include "CSatNotificationsTsy.h"   // Class header
#include "CSatNotifyDisplayText.h"  // Display text specific notify class
#include "CSatNotifyGetInkey.h"     // Get inkey specific notify class
#include "CSatNotifySimSessionEnd.h"// Sim session end specific notify class
#include "CSatNotifyGetInput.h"		// Get input specific notify class
#include "CSatNotifyPlayTone.h"		// Play tone specific notify class
#include "CSatNotifySetUpMenu.h"	// Set-up menu specific notify class
#include "CSatNotifySelectItem.h"	// Select item specific notify class
#include "CSatNotifySendSm.h"		// Send SMS specific notify class
#include "CSatNotifySendUssd.h"		// Send USSD specific notify class
#include "CSatNotifySendSs.h"		// Send SS specific notify class
#include "CSatNotifySetUpIdleModeText.h"// Set up idle mode notify class
#include "CSatNotifyLaunchBrowser.h"// Launch browser specific notify class
#include "CSatNotifyCallControlRequest.h"// Call control notify class
#include "CSatNotifySetUpCall.h"    // Set up call specific notify class
#include "CSatNotifySetUpEventList.h"    // Set up event list notify class
#include "CSatNotifyTimerMgmt.h"    // Timer management specific notify class
#include "CSatNotifyLanguageNotification.h"// Language notification notify class
#include "CSatNotifySendDtmf.h"     // Send Dtmf notification  notify class
#include "CSatNotifyRefresh.h"      // Refresh notification  notify class
#include "CSatNotifyPollingOff.h"   // Polling off notification  notify class
#include "CSatNotifyPollInterval.h" // Poll interval notification  notify class
#include "CSatNotifyLocalInfo.h"    // Local info notification  notify class
#include "CSatNotifyRefresh.h"      // Refresh notification  notify class
#include "CSatNotifyOpenChannel.h"	// Open channel notification  notify class
#include "CSatNotifyGetChannelStatus.h"// Get channel status notify notify class
#include "CSatNotifyMoSmControlRequest.h"// MO SM Control notify class      
#include "CSatNotifyCloseChannel.h"	// Close channel notification  notify class
#include "CSatNotifySendData.h"		// Send Data notification  notify class
#include "CSatNotifyReceiveData.h"  // Receive data notification  notify class
#include "CSatNotifyMoreTime.h"		// More time notification  notify class
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "CBerTlv.h"                // Ber Tlv
#include "TTlv.h"					// TTlv class
#include "msattsy_ipcdefs.h"		// Sat Tsy specific request types
#include "TSatUtility.h"            // Utilities


// -----------------------------------------------------------------------------
// CSatNotificationsTsy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatNotificationsTsy* CSatNotificationsTsy::NewL
        (    
        CSatTsy* aSatTsy,
		CSatTsyReqHandleStore* aSatReqHandleStore
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_NEWL_1, "CSAT: CSatNotificationsTsy::NewL");
    CSatNotificationsTsy* const satNotificationsTsy = 
        new ( ELeave ) CSatNotificationsTsy( aSatTsy, aSatReqHandleStore );

    CleanupStack::PushL( satNotificationsTsy );
    satNotificationsTsy->ConstructL();
    CleanupStack::Pop();
    
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_NEWL_2, "CSAT: CSatNotificationsTsy::NewL, end of method");
    return satNotificationsTsy;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::~CSatNotificationsTsy
// Destructor
// -----------------------------------------------------------------------------
//    
CSatNotificationsTsy::~CSatNotificationsTsy
        (    
        void   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_DTOR_1, "CSAT: CSatNotificationsTsy::~CSatNotificationsTsy");
    
    delete iSatTimer;

    // Specific notification objects
    delete iNotifyDisplayText;
    delete iNotifyGetInkey;
	delete iNotifyGetInput;
    delete iNotifyPlayTone;
	delete iNotifySetUpMenu;
    delete iNotifySelectItem;
	delete iNotifySendSm;
	delete iNotifySendSs;
	delete iNotifySendUssd;
	delete iNotifySetUpIdleModeText;
    delete iNotifyLaunchBrowser; 
    delete iNotifyCallControlRequest; 
    delete iNotifySimSessionEnd;
    delete iNotifySetUpCall;
    delete iNotifySetUpEventList;
    delete iNotifySendDtmf;
    delete iNotifyPollingOff;
    delete iNotifyPollInterval;
    delete iNotifyLanguageNotification;
    delete iNotifyLocalInfo;
    delete iNotifyTimerMgmt;
    delete iNotifyRefresh;
	delete iNotifyOpenChannel;
	delete iNotifyGetChannelStatus;
    delete iNotifyMoSmControlRequest;
	delete iNotifyCloseChannel;
	delete iNotifySendData;
	delete iNotifyReceiveData;
	delete iNotifyMoreTime;

    // Unregister.
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotificationsTsyObjType, NULL );
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_DTOR_2, "CSAT: CSatNotificationsTsy::~CSatNotificationsTsy, end of method");
    }
    
// -----------------------------------------------------------------------------
// CSatNotificationsTsy::CSatNotificationsTsy
// C++ constructor
// -----------------------------------------------------------------------------
//
CSatNotificationsTsy::CSatNotificationsTsy
        (    
        CSatTsy* aSatTsy,
		CSatTsyReqHandleStore* aSatReqHandleStore
        ): iSatTsy( aSatTsy ), iSatReqHandleStore( aSatReqHandleStore )

    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CSatNotificationsTsy::ConstructL
        (    
        void
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_CONSTRUCTL_1, "CSAT: CSatNotificationsTsy::ConstructL\n" );

    // Register
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotificationsTsyObjType, this );

    // Initialize SAT Notifications objects
    iNotifyDisplayText = CSatNotifyDisplayText::NewL( this );
	iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyDisplayTextObjType, iNotifyDisplayText );
    
	iNotifyGetInkey = CSatNotifyGetInkey::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyGetInkeyObjType, iNotifyGetInkey );
	
	iNotifyGetInput = CSatNotifyGetInput::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyGetInputObjType, iNotifyGetInput );
	
	iNotifyPlayTone = CSatNotifyPlayTone::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyPlayToneObjType, iNotifyPlayTone );

	iNotifySetUpMenu = CSatNotifySetUpMenu::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySetUpMenuObjType, iNotifySetUpMenu );

	iNotifySelectItem = CSatNotifySelectItem::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySelectItemObjType, iNotifySelectItem );

	iNotifySendSm = CSatNotifySendSm::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySendSmsObjType, iNotifySendSm );

	iNotifySendSs = CSatNotifySendSs::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySendSsObjType, iNotifySendSs );

	iNotifySendUssd = CSatNotifySendUssd::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySendUssdObjType, iNotifySendUssd );

	iNotifySetUpIdleModeText = CSatNotifySetUpIdleModeText::NewL( this );
	iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySetUpIdleModeTextObjType, 
		iNotifySetUpIdleModeText );

	iNotifyLaunchBrowser = CSatNotifyLaunchBrowser::NewL( this );
	iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyLaunchBrowserObjType, 
		iNotifyLaunchBrowser );

	iNotifyCallControlRequest = CSatNotifyCallControlRequest::NewL( this );
	iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyCallControlRequestObjType, 
		iNotifyCallControlRequest );

	iNotifySimSessionEnd = CSatNotifySimSessionEnd::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySimSessionEndObjType, 
		iNotifySimSessionEnd );

    iNotifySetUpCall = CSatNotifySetUpCall::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySetUpCallObjType, iNotifySetUpCall );

    iNotifyRefresh = CSatNotifyRefresh::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyRefreshObjType, iNotifyRefresh );

    iNotifyPollInterval = CSatNotifyPollInterval::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyPollIntervalObjType, 
		iNotifyPollInterval );

    iNotifySendDtmf = CSatNotifySendDtmf::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySendDtmfObjType, iNotifySendDtmf );

    iNotifySetUpEventList = CSatNotifySetUpEventList::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySetUpEventListObjType, 
		iNotifySetUpEventList );

    iNotifyPollingOff = CSatNotifyPollingOff::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyPollingOffObjType, iNotifyPollingOff );

    iNotifyLocalInfo = CSatNotifyLocalInfo::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyLocalInfoObjType, iNotifyLocalInfo );

    iNotifyTimerMgmt = CSatNotifyTimerMgmt::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyTimerMgmtObjType, iNotifyTimerMgmt );

    iNotifyLanguageNotification = CSatNotifyLanguageNotification::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyLanguageNotificationObjType, 
		iNotifyLanguageNotification );

	iNotifyOpenChannel = CSatNotifyOpenChannel::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyOpenChannelObjType, 
			iNotifyOpenChannel );	
    
    iNotifyMoSmControlRequest = CSatNotifyMoSmControlRequest::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyMoSmControlRequestObjType, 
			iNotifyMoSmControlRequest );   
	
	iNotifyGetChannelStatus = CSatNotifyGetChannelStatus::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyGetChannelStatusObjType, 
			iNotifyGetChannelStatus );
	
	iNotifyCloseChannel = CSatNotifyCloseChannel::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyCloseChannelObjType, 
			iNotifyCloseChannel );
	
	iNotifySendData = CSatNotifySendData::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifySendDataObjType, iNotifySendData );
	
	iNotifyReceiveData = CSatNotifyReceiveData::NewL( this );
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyReceiveDataObjType, 
			iNotifyReceiveData );					
    
    iNotifyMoreTime = CSatNotifyMoreTime::NewL( this );
   	iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatNotifyMoreTimeObjType, iNotifyMoreTime );

    // Initialize timer, this is used for implementing timer management
    iSatTimer = CSatTimer::NewL( this );
    		
    // Initalize flag, this is used to check if proactiveCommand is ongoing
    iSatTimer->SetProactiveCommandOnGoingStatus( EFalse );

	// Set polling status to the use of the default value ( 25 sec. )
    iPollingOff = ETrue;
    
    // No long poll interval requests yet
    iLongPollIntervalReq = EFalse;

    // Buffering for Proactive commands that are not notified by SatServer
    NotifySatReadyForNotification( KPollInterval );
    NotifySatReadyForNotification( KPollingOff );
    NotifySatReadyForNotification( KMoreTime );
    NotifySatReadyForNotification( KTimerManagement );

    // Request IMEI code. Needed in provide local info proactive command.
    iSatTsy->MessageManager()->HandleRequestL( ESatTsyGetIMEI );
    
    // Checks if MO-SMS control should be activated
    //unnecassary IPC request as this is already called in CSatNotifyMoSmControlRequest::ConstructL 
    //iSatTsy->MessageManager()->HandleRequestL( ESatTsyQueryMoSmsControlActivated );
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_CONSTRUCTL_2, "CSAT: CSatNotificationsTsy::ConstructL, end of method" );
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::DoExtFunc
// Notifications-specific functionality of CSatTsy::DoExtFuncL
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsTsy::DoExtFuncL
        ( 
        const TTsyReqHandle aTsyReqHandle, 
        const TInt aIpc,                   
        const TDataPackage& aPackage      
        )
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_DOEXTFUNCL_1, "CSAT: CSatNotificationsTsy::DoExtFuncL\t IPC:%d,\t Handle:%d",aIpc, aTsyReqHandle);

    TInt ret ( KErrNone );

    TAny* dataPtr = aPackage.Ptr1();

    switch ( aIpc )
        {
	    case ESatTerminalRsp:
            {
			TerminalResponseL( reinterpret_cast< RSat::TPCmd* >( dataPtr ),
                 aPackage.Des2n(), aTsyReqHandle );
			break;
            }
        case ESatNotifyDisplayTextPCmd:
            {
            iNotifyDisplayText->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifySelectItemPCmd:
            {
            iNotifySelectItem->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifyGetInkeyPCmd:
            {
            iNotifyGetInkey->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifyGetInputPCmd:
            {
            iNotifyGetInput->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifyPlayTonePCmd:
            {
            iNotifyPlayTone->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifySetUpMenuPCmd:
            {
            iNotifySetUpMenu->Notify( aTsyReqHandle, aPackage );
            break;                
            }
        case ESatNotifySendSmPCmd:
            {
            iNotifySendSm->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifySendSsPCmd:
            {
            iNotifySendSs->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
		case ESatNotifySendUssdPCmd:
            {
            iNotifySendUssd->Notify( aTsyReqHandle, aPackage );
            break;     
            }
		case ESatNotifyProactiveSimSessionEnd:
            {
            iNotifySimSessionEnd->Notify( aTsyReqHandle, aPackage );
            break;
            }
        case ESatNotifySetUpIdleModeTextPCmd:
            {
            iNotifySetUpIdleModeText->Notify( aTsyReqHandle, aPackage );
            break;
            }
        case ESatNotifyLaunchBrowserPCmd:
            {
            iNotifyLaunchBrowser->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifyCallControlRequest:
            {
            iNotifyCallControlRequest->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifySendDtmfPCmd:
            {
            iNotifySendDtmf->Notify( aTsyReqHandle, aPackage );
            break;
            }
		case ESatNotifySetUpEventListPCmd:
            {
            iNotifySetUpEventList->Notify( aTsyReqHandle, aPackage );
            break;
            }
        case ESatNotifySetUpCallPCmd:
            {
            iNotifySetUpCall->Notify( aTsyReqHandle, aPackage );
            break;  
            }
        case ESatNotifyRefreshPCmd:
            {
            iNotifyRefresh->Notify( aTsyReqHandle, aPackage );
            break;                  
            }
        case ESatNotifyRefreshRequiredParam:
        	{
            iNotifyRefresh->NotifyRefreshRequired( aTsyReqHandle, aPackage );
            break;            
        	}
        case ESatRefreshAllowed:
        	{
            iSatTsy->ReqCompleted( aTsyReqHandle, 
                iNotifyRefresh->RefreshAllowedL( aPackage ) );
            break;
        	}          
        case ESatNotifyCbDownload:
            {
            NotifyCbDownload( aTsyReqHandle );
            break;
            }
        case ESatNotifySmsPpDownload:
            {
            NotifySmsPpDownload( aTsyReqHandle );
            break;
            }
        case ESatNotifyLanguageNotificationPCmd:
            {
            iNotifyLanguageNotification->Notify( aTsyReqHandle, aPackage );
            break;
            }
        case ESatNotifyLocalInfoPCmd:
            {
            iNotifyLocalInfo->Notify( aTsyReqHandle, aPackage );
            break;
            }
       case ESatNotifyOpenChannelPCmd:
            {
            iNotifyOpenChannel->Notify( aTsyReqHandle, aPackage );
            break;
            }
       	case ESatNotifyGetChannelStatusPCmd:
       		{
       		iNotifyGetChannelStatus->Notify( aTsyReqHandle, aPackage );
       		break;	
       		}
        case ESatNotifyMoSmControlRequest:
            {
            iNotifyMoSmControlRequest->Notify( aTsyReqHandle, aPackage );
            break;
            }
        case ESatNotifyCloseChannelPCmd:
        	{
       		iNotifyCloseChannel->Notify( aTsyReqHandle, aPackage );
        	break;
        	}
       	case ESatNotifySendDataPCmd:
       		{
       		iNotifySendData->Notify( aTsyReqHandle, aPackage );
       		break;	
       		}
        case ESatNotifyReceiveDataPCmd:
        	{
        	iNotifyReceiveData->Notify( aTsyReqHandle, aPackage );
        	break;	
        	} 
        default:
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_DOEXTFUNCL_2, "CSAT: CSatNotificationsTsy::DoExtFuncL, IPC not supported");
            ret = KErrNotSupported;
            break;
        	}
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::CancelService
// Cancels Notifications requests
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsTsy::CancelService
        (    
        const TInt aIpc,               
        const TTsyReqHandle aTsyReqHandle  
        )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_CANCELSERVICE_1, "CSAT: CSatNotificationsTsy::CancelService.\n\t\t\t Handle:%d\n\t\t\t", aTsyReqHandle );

    TInt ret ( KErrNotSupported );
    
    // When the clients close their sub-sessions (eg. by calling RLine::Close), 
    // they may not have cancelled all their outstanding asynchronous requests 
    // before closing.  It is up to the ETel server to clean up in this 
    // situation, so the server will find the list of outstanding requests 
    // related to that sub-session object and pass these outstanding IPC 
    // request numbers, one at a time, to the CancelService method in the TSY.

    switch ( aIpc )
        {
        case ESatNotifyCallControlRequest:
            {
            ret = iNotifyCallControlRequest->CancelNotification( 
                aTsyReqHandle );
            break;
            }
        case ESatNotifyDisplayTextPCmd:
            {
            ret = iNotifyDisplayText->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyGetInkeyPCmd:
            {
            ret = iNotifyGetInkey->CancelNotification( aTsyReqHandle );
            break;        
            }
        case ESatNotifyGetInputPCmd:
            {
            ret = iNotifyGetInput->CancelNotification( aTsyReqHandle );
            break;        
            }
        case ESatNotifyPlayTonePCmd:
            {
            ret = iNotifyPlayTone->CancelNotification( aTsyReqHandle );
            break;        
            }
        case ESatNotifySetUpMenuPCmd:
            {
            ret =  iNotifySetUpMenu->CancelNotification( aTsyReqHandle );
            break;        
            }
        case ESatNotifySelectItemPCmd:
            {
            ret = iNotifySelectItem->CancelNotification( aTsyReqHandle );
            break;        
            }
        case ESatNotifySendSmPCmd:
            {
            ret = iNotifySendSm->CancelNotification( aTsyReqHandle );
            break;        
            }
        case ESatNotifySendSsPCmd:
            {
            ret = iNotifySendSs->CancelNotification( aTsyReqHandle );
            break;        
            }
        case ESatNotifySetUpEventListPCmd:
            {
            ret = iNotifySetUpEventList->CancelNotification( aTsyReqHandle );
			break;
            }
		case ESatNotifySendUssdPCmd:
            {
            ret = iNotifySendUssd->CancelNotification( aTsyReqHandle );
            break;        
            }
        case ESatNotifySetUpIdleModeTextPCmd:
            {
            ret = iNotifySetUpIdleModeText->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifySetUpCallPCmd:
            {
            ret = iNotifySetUpCall->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyRefreshPCmd:
            {
            ret = iNotifyRefresh->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyRefreshRequiredParam:
        	{
            ret = iNotifyRefresh->CancelRefreshRequiredNotification( 
            	aTsyReqHandle );
            break;            
        	}
        case ESatNotifySendDtmfPCmd:
            {
            ret = iNotifySendDtmf->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyLanguageNotificationPCmd:
            {
            ret = iNotifyLanguageNotification->CancelNotification(
                  	aTsyReqHandle );
            break;
            }
        case ESatNotifyLaunchBrowserPCmd:
            {
            ret = iNotifyLaunchBrowser->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyLocalInfoPCmd:
            {
            ret = iNotifyLocalInfo->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyProactiveSimSessionEnd:
            {
            ret = iNotifySimSessionEnd->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyOpenChannelPCmd:
            {
            ret = iNotifyOpenChannel->CancelNotification( aTsyReqHandle );
            break;
            }
        case ESatNotifyGetChannelStatusPCmd:
       		{
       		ret = iNotifyGetChannelStatus->CancelNotification( aTsyReqHandle );
       		break;	
       		}
        case ESatNotifyCloseChannelPCmd:
        	{
       		ret = iNotifyCloseChannel->CancelNotification( aTsyReqHandle );
        	break;
        	}
       	case ESatNotifySendDataPCmd:
       		{
       		ret = iNotifySendData->CancelNotification( aTsyReqHandle );
       		break;	
       		}
       	case ESatNotifyReceiveDataPCmd:
        	{
        	ret = iNotifyReceiveData->CancelNotification( aTsyReqHandle );
        	break;	
        	} 	        			                     
        case ESatNotifyMoSmControlRequest:
            {
            ret = iNotifyMoSmControlRequest->CancelNotification( 
                	aTsyReqHandle );
            break;
            }
        default:
        	{        	
            ret = KErrNone; 
            break;
        	}
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::SetPollingResultL
// Sends terminal response according to the response coming from DOS.
// This method handles both PollingInterval and PollingOff proactive commands.
// -----------------------------------------------------------------------------
//
void CSatNotificationsTsy::SetPollingResultL
        ( 
        CSatDataPackage* aDataPackage,
        TInt aErrorCode
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_SETPOLLINGRESULTL_1, "CSAT: CSatNotificationsTsy::SetPollingResultL");
    TUint8 pCmdNumber;
    TUint8 interval;
    TBuf<RSat::KAdditionalInfoMaxSize> additionalInfo;
    aDataPackage->UnPackData( pCmdNumber, interval );
    
    // NAA status OK 
    if ( KErrNone == aErrorCode ) 
        {
        if ( iPollingOff )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_SETPOLLINGRESULTL_2, "CSAT: CSatNotificationsTsy::SetPollingResultL, Polling Off");
            additionalInfo.Zero();
            additionalInfo.Append( RSat::KNoAdditionalInfo );
            iNotifyPollingOff->CreateTerminalRespL( pCmdNumber,
                RSat::KSuccess, additionalInfo );
            }
        else
            {
            iNotifyPollInterval->CreateTerminalRespL( pCmdNumber, 
            	RSat::KSuccess, RSat::KNoAdditionalInfo, interval );
            }
        }    
    // NAA status not OK
	else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_SETPOLLINGRESULTL_3, "CSAT: CSatNotificationsTsy::SetPollingResultL, NAA Status Not OK, means that value in request is out of range and default value is used.");
        if ( iPollingOff )
            {
            iPollingOff = EFalse;
            additionalInfo.Zero();
            additionalInfo.Append( RSat::KNoSpecificMeProblem );
            iNotifyPollingOff->CreateTerminalRespL( pCmdNumber,
                    RSat::KMeUnableToProcessCmd, additionalInfo );
            }
        else
            {
            // If last request rejected (request was over 25 seconds),
            // renew the request with legacy maximum.. 
            if ( iLongPollIntervalReq )
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_SETPOLLINGRESULTL_4, "CSAT: CSatNotificationsTsy::SetPollingResultL, Request set to legacy maximum (25 seconds)");
                SetPollingIntervalL( KMaxLegacyPollInterval );
                iLongPollIntervalReq = EFalse;
                }
            // ..otherwise there was an actual error.
            else
                {
                iNotifyPollInterval->CreateTerminalRespL( pCmdNumber, 
                	RSat::KMeUnableToProcessCmd, RSat::KNoSpecificMeProblem, 
                	interval );
                }
            }
        }
            
    // Terminal Response is sent or the request was resent, reset flag
    iLongPollIntervalReq = EFalse;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::SetPollingIntervalL
// Set Polling Interval, sending a request to DOS
// -----------------------------------------------------------------------------
//
void CSatNotificationsTsy::SetPollingIntervalL
        (
        TUint8 aValue
        )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_SETPOLLINGINTERVALL_1, "CSAT: CSatNotificationsTsy::SetPollingIntervalL, Value: %d",aValue);
    
    if ( KMaxLegacyPollInterval < aValue )
        {
        iLongPollIntervalReq = ETrue;
        }

    TBuf8<1> data;
    data.Append( aValue );
    // Pack data
    CSatDataPackage dataPackage;
	dataPackage.PackData( &data );
    iSatTsy->MessageManager()->HandleRequestL( ESatTsySetPolling, 
    	&dataPackage );
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::TimerExpirationL
// Converts timerValue to semi-octets and forwards message to LicenceeSimAtkTsy
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsTsy::TimerExpirationL
        (
        TInt aTimerId,
        TUint32 aTimerValue 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_TIMEREXPIRATIONL_1, "CSAT: CSatNotificationsTsy::TimerExpirationL");
    TUint8 time[3] = {0, 0, 0}; 
    TInt num[3];
    num[0] = aTimerValue / 3600;         // Hours
    num[1] = ( aTimerValue % 3600 )/60;  // Minutes
    num[2] = ( aTimerValue % 3600 )%60;  // Seconds

    for ( TInt i = 0; i < 3; i++ )
        { 
        // Convert to semi-octet
        time[i] = TSatUtility::ConvertToSemiOctet( num[i] );
        }
        
	TTlv data;    
    data.Begin( KBerTlvTimerExpirationTag );  
    // Create Device Identity TLV
    // Device identity tag
    data.AddTag( KTlvDeviceIdentityTag ); 
    // Source device identity = ME
    data.AddByte( KMe ); 
    // Destination device identity = SIM
    data.AddByte( KSim );
    // Value KTlvTimerIdentifierTagCRBitOn ( 0xA4 ) is added,
    // because used KTlvTimerIdentifierTag ( 0x24 ) doesn't
    // include comprehension bit in tag the value and it is expected
    // to be present in indications and terminal responses.
    data.AddTag( KTlvTimerIdentifierTagCRBitOn );
    data.AddByte( static_cast< TUint8 >( aTimerId ) );
    // Timer value in semi-octets
    data.AddTag( KTlvTimerValueTag );
    data.AddByte( time[0] ); // Hours
    data.AddByte( time[1] ); // Minutes
    data.AddByte( time[2] ); // Seconds
	// Pack parameter for DOS call
    TPtrC8 berTlv = data.End();
	CSatDataPackage dataPackage;
	dataPackage.PackData( &berTlv );

	return iSatTsy->MessageManager()->HandleRequestL( 
        ESatTsyTimerExpiredIPC, &dataPackage );
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::GetSatTimer
// Returns iSatTimer pointer and control to calling object
// -----------------------------------------------------------------------------
//
CSatTimer* CSatNotificationsTsy::GetSatTimer
        (
        // None 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_GETSATTIMER_1, "CSAT: CSatNotificationsTsy::GetSatTimer");
    // Pointer to SAT timer
    return iSatTimer; 
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::SatNotifySetUpCall
// Return pointer to CSatNotifySetUpCall.
// -----------------------------------------------------------------------------
//
CSatNotifySetUpCall* CSatNotificationsTsy::SatNotifySetUpCall
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_SATNOTIFYSETUPCALL_1, "CSAT: CSatNotificationsTsy::SatNotifySetUpCall");
    // Pointer to NotifySetupCall
    return iNotifySetUpCall;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::SetProactiveCommandOnGoingStatus
// Sets proactive command on-going status 
// -----------------------------------------------------------------------------
//
void CSatNotificationsTsy::SetProactiveCommandOnGoingStatus
        (
        CSatDataPackage* aDataPackage,  
		TInt /*aResult*/             
        )
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_SETPROACTIVECOMMANDONGOINGSTATUS_1, "CSAT: CSatNotificationsTsy::SetProactiveCommandOnGoingStatus");
    TBool ProactiveCommandOngoingStatus;
    aDataPackage->UnPackData( ProactiveCommandOngoingStatus );
    iSatTimer->SetProactiveCommandOnGoingStatus( 
    	ProactiveCommandOngoingStatus ); 
    }
    
// -----------------------------------------------------------------------------
// CSatNotificationsTsy::CompletePCmdL
// Forwards the received Proactive command data to the correct Notify class
// based on the type of command.
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsTsy::CompletePCmdL
        (
        CSatDataPackage* aDataPackage,
        TInt aResult
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_COMPLETEPCMDL_1, "CSAT: CSatNotificationsTsy::CompletePCmdL");

    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    CBerTlv berTlv;
    berTlv.SetData( *data );
	CTlv commandDetails;
	berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    TInt ret( KErrNone );
    // Set flag on to indicate that proactive command is ongoing
    iSatTimer->SetProactiveCommandOnGoingStatus( ETrue );
    
    switch( commandDetails.GetShortInfo( ETLV_TypeOfCommand ) )
        {
        case KDisplayText:
            {
            ret = iNotifyDisplayText->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KGetInkey:
            {
            ret = iNotifyGetInkey->CompleteNotifyL( aDataPackage, aResult );            
            break;    
            }
        case KGetInput:         
            {
            ret = iNotifyGetInput->CompleteNotifyL( aDataPackage, aResult );
            break;    
            }
        case KPlayTone:         
            {
            ret = iNotifyPlayTone->CompleteNotifyL( aDataPackage, aResult );
            break;    
            }
        case KSetUpMenu:     
            {
            ret = iNotifySetUpMenu->CompleteNotifyL( aDataPackage, aResult );
            break;    
            }
        case KSelectItem:
            {
            ret = iNotifySelectItem->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KSendShortMessage:         
            {
            ret = iNotifySendSm->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KSendSs:
            {
            ret = iNotifySendSs->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KSendUssd:
            {
            ret = iNotifySendUssd->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KSetUpCall:
            {
            ret = iNotifySetUpCall->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KRefresh:
            {
            ret = iNotifyRefresh->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KSetUpIdleModeText:
            {
			ret = iNotifySetUpIdleModeText->CompleteNotifyL( aDataPackage, 
					aResult );
            break;
            }
        case KPollInterval:
            {
            ret = iNotifyPollInterval->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KSendDtmf:
            {
            ret = iNotifySendDtmf->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KSetUpEventList:
            {
            ret = iNotifySetUpEventList->CompleteNotifyL( aDataPackage, 
            		aResult );
            break;
            }
        case KPollingOff:
            {
            ret = iNotifyPollingOff->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KProvideLocalInformation:
            {
            ret = iNotifyLocalInfo->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KTimerManagement:
            {
            ret = iNotifyTimerMgmt->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KMoreTime:
            {
            ret = iNotifyMoreTime->CompleteNotifyL( aDataPackage, aResult );
            break;
            }
        case KLaunchBrowser: 
            {
            ret = iNotifyLaunchBrowser->CompleteNotifyL( aDataPackage, 
            		aResult );
			break;
            }
        case KLanguageNotification:
            {
            ret = iNotifyLanguageNotification->CompleteNotifyL( aDataPackage, 
            		aResult );
            break;
            }          
		case KOpenChannel:
			{
			ret = iNotifyOpenChannel->CompleteNotifyL( aDataPackage, aResult );
			break;
			}
		case KCloseChannel:
			{
			ret = iNotifyCloseChannel->CompleteNotifyL( aDataPackage, aResult );
			break;
			}		
		case KReceiveData:
			{
			ret = iNotifyReceiveData->CompleteNotifyL( aDataPackage, aResult );
			break;
			}		
		case KSendData:
			{
			ret = iNotifySendData->CompleteNotifyL( aDataPackage, aResult );
			break;
			}		
		case KGetChannelStatus:            
			{
			ret = iNotifyGetChannelStatus->CompleteNotifyL( aDataPackage, 
					aResult );
			break;
			}
        default:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_COMPLETEPCMDL_2, "CSAT: CSatNotificationsTsy::CompletePCmdL, Proactive command not supported" );
        	// If proactive command is not supported, terminal response
            // with "Command is beyond ME's capabilities" information 
            // will be send. 
            // Get command details tlv and set it to the struct
        	iTerminalRespData.iCommandDetails.Copy( commandDetails.Data() );
        	// Create and append response data
        	TTlv tlvSpecificData;
        	// Create General Result TLV here
        	tlvSpecificData.AddTag( KTlvResultTag );
        	tlvSpecificData.AddByte( KCommandBeyondMeCapabilities );
        	TPtrC8 data = tlvSpecificData.GetDataWithoutTopLevelTag();
        	CSatDataPackage dataPackage;
        	dataPackage.PackData( &iTerminalRespData, &data );
            // Forward request to the DOS
    		ret = iSatTsy->MessageManager()->HandleRequestL( 
					ESatTerminalRsp, &dataPackage );	 
            break;
            }

        } // End of switch (msg type)

    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatNotificationsTsy::IsMoSmControlActivated
// Returns ETrue if MO-SMS Control is activated
// -----------------------------------------------------------------------------
//
TBool CSatNotificationsTsy::IsMoSmControlActivated
		(
		void
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_ISMOSMCONTROLACTIVATED_1, "CSAT: CSatNotificationsTsy::IsMoSmControlActivated");
    return iNotifyMoSmControlRequest->ActivationStatus();
    }
     
// -----------------------------------------------------------------------------
// CSatNotificationsTsy::SetPollingOff
// Set PollingOff value
// -----------------------------------------------------------------------------
//
void CSatNotificationsTsy::SetPollingOffStatus
		( 
		TBool aPollingOff 
		)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_SETPOLLINGOFFSTATUS_1, "CSAT: CSatNotificationsTsy::SetPollingOffStatus status: %d", aPollingOff);
	iPollingOff = aPollingOff;		
	}

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::SetPollingOff
// Returns iPollingOff
// -----------------------------------------------------------------------------
//	
TBool CSatNotificationsTsy::PollingOffStatus
		( 
		// None
		)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_POLLINGOFFSTATUS_1, "CSAT: CSatNotificationsTsy::PollingOffStatus");
	return iPollingOff;		
	}

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::AddLocationInformationToTlv
// Adds location informating to given TTlv.
// -----------------------------------------------------------------------------
//
void CSatNotificationsTsy::AddLocationInformationToTlv
        (
        TTlv& aTlv 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_ADDLOCATIONINFORMATIONTOTLV_1, "CSAT: CSatNotificationsTsy::AddLocationInformationToTlv");
    
    // Get the current location data from CSatNotifyLocalInfo
    const TLocalInformation& locInfo = iNotifyLocalInfo->LocalInformation();
        
    // Add location information data, start with the tag
    aTlv.AddTag( KTlvLocationInformationTag );
    // Append Mobile Country  & Network Codes, 3 bytes
    aTlv.AddData( locInfo.iOperatorCode );
    // Append Location Area Code (LAC)
    aTlv.AddByte( static_cast<TUint8>( locInfo.iLocationAreaCode >> 8 ) );//msb
    aTlv.AddByte( static_cast<TUint8>( locInfo.iLocationAreaCode >> 0 ) );//lsb
    // Append Cell Id
    aTlv.AddByte( static_cast<TUint8>( locInfo.iCellId >> 8 ) ); // msb
    aTlv.AddByte( static_cast<TUint8>( locInfo.iCellId >> 0 ) ); // lsb
    }	
    
// -----------------------------------------------------------------------------
// CSatNotificationsTsy::NotifyClientForAlphaId
// Requests completion for the ESatNotifyCallControlRequestPCmdReqType
// -----------------------------------------------------------------------------
//
void CSatNotificationsTsy::NotifyClientForAlphaId
	    (
	    RSat::TAlphaIdBuf& aAlphaId,
	    RSat::TControlResult aResult 
	    )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_NOTIFYCLIENTFORALPHAID_1, "CSAT: CSatNotificationsTsy::NotifyClientForAlphaId");
    iNotifyCallControlRequest->CompleteAlphaId( aAlphaId, aResult, KErrNone );
    }
    
// -----------------------------------------------------------------------------
// CSatNotificationsTsy::RequestHandleStore
// Returns pointer to the request handle store
// -----------------------------------------------------------------------------
//
CSatTsyReqHandleStore* CSatNotificationsTsy::RequestHandleStore
	    (
	    void
	    )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_REQUESTHANDLESTORE_1, "CSAT: CSatNotificationsTsy::RequestHandleStore");
    return iSatReqHandleStore;
    }
    	       
// -----------------------------------------------------------------------------
// CSatNotificationsTsy::NotifySatReadyForNotification
// Passes a Sat ready indication to DOS and requests the possible pending
// Proactive command
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsTsy::NotifySatReadyForNotification
	    (
	    TUint8 aPCmdMsgType 
	    )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_NOTIFYSATREADYFORNOTIFICATION_1, "CSAT: CSatNotificationsTsy::NotifySatReadyForNotification");
    TInt ret( KErrNone );
    // Pack parameter for DOS call
	CSatDataPackage dataPackage;
	dataPackage.PackData( &aPCmdMsgType );
	TRAPD( trapError, ret = iSatTsy->MessageManager()->HandleRequestL( 
		ESatTsyGetIfPendingNotification, &dataPackage ) 
		);
		
	if ( trapError )
		{
		ret = trapError;
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_NOTIFYSATREADYFORNOTIFICATION_2, "CSAT: CSatNotificationsTsy::NotifySatReadyForNotification Trap error: %d", trapError);
		}
		
	return ret;
    }
                
// -----------------------------------------------------------------------------
// CSatNotificationsTsy::TerminalResponseL
// Called by ETel server, passes a response to SIM
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsTsy::TerminalResponseL
            (                    
            RSat::TPCmd*    aPCmd,         
            TDes8*          aRsp,           
            TTsyReqHandle   aTsyReqHandle   
            ) 
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_TERMINALRESPONSEL_1, "CSAT: CSatNotificationsTsy::TerminalResponseL.\n\t\t\t Handle:%d\n\t\t\t", aTsyReqHandle );

    TInt ret( KErrNone );

    switch ( *aPCmd )
        {
        case RSat::EDisplayText:
            {
            ret = iNotifyDisplayText->TerminalResponseL( aRsp );
            break;
            }
        case RSat::EGetInkey:
            {
            ret = iNotifyGetInkey->TerminalResponseL( aRsp );
            break;
            }
        case RSat::EGetInput:
            {
            ret = iNotifyGetInput->TerminalResponseL( aRsp );
            break;
            }
        case RSat::EPlayTone:
            {
            ret = iNotifyPlayTone->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ESetUpMenu:
            {
            ret = iNotifySetUpMenu->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ESelectItem:
            {
            ret = iNotifySelectItem->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ESendSm:
            {
            ret = iNotifySendSm->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ESendSs:
            {
            ret = iNotifySendSs->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ESetUpEventList:
            {
            ret = iNotifySetUpEventList->TerminalResponseL( aRsp );
            break;
            }
       	case RSat::ESendUssd:
            {
            ret = iNotifySendUssd->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ESetUpIdleModeText:
            {
            ret = iNotifySetUpIdleModeText->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ESetUpCall:
            {
            ret = iNotifySetUpCall->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ERefresh:
            {
            ret = iNotifyRefresh->TerminalResponseL( aRsp );
            break;
            } 
        case RSat::ELaunchBrowser:
            {
            ret = iNotifyLaunchBrowser->TerminalResponseL( aRsp );
            break;
            }
		case RSat::ESendDtmf:
            {
			ret = iNotifySendDtmf->TerminalResponseL( aRsp );
			break;
            }
        case RSat::ELocalInfo:
            {
            ret = iNotifyLocalInfo->TerminalResponseL( aRsp );
            break;
            }
        case RSat::ELanguageNotification:
            {
            ret = iNotifyLanguageNotification->TerminalResponseL( aRsp );
            break;
            }
        case RSat::EOpenChannelCs:
        case RSat::EOpenChannelGprs:
        case RSat::EOpenChannelLocal:
            {
            ret = iNotifyOpenChannel->TerminalResponseL( aRsp );
            break;
            }
       	case RSat::EGetChannelStatus:
       		{
       		ret = iNotifyGetChannelStatus->TerminalResponseL( aRsp );
       		break;	
       		}
		case RSat::ECloseChannel:
        	{
       		ret = iNotifyCloseChannel->TerminalResponseL( aRsp );
        	break;
        	}
       	case RSat::ESendData:
       		{
       		ret = iNotifySendData->TerminalResponseL( aRsp );
       		break;	
       		}
       	case RSat::EReceiveData:
        	{
        	ret = iNotifyReceiveData->TerminalResponseL( aRsp );
        	break;	
        	}	         			                            
        default:
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_TERMINALRESPONSEL_2, "CSAT: CSatNotificationsTsy::TerminalResponseL. Proactive command unknown" );
            ret =  KErrGeneral;
            break;
        	}
        }
	
    // We must complete the request to the client right away, as an
	// indication that the request has been handled
    iSatTsy->ReqCompleted( aTsyReqHandle, ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::CompleteTerminalResponse
// This method completes an outstanding asynchronous Terminal response request. 
// -----------------------------------------------------------------------------
//
void CSatNotificationsTsy::CompleteTerminalResponse
        (
        TInt aErrorCode     // Error code
        )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_COMPLETETERMINALRESPONSE_1, "CSAT: CSatNotificationsTsy::CompleteTerminalResponse - Error:%d", aErrorCode);

    TTsyReqHandle reqHandle = iSatTsy->GetSatReqHandleStore()->
        ResetTsyReqHandle( CSatTsy::ESatTerminalRspReqType );

    // Check if handle for terminal response exists
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
        // Complete the request
        iSatTsy->ReqCompleted( reqHandle, aErrorCode );
        }
   
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::CommandPerformedSuccessfully
// Test if a proactive command was performed successfully
// -----------------------------------------------------------------------------
//
TBool CSatNotificationsTsy::CommandPerformedSuccessfully
        ( 
        TUint8 aGeneralResult 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_COMMANDPERFORMEDSUCCESSFULLY_1, "CSAT: CSatNotificationsTsy::CommandPerformedSuccessfully");
    TBool ret( EFalse );
	switch ( aGeneralResult )
	    { 
        case RSat::KSuccess:
        case RSat::KPartialComprehension:
        case RSat::KMissingInformation:
        case RSat::KRefreshAdditionEFRead:
        case RSat::KSuccessRequestedIconNotDisplayed:
        case RSat::KModifiedByCallControl:
        case RSat::KSuccessLimitedService:
        case RSat::KPerformedWithModifications:
        case RSat::KRefreshUSIMNotActive:
        	{
    		ret = ETrue;
            break;
        	}
	    default:
	    	{	
	    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_COMMANDPERFORMEDSUCCESSFULLY_2, "CSAT: CSatNotificationsTsy::CommandPerformedSuccessfully, EFalse");
		    ret = EFalse;
		    break;
	    	}
	    }
    return ret;
   }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::NotifyCbDownload
// Called by ETel server, passes a cell broadcast download response to NAA
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsTsy::NotifyCbDownload
        (
        TTsyReqHandle   aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_NOTIFYCBDOWNLOAD_1, "CSAT: CSatNotificationsTsy::NotifyCbDownload");
    TInt ret( KErrNone );
    iSatTsy->ReqCompleted( aTsyReqHandle, ret );   
    return ret; 
    }

// -----------------------------------------------------------------------------
// CSatNotificationsTsy::NotifySmsPpDownload
// Called by ETel server, passes a Sms Pp download response to NAA
// -----------------------------------------------------------------------------
//
TInt CSatNotificationsTsy::NotifySmsPpDownload
        (
        TTsyReqHandle   aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_NOTIFYSMSPPDOWNLOAD_1, "CSAT: CSatNotificationsTsy::NotifySmsPpDownload");
    TInt ret( KErrNone );
    iSatTsy->ReqCompleted( aTsyReqHandle, ret );   
    return ret; 
    }

//  End of File 
