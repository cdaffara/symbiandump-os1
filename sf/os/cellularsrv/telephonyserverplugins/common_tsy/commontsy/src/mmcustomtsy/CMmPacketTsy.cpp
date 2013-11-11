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




#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CMmPacketTsyTraces.h"
#endif

#include "CMmCustomTsy.h"
#include "CMmPacketTsy.h"
#include "Cmmpacketcontexttsy.h"
#include "Cmmpacketcontextlist.h"

// ======== MEMBER FUNCTIONS ========

CMmPacketTsy::CMmPacketTsy()
    {
    }

void CMmPacketTsy::ConstructL()
    {
#ifdef REQHANDLE_TIMER   
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( iMmCustomTsy,
        iMmPhone, EMultimodePacketMaxNumOfRequests, iPacketReqHandles );
#else
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        EMultimodePacketMaxNumOfRequests, iPacketReqHandles );
#endif // REQHANDLE_TIMER

    iGprsInformationPckg = NULL;
    }

CMmPacketTsy* CMmPacketTsy::NewL(
    CMmCustomTsy* aMmCustomTsy, 
    CMmPhoneTsy* aMmPhone )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_NEWL_1,  "CustomTSY: CMmPacketTsy::NewL" );
    CMmPacketTsy* mmPacketTsy = new ( ELeave ) CMmPacketTsy();
    CleanupStack::PushL( mmPacketTsy );
    mmPacketTsy->iMmCustomTsy = aMmCustomTsy;
    mmPacketTsy->iMmPhone = aMmPhone;
    mmPacketTsy->ConstructL();
    CleanupStack::Pop();

    return mmPacketTsy;
    }

CMmPacketTsy::~CMmPacketTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_DTOR_1,  "CustomTSY: CMmPacketTsy::~CMmPacketTsy" );
    delete iTsyReqHandleStore;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::Init
