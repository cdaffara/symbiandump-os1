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



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsecuritytsyTraces.h"
#endif

#include "cmmsecuritytsy.h"
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagemanagerbase.h"
#include "cmmphoneextinterface.h"
#include "CMmCommonStaticUtility.h"
#include "CMmCustomSecurityTsy.h"

// ======== MEMBER FUNCTIONS ========

CMmSecurityTsy* CMmSecurityTsy::NewL(    
        CMmPhoneTsy* aPhoneTsy ) // Ptr to PhoneTsy   
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_NEWL_1, "TSY: CMmSecurityTsy::NewL");
    CMmSecurityTsy* const mmSecurityTsy = new ( ELeave ) CMmSecurityTsy();
    CleanupStack::PushL( mmSecurityTsy );
    mmSecurityTsy->iMmPhoneTsy = aPhoneTsy;
    mmSecurityTsy->ConstructL();
    CleanupStack::Pop( mmSecurityTsy );

    return mmSecurityTsy;
    }

CMmSecurityTsy::CMmSecurityTsy()
    {
    }

void CMmSecurityTsy::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_CONSTRUCTL_1, "TSY: CMmSecurityTsy::ConstructL");
    // Is security codes (PIN and phone password) checked in boot
    iSecurityCheckedForBoot = EFalse;

    // information to know is PIN1 or PIN2 last requested
    iLastPinRequested = EPinUnknown;

    // Is TSY storing ntofication for "Lock if SIM changed" in boot
    iIsSecurityCodeRequestCachedInBoot = EFalse;

    // Is PIN1 disable supported, by default ETrue. Changed when SIM
    // Service Table is read and it does not support PIN1 disable.
    iPin1DisableSupported = ETrue;

    // Set lock setting to unknown 
    iLockSetting = RMobilePhone::ELockSetUnknown;

    //Set iPhonePasswordVerify to EFalse
    iPhonePasswordVerify = EFalse;

    //Set change UPIN state to idle
    iActiveCodeToUpinState = EActiveCodeToUpinIdle;
    }
      
