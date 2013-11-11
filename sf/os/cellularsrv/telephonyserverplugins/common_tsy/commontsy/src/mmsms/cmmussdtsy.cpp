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
#include "cmmussdtsyTraces.h"
#endif

#include <etelmm.h>
#include <mmlist.h>
#include "cmmussdtsy.h"
#include <ctsy/serviceapi/cmmsmsutility.h>
#include "MmTsy_timeoutdefs.h"
#include "MmTsy_numberOfSlots.h"
#include "cmmtsyreqhandlestore.h"
#include "CMmCommonStaticUtility.h"
#include "cmmmessagemanagerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/gsmerror.h>

// ======== MEMBER FUNCTIONS ========

CMmUssdTsy::CMmUssdTsy()
    {
    }

void CMmUssdTsy::ConstructL(
    CMmPhoneTsy* aMmPhoneTsy )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_CONSTRUCTL_1, "TSY: CMmUssdTsy::ConstructL\n" );
    iMmPhone = aMmPhoneTsy;

    // register .
    iMmPhone->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::EUssdMessagingTsy, this );

#ifdef REQHANDLE_TIMER   
    // create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this, iMmPhone,
        EMultimodeUssdMaxNumOfRequests, iUssdReqHandles);
#else
    // create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(
        EMultimodeUssdMaxNumOfRequests, iUssdReqHandles);

#endif // REQHANDLE_TIMER

    // Reset all CMmUssdTsy variables
    ResetVariables();

    // Initialize boolean flag
    iUssdNoFdnCheckFlag =  EUssdNoFdnCheckUnknown;
    }

CMmUssdTsy* CMmUssdTsy::NewL( 
    CMmPhoneTsy* aMmPhone )                                 
    {
    CMmUssdTsy* aMmUssdTsy = new( ELeave ) CMmUssdTsy();
    CleanupClosePushL( *aMmUssdTsy );
    aMmUssdTsy->ConstructL( aMmPhone );
    aMmUssdTsy->iMmPhone = aMmPhone;
    CleanupStack::Pop();
    
    return aMmUssdTsy;
    }

CMmUssdTsy::~CMmUssdTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_DTOR_1, "TSY: CMmUssdTsy::~CMmUssdTsy" );
    // unregister.
    if ( iMmPhone )
        {
        // deregister tsy object from message manager
        iMmPhone->MessageManager()->DeregisterTsyObject(this);        
        }

    // delete req handle store
    delete iTsyReqHandleStore;
    iTsyReqHandleStore = NULL;

    // Null other pointers
    iReturnResultPtr = NULL;
    iReceiveUssdMessagePtr = NULL;
    iReceiveUssdMessageAttributesPtr = NULL;
    iMmPhone = NULL;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::Init
// Initialisation method that is called from ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::ExtFunc
// ExtFunc is called by the server when it has an "extended",i.e.
// non-core ETel request for the TSY. To process a request handle, request type
// and request data are passed to the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::ExtFunc( 
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )        
    {
    TInt ret = KErrNone;
    TInt trapError = KErrNone;

    // before processing further the request, check if offline mode status
    // is enabled and if the given request can be perfomed in that case.
    if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() && 
        ( !IsRequestPossibleInOffline( aIpc ) ) )  
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_EXTFUNC_1, "TSY: Offline mode ON, request is not allowed: %{TIPCNamesList}", aIpc );
        ret = CMmCommonStaticUtility::EpocErrorCode( KErrGeneral, 
            KErrGsmOfflineOpNotAllowed );

        // Complete the request with appropiate error
        ReqCompleted ( aTsyReqHandle, ret ); 
        }
    else
        {
        // Original code continues here.
        TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ); );

        if ( trapError != KErrNone )
            {
            // Object cannot be created.
			// reset request handle to indicate the request is no longer ongoing
            iTsyReqHandleStore->FindAndResetTsyReqHandle(aTsyReqHandle);
            ReqCompleted( aTsyReqHandle, trapError );
            }
        else if ( ret != KErrNone )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::DoExtFuncL
