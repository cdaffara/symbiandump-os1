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
// Name        : CSatTsy.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Main entry point for Etel requests
// Version     : 1.0  
//



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csattsyTraces.h"
#endif

#include <etelsat.h>                // Etel SAT API
#include <satcs.h>                  // Etel SAT API
//#include "CMmPhoneTsy.h"			// Phone Tsy class
#include "CSatDataPackage.h"		// For data packages
//#include "MmTsy_numberOfSlots.h"    // Number of Slots
#include "cmmmessagemanagerbase.h"	// Message manager base
#include <ctsy/pluginapi/mmessagerouter.h>
#include <ctsy/serviceapi/mctsysatservice.h>
//#include "CMmMessageRouterBase.h"	// Message router

//#include "CMmSmsTsy.h"              // MmSmsTsy
//#include "CMmMessageManager.h"      // Message manager

#include "CSatTsy.h"				// Messaging class
#include "CSatCCTsy.h"              // SatCallControl class
#include "CSatIconTsy.h"            // SatIcon class
#include "CSatNotificationsTsy.h"   // SatNotifications class
#include "CSatNotifySetUpCall.h"    // CSatNotifySetUpCall    
#include "CSatEventDownloadTsy.h"   // SatEventDownload class
#include "CSatDataDownloadTsy.h"    // SatDataDownload class

#include "msattsy_ipcdefs.h"		// Sat Tsy specific request types
#include "TSatUtility.h"		    // Sat Tsy Utility class
#include "TTlv.h"					// TTlv class
#include "CSatTsyReqHandleStore.h"  // Request handle storage

#include "stktsy_number_of_slots.h"

// -----------------------------------------------------------------------------
// CSatTsy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatTsy* CSatTsy::NewL
		(
		MMessageRouter& aMmMessageRouter,
		MCtsySatService& aSatService
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_NEWL_1, "CSAT: CSatTsy::NewL");

	// Create subsession
    CSatTsy* subsession = new ( ELeave ) CSatTsy( aMmMessageRouter, aSatService );

    CleanupStack::PushL( subsession );
    subsession->ConstructL();
    CleanupStack::Pop();

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_NEWL_2, "CSAT: CSatTsy::NewL, end of method");
    return subsession;
    }

// -----------------------------------------------------------------------------
// CSatTsy::~CSatTsy
// Destructor
// -----------------------------------------------------------------------------
// 
CSatTsy::~CSatTsy
		(
		// None
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_DTOR_1, "CSAT: CSatTsy::~CSatTsy");

    // Unregister.
    iMessageManager->RegisterTsyObject(
		CMmMessageManagerBase::ESatTsyObjType, NULL );

    // Delete also specific TSY objects
    delete iSatCCTsy;
    delete iSatIconTsy;
    delete iSatNotificationsTsy;
    delete iSatDataDownloadTsy;
    delete iSatEventDownloadTsy;
	// Message router and message manager are deleted here
    delete iMessageManager;
    // Request handle store
    delete iSatReqHandleStore;
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_DTOR_2, "CSAT: CSatTsy::~CSatTsy, end of method");
    }

// -----------------------------------------------------------------------------
// CSatTsy::CSatTsy
// C++ constructor
// -----------------------------------------------------------------------------
//
CSatTsy::CSatTsy
		( 
        MMessageRouter& aMmMessageRouter,
        MCtsySatService& aSatService
		): 
		iMessageRouter ( aMmMessageRouter ),
		iSatService(aSatService)
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatTsy::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CSatTsy::ConstructL
		( 
        )
    { 
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_CONSTRUCTL_1, "CSAT: CSatTsy::ConstructL");
    
    // Set pointer to the message router 
    iMessageManager = CMmMessageManagerBase::NewL(iMessageRouter);
   
    // Register
    iMessageManager->RegisterTsyObject(
		CMmMessageManagerBase::ESatTsyObjType, this );

    // Create request handle store
    iSatReqHandleStore = CSatTsyReqHandleStore::NewL( 
        ESatMaxNumOfRequests, iSatReqHandles );

	// Create IconTsy instance
	iSatIconTsy = CSatIconTsy::NewL( this );
	// Create NotificationsTsy instance
	iSatNotificationsTsy = CSatNotificationsTsy::NewL( 
        this, iSatReqHandleStore );              
    // Create CCTsy instance
    iSatCCTsy = CSatCCTsy::NewL( this, iSatNotificationsTsy );
    // Create DataDownloadTsy instance
    iSatDataDownloadTsy = CSatDataDownloadTsy::NewL( this );
    // Create EventDownloadTsy instance
	iSatEventDownloadTsy = CSatEventDownloadTsy::NewL( this );
	
    // Request to read Cbmids