CMmSecurityTsy::~CMmSecurityTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_DTOR_1, "TSY: CMmSecurityTsy::~CMmSecurityTsy");
    }  
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::DoExtFunc
// Security-specific functionality of CMmPhoneTsy::DoExtFuncL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage ) 
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_DOEXTFUNCL_1, "TSY: CMmSecurityTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TInt ret ( KErrNone );

    TAny* dataPtr = aPackage.Ptr1();

    switch ( aIpc )
        {
        // Get Security Capabilities
        case EMobilePhoneGetSecurityCaps:
            ret = GetSecurityCaps( aTsyReqHandle, 
                REINTERPRET_CAST( TUint32*, dataPtr ) );
            break;
        // Notify Change of Security Capabilities
        case EMobilePhoneNotifySecurityCapsChange:
            ret = NotifySecurityCapsChange(
                REINTERPRET_CAST( TUint32*, dataPtr ) );
            break;
                case EMobilePhoneGetLockInfo:
            ret = GetLockInfoL( aTsyReqHandle, aPackage );
            break;
        // Notify Change of Lock Information
        case EMobilePhoneNotifyLockInfoChange:
            ret = NotifyLockInfoChange(  
                REINTERPRET_CAST( RMobilePhone::TMobilePhoneLock*, dataPtr ),
                aPackage.Des2n() );
            break;
        // Set Lock Setting
        case EMobilePhoneSetLockSetting:
            ret = SetLockSettingL( aTsyReqHandle, 
                  aPackage );
            break;
        // Change Security Code
        case EMobilePhoneChangeSecurityCode:
            ret = ChangeSecurityCodeL( aTsyReqHandle, aPackage );
            break;
        // Notify Security Event
        case EMobilePhoneNotifySecurityEvent:
            ret = NotifySecurityEventL(
                REINTERPRET_CAST( RMobilePhone::TMobilePhoneSecurityEvent*, 
                dataPtr ) );
            break;
        // Verify Security Code
        case EMobilePhoneVerifySecurityCode:
            ret = VerifySecurityCodeL( aTsyReqHandle, aPackage );
            break;
        // Abort Security Code
        case EMobilePhoneAbortSecurityCode:
            ret = AbortSecurityCodeL( aTsyReqHandle, aPackage );
            break;
        case EMobilePhoneGetSecurityCodeInfo:
            ret = GetSecurityCodeInfoL( aTsyReqHandle, REINTERPRET_CAST( 
                RMobilePhone::TMobilePhoneSecurityCode*, dataPtr ), 
                aPackage.Des2n() );
            break;
        case EMobilePhoneNotifySecurityCodeInfoChange:
            ret = NotifySecurityCodeInfoChange( REINTERPRET_CAST( 
                RMobilePhone::TMobilePhoneSecurityCode*, dataPtr ), 
                aPackage.Des2n() );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_DOEXTFUNCL_2, "TSY: CMmSecurityTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::CancelService
// Cancels Security requests
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::CancelService(
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle ) 
    {
    TInt ret ( KErrNotSupported );
    
    //When the clients close their sub-sessions (eg. by calling RLine::Close),
    //they may not have cancelled all their outstanding asynchronous requests 
    //before closing.  It is up to the ETel server to clean up in this 
    //situation, so the server will find the list of outstanding requests 
    //related to that sub-session object and pass these outstanding IPC 
    //request numbers, one at a time, to the CancelService method in the TSY.

    switch ( aIpc )
        {
        case EMobilePhoneNotifySecurityCapsChange:
            ret = NotifySecurityCapsChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetLockInfo:
            ret = GetLockInfoCancel ( aTsyReqHandle );
            break;
        case EMobilePhoneSetLockSetting:
            ret = SetLockSettingCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyLockInfoChange:
            ret = NotifyLockInfoChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifySecurityEvent:
            ret = NotifySecurityEventCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetSecurityCodeInfo:
            ret = GetSecurityCodeInfoCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifySecurityCodeInfoChange:
            ret = NotifySecurityCodeInfoChangeCancel( aTsyReqHandle );
            break;
        //Default case
        default:
            ret = KErrNone; 
            break;
        }

    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::GetSecurityCaps
// returns a snapshot of the combination of the 
// static and dynamic security capabilities of the phone. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::GetSecurityCaps(
    const TTsyReqHandle aTsyReqHandle, 
    TUint32* aCaps ) 
    {

    TInt ret ( KErrAccessDenied );
    ret = iMmPhoneTsy->iMmPhoneExtInterface->GetSecurityCaps( aCaps ); 
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::NotifySecurityCapsChange
// This request allows a client to be notified when the phone's 
// security capabilities change
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::NotifySecurityCapsChange(
    TUint32* aCaps ) 
    {
    iRetNotifySecurityCapsChange = aCaps;
    iMmPhoneTsy->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifySecurityCapsChange;

    return KErrNone;    
    }
// ---------------------------------------------------------------------------
// CMmSecurityTsy::NotifySecurityCapsChangeCancel
// This method cancels an outstanding asynchronous 
// NotifySecurityCapsChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::NotifySecurityCapsChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    iRetNotifySecurityCapsChange = NULL;
    
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifySecurityCapsChange );
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;    
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteNotifySecurityCapsChange
// This method completes an outstanding asynchronous 
// NotifySecurityCapsChange request. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::CompleteNotifySecurityCapsChange(
    TUint32 aCaps ) 
    {
    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifySecurityCapsChange );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        *iRetNotifySecurityCapsChange = aCaps;
        iMmPhoneTsy->ReqCompleted( reqHandle, KErrNone );
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::GetLockInfoL
// This method retrieves the current status and setting of a lock.   
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::GetLockInfoL( const TTsyReqHandle aTsyReqHandle, const TDataPackage& aPackage )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_GETLOCKINFOL_1, "LTSY: CMmSecurityTsy::GetLockInfoL - Client call");
    
    TInt ret ( KErrArgument );
    
    TDes8* data = reinterpret_cast<TDes8*> ( aPackage.Des2n() );
    
    if ( sizeof ( RMobilePhone::TMobilePhoneLockInfoV1 ) <= 
        data->MaxLength() )
        {   
        //save pointer to client side for completion
        iRetGetLockInfo = data;

#ifdef REQHANDLE_TIMER
        iMmPhoneTsy->SetTypeOfResponse( CMmPhoneTsy::EMultimodePhoneGetLockInfo, aTsyReqHandle );
#else
        iMmPhoneTsy->iTsyReqHandleStore->SetTsyReqHandle( 
                    CMmPhoneTsy::EMultimodePhoneGetLockInfo, aTsyReqHandle );
#endif //REQHANDLE_TIMER

        ret = iMmPhoneTsy->iMmPhoneExtInterface->GetLockInfoL( aPackage );
         
        if ( KErrNone != ret )
            {
            iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneGetLockInfo );
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::NotifyLockInfoChange
// This method allows a client to be notified when any part of 
// the lock information changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::NotifyLockInfoChange(
    RMobilePhone::TMobilePhoneLock* aLock, 
    TDes8* aLockInfo ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_NOTIFYLOCKINFOCHANGE_1, "LTSY: CMmSecurityTsy::NotifyLockInfoChange - Lock: %d", aLock);
    
    TInt ret( KErrNone );
    
    if ( sizeof ( RMobilePhone::TMobilePhoneLockInfoV1 ) <= 
        aLockInfo->MaxLength() )
        {
        iRetNotifyLockInfoChange = aLockInfo;
        iRetNotifyPhoneLockChange = aLock;

        //save tsy req handle type
        iMmPhoneTsy->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneNotifyLockInfoChange;
        }
    else
        {
        ret = KErrArgument;
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::NotifyLockInfoChangeCancel
// This method cancels an outstanding asynchronous 
// NotifyLockInfoChange request 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::NotifyLockInfoChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_NOTIFYLOCKINFOCHANGECANCEL_1, "LTSY: CMmSecurityTsy::NotifyLockInfoChangeCancel");
    
    iRetNotifyLockInfoChange = NULL;
    iRetNotifyPhoneLockChange = NULL;
        
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyLockInfoChange );
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteNotifyLockInfoChange
// This method completes an outstanding asynchronous 
// NotifyLockInfoChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::CompleteNotifyLockInfoChange(
    CMmDataPackage* aDataPackage, TInt aErrorCode ) 
    {
    
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
            ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneNotifyLockInfoChange );
    
    if (KErrNone != aErrorCode)
        {
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        return;
        }
    
    RMobilePhone::TMobilePhoneLock* lock = NULL;
    RMobilePhone::TMobilePhoneLockInfoV1* lockInfo = NULL;
    aDataPackage->UnPackData( &lockInfo, &lock );
    
    //update the status of the lock in the extension
    iMmPhoneTsy->iMmPhoneExtInterface->UpdateLockInfo( lockInfo->iStatus,
        lockInfo->iSetting, *lock );
    
    
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoPckg( *lockInfo );

        *iRetNotifyLockInfoChange = lockInfoPckg;
        *iRetNotifyPhoneLockChange = *lock;
        
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::SetLockSettingL
// This function prevents some other client to set lock setting
// while previous process is still going on because of password query from 
// the client. If process in question is still going function then the request
// complete with error code KErrServerBusy. Some lock settings requires 
// code verification. This method will call CompleteNotifySecurityEvent with 
// required even in each case.(other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::SetLockSettingL(
    const TTsyReqHandle aTsyReqHandle,    
    const TDataPackage& aPackage ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_SETLOCKSETTINGL_1, "LTSY: CMmSecurityTsy::SetLockSettingL - Client call" );

    TInt ret( KErrNone );

    // Check if request handle already exists
    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        GetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneSetLockSetting );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        ret = LockSettingL( aTsyReqHandle, aPackage );
        // if error is other than KErrAccessDenied or KErrNone, complete now
        // KErrAccessDenied means we need verification first, not that an
        // error has occured and we ought to complete the request. 
        // KErrNone means that EMobilePhoneSetLockSetting request has been 
        // successfully sent to LTSY.
        if ( !( KErrNone == ret || KErrAccessDenied == ret ) )
            {
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }                     
        }
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::SetLockSettingCancel
// Use this method to cancel a previously placed asynchronous 
// SetLockSetting request 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::SetLockSettingCancel(
    TTsyReqHandle aTsyReqHandle ) 
    {
    iActiveCodeToUpinState = EActiveCodeToUpinIdle;

    //reset the req handle
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSetLockSetting );

    //complete with cancel
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
       
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::LockSettingL
// This method allows a client to change the current setting of 
// a lock. If password is required, function triggers completion of 
// security event notification and stores required variables for possible 
// retry. Other possible errors are reported to client as well 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::LockSettingL(
    const TTsyReqHandle, 
    const TDataPackage& aPackage ) 
    {
    TInt ret ( KErrNotSupported );

    RMobilePhone::TMobilePhoneLock* lock = 
        reinterpret_cast<RMobilePhone::TMobilePhoneLock* > 
                                                        ( aPackage.Ptr1() );
    RMobilePhone::TMobilePhoneLockSetting* setting = 
        reinterpret_cast<RMobilePhone::TMobilePhoneLockSetting* >
        ( aPackage.Ptr2() );

OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_LOCKSETTINGL_1, "LTSY: CMmSecurityTsy::LockSetting - Lock:%d, Setting:%d",*lock, *setting );

    // Some SIM cards might not support PIN1 disable. Thus
    // return error if client tries to disable it.
    // complete the request immediately with error.
    if ( ( RMobilePhone::ELockSetDisabled == *setting ) &&
         ( RMobilePhone::ELockICC == *lock ||
           RMobilePhone::ELockUniversalPin == *lock ) &&
           !iPin1DisableSupported )
        {
        // Operation is not allowed as SIM does not support changing
        // SIM from ON to OFF.
        ret = KErrGsm0707OperationNotAllowed;
        }
    else
        {
         //check if the request made is an state change attempt
        if ( RMobilePhone::ELockSetUnknown != *setting )
            {   
            //check if the lock to be set requires previous verification
            RMobilePhone::TMobilePhoneSecurityEvent event (
                RMobilePhone::ENoICCFound );
            //be pesimistic
            ret = KErrAccessDenied;

            //Check the lock to be set
            switch ( *lock )
                {
                case RMobilePhone::ELockPhoneDevice:
                case RMobilePhone::ELockPhoneToICC:
                    event = RMobilePhone::EPhonePasswordRequired;
                    break;
                case RMobilePhone::ELockICC:
                    event = RMobilePhone::EPin1Required;
                    break;
                // In USIM cards disabling PIN2 is also allowed
                case RMobilePhone::ELockPin2:
                    event = RMobilePhone::EPin2Required;
                    break;
                case RMobilePhone::ELockUniversalPin:
                    // This is here to prompt PIN query in case of replacing
                    // PIN1 with UPIN
                    event = RMobilePhone::EUniversalPinRequired;
                    if ( RMobilePhone::ELockReplaced == *setting )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_LOCKSETTINGL_2, "TSY: CMmSecurityTsy::LockSetting RMobilePhone::ELockReplaced");
                        iActiveCodeToUpinState = EActiveCodeToUpinAskUpin;
                        }
                    break;
                // This setting is not supported
                case RMobilePhone::ELockPhoneToFirstICC:
                    ret = KErrNotSupported;
                    break;

                case RMobilePhone::ELockOTA:
                case RMobilePhone::ELockHiddenKey:
                case RMobilePhone::ELockUSimApp:
                case RMobilePhone::ELockSecondUSimApp:
                default:
                    //other locks are not a problem
                    ret = KErrNone;
                    break;
                }
            //if the access is denied, notify the client about
            //the security event
            if ( KErrAccessDenied == ret )
                {
                CompleteNotifySecurityEventL( event, KErrNone );
                // Store these for the retry once the 
                //VerifySecurityCode is issued by client
                iLockSettingPhoneLock = *lock;
                iLockSetting = *setting;
                //the setLockSetting is ongoing
                iMmPhoneTsy->iReqHandleType = 
                    CMmPhoneTsy::EMultimodePhoneSetLockSetting;
                }
            //Check if the lock can be set directly
            else if ( KErrNone ==  ret ) 
                {
                CMmDataPackage dataPackage;
                dataPackage.SetPacketData ( &aPackage );
                iMmPhoneTsy->iMmPhoneExtInterface->
                    SetLockSettingL( &dataPackage );
                //the SetLockSetting is ongoing
                iMmPhoneTsy->iReqHandleType =
                    CMmPhoneTsy::EMultimodePhoneSetLockSetting;
                }
            }
        
        }

    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteSetLockSetting
