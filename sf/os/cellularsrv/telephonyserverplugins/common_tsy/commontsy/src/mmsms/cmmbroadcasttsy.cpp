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
//



// INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmbroadcasttsyTraces.h"
#endif

#include <mmlist.h>
#include <etelmm.h>
#include "cmmbroadcasttsy.h"
#include "cmmphonetsy.h"
#include "CMmPrivateUtility.h"    // Read all list class 
#include <ctsy/serviceapi/cmmsmsutility.h>
#include "MmTsy_numberOfSlots.h"
#include "cmmtsyreqhandlestore.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmnettsy.h"

// ======== MEMBER FUNCTIONS ========

CMmBroadcastTsy::CMmBroadcastTsy():
    iReqHandleType(EMultimodeBroadcastReqHandleUnknown)
    {
    // Set number of WCDMA CBS Pages to 0
	iWcdmaPageNumber = 0;

	// Defaut value for iWcdmaCbsPageLeft.it means that there are
	// no page left to send to upper layer
	iWcdmaCbsPageLeft = EFalse;

	// First page is always at index 0
	iWcdmaCbsMsgPageIndex = 0;
    }

void CMmBroadcastTsy::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_CONSTRUCTL_1, "TSY: CMmBroadcastTsy::ConstructL");
#ifdef REQHANDLE_TIMER   
    // create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this, iMmPhone, 
        EMultimodeBroadcastMaxNumOfRequests, iBroadcastReqHandles );
#else
    // create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        EMultimodeBroadcastMaxNumOfRequests, iBroadcastReqHandles );
#endif // REQHANDLE_TIMER

    // Reset all CMmBroadcastTsy variables
    ResetVariables();

	// Maximun of subBlocks is 15 so let's put the granularity to 10
	iCbsMsg = new( ELeave ) CArrayPtrFlat< TWcdmaCbsMsg >( 10 );

    // register broadcast tsy in the message manager
    iMmPhone->MessageManager()->RegisterTsyObject( 
        CMmMessageManagerBase::EBroadcastMessagingTsy, this );
    }

CMmBroadcastTsy* CMmBroadcastTsy::NewL( 
    CMmPhoneTsy* aMmPhone )    
    {
    CMmBroadcastTsy* aMmBroadcastTsy = new ( ELeave ) CMmBroadcastTsy();
    CleanupClosePushL( *aMmBroadcastTsy );
    aMmBroadcastTsy->iMmPhone = aMmPhone;
    aMmBroadcastTsy->ConstructL();
    CleanupStack::Pop();

    return aMmBroadcastTsy;
    }

