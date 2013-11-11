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



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CMmWimTsyTraces.h"
#endif

#include "CMmWimTsy.h"
#include <ctsy/rmmcustomapi.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmWimTsy::CMmWimTsy()
    {
    }

void CMmWimTsy::ConstructL(
    CMmCustomTsy* aMmCustomTsy )
    {
    iMmCustomTsy = aMmCustomTsy;

    // Create req handle store
#ifdef REQHANDLE_TIMER   
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( iMmCustomTsy,
        iMmCustomTsy->iMmPhoneTsy, EWIMRequestTypeMaxNumOfRequests, 
        iWIMReqHandles );
#else // REQHANDLE_TIMER
   iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        EWIMRequestTypeMaxNumOfRequests, iWIMReqHandles );

    return;
#endif // REQHANDLE_TIMER
    }

CMmWimTsy* CMmWimTsy::NewL(
    CMmCustomTsy* aMmCustomTsy )
    {
    CMmWimTsy* mmWIM = new ( ELeave ) CMmWimTsy();
    CleanupStack::PushL( mmWIM );
    mmWIM->ConstructL( aMmCustomTsy );
    CleanupStack::Pop( mmWIM );
    
    return mmWIM;
    }

CMmWimTsy::~CMmWimTsy()
    {
    if ( iTsyReqHandleStore )
        {
        delete iTsyReqHandleStore;
        }

    iMmCustomTsy = NULL;
    iRetApduInfo = NULL;
    iRetApduData = NULL;
    iAnswerToReset = NULL;
    iApduParameters = NULL;
    iSimCardReaderStatus = NULL;
    iNotifiedSimCardStatus = NULL;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::Init
// Initialisation method that is called from ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmWimTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::RegisterNotification
// Called when the server recognises that this notification is being posted 
// for the first time on this sub-session object. It enables the TSY to "turn 
// on" any regular notification messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::RegisterNotification(    
    const TInt aIpc )
    {
    TInt ret ( KErrNotSupported );

    switch ( aIpc )
        {
        case ECustomNotifySimCardStatusIPC:
            ret =  KErrNone;
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::SupportingIPC
// Tells whether the object supports given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmWimTsy::SupportingIPC(
    const TInt aIpc )
    { 
    TInt ret( EFalse );
    
    switch ( aIpc )
        {
        case ECustomSimWarmResetIPC:
        case ECustomGetATRIPC:
        case ECustomSendAPDUReqIPC:
        case ECustomGetSimCardReaderStatusIPC:
        case ECustomNotifySimCardStatusIPC:
        case ECustomSendAPDUReqV2IPC:
        case ECustomPowerSimOnIPC:
        case ECustomPowerSimOffIPC:
            ret = ETrue;
            break;
        default:
            ret = EFalse;
            break;
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::DeregisterNotification
// Called when the server recognises that this notification will not be posted 
// again because the last client to have a handle on this sub-session object 
// has just closed the handle. It enables the TSY to "turn off" any regular 
// notification messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::DeregisterNotification(    
    const TInt aIpc )
    {
    TInt ret ( KErrNotSupported );

    switch ( aIpc )
        {
        case ECustomNotifySimCardStatusIPC:
            ret =  KErrNone;
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::DoExtFuncL
// Dispatches extension function requests. At the beginning of this method,
// packaged pointers are unpacked. Then the correct function is selected
// using the IPC number.
// If the IPC is supported, returns the return value of corresponding function
// else returns KErrNotSupported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_DOEXTFUNCL_1, "TSY: CMmWimTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, aTsyReqHandle, (TUint)this);
    TInt ret( KErrNotSupported );

    switch ( aIpc )
        {
        case ECustomSendAPDUReqIPC:
            ret = SendAPDUReqL( aTsyReqHandle, aPackage.Des1n(), 
                aPackage.Des2n() );
            break;                    
        case ECustomGetATRIPC:
            ret = GetATRL( aTsyReqHandle, aPackage.Des1n() );
            break;
        case ECustomSimWarmResetIPC:
            ret = SimWarmResetL( aTsyReqHandle );
            break;
        case ECustomGetSimCardReaderStatusIPC:
            ret =  GetSimCardReaderStatusL( aTsyReqHandle, REINTERPRET_CAST( 
                RMmCustomAPI::TSimCardReaderStatus*, aPackage.Ptr1() ) );
            break;                            
        case ECustomNotifySimCardStatusIPC:
            ret =  NotifySimCardStatus( aTsyReqHandle, REINTERPRET_CAST( 
                RMmCustomAPI::TSIMCardStatus*, aPackage.Ptr1() ) );
            break;                        
        case ECustomSendAPDUReqV2IPC:
            ret = SendAPDUReqV2L( aTsyReqHandle, REINTERPRET_CAST( 
                RMmCustomAPI::TApduParameters*, aPackage.Ptr1() ) );
            break;
        case ECustomPowerSimOnIPC:
            ret = PowerSimOnL( aTsyReqHandle );
            break;
        case ECustomPowerSimOffIPC:
            ret = PowerSimOffL( aTsyReqHandle );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }
        
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_DOEXTFUNCL_2, "<-- TSY: CMmWimTsy::DoExtFuncL, error = %{TSymbianErrorCodes}", ret);
    return ret;        
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::ReqModeL
// Returns request mode for given IPC. Leaves with error code KErrNotSupported
// if the IPC number does not belong to some of the WIM-related API functions. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmWimTsy::ReqModeL(
    const TInt aIpc )
    {
    CTelObject::TReqMode ret=0;

    switch ( aIpc )
        {
        case ECustomSendAPDUReqIPC: 
            // Custom TSY has its own flow control and allows several 
            // simultaneous requests.
            ret = KReqModeMultipleCompletionEnabled;
            break;
        case ECustomNotifySimCardStatusIPC:
            ret = KReqModeRePostImmediately | 
                  KReqModeMultipleCompletionEnabled;
            break;
        default:
            break;    
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC. Leaves with error code 
// KErrNotSupported if the IPC number does not belong to some of the 
// WIM-related API functions. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    TInt numberOfSlots = KMmCustomDefaultSlots;

    switch ( aIpc )
        {
        case ECustomSendAPDUReqIPC:
            break;    
        case ECustomNotifySimCardStatusIPC:
            break;
        default:
            // Unknown or invalid IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::CancelService
// When the clients close their sub-sessions (eg. by calling RPhone::Close),
// they may not have cancelled all their outstanding asynchronous requests
// before closing. It is up to the ETel server to clean up in this situation,
// so the server will find the list of outstanding requests related to that
// sub-session object and pass these outstanding IPC request numbers, one at
// a time, to the CancelService function in the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::CancelService(
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt ret( KErrGeneral );
    
    switch ( aIpc )
        {
        case ECustomSendAPDUReqIPC:
            ret = SendAPDUReqCancel( aTsyReqHandle );
            break;
        case ECustomGetATRIPC:
            ret = GetATRCancel();
            break;
        case ECustomSimWarmResetIPC:
            ret = SimWarmResetCancel();
            break;
        case ECustomGetSimCardReaderStatusIPC:
            ret = GetSimCardReaderStatusCancel();
            break;
        case ECustomNotifySimCardStatusIPC:
            ret = NotifySimCardStatusCancel();
            break;
        case ECustomSendAPDUReqV2IPC:
            ret = SendAPDUReqV2Cancel();
            break;
        case ECustomPowerSimOnIPC:
            ret = PowerSimOnCancel();
            break;
        case ECustomPowerSimOffIPC:
            ret = PowerSimOffCancel();
            break;
        default:
            ret = KErrGeneral;
            break; 
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::SendAPDUReqL
// Send APDU request to DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::SendAPDUReqL( 
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aInfo, 
    TDes8* aData )
    {
    TInt ret ( KErrNone );
  
    // This is originally a TApdu in the API
    RMmCustomAPI::TApdu apduReq;
    apduReq.iInfo = aInfo;
    apduReq.iData = aData;

    // call DOS, packed parameter: a RMmCustomAPI::TApdu
    CMmDataPackage dataPackage;
    dataPackage.PackData( &apduReq );
    ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
        ECustomSendAPDUReqIPC, &dataPackage );

    if ( KErrNone == ret )
        {
        // save pointer to client for response data
        iRetApduInfo = aInfo;
        iRetApduData = aData;

#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( EWIMRequestTypeSendAPDUReq, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle(
            EWIMRequestTypeSendAPDUReq, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        }	

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::CompleteSendAPDUReq
// Completes a Send APDU request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
#ifndef USING_CTSY_DISPATCHER
void CMmWimTsy::CompleteSendAPDUReq(
    CMmDataPackage* aDataPackage, 
    TInt aError )
    {
	// Complete request
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EWIMRequestTypeSendAPDUReq );
    
	if( reqHandle )
		{
    if ( KErrNone == aError )
        {
        //unpack the data
        TPtrC8* info = NULL;
        TPtrC8* data = NULL;

        aDataPackage->UnPackData( &info, &data );
    // request was successful, copy data to client side
    if ( iRetApduInfo && info )
        {
        iRetApduInfo->Copy( info->Ptr(), info->Length() );
        }

    if ( iRetApduData && data )
        {
        iRetApduData->Copy( data->Ptr(), data->Length() );
        }
        }
       iMmCustomTsy->ReqCompleted( reqHandle, aError );
       }
    }
#endif //USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmWimTsy::SendAPDUReqCancel
// Cancel outstanding Send APDU request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::SendAPDUReqCancel( 
    TTsyReqHandle aTsyReqHandle )
    {
    // reset the reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle(
        EWIMRequestTypeSendAPDUReq );

    // reset return pointers
    iRetApduInfo = NULL;
    iRetApduData = NULL;

    // complete
    iMmCustomTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmWimTsy::SetTypeOfResponse
// Sets the type of response for a given handle. Automatic mode includes an 
// automatic response in case of non response from the DOS in a specified time.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmWimTsy::SetTypeOfResponse(
    const TInt aReqHandleType, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );
    TInt ipc( 0 );

    switch ( aReqHandleType )
        {
        case EWIMRequestTypeSendAPDUReq:
            timeOut = KMmSendAPDUTimeOut;
            ipc = ECustomSendAPDUReqIPC;
            break;
        case EWIMRequestTypeSimWarmReset:
            timeOut = KMmSimWarmResetTimeOut;
            ipc = ECustomSimWarmResetIPC;
            break;
        case EWIMRequestTypeGetATR:
            timeOut = KMmGetATRTimeOut;
            ipc = ECustomGetATRIPC;
            break;
        case EWIMRequestTypeGetSimCardReaderStatus:
            timeOut = KMmGetSimCardReaderStatusTimeOut;
            ipc = ECustomGetSimCardReaderStatusIPC;
            break;
        case EWIMRequestTypeSendAPDUReqV2:
            timeOut = KMmSendAPDUV2TimeOut;            
            ipc = ECustomSendAPDUReqV2IPC;
            break;
        case EWIMRequestTypePowerSimOn:
            timeOut = KMmPowerSimOnTimeOut;
            ipc = ECustomPowerSimOnIPC;
            break;
        case EWIMRequestTypePowerSimOff:  
            timeOut = KMmPowerSimOffTimeOut;
            ipc = ECustomPowerSimOffIPC;
            break;
        default:
            // Does not use timer
            break;
        }

    if ( 0 < timeOut )
        {
        // The timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType, aTsyReqHandle,
            timeOut, ipc );
        }
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::Complete
// Completes the request due the timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmWimTsy::Complete(
    TInt aReqHandleType, 
    TInt aError )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_COMPLETE_1,  "CustomTSY: CMmWimTsy::Complete.\n\t ReqHandleType:%d \n\t Error:%{TSymbianErrorCodes}\n", aReqHandleType, aError );
    iMmCustomTsy->ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle( 
        aReqHandleType ), aError );
    }