// This method completes either an outstanding asynchronous 
// SimPinEnable or SetLockSetting request. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::CompleteSetLockSetting(
    TInt aErrorCode, //Error code
    RMobilePhone::TMobilePhoneLockStatus, 
    RMobilePhone::TMobilePhoneLockSetting ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETESETLOCKSETTING_1, "LTSY: CMmSecurityTsy::CompleteSetLockSetting - Error:%{TSymbianErrorCodes}", aErrorCode);

    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneSetLockSetting );

    // Check if handle for set lock setting exists
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        iActiveCodeToUpinState = EActiveCodeToUpinIdle;

        //If the code verification has been requested complete it too.
        TTsyReqHandle verifyHandle = iMmPhoneTsy->iTsyReqHandleStore->
            ResetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneVerifySecurityCode );

        if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != verifyHandle )
            {
            // Change these so that they wont mess up anything.
            iLockSetting = RMobilePhone::ELockSetUnknown;
            // Complete verify
            iMmPhoneTsy->ReqCompleted( verifyHandle, aErrorCode );
            }
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::ChangeSecurityCodeL
// This method allows a client to change a security code.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::ChangeSecurityCodeL(
    const TTsyReqHandle aTsyReqHandle, 
    const TDataPackage& aPackage )
    {

    TInt ret = iMmPhoneTsy->iMmPhoneExtInterface->
        ChangeSecurityCodeL( aPackage );

    if ( KErrNone == ret )
        {
        iMmPhoneTsy->iReqHandleType =
            CMmPhoneTsy::EMultimodePhoneChangeSecurityCode;
        }
    else
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteChangeSecurityCode
// This method completes an outstanding asynchronous 
// ChangeSecurityCode request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::CompleteChangeSecurityCode(
    TInt aErrorCode ) // Error code
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETECHANGESECURITYCODE_1, "TSY: CMmSecurityTsy::CompleteChangeSecurityCode - Error:%{TSymbianErrorCodes}", aErrorCode);


    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneChangeSecurityCode );

    if ( reqHandle != CMmPhoneTsy::EMultimodePhoneReqHandleUnknown )
        {
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::NotifySecurityEventL
// This method allows a client to be notified when the phone 
// has detected the occurrence of one of the security related events.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::NotifySecurityEventL(
        RMobilePhone::TMobilePhoneSecurityEvent* aEvent ) 
    {

    iRetNotifySecurityEvent = aEvent;

    iMmPhoneTsy->iReqHandleType =
        CMmPhoneTsy::EMultimodePhoneNotifySecurityEvent;

    // If requested after boot, then we need to query security server
    // state for phone password. If it is enabled, we will not have
    // security indication as Security server boots up faster than 
    // TSY and has already sent the indications when TSY is loaded.
    // Thus this is the only way to get the information in boot.
    if  ( iSecurityCheckedForBoot == EFalse )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_NOTIFYSECURITYEVENTL_1,  "TSY: CMmSecurityTsy::NotifySecurityEvent - Checking PIN state");
        // We can't do anything if sending fails. If this happens,
        // then every send to IsaApi should fail.
        (void)iMmPhoneTsy->iMmPhoneExtInterface->GetICCTypeL();
        }

    // If we have cached notification for security code in boot, then
    // complete it once to client.
    if ( iIsSecurityCodeRequestCachedInBoot  && 
         !iMmPhoneTsy->iBootState.iSecReady  )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_NOTIFYSECURITYEVENTL_2,  "TSY: CMmSecurityTsy::NotifySecurityEventL - Completing security code event");

        CompleteNotifySecurityEventL( RMobilePhone::EPhonePasswordRequired,
            KErrNone );
        iIsSecurityCodeRequestCachedInBoot = EFalse;
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::NotifySecurityEventCancel
// This function cancels an outstanding asynchronous 
// NotifySecurityEvent request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::NotifySecurityEventCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    iRetNotifySecurityEvent = NULL;

    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifySecurityEvent );

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteNotifySecurityEventL
// This function completes an outstanding asynchronous 
// NotifySecurityEvent request. If handle for security event notification
// does not exist, it also completes those functions that would otherwise 
// require the presence of the handle to complete themselves.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::CompleteNotifySecurityEventL(
    RMobilePhone::TMobilePhoneSecurityEvent aEvent, 
    TInt aErrorCode ) 
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETENOTIFYSECURITYEVENTL_1, "TSY: CMmSecurityTsy::CompleteNotifySecurityEvent - Event: %d, Error: %{TSymbianErrorCodes}",aEvent, aErrorCode );

	if ( iLastPinRequested != EPinUnknown && 
		( RMobilePhone::EPin1Verified == aEvent ) || 
		( RMobilePhone::EPin2Verified == aEvent ) )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETENOTIFYSECURITYEVENTL_2,  "TSY: CMmSecurityTsy::CompleteNotifySecurityEvent - iLastPinRequested != EPinUnknown");
    	iLastPinRequested = EPinUnknown;
        }

    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneNotifySecurityEvent );
    
    //Check if handle exists
    if ( reqHandle != CMmPhoneTsy::EMultimodePhoneReqHandleUnknown )
        {
        // When client is informed about security code state in boot,
        // then this flag needs to be ETrue. There will be no more checking
        // later on
        iSecurityCheckedForBoot = ETrue;

        // Set value if the result is KErrNone
        if ( KErrNone == aErrorCode )
            {
            *iRetNotifySecurityEvent = aEvent;
            }
        //reset pointer to client side
        iRetNotifySecurityEvent = NULL;
        // Complete the client request
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    else 
        {
        // Client has not yet requested this notification, so cache it
        // if event is for phone password and boot is not yet done.
        // It is possible that this indication is received before 
        // client has booted up.
        if ( ( RMobilePhone::EPhonePasswordRequired == aEvent ) &&
               !iMmPhoneTsy->iBootState.iSecReady &&
               !iIsSecurityCodeRequestCachedInBoot  )

            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETENOTIFYSECURITYEVENTL_3, "TSY: CMmSecurityTsy::CompleteNotifySecurityEvent - Caching event");
            iIsSecurityCodeRequestCachedInBoot = ETrue;
            }

        // Check if the event is a PIN2 requirement
        if ( aEvent == RMobilePhone::EPin2Required )
            {
            // These are the methods that must be completed, if there is no
            // handle that can be used for informing client about password
            // verification query.
            TInt errorCode = CMmCommonStaticUtility::EpocErrorCode(
                KErrAccessDenied, KErrGsm0707SimPin2Required );

            reqHandle = iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneSetFdnSetting );
            // Check if handle  for setfdnsetting exists
            if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
                {
                iMmPhoneTsy->CompleteSetFdnSettingL( errorCode );
                }
            }
        // Check if the event is a PIN1 or security code requirement.
        if ( aEvent == RMobilePhone::EPin1Required || 
             aEvent == RMobilePhone::EPhonePasswordRequired )
            {
            TTsyReqHandle setLockSettingHandle = 
                iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneSetLockSetting ); 
            // Check if handle for setlocksetting exists. If so, complete
            // lock setting, because client can not be informed about password
            // verification requirement without notify security event handle
            if ( setLockSettingHandle > 0 )
                {
                CompleteSetLockSetting( KErrAccessDenied, 
                    RMobilePhone::EStatusLockUnknown,
                    RMobilePhone::ELockSetUnknown );
                }
            }
        }
    // Update security capabilities and check if notification completion for 
    // capabilities is required 
    iMmPhoneTsy->iMmPhoneExtInterface->UpdateSecurityCaps( aEvent );
    }