// Initialisation method that is called from ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::RegisterNotification
// RegisterNotification is called when the ETel Server recognises that this 
// notification is being posted for the first time on this subsession object.
// It enables the TSY to "turn on" any regular notification messages that it 
// may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketTsy::RegisterNotification(
    const TInt aIpc )
    {
    TInt ret( KErrNotSupported );

    if ( ECustomNotifyEGprsInfoChange == aIpc )
        {
        ret = KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::DeregisterNotification
// DeregisterNotification is called when the ETel Server recognises that this
// notification will not be posted again because the last client to have
// a handle on this sub-session object has just closed the handle. It
// enables the TSY to "turn off" any regular notification messages that
// it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketTsy::DeregisterNotification( 
    const TInt aIpc )
    {
    TInt ret( KErrNotSupported );

    if ( ECustomNotifyEGprsInfoChange == aIpc )
        {
        ret = KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::SupportingIPC
// Use switch to check if given IPC is supported
// in case IPC supported
//     return ETrue
// in case IPC not supported
//     return EFalse
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPacketTsy::SupportingIPC( 
    TInt aIpc )
    {
    TBool ret( EFalse );
    switch ( aIpc )
        {
        // from CustomApi
        case ECustomNotifyEGprsInfoChange:
        case ECustomGetEGprsInfo:
        case ECustomSetAlwaysOnMode:
            ret = ETrue;
            break;
        default:
            ret = EFalse;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::DoExtFuncL
// Dispatches extension function requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_DOEXTFUNCL_1,  "TSY: CMmPacketTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", aIpc, (TUint)aTsyReqHandle, (TUint)this );
    TInt ret( KErrNotSupported );

    switch ( aIpc )
        {
        case ECustomNotifyEGprsInfoChange:
            ret = NotifyEGprsInfoChange( aTsyReqHandle, aPackage.Des1n() );
            break;
        case ECustomGetEGprsInfo:
            ret = GetEGprsInfo( aTsyReqHandle, aPackage.Des1n() );
            break;
        case ECustomSetAlwaysOnMode:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_DOEXTFUNCL_2,  "TSY:CMmPacketTsy::DoExtFuncL ECustomSetAlwaysOnMode");
            ret = SetAlwaysOnL( aTsyReqHandle,
               ( REINTERPRET_CAST( RMmCustomAPI::TSetAlwaysOnMode*,
                              					aPackage.Ptr1() ) ) );
        default:
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_DOEXTFUNCL_3, "<-- TSY: CMmPacketTsy::DoExtFuncL, error = %{TSymbianErrorCodes}", ret);    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::ReqModeL
// Returns request mode for given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmPacketTsy::ReqModeL( 
    const TInt aIpc )
    {
    CTelObject::TReqMode reqMode( 0 );

    switch ( aIpc )
        {
        // Etel Server takes care of flow control. Several simultaneous
        // requests allowed.

        // All disabled
        // TSYs wishing to implement their own buffering algorithm will place
        // all requests in this category. 

        // Get methods that do not use DOS and return immediately.
        // Flow control not required.

        // Methods that can probably take a long time and cannot therefore be
        // flow controlled. Solution: All these methods must check req handle
        // table before handling the request. If the request table indicates
        // that same method has been called and has not been completed, the
        // method should return KErrServerBusy.
        case ECustomGetEGprsInfo:
        case ECustomSetAlwaysOnMode:
            break;

        // KReqModeMultipleCompletionEnabled | ReqModePostImmediately
        // Requests that notify a client about a change of state.
        // Since these requests do not require the issuing of any modem 
        // commands, they do not have to obey flow control.
        // The TSY never gets more than one of these outstanding.
        case ECustomNotifyEGprsInfoChange:
            reqMode = KReqModeMultipleCompletionEnabled |
                KReqModeRePostImmediately;
            break;
        // KReqModeFlowControlObeyed
        // Commands that change the state of the phone, e.g. clearing the AoC 
        // counter, are commands that the TSY should only deal with one at 
        // a time.

        //Other cases
            //reqMode = KReqModeFlowControlObeyed;
            //break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return reqMode;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    TInt numberOfSlots( KMmCustomDefaultSlots );

    switch ( aIpc )
        {
        // from CustomApi
        case ECustomNotifyEGprsInfoChange: 
        case ECustomGetEGprsInfo:
        case ECustomSetAlwaysOnMode:
            break;
        default:
            // Unknown or invalid IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }
    
// ---------------------------------------------------------------------------
// CMmPacketTsy::CancelService
// This function is called to cancel the outstanding request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketTsy::CancelService(
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_CANCELSERVICE_1,  "CustomTSY: CMmPacketTsy::CancelService IPC:%{TIPCNamesList}", aIpc );
    TInt ret( KErrNone );

    // Req handle type
    TPacketRequestType reqHandletype( 
        EMultimodePacketContextReqHandleUnknown );

    switch ( aIpc )
        {
        // TSY has started a request and it is not possible to then cancel 
        // this request. The best thing for the TSY to do in this case is to 
        // proceed as though the Cancel never happened. The server's call to
        // the TSY cancel function will return synchronously. The TSY then 
        // continues to wait for the original acknowledgement and when it 
        // receives it, the TSY will complete the original request.
        case ECustomGetEGprsInfo:
            break;
            
        case ECustomSetAlwaysOnMode:
        	 CancelSetAlwaysOn ( aTsyReqHandle );
            break;

        // Notification Cancels, no special requirements.
        case ECustomNotifyEGprsInfoChange:
            iGprsInformationPckg = NULL;
            reqHandletype = EMultimodePacketContextNotifyEGprsInfoChange;
            break;
        // Cancel methods that are not supported
        // Default case
        default:
            ret = KErrNotSupported;
            break;
        }

    if ( reqHandletype != EMultimodePacketContextReqHandleUnknown )
        {
        // Reset tsy request handle
        iTsyReqHandleStore->ResetTsyReqHandle( reqHandletype );

        // Complete request to client with KErrCancel
        ReqCompleted( aTsyReqHandle, KErrCancel );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted for logging purposes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aError )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_REQCOMPLETED_1, "TSY: CMmPacketTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", (TUint)aTsyReqHandle, aError, (TUint)this);
    iMmCustomTsy->ReqCompleted( aTsyReqHandle, aError );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_REQCOMPLETED_2, "<-- TSY: CMmPacketTsy::ReqCompleted");
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::NotifyEGprsInfoChange
// Request notification when EGPRS support changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketTsy::NotifyEGprsInfoChange(
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aGprsInformation )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_NOTIFYEGPRSINFOCHANGE_1,  "CustomTSY: CMmPacketTsy::NotifyEGprsInfoChange" );

    RMmCustomAPI::TGprsInformation temp;
	RMmCustomAPI::TGprsInformationPckg tempPckg(temp);

	TInt expectedLength = tempPckg.MaxLength();
	TInt paramLength = aGprsInformation->MaxLength();
	
	TInt ret = KErrArgument;
	if (expectedLength == paramLength)
		{
	    // Store pointer of Gprs information package
	    iGprsInformationPckg = 
	        ( RMmCustomAPI::TGprsInformationPckg* ) aGprsInformation;

	    // Store request handle
	    iTsyReqHandleStore->SetTsyReqHandle( 
	        EMultimodePacketContextNotifyEGprsInfoChange, aTsyReqHandle );
		ret = KErrNone;
		}
			
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::GetEGprsInfo
// Informs client about EGPRS information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketTsy::GetEGprsInfo(
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aGprsInformation )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_GETEGPRSINFO_1,  "CustomTSY: CMmPacketTsy::GetEGprsInfo" );

    RMmCustomAPI::TGprsInformation temp;
	RMmCustomAPI::TGprsInformationPckg tempPckg(temp);

	TInt expectedLength = tempPckg.MaxLength();
	TInt paramLength = aGprsInformation->MaxLength();
	
	TInt ret = KErrArgument;
	if (expectedLength == paramLength && iMmCustomTsy)
		{			
	    RMmCustomAPI::TGprsInformationPckg* gprsInformationPckg =
	        ( RMmCustomAPI::TGprsInformationPckg* ) aGprsInformation;

	    // Unpack gprsInformationPckg
	    RMmCustomAPI::TGprsInformation& gprsInformation = 
	        ( *gprsInformationPckg )();
	    TBool edgeGprsSupportInCell;
	 
	    if ( !iMmCustomTsy->Phone() 
	    	   || !iMmCustomTsy->Phone()->PacketDataSession() 
	    	   || !iMmCustomTsy->Phone()->PacketDataSession()->
	    		    ActivePacketServiceExtension() )
	        {
	        // returns false if EGPRS support failed
	       	edgeGprsSupportInCell = EFalse;
	        }
	    else
	        {
	        // returns information if EGPRS connection supported
	    	edgeGprsSupportInCell = iMmCustomTsy->Phone()->PacketDataSession()->
	            ActivePacketServiceExtension()->EGprsSupportInCell();
	        }

	    // if gprsSupportInCell is ETrue -> cell is supporting EGPRS connection 
	    if ( edgeGprsSupportInCell )
	        {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_GETEGPRSINFO_2,  "CustomTSY: CMmPacketTsy::GetEGprsInfo  RMmCustomAPI::EEdgeGprs" );
	        gprsInformation.iGprsInfo = RMmCustomAPI::EEdgeGprs; 
	        }

	    // else EGPRS connection is not supported
	    else 
	        {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_GETEGPRSINFO_3,  "CustomTSY: CMmPacketTsy::GetEGprsInfo  RMmCustomAPI::EGprs" );
	        gprsInformation.iGprsInfo = RMmCustomAPI::EGprs;
	        }

		ret = KErrNone;
		
	    // Complete the client request
	    ReqCompleted( aTsyReqHandle, ret );
		}
		
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::CompleteNotifyEGprsInfoChange
// Completes EGPRS notification request to Client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketTsy::CompleteNotifyEGprsInfoChange(
    const TBool aGprsSupportInCell )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_COMPLETENOTIFYEGPRSINFOCHANGE_1,  "CustomTSY: CMmPacketTsy::CompleteNotifyEGprsInfoChange" );
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketContextNotifyEGprsInfoChange );

    if ( EMultimodePacketContextReqHandleUnknown != reqHandle )
        {
        // Unpack iGprsInformationPckg
        RMmCustomAPI::TGprsInformation& gprsInformation = 
            ( *iGprsInformationPckg )();

        // if aGprsSupportInCell is ETrue -> cell is supporting edge GPRS
        if ( aGprsSupportInCell )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_COMPLETENOTIFYEGPRSINFOCHANGE_2,  "CustomTSY: CMmPacketTsy::CompleteNotifyEGprsInfoChange  RMmCustomAPI::EEdgeGprs" );
            gprsInformation.iGprsInfo = RMmCustomAPI::EEdgeGprs;
            }

        // if aGprsSupportInCell is EFalse -> cell is not supporting edge GPRS
        else 
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_COMPLETENOTIFYEGPRSINFOCHANGE_3,  "CustomTSY: CMmPacketTsy::CompleteNotifyEGprsInfoChange  RMmCustomAPI::EGprs" );
            gprsInformation.iGprsInfo = RMmCustomAPI::EGprs;
            }
                    
        iGprsInformationPckg = NULL;

        // Complete the client request
        ReqCompleted( reqHandle, KErrNone );
        }
    }
    