#endif // REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmWimTsy::GetATRL
// Send GetATR request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//  
TInt CMmWimTsy::GetATRL(
    const TTsyReqHandle aTsyReqHandle, 
    TDes8 *aATR )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_GETATRL_1,  "TSY: CMmWimTsy::GetATRL" );
    
    TTsyReqHandle getAtrHandle = iTsyReqHandleStore->
        GetTsyReqHandle( EWIMRequestTypeGetATR );
        
    if ( 0 < getAtrHandle )
        {         
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_GETATRL_2,  "TSY: CMmWimTsy::GetATRL - KErrServerBusy" );
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {        
        // save pointer to client for response data
        iAnswerToReset = aATR;
    
        // Call DOS, packed parameter: aATR
        CMmDataPackage dataPackage;
        dataPackage.PackData( &aATR );
        TInt ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
            ECustomGetATRIPC, &dataPackage );

        // check the return value of the call to DOS
        if ( KErrNone != ret )
            {
            iMmCustomTsy->ReqCompleted( aTsyReqHandle, ret );
            }     
        else
            {
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EWIMRequestTypeGetATR, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                EWIMRequestTypeGetATR, aTsyReqHandle );
#endif
            }
        }            
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::CompleteGetATR
// Completes a GetATR request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//      
void CMmWimTsy::CompleteGetATR(
    TPtrC8& aData, 
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_COMPLETEGETATR_1,  "TSY: CMmWimTsy::CompleteGetATR ");
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle = 
        iTsyReqHandleStore->ResetTsyReqHandle( EWIMRequestTypeGetATR );

    // Check the reqHandle
    if ( EWIMTsyReqHandleUnknown != tsyReqHandle )
        {
        // request was successful, copy data to client side
        if ( KErrNone == aError )
            {
            //check the size....
            if ( (*iAnswerToReset).MaxLength() >= aData.Length() )
                {
                (*iAnswerToReset).Copy( aData );
                }
            else // data is too big for client's buffer
                {
                aError = KErrOverflow;
                }
            }
        iAnswerToReset = NULL;
        iMmCustomTsy->ReqCompleted( tsyReqHandle, aError );
        }  
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::GetATRCancel
// Cancels an outstanding GetATR request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::GetATRCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_GETATRCANCEL_1,  "CMmWimTsy::GetATRCancel" );
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EWIMRequestTypeGetATR );
    
    // reset pointer to client data
    iAnswerToReset = NULL; 

    if( EWIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::SimWarmResetL
// Send SimWarmReset request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
TInt CMmWimTsy::SimWarmResetL(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_SIMWARMRESETL_1,  "CMmWimTsy::SimWarmResetL");
    TTsyReqHandle simWarmResetHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EWIMRequestTypeSimWarmReset );
        
    if ( 0 < simWarmResetHandle )
        {         
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_SIMWARMRESETL_2,  "TSY: CMmWimTsy::SimWarmResetL - KErrServerBusy" );
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        TInt ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
            ECustomSimWarmResetIPC );

        // check the return value of the call to DOS
        if ( KErrNone != ret )
            {
            iMmCustomTsy->ReqCompleted( aTsyReqHandle, ret );
            }     
        else
            {
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EWIMRequestTypeSimWarmReset, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                EWIMRequestTypeSimWarmReset, aTsyReqHandle );
#endif
            }
        }
                    
    return KErrNone;
    }    