#ifndef USING_CTSY_DISPATCHER

/**
  * Checks that security codes are well formed.
  */
TBool CMmSecurityTsy::AreCodesWellFormed(
    const RMobilePhone::TMobilePhoneSecurityCode& aCodeType,
    const RMobilePhone::TCodeAndUnblockCode& aCodes ) const
    {
    
    const TUint8 KPukCodeLength(8);
    const TUint8 KMaxPinCodeLength(8);
    const TUint8 KMinPinCodeLength(4);
    
    switch (aCodeType)
        {
        case RMobilePhone::ESecurityCodePuk1:
        case RMobilePhone::ESecurityCodePuk2:
            {
            //
            // Verify PUK code is well formed according to 3GPP TS 11.11
            // (8 numeric digits)
            //
            if (aCodes.iUnblockCode.Length() != KPukCodeLength)
                {
                return EFalse;
                }
            TChar c(0);
            for (TInt i=0; i<KPukCodeLength; ++i)
                {
                c = aCodes.iUnblockCode[i];
                if (c < '0' || c > '9' )
                    return EFalse;
                }
            }
            //
            // and fall through to verify PIN is well formed too
            //
        case RMobilePhone::ESecurityCodePin1:
        case RMobilePhone::ESecurityCodePin2:
            {
            //
            // Verify PIN code conforms with 3GPP TS 11.11
            // (between 4 and 8 numeric digits)
            //        
            TInt len = aCodes.iCode.Length();
            if ( (len < KMinPinCodeLength) || (len > KMaxPinCodeLength) )
                return EFalse;
            TChar c(0);
            for (TInt i=0; i < len; ++i)
                {
                c = aCodes.iCode[i];
                if (c < '0' || c > '9' )
                    return EFalse;
                }
            }
            break;

        default:
            // For all other types of code, check should happen in the LTSY.
            // Returning ETrue will ensure request goes on to LTSY.
            break;
        }
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::VerifySecurityCodeL
// This method sends a security code requiring verification to 
// the phone. If code is required for lock setting, it sends it to the
// extension and orders re-execution of SetLockSetting function. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::VerifySecurityCodeL(
    const TTsyReqHandle aTsyReqHandle, 
    const TDataPackage& aPackage )
    {

    RMobilePhone::TMobilePhoneSecurityCode* type = 
        reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCode*>
            ( aPackage.Ptr1() );
    RMobilePhone::TCodeAndUnblockCode* codes = 
        reinterpret_cast<RMobilePhone::TCodeAndUnblockCode*>
            ( aPackage.Ptr2() );

    // Get request handle to check whether server is busy with previous 
    // request
    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        GetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneVerifySecurityCode );
    
    if ( 0 < reqHandle )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {

        TInt ret( KErrArgument );
        
        // Check that received codes are well formed
        if (!AreCodesWellFormed(*type,*codes))
            {
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrArgument );
            return KErrNone;
            }

        if ( RMobilePhone::ESecurityCodePin1 == *type )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_VERIFYSECURITYCODEL_1, "TSY: CMmSecurityTsy::VerifySecurityCodeL - PIN VERIFY REQUESTED");
            iLastPinRequested = EPin1Requested;
            }
        if ( RMobilePhone::ESecurityCodePin2 == *type )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_VERIFYSECURITYCODEL_2, "TSY: CMmSecurityTsy::VerifySecurityCodeL - PIN2 VERIFY REQUESTED");
            iLastPinRequested = EPin2Requested;
            }
        //This is to prevent unnecessary PIN1 request after PUK code 
        //request. Corrected at the same time with error TKEN-5WFJ7Y
        if ( ( ( RMobilePhone::ESecurityCodePuk1 == *type ) ||
               ( RMobilePhone::ESecurityCodePuk2 == *type ) ) &&  
               ( 0 < codes->iUnblockCode.Length() ) )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_VERIFYSECURITYCODEL_3, "TSY: CMmSecurityTsy::VerifySecurityCodeL - PUK VERIFY REQUESTED");
            iPukCodeVerify = ETrue;
            }
        //This is to prevent unnecessary PIN1 request after phone password
        //request (PYRA-5UBCLC)
        if ( RMobilePhone::ESecurityCodePhonePassword == *type )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_VERIFYSECURITYCODEL_4, "TSY: CMmSecurityTsy::VerifySecurityCodeL - PHONE PASSWORD VERIFY REQUESTED");
            iPhonePasswordVerify = ETrue;
            }
        iMmPhoneTsy->iMmPhoneExtInterface->DeliverCode( *codes );

        // CheckSecurityCode needs to set the lock as well, so pass the
        // code for lock setting
        if ( iMmPhoneTsy->iMmCustomTsy )
            {
            iMmPhoneTsy->iMmCustomTsy->DeliverCodeL( *codes ); 
            }

        //Check if the SetLockSetting is ongoing, in some cases
        //SetLockSetting requires a verification of PIN1 or security code
        //before continue with the lock setting 
        reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
            GetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneSetLockSetting );

        if ( reqHandle != CMmPhoneTsy::EMultimodePhoneReqHandleUnknown &&
             EActiveCodeToUpinAskUpin != iActiveCodeToUpinState )
            {
            CMmDataPackage dataPackage;
            //add into the package the stored lock and setting
            dataPackage.PackData( &iLockSettingPhoneLock, &iLockSetting );
            ret = iMmPhoneTsy->iMmPhoneExtInterface->
                SetLockSettingL( &dataPackage );
            iMmPhoneTsy->iReqHandleType = 
                    CMmPhoneTsy::EMultimodePhoneVerifySecurityCode;

            if ( KErrNone != ret )
                {
                CompleteSetLockSetting( ret, 
                    RMobilePhone::EStatusLockUnknown,
                    RMobilePhone::ELockSetUnknown );
                }
            }
        //This is a regular verification or active code 
        //change from PIN to UPIN
        else 
            {
            ret = iMmPhoneTsy->iMmPhoneExtInterface->
                VerifySecurityCodeL( aPackage );

            if ( KErrNone == ret )
                {
                iMmPhoneTsy->iReqHandleType = 
                    CMmPhoneTsy::EMultimodePhoneVerifySecurityCode;
                }
            else
                {
                // Complete verify with error
                iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );

                // If FDN request is on-going, complete is also with error
                reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
                    GetTsyReqHandle( 
                        CMmPhoneTsy::EMultimodePhoneSetFdnSetting );
                if ( reqHandle != 
                     CMmPhoneTsy::EMultimodePhoneReqHandleUnknown )
                    {
                    iMmPhoneTsy->CompleteSetFdnSettingL( ret );
                    }
                reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
                    GetTsyReqHandle( 
                        CMmPhoneTsy::EMultimodePhoneSetLockSetting );
                        
                if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != 
                      reqHandle && EActiveCodeToUpinAskUpin == 
                                      iActiveCodeToUpinState )
                    {
                    //Verifying UPIN failed
                    CompleteSetLockSetting( ret, 
                        RMobilePhone::EStatusLockUnknown,
                            RMobilePhone::ELockSetUnknown );
                    }
               }
            }

        } // end check request handle

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteVerifySecurityCodeL
// This method completes an outstanding asynchronous 
// VerifySecurityCode request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::CompleteVerifySecurityCodeL(
        TInt aErrorCode ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEVERIFYSECURITYCODEL_1, "TSY: CMmSecurityTsy::CompleteVerifySecurityCode - Error:%{TSymbianErrorCodes}", aErrorCode);

    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneVerifySecurityCode );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Complete the request
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );

        // This needs to be done for PIN code verification when user has 
        // dual line SIM and user has changed the line. If PIN code 
        // verification fails, we need to request PIN code again.

        // NOTE! This MUST NOT happen in bootup when PIN code is verified
        // and NOT when PIN code is changed to ON/OFF from settings.

        // DO NOT complete if SetLockSetting is called -> PIN ON/OFF change 
        // from general settings.

        // Not completed if PUK code verify or Phonepassword verify
        TTsyReqHandle phoneSetLockSettingHandle = 
                iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneSetLockSetting );

        // DO NOT complete if booting up 
        if ( ( iMmPhoneTsy->iBootState.iSecReady ) &&
             ( iLastPinRequested != EPinUnknown ) && 
             ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown ==
               phoneSetLockSettingHandle ) &&
             ( KErrNone != aErrorCode ) &&
             ( !iPukCodeVerify ) &&
             ( !iPhonePasswordVerify ) )
            {
            // IF PIN1 REQUESTED LAST
            if ( iLastPinRequested == EPin1Requested )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEVERIFYSECURITYCODEL_2, "TSY: CMmSecurityTsy::CompleteVerifySecurityCodeL - PIN VERIFICATION NEEDED");
            	CompleteNotifySecurityEventL( RMobilePhone::EPin1Required,
                KErrNone );
            	iLastPinRequested = EPinUnknown;
                }
            // IF PIN2 REQUESTED LAST
            if (iLastPinRequested == EPin2Requested)
            	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEVERIFYSECURITYCODEL_3, "TSY: CMmSecurityTsy::CompleteVerifySecurityCodeL - PIN2");
            	iLastPinRequested = EPinUnknown;
            	}
            }
        else if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != 
                  phoneSetLockSettingHandle && EActiveCodeToUpinAskUpin == 
                                                      iActiveCodeToUpinState )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEVERIFYSECURITYCODEL_4, "TSY: CMmSecurityTsy::CompleteVerifySecurityCodeL - iActiveCodeToUpinState = EActiveCodeToUpinAskPin");
            if ( KErrNone == aErrorCode )
                {
                iActiveCodeToUpinState = EActiveCodeToUpinAskPin;
                CompleteNotifySecurityEventL( 
                    RMobilePhone::EPin1Required, KErrNone );
                }
            else
                {
                iActiveCodeToUpinState = EActiveCodeToUpinIdle;
                CompleteSetLockSetting( KErrAccessDenied, 
                    RMobilePhone::EStatusLockUnknown,
                    RMobilePhone::ELockSetUnknown );
                }
            }
        //Set iPukCodeVerify to EFalse
        iPukCodeVerify = EFalse;

        //Set iPhonePasswordVerify to EFalse
        iPhonePasswordVerify = EFalse;

        // FDN HANDLING
        reqHandle = iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSetFdnSetting );

        if ( reqHandle != CMmPhoneTsy::EMultimodePhoneReqHandleUnknown )
            {
            if ( KErrNone == aErrorCode )
                {
                aErrorCode = iMmPhoneTsy->iMmPhoneExtInterface->
                    SetFdnSetting( iMmPhoneTsy->iRetFdnSetting );
                }
            else
                {
                iMmPhoneTsy->CompleteSetFdnSettingL( aErrorCode );
                }
            }

        // Reset just used code, empty for next round.
        RMobilePhone::TCodeAndUnblockCode codes;
        _LIT( KNullCharacter, "\0" );
        codes.iCode.Copy( KNullCharacter );
        iMmPhoneTsy->iMmPhoneExtInterface->DeliverCode( codes );
        }
    }