// ExtFunc is called by the server when it has an "extended",i.e.
// non-core ETel request for the TSY. To process a request handle,request type
// and request data are passed to the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::DoExtFuncL( 
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_DOEXTFUNCL_1, "TSY: CMmUssdTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TInt)aIpc, (TUint)aTsyReqHandle, (TUint)this);
    TInt ret(KErrNone);

    switch ( aIpc ) 
        {
        // Ussd Messaging requests
        case EMobileUssdMessagingGetCaps:
            ret = GetCaps( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EMobileUssdMessagingReceiveMessage:
            ret = ReceiveMessage( aTsyReqHandle, aPackage.Des1n(), 
                aPackage.Des2n() );
            break;
        case EMobileUssdMessagingSendMessage:
            {
            iUssdNoFdnCheckFlag = EUssdNoFdnCheckNotUsed;
            ret = SendMessageL( 
                aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n() );
            }
            break;
        case EMobileUssdMessagingSendMessageNoFdnCheck:
            {
            iUssdNoFdnCheckFlag = EUssdNoFdnCheckUsed;
            ret = SendMessageL( 
                aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n() );                
            }
            break;
        case EMobileUssdMessagingSendRelease:
            ret = SendReleaseL( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EMobileUssdMessagingNotifyNetworkRelease:
            switch( aPackage.Type() )
                 {
             	// Switch between alternative implementations of NotifyNetworkRelease
                 case TDataPackage::EPackage1n:
                	 ret = NotifyNetworkRelease( aTsyReqHandle, aPackage.Des1n(), NULL );
                	 break;
                 case TDataPackage::EPackage1n2n:
                	 ret = NotifyNetworkRelease( aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n() );
                     break;
                 default:
                     ret = KErrNotSupported;
                     break;
                 }
            
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_DOEXTFUNCL_2, "TSY: CMmUssdTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::CancelService
// CancelService is called by the server when it is "cleaning-up"
// any still outstanding asynchronous requests before closing a client's
// sub-session. This will happen if a client closes its R-class handle without
// cancelling outstanding asynchronous requests
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::CancelService( 
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle )           
    {
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobileUssdMessagingReceiveMessage:
            ReceiveMessageCancel( aTsyReqHandle );
            ret = KErrNone;
            break;
        case EMobileUssdMessagingSendMessage:
            SendMessageCancel( aTsyReqHandle );
            ret = KErrNone;
            break;
        case EMobileUssdMessagingSendMessageNoFdnCheck:
            SendMessageNoFdnCheckCancel( aTsyReqHandle );
            ret = KErrNone;
            break;            
        case EMobileUssdMessagingSendRelease:
            SendReleaseCancel();
            ret = KErrNone;
            break;
        case EMobileUssdMessagingNotifyNetworkRelease:
            NotifyNetworkReleaseCancel();
            ret = KErrNone;
            break;
        default:
            ret = KErrNotSupported;
            break;
        } 

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::ReqModeL
// When the ETel server receives an "extension" client request,
// it will pass the IPC request number down to the TSY in order to find out
// what type of request it is. The virtual method that gets called is ReqModeL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmUssdTsy::ReqModeL(
    const TInt aIpc )    
    {
    CTelObject::TReqMode ret ( 0 );

    switch ( aIpc )
        {
        // Non-Flow Controlled Services
        case EMobileUssdMessagingGetCaps:
            break;
        // Flow Controlled Services

        // Immediate Server Repost
        case EMobileUssdMessagingReceiveMessage:
        case EMobileUssdMessagingNotifyNetworkRelease:
            ret = KReqModeRePostImmediately | 
                KReqModeMultipleCompletionEnabled;
            break;
        // Services handled by TSY
        case EMobileUssdMessagingSendMessage:
        case EMobileUssdMessagingSendMessageNoFdnCheck:
        case EMobileUssdMessagingSendRelease:
            ret = 0;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::OpenNewObjectL
// Opens new object and names it. Not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmUssdTsy::OpenNewObjectL( 
    TDes& /*aName*/ )    
    {
    User::Leave( KErrNotSupported );
    //lint -e{527} "unreachable code"

    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::OpenNewObjectByNameL
// Open extendible sub-sessions. Not Supported
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmUssdTsy::OpenNewObjectByNameL(
    const TDesC& /*aName*/ )       
    {
    User::Leave( KErrNotSupported );
    //lint -e{527} "unreachable code"

    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::RegisterNotification
// RegisterNotification is called when the server recognises that
// this notification is being posted for the first time on this sub-session
// object. It enables the TSY to "turn on" any regular notification messages
// that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::RegisterNotification(
    const TInt aIpc )                             // Notification IPC number
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_REGISTERNOTIFICATION_1,  "TSY: CMmUssdTsy::RegisterNotification. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobileUssdMessagingNotifyNetworkRelease:
        case EMobileUssdMessagingReceiveMessage:
        	break;
        default:
            // Not supported
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmUssdTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises 
// that this notification will not be posted again because the last client 
// to have a handle on this sub-session object has just closed the handle.
// It enables the TSY to "turn off" any regular notification messages that it
// may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::DeregisterNotification(
    const TInt aIpc )                             // Notification IPC number
    {        
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_DEREGISTERNOTIFICATION_1,  "TSY: CMmUssdTsy::DeregisterNotification. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobileUssdMessagingNotifyNetworkRelease:
        case EMobileUssdMessagingReceiveMessage:
            break;
        default:
            // Not supported
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmUssdTsy::NumberOfSlotsL
// NumberOfSlotsL is called by the server when it is registering 
// a new NOTIFICATION. It enables the TSY to tell the server how many buffer 
// slots to allocate for "repost immediately" notifications that may trigger
// before clients collect them.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    TInt numberOfSlots = 1;
    switch ( aIpc )
        {
        case EMobileUssdMessagingReceiveMessage:
            numberOfSlots = KMmUssdMessagingReceiveMessageSlots;  
            break;
        case EMobileUssdMessagingNotifyNetworkRelease:
            numberOfSlots = KMmUssdMessagingNotifyNetworkReleaseSlots;  
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::GetCaps
// This method returns a class that reflects the Ussd messaging 
// capabilities of the phone. This feature can be completed without sending 
// message, because TSY knows phones Ussd capabilities
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::GetCaps( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aCaps )
    {
    TInt ret = KErrNone;
    if( aCaps->MaxLength() >= sizeof(RMobileUssdMessaging::TMobileUssdCapsV1Pckg) )
    	{
        RMobileUssdMessaging::TMobileUssdCapsV1Pckg* ussdCapsPckg =
            reinterpret_cast<RMobileUssdMessaging::TMobileUssdCapsV1Pckg*> 
            ( aCaps );
        
    	RMobileUssdMessaging::TMobileUssdCapsV1& ussdCaps = ( *ussdCapsPckg )();
    	
    	// TSY knows what capabilities it has.
		// Capabilities depend on the TSY implementation.
		ussdCaps.iUssdTypes = KUssdTypeCaps;

		ussdCaps.iUssdFormat = RMobileUssdMessaging::KCapsPackedString;
    	}
    else
    	{
    	ret = KErrArgument;
    	}

    ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::ReceiveMessage
// This method activates receiving of Ussd messages.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::ReceiveMessage(
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aData, 
    TDes8* aAttributes )              
    {
    if(sizeof(RMobileUssdMessaging::TMobileUssdAttributesV1) > aAttributes->MaxLength())
    	{
    	return KErrArgument;
    	}

    TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1>* ussdAttributesPckg =
         reinterpret_cast<TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1>*>
         ( aAttributes );

    RMobileUssdMessaging::TMobileUssdAttributesV1& ussdAttributes = 
    ( *ussdAttributesPckg )();

    // Save request handle
    iTsyReqHandleStore->SetTsyReqHandle( 
        EMultimodeUssdReceiveMessage, aTsyReqHandle );
    iReceiveUssdMessagePtr = aData;
    iReceiveUssdMessageAttributesPtr = &ussdAttributes;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::ReceiveMessageCancel
// This method cancels reception of USSD messages
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::ReceiveMessageCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeUssdReceiveMessage );

    ReqCompleted( aTsyReqHandle, KErrCancel );
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::CompleteReceiveMessage
// This method completes reception of USSD messages request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::CompleteReceiveMessage(
    TInt aError, 
	CMmDataPackage* aDataPackage )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_COMPLETERECEIVEMESSAGE_1, "TSY: CMmUssdTsy::CompleteReceiveMessage.\n" );
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeUssdReceiveMessage );

    if ( ( EMultimodeUssdReqHandleUnknown != reqHandle ) )
        {
        // reset req handle. Returns the deleted req handle
        reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodeUssdReceiveMessage );

        if ( KErrNone == aError )
            { 
            TDes8* unpackedReceiveUssdMessagePtr = NULL;
            RMobileUssdMessaging::TMobileUssdAttributesV1* unpackedReceiveUssdMessageAttributesPtr = NULL;            
            aDataPackage->UnPackData (&unpackedReceiveUssdMessagePtr, &unpackedReceiveUssdMessageAttributesPtr);           
                                  
            if(iReceiveUssdMessagePtr->MaxLength() >= unpackedReceiveUssdMessagePtr->Length())
               	{             
				*iReceiveUssdMessagePtr = *unpackedReceiveUssdMessagePtr;
				*iReceiveUssdMessageAttributesPtr = *unpackedReceiveUssdMessageAttributesPtr;            		            	
            	}
            else
            	{            	
            	aError = KErrArgument;
            	}               
			}
        ReqCompleted( reqHandle, aError );
        }
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::SendMessageL
// Routes send message to message handler.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::SendMessageL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aData, 
    TDes8* aAttributes )
    { 
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_SENDMESSAGEL_1, "TSY: CMmUssdTsy::SendMessage - Data length: %d", aData->Length() );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_SENDMESSAGEL_2, "TSY: CMmUssdTsy::SendMessageL: iUssdNoFdnCheckFlag: %d", iUssdNoFdnCheckFlag);
    TInt ret = KErrNone;

    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    else
        {
        // Create package
        CMmDataPackage package;
    
	    // Pack call parameters and mobile call info
        package.PackData( aData, &aAttributes );
        
        // Check if NoFdnCheck is used or not
        if ( iUssdNoFdnCheckFlag == EUssdNoFdnCheckUsed )
            {
            // Send request to the Domestic OS layer.
            ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobileUssdMessagingSendMessageNoFdnCheck, &package );   
            }
        if ( iUssdNoFdnCheckFlag == EUssdNoFdnCheckNotUsed ) 
            {
            // Send request to the Domestic OS layer.
            ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobileUssdMessagingSendMessage, &package );            
            }
            
        iSsTransactionOngoing = ETrue;
        }

    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        iUssdNoFdnCheckFlag = EUssdNoFdnCheckUnknown;
        }
    else
    	{
#ifdef REQHANDLE_TIMER       
        // Check if NoFdnCheck is used or not
        if ( iUssdNoFdnCheckFlag == EUssdNoFdnCheckUsed )
            {
            // Set timer for the request
            SetTypeOfResponse( EMultimodeUssdSendMessageNoFdnCheck, 
                aTsyReqHandle );     
            }
        if ( iUssdNoFdnCheckFlag == EUssdNoFdnCheckNotUsed ) 
            {
            // Set timer for the request
            SetTypeOfResponse( EMultimodeUssdSendMessage, 
                aTsyReqHandle );
            }      
#else
        // Check if NoFdnCheck is used or not
        if ( iUssdNoFdnCheckFlag == EUssdNoFdnCheckUsed )
            {
            // Save SendMessageNoFdnCheck request handle, set timer
            iTsyReqHandleStore->SetTsyReqHandle( 
                EMultimodeUssdSendMessageNoFdnCheck, aTsyReqHandle );
            }
        
        if ( iUssdNoFdnCheckFlag == EUssdNoFdnCheckNotUsed ) 
            {
            // Save SendMessage request handle, set timer
            iTsyReqHandleStore->SetTsyReqHandle( 
                EMultimodeUssdSendMessage, aTsyReqHandle );
            }           
#endif // REQHANDLE_TIMER
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SendMessageCancel
// Cancels cancelling of USSD session.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::SendMessageCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // reset the req handle
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeUssdSendMessage );
    
    // complete with cancel
    ReqCompleted( aTsyReqHandle, KErrCancel );
    iSsTransactionOngoing = EFalse;
    
    iUssdNoFdnCheckFlag = EUssdNoFdnCheckUnknown;
    
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmUssdTsy::CompleteSendMessage
// Complete SendMessage 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::CompleteSendMessage(
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_COMPLETESENDMESSAGE_1, "TSY: CMmUssdTsy::CompleteSendMessage.\n" );
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeUssdSendMessage );
	
    if ( EMultimodeUssdReqHandleUnknown != reqHandle )
        {
        // reset req handle. Returns the deleted req handle
        reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodeUssdSendMessage );
        
        ReqCompleted( reqHandle, aError );
        iSsTransactionOngoing = EFalse;
        iUssdNoFdnCheckFlag = EUssdNoFdnCheckUnknown;
        }
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::SendReleaseL
// This function cancels active ussd session asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::SendReleaseL(
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aReturnResult )
    {
    TTsyReqHandle sendReleaseHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodeUssdSendRelease );

    if ( 0 < sendReleaseHandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_SENDRELEASEL_1, "LTSY: CMmUssdTsy::SendRelease - KErrServerBusy");
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_SENDRELEASEL_2, "TSY: CMmUssdTsy::SendRelease called");

        TInt ret = KErrGeneral;

        TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3>* ussdSendSSRequestPckg =
            reinterpret_cast< TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3>* > 
            ( aReturnResult );

        if ( sizeof(RMobilePhone::TMobilePhoneSendSSRequestV3) >
                ussdSendSSRequestPckg->MaxLength() )
            {
            ret = KErrArgument;
            }
        else
            {
            RMobilePhone::TMobilePhoneSendSSRequestV3& returnResult = 
                ( *ussdSendSSRequestPckg )();

            iReturnResultPtr = &returnResult;

            // Send request to the Domestic OS layer.
            ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobileUssdMessagingSendRelease );
            }

        // If ret is not KErrNone
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
        	{
#ifdef REQHANDLE_TIMER
            // Set timer for the request
            SetTypeOfResponse( EMultimodeUssdSendRelease, aTsyReqHandle );
#else
            // Save SendMessage request handle, set timer
            iTsyReqHandleStore->SetTsyReqHandle( 
                EMultimodeUssdSendRelease, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        	}
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::CompleteSendRelease
// This method completes an outstanding SendRelease request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::CompleteSendRelease(
    TInt aErrorCode, 
    CMmDataPackage* aDataPackage )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_COMPLETESENDRELEASE_1, "TSY: CMmUssdTsy::CompleteSendRelease" );
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
		EMultimodeUssdSendRelease );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            aDataPackage->UnPackData ( *iReturnResultPtr );
            }
        ReqCompleted( reqHandle, aErrorCode );
        }  
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SendReleaseCancel
// Cancels cancelling of USSD session.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::SendReleaseCancel()
    {
    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeUssdSendRelease );
    
        if ( EMultimodeUssdReqHandleUnknown != reqHandle )
           {
            // complete with cancel
            ReqCompleted( reqHandle, KErrCancel );
           }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::NotifyNetworkRelease