#ifdef USING_CTSY_DISPATCHER
	//Commented out due to not seemingly doing anything as the STK TSY does not hold EF_cbmid cache
    //iSatDataDownloadTsy->CompleteReadCbmidsL( );
    // Request Network status (removed as no known completion)   
    //iMessageManager->HandleRequestL( ESatTsyNetStatus );
    // Check if guardians should be activated (removed as no known completion)
	//iMessageManager->HandleRequestL( ESatTsyCallControlActivation );
	//Check if the SIM card suppords USSD tlv's in the call control envelopes (removed as no known completion)
	//iMessageManager->HandleRequestL( ESatTsyUssdControlActivation );
	// Get the default bearer data
	iMessageManager->HandleRequestL( ESatTsyGetBearerCapability );
	//prime the cached version of this setting
	iMessageManager->HandleRequestL( ESatTsyUssdStatus);
#else
	iSatDataDownloadTsy->CompleteReadCbmidsL( );
	// Request Network status    
	iMessageManager->HandleRequestL( ESatTsyNetStatus );
	// Check if guardians should be activated
	iMessageManager->HandleRequestL( ESatTsyCallControlActivation );
	//Check if the SIM card suppords USSD tlv's in the call control envelopes
	iMessageManager->HandleRequestL( ESatTsyUssdControlActivation );
	// Get the default bearer data
	iMessageManager->HandleRequestL( ESatTsyGetBearerCapability );
#endif //USING_CTSY_DISPATCHER
	// Notify Licencee SIM ATK TSY that we are ready to receive messages
	iMessageManager->HandleRequestL( ESatTsyReady );
	// Get SMS Point to Point Data Download support status
    iMessageManager->HandleRequestL( ESatTsySmsPpDdlStatus );
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_CONSTRUCTL_2, "CSAT: CSatTsy::ConstructL, end of method");
    }
    
// -----------------------------------------------------------------------------
// CSatTsy::ExtFunc
// TRAP's all CSatTsy related Etel API requests in cases that they fail.
// -----------------------------------------------------------------------------
//
TInt CSatTsy::ExtFunc
		( 
		const TTsyReqHandle aTsyReqHandle,  
		const TInt aIpc,           
		const TDataPackage& aPackage        
		)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_EXTFUNC_1, "CSAT: CSatTsy::ExtFunc ReqHandle=0x%08x IPC=%d", (TUint)aTsyReqHandle, aIpc);
	TInt ret( KErrNone );

    // Some SAT functions need trapping so we use two level function where 
    // first one is just a trap harness
    
    // It is responsibility of IPC handling functions to always handle 
    // calling ReqComplete, ( it must either call it, or arrange it to 
    // be called in some way ), except in the case of Leave. It is also 
    // assumed that if after IPC handling function has called ReqComplete, 
    // then it doesn't Leave.
    TInt trapError( KErrNone );

    TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ) );

    if ( KErrNone != trapError )
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_EXTFUNC_2, "CSAT: CSatTsy::ExtFunc, trapError: %d", trapError );
        ReqCompleted( aTsyReqHandle, trapError );
        }
	else if ( KErrNone != ret )
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_EXTFUNC_3, "CSAT: CSatTsy::ExtFunc, ret: %d", ret);
        ReqCompleted( aTsyReqHandle, ret );
        }
    else										
	    {
        // Reset last Tsy request type
        iReqHandleType = ESatReqHandleUnknown;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatTsy::SaveReqHandle
// Saves the request handle
// -----------------------------------------------------------------------------
//
void CSatTsy::SaveReqHandle
		( 
		const TTsyReqHandle aTsyReqHandle,  
        const TSatRequestTypes aSatRequestType
		)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SAVEREQHANDLE_1, "CSAT: CSatTsy::SaveReqHandle ReqHandle=0x%08x ReqHandleType=%d", (TUint)aTsyReqHandle, aSatRequestType);
    
    // Don't save the request handle if the type is unknown.
    if ( ESatReqHandleUnknown != aSatRequestType )
        {
        iSatReqHandleStore->SetTsyReqHandle( aSatRequestType, aTsyReqHandle );
        }
    }