// ---------------------------------------------------------------------------
// CMmWimTsy::CompleteSimWarmReset
// Completes a SimWarmReset request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
void CMmWimTsy::CompleteSimWarmReset(
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_COMPLETESIMWARMRESET_1,  "CMmWimTsy::CompleteSimWarmReset" );
       
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle = 
        iTsyReqHandleStore->ResetTsyReqHandle( EWIMRequestTypeSimWarmReset );

    if ( EWIMTsyReqHandleUnknown != tsyReqHandle )
        {
        // Complete request
        iMmCustomTsy->ReqCompleted( tsyReqHandle, aError );
        }  
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::SimWarmResetCancel
// Cancels an outstanding SimWarmReset request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::SimWarmResetCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_SIMWARMRESETCANCEL_1,  "TSY: CMmWimTsy::SimWarmResetCancel" );
    // reset the reqhandle
    TTsyReqHandle reqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( EWIMRequestTypeSimWarmReset );

    if( EWIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::GetSimCardReaderStatusL
// Send GetSimCardReaderStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
TInt CMmWimTsy::GetSimCardReaderStatusL(
    const TTsyReqHandle aTsyReqHandle, 
    RMmCustomAPI::TSimCardReaderStatus* aSimCardReaderStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_GETSIMCARDREADERSTATUSL_1,  "TSY: CMmWimTsy::GetSimCardReaderStatusL" );
    TTsyReqHandle getSimCardReaderStatusHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( 
        EWIMRequestTypeGetSimCardReaderStatus );
        
    if ( 0 < getSimCardReaderStatusHandle )
        {         
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_GETSIMCARDREADERSTATUSL_2,  "TSY: CMmWimTsy::GetSimCardReaderStatusL - KErrServerBusy" );
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {    
        // call DOS, packed parameter: aSimCardReaderStatus
        CMmDataPackage dataPackage;
        dataPackage.PackData( &aSimCardReaderStatus );
        TInt ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
            ECustomGetSimCardReaderStatusIPC, &dataPackage );
       
        // check the return value of the call to DOS
        if ( KErrNone != ret )
            {
            iMmCustomTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // save pointer to client for response data
            iSimCardReaderStatus = aSimCardReaderStatus;
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EWIMRequestTypeGetSimCardReaderStatus, 
                aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                EWIMRequestTypeGetSimCardReaderStatus, aTsyReqHandle );
#endif
            }
        }
        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::CompleteGetSimCardReaderStatus