// This method activates notifying of network release
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::NotifyNetworkRelease(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aMsgData,
    TDes8* aMsgAttributes) // aMsgAttributes may be NULL
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_NOTIFYNETWORKRELEASE_1, "TSY: CMmUssdTsy::NotifyNetworkRelease" );
    
	if (aMsgData->MaxLength() < sizeof(RMobilePhone::TMobilePhoneSendSSRequestV3Pckg))
		{
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_NOTIFYNETWORKRELEASE_2, "TSY: CMmNetTsy::NotifyNetworkRelease Bad size argument (arg1)");
	    // Complete the request with appropiate error        
	    return KErrArgument;		
		}
	if (aMsgAttributes && 
		aMsgAttributes->MaxLength() < sizeof(RMobileUssdMessaging::TMobileUssdAttributesV1Pckg))
		{
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_NOTIFYNETWORKRELEASE_3, "TSY: CMmNetTsy::NotifyNetworkRelease Bad size argument (arg2)");
	    // Complete the request with appropiate error        
	    return KErrArgument;			
		}

	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3>* ussdSendSSRequestPckg =
            reinterpret_cast< TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3>* > 
            ( aMsgData );

    RMobilePhone::TMobilePhoneSendSSRequestV3& msgData = 
            ( *ussdSendSSRequestPckg )();

    iReturnNotifyPtr = &msgData;

    if (aMsgAttributes != NULL)
    	{
	    RMobileUssdMessaging::TMobileUssdAttributesV1Pckg* ussdAttributesPckg = 
	    		reinterpret_cast< RMobileUssdMessaging::TMobileUssdAttributesV1Pckg* > (aMsgAttributes);
	    		
	    RMobileUssdMessaging::TMobileUssdAttributesV1& msgAttributes = (*ussdAttributesPckg)();
	    
	    iReturnNotifyUssdMessageAttributesPtr = &msgAttributes;
    	}
    else
    	{
    	iReturnNotifyUssdMessageAttributesPtr = NULL;
    	}