// -----------------------------------------------------------------------------
// CSatTsy::ReqModeL
// Mode service function used by ETel, request mode. ReqModeL is called from 
// the server's CTelObject::ReqAnalyserL in order to check the type of request
// it has.
// -----------------------------------------------------------------------------
//
CTelObject::TReqMode CSatTsy::ReqModeL
		( 
		const TInt aIpc    
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_REQMODEL_1, "CSAT: CSatTsy::ReqModeL.");
    CTelObject::TReqMode ret = 0;

    switch ( aIpc )
        {
        // Multiple Completion Services
        case ESatNotifyDisplayTextPCmd:
        case ESatNotifyGetInkeyPCmd:
        case ESatNotifyGetInputPCmd:
        case ESatNotifyPlayTonePCmd:
        case ESatNotifySetUpMenuPCmd:
        case ESatNotifySelectItemPCmd:
        case ESatNotifySendSmPCmd:
        case ESatNotifySendSsPCmd:
        case ESatNotifySendUssdPCmd:
        case ESatNotifySetUpCallPCmd:
        case ESatNotifyRefreshPCmd:
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:        
        case ESatNotifyCallControlRequest:
        case ESatNotifySendDtmfPCmd:
        case ESatNotifyMoSmControlRequest:
        case ESatNotifySetUpEventListPCmd:
        case ESatNotifySetUpIdleModeTextPCmd:
        case ESatNotifyProactiveSimSessionEnd:
        case ESatNotifyLaunchBrowserPCmd:
        case ESatNotifyPollingIntervalPCmd:
        case ESatNotifyPollingOffPCmd:
        case ESatNotifyLanguageNotificationPCmd:
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyReceiveDataPCmd:               
            {
            ret = KReqModeMultipleCompletionEnabled;
            break;
            }
        // Non-flow Controlled Services
        case ESatTerminalRsp:
        case ESatMenuSelection:
        case ESatEventDownload:
        case ESatGetIcon:
        case ESatGetImageInstance:
        case ESatGetClut:
        case ESatGetMeSideSatProfile:
        case ESatClientSatProfileIndication:
        case ESatSendMessageNoLogging:
            {
            break;
            }
        default:
            {
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_REQMODEL_2, "CSAT: CSatTsy::ReqModeL unsupported IPC %d", aIpc);
            User::Leave( KErrNotSupported );
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTsy::CancelService
// Cancels Sat requests
// -----------------------------------------------------------------------------
//
TInt CSatTsy::CancelService
		( 
		const TInt aIpc,           
		const TTsyReqHandle aTsyReqHandle   
		)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_CANCELSERVICE_1, "CSAT: CSatTsy::CancelService ReqHandle=0x%08x IPC=%d", (TUint)aTsyReqHandle, aIpc);
    TInt ret( KErrNotSupported );

	// When the clients close their sub-sessions (eg. by calling RLine::Close), 
    // they may not have cancelled all their outstanding asynchronous requests 
    // before closing.  It is up to the ETel server to clean up in this 
    // situation, so the server will find the list of outstanding requests 
    // related to that sub-session object and pass these outstanding IPC 
    // request numbers, one at a time, to the CancelService method in the TSY.

    switch ( aIpc )
        {
        case ESatTerminalRsp:
        case ESatMenuSelection:
        //case ESatSendMessageNoLogging:
            {
            ret = KErrNone;
            break;
            }
        case ESatNotifyCallControlRequest:
		case ESatNotifyMoSmControlRequest:        
        case ESatNotifyDisplayTextPCmd:
        case ESatNotifyGetInkeyPCmd:
        case ESatNotifyGetInputPCmd:
        case ESatNotifyPlayTonePCmd:
        case ESatNotifySetUpMenuPCmd:
        case ESatNotifySelectItemPCmd:
        case ESatNotifySetUpCallPCmd:
        case ESatNotifySendSmPCmd:
        case ESatNotifySendSsPCmd:
        case ESatNotifySendUssdPCmd:   
        case ESatNotifyRefreshPCmd:
        case ESatNotifyRefreshRequiredParam:
        case ESatNotifySetUpIdleModeTextPCmd:
        case ESatNotifySendDtmfPCmd:
        case ESatNotifyLaunchBrowserPCmd:
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyLanguageNotificationPCmd:
        case ESatNotifySetUpEventListPCmd:
        case ESatNotifyProactiveSimSessionEnd:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyReceiveDataPCmd:       
            {
            ret = iSatNotificationsTsy->CancelService ( aIpc, aTsyReqHandle );
            break;
            }
        
        case ESatEventDownload:
            {
            ret = iSatEventDownloadTsy->CancelService ( aIpc, aTsyReqHandle );
            break;
            }
        
        // Following functions are not supported, hence they are completed
        // by DoExtFuncL function at once.
        // ETel server will not pass cancel function to SAT, because the 
        // request is not 'ON' in ETel.
        // They are listed here for future reference.
        //

        // ESatNotifyMoSmControlRequest
        // ESatEventDownload
        // ESatGetMeSideSatProfile

        default:
            {
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_CANCELSERVICE_2, "CSAT: CSatTsy::CancelService invalid IPC %d", aIpc);
            ret = KErrGeneral; 
            break;           	
            }
        }
    return ret;

    }
        