CMmBroadcastTsy::~CMmBroadcastTsy()
    {     
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_DTOR_1, "TSY: CMmBroadcastTsy::~CMmBroadcastTsy");
    if ( iMmPhone )
        {
        // deregister tsy object from message manager
        iMmPhone->MessageManager()->DeregisterTsyObject(this);        
        }
    
    delete iTsyReqHandleStore;
    iTsyReqHandleStore = NULL;

    // Delete iCbsMsg
    delete iCbsMsg;
    iCbsMsg = NULL;

	// Set pointers to NULL
	iMmPhone = NULL;
	iMessageManager = NULL;
	iReceiveCbMessagePtr = NULL;
	iReceiveCbMessageAttributesPtr = NULL;
	iNotifyFilterSettingChangePtr = NULL;
	
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::Init
// Initialisation method
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::Init()
    {
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::ExtFunc
// Handling of extended requests
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::ExtFunc( 
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )        
    {
    TInt ret( KErrNone );
    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    switch ( aIpc )
        {
         // Cell broadcast requests that doesn't need trapping
        case EMobileBroadcastMessagingGetCaps:
            ret = GetCaps( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EMobileBroadcastMessagingGetFilterSetting:
            ret = GetFilterSetting( aTsyReqHandle, reinterpret_cast<
                RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter*>
                ( dataPtr ) );
            break;
        case EMobileBroadcastMessagingGetIdListPhase1:
            TRAP_IGNORE( ret = GetBroadcastIdListPhase1L( aTsyReqHandle,
                reinterpret_cast< CRetrieveMobilePhoneBroadcastIdList::
                TGetBroadcastIdRequest* > ( dataPtr ),
                reinterpret_cast< TInt* > ( dataPtr2 ) ); );
            break;
        case EMobileBroadcastMessagingGetIdListPhase2:
            ret = GetBroadcastIdListPhase2( aTsyReqHandle,
                reinterpret_cast< RMobilePhone::TClientId* > ( dataPtr ),
                aPackage.Des2n() );
            break;
        // Cell broadcast requests that may need trapping
        default:
            // Ensure the ReqHandleType is unset.
            // This will detect cases where this method indirectly calls itself
            // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
            // Such cases are not supported because iReqHandleType is in the context of this class instance,
            // not this request, and we don't want the values set by the inner request and the outer request
            // interfering with each other.
            __ASSERT_DEBUG(iReqHandleType==EMultimodeBroadcastReqHandleUnknown, User::Invariant());

            TInt leaveCode( KErrNone );
            TRAP( leaveCode, ret = DoExtFuncL( aTsyReqHandle, aIpc, 
                aPackage ); );

            if ( KErrNone != leaveCode )
                {
				// reset request handle to indicate the request is no longer ongoing
                iTsyReqHandleStore->FindAndResetTsyReqHandle(aTsyReqHandle);
                ReqCompleted( aTsyReqHandle, leaveCode );				
                }

            // save request handle
            else if ( EMultimodeBroadcastReqHandleUnknown != iReqHandleType )
                {
#ifdef REQHANDLE_TIMER
                SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
                //Never comes here ? See SetTypeOfResponse.
                iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, 
                    aTsyReqHandle );
#endif // REQHANDLE_TIMER
                }
            // We've finished with this value now. Clear it so it doesn't leak
            //  up to any other instances of this method down the call stack
            iReqHandleType = EMultimodeBroadcastReqHandleUnknown;
            break;
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::DoExtFuncL
// Handling of broadcast related requests that needs trapping
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::DoExtFuncL( 
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_DOEXTFUNCL_1, "TSY: CMmBroadcastTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, aTsyReqHandle, (TUint)this);
    TInt ret( KErrNone );
    TAny* dataPtr = aPackage.Ptr1();

    switch ( aIpc )
        {
        // Cell broadcast requests that may need trapping
        case EMobileBroadcastMessagingReceiveMessage:
            ret = ReceiveMessageL( 
                aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n() );
            break;
        case EMobileBroadcastMessagingSetFilterSetting:
            ret = SetFilterSettingL( aTsyReqHandle, 
                reinterpret_cast<
                RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* >
                ( dataPtr ) );
            break;
        case EMobileBroadcastMessagingNotifyFilterSettingChange:
            ret = NotifyFilterSettingChange( reinterpret_cast< 
                RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* >
                ( dataPtr ) );
            break;
        case EMobileBroadcastMessagingStoreIdList:
            ret = StoreBroadcastIdListL( aTsyReqHandle, aPackage.Des1n() );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_DOEXTFUNCL_2, "TSY: CMmBroadcastTsy::DoExtFuncL, error = %{TSymbianErrorCodes}", ret);
    return ret;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::CancelService
// CancelService is called by the server when it is "cleaning-up"
// any still outstanding asynchronous requests before closing a 
// client's sub-session. This will happen if a client closes its
// R-class handle without cancelling outstanding asynchronous 
// requests on
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::CancelService( 
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt ret( KErrNone );
    switch ( aIpc )
        {
        case EMobileBroadcastMessagingNotifyFilterSettingChange:
            ret = NotifyFilterSettingChangeCancel ( aTsyReqHandle );
            break;
        case EMobileBroadcastMessagingSetFilterSetting:
        case EMobileBroadcastMessagingStoreIdList:
        case EMobileBroadcastMessagingGetIdListPhase1:
        case EMobileBroadcastMessagingGetIdListPhase2:
            ret = KErrNone;
            break;
        // This cancel request may need message construction, 
        // so trapping is needed
        case EMobileBroadcastMessagingReceiveMessage:
            // call cancel handling
            TRAPD( leaveCode, ( ret = ReceiveMessageCancelL( aTsyReqHandle ) ) );
            if ( KErrNone != leaveCode )
                {
                ReqCompleted( aTsyReqHandle, leaveCode );
                }
            break;
        default:
            ret = KErrGeneral; 
            break;
        } 

    return ret;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::ReqModeL
// When the ETel server receives an "extension" client request,
// it will pass the IPC request number down to the TSY in order 
// to find out what type of request it is
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmBroadcastTsy::ReqModeL( 
    const TInt aIpc )
    {
    CTelObject::TReqMode ret = 0;

    switch ( aIpc )
        {
        // Non-Flow Controlled Services
        case EMobileBroadcastMessagingGetCaps:
        case EMobileBroadcastMessagingGetFilterSetting:
        case EMobileBroadcastMessagingGetIdListPhase1:
        case EMobileBroadcastMessagingGetIdListPhase2:
        case EMobileBroadcastMessagingStoreIdList:
            break;
        // Flow Controlled Services
        case EMobileBroadcastMessagingSetFilterSetting:
            ret = KReqModeFlowControlObeyed;
            break;
        // Multiple Completion Services with Immediate Server Repost
        // (Usually Notifications)
        case EMobileBroadcastMessagingReceiveMessage:
        case EMobileBroadcastMessagingNotifyFilterSettingChange:
            ret = KReqModeMultipleCompletionEnabled |
                     KReqModeRePostImmediately;
            break;
        default:
            User::Leave ( KErrNotSupported );
            break;         
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::NumberOfSlotsL
// When the ETel server discovers that a request is "repost 
// immediately" it will ask the TSY how big a buffer it wants. 
// NumberOfSlotsL Returns number of slots to be used for given 
// requests
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::NumberOfSlotsL( 
    const TInt aIpc )
    {
    TInt numberOfSlots = 1;
    switch ( aIpc )
        {
        // Number of slots for the receive message
        case EMobileBroadcastMessagingReceiveMessage:   
            numberOfSlots = KMmBroadcastMessagingReceiveMessageSlots;
            break;
        // Number of the slots for the notify changes
        case EMobileBroadcastMessagingNotifyFilterSettingChange:
            numberOfSlots = 
                KMmBroadcastMessagingNotifyFilterSettingChangeSlots;
            break;
        default:
            // Unknown or invalid Broadcast IPC
            User::Leave ( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::OpenNewObjectByNameL
// Creates new object and returns a pointer to it
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmBroadcastTsy::OpenNewObjectByNameL( 
    const TDesC& /*aName*/ )
    {
    User::Leave( KErrNotSupported );
	//lint -e{527} "unreachable code"

    return NULL;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::OpenNewObjectL
// Creates new object and returns a pointer to it
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmBroadcastTsy::OpenNewObjectL( 
    TDes& /*aName*/ )
    {
    User::Leave( KErrNotSupported );
	//lint -e{527} "unreachable code"
    
    return NULL;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::RegisterNotification
// RegisterNotification is called when the server recognises 
// that this notification is being posted for the first time on 
// this sub-session object. It enables the TSY to "turn on" any 
// regular notification messages that it may receive from DOS
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::RegisterNotification( 
    const TInt aIpc )         
    {
    switch ( aIpc )
        {
        case EMobileBroadcastMessagingReceiveMessage:
        case EMobileBroadcastMessagingNotifyFilterSettingChange:
            return KErrNone;
        default:
            // Unknown or invalid Broadcast IPC
            return KErrNotSupported;
        }
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises 
// that this notification will not be posted again because the 
// last client to have a handle on this sub-session object has 
// just closed the handle. It enables the TSY to "turn off" any 
// regular notification messages that it may receive from DOS
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::DeregisterNotification( 
    const TInt aIpc )
    {
    switch ( aIpc )
        {
        case EMobileBroadcastMessagingReceiveMessage:
        case EMobileBroadcastMessagingNotifyFilterSettingChange:
            return KErrNone;
        default:
            // Unknown or invalid Broadcast IPC
            return KErrNotSupported;
        }
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::GetCaps
// This method returns a class that reflects the broadcast 
// messaging capabilities of the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::GetCaps( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aCaps )
    {
    
    TInt ret( KErrArgument );     
    
    if ( aCaps )
        {        
        TInt paramLength = aCaps->MaxLength();
        TInt expectedLength = sizeof( RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg );
                
        if (paramLength == expectedLength)
            {    
            RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg* cbsCapsPckg = 
                reinterpret_cast< RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg* > 
                ( aCaps );
            RMobileBroadcastMessaging::TMobileBroadcastCapsV1& cbsCaps = 
                ( *cbsCapsPckg )();

            cbsCaps.iModeCaps = RMobileBroadcastMessaging::KCapsGsmTpduFormat; 

#ifdef __WINS__

            // force GSM+WCDMA capabilities for emulator testing
            cbsCaps.iModeCaps = RMobileBroadcastMessaging::KCapsWcdmaTpduFormat 
                | RMobileBroadcastMessaging::KCapsGsmTpduFormat;

#endif // __WINS__       

            // TSY supports only "accept all" and "reject all" filtering.
            cbsCaps.iFilterCaps = RMobileBroadcastMessaging::KCapsSimpleFilter;
            
            OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_GETCAPS_1, "TSY:CMmBroadcastTsy::GetCaps:Mode caps=0x%08x, Filter caps=0x%08x",cbsCaps.iModeCaps,cbsCaps.iFilterCaps);
            
            ret = KErrNone;
            ReqCompleted( aTsyReqHandle, ret );
            }
        }        

    return ret;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::ReceiveMessageL
// Activates routing of CB messages, and then waits for incoming CB messages
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::ReceiveMessageL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aMsgData, 
    TDes8* aMsgAttributes )
    {
    TInt ret = KErrArgument;
    
    if (aMsgData && aMsgAttributes)
        {

		RMobileBroadcastMessaging::TBroadcastPageData tempData;
		
		if (aMsgData->MaxLength() == tempData.MaxLength())
			{
			RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 tempV1;
			RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg tempV1Pckg(tempV1);
			
			RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 tempV2;
			RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg tempV2Pckg(tempV2);
			
			if ( (aMsgAttributes->MaxLength() == tempV1Pckg.MaxLength()) || (aMsgAttributes->MaxLength() == tempV2Pckg.MaxLength()) )
				{
				ret = KErrNone;
				}
		    
		    if (ret == KErrNone)
		    	{
			    iReceiveCbMessagePtr = aMsgData;
			    iReceiveCbMessageAttributesPtr = aMsgAttributes;


    		    if ( iWcdmaCbsPageLeft )
    		        {
					// currentpage increased, because pages left to deliver
    		        iWcdmaCurrentPage++;    		        
					
    		        // there are previously received pages left
    		        CompleteReceivedWcdmaCbsMessagePageLeft();
    		        ReqCompleted( aTsyReqHandle, KErrNone );
    		        }
    		    else if ( !iCbRoutingActivated )
    		        {
    		        // DOS's CB routing is not activated
    		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_RECEIVEMESSAGEL_1, "TSY:CMmBroadcastTsy::ReceiveMessageL:DOS's CB routing is not activated, sending activation request.");

    		        //Create package
    		        CMmDataPackage package;

    		        TCbsCbmiAndLangAndFilter data;
    		        data.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
    		        data.iCbmiStorage = KNullDesC; // deprecated
    		        data.iLanguageStorage = KNullDesC; // deprecated

    		        // Pack parameters
    		        package.PackData( &data );

    		        // Send request to the Domestic OS layer.
    		        TInt error = iMmPhone->MessageManager()->HandleRequestL( 
    		            EMobileBroadcastMessagingReceiveMessage, &package );

    		        if ( KErrNone == error )
    		            {
    		            iReqHandleType = EMultimodeBroadcastReceiveMessage;
    		            }    
    		        else
    		            {
    		            // Message construction failed or phonet sender returned error        
    		            ReqCompleted( aTsyReqHandle, error );
    		            }
    		        }
		        else
		            {
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_RECEIVEMESSAGEL_2, "TSY:CMmBroadcastTsy::ReceiveMessageL:DOS's CB routing is activated, waiting for messages.");
		            // routing is active, wait for messages from DOS
		            iReqHandleType = EMultimodeBroadcastReceiveMessage;
					
					// currentpage set to zero - no pages to deliver
		            iWcdmaCurrentPage = 0;
		            }
		        }
		    }
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::InternalCompleteCbRoutingRequest
// Completes routing of CB messages request (does not include CB message)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::InternalCompleteCbRoutingRequest( 
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_INTERNALCOMPLETECBROUTINGREQUEST_1, "TSY:CMmBroadcastTsy::InternalCompleteCbRoutingRequest:error=%{TSymbianErrorCodes}.", aError);
    if ( KErrNone == aError )
        {
        iCbRoutingActivated = ETrue;
        }
    else
        {
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
            EMultimodeBroadcastReceiveMessage );
        
        if ( reqHandle )
            {
            ReqCompleted( reqHandle, aError );      
            }
        }   
	// Check if there are some WCDMA CBS Pages left to send to upper layer
	if ( ( iWcdmaCbsPageLeft ) && ( KErrNone == aError ) )
		{
		TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
            EMultimodeBroadcastReceiveMessage );
        
        if ( reqHandle )
            {
			CompleteReceivedWcdmaCbsMessagePageLeft();
            ReqCompleted( reqHandle, aError );      
            }
		}
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::CompleteReceivedWcdmaCbsMessagePageLeft
// This method sends the pages left for WCDMA CBS message
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::CompleteReceivedWcdmaCbsMessagePageLeft()
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEDWCDMACBSMESSAGEPAGELEFT_1, "TSY:CMmBroadcastTsy::CompleteReceivedWcdmaCbsMessagePageLeft:Delivering page %d to client.", iWcdmaCbsMsgPageIndex);
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg* 
		attrPckg = reinterpret_cast
		< RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg* >
		( iReceiveCbMessageAttributesPtr );

	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2& cbAttrib =
		( *attrPckg )();

	// The bit-mask flags indicating which attributes are present in this 
    // instance
	cbAttrib.iFlags = 
		( RMobileBroadcastMessaging::KBroadcastDataFormat );

	// WCDMA
	cbAttrib.iFormat = RMobileBroadcastMessaging::EFormatWcdmaTpdu;

	// Number of pages
	cbAttrib.iNumberOfPages = ( *iCbsMsg )[iWcdmaCbsMsgPageIndex]
		->iNumberOfPages;

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEDWCDMACBSMESSAGEPAGELEFT_2, "TSY:CMmBroadcastTsy::CompleteReceiveMessageWcdmaCbs: cbAttrib.iNumberOfPages %d .", cbAttrib.iNumberOfPages);

	// Message Type
	cbAttrib.iMessageType = ( *iCbsMsg )[iWcdmaCbsMsgPageIndex]
		->iMessageType;

	// Message ID
	cbAttrib.iMessageId = ( *iCbsMsg )[iWcdmaCbsMsgPageIndex]->iMessageId;

	// Serial Number
	cbAttrib.iSerialNum = ( *iCbsMsg )[iWcdmaCbsMsgPageIndex]->iSerialNum;

	// data coding scheme
	cbAttrib.iDCS = ( *iCbsMsg )[iWcdmaCbsMsgPageIndex]->iDCS;

	iReceiveCbMessagePtr->Copy( ( *iCbsMsg )[iWcdmaCbsMsgPageIndex]->iWcdmaCbsData.Ptr(), 
						( *iCbsMsg )[iWcdmaCbsMsgPageIndex]->iInfoLength );
	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEDWCDMACBSMESSAGEPAGELEFT_3, "TSY:CMmBroadcastTsy::CompleteReceivedWcdmaCbsMessagePageLeft: AppendFormat in use iWcdmaCurrentPage: %d.", iWcdmaCurrentPage );
	_LIT8(KFormat, "%c");

	// Append pagenumber to end of CBS message     
	iReceiveCbMessagePtr->AppendFormat(KFormat, iWcdmaCurrentPage);

	// Increase by 1 the page index 
	iWcdmaCbsMsgPageIndex++;

	// if all the pages have been sent to upper layer
	if ( iWcdmaCbsMsgPageIndex == iWcdmaPageNumber )
		{
		// all the pages have been read so set variable to its default
		// value which means that there are no WCDMA CBS page to pass
		// to upper layer anymore
		iWcdmaCbsPageLeft = EFalse;
		iWcdmaCbsMsgPageIndex = 0;

		// Reset the array
		iCbsMsg->Reset();
		}
	}	

//----------------------------------------------------------------------------
// CMmBroadcastTsy::CompleteReceiveMessage
// This method completes routing of CB messages request 
// (includes message)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::CompleteReceiveMessageGsmCbs( 
    TInt aError, 
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEMESSAGEGSMCBS_1, "TSY:CMmBroadcastTsy::CompleteReceiveMessageGsmCbs:error=%{TSymbianErrorCodes}.",aError);
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeBroadcastReceiveMessage );

    if ( reqHandle )
        {
        if ( KErrNone == aError )
            {
			TGsmCbsMsg cbsMsg;

			//Unpack data
			aDataPackage->UnPackData( cbsMsg );

			RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg* 
				attrPckg = reinterpret_cast
				< RMobileBroadcastMessaging::
				TMobileBroadcastAttributesV1Pckg* >
				( iReceiveCbMessageAttributesPtr );
			
			RMobileBroadcastMessaging::TMobileBroadcastAttributesV1& 
				cbAttrib = ( *attrPckg )();

			cbAttrib.iFlags = 
				( RMobileBroadcastMessaging::KBroadcastDataFormat );

			// GSM Mode
			cbAttrib.iFormat = RMobileBroadcastMessaging::EFormatGsmTpdu;                

			iReceiveCbMessagePtr->Copy( cbsMsg.iCbsMsg );
			}
		ReqCompleted( reqHandle, aError );
		}
	}

//----------------------------------------------------------------------------
// CMmBroadcastTsy::CompleteReceiveMessageWcdmaCbs
// This method complete routing of Wcdma CB messages request 
// (include message)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::CompleteReceiveMessageWcdmaCbs( 
    TInt aError, 
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEMESSAGEWCDMACBS_1, "TSY:CMmBroadcastTsy::CompleteReceiveMessageWcdmaCbs:error=%{TSymbianErrorCodes}.", aError);
	TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeBroadcastReceiveMessage );

	// Reset array
	iCbsMsg->Reset();
	
	// Set to EFalse
	iWcdmaCbsPageLeft = EFalse;
	
    if ( reqHandle )
        {
		 if ( KErrNone == aError )
            {
            CArrayPtrFlat< TWcdmaCbsMsg >* cbsMsgTemp = NULL;
			
			// Unpack data
			aDataPackage->UnPackData( cbsMsgTemp, iWcdmaPageNumber );

			if (iWcdmaPageNumber <= cbsMsgTemp->Count())
				{
				// Copy the pages
				TRAPD( trapError,
					for ( TUint8 i = 0; i < iWcdmaPageNumber; i++ )
						{
						iCbsMsg->AppendL( cbsMsgTemp->At( i ) );
						}		
					);

				if ( KErrNone == trapError )
					{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEMESSAGEWCDMACBS_2, "TSY:CMmBroadcastTsy::CompleteReceiveMessageWcdmaCbs: %d pages received.",iWcdmaPageNumber);
					// first page. index is 0
					iWcdmaCbsMsgPageIndex = 0;
					
					// currentpage is 1 at this state even if multipage cbs
					iWcdmaCurrentPage = 1;
					
					RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg* 
						attrPckg = reinterpret_cast
						< RMobileBroadcastMessaging::
						TMobileBroadcastAttributesV2Pckg* >
						( iReceiveCbMessageAttributesPtr );

					RMobileBroadcastMessaging::TMobileBroadcastAttributesV2& 
						cbAttrib = ( *attrPckg )();
					
					// The bit-mask flags indicating which attributes are present 
					// in this instance
					cbAttrib.iFlags = 
						( RMobileBroadcastMessaging::KBroadcastDataFormat );

					// WCDMA
					cbAttrib.iFormat = RMobileBroadcastMessaging::EFormatWcdmaTpdu;

					// Number of pages
					cbAttrib.iNumberOfPages = ( *iCbsMsg )[iWcdmaCbsMsgPageIndex]
						->iNumberOfPages;

					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEMESSAGEWCDMACBS_3, "TSY:CMmBroadcastTsy::CompleteReceiveMessageWcdmaCbs: cbAttrib.iNumberOfPages %d.", cbAttrib.iNumberOfPages);
				
					// Message Type
					cbAttrib.iMessageType = ( *iCbsMsg )
						[iWcdmaCbsMsgPageIndex]->iMessageType;
					
					// Message ID
					cbAttrib.iMessageId = ( *iCbsMsg )
						[iWcdmaCbsMsgPageIndex]->iMessageId;
					
					// Serial Number
					cbAttrib.iSerialNum = ( *iCbsMsg )
						[iWcdmaCbsMsgPageIndex]->iSerialNum;
					
					// data coding scheme
					cbAttrib.iDCS = ( *iCbsMsg )[iWcdmaCbsMsgPageIndex]->iDCS;

					iReceiveCbMessagePtr->Copy( ( *iCbsMsg )
						[iWcdmaCbsMsgPageIndex]->iWcdmaCbsData );

					
					_LIT8(KFormat, "%c");

					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEMESSAGEWCDMACBS_4, "TSY:CMmBroadcastTsy::CompleteReceiveMessageWcdmaCbs: 1st Page - AppendFormat in use iWcdmaCurrentPage: %d.", iWcdmaCurrentPage );
					// insert current pagenumber end of CBS message
					iReceiveCbMessagePtr->AppendFormat(KFormat, iWcdmaCurrentPage);
					
					// index of the pages begins at 0
					if ( iWcdmaCbsMsgPageIndex == ( iWcdmaPageNumber -1 ) )
						{
						// No page left to send to upper layer
						iWcdmaCbsPageLeft = EFalse;
						iWcdmaCbsMsgPageIndex = 0;
	                    
						// no pages left update current page
						iWcdmaCurrentPage = 0;
						
						// Reset the array
						iCbsMsg->Reset();
						}
					else
						{
						// some pages are waiting to be send to upper layer
						iWcdmaCbsPageLeft = ETrue;
						// Increase by 1 the index of the pages
						iWcdmaCbsMsgPageIndex++;
						}
					}
				else
					{
					aError = trapError;	
					}
				}
			else
				{
				aError = KErrCorrupt;
				}			
			}
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEMESSAGEWCDMACBS_5, "TSY:CMmBroadcastTsy::CompleteReceiveMessageWcdmaCbs:Completing with error=%{TSymbianErrorCodes}.", aError);
			
		ReqCompleted( reqHandle, aError );		
		}		
    }
    