//-----------------------------------------------------------------------------
// CMmPacketTsy::SetAlwaysOnL
// Requests Set Always On with mode given as parameter aMode
// (other items were commented in a header).
//-----------------------------------------------------------------------------
//
TInt CMmPacketTsy::SetAlwaysOnL( 
	TTsyReqHandle aTsyReqHandle,
	RMmCustomAPI::TSetAlwaysOnMode* aMode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_SETALWAYSONL_1,  "CMmPacketTsy::SetAlwaysOn Reg Handle = %d", aTsyReqHandle );
	
	TInt ret( KErrGeneral );
	
											
	//Save requested mode 
	iAllwaysOnMode = ( *aMode );
	//Get packetdata session
	CMmPacketServiceTsy* packetDataSession = iMmPhone->PacketDataSession();
	
    if ( NULL != packetDataSession )
        {
        //request set allways on
        ret = packetDataSession->SetAlwaysOnL( aTsyReqHandle, iAllwaysOnMode );
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_SETALWAYSONL_2,  "CMmPacketTsy::SetAlwaysOn packetDataSession is NULL " );
        }	
		
    return ret;
    }

//-----------------------------------------------------------------------------
// CMmPacketTsy::CompleteSetAlwaysOn
// Completes Set Always On request 
// (other items were commented in a header).
//-----------------------------------------------------------------------------
//
void CMmPacketTsy::CompleteSetAlwaysOn( 
	TTsyReqHandle aReqHandle, 
	TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_COMPLETESETALWAYSON_1,  "CMmPacketTsy::CompleteSetAlwaysOn" );

	ReqCompleted( aReqHandle, aError );

    }