// -----------------------------------------------------------------------------
// CSatTsy::RegisterNotification
// ETel Server subscribe notifications from NOS. RegisterNotification is called
// when the server recognises that this notification is being posted for the 
// first time on this sub-session object. It enables the TSY to "turn on" any 
// regular notification messages that it may receive from the phone.
// -----------------------------------------------------------------------------
//
TInt CSatTsy::RegisterNotification
		( 
		const TInt aIpc    // IPC number of request
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_REGISTERNOTIFICATION_1, "CSAT: CSatTsy::RegisterNotification.");
    // Initialize return value
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case ESatNotifyDisplayTextPCmd:
        case ESatNotifyGetInkeyPCmd:
        case ESatNotifyGetInputPCmd:
        case ESatNotifyPlayTonePCmd:
        case ESatNotifySetUpMenuPCmd:
        case ESatNotifySelectItemPCmd:
        case ESatNotifySendSmPCmd:
        case ESatNotifySendSsPCmd:
        case ESatNotifySendUssdPCmd:
        case ESatNotifySetUpCallPCmd:
        case ESatNotifyRefreshPCmd:
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:        
        case ESatNotifyProactiveSimSessionEnd:
        case ESatNotifySetUpIdleModeTextPCmd:
        case ESatNotifyLaunchBrowserPCmd:
        case ESatNotifyCallControlRequest:
		case ESatNotifySendDtmfPCmd:
        case ESatNotifySetUpEventListPCmd:
        case ESatNotifyLanguageNotificationPCmd:
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyMoSmControlRequest:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyReceiveDataPCmd:      
            {
            // Ret already set to KErrNone
            break;
            }
        default:
            {
            // Unknown or invalid IPC
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_REGISTERNOTIFICATION_2, "CSAT: CSatTsy::RegisterNotification, Unsupported IPC %d", aIpc);
            ret = KErrNotSupported;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTsy::DeregisterNotification
// ETel server unsubscribe notifications from NOS. DeregisterNotification is 
// called when the server recognizes that this notification will not be posted 
// again because the last client to have a handle on this sub-session object  
// has just closed the handle. It enables the TSY to "turn off" any regular 
// notification messages that it may receive from the phone.
// -----------------------------------------------------------------------------
//
TInt CSatTsy::DeregisterNotification
		( 
		const TInt aIpc    // IPC number of request
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_DEREGISTERNOTIFICATION_1, "CSAT: CSatTsy::DeregisterNotification.");
    // Initialize return value
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case ESatNotifyDisplayTextPCmd:
        case ESatNotifyGetInkeyPCmd:
        case ESatNotifyGetInputPCmd:
        case ESatNotifyPlayTonePCmd:
        case ESatNotifySetUpMenuPCmd:
        case ESatNotifySelectItemPCmd:
        case ESatNotifySendSmPCmd:
        case ESatNotifySendSsPCmd:
        case ESatNotifySendUssdPCmd:
        case ESatNotifySetUpCallPCmd:
        case ESatNotifyRefreshPCmd:
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:        
        case ESatNotifyProactiveSimSessionEnd:
        case ESatNotifySetUpIdleModeTextPCmd:
        case ESatNotifyLaunchBrowserPCmd:
        case ESatNotifyCallControlRequest:
		case ESatNotifySendDtmfPCmd:		
        case ESatNotifySetUpEventListPCmd:
        case ESatNotifyLanguageNotificationPCmd:
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyMoSmControlRequest:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyReceiveDataPCmd:       
            {
            // Ret already set to KErrNone
            break;
            }
        default:
            {
            // Unknown or invalid IPC
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_DEREGISTERNOTIFICATION_2, "CSAT: CSatTsy::DeregisterNotification, Unsupported IPC %d", aIpc);
            ret = KErrNotSupported;
            break;            	
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC. NumberOfSlotsL is called 
// by the server when it is registering a new notification. It enables the TSY 
// to tell the server how many buffer slots to allocate for "repost 
// immediately" notifications that may trigger before clients collect them.
// -----------------------------------------------------------------------------
//
TInt CSatTsy::NumberOfSlotsL
		( 
		const TInt aIpc    // IPC number of request
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_NUMBEROFSLOTSL_1, "CSAT: CSatTsy::NumberOfSlotsL.");
    TInt numberOfSlots( 1 );

    switch ( aIpc )
        {
        case ESatNotifyDisplayTextPCmd:
            {
            numberOfSlots = KSatNotifyDisplayTextPCmdSlots;
            break;
            }
        case ESatNotifyGetInkeyPCmd:
            {
            numberOfSlots = KSatNotifyGetInkeyPCmdSlots;
            break;
            }
        case ESatNotifyGetInputPCmd:
            {
            numberOfSlots = KSatNotifyGetInputPCmdSlots;
            break;
            }
        case ESatNotifyPlayTonePCmd:
            {
            numberOfSlots = KSatNotifyPlayTonePCmdSlots;
            break;
            }
        case ESatNotifySetUpMenuPCmd:
            {
            numberOfSlots = KSatNotifySetUpMenuPCmdSlots;
            break;
            }
        case ESatNotifySelectItemPCmd:
            {
            numberOfSlots = KSatNotifySelectItemPCmdSlots;
            break;
            }
        case ESatNotifySendSmPCmd:
            {
            numberOfSlots = KSatNotifySendSmPCmdSlots;
            break;
            }
        case ESatNotifySendSsPCmd:
            {
            numberOfSlots = KSatNotifySendSsPCmdSlots;
            break;
            }
        case ESatNotifySendUssdPCmd:
            {
            numberOfSlots = KSatNotifySendUssdPCmdSlots;
            break;
            }
        case ESatNotifySetUpCallPCmd:
            {
            numberOfSlots = KSatNotifySetUpCallPCmdSlots;
            break;
            }
        case ESatNotifyRefreshPCmd:
            {
            numberOfSlots = KSatNotifyRefreshPCmdSlots;
            break;
            }
        case ESatNotifyProactiveSimSessionEnd:
            {
            numberOfSlots = KSatNotifyProactiveSimSessionEndSlots;
            break;
            }
        case ESatNotifySetUpIdleModeTextPCmd:
            {
            numberOfSlots = KSatNotifySetUpIdleModeTextPCmdSlots;
            break;
            }
        case ESatNotifyLaunchBrowserPCmd:
            {
            numberOfSlots = KSatNotifyLaunchBrowserSlots;
            break;
            }
        case ESatNotifyCallControlRequest:
            {
            numberOfSlots = KSatNotifyCallControlRequestSlots;
            break;
            }
        case ESatNotifySendDtmfPCmd:
            {
            numberOfSlots = KSatNotifySendDtmfSlots;
            break;
            }
        case ESatNotifySetUpEventListPCmd:
            {
            numberOfSlots = KSatNotifySetUpEventListSlots;
            break;
            }
        case ESatNotifyLanguageNotificationPCmd:
        	{
            numberOfSlots = KSatNotifySetUpEventListSlots;
            break;
        	}
        case ESatNotifyLocalInfoPCmd:
        case ESatNotifyOpenChannelPCmd:							
	    case ESatNotifyCloseChannelPCmd:							
	    case ESatNotifyReceiveDataPCmd:						
	    case ESatNotifySendDataPCmd:								
	    case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyMoSmControlRequest:
        	{
            numberOfSlots = KSatNotifySetUpEventListSlots;
            break;
        	}
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:
        	{
        	// Default is set to one
        	break;        	
        	}
        default:
            {
            // Unknown or invalid IPC
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_NUMBEROFSLOTSL_2, "CSAT: CSatTsy::NumberOfSlotsL, Unsupported IPC %d", aIpc);
            User::Leave( KErrNotSupported );
            break;
            }
        }  
    return numberOfSlots;
    }