#ifdef REQHANDLE_TIMER
    // Set timer for the request
    SetTypeOfResponse( EMultimodeUssdNotifyNetworkRelease, aTsyReqHandle );
#else
    // Save SendMessage request handle, set timer
    iTsyReqHandleStore->SetTsyReqHandle( 
        EMultimodeUssdNotifyNetworkRelease, aTsyReqHandle );
#endif // REQHANDLE_TIMER
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::CompleteNotifyNetworkRelease
// This method completes notify network release request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::CompleteNotifyNetworkRelease(
    TInt aErrorCode,
    CMmDataPackage* aDataPackage )
    {

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_COMPLETENOTIFYNETWORKRELEASE_1, "TSY: CMmUssdTsy::CompleteNotifyNetworkRelease. Error: %{TSymbianErrorCodes}", aErrorCode );
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
		EMultimodeUssdNotifyNetworkRelease );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            RMobilePhone::TMobilePhoneSendSSRequestV3* tempReturnNotifyPtr;
            if (iReturnNotifyUssdMessageAttributesPtr != NULL)
				{
				RMobileUssdMessaging::TMobileUssdAttributesV1* tempReturnNotifyUssdMessageAttributesPtr;
				aDataPackage->UnPackData ( &tempReturnNotifyPtr, &tempReturnNotifyUssdMessageAttributesPtr );
				if (tempReturnNotifyPtr != NULL)
				    {
				    *iReturnNotifyPtr = *tempReturnNotifyPtr;
				    }
				else
				    {
                    iReturnNotifyPtr->iOpCode = KErrNone;
                    iReturnNotifyPtr->iAdditionalInfo.Append( KErrNone );
				    }
				if (tempReturnNotifyUssdMessageAttributesPtr != NULL)
					{
					*iReturnNotifyUssdMessageAttributesPtr = *tempReturnNotifyUssdMessageAttributesPtr;
					}
				else
				    {
                    iReturnNotifyUssdMessageAttributesPtr->iFormat = RMobileUssdMessaging::EFormatUnspecified;
                    iReturnNotifyUssdMessageAttributesPtr->iType = RMobileUssdMessaging::EUssdUnknown;
				    }
				}
            else
				{
				aDataPackage->UnPackData ( &tempReturnNotifyPtr );
				if(tempReturnNotifyPtr != NULL)
				    {
                    *iReturnNotifyPtr = *tempReturnNotifyPtr;
				    }
				else
				    {
                    iReturnNotifyPtr->iOpCode = KErrNone;
                    iReturnNotifyPtr->iAdditionalInfo.Append( KErrNone );
				    }
				}
            }
         // Null ret pointers
        iReturnNotifyPtr = NULL;
        iReturnNotifyUssdMessageAttributesPtr = NULL;
       
        // Complete the client request
        ReqCompleted( reqHandle, aErrorCode );
        } 
    } 

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyNetworkReleaseCancel
// Cancels notify network release request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::NotifyNetworkReleaseCancel()
    {
    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeUssdNotifyNetworkRelease );
   
    // This will always be true if the etel is handling the request correctly.
    // In case etel fails to block the request this will prevent crashing. 
    if ( EMultimodeUssdReqHandleUnknown != reqHandle ) 
        {
        // Null ret pointers
        iReturnNotifyPtr = NULL;
        iReturnNotifyUssdMessageAttributesPtr = NULL;
       
        // complete with cancel
        ReqCompleted( reqHandle, KErrCancel );
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::ResetVariables
// Reset used member variables.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::ResetVariables()
    {
    iReceiveUssdMessagePtr = NULL;
    iReceiveUssdMessageAttributesPtr = NULL;
    iReturnResultPtr = NULL;
    iReturnNotifyPtr = NULL;
    iReturnNotifyUssdMessageAttributesPtr = NULL;    
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmUssdTsy::SetTypeOfResponse
// Sets the type of response for a given Handle. Automatic
// mode includes an automatic response in case of non response
// from the DOS in a specified time
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::SetTypeOfResponse(
    const TInt aReqHandleType, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );

    switch ( aReqHandleType )
        {
        case EMultimodeUssdSendMessage:
            timeOut = KMmUssdSendMessageTimeOut;
            break;
        case EMultimodeUssdSendMessageNoFdnCheck:
            timeOut = KMmUssdSendMessageNoFdnCheckTimeOut;
            break;            
        case EMultimodeUssdSendRelease:
            timeOut = KMmUssdSendReleaseTimeOut;
            break;
        default:
            // Does not use timer
            iTsyReqHandleStore->SetTsyReqHandle( 
                aReqHandleType, aTsyReqHandle );
            break;
        }

    if ( timeOut > 0 )
        {
        // The timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType, 
            aTsyReqHandle, timeOut );
        }
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::Complete
// Completes the request due timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::Complete(
    TInt aReqHandleType, 
    TInt aError )
    {
    // All possible TSY req handle types are listed in the
    // switch case below. 
    
    RMobilePhone::TMobilePhoneSendSSRequestV3 returnResult;
    // create package.
    CMmDataPackage package;
    
    switch( aReqHandleType )
        {
        // Cases handled with automatic completion
        case EMultimodeUssdSendMessage:
            CompleteSendMessage( aError );
            break;
        case EMultimodeUssdSendMessageNoFdnCheck:
            CompleteSendMessageNoFdnCheck( aError );
            break;
        case EMultimodeUssdSendRelease:
            returnResult.iOpCode = static_cast< TUint8 >( KErrTimedOut );
            returnResult.iAdditionalInfo.Append ( KErrNone );

            // pack the data
            package.PackData( &returnResult );
            CompleteSendRelease( aError, &package );
            break;
        default:
            ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle( 
				aReqHandleType ), aError );
            break;
        }
    }