// Completes a GetSimCardReaderStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
void CMmWimTsy::CompleteGetSimCardReaderStatus(
    TPtrC8& aData, 
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_COMPLETEGETSIMCARDREADERSTATUS_1,  "CMmWimTsy::CompleteGetSimCardReaderStatus" );
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle = 
        iTsyReqHandleStore->ResetTsyReqHandle( 
        EWIMRequestTypeGetSimCardReaderStatus );
    
    // check the handle and complete
    if ( EWIMTsyReqHandleUnknown != tsyReqHandle )
        {
        if(KErrNone == aError)
		   {
		   // Copy the data 
           if ( 0 < aData.Length() )
               {
               iSimCardReaderStatus->SetStatus( aData[0] );
               }
           else
               {
               // There was no data, set status to zero
               iSimCardReaderStatus->SetStatus( 0 );
               }
		   }
		iSimCardReaderStatus = NULL;
        iMmCustomTsy->ReqCompleted( tsyReqHandle, aError ); 
        }
    }  

// ---------------------------------------------------------------------------
// CMmWimTsy::GetSimCardReaderStatusCancel
// Cancels an outstanding GetSimCardReaderStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::GetSimCardReaderStatusCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_GETSIMCARDREADERSTATUSCANCEL_1,  "TSY: CMmWimTsy::GetSimCardReaderStatusCancel" );
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EWIMRequestTypeGetSimCardReaderStatus );
    
    // reset pointer to client data
    iSimCardReaderStatus = NULL; 

    if( EWIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::NotifySimCardStatus
// Notifies a client of physical status change of SIM.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
// 
TInt CMmWimTsy::NotifySimCardStatus(
    const TTsyReqHandle aTsyReqHandle, 
    RMmCustomAPI::TSIMCardStatus* aCardStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_NOTIFYSIMCARDSTATUS_1,  "TSY: CMmWimTsy::NotifySimCardStatus" );
    if ( NULL != aCardStatus )
        {
        // save pointer to client for response data
        iNotifiedSimCardStatus = aCardStatus;
        
        iTsyReqHandleStore->SetTsyReqHandle( 
            EWIMRequestTypeNotifySimCardStatus, aTsyReqHandle );
        
        return KErrNone;
        }
        
    return KErrArgument;
    }