// -----------------------------------------------------------------------------
// CSatTsy::ReqCompleted
// Completes requests to ETel
// -----------------------------------------------------------------------------
//
void CSatTsy::ReqCompleted
		( 
		TTsyReqHandle aReqHandle,
        TInt aRet 
        )
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_REQCOMPLETED_1, "CSAT: CSatTsy::ReqCompleted, ReqHandle=%d ", aReqHandle);
#ifdef USING_CTSY_DISPATCHER
	CTelObject::ReqCompleted( aReqHandle, aRet );
#else
	ReqCompleted( aReqHandle, aRet );
#endif //USING_CTSY_DISPATCHER
	}
	
// -----------------------------------------------------------------------------
// CSatTsy::StoreCallConnectedEvent
// Cache call connected envelope to CSatNotifySetUpCall class
// -----------------------------------------------------------------------------
//
void CSatTsy::StoreCallConnectedEvent
        (
        const TDesC8& aEnvelope
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_STORECALLCONNECTEDEVENT_1, "CSAT: CSatTsy::StoreCallConnectedEvent");
    iSatNotificationsTsy->SatNotifySetUpCall()->StoreCallConnectedEvent( 
        aEnvelope );
    }

// -----------------------------------------------------------------------------
// CSatTsy::SetSetUpCallStatus
// Set Status of SetUpCall command to the CSatEventDownloadTsy class
// -----------------------------------------------------------------------------
//
void CSatTsy::SetSetUpCallStatus
        (
        const TBool aStatus
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SETSETUPCALLSTATUS_1, "CSAT: CSatTsy::SetSetUpCallStatus");
    iSatEventDownloadTsy->SetSetUpCallStatus( aStatus );
    }

// -----------------------------------------------------------------------------
// CSatTsy::SetUpEventList
// Sets events that are reported to NAA
// -----------------------------------------------------------------------------
//
void CSatTsy::SetUpEventList
        (
        TUint32 aEvents     // events to be monitored
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SETUPEVENTLIST_1, "CSAT: CSatTsy::SetUpEventList");
    iSatEventDownloadTsy->SetUpEventList( aEvents );
    }

// -----------------------------------------------------------------------------
// CSatTsy::MessageManager
// This method returns a pointer to iMessageManager object.
// -----------------------------------------------------------------------------
//
CMmMessageManagerBase* CSatTsy::MessageManager
        (
        // none
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_MESSAGEMANAGER_1, "CSAT: CSatTsy::MessageManager");
    return iMessageManager;
    }

// -----------------------------------------------------------------------------
// CSatTsy::SatCCTsy
// This method returns a pointer to Call Control TSY object.
// -----------------------------------------------------------------------------
//
CSatCCTsy* CSatTsy::SatCCTsy
        (
        // none
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SATCCTSY_1, "CSAT: CSatTsy::SatCCTsy");
    return iSatCCTsy;
    }  

