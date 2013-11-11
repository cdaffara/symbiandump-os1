// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <etelext.h>
#include <etelmm.h>
#include <ctsy/rmmcustomapi.h>

// MODULE DATA STRUCTURES
enum
    {
    ESlotActivateSimLockPassword,
    ESlotActivateSimLockNumber,
    ESlotDeActivateSimLockPassword,
    ESlotDeActivateSimLockNumber,
    ESlotNotifyDtmfEvent,
    ESlotSetAlsBlocked,
    ESlotGetAlsBlocked,
    ESlotCallOrigin,
    ESlotNotifyAlsBlockedChanged,
    ESlotTerminateCall,
    ESlotGetCipheringInfo,
    ESlotNotifyCipheringInfoChange,
    ESlotNotifyNSPSStatus,
    ESlotNetWakeup,
    ESlotNotifySsAdditionalInfo,
    ESlotNotifySsRequestComplete,
    ESlotCheckAlsPpSupport,
    ESlotViagHomeZoneParams,
    ESlotViagHomeZoneElems,
    ESlotReadViagCacheId,
    ESlotReadViagCacheRecord,
    ESlotWriteViagCacheId,
    ESlotWriteViagCacheRecord,
    ESlotWriteViagUHZIUESettings,
    ESlotCheckSecurityCode,
    ESlotCheckEmergencyNumberNumMode,
    ESlotCheckEmergencyNumberResult,
    ESlotGetOperatorName,
    ESlotGetProgrammableOperatorId,
    ESlotGetProgrammableOperatorLogo,
    ESlotCheckTwoDigitDialSupport,
    ESlotNotifyPndCacheReady,
    ESlotGetPndCacheStatus,
    ESlotSsNotifyTypeAndMode,
    ESlotSsNotifyInfo,
    ESlotGetOperatorNameType,
    ESlotGetNextSimCbTopic,
    ESlotDeleteSimCbTopic,
    ESlotNotifyProgrammableOperatorId,
    ESlotDisablePhoneLock,
    ESlotGet3GPBInfo,
    ESlotGetSystemNetworkModes,
    ESlotSetSystemNetworkMode,
    ESlotGetCurrentSystemNetworkModes,
    ESlotGetSimCardReaderStatus,
    ESlotNotifySimCardStatus,
    ESlotSendAPDUReqV2Status,
    ESlotSetDriveMode,
    ESlotSetAlwaysOn,
    ESlotNotifyRauEvent,
    ESlotReadHSxPAStatus,
    ESlotWriteHSxPAStatus,
    ESlotNotifyHSxPAStatus,
    ESlotGetIccCallForwardingStatusCFInd,
    ESlotGetIccCallForwardingStatusMspId,
    ESlotNotifyIccCallForwardingStatusChangeCFInd,
    ESlotNotifyIccCallForwardingStatusChangeMspId,
    ESlotNotifyCellInfoChange,
    ESlotGetCellInfo,
    ESlotGetSystemNetworkBand, 
    ESlotSetSystemNetworkBand, 
    ESlotGetUSIMServiceSupport,
    ESlotNotifyRemoteAlertingToneStatusChange,
    //let the next value be the last in the enum
    ESlotMaxNumOfNonConstSlots
    };

// Const slots enumeration
enum
    {
    //let the next value be the last in the enum
    ESlotCMaxNumOfConstSlots,
    };

#define K_EXPECTED_ELEMENT_COUNT_FOR_READ_VIAG_HOME_ZONE			4


// ======== MEMBER FUNCTIONS ========

EXPORT_C RMmCustomAPI::RMmCustomAPI():iCustomPtrHolder( NULL )
    {
    }

EXPORT_C void RMmCustomAPI::ConstructL()
    {
    iCustomPtrHolder = CMmCustomPtrHolder::NewL( ESlotMaxNumOfNonConstSlots,
        ESlotCMaxNumOfConstSlots );
    }

