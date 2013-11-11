// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef USING_CTSY_DISPATCHER

/**
 * This file contains additional function definitions for the CMmSecurityTsy class
 * in CTSY for use when the CTSY is used with the CTSY Dispatcher. 
 */

//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsecuritytsywithdispatcherTraces.h"
#endif

#include "cmmsecuritytsy.h"
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmphoneextinterface.h"
#include "CMmCustomSecurityTsy.h"

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
        
        //check that the code is available
        if ( 0 != codes->iCode.Length() )
            {
            if ( RMobilePhone::ESecurityCodePin1 == *type )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_VERIFYSECURITYCODEL1_1, "TSY: CMmSecurityTsy::VerifySecurityCodeL - PIN VERIFY REQUESTED");
				iLastPinRequested = EPin1Requested;
				}
			if ( RMobilePhone::ESecurityCodePin2 == *type )
				{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_VERIFYSECURITYCODEL1_2, "TSY: CMmSecurityTsy::VerifySecurityCodeL - PIN2 VERIFY REQUESTED");
				iLastPinRequested = EPin2Requested;
                }
            //This is to prevent unnecessary PIN1 request after PUK code 
            //request. Corrected at the same time with error TKEN-5WFJ7Y
            if ( ( ( RMobilePhone::ESecurityCodePuk1 == *type ) ||
                   ( RMobilePhone::ESecurityCodePuk2 == *type ) ) &&  
                   ( 0 < codes->iUnblockCode.Length() ) )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_VERIFYSECURITYCODEL1_3, "TSY: CMmSecurityTsy::VerifySecurityCodeL - PUK VERIFY REQUESTED");
                iPukCodeVerify = ETrue;
                }
            //This is to prevent unnecessary PIN1 request after phone password
            //request (PYRA-5UBCLC)
            if ( RMobilePhone::ESecurityCodePhonePassword == *type )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_VERIFYSECURITYCODEL1_4, "TSY: CMmSecurityTsy::VerifySecurityCodeL - PHONE PASSWORD VERIFY REQUESTED");
                iPhonePasswordVerify = ETrue;
                }
            iMmPhoneTsy->iMmPhoneExtInterface->DeliverCode( *codes );

            // CheckSecurityCode needs to set the lock as well, so pass the
            // code for lock setting
            if ( iMmPhoneTsy->iMmCustomTsy )
                {
                iMmPhoneTsy->iMmCustomTsy->DeliverCodeL( *codes ); 
                }

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
        else
            {       
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrArgument );    
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
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEVERIFYSECURITYCODEL1_1, "TSY: CMmSecurityTsy::CompleteVerifySecurityCode - Error:%d", aErrorCode);

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
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEVERIFYSECURITYCODEL1_2, "TSY: CMmSecurityTsy::CompleteVerifySecurityCodeL - PIN VERIFICATION NEEDED");
				CompleteNotifySecurityEventL( RMobilePhone::EPin1Required,
						KErrNone );
				iLastPinRequested = EPinUnknown;
				}
			// IF PIN2 REQUESTED LAST
			if (iLastPinRequested == EPin2Requested)
				{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEVERIFYSECURITYCODEL1_3, "TSY: CMmSecurityTsy::CompleteVerifySecurityCodeL - PIN2");
				iLastPinRequested = EPinUnknown;
				}
            }
        else if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != 
                  phoneSetLockSettingHandle && EActiveCodeToUpinAskUpin == 
                                                      iActiveCodeToUpinState )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSECURITYTSY_COMPLETEVERIFYSECURITYCODEL1_4, "TSY: CMmSecurityTsy::CompleteVerifySecurityCodeL - iActiveCodeToUpinState = EActiveCodeToUpinAskPin");
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
        
        if ( phoneSetLockSettingHandle != CMmPhoneTsy::EMultimodePhoneReqHandleUnknown )
            if ( KErrNone == aErrorCode )
                {
                CMmDataPackage dataPackage;
                //add into the package the stored lock and setting
                dataPackage.PackData( &iLockSettingPhoneLock, &iLockSetting );
                aErrorCode = iMmPhoneTsy->iMmPhoneExtInterface->
                    SetLockSettingL( &dataPackage );
                }
            else
                {
                CompleteSetLockSetting( aErrorCode, RMobilePhone::EStatusLockUnknown, RMobilePhone::ELockSetUnknown );
                }        	        

        // Reset just used code, empty for next round.
        RMobilePhone::TCodeAndUnblockCode codes;
        _LIT( KNullCharacter, "\0" );
        codes.iCode.Copy( KNullCharacter );
        iMmPhoneTsy->iMmPhoneExtInterface->DeliverCode( codes );
        }
    }
    
#endif //USING_CTSY_DISPATCHER
