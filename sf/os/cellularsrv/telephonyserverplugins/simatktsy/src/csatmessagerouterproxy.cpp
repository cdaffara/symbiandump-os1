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
// Name        : CSatMessageRouterProxy.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Routes between Tsy objects and Message Handler objects
// Version     : 1.0
// INCLUDE FILES
//




#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csatmessagerouterproxyTraces.h"
#endif

#include <satcs.h>
// Tsy module files
#include "CSatDataPackage.h"
// Sat
#include "CSatMessageRouterProxy.h"
#include "CSatTsy.h"
#include "CSatNotificationsTsy.h"
#include "CSatCCTsy.h"	
#include "CSatIconTsy.h"	
#include "CSatNotifyDisplayText.h"
#include "CSatNotifyGetInkey.h"
#include "CSatNotifyGetInput.h"
#include "CSatNotifyPlayTone.h"
#include "CSatNotifySetUpMenu.h"
#include "CSatNotifySelectItem.h"
#include "CSatNotifySendSm.h"
#include "CSatNotifySendUssd.h"
#include "CSatNotifySendSs.h"
#include "CSatNotifySetUpIdleModeText.h"
#include "CSatNotifyLaunchBrowser.h"
#include "CSatNotifySimSessionEnd.h"
#include "CSatNotifyCallControlRequest.h"
#include "CSatNotifySetUpCall.h"
#include "CSatNotifySetUpEventList.h"
#include "CSatNotifyTimerMgmt.h"
#include "CSatNotifyLanguageNotification.h"
#include "CSatNotifySendDtmf.h"
#include "CSatNotifyPollingOff.h"
#include "CSatNotifyPollInterval.h"
#include "CSatNotifyLocalInfo.h"
#include "CSatNotifyRefresh.h"
#include "CSatNotifyMoSmControlRequest.h"
#include "CSatEventDownloadTsy.h"
#include "CSatDataDownloadTsy.h"
// Utilities
#include "msattsy_ipcdefs.h"
#include "ctsydelegates.h"

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES  
    //None

// CONSTANTS
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// ==================== LOCAL FUNCTIONS =====================================

    //None


// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatMessageRouterProxy* CSatMessageRouterProxy::NewL(CTsyDelegates& aTsyDelegates) 
    { 
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_NEWL_1,  "CSAT: CSatMessageRouterProxy::NewL" );
    
    CSatMessageRouterProxy* satMessageRouterProxy = 
        new ( ELeave ) CSatMessageRouterProxy(aTsyDelegates);
    CleanupStack::PushL( satMessageRouterProxy );
    satMessageRouterProxy->ConstructL();
    CleanupStack::Pop();

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_NEWL_2, "CSAT: CSatMessageRouterProxy::NewL, end of method");
    return satMessageRouterProxy;
    }

// -----------------------------------------------------------------------------
// CSatMessageRouterProxy::~CSatMessageRouterProxy
// Destructor
// -----------------------------------------------------------------------------
//
CSatMessageRouterProxy::~CSatMessageRouterProxy
        (
        void
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_DTOR_1,  "CSAT: CSatMessageRouterProxy::~CSatMessageRouterProxy" );
    }

// -----------------------------------------------------------------------------
// CSatMessageRouterProxy::CSatMessageRouterProxy
// C++ constructor
// -----------------------------------------------------------------------------
//
CSatMessageRouterProxy::CSatMessageRouterProxy(CTsyDelegates& aTsyDelegates)
: iTsyDelegates(aTsyDelegates)
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatMessageRouterProxy::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CSatMessageRouterProxy::ConstructL
        (    
        void
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_CONSTRUCTL_1,  "CSAT: CSatMessageRouterProxy::ConstructL, noes nothing" );
    }
    