EXPORT_C void RMmCustomAPI::Destruct()
    {
    if ( NULL != iCustomPtrHolder )
        {
        delete iCustomPtrHolder;
        }
    iCustomPtrHolder = NULL;
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::Open
// This method opens a RMmCustomAPI subsession from RMobilePhone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::Open(
    RMobilePhone& aPhone )
    {
    RSessionBase* session=&aPhone.SessionHandle();
    __ASSERT_ALWAYS( session != 0, PanicClient( EEtelPanicNullHandle ) );
    TInt subSessionHandle=aPhone.SubSessionHandle();
    __ASSERT_ALWAYS( subSessionHandle != 0, PanicClient(
        EEtelPanicNullHandle ) );
    TRAPD( ret, ConstructL() );
    if ( ret )
        {
        return ret;
        }

    TPtrC name( KCustomAPIName );
    TIpcArgs args( &name, TIpcArgs::ENothing, subSessionHandle );
    SetSessionHandle( *session );
    ret = CreateSubSession( *session, EEtelOpenFromSubSession, args);

    if ( ret )
        {
        Destruct();
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::Close
// This method closes a RMmCustomAPI subsession
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::Close()
    {
    CloseSubSession( EEtelClose );
    Destruct();
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SetDriveMode
// This function sets the Drive Mode status with the value defined in
// aModeStatus.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SetDriveMode(
    TRequestStatus& aStatus,
    const TSetDriveMode aModeStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iSetDriveMode = aModeStatus;

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotSetDriveMode,
        iCustomPtrHolder->iSetDriveMode );
    Set( ECustomSetDriveModeIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ActivateSimLock
// This function activates the sim lock synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::ActivateSimLock(
    const TSimLockPassword& aPassWord,
    const TLockNumber aNumber ) const
    {
    TPtr8 passWord( REINTERPRET_CAST( TText8*, CONST_CAST( TSimLockPassword*,
        &aPassWord ) ), sizeof( TSimLockPassword ),
        sizeof( TSimLockPassword ) );

    TPtr8 number( REINTERPRET_CAST( TText8*, CONST_CAST( TLockNumber*,
         &aNumber ) ), sizeof( TLockNumber ), sizeof( TLockNumber ) );

    return Set( ECustomSimLockActivateIPC, passWord, number );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ActivateSimLock
// This function activates the sim lock asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ActivateSimLock(
    TRequestStatus& aStatus,
    const TSimLockPassword& aPassWord,
    const TLockNumber aNumber ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    iCustomPtrHolder->iSimLockPassword = aPassWord;
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotActivateSimLockPassword,
        iCustomPtrHolder->iSimLockPassword );

    iCustomPtrHolder->iSimLockNumber=aNumber;
    TPtr8& ptr2 = iCustomPtrHolder->Set( ESlotActivateSimLockNumber,
        iCustomPtrHolder->iSimLockNumber );

    Set( ECustomSimLockActivateIPC, aStatus, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::DeActivateSimLock
// This function Deactivates the sim lock synchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::DeActivateSimLock(
    const TSimLockPassword& aPassWord,
    const TLockNumber aNumber ) const
    {
    TPtr8 passWord( REINTERPRET_CAST( TText8*, CONST_CAST( TSimLockPassword*,
        &aPassWord ) ), sizeof( TSimLockPassword ),
        sizeof( TSimLockPassword ) );

    TPtr8 number( REINTERPRET_CAST( TText8*, CONST_CAST(
        TLockNumber*, &aNumber ) ), sizeof( TLockNumber ),
        sizeof( TLockNumber) );

    return Set( ECustomSimLockDeActivateIPC, passWord, number );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::DeActivateSimLock
// This function Deactivates the sim lock asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::DeActivateSimLock(
    TRequestStatus& aStatus,
    const TSimLockPassword& aPassWord,
    const TLockNumber aNumber ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    iCustomPtrHolder->iSimLockPassword=aPassWord;
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotDeActivateSimLockPassword,
        iCustomPtrHolder->iSimLockPassword );

    iCustomPtrHolder->iSimLockNumber=aNumber;
    TPtr8& ptr2 = iCustomPtrHolder->Set( ESlotDeActivateSimLockNumber,
        iCustomPtrHolder->iSimLockNumber );

    Set( ECustomSimLockDeActivateIPC, aStatus, ptr, ptr2 );
    }


// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyDtmfEvent
// This function notifies a client of a Dtmf event.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyDtmfEvent(
    TRequestStatus& aStatus,
    TDtmfInfo& aInfo )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifyDtmfEvent, aInfo );
    Get( ECustomNotifyDtmfEventIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetDiagnosticInfo
// Gets the Diagnostics octets of a given call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetDiagnosticInfo(
    TName& aCallName) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aCallName ), sizeof( TName ),
        sizeof( TName ) );

    return Get( ECustomGetDiagnosticOctetsIPC, ptr);
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SetAlsBlocked
// This function sets the alternating line service block
// status with the value defined in aBlockStatus.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::SetAlsBlocked(
    const TSetAlsBlock aBlockStatus ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, CONST_CAST( TSetAlsBlock*,
        &aBlockStatus ) ), sizeof( TSetAlsBlock ), sizeof( TSetAlsBlock ) );

    return Set( ECustomSetAlsBlockedIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SetAlsBlocked
// This function sets the alternating line service block
// status with the value defined in aBlockStatus.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SetAlsBlocked(
    TRequestStatus& aStatus,
    const TSetAlsBlock aBlockStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle) );
    iCustomPtrHolder->iSetAlsBlock = aBlockStatus;

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotSetAlsBlocked,
        iCustomPtrHolder->iSetAlsBlock );
    Set( ECustomSetAlsBlockedIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetAlsBlocked
// This function returns the alternating line service block status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetAlsBlocked(
    TGetAlsBlockStatus& aBlockStatus ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aBlockStatus ),
        sizeof( TGetAlsBlockStatus ), sizeof( TGetAlsBlockStatus ) );

    return Get( ECustomGetAlsBlockedIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetAlsBlocked
// This function returns the alternating line service block status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetAlsBlocked(
    TRequestStatus& aStatus,
    TGetAlsBlockStatus& aBlockStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotGetAlsBlocked, aBlockStatus );
    Get( ECustomGetAlsBlockedIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CheckAlsPpSupport
// This function returns Als Pp Support state.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::CheckAlsPpSupport(
    TAlsSupport& aSupport ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aSupport ),
        sizeof( TAlsSupport ), sizeof ( TAlsSupport ) );

    return Get( ECustomCheckAlsPpSupportIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CheckAlsPpSupport
// This function returns Als Pp Support state.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::CheckAlsPpSupport(
    TRequestStatus& aStatus,
    TAlsSupport& aSupport )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient(EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotCheckAlsPpSupport, aSupport );
    Get( ECustomCheckAlsPpSupportIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetRemoteAlertingToneStatus
// This function reads the remote alerting tone status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetRemoteAlertingToneStatus(
    TRemoteAlertingToneStatus& aToneStatus ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aToneStatus ),
        sizeof( TRemoteAlertingToneStatus ), sizeof(
        TRemoteAlertingToneStatus ) );

    return Get( ECustomGetRemoteAlertingToneStatusIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CallOrigin
// This function returns the origin of the call,
// defined by aCallName parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::CallOrigin(
    TName& aCallName,
    TCallOrigin& aOrigin ) const
    {
    TPtr8 ptr1( REINTERPRET_CAST( TText8*, &aCallName ), sizeof( TName ),
        sizeof( TName ) );

    TPtr8 ptr2( REINTERPRET_CAST( TText8*, &aOrigin ), sizeof( TCallOrigin ),
        sizeof( TCallOrigin ) );

    return Get( ECustomCallOriginIPC, ptr1, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TerminateCall
// This synchronous function terminates a given call or all the ongoing calls
// based in the value of aCallName
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::TerminateCall(
    TName& aCallName )const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aCallName ), sizeof( TName ),
        sizeof( TName ) );

    return Get( ECustomTerminateCallIPC, ptr);
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TerminateCall
// This asynchronous function terminates a given call or all
// the ongoing calls based in the value of aCallName
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::TerminateCall(
    TRequestStatus& aStatus,
    TName& aCallName )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotTerminateCall, aCallName );
    Get( ECustomTerminateCallIPC, aStatus, ptr);
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyAlsBlockedChanged
// This function notifies a client of changed Als blocked status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyAlsBlockedChanged(
    TRequestStatus& aStatus,
    TGetAlsBlockStatus& aBlockStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifyAlsBlockedChanged,
        aBlockStatus );
    Get( ECustomNotifyAlsBlockedChangedIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetCipheringInfo
// This function gets the ciphering information synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetCipheringInfo(
    TCipheringInfo& aInfo )
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aInfo ), sizeof( TCipheringInfo ),
        sizeof( TCipheringInfo ) );

    return Get( ECustomGetCipheringInfoIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetCipheringInfo
// This function gets the ciphering info asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetCipheringInfo(
    TRequestStatus& aStatus,
    TCipheringInfo& aInfo )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotGetCipheringInfo, aInfo );
    Get( ECustomGetCipheringInfoIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyCipheringInfoChange
// This function notifies a client of ciphering info change
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyCipheringInfoChange(
    TRequestStatus& aStatus,
    TCipheringInfo& aInfo )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifyCipheringInfoChange, aInfo );
    Get( ECustomNotifyCipheringInfoChangeIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyNSPSStatus
// This function notifies a client of a change in NSPS
// (No Service Power Save) status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyNSPSStatus(
    TRequestStatus& aStatus,
    TNspsStatus& aNspsStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifyNSPSStatus, aNspsStatus );
    Get( ECustomNotifyNSPSStatusIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NetWakeup
// This function wakes up the Net from the NSPS state
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NetWakeup(
    TRequestStatus& aStatus )
    {
    Blank( ECustomNetWakeupIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ReadViagHomeZoneParams
// This function reads VIAG Home Zone Parameters synchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::ReadViagHomeZoneParams(
    TViagParams& aParam,
    TViagElements& aElems ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aParam ),
        sizeof( TViagParams ), sizeof( TViagParams ) );

    TPtr8 ptr2( REINTERPRET_CAST( TText8*, &aElems ),
        sizeof( TViagElements ), sizeof( TViagElements ) );

     __ASSERT_ALWAYS( aElems.Count() == 4, PanicClient(
        EEtelPanicIndexOutOfRange ) );

     RMmCustomAPI::TViagElement* b = aElems.Back( 0 );
     const TInt sizeOfElement = sizeof( RMmCustomAPI::TViagElement );
     const TInt sizeOfArray = 4*sizeOfElement;

     ptr2.Set(
        REINTERPRET_CAST( TText8*, b ),
        sizeOfArray,
        sizeOfArray );

    return Get( EReadViagHomeZoneParamsIPC, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ReadViagHomeZoneParams
// This function reads VIAG Home Zone Parameters asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ReadViagHomeZoneParams(
    TRequestStatus& aStatus,
    TViagParams& aParam,
    TViagElements& aElems )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    
    if(aElems.Count() != K_EXPECTED_ELEMENT_COUNT_FOR_READ_VIAG_HOME_ZONE)
        {
        TRequestStatus* reqStatus = &aStatus;
        User::RequestComplete(reqStatus, KErrArgument);
        return;
        }
    
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotViagHomeZoneParams, aParam );
    TPtr8& ptr2 = iCustomPtrHolder->Set( ESlotViagHomeZoneElems, aElems );

     RMmCustomAPI::TViagElement* b = aElems.Back( 0 );
     const TInt sizeOfElement = sizeof( RMmCustomAPI::TViagElement );
     const TInt sizeOfArray = 4*sizeOfElement;

     ptr2.Set(
        REINTERPRET_CAST( TText8*, b ),
        sizeOfArray,
        sizeOfArray );

     Get( EReadViagHomeZoneParamsIPC, aStatus, ptr, ptr2 );
     }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ReadViagHomeZoneCache