// ---------------------------------------------------------------------------
// CMmWimTsy::CompleteNotifySimCardStatus
// Completes a NotifySimCardStatus method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
// 
void CMmWimTsy::CompleteNotifySimCardStatus(
    RMmCustomAPI::TSIMCardStatus aCardStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_COMPLETENOTIFYSIMCARDSTATUS_1,  "CMmWimTsy::CompleteNotifySimCardStatus" );
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EWIMRequestTypeNotifySimCardStatus );

    // check the handle and complete
    if ( EWIMTsyReqHandleUnknown != reqHandle )
        {
        // Copy the data
        if ( NULL != iNotifiedSimCardStatus )
            {
            *iNotifiedSimCardStatus = aCardStatus;
            iNotifiedSimCardStatus = NULL;
            }
        iMmCustomTsy->ReqCompleted( reqHandle, KErrNone );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmWimTsy::NotifySimCardStatusCancel
// Cancels an outstanding NotifySimCardStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::NotifySimCardStatusCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_NOTIFYSIMCARDSTATUSCANCEL_1,  "TSY: CMmWimTsy::NotifySimCardStatusCancel" );
    // reset pointer to client data
    iNotifiedSimCardStatus = NULL;

    // reset reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EWIMRequestTypeNotifySimCardStatus );

    if( EWIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with KErrCancel
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}
    
    return KErrNone;
    }
     