// -----------------------------------------------------------------------------
// CSatTsy::GetSatReqHandleStore
// Returns iSatReqHandleStore pointer and control to calling object
// -----------------------------------------------------------------------------
//
CSatTsyReqHandleStore* CSatTsy::GetSatReqHandleStore
	    (
	    //none
	    )
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_GETSATREQHANDLESTORE_1, "CSAT: CSatTsy::GetSatReqHandleStore");
	return iSatReqHandleStore;
	}

// -----------------------------------------------------------------------------
// CSatTsy::GetReqHandleType
// Returns pointer to request handle type
// -----------------------------------------------------------------------------
//
CSatTsy::TSatRequestTypes* CSatTsy::GetReqHandleType
	    (
	    //none
	    )
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_GETREQHANDLETYPE_1, "CSAT: CSatTsy::GetReqHandleType");
	return &iReqHandleType;
	}

// -----------------------------------------------------------------------------
// CSatTsy::CompleteSendSmsMessage
// Message from MMSMS side that message sending has been completed
// -----------------------------------------------------------------------------
//
TInt CSatTsy::CompleteSendSmsMessage
        (
        TInt aStatus 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_COMPLETESENDSMSMESSAGE_1, "CSAT: CSatTsy::CompleteSendSmsMessage");
    
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iSatNotificationsTsy->RequestHandleStore()->
        ResetTsyReqHandle( CSatTsy::ESatSendMessageNoLoggingReqType );
		
	// In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
    	ReqCompleted( reqHandle, aStatus );
        }
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CSatTsy::IsMoSmControlBySimActivated
// This method is required by CommonTSY, which needs to know whether the 
// MoSmControlBySim is activated or not.
// -----------------------------------------------------------------------------
//
TBool CSatTsy::IsMoSmControlBySimActivated
		(
		void
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_ISMOSMCONTROLBYSIMACTIVATED_1, "CSAT: CSatTsy::IsMoSmControlBySimActivated");
    return iSatNotificationsTsy->IsMoSmControlActivated();
    }

#ifdef USING_CTSY_DISPATCHER
    /**
     * From MStkTsyCallOrigin 
     * Dummy implementation for IsSimOriginatedCall function
     *          
     * @param aAddress The number whose origin is required.
     * @return ETrue if call originates from a Proactive Command
     */ 
TBool CSatTsy::IsSimOriginatedCall
			(
			const TDesC8& aAddress
			)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_ISSIMORIGINATEDCALL_1, "CSAT: CSatTsy::IsSimOriginatedCall");
	TBool ret = EFalse;
	if (iSatCCTsy)
		{
		ret = iSatCCTsy->HasProactiveOrigin(aAddress);
		}
	return ret;
	}
#endif //USING_CTSY_DISPATCHER

// -----------------------------------------------------------------------------
// CSatTsy::StoreProactiveAddress
// Stores latest address of Send SS/Setup call proactive command to CSatCC
// (other items were commented in a header). Method is also called when 
// proactive session is over with parameter NULL.
// -----------------------------------------------------------------------------
//    
void CSatTsy::StoreProactiveAddress
        ( 
        const TDesC8* aAddress 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_STOREPROACTIVEADDRESS_1, "CSAT: CSatTsy::StoreProactiveAddress");
    iSatCCTsy->StoreAddressForCC( *aAddress );
    }  
    
// -----------------------------------------------------------------------------
// CSatTsy::SetTonNpi
// Stores Ton Npi to CSatCC
// -----------------------------------------------------------------------------
//    
void CSatTsy::SetTonNpi
        ( 
        const TUint8 aTonNpi 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SETTONNPI_1, "CSAT: CSatTsy::SetTonNpi");
    iSatCCTsy->SetTonNpiForSS( aTonNpi );
    }  
    