//----------------------------------------------------------------------------
// CMmBroadcastTsy::ReceiveMessageCancelL
// This method releases routing of CB messages, and 
// cancels receiving of next incoming Broadcast Message
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::ReceiveMessageCancelL( 
    const TTsyReqHandle aTsyReqHandle )
    {
    // Lets set this setting to null. Now TSY doesn't anymore try to send 
    // incoming CB message to the client.
    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeBroadcastReceiveMessage );

    if ( iCbRoutingActivated )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_RECEIVEMESSAGECANCELL_1, "TSY:CMmBroadcastTsy::ReceiveMessageCancelL:Routing was active, sending de-activation request.");
        // Create package
        CMmDataPackage package;

        TCbsCbmiAndLangAndFilter data;
        data.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
        data.iCbmiStorage = KNullDesC; // deprecated
        data.iLanguageStorage = KNullDesC; // deprecated

        // Pack parameters
        package.PackData( &data );

        // Send request to the Domestic OS layer.
        TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobileBroadcastMessagingReceiveMessageCancel, &package );

        if ( KErrNone == ret )
            {
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( 
                EMultimodeBroadcastReceiveMessageCancel, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                EMultimodeBroadcastReceiveMessageCancel, aTsyReqHandle );
#endif // REQHANDLE_TIMER
            }
        else    // DOS call returned error
            {
            // This setting must be set to false
            iCbRoutingActivated = EFalse;

            // We have to complete this now, because Etel assumes that 
            // cancel-requests never fail and doesn't call again 
            // ReceiveMessage method (after canceling Etel crash if TSY try to
            // deliver next incoming CB message to Etel). Failing of CB 
            // routing release request shouldn't be possible.
            ReqCompleted( aTsyReqHandle, ret );
            }
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_RECEIVEMESSAGECANCELL_2, "TSY:CMmBroadcastTsy::ReceiveMessageCancelL:Routing was not active.");
        ReqCompleted( aTsyReqHandle, KErrCancel );
        }

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::CompleteReceiveMessageCancel
// This method complete canceling of CB messages routing
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::CompleteReceiveMessageCancel( 
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETERECEIVEMESSAGECANCEL_1, "TSY:CMmBroadcastTsy::CompleteReceiveMessageCancel:error=%{TSymbianErrorCodes}.",aError);
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeBroadcastReceiveMessageCancel );

    if ( reqHandle )
        {
        // This setting must be set to false even if DOS returned error.
        iCbRoutingActivated = EFalse;

        if ( KErrNone == aError )
            {
            ReqCompleted( reqHandle, KErrCancel );
            }
        else
            {
            ReqCompleted( reqHandle, aError );
            }
        }
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::GetFilterSetting
// This method returns the current setting for the receipt of 
// broadcast messages. And completes the request to the client 
// using ReqCompleted
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::GetFilterSetting( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting )
    {
    *aSetting = iCbFilterSetting;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_GETFILTERSETTING_1, "TSY:CMmBroadcastTsy::GetFilterSetting:Filter setting=0x%08x.",iCbFilterSetting);

    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;        
    }   

