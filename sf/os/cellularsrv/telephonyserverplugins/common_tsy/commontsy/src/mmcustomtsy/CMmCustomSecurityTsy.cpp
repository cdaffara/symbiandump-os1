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
#include "CMmCustomSecurityTsyTraces.h"
#endif

#include "CMmCustomSecurityTsy.h"
#include "cmmphonegsmwcdmaext.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmCustomSecurityTsy::CMmCustomSecurityTsy()
    {
    }

void CMmCustomSecurityTsy::ConstructL(
    CMmCustomTsy* aMmCustomTsy, 
    CMmPhoneTsy* aMmPhoneTsy )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_CONSTRUCTL_1, "TSY: CMmCustomSecurityTsy::ConstructL");
    iMmCustomTsy = aMmCustomTsy;
    iMmPhoneTsy = aMmPhoneTsy;

    // register securitytsy in the message manager
    iMmPhoneTsy->MessageManager()->RegisterTsyObject( 
        CMmMessageManagerBase::ESecurityTsy, this );
    
    // Create req handle store
#ifdef REQHANDLE_TIMER   
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( iMmCustomTsy,
        iMmCustomTsy->iMmPhoneTsy, ESecurityRequestTypeMaxNumOfRequests, 
        iSecReqHandles );
#else
   iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        ESecurityRequestTypeMaxNumOfRequests, iSecReqHandles );
#endif //REQHANDLE_TIMER
    
    return;
    }

CMmCustomSecurityTsy* CMmCustomSecurityTsy::NewL(
    CMmCustomTsy* aMmCustomTsy, 
    CMmPhoneTsy* aPhoneTsy )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_NEWL_1, "TSY: CMmCustomSecurityTsy::NewL");
    CMmCustomSecurityTsy* self = new (ELeave) CMmCustomSecurityTsy();

    CleanupStack::PushL( self );
    self->ConstructL( aMmCustomTsy, aPhoneTsy );
    CleanupStack::Pop();

    return self;
    }