// -----------------------------------------------------------------------------
// CSatTsy::DoExtFuncL
// Dispatch to correct SAT Tsy/function according to the IPC
// -----------------------------------------------------------------------------
//
TInt CSatTsy::DoExtFuncL
		(
		const TTsyReqHandle aTsyReqHandle,  
		const TInt aIpc,           
		const TDataPackage& aPackage	
		)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_DOEXTFUNCL_1, "CSAT: CSatTsy::DoExtFuncL\t IPC:%d,\t Handle:%d",aIpc, aTsyReqHandle);
	TInt ret( KErrNone );

	// Add here ALL supported Sat request types, and call either the correct 
	//	Sat Tsy to handle the request, or a request directly here in CSatTsy.
    switch ( aIpc )
        {
        // General
        case ESatMenuSelection:
            {
            ret = MenuSelectionL( aTsyReqHandle, 
				reinterpret_cast<RSat::TMenuSelectionV1Pckg*>( 
				aPackage.Des1n() ) );
            break;
            }
        case ESatSendMessageNoLogging:
            {
            SendMessageNoLogging( aPackage.Des1n(), aTsyReqHandle );
            break;
            }
        // Icon
        case ESatGetIcon:
		case ESatGetImageInstance:
		case ESatGetClut:
            {
            ret = iSatIconTsy->DoExtFuncL ( aTsyReqHandle, aIpc, aPackage );
            break;
            }
        // Notifications
        case ESatTerminalRsp:
        case ESatNotifyCallControlRequest:
        case ESatNotifyDisplayTextPCmd:
        case ESatNotifyGetInkeyPCmd:
        case ESatNotifyGetInputPCmd:
        case ESatNotifyPlayTonePCmd:
        case ESatNotifySetUpMenuPCmd:
        case ESatNotifySelectItemPCmd:
        case ESatNotifySendSmPCmd:
        case ESatNotifySendSsPCmd:
        case ESatNotifySendUssdPCmd:
        case ESatNotifySetUpCallPCmd:
		case ESatNotifyRefreshPCmd:
        case ESatNotifyRefreshRequiredParam:
        case ESatRefreshAllowed:
        case ESatNotifySetUpEventListPCmd:
        case ESatNotifySetUpIdleModeTextPCmd:
        case ESatNotifySendDtmfPCmd:
        case ESatNotifyLanguageNotificationPCmd:
        case ESatNotifyLaunchBrowserPCmd:
        case ESatNotifyLocalInfoPCmd:   
        case ESatNotifyProactiveSimSessionEnd:
        case ESatNotifyOpenChannelPCmd:
        case ESatNotifyGetChannelStatusPCmd:
        case ESatNotifyCloseChannelPCmd:
        case ESatNotifySendDataPCmd:
        case ESatNotifyReceiveDataPCmd:
        case ESatNotifySmsPpDownload:
        case ESatNotifyCbDownload:
		case ESatNotifyMoSmControlRequest:                    
            {
            iSatNotificationsTsy->DoExtFuncL ( aTsyReqHandle, aIpc, aPackage );
            break;
            }
        // EventDownload
        case ESatEventDownload:
            {
            ret = iSatEventDownloadTsy->DoExtFuncL ( aTsyReqHandle, aIpc, 
                aPackage );
            break;
            }
        // Tsy internal requests
        // Not supported
        /*
        case ESatGetMeSideSatProfile:
        case ESatClientSatProfileIndication:
        case ESatUsatClientReadyIndication:
        case ESatGetProvisioningRefFile:
        case ESatNotifyRefreshRequired:
        // Notifications
        case ESatNotifyPerformCardApduPCmd:
        case ESatNotifyPowerOffCardPCmd:
        case ESatNotifyPowerOnCardPCmd:
        case ESatNotifyGetReaderStatusPCmd:
        case ESatNotifyRunAtCommandPCmd:
		case ESatNotifyPollingOff:
		case ESatNotifyPollingInterval:
        case ESatNotifyServiceSearchPCmd:
        case ESatNotifyGetServiceInfoPCmd:
        case ESatNotifyDeclareServicePCmd:
        case ESatNotifyProactiveSimSessionStart:
        case ESatNotifyTimerExpiration:
        case ESatNotifyTimerMgmtPCmd:
        case ESatNotifyTsyStateUpdated:
        */

		// Complete with KErrNotSupported, function not supported
		default:
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_DOEXTFUNCL_2, "CSAT: CSatTsy::DoExtFuncL unsupported IPC %d", aIpc);
			ReqCompleted( aTsyReqHandle, KErrNotSupported );
			break;
			}
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTsy::MenuSelection
// Passes a menu selection response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatTsy::MenuSelectionL
		(
		TTsyReqHandle   aTsyReqHandle,			
		RSat::TMenuSelectionV1Pckg* aSelection	
		) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_MENUSELECTIONL_1, "CSAT: CSatTsy::MenuSelection");
    TInt ret( KErrNone );

	// It should not be possible that the request is asked more than once at 
	// time, therefore we do not need to check whether it is already ongoing

	RSat::TMenuSelectionV1& selectionV1 = ( *aSelection ) ();
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_MENUSELECTIONL_2, "CSAT:MenuSelection, iItemId %x, Help: %c", selectionV1.iItemId, selectionV1.iHelp );

    TTlv tlvSpecificData;
    
    tlvSpecificData.Begin( KBerTlvMenuSelectionTag );
    tlvSpecificData.AddTag( KTlvDeviceIdentityTag );
    tlvSpecificData.AddByte( KKeypad );
    tlvSpecificData.AddByte( KSim );
    tlvSpecificData.AddTag( KTlvItemIdentifierTag );
    tlvSpecificData.AddByte( selectionV1.iItemId );

	// If user has requested help, add tag.
    if ( RSat::EHelpRequested == selectionV1.iHelp )
        {
        tlvSpecificData.AddTag( KTlvHelpRequestTag );
        }

	// Pack parameter for DOS call.
    TPtrC8 berTlv = tlvSpecificData.End();
	CSatDataPackage dataPackage;
	dataPackage.PackData( &berTlv );

	ret = iMessageManager->HandleRequestL( ESatMenuSelection, &dataPackage );

	if ( KErrNone == ret )
        {
    	// We must complete the request to the client right away, as an
    	// indication that the request has been handled
    	ReqCompleted( aTsyReqHandle, ret );    
        }
    
    return ret; 
    }