// This function reads specified VIAG home zone cache synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::ReadViagHomeZoneCache(
    TViagCacheRecordId& aId,
    TViagCacheRecordContent& aRecord ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aId ),
        sizeof( TViagCacheRecordId ), sizeof( TViagCacheRecordId ) );
    TPtr8 ptr2( REINTERPRET_CAST( TText8*, &aRecord),
        sizeof( TViagCacheRecordContent ),
        sizeof( TViagCacheRecordContent ) );

    return Get( EReadViagHomeZoneCacheIPC, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ReadViagHomeZoneCache
// This function reads specified VIAG home zone cache asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ReadViagHomeZoneCache(
    TRequestStatus& aStatus,
    TViagCacheRecordId& aId,
    TViagCacheRecordContent& aRecord )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotReadViagCacheId, aId );
    TPtr8& ptr2 = iCustomPtrHolder->Set( ESlotReadViagCacheRecord, aRecord );

    Get( EReadViagHomeZoneCacheIPC, aStatus, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::WriteViagHomeZoneCache
// This function writes specified VIAG home zone cache synchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::WriteViagHomeZoneCache(
    const TViagCacheRecordId& aId,
    const TViagCacheRecordContent& aRecord ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, CONST_CAST(
        TViagCacheRecordId*, &aId ) ), sizeof( TViagCacheRecordId ),
        sizeof( TViagCacheRecordId ) );

    TPtr8 ptr2( REINTERPRET_CAST( TText8*, CONST_CAST(
        TViagCacheRecordContent*, &aRecord ) ),
        sizeof( TViagCacheRecordContent ),
        sizeof( TViagCacheRecordContent ) );

    return Set( EWriteViagHomeZoneCacheIPC, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::WriteViagHomeZoneCache
// This function writes specified VIAG home zone cache asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::WriteViagHomeZoneCache(
    TRequestStatus& aStatus,
    const TViagCacheRecordId& aId,
    const TViagCacheRecordContent& aRecord )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iViagCacheId = aId;
    iCustomPtrHolder->iViagCacheRecord = aRecord;
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotWriteViagCacheId,
        iCustomPtrHolder->iViagCacheId );
    TPtr8& ptr2 = iCustomPtrHolder->Set( ESlotReadViagCacheRecord,
        iCustomPtrHolder->iViagCacheRecord );
    Set( EWriteViagHomeZoneCacheIPC, aStatus, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ClearCallBlackList
// Clears asynchronously call blacklist.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ClearCallBlackList(
    TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    Blank( ECustomClearCallBlackListIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ClearCallBlackList
// Clears synchronously call blacklist.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::ClearCallBlackList() const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    return Blank( ECustomClearCallBlackListIPC );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SsAdditionalInfoNotification
// Notification request for additionalinfo message
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SsAdditionalInfoNotification(
    TRequestStatus& aStatus,
    TSsAdditionalInfo& aSsAdditionalInfo )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifySsAdditionalInfo,
        aSsAdditionalInfo );
    Get( ECustomSsAdditionalInfoNotificationIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SsRequestCompleteNotification
// Notify a client of the success of the sended SS command.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SsRequestCompleteNotification(
        TRequestStatus& aStatus,
        TInt& aSsStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifySsRequestComplete,
        aSsStatus );
    Get( ECustomNotifySsRequestCompleteIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::IsBlocked
// This method retrieves syncronously the information if the
// security code is blocked or not.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::IsBlocked(
    TSecurityCodeType& aType,
    TBool& aIsBlocked )
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aType ),
        sizeof( TSecurityCodeType ), sizeof( TSecurityCodeType ) );
    TPtr8 ptr2( REINTERPRET_CAST( TText8*, &aIsBlocked ),
        sizeof( TBool ), sizeof( TBool ) );

    return Get( ECustomIsBlockedIPC, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CheckSecurityCode
// This method activates syncronously the security code request and
// triggers the security server to prompt PIN-code from client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::CheckSecurityCode(
    const TSecurityCodeType aCode )
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, CONST_CAST(
        TSecurityCodeType*, &aCode ) ), sizeof( TSecurityCodeType ),
        sizeof( TSecurityCodeType ) );

    return Get( ECustomCheckSecurityCodeIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CheckSecurityCode
// This method activates asyncronously the security code request and
// triggers the security server to prompt PIN-code from client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::CheckSecurityCode(
    TRequestStatus& aStatus,
    const TSecurityCodeType aCode )
    {
    __ASSERT_ALWAYS ( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iSecurityCodeType = aCode;
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotCheckSecurityCode,
        iCustomPtrHolder->iSecurityCodeType );
    Get( ECustomCheckSecurityCodeIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetActivePin
// This method retrieves syncronously the information whether the currently
// active PIN is PIN1 or Universal PIN (UPIN).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetActivePin(
    RMobilePhone::TMobilePhoneSecurityCode& aActivePin )
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aActivePin ),
        sizeof( RMobilePhone::TMobilePhoneSecurityCode ),
        sizeof( RMobilePhone::TMobilePhoneSecurityCode ) );

    Get( ECustomGetActivePinIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetAirTimeDuration
// This method gets the air time duration. The air time
// calculation starts when a first call is connected and ends when last call
// is disconnected. If no calls are active, this method will return the value
// of the last air time duration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetAirTimeDuration(
    TTimeIntervalSeconds& aTime ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aTime ),
        sizeof( TTimeIntervalSeconds ), sizeof( TTimeIntervalSeconds ) );

    return Get( ECustomGetAirTimeDurationIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetNetworkProviderName
// This function gets network provider name synchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetNetworkProviderName(
    TDes& aNetworkProviderName ) const
    {
    return Get( ECustomGetNetworkProviderNameIPC, aNetworkProviderName );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetNetworkProviderName
// This function gets network provider name asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetNetworkProviderName(
    TRequestStatus& aStatus,
    TDes& aNetworkProviderName )
    {
    Get( ECustomGetNetworkProviderNameIPC, aStatus, aNetworkProviderName );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CheckEmergencyNumber
// This function checks if given is emergency number with given mode and
// returns the result synchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::CheckEmergencyNumber(
    TEmerNumberCheckMode& aNumMode,
    TBool& aResult ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aNumMode ),
        sizeof( TEmerNumberCheckMode ), sizeof( TEmerNumberCheckMode ) );

    TPtr8 ptr2( REINTERPRET_CAST( TText8*, &aResult ),
        sizeof( TBool ), sizeof( TBool ) );

    return Get( ECustomCheckEmergencyNumberIPC, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CheckEmergencyNumber
// This function checks if given is emergency number with given mode and
// returns the result asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::CheckEmergencyNumber(
    TRequestStatus& aStatus,
    TEmerNumberCheckMode& aNumMode,
    TBool& aResult )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotCheckEmergencyNumberNumMode,
        aNumMode );
    TPtr8& ptr2 = iCustomPtrHolder->Set( ESlotCheckEmergencyNumberResult,
        aResult );

    Get( ECustomCheckEmergencyNumberIPC, aStatus, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyPndCacheReady
// Notifies client when phonebook caching is ready
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyPndCacheReady(
    TRequestStatus& aStatus,
    TName& aPndName )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifyPndCacheReady, aPndName );

    Get( ECustomNotifyPndCacheReadyIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetPndCacheStatus
// This function gets current status of caching
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetPndCacheStatus(
    TRequestStatus& aStatus,
    RMmCustomAPI::TPndCacheStatus& aPndStatus,
    const TName& aPndName ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr1 = iCustomPtrHolder->Set( ESlotGetPndCacheStatus, aPndStatus );

    SetAndGet( ECustomGetPndCacheStatusIPC, aStatus, ptr1, aPndName );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetOperatorName
// This function gets asynchronously the operator name of the
// current Network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetOperatorName(
    TRequestStatus& aStatus,
    TOperatorNameInfo& aOperatorNameInfo )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    TPtr8& ptr1 = iCustomPtrHolder->Set( ESlotGetOperatorName,
        aOperatorNameInfo );

    Get( ECustomGetOperatorNameIPC, aStatus, ptr1 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetProgrammableOperatorLogo
// This function gets asynchronously the programmable operator logo and
// operator Id of the current Network.
// (other items were commented in a header).
//
//	This API is no longer supported!
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetProgrammableOperatorLogo(
    TRequestStatus& aStatus,
    TOperatorId& aOperatorId,
    TOperatorLogo& aLogo )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotGetProgrammableOperatorId,
        aOperatorId );

    TPtr8& ptr2 = iCustomPtrHolder->Set( ESlotGetProgrammableOperatorLogo,
        aLogo );

    Get( ECustomGetProgrammableOperatorLogoIPC, aStatus, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyProgrammableOperatorLogoChange
// This function notifies the programmable operator logo change
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyProgrammableOperatorLogoChange(
    TRequestStatus& aStatus,
    TOperatorId& aOperatorId )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifyProgrammableOperatorId,
        aOperatorId );

    Get( ECustomNotifyProgrammableOperatorLogoChangeIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifySsNetworkEvent
// This function notifies the client about different network generated
// SS events
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifySsNetworkEvent(
    TRequestStatus& aStatus,
    TSsTypeAndMode& aSsTypeAndMode,
    TSsInfo& aSsInfo )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotSsNotifyTypeAndMode,
        aSsTypeAndMode );
    TPtr8& ptr2 = iCustomPtrHolder->Set( ESlotSsNotifyInfo, aSsInfo );

    Get( ECustomNotifySsNetworkEventIPC, aStatus, ptr, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CancelUssdSession
// This function cancel active ussd session synchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::CancelUssdSession() const
    {
    return Blank( ECustomCancelUssdSessionIPC );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CancelUssdSession
// This function cancel active ussd session asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::CancelUssdSession(
    TRequestStatus& aStatus )
    {
    Blank( ECustomCancelUssdSessionIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SatRefreshCompleteNotification
// This function notifies a client of SAT refresh
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SatRefreshCompleteNotification(
    TRequestStatus& aStatus )
    {
    Blank( ECustomSatRefreshCompleteNotificationIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TCustomCallParams::TCustomCallParams
// This method is the constructor of class TCustomCallParams.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C RMmCustomAPI::TCustomCallParams::TCustomCallParams()
    {
    iExtensionId = KETelCustomExtCustomCallParamsV3;
    iSubAddress.Zero();
    iBearer.Zero();
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CheckTwoDigitDialSupport
// This function checks two digit dialing support state synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::CheckTwoDigitDialSupport(
    TTwoDigitDialSupport& aSupport ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, &aSupport ),
        sizeof( TTwoDigitDialSupport ), sizeof( TTwoDigitDialSupport ) );

    return Get( ECustomCheckTwoDigitDialSupportIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::CheckTwoDigitDialSupport
// This function checks two digit dialing support state asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::CheckTwoDigitDialSupport(
    TRequestStatus& aStatus,
    TTwoDigitDialSupport& aSupport )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotCheckTwoDigitDialSupport,
        aSupport );
    Get( ECustomCheckTwoDigitDialSupportIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ResetNetServer
// This function resets the net server
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ResetNetServer() const
    {
    Blank ( ECustomResetNetServerIPC );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ResetNetServer
// This function resets the net server asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ResetNetServer(
    TRequestStatus& aStatus )
    {
    Blank ( ECustomResetNetServerIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ReleaseFile
// This function releases file in CommDB.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ReleaseFile(
    TRequestStatus& aStatus,
    const TDesC& aFileName )
    {
    Set( ECustomReleaseFileIPC, aStatus, aFileName );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::RestartFile
// This function restarts the file in CommDB.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::RestartFile(
    TRequestStatus& aStatus,
    const TDesC& aFileName )
    {
    Set( ECustomRestartFileIPC, aStatus, aFileName );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::StartSimCbTopicBrowsing
// This function gets the Cell Broadcast message ID's from SIM
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::StartSimCbTopicBrowsing()
    {
    return Blank( ECustomStartSimCbTopicBrowsingIPC );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetNextSimCbTopic
// This function gets next Cell Broadcast Id.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetNextSimCbTopic(
    TSimCbTopic& aSimCbTopic )
    {
    TPtr8 ptr( REINTERPRET_CAST ( TText8*, &aSimCbTopic ),
        sizeof( TSimCbTopic ),sizeof( TSimCbTopic ) );

    return Get( ECustomGetNextSimCbTopicIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetNextSimCbTopic
// This function gets next Cell Broadcast Id.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetNextSimCbTopic(
    TRequestStatus& aStatus,
    TSimCbTopic& aSimCbTopic )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iSimCbTopic = aSimCbTopic;
    TPtr8& ptr = iCustomPtrHolder->Set(
        ESlotGetNextSimCbTopic, aSimCbTopic );
    Get( ECustomGetNextSimCbTopicIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::DeleteSimCbTopic
// This function deletes a Cell Broadcast Id from SIM
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::DeleteSimCbTopic(
    const TUint &aSimCbTopicNumber)
    {
    TPtr8 ptr(REINTERPRET_CAST( TText8*,CONST_CAST(
        TUint*,&aSimCbTopicNumber ) ), sizeof( TUint ),sizeof( TUint ) );

    return Set( ECustomDeleteSimCbTopicIPC, ptr );
    }

// ---------------------------------------------------------------------------
//  RMmCustomAPI::DeleteSimCbTopic
//  This function deletes a Cell Broadcast Id from SIM
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::DeleteSimCbTopic(
    TRequestStatus& aStatus,
    const TUint &aSimCbTopicNumber )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );
    iCustomPtrHolder->iSimCbTopicNumber = aSimCbTopicNumber;
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotDeleteSimCbTopic,
        iCustomPtrHolder->iSimCbTopicNumber );
    Set( ECustomDeleteSimCbTopicIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyNetworkConnectionFailure
// Notifies of a network connection failure. This failure is so sever that
// the only way to recover is to restart the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyNetworkConnectionFailure(
    TRequestStatus& aStatus )
    {
    Blank ( ECustomNotifyNetworkConnectionFailureIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SendAPDUReq
// This method transmits APDU requests message asyncronously to Etel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SendAPDUReq(
    TRequestStatus& aStatus,
    TApdu& anAPDUReq )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    Get( ECustomSendAPDUReqIPC, aStatus, *( anAPDUReq.iInfo ),
        *( anAPDUReq.iData ) );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SendAPDUReq
// This method transmits APDU requests message syncronously to Etel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::SendAPDUReq(
    TApdu& anAPDUReq )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    return Get( ECustomSendAPDUReqIPC, *( anAPDUReq.iInfo ), *(
        anAPDUReq.iData ) );
    }

// ---------------------------------------------------------------------------
// RMmCustomApiExt::DisablePhoneLock
// Disable phone lock with password supplied in aVerifyCode.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::DisablePhoneLock(
    TRequestStatus& aStatus,
    RMobilePhone::TMobilePassword& aVerifyCode ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr1 = iCustomPtrHolder->Set( ESlotDisablePhoneLock, aVerifyCode );

    Set( ECustomDisablePhoneLockIPC, aStatus, ptr1 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyEGprsInfoChange
// This method informs upper levels about EGPRS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyEGprsInfoChange(
    TRequestStatus& aStatus,
    TDes8& aGprsInfo ) const
    {
    Get( ECustomNotifyEGprsInfoChange, aStatus, aGprsInfo );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetEGprsInfo
// This method gets information about EGPRS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetEGprsInfo(
    TRequestStatus& aStatus,
    TDes8& aGprsInfo ) const
    {
    Get( ECustomGetEGprsInfo, aStatus, aGprsInfo );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TGprsInformation
// This is Constructor of class TGprsInformation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C RMmCustomAPI::TGprsInformation::TGprsInformation()
    {
    iGprsInfo = EGprs;
    }

// ---------------------------------------------------------------------------
// RMmCustomApiExt::ReadSimFile
// Reads information from the SIM file specified in aSimFileInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ReadSimFile (
    TRequestStatus& aStatus,
    TDes8& aSimFileInfo,
    TDes8& aResponseBytes ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    Get( ECustomReadSimFileIPC, aStatus, aSimFileInfo, aResponseBytes );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TSimFileInfo TSimFileInfo
// Default constructor for TSimFileInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C RMmCustomAPI::TSimFileInfo::TSimFileInfo()
    {
    iPath.Zero();
    iOffSet = 0;
    iSize = 0;
    }

// ---------------------------------------------------------------------------
// RMmCustomApiExt::TLifeTimeData::TLifeTimeData
// Default constructor for TLifeTimeData
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C RMmCustomAPI::TLifeTimeData::TLifeTimeData()
    {
    iCaps = ELifeTimeDataCapsUnknown;
    iHours = 0;
    iMinutes = 0;
    iManufacturingDate.Set( 0, EJanuary, 0, 0, 0, 0, 0 );
    }

// ---------------------------------------------------------------------------
// RMmCustomApiExt::GetLifeTime
// Get Life time information synchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetLifeTime(
    TDes8& aLifeTimeInfo ) const
    {
    return Get( ECustomGetLifeTimeIPC, aLifeTimeInfo );
    }

// ---------------------------------------------------------------------------
// RMmCustomApiExt::GetLifeTime
// Get Life time information asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetLifeTime(
    TRequestStatus& aStatus,
    TDes8& aLifeTimeInfo ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    Get( ECustomGetLifeTimeIPC, aStatus, aLifeTimeInfo );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetNextSimCbTopic
// This function gets next Cell Broadcast Id.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::Get3GPBInfo(
    TRequestStatus& aStatus,
    T3GPBInfo& aInfo ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotGet3GPBInfo, aInfo );
    Get( EGet3GPBInfoIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetSystemNetworkModes
// This method reads all network system modes synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetSystemNetworkModes(
    TUint32& aSupportedNetworkModes ) const
    {
    TPtr8 ptr(REINTERPRET_CAST( TText8*, CONST_CAST(
    TUint32*, &aSupportedNetworkModes ) ), sizeof( TUint32 ),
        sizeof( TUint32 ) );

    return Get( ECustomGetSystemNetworkModesIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetSystemNetworkModes
// This method reads all network system modes asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetSystemNetworkModes(
    TRequestStatus& aStatus,
    TUint32& aSupportedNetworkModes ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iSupportedNetworkModes = aSupportedNetworkModes;
    TPtr8& ptr = iCustomPtrHolder->Set(
        ESlotGetSystemNetworkModes, aSupportedNetworkModes );
    Get( ECustomGetSystemNetworkModesIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SetSystemNetworkMode
// This method sets network system mode synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::SetSystemNetworkMode(
    const TNetworkModeCaps aNetworkMode )
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, CONST_CAST(
    TNetworkModeCaps*, &aNetworkMode ) ),
        sizeof( TNetworkModeCaps ), sizeof( TNetworkModeCaps ) );

    return Set( ECustomSetSystemNetworkModeIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SetSystemNetworkMode
// This method sets network system mode asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SetSystemNetworkMode(
    TRequestStatus& aStatus,
    const TNetworkModeCaps aNetworkMode )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iNetworkModeCaps = aNetworkMode;
    TPtr8& ptr = iCustomPtrHolder->Set(
        ESlotSetSystemNetworkMode, iCustomPtrHolder->iNetworkModeCaps );
    Set( ECustomSetSystemNetworkModeIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetCurrentSystemNetworkModes
// This method reads current network system modes synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetCurrentSystemNetworkModes(
    TUint32& aCurrentNetworkModes ) const
    {
    TPtr8 ptr(REINTERPRET_CAST( TText8*, CONST_CAST(
    TUint32*,&aCurrentNetworkModes ) ), sizeof( TUint32 ),sizeof( TUint32 ) );

    return Get( ECustomGetCurrentSystemNetworkModesIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetCurrentSystemNetworkModes
// This method reads current network system modes asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetCurrentSystemNetworkModes(
    TRequestStatus& aStatus,
    TUint32& aCurrentNetworkModes ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iCurrentNetworkModes = aCurrentNetworkModes;

    TPtr8& ptr = iCustomPtrHolder->Set(
        ESlotGetCurrentSystemNetworkModes, aCurrentNetworkModes );
    Get( ECustomGetCurrentSystemNetworkModesIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::PowerSimOn
// Send Power SIM On Request to APDU server
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::PowerSimOn(
    TRequestStatus& aStatus )
    {
    Blank( ECustomPowerSimOnIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::PowerSimOff
// Send Power SIM Off Request to APDU server
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::PowerSimOff(
    TRequestStatus& aStatus )
    {
    Blank( ECustomPowerSimOffIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SimWarmReset
// Send Sim Warm Reset Request to APDU server
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SimWarmReset(
    TRequestStatus& aStatus )
    {
    Blank( ECustomSimWarmResetIPC, aStatus );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetATR
// Get ATR from APDU server
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetATR(
    TRequestStatus& aStatus, TDes8& aATR )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );
    Get( ECustomGetATRIPC, aStatus, aATR );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetSimCardReaderStatus
// Get card reader status from APDU server asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetSimCardReaderStatus(
    TRequestStatus& aStatus,
    TSimCardReaderStatus& aCardReaderStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotGetSimCardReaderStatus,
        aCardReaderStatus );
    Get( ECustomGetSimCardReaderStatusIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifySimCardStatus
// Notifies a client of physical status change of SIM
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifySimCardStatus(
    TRequestStatus& aStatus,
    TSIMCardStatus& aCardStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifySimCardStatus,
        aCardStatus );
    Get( ECustomNotifySimCardStatusIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TSimCardReaderStatus::GetCardReaderID
// Get card reader ID
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 RMmCustomAPI::TSimCardReaderStatus::GetCardReaderID()
    {
    // ReaderStatus byte (see /6/ GSM 11.14 section 12.33)
    // Bits
    // 1-3  Identity of card reader
    // 4    0=Card reader is not removable
    //      1=Card reader is removable
    // 5    0=Card reader is not present
    //      1=Card reader is present
    // 6    0=Card reader present is not ID-1 size
    //      1=Card reader present is ID-1 size
    // 7    0=No card present
    //      1=Card is present in reader
    // 8    0=No card powered
    //      1=Card in reader is powered

    return ( TUint8 )( iSimCardReaderStatus & 0x07 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TSimCardReaderStatus::IsCardReaderRemovable
// Is card reader removable
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool RMmCustomAPI::TSimCardReaderStatus::IsCardReaderRemovable()
    {
    // ReaderStatus byte (see /6/ GSM 11.14 section 12.33)
    // Bits
    // 1-3  Identity of card reader
    // 4    0=Card reader is not removable
    //      1=Card reader is removable
    // 5    0=Card reader is not present
    //      1=Card reader is present
    // 6    0=Card reader present is not ID-1 size
    //      1=Card reader present is ID-1 size
    // 7    0=No card present
    //      1=Card is present in reader
    // 8    0=No card powered
    //      1=Card in reader is powered

    return ( iSimCardReaderStatus >> 3 ) & 0x01;
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TSimCardReaderStatus::IsCardReaderPresent
// Is card reader present
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool RMmCustomAPI::TSimCardReaderStatus::IsCardReaderPresent()
    {
    // ReaderStatus byte (see /6/ GSM 11.14 section 12.33)
    // Bits
    // 1-3  Identity of card reader
    // 4    0=Card reader is not removable
    //      1=Card reader is removable
    // 5    0=Card reader is not present
    //      1=Card reader is present
    // 6    0=Card reader present is not ID-1 size
    //      1=Card reader present is ID-1 size
    // 7    0=No card present
    //      1=Card is present in reader
    // 8    0=No card powered
    //      1=Card in reader is powered

    return ( iSimCardReaderStatus  >> 4 ) & 0x01;
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TSimCardReaderStatus::IsCardPowered
// Is card powered
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool RMmCustomAPI::TSimCardReaderStatus::IsCardPowered()
    {
    // ReaderStatus byte (see /6/ GSM 11.14 section 12.33)
    // Bits
    // 1-3  Identity of card reader
    // 4    0=Card reader is not removable
    //      1=Card reader is removable
    // 5    0=Card reader is not present
    //      1=Card reader is present
    // 6    0=Card reader present is not ID-1 size
    //      1=Card reader present is ID-1 size
    // 7    0=No card present
    //      1=Card is present in reader
    // 8    0=No card powered
    //      1=Card in reader is powered

    return ( iSimCardReaderStatus  >> 6 ) & 0x01;
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TSimCardReaderStatus::IsCardPresent
// Is card present
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool RMmCustomAPI::TSimCardReaderStatus::IsCardPresent()
    {
    // ReaderStatus byte (see /6/ GSM 11.14 section 12.33)
    // Bits
    // 1-3  Identity of card reader
    // 4    0=Card reader is not removable
    //      1=Card reader is removable
    // 5    0=Card reader is not present
    //      1=Card reader is present
    // 6    0=Card reader present is not ID-1 size
    //      1=Card reader present is ID-1 size
    // 7    0=No card present
    //      1=Card is present in reader
    // 8    0=No card powered
    //      1=Card in reader is powered

    return ( iSimCardReaderStatus >> 7 ) & 0x01;
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TSimCardReaderStatus::SetStatus
// Set card status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::TSimCardReaderStatus::SetStatus(
    TUint8 newStatus )
    {
    iSimCardReaderStatus = newStatus;
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::TSimCardReaderStatus::GetStatus
// Get card status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 RMmCustomAPI::TSimCardReaderStatus::GetStatus()
    {
    return iSimCardReaderStatus;
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SendAPDUReq
// Send APDU to APDU server asynchronously
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SendAPDUReq(
    TRequestStatus& aStatus,
    TApduParameters& aParams )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotSendAPDUReqV2Status, aParams );
    Get( ECustomSendAPDUReqV2IPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TSimAuthenticationEapSim
// TSimAuthenticationEapSim constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C RMmCustomAPI::TSimAuthenticationEapSim::TSimAuthenticationEapSim()
    {
    iRandomParameters.Zero();
    iSRES.Zero();
    iKC.Zero();

    iExtensionId = EEapSim;
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TSimAuthenticationEapAka
// TSimAuthenticationEapAka constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C RMmCustomAPI::TSimAuthenticationEapAka::TSimAuthenticationEapAka()
    {
    iRandomParameters.Zero();
    iAUTN.Zero();
    iRES.Zero();
    iCK.Zero();
    iIK.Zero();
    iAUTS.Zero();

    iExtensionId = EEapAka;
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TSimAuthenticationGbaBootstrap
// TSimAuthenticationGbaBootstrap constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C 
    RMmCustomAPI::TSimAuthenticationGbaBootstrap::
        TSimAuthenticationGbaBootstrap()
    {
    iExtensionId = EGbaBootstrap;
    iRandomParameters.Zero();
    iAUTN.Zero();
    iRES.Zero();
    iAUTS.Zero();
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TSimAuthenticationGbaBootstrapUpdate
// TSimAuthenticationGbaBootstrapUpdate constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C 
    RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate::
        TSimAuthenticationGbaBootstrapUpdate()
    {
    iExtensionId = EGbaBootstrapUpdate;
    iRandomParameters.Zero();
    iBTid.Zero();
    iKeyLifeTime.Zero();
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TSimAuthenticationGbaNafDerivation
// TSimAuthenticationGbaNafDerivation constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C 
    RMmCustomAPI::TSimAuthenticationGbaNafDerivation::
        TSimAuthenticationGbaNafDerivation()
    {
    iExtensionId =  EGbaBootstrapNafDerivation;
    iNafId.Zero();
    iImpi.Zero();
    iKsExtNaf.Zero();
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TSimAuthenticationMgvMskUpdate
// TSimAuthenticationMgvMskUpdate constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C 
    RMmCustomAPI::TSimAuthenticationMgvMskUpdate::
        TSimAuthenticationMgvMskUpdate()
    {
    iExtensionId = EMgvMskUpdate;
    iMikey.Zero();
    // Smartcard support for MobileTV
    iBCASTManagement.Zero();
    iParentalRating.Zero();
    iSecurityPolicyExt.Zero();
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TSimAuthenticationMgvMskGeneration
// TSimAuthenticationMgvMskGeneration constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C 
    RMmCustomAPI::TSimAuthenticationMgvMtkGeneration::
        TSimAuthenticationMgvMtkGeneration()
    {
    iExtensionId = EMgvMtkGeneration;
    iMikey.Zero();
    iMtkSalt.Zero();
    // Smartcard support for MobileTV
    iBCASTManagement.Zero();
    iParentalControl.Zero();
    iTrafficEncryptionKey.Zero();
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TSimAuthenticationMgvMskDeletion
// TSimAuthenticationMgvMskDeletion constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C 
    RMmCustomAPI::TSimAuthenticationMgvMskDeletion::
        TSimAuthenticationMgvMskDeletion()
    {
    iExtensionId = EMgvMskDeletion;
    iKeyDomainId.Zero();
    iKeyGroupIdPart.Zero();
    }   

// ---------------------------------------------------------------------------
// RMmCustomApi::GetWlanSimAuthenticationData
// Gets WLAN authentication data for EAP SIM or EAP AKA authentication
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetWlanSimAuthenticationData(
    TRequestStatus& aReqStatus,
    TDes8& aAuthenticationData ) const
    {
    __ASSERT_ALWAYS ( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    Get( ECustomGetSimAuthenticationDataIPC, aReqStatus,
        aAuthenticationData );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SetSimMessageStatusRead
// Sets "storage status" byte of a SIM-stored SMS message to "read".
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SetSimMessageStatusRead(
    TTime& aTime,
    TInt& aTimezoneDiff ) const
    {
    TPtr8 ptr1( REINTERPRET_CAST( TText8*, &aTime ),
        sizeof( TTime ), sizeof( TTime ) );
    TPtr8 ptr2( REINTERPRET_CAST( TText8*, &aTimezoneDiff ),
        sizeof( TInt ), sizeof( TInt ) );
    Set( ECustomSetSimMessageStatusReadIPC, ptr1, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
// This method writes specified VIAG Home Zone UHZIUE settings synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::WriteViagHomeZoneUHZIUESettings (
    TViagUHZIUESettings& aSettings ) const
    {
    TPtr8 ptr( REINTERPRET_CAST( TText8*, CONST_CAST(
        TViagUHZIUESettings*, &aSettings ) ), sizeof( TViagUHZIUESettings ),
            sizeof( TViagUHZIUESettings ) );

    return Set( EWriteViagHomeZoneUHZIUESettingsIPC, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
// This method writes specified VIAG Home Zone UHZIUE settings asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::WriteViagHomeZoneUHZIUESettings (
    TRequestStatus& aStatus,
    TViagUHZIUESettings& aSettings ) const
    {
    TPtr8& ptr = iCustomPtrHolder->Set(
        ESlotWriteViagUHZIUESettings, aSettings );

    Set( EWriteViagHomeZoneUHZIUESettingsIPC, aStatus, ptr );
    }

//----------------------------------------------------------------------------
// RMmCustomAPI::SetAlwaysOn
// (other items were commented in a header).
//----------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SetAlwaysOn(
    TRequestStatus& aStatus,
    TSetAlwaysOnMode& aMode ) const
    {

    TPtr8& ptr = iCustomPtrHolder->Set(
        ESlotSetAlwaysOn, aMode );

    Set( ECustomSetAlwaysOnMode, aStatus , ptr );
    }

//----------------------------------------------------------------------------
// RMmCustomAPI::NotifyRauEvent
// (other items were commented in a header).
//----------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyRauEvent(
    TRequestStatus& aStatus,
    TRauEventStatus& aEventStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifyRauEvent, aEventStatus );
    Get( ECustomNotifyRauEventIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::ReadHSxPAStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::ReadHSxPAStatus(
    TRequestStatus& aStatus,
    THSxPAStatus& aHSxPAStatus )
    {
    __ASSERT_ALWAYS ( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotReadHSxPAStatus, aHSxPAStatus );
    Get( ECustomReadHSxPAStatusIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::WriteHSxPAStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::WriteHSxPAStatus(
    TRequestStatus& aStatus,
    THSxPAStatus& aHSxPAStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL,
        PanicClient( EEtelPanicNullHandle ) );

    iCustomPtrHolder->iHSxPAStatus = aHSxPAStatus;
    TPtr8& ptr = iCustomPtrHolder->Set( ESlotWriteHSxPAStatus,
        iCustomPtrHolder->iHSxPAStatus );
    Set( ECustomWriteHSxPAStatusIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyHSxPAStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyHSxPAStatus(
    TRequestStatus& aStatus,
    THSxPAStatus& aHSxPAStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotNotifyHSxPAStatus,
        aHSxPAStatus );
    Get( ECustomNotifyHSxPAStatusIPC, aStatus, ptr );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetIccCallForwardingIndicatorStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetIccCallForwardingIndicatorStatus(
    TRequestStatus& aReqStatus,
    TDes8& aCFIndicators ) const
  {
  __ASSERT_ALWAYS ( iCustomPtrHolder != NULL, PanicClient(
      EEtelPanicNullHandle ) );

    Get( ECustomGetIccCallForwardingStatusIPC, aReqStatus, aCFIndicators );
  }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyIccCallForwardingStatusChange
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyIccCallForwardingStatusChange(
    TRequestStatus& aReqStatus,
    TDes8& aCFIndicators )
  {
  __ASSERT_ALWAYS ( iCustomPtrHolder != NULL, PanicClient(
      EEtelPanicNullHandle ) );

    Get( ECustomNotifyIccCallForwardingStatusChangeIPC, aReqStatus,
        aCFIndicators );
  }

// ---------------------------------------------------------------------------
// RMmCustomApi::TCFIndicators
// TCFIndicators constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C RMmCustomAPI::TCFIndicators::TCFIndicators()
    {
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetCellInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetCellInfo (
    TRequestStatus& aStatus,
    TDes8& aCellInfo ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    Get( ECustomGetCellInfoIPC, aStatus, aCellInfo );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::NotifyCellInfoChange
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyCellInfoChange(
    TRequestStatus& aStatus,
    TDes8& aCellInfo ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    Get( ECustomNotifyCellInfoChangeIPC, aStatus, aCellInfo );
    }

// ---------------------------------------------------------------------------
// RMmCustomApi::TMmCellInfo::TMmCellInfo
// Default constructor for TMmCellInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C RMmCustomAPI::TMmCellInfo::TMmCellInfo()
    {
    // Initialize GsmCellInfo values
    iMode = RMmCustomAPI::TMmCellInfo::EUnknown;
    iGsmCellInfo.iTA = -1;
    for( TInt i = 0; i <KMaxNmrAmount; i++ )
        {
        iGsmCellInfo.iNmr[i].iARFCN = -1;
        iGsmCellInfo.iNmr[i].iBSIC = -1;
        iGsmCellInfo.iNmr[i].iRxLEV = -1;
        }

    // Initialize WcdmaCellInfo values
    iWcdmaCellInfo.iFrequencyInfo.iFddUL = -1;
    iWcdmaCellInfo.iFrequencyInfo.iFddDL = -1;
    iWcdmaCellInfo.iFrequencyInfo.iTddNt = -1;
    iWcdmaCellInfo.iPrimaryScrambilingCode = -1;

    for( TInt j = 0; j <KMaxNetworkMeasureReports; j++ )
        {
        //Initialize TNetworkMeasureReport values
        iWcdmaCellInfo.iNwkMeasureReport[j].iCarrierRSSI = -1;
        iWcdmaCellInfo.iNwkMeasureReport[j].iFrequencyInfo.iFddDL = -1;
    	iWcdmaCellInfo.iNwkMeasureReport[j].iFrequencyInfo.iFddUL = -1;
		iWcdmaCellInfo.iNwkMeasureReport[j].iFrequencyInfo.iTddNt = -1;

        for( TInt k = 0; k <KMaxCellMeasuredResults; k++ )
            {
            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iCID = -1;
            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iFddInfo.iPrimaryCPICH = -1;
            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iFddInfo.iCpichEcN0 = -1;
            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iFddInfo.iCpichRscp = -1;
            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iFddInfo.iPathloss = -1;

            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iTddInfo.iCellParamID = -1;
            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iTddInfo.iProposedTGSN = -1;
            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iTddInfo.iPrimaryCcpchRscp = -1;
            iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                iTddInfo.iPathloss = -1;
            for( TInt l = 0; l <KMaxTimeSlotIscpAmount; l++ )
                {
                iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].
                    iTddInfo.iTimeslotISCP[l] = -1;
                }
            }
        }
    }
    
// ---------------------------------------------------------------------------
// RMmCustomAPI::GetSystemNetworkBand
// This method reads network system band synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::GetSystemNetworkBand(
    TBandSelection& aSupportedNetworkBand,
    TNetworkModeCaps& aNetworkMode ) const
    {
    TPtr8 ptr1(REINTERPRET_CAST( TText8*, CONST_CAST(
    TBandSelection*, &aSupportedNetworkBand ) ), sizeof( TBandSelection ),
        sizeof( TBandSelection ) );
    TPtr8 ptr2(REINTERPRET_CAST( TText8*, CONST_CAST(
    TNetworkModeCaps*, &aNetworkMode ) ), sizeof( TNetworkModeCaps ),
        sizeof( TNetworkModeCaps ) );

    return Get( ECustomGetBandSelectionIPC, ptr1, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetSystemNetworkBand
// This method reads network system band asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetSystemNetworkBand(
    TRequestStatus& aStatus,
    TBandSelection& aSupportedNetworkBand,
    TNetworkModeCaps& aNetworkMode ) const
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iSupportedNetworkBand = aSupportedNetworkBand;
    TPtr8& ptr1 = iCustomPtrHolder->Set(
        ESlotGetSystemNetworkBand, aSupportedNetworkBand );
    iCustomPtrHolder->iNetworkModeCaps = aNetworkMode;
    TPtr8& ptr2 = iCustomPtrHolder->Set(
        ESlotSetSystemNetworkMode, aNetworkMode );

    Get( ECustomGetBandSelectionIPC, aStatus, ptr1, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::SetSystemNetworkBand
// This method sets network system band synchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMmCustomAPI::SetSystemNetworkBand(
    const TBandSelection aNetworkBand,
    const TNetworkModeCaps aNetworkMode )
    {
    TPtr8 ptr1( REINTERPRET_CAST( TText8*, CONST_CAST(
    TBandSelection*, &aNetworkBand ) ),
        sizeof( TBandSelection ), sizeof( TBandSelection ) );
        
    TPtr8 ptr2( REINTERPRET_CAST( TText8*, CONST_CAST(
    TNetworkModeCaps*, &aNetworkMode ) ),
        sizeof( TNetworkModeCaps ), sizeof( TNetworkModeCaps ) );

    return Set( ECustomSetBandSelectionIPC, ptr1, ptr2 );
    }
    
// ---------------------------------------------------------------------------
// RMmCustomAPI::SetSystemNetworkBand
// This method sets network system band asynchronously.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::SetSystemNetworkBand(
    TRequestStatus& aStatus,
    const TBandSelection aNetworkBand,
    const TNetworkModeCaps aNetworkMode )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    iCustomPtrHolder->iNetworkBand = aNetworkBand;
    TPtr8& ptr1 = iCustomPtrHolder->Set(
        ESlotSetSystemNetworkBand, iCustomPtrHolder->iNetworkBand );
        
    iCustomPtrHolder->iNetworkModeCaps = aNetworkMode;
    TPtr8& ptr2 = iCustomPtrHolder->Set(
        ESlotSetSystemNetworkMode, iCustomPtrHolder->iNetworkModeCaps );
       
    Set( ECustomSetBandSelectionIPC, aStatus, ptr1, ptr2 );
    }

// ---------------------------------------------------------------------------
// RMmCustomAPI::GetUSIMServiceSupport
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::GetUSIMServiceSupport( 
	TRequestStatus& aStatus, 
	TAppSupport& aParams) const
    {
    __ASSERT_ALWAYS ( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );

    TPtr8& ptr = iCustomPtrHolder->Set( ESlotGetUSIMServiceSupport, aParams );
    Get( ECustomGetServiceTableSupportbyApplicationIPC, aStatus, ptr );

    }
    
//----------------------------------------------------------------------------
// RMmCustomAPI::NotifyRemoteAlertingToneStatusChange
// (other items were commented in a header).
//----------------------------------------------------------------------------
//
EXPORT_C void RMmCustomAPI::NotifyRemoteAlertingToneStatusChange(
    TRequestStatus& aStatus,
    TRemoteAlertingToneStatus& aToneStatus )
    {
    __ASSERT_ALWAYS( iCustomPtrHolder != NULL, PanicClient(
        EEtelPanicNullHandle ) );
    TPtr8& ptr = iCustomPtrHolder->Set( 
        ESlotNotifyRemoteAlertingToneStatusChange, aToneStatus );
    Get( ECustomNotifyRemoteAlertingToneStatusChangeIPC, aStatus, ptr );
    }

//  End of File