//-----------------------------------------------------------------------------
// CMmPacketTsy::CancelSetAlwaysOn
// Cancels Set Always On request
// (other items were commented in a header).
//-----------------------------------------------------------------------------
//
void CMmPacketTsy::CancelSetAlwaysOn( TTsyReqHandle aReqHandle )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETTSY_CANCELSETALWAYSON_1,  "CMmPacketTsy::CancelSetAlwaysOn" );
	
	//Direct request to Service Tsy
	iMmPhone->PacketDataSession()->CancelSetAlwaysOn( aReqHandle );
	
	}
	
#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmPacketTsy::SetTypeOfResponse
// Sets the type of response for a given handle.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketTsy::SetTypeOfResponse(
    const TInt /*aReqHandleType*/,
    const TTsyReqHandle /*aTsyReqHandle*/)
    {
    }

// ---------------------------------------------------------------------------
// CMmPacketTsy::Complete
// Completes the request due the timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketTsy::Complete(
    TInt /*aReqHandleType*/, 
    TInt /*aError*/ )
    {
    }
#endif // REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmPacketTsy::ResetReqHandle
// Resets the request handle in req handle store
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPacketTsy::ResetReqHandle(const TTsyReqHandle aTsyReqHandle, const TInt aIpc)
    {
    //handle has not been reset yet
    TInt ret = EFalse;
    if(aIpc == ECustomSetAlwaysOnMode)
        { 
		// request handle is set in packet data session
        CMmPacketServiceTsy* packetSession = iMmPhone->PacketDataSession();
        if(packetSession)
            {
            ret = packetSession->ResetReqHandle(aTsyReqHandle);
            }
        }
    else
        { 
		// request handle stored locally
        ret = iTsyReqHandleStore->FindAndResetTsyReqHandle(aTsyReqHandle);
        }
    return ret;
    }

//  End of File