#endif //USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmSecurityTsy::AbortSecurityCodeL
// This function informs the phone that the user has cancelled 
// an outstanding "get security code" request 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::AbortSecurityCodeL(
    const TTsyReqHandle aTsyReqHandle, 
    const TDataPackage& aPackage ) 
    {
    TInt ret = KErrNone;

    // Check if security code verification was aborted for the lock setting
    TTsyReqHandle reqHandle = 
        iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneSetLockSetting );

    // Check also that VerifySecurityCode has not yet been called
    TTsyReqHandle verifyHandle =
        iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneVerifySecurityCode );

    // If so, complete set lock setting
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle &&
         CMmPhoneTsy::EMultimodePhoneReqHandleUnknown == verifyHandle )
        {
        // Complete with abort error code
        CompleteSetLockSetting( KErrAbort, 
            RMobilePhone::EStatusLockUnknown,
            RMobilePhone::ELockSetUnknown );
        
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
        }
    // In other cases we need to inform Licencee TSY about the abort request
    //for further processing
    else 
        {
        // Check if security code verification was aborted for the fdn setting
        reqHandle = iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSetFdnSetting );      

        if ( reqHandle != CMmPhoneTsy::EMultimodePhoneReqHandleUnknown )
            {
            // Security UI excepts KErrAbort for abort security code
            iMmPhoneTsy->CompleteSetFdnSettingL( KErrAbort );

            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            
            // allow for EMobilePhoneAbortSecurityCode immediate completion
#ifdef REQHANDLE_TIMER
            iMmPhoneTsy->SetTypeOfResponse(
                CMmPhoneTsy::EMultimodePhoneAbortSecurityCode, 
                    aTsyReqHandle );
#else
            iMmPhoneTsy->iTsyReqHandleStore->SetTsyReqHandle(
                CMmPhoneTsy::EMultimodePhoneAbortSecurityCode, 
                    aTsyReqHandle );
#endif            
            ret = iMmPhoneTsy->iMmPhoneExtInterface->AbortSecurityCodeL( 
                aPackage );

            // dos layer returned with error and didn't complete request
            if ( ret != KErrNone )
                {
                CompleteAbortSecurityCode(ret);
                }   
            }
        }

    return KErrNone;   
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteAbortSecurityCode
// This function completes outstanding asynchronous 
// AbortSecurityCode request. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::CompleteAbortSecurityCode(
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEABORTSECURITYCODE_1, "TSY: CMmSecurityTsy::CompleteAbortSecurityCode - Error:%{TSymbianErrorCodes}", aErrorCode);

    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneAbortSecurityCode );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }  
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::SetPin1DisableNotSupported
// Sets iPin1DisableSupported to EFalse. PIN can't be disabled.  
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::SetPin1DisableNotSupported()
    {
    iPin1DisableSupported = EFalse;
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteGetLockInfo
// This method completes either an outstanding asynchronous 
// SimPinEnable, GetLockInfo or SetLockSetting request. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::CompleteGetLockInfo(
    CMmDataPackage* aDataPackage, 
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEGETLOCKINFO_1, "LTSY: CMmSecurityTsy::CompleteGetLockInfo - Error:%{TSymbianErrorCodes}", aErrorCode);

    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneGetLockInfo );

    // check if handle is present
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {

        if ( KErrNone == aErrorCode )
            {
            //unpack the data
            RMobilePhone::TMobilePhoneLockStatus* status = NULL;
            RMobilePhone::TMobilePhoneLockSetting* setting = NULL;

            aDataPackage->UnPackData( &status, &setting );

            RMobilePhone::TMobilePhoneLockInfoV1 apckg;
            apckg.iStatus = *status;
            apckg.iSetting = *setting;

            RMobilePhone::TMobilePhoneLockInfoV1Pckg lockInfoPckg( apckg );

            *iRetGetLockInfo = lockInfoPckg;
            }

        iRetGetLockInfo = NULL;
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmSecurityTsy::GetLockInfoCancel
// This method cancels either an outstanding asynchronous 
// SimPinEnable, GetLockInfo or SetLockSetting request. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::GetLockInfoCancel(
    TTsyReqHandle aTsyReqHandle )
    {
    //reset the pointer to the client side
    iRetGetLockInfo = NULL;

    //reset the req handle
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneGetLockInfo );

    //complete with cancel
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
       
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::GetSecurityCodeInfoL
// This method retrieves the current number of remaining entry attemps of 
// security code.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::GetSecurityCodeInfoL( 
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode, 
    TDes8* aSecurityCodeInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_GETSECURITYCODEINFOL_1, "TSY: CMmSecurityTsy::GetSecurityCodeInfoL");

    TInt ret( KErrNone );
    
    if ( sizeof ( RMobilePhone::TMobilePhoneSecurityCodeInfoV5  ) <= 
        aSecurityCodeInfo->MaxLength() )
        {
        TBool reqAlreadyProcessing( EFalse );
    	// Check if the request is already processing
    	for( TInt i = 0; i < iGetSecurityCodeInfoRequests.Count(); i++ )
    	    {
    	    if( iGetSecurityCodeInfoRequests[ i ]->iSecurityCode 
    	        == *aSecurityCode )
    	        {
    	        reqAlreadyProcessing = ETrue;
    	        break;
    	        }
    	    }

        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
    	if( reqAlreadyProcessing )
            {
            TGetSecurityCodeInfoRequest* req = new ( ELeave ) 
                TGetSecurityCodeInfoRequest();
            req->iReqHandle = aTsyReqHandle;
            req->iSecurityCode = *aSecurityCode;
            req->iSecurityCodeInfo = aSecurityCodeInfo;
            iGetSecurityCodeInfoRequests.AppendL( req );
            }
        else
            {
            CMmDataPackage dataPackage;
            dataPackage.PackData( aSecurityCode );

            ret = iMmPhoneTsy->MessageManager()->HandleRequestL( 
                EMobilePhoneGetSecurityCodeInfo, &dataPackage );

            if ( KErrNone == ret )  
                { 
                //save tsy req handle type 
                switch (*aSecurityCode)
                	{
                	case RMobilePhone::ESecurityCodePin1:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPin1; 
                		break;
                	case RMobilePhone::ESecurityCodePin2:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPin2; 
                		break;
                	case RMobilePhone::ESecurityCodePuk1:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPuk1; 
                		break;
                	case RMobilePhone::ESecurityCodePuk2:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPuk2; 
                		break;
                	case RMobilePhone::ESecurityCodePhonePassword:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPhonePassword; 
                		break;
                	case RMobilePhone::ESecurityCodeSPC:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoSPC; 
                		break;
                	case RMobilePhone::ESecurityHiddenKey:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPhonebookHiddenKey; 
                		break;
                	case RMobilePhone::ESecurityUSIMAppPin:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoUSIMAppPin; 
                		break;
                	case RMobilePhone::ESecuritySecondUSIMAppPin:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoSecondUSIMAppPin; 
                		break;
                	case RMobilePhone::ESecurityUniversalPin:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoUniversalPin; 
                		break;
                	case RMobilePhone::ESecurityUniversalPuk:
                		iMmPhoneTsy->iReqHandleType = 
                			CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoUniversalPuk; 
                		break;
                	}
                
                // save request in queue for completion
                TGetSecurityCodeInfoRequest* req = new ( ELeave ) 
                    TGetSecurityCodeInfoRequest();
                req->iReqHandle = aTsyReqHandle;
                req->iSecurityCode = *aSecurityCode;
                req->iSecurityCodeInfo = aSecurityCodeInfo;
                iGetSecurityCodeInfoRequests.AppendL( req );
                } 
    		}
        }
    else
        {
        ret = KErrArgument;
        }
            
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteGetSecurityCodeInfo
// This method completes the GetSecurityCodeInfo request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::CompleteGetSecurityCodeInfo( 
    CMmDataPackage* aDataPackage, 
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEGETSECURITYCODEINFO_1, "TSY: CMmSecurityTsy::CompleteGetSecurityCodeInfo");

	RMobilePhone::TMobilePhoneSecurityCode* securityCode;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5* securityCodeInfoV5;
	aDataPackage->UnPackData( &securityCode, &securityCodeInfoV5 );
	
	ResetGetSecurityCodeInfoTsyReqHandle(*securityCode);
	
	for ( TInt i = 0; i < iGetSecurityCodeInfoRequests.Count(); i++ )
	    {
	    if( iGetSecurityCodeInfoRequests[ i ]->iSecurityCode 
	        == *securityCode )
	        {
	        if (aErrorCode == KErrNone)
	        	{
		        RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg* 
		            securityCodeInfoV5Pckg = REINTERPRET_CAST( 
		            RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg*, 
		            iGetSecurityCodeInfoRequests[ i ]->iSecurityCodeInfo );
	            
	            RMobilePhone::TMobilePhoneSecurityCodeInfoV5& securityCodeInfo = 
	                ( *securityCodeInfoV5Pckg )();
	
		        securityCodeInfo.iRemainingEntryAttempts 
		            = securityCodeInfoV5->iRemainingEntryAttempts;
	        	}
	        iMmPhoneTsy->ReqCompleted( 
	            iGetSecurityCodeInfoRequests[ i ]->iReqHandle, aErrorCode );
	        	        
	        delete iGetSecurityCodeInfoRequests[ i ];
	        iGetSecurityCodeInfoRequests.Remove( i );
	        }
	    }
    
    iGetSecurityCodeInfoRequests.Compress();
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::GetSecurityCodeInfoCancel
// This method cancels the GetSecurityCodeInfo request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::GetSecurityCodeInfoCancel( 
    TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_GETSECURITYCODEINFOCANCEL_1, "TSY: CMmSecurityTsy::GetSecurityCodeInfoCancel");

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin1;
	TBool reqHandleExist = EFalse;
	
	for ( TInt i = 0; i < iGetSecurityCodeInfoRequests.Count(); i++ )
		{
	  	TGetSecurityCodeInfoRequest* req = iGetSecurityCodeInfoRequests[ i ];
	  	
	  	if( aTsyReqHandle == req->iReqHandle  )
	  		{
	  		secCode = req->iSecurityCode;
	  		iMmPhoneTsy->ReqCompleted( req->iReqHandle, KErrCancel );
	  		delete iGetSecurityCodeInfoRequests[ i ];
	  		iGetSecurityCodeInfoRequests.Remove( i );
	  		reqHandleExist = ETrue;
	  		break;	
	  		}
		}

	if (reqHandleExist)
		{
		iGetSecurityCodeInfoRequests.Compress();
		
		TBool secCodeExist = EFalse;
		
		for ( TInt i = 0; i < iGetSecurityCodeInfoRequests.Count(); i++ )
			{
		  	TGetSecurityCodeInfoRequest* req = iGetSecurityCodeInfoRequests[ i ];
		  	
		  	if (iGetSecurityCodeInfoRequests[i]->iSecurityCode == secCode)
		  		{
		  		secCodeExist = ETrue;
		  		break;	
		  		}
			}
		
		if (!secCodeExist)
			{
			ResetGetSecurityCodeInfoTsyReqHandle(secCode);
			}
		}
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::ResetGetSecurityCodeInfoTsyReqHandle
// This method resets the (GetSecurityCodeInfo) request handle.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::ResetGetSecurityCodeInfoTsyReqHandle( 
		const RMobilePhone::TMobilePhoneSecurityCode aSecurityCode )
	{
    switch (aSecurityCode)
		{
		case RMobilePhone::ESecurityCodePin1:
			iMmPhoneTsy->iTsyReqHandleStore->
						ResetTsyReqHandle(
						CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPin1);
			break;
        case RMobilePhone::ESecurityCodePin2:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPin2 );
        	break;
        case RMobilePhone::ESecurityCodePuk1:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPuk1 );
        	break;
        case RMobilePhone::ESecurityCodePuk2:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPuk2 );
        	break;
        case RMobilePhone::ESecurityCodePhonePassword:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPhonePassword );
        	break;
        case RMobilePhone::ESecurityCodeSPC:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoSPC );
        	break;
        case RMobilePhone::ESecurityHiddenKey:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoPhonebookHiddenKey );
        	break;
        case RMobilePhone::ESecurityUSIMAppPin:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoUSIMAppPin );
        	break;
        case RMobilePhone::ESecuritySecondUSIMAppPin:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoSecondUSIMAppPin );
        	break;
        case RMobilePhone::ESecurityUniversalPin:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoUniversalPin );
        	break;
        case RMobilePhone::ESecurityUniversalPuk:
			iMmPhoneTsy->iTsyReqHandleStore->
			            ResetTsyReqHandle( 
			            CMmPhoneTsy::EMultimodePhoneGetSecurityCodeInfoUniversalPuk );
        	break;
		}
	}