// -----------------------------------------------------------------------------
// CSatMessageRouterProxy::RouteRequest
// Dummy implementation
// -----------------------------------------------------------------------------
//
MMmMessHandlerBase* CSatMessageRouterProxy::RouteRequest
		( 
		TInt /*aIpc*/ 
		)
	{
	// Dummy implementation
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_ROUTEREQUEST_1,  "CSAT: CSatMessageRouterProxy::RouteRequest: DUMMY" );
	return NULL;
	}
	
// -----------------------------------------------------------------------------
// CSatMessageRouterProxy::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
void CSatMessageRouterProxy::RouteCompletion
        (
        TInt aIpc,
        CSatDataPackage* aDataPackage,
        TInt aResult 
        )        
    {
    TInt trapError( KErrNone );
    TRAP( trapError, RouteCompletionL( aIpc, aDataPackage, aResult ) );
    
    if ( KErrNone != trapError )
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_ROUTECOMPLETION_1, "CSatMessageRouterProxy::RouteCompletion, trapError=%d", trapError);
		}
    }
	
	
// -----------------------------------------------------------------------------
// CSatMessageRouterProxy::GetObjectForCompletion
// Returns the pointer to the Tsy object needed for completion, based 
// on the IPC number and the data package
// -----------------------------------------------------------------------------
//
CBase* CSatMessageRouterProxy::GetObjectForCompletion
        (
        const TInt aIpc,
        CSatDataPackage* /*aDataPackage*/
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_GETOBJECTFORCOMPLETION_1, "CSAT: CMmMessageRouterBase::GetObjectForCompletion");
    CBase* satObject = NULL;

	CMmMessageManagerBase::TTsyObjects tsyObject = 
        GetTsyObjectFromIpc( aIpc );
    // Get the object
	satObject = iTsyDelegates.GetTsyObject(tsyObject);
	return satObject;
    }

// -----------------------------------------------------------------------------
// CSatMessageRouterProxy::GetTsyObjectFromIpc
// Returns the type of Tsy object needed for completion, based 
// on the IPC number
// -----------------------------------------------------------------------------
//
 CMmMessageManagerBase::TTsyObjects CSatMessageRouterProxy::GetTsyObjectFromIpc
        (
        const TInt aIpc
        ) const

    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_GETTSYOBJECTFROMIPC_1,  "CSAT: CSatMessageRouterProxy::GetTsyObjectFromIpc" );
    // A default value must be given as a initialization, however, nothing will
    // be done if the IPC does not mach as well in RouteCompletion
    CMmMessageManagerBase::TTsyObjects retObject = 
        CMmMessageManagerBase::ESatTsyObjType;

	switch ( aIpc )
        {
        // General
        case ESatTerminalRsp:
        case ESatMenuSelection:
        case ESatSendMessageNoLogging:
            {
            retObject = CMmMessageManagerBase::ESatTsyObjType; 
            break;
            }
        // Icon
        case ESatTsyGetIconData:                             
        case ESatTsyGetImageInstance:
        case ESatTsyGetClut:
            {
            retObject = CMmMessageManagerBase::ESatIconTsyObjType; 
            break;
            }
        // Notifications
        case ESatTsyPCmd:
        case ESatTsySetPolling:
        case ESatTsySetProactiveCommandOnGoingStatus:
            {
            retObject = CMmMessageManagerBase::ESatNotificationsTsyObjType; 
            break;
            }
        case ESatNotifyProactiveSimSessionEnd:
        	{
        	retObject = CMmMessageManagerBase::ESatNotifySimSessionEndObjType;
        	break;
        	}
        case ESatTsyRefresh:
        case ESatTsyCompleteRefresh:
        case ESatTsyImsiChanged:
            {
            retObject = CMmMessageManagerBase::ESatNotifyRefreshObjType;
            break;
            }
        case ESatTsyNotifyLocalInfo:
        case ESatTsyProvideLocalInfo:
        case ESatTsyLocalInformationNmr:
        case ESatTsyAccTech:
        case ESatTsyTimingAdvance:
        case ESatTsyTimeZone:
        case ESatTsyGetIMEI:
        case ESatNotifyLocalInfoPCmd:
            {
            retObject = CMmMessageManagerBase::ESatNotifyLocalInfoObjType; 
            break;
            }
        case ESatNotifySetUpEventListPCmd:
            {
            retObject = CMmMessageManagerBase::ESatNotifySetUpEventListObjType; 
            break;
            }
        case ESatNotifyMoSmControlRequest:
        case ESatTsyMoSmControlEnvelope:
        case ESatTsyMoSmsControlActivation:
        	{
        	retObject = 
        		CMmMessageManagerBase::ESatNotifyMoSmControlRequestObjType;
        	break;	 
        	}
        case ESatTsySsStatus:
        	{
        	retObject = CMmMessageManagerBase::ESatNotifyLaunchBrowserObjType;
        	break;
        	}
        case ESatNotifyCallControlRequest:
            {
            retObject = 
                CMmMessageManagerBase::ESatNotifyCallControlRequestObjType;
            break;
            } 
        // Event Download
        case ESatTsyCallDisconnected:
        case ESatTsyLocationStatus:
        case ESatTsyCallConnected:
        case ESatTsyMTCall:
        case ESatTsyAccessTechnologyChange:
        case ESatEventDownload:
        case ESatTsyRemoveEventList:
            {
            retObject = CMmMessageManagerBase::ESatEventDownloadTsyObjType;  
            break;
            }
		// Call Control
		case ESatTsyGetBearerCapability:
        case ESatTsyCheckAtkDataForAlphaId:
        case ESatTsyCallEvent:
        case ESatTsySSEvent:
        case ESatTsyUSSDEvent:
        case ESatTsyCreateEnvelope:
        case ESatTsyUssdStatus:
            {
            retObject = CMmMessageManagerBase::ESatCCTsyObjType;
            break;
            }           
        case ESatTsyCellBroadcast:
        case ESatTsySmsPpDdl:
        case ESatTsyReadCbmids:
        case ESatTsySmsPpDdlStatus:
        case ESatTsySmsDeliverReport:
            {
            retObject = CMmMessageManagerBase::ESatDataDownloadTsyObjType; 
            break;
            }
        default:
            break;
        }
  
    return retObject;
    }
    