//----------------------------------------------------------------------------
// CMmBroadcastTsy::SetFilterSettingL
// This method returns the current setting for the receipt 
// of broadcast messages
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::SetFilterSettingL( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter const* aSetting )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_SETFILTERSETTINGL_1, "TSY:CMmBroadcastTsy::SetFilterSettingL:Old filter setting=0x%08x, setting to 0x%08x.",iCbFilterSetting,(TUint)*aSetting);
    if ( ( RMobileBroadcastMessaging::EBroadcastAcceptAll == *aSetting ) ||
         ( RMobileBroadcastMessaging::EBroadcastAcceptNone == *aSetting ) )
       {
        // We can complete this after response from DOS
        TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
            EMultimodeBroadcastReceiveMessage );

        if ( iCbRoutingActivated || 
             ( RMobileBroadcastMessaging::EBroadcastAcceptNone == 
                    iCbFilterSetting &&
               reqHandle ) ) 
            {
            iCbFilterTempSetting = *aSetting;

            // Create package
            CMmDataPackage package;

            TCbsCbmiAndLangAndFilter data;
            data.iSetting = iCbFilterTempSetting;
            data.iCbmiStorage = KNullDesC; // deprecated
            data.iLanguageStorage = KNullDesC; // deprecated

            // Pack parameters
            package.PackData( &data );

            // Lets make new routing request so new filter settings can be 
            // delivered to DOS
            TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobileBroadcastMessagingSetFilterSetting, &package );

            if ( KErrNone == ret )
                {
                iReqHandleType = EMultimodeBroadcastSetFilterSetting;
                }
            else
                {
                // DOS call failed
                ReqCompleted( aTsyReqHandle, ret );
                }
            }
        else    
            {
            iCbFilterSetting = *aSetting;

            // CB routing is not activated. We can complete this now.
            CompleteNotifyFilterSettingChange();
            
            ReqCompleted( aTsyReqHandle, KErrNone );
            }    
        }
    else    
        {
        ReqCompleted( aTsyReqHandle, KErrNotSupported );
        }
        
    return KErrNone;    
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::CompleteSetFilterSetting
// This method complete new filter setting sending
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::CompleteSetFilterSetting( 
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETESETFILTERSETTING_1, "TSY:CMmBroadcastTsy::CompleteSetFilterSetting:error=%{TSymbianErrorCodes}.",aError);
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeBroadcastSetFilterSetting );

    if( reqHandle )
        {
        if ( KErrNone == aError )
            { 
            iCbFilterSetting = iCbFilterTempSetting;

            if ( RMobileBroadcastMessaging::EBroadcastAcceptNone == 
                 iCbFilterSetting )
                {
                iCbRoutingActivated = EFalse;
                }
            else
                {
                iCbRoutingActivated = ETrue;
                }

            CompleteNotifyFilterSettingChange();
            }

        if ( reqHandle )
            {
            ReqCompleted( reqHandle, aError );
            }
        
        //complete the cancel request as well
        CompleteReceiveMessageCancel( aError );
        }
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::NotifyFilterSettingChange
// This method allows a client to be notified if there is a 
// change in the setting for the receipt of broadcast messages
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::NotifyFilterSettingChange( 
    RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_NOTIFYFILTERSETTINGCHANGE_1, "TSY:CMmBroadcastTsy::NotifyFilterSettingChange.");
    iReqHandleType = EMultimodeBroadcastNotifyFilterSetting;
    iNotifyFilterSettingChangePtr = aSetting;

    return KErrNone;    
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::NotifyFilterSettingChangeCancel
// This method cancels an outstanding asynchronous 
// NotifyFilterSettingChange request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::NotifyFilterSettingChangeCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_NOTIFYFILTERSETTINGCHANGECANCEL_1, "TSY:CMmBroadcastTsy::NotifyFilterSettingChangeCancel.");
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeBroadcastNotifyFilterSetting );
    ReqCompleted( aTsyReqHandle, KErrCancel );
	iNotifyFilterSettingChangePtr = NULL; // reset pointer to client memory

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::CompleteNotifyFilterSettingChange
// This method is called when some client change CB Filter Settings
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::CompleteNotifyFilterSettingChange()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETENOTIFYFILTERSETTINGCHANGE_1, "TSY:CMmBroadcastTsy::CompleteNotifyFilterSettingChange.");
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeBroadcastNotifyFilterSetting );

    if ( reqHandle )
        {
		if ( iNotifyFilterSettingChangePtr )
			{
			*iNotifyFilterSettingChangePtr = iCbFilterSetting;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_COMPLETENOTIFYFILTERSETTINGCHANGE_2, "TSY:CMmBroadcastTsy::CompleteNotifyFilterSettingChange.New filter setting is 0x%08x.",iCbFilterSetting);
			}

        ReqCompleted( reqHandle, KErrNone );
        }
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::GetBroadcastIdListPhase1L
// CBMI list is returned to the client in two phases. First phase 
// returns how many bytes the streamed list is
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::GetBroadcastIdListPhase1L( 
    const TTsyReqHandle , 
    CRetrieveMobilePhoneBroadcastIdList::TGetBroadcastIdRequest const* , 
    TInt*  )
    {
    // not supported.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_GETBROADCASTIDLISTPHASE1L_1, "TSY: CMmBroadcastTsy::GetBroadcastIdListPhase1L");
    return KErrNotSupported;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::GetBroadcastIdListPhase2
// CBMI list is returned to the client in two phases. In second 
// phase TSY writes the entries into CMobilePhoneCbmiList using 
// the AddEntryL method
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::GetBroadcastIdListPhase2( 
    const TTsyReqHandle , 
    RMobilePhone::TClientId const* , 
    TDes8*  )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_GETBROADCASTIDLISTPHASE2_1, "TSY:CMmBroadcastTsy::GetBroadcastIdListPhase2.List returned to client.");
    return KErrNotSupported;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::StoreBroadcastIdListL