// ---------------------------------------------------------------------------
// CMmWimTsy::SendAPDUReqV2L
// Send APDU request to APDU server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//  
TInt CMmWimTsy::SendAPDUReqV2L( 
    const TTsyReqHandle aTsyReqHandle, 
    RMmCustomAPI::TApduParameters* aApduParameters )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_SENDAPDUREQV2L_1,  "TSY: CMmWimTsy::SendAPDUReqV2L" );
    TTsyReqHandle sendApduReqV2Handle = 
        iTsyReqHandleStore->GetTsyReqHandle( EWIMRequestTypeSendAPDUReqV2 );
    TInt ret = KErrNone; 
        
    if ( 0 < sendApduReqV2Handle )
        {         
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_SENDAPDUREQV2L_2,  "TSY: CMmWimTsy::SendAPDUReqV2L - KErrServerBusy" );
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        return ret; 
        }
    else
        { 
        // call DOS, packed parameter: aData
        CMmDataPackage dataPackage;
        dataPackage.PackData( &aApduParameters );
        ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
            ECustomSendAPDUReqV2IPC, &dataPackage );
        // check result
        if ( KErrNone == ret )
            {
            // save pointer to client for response data
            iApduParameters = aApduParameters;
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EWIMRequestTypeSendAPDUReqV2, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                EWIMRequestTypeSendAPDUReqV2, aTsyReqHandle );
#endif
            }
        }
        
    return ret;
    }       
    
// ---------------------------------------------------------------------------
// CMmWimTsy::CompleteSendAPDUReqV2
// Completes a Send APDU request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//      
void CMmWimTsy::CompleteSendAPDUReqV2(
    TPtrC8 &aData, 
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_COMPLETESENDAPDUREQV2_1,  "TSY: CMmWimTsy::CompleteSendAPDUReqV2" );
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle = 
        iTsyReqHandleStore->ResetTsyReqHandle( EWIMRequestTypeSendAPDUReqV2 );
        
    // check the handle and complete
    if ( EWIMTsyReqHandleUnknown != tsyReqHandle )
        {
        // request was successful, copy data to client side 
        if ( KErrNone == aError )
            {
            // check the size....
            if ( iApduParameters->iRspData.MaxLength() >= aData.Length())
                {
                iApduParameters->iRspData.Copy( aData);
                }
            else    // data is too big for client's buffer
                {
                aError = KErrOverflow;
                }
            }
        iApduParameters = NULL;
        iMmCustomTsy->ReqCompleted( tsyReqHandle, aError ); 
        }
    }    
 