#endif // REQHANDLE_TIMER

#ifdef TF_LOGGING_ENABLED
// ---------------------------------------------------------------------------
// CMmUssdTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted for logging purposes. It 
// prints the aTsyReqHandle and aError variable in the log file and then 
// calls CTelObject::ReqCompleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aError )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_REQCOMPLETED_1, "TSY: CMmUssdTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", aTsyReqHandle, aError, this);
    CTelObject::ReqCompleted( aTsyReqHandle, aError );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_REQCOMPLETED_2, "<-- TSY: CMmUssdTsy::ReqCompleted");
    }
#endif // TF_LOGGING_ENABLED

// ---------------------------------------------------------------------------
// CMmUssdTsy::Phone
// This method returns pointer to the Phone object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneTsy* CMmUssdTsy::Phone()
    {
    return iMmPhone;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::IsRequestPossibleInOffLine
// Checks whether a ETel request can be performed or not while offline mode is
// enabled
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmUssdTsy::IsRequestPossibleInOffline( TInt aIpc ) const
    {
    TBool isRequestPossible ( ETrue );

    switch ( aIpc )
        {
        case EMobileUssdMessagingSendMessage:      
        case EMobileUssdMessagingSendMessageNoFdnCheck:
        case EMobileUssdMessagingSendRelease:
            isRequestPossible = EFalse;
            break;
        case EMobileUssdMessagingReceiveMessage:
        default:
            break;
        }
        
    return isRequestPossible;
    }

// ---------------------------------------------------------------------------
// CMmUssdTsy::SendMessageNoFDNCheckCancel
// Cancels SendMessageNoFDNCheck request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmUssdTsy::SendMessageNoFdnCheckCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_SENDMESSAGENOFDNCHECKCANCEL_1, "TSY: CMmUssdTsy::SendMessageNoFdnCheckCancel" );

    // reset the req handle
    TTsyReqHandle reqHandle  = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeUssdSendMessageNoFdnCheck );
    
    if ( EMultimodeUssdReqHandleUnknown < reqHandle )
        {
        // complete with cancel
        ReqCompleted( aTsyReqHandle, KErrCancel );
        iSsTransactionOngoing = EFalse; 
        }
    
    iUssdNoFdnCheckFlag = EUssdNoFdnCheckUnknown;
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmUssdTsy::CompleteSendMessageNoFdnCheck
// Complete SendMessage 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmUssdTsy::CompleteSendMessageNoFdnCheck(
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMUSSDTSY_COMPLETESENDMESSAGENOFDNCHECK_1, "TSY: CMmUssdTsy::CompleteSendMessageNoFdnCheck.\n" );
    // get reg handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodeUssdSendMessageNoFdnCheck );
	
    if ( EMultimodeUssdReqHandleUnknown != reqHandle )
        {
        // reset req handle. Returns the deleted req handle
        reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodeUssdSendMessageNoFdnCheck );
        
        ReqCompleted( reqHandle, aError );
        iSsTransactionOngoing = EFalse;
        iUssdNoFdnCheckFlag = EUssdNoFdnCheckUnknown;
        }
    }


// End of file