// -----------------------------------------------------------------------------
// CSatTsy::SendMessageNoLogging
// Called by ETel server, passes SMS message to MMSMS
// -----------------------------------------------------------------------------
//
TInt CSatTsy::SendMessageNoLogging
        (
        TDes8* aMsg,           
        TTsyReqHandle   aTsyReqHandle   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SENDMESSAGENOLOGGING_1, "CSAT: CSatTsy::SendMessageNoLogging");
    TInt ret( KErrNone );
	
	// Save the request handle
    SaveReqHandle( aTsyReqHandle, ESatSendMessageNoLoggingReqType );
	
    RSat::TSatSmsV1Pckg* aPckg = reinterpret_cast<RSat::TSatSmsV1Pckg*>( 
        aMsg );
    RSat::TSatSmsV1& satSmsV1 = ( *aPckg ) ();

    // SMS GSM extension checks that SCA length is not over the max limit
    // that SMS allows

    // Check that we have a SCA address
    if ( 0x0 < satSmsV1.iServiceCenter.iTelNumber.Length() )
        {
        // SCA number conversion from unicode to GSM 3.40 format 
        // is done by MMSMS dll

        // Set SCA pointer
        TDes16* sca;
        sca = &satSmsV1.iServiceCenter.iTelNumber;

        // Type of Number, mapping from ETelSat format to MMETel format
        RMobilePhone::TMobileTON numberType;
        
        switch ( satSmsV1.iServiceCenter.iTypeOfNumber )
            {
            case RSat::ETypeOfNumberNotSet:
            case RSat::EUnknownNumber:
            	{	
                numberType = RMobilePhone::EUnknownNumber;
                break;
            	}
            case RSat::EInternationalNumber:
            	{
                numberType = RMobilePhone::EInternationalNumber;
                break;
            	}
            case RSat::ENationalNumber:
            	{
                numberType = RMobilePhone::ENationalNumber;
                break;
            	}
            case RSat::ENetworkSpecificNumber:
            	{
                numberType = RMobilePhone::ENetworkSpecificNumber;
                break;
            	}
            case RSat::EDedicatedNumber:
            	{
                numberType = RMobilePhone::ESubscriberNumber;
                break;
            	}
            default:
            	{
                // Unknown or invalid Type of Number
                numberType = RMobilePhone::EUnknownNumber;
                break;
            	}
            }

        // Numbering Plan, mapping from ETelSat format to MMETel format
        RMobilePhone::TMobileNPI numberPlan;
        switch ( satSmsV1.iServiceCenter.iNumberPlan )
            {
            case RSat::ENumberingPlanNotSet:
            case RSat::EUnknownNumberingPlan:
            	{
                numberPlan = RMobilePhone::EUnknownNumberingPlan;
                break;
            	}
            case RSat::EIsdnNumberPlan:
            	{
                numberPlan = RMobilePhone::EIsdnNumberPlan;
                break;
            	}
            case RSat::EDataNumberPlan:
            	{
                numberPlan = RMobilePhone::EDataNumberPlan;
                break;
            	}
            case RSat::ETelexNumberPlan:
            	{
                numberPlan = RMobilePhone::ETelexNumberPlan;
                break;
            	}
            case RSat::ENationalNumberPlan:
            	{
                numberPlan = RMobilePhone::ENationalNumberPlan;
                break;
            	}
            case RSat::EPrivateNumberPlan:
            	{
                numberPlan = RMobilePhone::EPrivateNumberPlan;
                break;
            	}
            default:
            	{
                // Unknown or invalid Numbering Plan
                numberPlan = RMobilePhone::EUnknownNumberingPlan;
                break;
            	}
            }

        // SMS message, Packet Data Unit
        TDes8* pdu;
        pdu = &satSmsV1.iBuf;

        // Call SMS sending function from MMSMS
        ret = iSatService.SendSatMessage( *this, pdu, sca, &numberType, 
                &numberPlan, EFalse, aTsyReqHandle );
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SENDMESSAGENOLOGGING_2, "CSAT: CSatTsy::SendMessageNoLogging, SendSmsMessage done, ret: %x", ret);
       
       
        }
    else // SCA missing
        {
        ret = KErrGeneral;
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SENDMESSAGENOLOGGING_3, "CSAT: CSatTsy::SendMessageNoLogging, SCA missing");
        }

    // Failure in sending of SAT SMS, call complete method
    if ( KErrNone != ret )
        {
        CompleteSendSmsMessage( ret ); 
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTSY_SENDMESSAGENOLOGGING_4, "CSAT: CSatTsy::SendMessageNoLogging, failure sending SAT SMS, complete");
        }
    
    return KErrNone;

    }     

MmMessageManagerCallback* CSatTsy::GetMessageManagerCallback()    
{
	return iMessageManager->GetMessageManagerCallback();
}


//  End of File  