// This method may be used to store a new version of the entire 
// list of CBMI entries
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmBroadcastTsy::StoreBroadcastIdListL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8 const* /*aBuffer*/ )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMBROADCASTTSY_STOREBROADCASTIDLISTL_1, "TSY:CMmBroadcastTsy::StoreBroadcastIdListL.");
    ReqCompleted( aTsyReqHandle, KErrNotSupported );

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::ResetVariables
// Reset used variables
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::ResetVariables()
    {
    // Set accept all to the CB messages fiter setting
    iCbFilterSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;

    // Set CB routing to unactivated
    iCbRoutingActivated = EFalse;

    // Resets receive message variables
    iReceiveCbMessagePtr = NULL;
    iReceiveCbMessageAttributesPtr = NULL;

    // Reset CB notify variables
    iNotifyFilterSettingChangePtr = NULL;
    }

#ifdef REQHANDLE_TIMER
//----------------------------------------------------------------------------
// CMmBroadcastTsy::SetTypeOfResponse
// Sets the type of response for a given Handle. Automatic
// mode includes an automatic response in case of non response
// from the DOS in a specified time
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::SetTypeOfResponse(
    const TInt aReqHandleType, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );

    // example switch
    switch ( aReqHandleType )
        {
        case EMultimodeBroadcastReceiveMessageCancel:
            timeOut = KMmBroadcastReceiveMessageCancel;
            break;
        case EMultimodeBroadcastSetFilterSetting:
            timeOut = KMmBroadcastSetFilter;
            break;
        // Must not use timer:
        // case EMultimodeBroadcastReceiveMessage:
        // case EMultimodeBroadcastNotifyFilterSetting:
        default:
            // does not use timer
            iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType,    
                aTsyReqHandle );
            break;
        }

    if ( timeOut > 0 )
        {
        // the timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType, aTsyReqHandle, 
            timeOut );
        }
    }

//----------------------------------------------------------------------------
// CMmBroadcastTsy::Complete
// Completes the request due timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmBroadcastTsy::Complete(
    TInt aReqHandleType, 
    TInt aError )
    {
    // All possible TSY req handle types are listed in the
    // switch case below. 
    switch( aReqHandleType )
        {
        //Cases handled with automatic completion
        case EMultimodeBroadcastReceiveMessageCancel:
            CompleteReceiveMessageCancel( aError );
            break;
        case EMultimodeBroadcastSetFilterSetting:
            CompleteSetFilterSetting( aError );
            break;
        // Can't use timer:
        // case EMultimodeBroadcastReceiveMessage:
        // case EMultimodeBroadcastNotifyFilterSetting:
        default:
            ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle( 
                aReqHandleType ), aError );
            break;
        }
    }
#endif // REQHANDLE_TIMER

// End of the file