CMmCustomSecurityTsy::~CMmCustomSecurityTsy()
    {
    if (iMmPhoneTsy )
        {
        // deregister tsy object from message manager
        iMmPhoneTsy->MessageManager()->DeregisterTsyObject(this);        
        }
    delete iTsyReqHandleStore;

    return;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::Init
// Initialisation method that is called from ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomSecurityTsy::Init()
    {
    return;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::SupportingIPC
// Tells whether the object supports given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCustomSecurityTsy::SupportingIPC(
    TInt aIpc )
    {
    TInt ret( EFalse );

    switch (aIpc)
        {
        case ECustomIsBlockedIPC:
        case ECustomCheckSecurityCodeIPC:
        case ECustomDisablePhoneLockIPC:
        case ECustomGetActivePinIPC:
            ret = ETrue;
            break;
        default:
            ret = EFalse;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::DoExtFuncL
// Dispatches extension function requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_DOEXTFUNCL_1, "TSY: CMmCustomSecurityTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, aTsyReqHandle, (TUint)this);
    TInt ret( KErrGeneral );

    // reset last tsy request type
    iReqHandleType = ESecurityTsyReqHandleUnknown;

    switch (aIpc)
        {
        case ECustomIsBlockedIPC:
            ret = IsBlocked( aTsyReqHandle,
                REINTERPRET_CAST( RMmCustomAPI::TSecurityCodeType*,
                aPackage.Ptr1() ),
                REINTERPRET_CAST( TBool*, aPackage.Ptr2() ) );
            break;
        case ECustomCheckSecurityCodeIPC:
            ret = CheckSecurityCodeL( aTsyReqHandle,
                REINTERPRET_CAST( RMmCustomAPI::TSecurityCodeType*,
                aPackage.Ptr1() ) );
            break;
        case ECustomDisablePhoneLockIPC:
            ret = DisablePhoneLockL( aTsyReqHandle,
                REINTERPRET_CAST( RMobilePhone::TMobilePassword*,
                aPackage.Ptr1() ) );
            break;
         case ECustomGetActivePinIPC:
            ret = GetActivePin( aTsyReqHandle,
                REINTERPRET_CAST( RMobilePhone::TMobilePhoneSecurityCode*, 
                aPackage.Ptr1() ) );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

    // save request handle
    if( ESecurityTsyReqHandleUnknown != iReqHandleType )
        {
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_DOEXTFUNCL_2, "<-- TSY: CMmCustomSecurityTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::ReqModeL
// Returns request mode for given IPC. If the IPC number belongs to some of
// the Security API functions.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmCustomSecurityTsy::ReqModeL( 
    const TInt aIpc )
    {
    CTelObject::TReqMode ret = 0;

    switch (aIpc)
        {
        case ECustomCheckSecurityCodeIPC:
        case ECustomIsBlockedIPC:
        case ECustomDisablePhoneLockIPC:
        case ECustomGetActivePinIPC:
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    TInt numberOfSlots( KMmCustomDefaultSlots );

    switch ( aIpc )
        {
        case ECustomIsBlockedIPC:
           numberOfSlots = KMmCustomIsBlockedSlots;
           break;
        case ECustomCheckSecurityCodeIPC:
        case ECustomDisablePhoneLockIPC:
           numberOfSlots = KMmCustomCheckSecurityCodeSlots;
           break;
        case ECustomGetActivePinIPC:
           numberOfSlots = KMmCustomDefaultSlots;
           break;
        default:
            // Unknown or invalid IPC
            User::Leave( KErrNotSupported );
            break;
        }
        
    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::CancelService
// When the clients close their sub-sessions (eg. by calling RPhone::Close),
// they may not have cancelled all their outstanding asynchronous requests
// before closing. It is up to the ETel server to clean up in this situation,
// so the server will find the list of outstanding requests related to that
// sub-session object and pass these outstanding IPC request numbers, one at
// a time, to the CancelService function in the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::CancelService(
    const TInt aIpc, 
    const TTsyReqHandle )
    {
    TInt ret( KErrNone );
    TInt trapError( KErrNone );

    TRAP( trapError, // START TRAP
        switch ( aIpc )
            {
            case ECustomCheckSecurityCodeIPC:
                ret = CheckSecurityCodeCancelL();
                break;
            case ECustomDisablePhoneLockIPC: //This IPC cannot be cancelled 
                ret = KErrNone;
                break;
            default:
                ret = KErrGeneral;
                break;
            } 
        ); // END TRAP
    if ( KErrNone != trapError )
        {
        ret = trapError;
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::RegisterNotification
// RegisterNotification is called when the server recognises
// that this notification is being posted for the first time on this sub-
// session object. It enables the TSY to "turn on" any regular notification
// messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::RegisterNotification(    
    const TInt )
    {
    // No supported notifications in CMmCustomSecurityTsy
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises that this
// notification will not be posted again because the last client to have
// a handle on this sub-session object has just closed the handle. It
// enables the TSY to "turn off" any regular notification messages that
// it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::DeregisterNotification(    
    const TInt )
    {
    // No supported notifications in CMmCustomSecurityTsy
    return KErrNotSupported;
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::SetTypeOfResponse
// Sets the type of response for a given handle.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomSecurityTsy::SetTypeOfResponse(
    const TInt aReqHandleType, 
    const TTsyReqHandle aTsyReqHandle )
    {
    // Does not use timer
    iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType, aTsyReqHandle );
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::Complete
// Completes the request due the timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomSecurityTsy::Complete(
    TInt aReqHandleType, 
    TInt aError )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_COMPLETE_1,  "CustomTSY: CMmCustomSecurityTsy::Complete.\n\t ReqHandleType:%d \n\t Error:%{TSymbianErrorCodes}\n", aReqHandleType, aError );
    // All possible TSY req handle types are listed in the
    // switch case below.
    switch ( aReqHandleType )
        {
        case ESecurityRequestTypeCheckSecurityCode:
            CompleteCheckSecurityCode( aError );
            break;
        case ESecurityRequestTypeIsBlocked:
            CompleteIsBlocked( aError, *iIsBlocked );
            break;
        case ESecurityRequestTypeDisablePhoneLock:
            CompleteDisablePhoneLock( aError );
            break;
        default:
            iMmCustomTsy->ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle( 
                aReqHandleType ), aError );
            break;
        }
    }
#endif // REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::IsBlocked
// This method gets the is blocked status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::IsBlocked( 
    const TTsyReqHandle aTsyReqHandle, 
    RMmCustomAPI::TSecurityCodeType* aCode,
    TBool* aIsBlocked )
    {
    if ( NULL != iTsyReqHandleStore->GetTsyReqHandle(
        ESecurityRequestTypeIsBlocked ) )
        {
        return KErrServerBusy;
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_ISBLOCKED_1,  "TSY: CMmPhoneTsy::IsBlocked - Code to check: %d", *aCode );

    switch ( *aCode )
        {
        case RMmCustomAPI::ESecurityCodePin1:
            iCodeID = KSecCodePin;
            break;
        case RMmCustomAPI::ESecurityCodePin2:
            iCodeID = KSecCodePin2;
            break;
        case RMmCustomAPI::ESecurityUniversalPin:
            if ( CMmPhoneGsmWcdmaExt::EICCTypeSim3G == 
				iMmPhoneTsy->GetActivePhoneExtension()->CurrentICCType() )
                {
                iCodeID = KSecCodeUpin;
                }
            else
                {
                return KErrArgument;
                }
            break;
        case RMmCustomAPI::ESecurityCodePassPhrase:
            iCodeID = KSecCodeSecurity;
            break;
        default:
            return KErrArgument;
        }

    // save pointer to client data
    iIsBlocked = aIsBlocked;

    // Send ISI message to SIM Server to check security code  and
    // intialize ret value

    CMmDataPackage aPackage;
    aPackage.PackData( &iCodeID );

    TInt retValue = KErrNone;
	TRAPD(err, retValue = iMmPhoneTsy->MessageManager()->HandleRequestL
        ( ECustomIsBlockedIPC, &aPackage ) );
	if(err != KErrNone)
		{
		retValue = err;
		}

    // check result
    if ( KErrNone != retValue )
        {
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, retValue );
        }
    else
        {
        // Save the req handle type 
        iReqHandleType = ESecurityRequestTypeIsBlocked;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::CompleteIsBlocked
// Completes a isblocked request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomSecurityTsy::CompleteIsBlocked(
     TInt aErrorCode, 
     TBool aIsBlocked )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        ESecurityRequestTypeIsBlocked );

    // check the handle and complete
    if( ESecurityTsyReqHandleUnknown != reqHandle )
        {
        //set the information for the client
        *iIsBlocked = aIsBlocked;
        
        //reset pointer to client data
        iIsBlocked = NULL;
        
        iMmCustomTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::CheckSecurityCodeL
// This method sends the check security code request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::CheckSecurityCodeL(
    const TTsyReqHandle aTsyReqHandle, 
    RMmCustomAPI::TSecurityCodeType* aCode )
    {
    if ( NULL != iTsyReqHandleStore->GetTsyReqHandle(
        ESecurityRequestTypeCheckSecurityCode ) )
        {
        return KErrServerBusy;
        }

    switch ( *aCode )
        {
        case RMmCustomAPI::ESecurityCodePin1:
            iCodeID = KSecCodePin;
            break;
        case RMmCustomAPI::ESecurityCodePin2:
            iCodeID = KSecCodePin2;
            break;
        case RMmCustomAPI::ESecurityCodePuk1:
            iCodeID = KSecCodePuk;
            break;
        case RMmCustomAPI::ESecurityCodePuk2:
            iCodeID = KSecCodePuk2;
            break;
        case RMmCustomAPI::ESecurityCodePassPhrase:
            iCodeID = KSecCodeSecurity;
            break;
        default:
            return KErrArgument;
        }

    CMmDataPackage aPackage;
    aPackage.PackData( &iCodeID, &iSecCode );

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_CHECKSECURITYCODEL_1,  "TSY: CMmCustomSecurityTsy::CheckSecurityCodeL iSecCode: %S", iSecCode );
    TInt retValue ( iMmPhoneTsy->MessageManager()->HandleRequestL( 
        ECustomCheckSecurityCodeIPC, &aPackage ) );

    // check result
    if ( KErrNone != retValue )
        {
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, retValue );
        }
    else
        {
        // Save the req handle type 
        iReqHandleType = ESecurityRequestTypeCheckSecurityCode;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::CompleteCheckSecurityCode
// Completes a CheckSecurityCode request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomSecurityTsy::CompleteCheckSecurityCode(
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_COMPLETECHECKSECURITYCODE_1, "TSY: CMmCustomSecurityTsy::CompleteCheckSecurityCode - Error code: %{TSymbianErrorCodes}", aErrorCode );
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        ESecurityRequestTypeCheckSecurityCode );

    if ( ESecurityTsyReqHandleUnknown != reqHandle )
        {
        iMmCustomTsy->ReqCompleted( reqHandle, aErrorCode );
        }

    // If error is not KErrNone, then security code verify has failed and
    // we need to cancel ECustomCheckSecurityCodeCancelIPC.
    // If error is KErrCancel, we have already cancelled 
    // ECustomCheckSecurityCodeCancelIPC.
    if ( ( KErrNone != aErrorCode ) && ( KErrCancel != aErrorCode ) )
        {
        TBool cancelFalse (EFalse);
        CMmDataPackage aPackage;
        aPackage.PackData( &iCodeID, &cancelFalse );

        TRAP_IGNORE( void( iMmPhoneTsy->MessageManager()->HandleRequestL(
              ECustomCheckSecurityCodeCancelIPC, &aPackage ) ); );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::CheckSecurityCodeCancelL
// This function cancels an check security code request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::CheckSecurityCodeCancelL()
    {
    // Send message to Security Server to check security code  and
    // intialize ret value. This is cancel request, so 3rd parameter
    // is true to indicate that.
    CMmDataPackage aPackage;
    TBool trueCancel (ETrue);
    aPackage.PackData( &iCodeID, &trueCancel );

    TInt retValue ( iMmPhoneTsy->MessageManager()->HandleRequestL( 
        ECustomCheckSecurityCodeCancelIPC, &aPackage ) );

    return retValue;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::CompleteCheckSecurityCodeCancel
// Completes a CheckSecurityCodeCancel request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomSecurityTsy::CompleteCheckSecurityCodeCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_COMPLETECHECKSECURITYCODECANCEL_1, "TSY: CMmCustomSecurityTsy::CompleteCheckSecurityCodeCancel");
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::DeliverCodeL
// Brings required code to the Custom TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::DeliverCodeL( 
    RMobilePhone::TCodeAndUnblockCode aCodes )
    {
    iSecCode.Copy( aCodes.iCode );

    CMmDataPackage aPackage;
    aPackage.PackData( &iSecCode );
    iMmPhoneTsy->MessageManager()->HandleRequestL( 
        ECustomSecurityDeliverCodeIPC, &aPackage );

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_DELIVERCODEL_1,  "TSY: CMmCustomSecurityTsy::DeliverCodeL iSecCode: %S", iSecCode );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::DisablePhoneLockL
// This method disables phone lock
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::DisablePhoneLockL(
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TMobilePassword* aVerifyCode )
    {
    if ( NULL != iTsyReqHandleStore->GetTsyReqHandle(
        ESecurityRequestTypeDisablePhoneLock ) )
        {
        return KErrServerBusy;
        }

    // Send message to Security Server to disable security code
    // and intialize ret value
    CMmDataPackage aPackage;
    aPackage.PackData( aVerifyCode );
    TInt retValue ( iMmPhoneTsy->MessageManager()->HandleRequestL( 
        ECustomDisablePhoneLockIPC, &aPackage ) );

    // check result
    if ( KErrNone != retValue )
        {
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, retValue );
        }
    else
        {
        // Save the req handle type 
        iReqHandleType = ESecurityRequestTypeDisablePhoneLock;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::CompleteDisablePhoneLock
// Completes a DisablePhoneLock request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomSecurityTsy::CompleteDisablePhoneLock(
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSECURITYTSY_COMPLETEDISABLEPHONELOCK_1, "TSY: CMmCustomSecurityTsy::CompleteDisablePhoneLock - Error code: %{TSymbianErrorCodes}", aErrorCode );
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        ESecurityRequestTypeDisablePhoneLock );

    if ( ESecurityTsyReqHandleUnknown != reqHandle )
        {
        iMmCustomTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::GetActivePin
// Returns the currently active PIN. The value is updated by
// events coming from security server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomSecurityTsy::GetActivePin(
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TMobilePhoneSecurityCode* aActivePin )
    {
    *aActivePin = iActivePin;
    iMmCustomTsy->ReqCompleted ( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomSecurityTsy::SetActivePin
// Updates the information of the currently active PIN.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomSecurityTsy::SetActivePin(
    CMmDataPackage* aDataPackage )
    {
    RMobilePhone::TMobilePhoneSecurityCode* activePin = NULL;
    // Get the new data
    aDataPackage->UnPackData( &activePin );
        
    // Update the current information
    iActivePin = *activePin;
    }

//  End of File 