// ---------------------------------------------------------------------------
// CMmSecurityTsy::NotifySecurityCodeInfoChange
// This method allows a client to be notified when the security code 
// information changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::NotifySecurityCodeInfoChange( 
    RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode, 
    TDes8* aSecurityCodeInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_NOTIFYSECURITYCODEINFOCHANGE_1, "TSY: CMmSecurityTsy::NotifySecurityCodeInfoChange");

    TInt ret( KErrNone );

    if ( sizeof ( RMobilePhone::TMobilePhoneSecurityCodeInfoV5 ) <= 
        aSecurityCodeInfo->MaxLength() )
        {
        iRetNotifySecurityCode = aSecurityCode;
        iRetNotifySecurityCodeInfo = aSecurityCodeInfo;
    
        iMmPhoneTsy->iReqHandleType 
            = CMmPhoneTsy::EMultimodePhoneNotifySecurityCodeInfoChange;
        }
    else
        {
        ret = KErrArgument;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::CompleteNotifySecurityCodeInfoChange
// This method completes the NotifySecurityCodeInfoChange notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSecurityTsy::CompleteNotifySecurityCodeInfoChange( 
    CMmDataPackage* aDataPackage, 
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETENOTIFYSECURITYCODEINFOCHANGE_1, "TSY: CMmSecurityTsy::CompleteNotifySecurityCodeInfoChange");

    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
        ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifySecurityCodeInfoChange );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            RMobilePhone::TMobilePhoneSecurityCode* securityCode;
            RMobilePhone::TMobilePhoneSecurityCodeInfoV5* securityCodeInfoV5;
            aDataPackage->UnPackData( &securityCode, &securityCodeInfoV5 );
            
            RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg* 
                securityCodeInfoV5Pckg = REINTERPRET_CAST( 
                RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg*, 
                iRetNotifySecurityCodeInfo );
            
            RMobilePhone::TMobilePhoneSecurityCodeInfoV5& securityCodeInfo 
                = ( *securityCodeInfoV5Pckg )();

            securityCodeInfo.iRemainingEntryAttempts 
                = securityCodeInfoV5->iRemainingEntryAttempts;
            *iRetNotifySecurityCode = *securityCode;
            }
            
        // clean the pointers to client side
        iRetNotifySecurityCodeInfo = NULL;
        iRetNotifySecurityCode = NULL;

        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmSecurityTsy::NotifySecurityCodeInfoChangeCancel
// This method cancels the NotifySecurityCodeInfoChange notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSecurityTsy::NotifySecurityCodeInfoChangeCancel( 
    TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_NOTIFYSECURITYCODEINFOCHANGECANCEL_1, "TSY: CMmSecurityTsy::NotifySecurityCodeInfoChangeCancel");

    iRetNotifySecurityCodeInfo = NULL;
    iRetNotifySecurityCode = NULL;

    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifySecurityCodeInfoChange );

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

    
//  End of File 