// ---------------------------------------------------------------------------
// CMmWimTsy::SendAPDUReqV2Cancel
// Cancels an outstanding Send APDU request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::SendAPDUReqV2Cancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_SENDAPDUREQV2CANCEL_1,  "TSY: CMmWimTsy::SendAPDUReqV2Cancel" );
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EWIMRequestTypeSendAPDUReqV2 );
    
    // reset return pointer
    iApduParameters = NULL;
    
    if( EWIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::PowerSimOnL
// Send PowerSimOn request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
TInt CMmWimTsy::PowerSimOnL(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_POWERSIMONL_1,  "TSY: CMmWimTsy::PowerSimOnL" );
    TTsyReqHandle powerSimOnHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EWIMRequestTypePowerSimOn );
        
    if ( 0 < powerSimOnHandle )
        {         
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_POWERSIMONL_2,  "TSY: CMmWimTsy::PowerSimOnL - KErrServerBusy" );
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        { 
        TInt ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
            ECustomPowerSimOnIPC );

        // check the return value of the call to DOS
        if ( KErrNone != ret )
            {
            iMmCustomTsy->ReqCompleted( aTsyReqHandle, ret );
            }     
        else
            {
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EWIMRequestTypePowerSimOn, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                EWIMRequestTypePowerSimOn, aTsyReqHandle );
#endif
            }
        }  
               
    return KErrNone;
    }
     
// ---------------------------------------------------------------------------
// CMmWimTsy::CompletePowerSimOn
// Completes a PowerSimOn request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
void CMmWimTsy::CompletePowerSimOn(
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_COMPLETEPOWERSIMON_1,  "TSY: CMmWimTsy::CompletePowerSimOn" );
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle = 
        iTsyReqHandleStore->ResetTsyReqHandle( EWIMRequestTypePowerSimOn );

    if ( EWIMTsyReqHandleUnknown != tsyReqHandle )
        {
        // Complete request
        iMmCustomTsy->ReqCompleted( tsyReqHandle, aError );
        }
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::PowerSimOnCancel
// Cancels an outstanding PowerSimOn request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::PowerSimOnCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_POWERSIMONCANCEL_1,  "TSY: CMmWimTsy::PowerSimOnCancel" );
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EWIMRequestTypePowerSimOn );

    if( EWIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::PowerSimOffL
// Send PowerSimOff request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
TInt CMmWimTsy::PowerSimOffL(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_POWERSIMOFFL_1,  "TSY: CMmWimTsy::PowerSimOffL" );
    TTsyReqHandle powerSimOffHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( EWIMRequestTypePowerSimOff );
        
    if ( 0 < powerSimOffHandle )
        {         
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about 
        // the situation.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_POWERSIMOFFL_2,  "TSY: CMmWimTsy::PowerSimOffL - KErrServerBusy" );
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        TInt ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
            ECustomPowerSimOffIPC );

        // check the return value of the call to DOS
        if ( KErrNone != ret )
            {
            iMmCustomTsy->ReqCompleted( aTsyReqHandle, ret );
            }     
        else
            {
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EWIMRequestTypePowerSimOff, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                EWIMRequestTypePowerSimOff, aTsyReqHandle );
#endif        
            }
        }
        
    return KErrNone;
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::CompletePowerSimOff
// Completes a PowerSimOff request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
void CMmWimTsy::CompletePowerSimOff(
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_COMPLETEPOWERSIMOFF_1,  "TSY: CMmWimTsy::CompletePowerSimOff" );
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle = 
        iTsyReqHandleStore->ResetTsyReqHandle( EWIMRequestTypePowerSimOff );

    if ( EWIMTsyReqHandleUnknown != tsyReqHandle )
        {
        // Complete request
        iMmCustomTsy->ReqCompleted( tsyReqHandle, aError );
        }  
    }    

// ---------------------------------------------------------------------------
// CMmWimTsy::PowerSimOffCancel
// Cancels an outstanding PowerSimOff request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmWimTsy::PowerSimOffCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMWIMTSY_POWERSIMOFFCANCEL_1,  "TSY: CMmWimTsy::PowerSimOffCancel" );
    // reset the reqhandle
    TTsyReqHandle reqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( EWIMRequestTypePowerSimOff );

    if( EWIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }    

//  End of File