// ---------------------------------------------------------------------------
// CMmMessageRouterProxy::Complete 
// Complete with a data package
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CSatMessageRouterProxy::Complete( 
    TInt aIpc, 
    CMmDataPackage* aData, 
    TInt aResult )
	{
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_COMPLETE_1, "TSY: CMmMessageRouterProxy::Complete. IPC = %d, result: %d", aIpc, aResult);
	RouteCompletion( aIpc, static_cast<CSatDataPackage*>(aData), aResult ); 
	}
	
// ---------------------------------------------------------------------------
// CMmMessageRouterProxy::Complete 
// Complete without a data package
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CSatMessageRouterProxy::Complete( TInt aIpc, TInt aResult )
	{    
    CSatDataPackage emptyDataPackage;
	Complete( aIpc, &emptyDataPackage, aResult );
	}

// -----------------------------------------------------------------------------
// CSatMessageRouterProxy::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on 
// IPC number and data package
// -----------------------------------------------------------------------------
//
void CSatMessageRouterProxy::RouteCompletionL
        ( 
        TInt aIpc,
        CSatDataPackage* aDataPackage,
        TInt aResult
        )
    {
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_ROUTECOMPLETIONL_1, "CSAT: CSatMessageRouterProxy::RouteCompletion ipc=%d, error: %d", aIpc, aResult);
    CBase* object = GetObjectForCompletion( aIpc, aDataPackage );

    if ( NULL != object )
        { 
		switch ( aIpc )
			{
			case ESatTsyPCmd:
				{
				reinterpret_cast<CSatNotificationsTsy*>( object )->
                	CompletePCmdL( aDataPackage, aResult );
                break;
                }
           case ESatNotifyProactiveSimSessionEnd:
                {
                reinterpret_cast<CSatNotifySimSessionEnd*>( object )->
                    CompleteNotifyL( aDataPackage, aResult );
                break;
                }
			case ESatTsyProvideLocalInfo:
                {
                reinterpret_cast<CSatNotifyLocalInfo*>( object )->
                    CompleteUpdateLocalInfo( aDataPackage, aResult ); 
                break;
                }			
			case ESatTsyLocalInformationNmr:
                {
                reinterpret_cast<CSatNotifyLocalInfo*>( object )->
                    CompleteUpdateNetInfo( aDataPackage, aResult ); 
                break;
                }			
			case ESatTsyAccTech:
                {
                reinterpret_cast<CSatNotifyLocalInfo*>( object )->
                    CompleteUpdateAccTech( aDataPackage, aResult ); 
                break;
                }			
			case ESatTsyTimeZone:
                {
                reinterpret_cast<CSatNotifyLocalInfo*>( object )->
                    CompleteUpdateTimeZone( aDataPackage, aResult ); 
                break;
                }
			case ESatTsyGetIMEI:
                {
                reinterpret_cast<CSatNotifyLocalInfo*>( object )->
                    CompleteUpdateIMEI( aDataPackage, aResult ); 
                break;
                }
			case ESatTsyTimingAdvance:
				{
				reinterpret_cast<CSatNotifyLocalInfo*>( object )->
                    CompleteUpdateTimingAdvAndMEStatus( aDataPackage, aResult ); 
				break;
				}
            case ESatTsyNotifyLocalInfo:
                {
                reinterpret_cast<CSatNotifyLocalInfo*>( object )->
                    CompleteTerminalRespDataL( aDataPackage, aResult ); 
                break;
                }
            case ESatTsySetPolling:
                {
                reinterpret_cast<CSatNotificationsTsy*>( object )->
                    SetPollingResultL( aDataPackage, aResult );
                 break;
                }
			case ESatTsyCreateEnvelope:
			    {
				reinterpret_cast<CSatCCTsy*>( object )->CreateEnvelopeL( 
				    aDataPackage );
				break;
				}
			case ESatTsyMoSmControlEnvelope:
				{
				reinterpret_cast<CSatNotifyMoSmControlRequest*>( object )->
                    CreateMoSmsCtrlEnvelopeL( aDataPackage, aResult) ;
                break;
				}
			case ESatTsyGetBearerCapability:
				{
				reinterpret_cast<CSatCCTsy*>( object )->
                    CompleteBearerCapability( aDataPackage, aResult );
				break;
				}
			case ESatTsyCheckAtkDataForAlphaId:
				{
				reinterpret_cast<CSatCCTsy*>( object )->CheckIfAlphaIdPresent( 
				    aDataPackage );
				break;
				}
			case ESatTsyCallEvent:
			case ESatTsySSEvent:
			case ESatTsyUSSDEvent:
				{
				reinterpret_cast<CSatCCTsy*>( object )->CompleteCCEventL( 
				    aIpc, aDataPackage );
				break;
				}
			case ESatNotifyCallControlRequest:
				{
				reinterpret_cast<CSatNotifyCallControlRequest*>( object )->
                    CompleteNotifyL( aDataPackage, aResult );
				break;
				}
			case ESatNotifyMoSmControlRequest:
				{
				reinterpret_cast<CSatNotifyMoSmControlRequest*>( object )->
                    CompleteNotifyL( aDataPackage, aResult) ;
                break;
				}
			case ESatTsyMoSmsControlActivation:
				{
				reinterpret_cast<CSatNotifyMoSmControlRequest*>( object )->
                    SetActivationStatus( aDataPackage ) ;
                break;
				}
            case ESatTsyCallConnected:
				{
				reinterpret_cast<CSatEventDownloadTsy*>( object )->
                    CompleteCallConnectedL( aDataPackage );
				break;
				}
            case ESatTsyCallDisconnected:
				{
				reinterpret_cast<CSatEventDownloadTsy*>( object )->
                    CompleteCallDisconnectedL( aDataPackage );
				break;
				}
            case ESatTsyLocationStatus:
				{
				reinterpret_cast<CSatEventDownloadTsy*>( object )->
                    CompleteLocationStatusL( aDataPackage );
				break;
				}
          	case ESatTsyAccessTechnologyChange:
				{
				reinterpret_cast<CSatEventDownloadTsy*>( object )->
                    CompleteAccessTechnologyChangeL( aDataPackage );
				break;
				}
			case ESatTsyMTCall:
				{
				reinterpret_cast<CSatEventDownloadTsy*>( object )->
                    CompleteMTCallL( aDataPackage );
				break;
				}
			case ESatTsyRemoveEventList:
				{
				reinterpret_cast<CSatEventDownloadTsy*>( object )->
                    SetUpEventList( 0 );
				break;
				}
            case ESatTsyCellBroadcast:
				{
				reinterpret_cast<CSatDataDownloadTsy*>( object )->
                    CompleteCellBroadcastDdlL( aDataPackage );
				break;
				}
            case ESatTsySmsPpDdl:
				{
				reinterpret_cast<CSatDataDownloadTsy*>( object )->
                    CompleteSmsPpDdlL( aDataPackage );
				break;
				}
            case ESatTsyReadCbmids:
				{
				reinterpret_cast<CSatDataDownloadTsy*>( object )->
                    CompleteReadCbmidsL( );
				break;
				}
			case ESatTsySmsPpDdlStatus:
			    {
				reinterpret_cast<CSatDataDownloadTsy*>( object )->
				    SetSmsPpDdlStatus( aDataPackage ) ;
                break;	
				}
			case ESatTsySmsDeliverReport:
			    {
			    reinterpret_cast<CSatDataDownloadTsy*>( object )->
				    CreateSmsDeliverReportL( aResult ) ;
                break;
			    }
            case ESatTsyGetIconData:
				{
				reinterpret_cast<CSatIconTsy*>( object )->
                    CompleteGetIcon( aDataPackage, aResult );
				break;
				}
			case ESatTsyGetImageInstance:
				{
				reinterpret_cast<CSatIconTsy*>( object )->
                    CompleteGetImageInstance( aDataPackage, aResult );
				break;
				}
        	case ESatTsyGetClut:
        		{
				reinterpret_cast<CSatIconTsy*>( object )->
                    CompleteGetClut( aDataPackage, aResult );
				break;
				}
            case ESatTsyRefresh:
                {
                reinterpret_cast<CSatNotifyRefresh*>( object )->
                    CompleteTerminalRespDataL( aDataPackage, aResult);
                break;
                }
			case ESatTsyCompleteRefresh:
				{
                reinterpret_cast<CSatNotifyRefresh*>( object )->
                    CompleteRequest( aDataPackage, aResult);
                break;    				
				}
			case ESatTsyImsiChanged:
				{
				 reinterpret_cast<CSatNotifyRefresh*>( object )->
                    CompleteImsiChange();
                break;	
				}
            case ESatTsySetProactiveCommandOnGoingStatus:
                {
                reinterpret_cast<CSatNotificationsTsy*>( object )->
                    SetProactiveCommandOnGoingStatus( aDataPackage, aResult );
                break;
                }
			case ESatTsySsStatus:
				{
				 reinterpret_cast<CSatNotifyLaunchBrowser*>( object )->
                    SetSsStatus( aDataPackage ) ;
                break;	
				}	
			case ESatTsyUssdStatus:
			    {
				 reinterpret_cast<CSatCCTsy*>( object )->SetUssdStatus( 
				    aDataPackage ) ;
                break;	
				}
            default:
                break;
            }
        }
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATMESSAGEROUTERPROXY_ROUTECOMPLETIONL_2, "CSatMessageRouterProxy::RouteCompletion, IPC NOT HANDLED.!!!");
		}
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================

    //None


//  End of File 

