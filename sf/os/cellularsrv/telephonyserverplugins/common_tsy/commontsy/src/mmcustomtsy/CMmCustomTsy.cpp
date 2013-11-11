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
#include "CMmCustomTsyTraces.h"
#endif

#include "CMmCustomTsy.h"
#include "CMmCustomGsmExt.h"
#include "cmmcalllist.h"
#include "cmmphoneextinterface.h"
#include "CMmWimTsy.h"
#include "cmmcalltsy.h"
#include "CMmCommonStaticUtility.h"
#include "CMmSimLockTsy.h"
#include "MmTsy_conf.h"
#include "cmmpblist.h"
#include "cmmphonebookstoretsy.h"
#include "CMmPacketTsy.h"
#include "CMmSIMTsy.h"
#include "CMmCustomSecurityTsy.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/cmmcustomvendorext.h>
#include "cmmnettsy.h"  // for CMmNetTsy::CipheringIndicatorForcedOff()
#include <ctsy/serviceapi/ctsydomaincrkeys.h>
#include "ctsyprivatecrpkeys.h"
#include <featureuids.h>

// ======== MEMBER FUNCTIONS ========

CMmCustomTsy::CMmCustomTsy():
    iReqHandleType(ECustomTsyReqHandleUnknown)
    {
    iMmPhoneTsy = NULL;
    iMmCustomExtInterface = NULL;
    Mem::FillZ( iMmSubTsy, sizeof( iMmSubTsy ) );

    // initialize internal data
    iCipheringInfo.iIndStatus = EFalse;
    iCipheringInfo.iCiphStatus = EFalse;
    }

void CMmCustomTsy::ConstructL(
    CMmPhoneTsy* aMmPhoneTsy )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CONSTRUCTL_1, "TSY: CMmCustomTsy::ConstructL");
    iMmPhoneTsy = aMmPhoneTsy;

    iMmCustomExtInterface = CMmCustomGsmExt::NewL( aMmPhoneTsy, this );
    iMmSubTsy[ESubTsyIdxWIM] = CMmWimTsy::NewL( this );
    iMmSubTsy[ESubTsyIdxSimLock] = CMmSimLockTsy::NewL( this );
    iMmSubTsy[ESubTsyIdxPacket] = CMmPacketTsy::NewL( this, aMmPhoneTsy );

    iMmSubTsy[ESubTsyIdxSIM]  = CMmSIMTsy::NewL( this );

    iMmSecurityTsy = CMmCustomSecurityTsy::NewL( this, aMmPhoneTsy );



#ifdef REQHANDLE_TIMER
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this, iMmPhoneTsy,
        ECustomTsyMaxNumOfRequests, iCustomReqHandles );
#else
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(
        ECustomTsyMaxNumOfRequests, iCustomReqHandles );
#endif

    // register custom tsy in the message manager
    Phone()->MessageManager()->RegisterTsyObject(
        CMmMessageManagerBase::ECustomTsy, this );

    iAdnCacheStatus = RMmCustomAPI::ECacheNotReady;
    iFdnCacheStatus = RMmCustomAPI::ECacheNotReady;

    iISVDialNumberCheckObject = NULL;
    iISVDialNumberCheck = EFalse;
    iGetIccCfStatusBootUp = ETrue;
    //Get pointer to public CenRep
    iCFISCentRep = iMmPhoneTsy->GetCfisCenRep();
    iCFISPrivateCentRep = iMmPhoneTsy->GetCfisPRCenRep();
	TInt err = iFeatureControl.Open();
	iUsingFeatureManager = (err == KErrNone);
    
    // update/receive Icc Call forward information
    if( iMmPhoneTsy->NosBootState()->iSIMReady )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CONSTRUCTL_2, "TSY: CMmCustomTsy::ConstructL -- GET ICC call forward indicators");
        // Get Icc cfis values from LTSY
        Phone()->MessageManager()->HandleRequestL( 
            ECustomGetIccCallForwardingStatusIPC );
        }
        
    // check if HomeZoneParams has been already read in NotifySimStatusReady
 	// if not, query again
    if ( !iMmPhoneTsy->GetHomeZoneParamsChecked() )
    	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CONSTRUCTL_3, "TSY: CMmCustomTsy::ConstructL - GetHomeZoneParamsChecked()");
        CMmSIMTsy* simCustomTsy = NULL;
        simCustomTsy = ( CMmSIMTsy* )GetSIMTsyPtr();
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CONSTRUCTL_4, "TSY: CMmCustomTsy::ConstructL - pointer created");
	
        if ( simCustomTsy )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CONSTRUCTL_5, "TSY: CMmCustomTsy::ConstructL - CheckViagHomeZoneParamsL()");
            simCustomTsy->CheckViagHomezoneParamsL();
            iMmPhoneTsy->SetHomeZoneParamsChecked( ETrue );
            }
        else
        	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CONSTRUCTL_6, "TSY: CMmCustomTsy::ConstructL - CheckViagHomeZoneParamsL() False");
			iMmPhoneTsy->SetHomeZoneParamsChecked( EFalse );
        	}
    	}     
    }

CMmCustomTsy* CMmCustomTsy::NewL(
    CMmPhoneTsy* aMmPhoneTsy )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_NEWL_1, "TSY: CMmCustomTsy::NewL");
    CMmCustomTsy* mmCustomTsy = new ( ELeave ) CMmCustomTsy();
    CleanupClosePushL( *mmCustomTsy );
    mmCustomTsy->ConstructL( aMmPhoneTsy );
    CleanupStack::Pop( mmCustomTsy );

    return mmCustomTsy;
    }

CMmCustomTsy::~CMmCustomTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_DTOR_1, "TSY: CMmCustomTsy::~CMmCustomTsy");

	iFeatureControl.Close();
	
    // Delete subsystems
    for ( TInt i = 0; ESubTsyIdxMaxNum > i; i++ )
        {
        delete iMmSubTsy[i];
        }

    // Delete SecurityTsy
    delete iMmSecurityTsy;

    // Delete GSM extensions
    delete iMmCustomExtInterface;
    
    // delete req handle store
    delete iTsyReqHandleStore;

    if ( iMmPhoneTsy )
        {
        iMmPhoneTsy->SetHomeZoneParamsChecked( EFalse );
        iMmPhoneTsy->SetCustomTsy( NULL );
        // unregister custom tsy in the message manager
        iMmPhoneTsy->MessageManager()->DeregisterTsyObject(this);
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::Phone
// Returns pointer to PhoneTsy object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneTsy* CMmCustomTsy::Phone()
    {
    return iMmPhoneTsy;
    }


// ---------------------------------------------------------------------------
// CMmCustomTsy::ExtFunc
// Dispatches extension function requests. All functions that are not part
// of the core function set, are routed via the ExtFunc method.
// At the beginning of this method, packaged pointers are unpacked. Then
// the correct function is selected using the IPC number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::ExtFunc(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
    TInt ret = KErrNone;
    TInt trapError = KErrNone;
    
    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==ECustomTsyReqHandleUnknown, User::Invariant());

    // before processing further the request, check if offline mode status
    // is enabled and if the given request can be perfomed in that case.
    if ( ERfsStateInfoInactive == iMmPhoneTsy->GetRfStateInfo() &&
        !IsRequestPossibleInOffline( aIpc ) )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_EXTFUNC_1, "TSY: Offline mode ON, request is not allowed: %{TIPCNamesList}", aIpc );
        ret = CMmCommonStaticUtility::EpocErrorCode( KErrGeneral,
                KErrGsmOfflineOpNotAllowed );

        // Complete the request with appropiate error
        ReqCompleted ( aTsyReqHandle, ret );
        }
    else
        {
        TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ); );

        if ( KErrNone != trapError )
            {
            // error handling. Object cannot be created.
            ResetReqHandle(aTsyReqHandle, aIpc);
		    ret = trapError;            
			}
      	if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::DoExtFuncL
// This DoExtFuncL is called by Etel server when it has Custom API
// request for the TSY. To process a request handle, request type and request
// data are passed to the Custom TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_DOEXTFUNCL_1, "TSY: CMmCustomTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", aIpc, (TUint)aTsyReqHandle, (TUint)this);
    TInt ret( KErrNotSupported );

    // first check if request is supported in CMmSecurityTsy
    // (due layering that class is no longer a part of extension modules)

    ret = iMmSecurityTsy->DoExtFuncL( aTsyReqHandle, aIpc, aPackage );

    if ( KErrNotSupported == ret )
        {
 
        // then check all the Custom extension modules
        ret = CMmTsyBase::DoBaseExtFuncL( aTsyReqHandle, aIpc, aPackage );

        // if extension modules did not serve this request
        if ( KErrNotSupported == ret )
            {
            // Ensure the ReqHandleType is unset.
            // This will detect cases where this method indirectly calls itself
            // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
            // Such cases are not supported because iReqHandleType is in the context of this class instance,
            // not this request, and we don't want the values set by the inner request and the outer request
            // interfering with each other.
            __ASSERT_DEBUG(iReqHandleType==ECustomTsyReqHandleUnknown, User::Invariant());
            
            switch ( aIpc )
                {
                // Perform IMS Authentication
                case EMobilePhoneIMSAuthenticate:
                    ret = ImsAuthenticationL( aTsyReqHandle,
                        REINTERPRET_CAST(
                        RMobilePhone::TImsAuthenticateDataV5*,
                        aPackage.Ptr1() ) );
                    break;
                // Get sim authentication data
                case ECustomGetSimAuthenticationDataIPC:
                    ret = SimAuthenticationL( aTsyReqHandle, aPackage );
                    break;
                // Notify Dtmf event
                case ECustomNotifyDtmfEventIPC:
                    ret = NotifyDtmfEvent(
                        REINTERPRET_CAST( RMmCustomAPI::TDtmfInfo*,
                        aPackage.Ptr1() ) );
                    break;
                // Get Diagnostic Octets - info 
                case ECustomGetDiagnosticOctetsIPC:
                   ret = GetDiagnosticInfo ( aTsyReqHandle,
                       REINTERPRET_CAST( TName*, aPackage.Ptr1() ) );
                   break;
                // Get Remote Alerting Tone Status
                case ECustomGetRemoteAlertingToneStatusIPC:
                    ret = GetRemoteAlertingToneStatus ( aTsyReqHandle,
                        REINTERPRET_CAST(
                        RMmCustomAPI::TRemoteAlertingToneStatus*,
                        aPackage.Ptr1() ) );
                    break;
                // Call Origin
                case ECustomCallOriginIPC:
                    ret = GetCallOrigin( aTsyReqHandle,
                        REINTERPRET_CAST( TName*, aPackage.Ptr1() ),
                        REINTERPRET_CAST( RMmCustomAPI::TCallOrigin*,
                        aPackage.Ptr2() ) );
                    break;
                // Terminate a Call/all calls
                case ECustomTerminateCallIPC:
                    ret = TerminateCallL( aTsyReqHandle,
                        REINTERPRET_CAST( TName*, aPackage.Ptr1() ) );
                    break;
                // Get Als Blocked
                case ECustomGetAlsBlockedIPC:
                    ret = GetAlsBlockedL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TGetAlsBlockStatus*,
                        aPackage.Ptr1() ) );
                    break;
                // Set Alternative Line Service Blocked
                case ECustomSetAlsBlockedIPC:
                    ret = SetAlsBlockedL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TSetAlsBlock*,
                        aPackage.Ptr1() ) );
                    break;
                // Notify Als Blocked Change
                case ECustomNotifyAlsBlockedChangedIPC:
                    ret = NotifyAlsBlockedChanged (
                        REINTERPRET_CAST( RMmCustomAPI::TGetAlsBlockStatus*,
                        aPackage.Ptr1() ) );
                    break;
                // Get ALS pp support
                case ECustomCheckAlsPpSupportIPC:
                    ret = GetAlsPpSupportL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TAlsSupport*,
                                          aPackage.Ptr1() ) );
                    break;
                // Get ciphering information
                case ECustomGetCipheringInfoIPC:
                    ret = GetCipheringInfoL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TCipheringInfo*,
                        aPackage.Ptr1() ) );
                    break;
                // Notify ciphering information change
                case ECustomNotifyCipheringInfoChangeIPC:
                    ret = NotifyCipheringInfoChange(
                        REINTERPRET_CAST(RMmCustomAPI::TCipheringInfo*,
                        aPackage.Ptr1() ) );
                    break;
                // Notify NSPS status
                case ECustomNotifyNSPSStatusIPC:
                    ret = NotifyNSPSStatus (
                         REINTERPRET_CAST(RMmCustomAPI::TNspsStatus*,
                         aPackage.Ptr1() ) );
                    break;
                // Wake up network from NSPS status
                case ECustomNetWakeupIPC:
                    ret = NetWakeupL( aTsyReqHandle );
                    break;
                case ECustomClearCallBlackListIPC:
                    ret = ClearCallBlackListL ( aTsyReqHandle );
                    break;
                // SS additional info notification
                case ECustomSsAdditionalInfoNotificationIPC:
                     ret = SsAdditionalInfoNotification(
                        REINTERPRET_CAST( RMmCustomAPI::TSsAdditionalInfo*,
                        aPackage.Ptr1() ) );
                     break;
                // Get the air time duration
                case ECustomGetAirTimeDurationIPC:
                    ret = GetAirTimeDuration ( aTsyReqHandle,
                        REINTERPRET_CAST( TTimeIntervalSeconds*,
                        aPackage.Ptr1() ) );
                    break;
                // Check emergency number
                case ECustomCheckEmergencyNumberIPC:
                    ret = CheckEmergencyNumberL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TEmerNumberCheckMode*,
                            aPackage.Ptr1() ),
                        REINTERPRET_CAST( TBool*, aPackage.Ptr2() ) );
                    break;
                // Get Network Operator Name
                case ECustomGetNetworkProviderNameIPC:
                    ret = GetNetworkOperatorNameL( aPackage.Des1u() );
                    break;
                // Get two digit dial support
                case ECustomCheckTwoDigitDialSupportIPC:
                    ret = CheckTwoDigitDialSupportL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TTwoDigitDialSupport*,
                        aPackage.Ptr1() ) );
                    break;
                // Notify when phonebook cache is ready
                case ECustomNotifyPndCacheReadyIPC:
                    ret = NotifyPhoneBookCacheReady( aTsyReqHandle,
                        REINTERPRET_CAST( TName*, aPackage.Ptr1() ) );
                    break;
                // Get phonebook cache status
                case ECustomGetPndCacheStatusIPC:
                    ret = GetPndCacheStatus( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TPndCacheStatus*,
                            aPackage.Ptr1() ),
                        REINTERPRET_CAST( const TName*, aPackage.Des2u() ) );
                    break;
                // Get the programmable operator name
                case ECustomGetOperatorNameIPC:
                    ret = GetOperatorNameL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TOperatorNameInfo*,
                        aPackage.Ptr1() ) );
                    break;
                // Notify SS Network event
                case ECustomNotifySsNetworkEventIPC:
                    ret = NotifySsNetworkEvent(
                        REINTERPRET_CAST( RMmCustomAPI::TSsTypeAndMode*,
                        aPackage.Ptr1() ),
                        REINTERPRET_CAST( RMmCustomAPI::TSsInfo*,
                        aPackage.Ptr2() ) );
                    break;
                // Get the programmable operator logo
                case ECustomGetProgrammableOperatorLogoIPC:
                    ret = GetProgrammableOperatorLogoL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TOperatorId*,
                        aPackage.Ptr1() ),
                        REINTERPRET_CAST( RMmCustomAPI::TOperatorLogo*,
                        aPackage.Ptr2() ) );
                    break;
                // Sat refresh complete notification
                case ECustomSatRefreshCompleteNotificationIPC:
                    ret = SatRefreshCompleteNotification();
                    break;
                // Reset the net server
                case ECustomResetNetServerIPC:
                    ret = ResetNetServerL( aTsyReqHandle );
                    break;
                // Notify Network Connection Failure
                case ECustomNotifyNetworkConnectionFailureIPC:
                    ret = NotifyNetworkConnectionFailure( );
                    break;
                // Get supported network modes
                case ECustomGetSystemNetworkModesIPC:
                    ret = GetSystemNetworkModesL( aTsyReqHandle,
                        REINTERPRET_CAST(TUint32*, aPackage.Ptr1()) );
                    break;
                // Set system network mode
                case ECustomSetSystemNetworkModeIPC:
                    ret = SetSystemNetworkModeL( aTsyReqHandle,
                        REINTERPRET_CAST(RMmCustomAPI::TNetworkModeCaps*,
                        aPackage.Ptr1()) );
                    break;
                // Get current system network mode
                case ECustomGetCurrentSystemNetworkModesIPC:
                    ret = GetCurrentSystemNetworkModeL( aTsyReqHandle,
                        REINTERPRET_CAST(TUint32*, aPackage.Ptr1()) );
                    break;
                // Get supported network bands
                case ECustomGetBandSelectionIPC:
                    ret = GetSystemNetworkBandL( aTsyReqHandle,
                        REINTERPRET_CAST(RMmCustomAPI::TBandSelection*,
                        aPackage.Ptr1()),
                        REINTERPRET_CAST(RMmCustomAPI::TNetworkModeCaps*,
                        aPackage.Ptr2()) );
                    break;
                // Set system network band
                case ECustomSetBandSelectionIPC:
                    ret = SetSystemNetworkBandL( aTsyReqHandle,
                        REINTERPRET_CAST(RMmCustomAPI::TBandSelection*,
                        aPackage.Ptr1() ),
                        REINTERPRET_CAST(RMmCustomAPI::TNetworkModeCaps*,
                        aPackage.Ptr2() ) );
                    break;
                // Notify SS request complete
                case ECustomNotifySsRequestCompleteIPC:
                    ret = SsRequestCompleteNotification(
                         REINTERPRET_CAST( TInt*, aPackage.Ptr1() ) );
                    break;
                case ECustomGetLifeTimeIPC:
                    ret = GetLifeTimeL( aTsyReqHandle, aPackage.Des1n() );
                    break;
                case ECustomReadSimFileIPC:
                    ret = GetSimFileInfoL(aTsyReqHandle, aPackage.Des1n(),
                        aPackage.Des2n());
                    break;
                // Get 3G phonebook info
                case EGet3GPBInfoIPC:
                    ret = Get3GPBInfo ( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::T3GPBInfo*,
                        aPackage.Ptr1() ) );
                    break;
                case ECustomSetDriveModeIPC:
                    ret = SetDriveModeL( aTsyReqHandle,
                        REINTERPRET_CAST( RMmCustomAPI::TSetDriveMode*,
                        aPackage.Ptr1() ) );
                    break;
                case ECustomSetSimMessageStatusReadIPC:
                    ret = SetSimMessageStatusReadL( aTsyReqHandle,
                        *( REINTERPRET_CAST(TTime*, aPackage.Ptr1() ) ),
                        *( REINTERPRET_CAST(TInt*, aPackage.Ptr2() ) ) );
                    break;
                case ECustomNotifyRauEventIPC:
                    ret = NotifyRauEvent(
                        REINTERPRET_CAST( RMmCustomAPI::TRauEventStatus*,
                        aPackage.Ptr1() ) );
                    break;
                case ECustomReadHSxPAStatusIPC:
                    ret = ReadHSxPAStatusL( aTsyReqHandle, REINTERPRET_CAST(
                        RMmCustomAPI::THSxPAStatus*, aPackage.Ptr1() ) );
                    break;
                case ECustomWriteHSxPAStatusIPC:
                    ret = WriteHSxPAStatusL( aTsyReqHandle, REINTERPRET_CAST(
                        RMmCustomAPI::THSxPAStatus*, aPackage.Ptr1() ) );
                    break;
                case ECustomNotifyHSxPAStatusIPC:
                    ret = NotifyHSxPAStatus( REINTERPRET_CAST(
                        RMmCustomAPI::THSxPAStatus*, aPackage.Ptr1() ) );
                    break;
                case ECustomGetIccCallForwardingStatusIPC:
                	ret = GetIccCallForwardingStatusL( aTsyReqHandle,
                    	aPackage.Des1n() );
                	break;
                case ECustomNotifyIccCallForwardingStatusChangeIPC:
                    ret = NotifyIccCallForwardingStatusChange(
                    	aPackage.Des1n() );
                    break;
                case ECustomNotifyCellInfoChangeIPC:
                    ret = NotifyCellInfoChange( aPackage.Des1n() );
                    break;
                case ECustomGetCellInfoIPC:
                    ret = GetCellInfoL( aPackage.Des1n() );
                    break;
                case ECustomGetServiceTableSupportbyApplicationIPC:
                    ret = GetUSIMServiceSupportL( aTsyReqHandle, 
                        REINTERPRET_CAST( RMmCustomAPI::TAppSupport*, 
                        aPackage.Ptr1() ) );
                    break;
                case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
                    ret = NotifyRemoteAlertingToneStatusChange(
                        REINTERPRET_CAST( 
                        RMmCustomAPI::TRemoteAlertingToneStatus*,
                        aPackage.Ptr1() ) );
                    break;
                default:
                    // ret is already set as KErrNotSupported
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_DOEXTFUNCL_2, "TSY: CMmCustomTsy::DoExtFuncL unsupported ipc=%{TIPCNamesList}", aIpc);
                    break;
                }

            // save request handle
            if ( ECustomTsyReqHandleUnknown != iReqHandleType )
                {
#ifdef REQHANDLE_TIMER
                // set timer for the request
                SetTypeOfResponse ( iReqHandleType, aTsyReqHandle );
#else
                iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType,
                    aTsyReqHandle );
#endif // REQHANDLE_TIMER
                // We've finished with this value now. Clear it so it doesn't leak
                //  up to any other instances of this method down the call stack
                iReqHandleType = ECustomTsyReqHandleUnknown;
                }
            }
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_DOEXTFUNCL_3, "<-- TSY: CMmCustomTsy::DoExtFuncL, error = %{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::RegisterNotification
// Called when the server recognises that this notification is being posted
// for the first time on this sub-session object. It enables the TSY to
// "turn on" any regular notification messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::RegisterNotification(
    const TInt aIpc )
    {
    TInt ret ( KErrNotSupported );

    switch( aIpc )
        {
        case ECustomNotifyRauEventIPC:
        case ECustomNotifyDtmfEventIPC:
        case ECustomNotifyAlsBlockedChangedIPC:
        case ECustomNotifyCipheringInfoChangeIPC:
        case ECustomNotifyNSPSStatusIPC:
        case ECustomSsAdditionalInfoNotificationIPC:
        case ECustomNotifySsRequestCompleteIPC:
        case ECustomNotifyPndCacheReadyIPC:
        case ECustomNotifySsNetworkEventIPC:
        case ECustomSatRefreshCompleteNotificationIPC:
        case ECustomNotifyNetworkConnectionFailureIPC:
        case ECustomNotifyHSxPAStatusIPC:
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
        case ECustomNotifyCellInfoChangeIPC:
        case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
            ret =  KErrNone;
            break;

        default:
            ret = CMmTsyBase::RegisterNotification( aIpc );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::DeregisterNotification
// Called when the server recognises that this notification will not be posted
// again because the last client to have a handle on this sub-session object
// has just closed the handle. It enables the TSY to "turn off" any regular
// notification messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::DeregisterNotification(
    const TInt aIpc )
    {
    TInt ret ( KErrNotFound );

    switch( aIpc )
        {
        case ECustomNotifyRauEventIPC:
        case ECustomNotifyDtmfEventIPC:
        case ECustomNotifyAlsBlockedChangedIPC:
        case ECustomNotifyCipheringInfoChangeIPC:
        case ECustomNotifyNSPSStatusIPC:
        case ECustomSsAdditionalInfoNotificationIPC:
        case ECustomNotifySsRequestCompleteIPC:
        case ECustomNotifyPndCacheReadyIPC:
        case ECustomNotifySsNetworkEventIPC:
        case ECustomSatRefreshCompleteNotificationIPC:
        case ECustomNotifyNetworkConnectionFailureIPC:
        case ECustomNotifyHSxPAStatusIPC:
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
        case ECustomNotifyCellInfoChangeIPC:
        case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
            ret =  KErrNone;
            break;
        default:
            ret = CMmTsyBase::DeregisterNotification( aIpc );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::ReqModeL
// Returns request mode for given IPC. If the IPC number belongs to some of
// the Custom API functions.
// Possible return values:
//     tsy has own buffering            =0x00000000  
//     KReqModeMultipleCompletionEnabled=0x00000001
//     KReqModeFlowControlObeyed=0x00000002
//     KReqModeRePostImmediately=0x00000004
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmCustomTsy::ReqModeL(
    const TInt aIpc )
    {
    CTelObject::TReqMode ret = 0;

    switch ( aIpc )
        {
        // methods using DOS
        case ECustomNetWakeupIPC:
        case ECustomGetNetworkProviderNameIPC:
        case ECustomCheckTwoDigitDialSupportIPC:
        case ECustomGetOperatorNameIPC:
        case ECustomResetNetServerIPC:
            ret = KReqModeFlowControlObeyed;
            break;
            
        //TSYs wishing to implement their own buffering can return 0 
        case ECustomSetAlsBlockedIPC:            
        case ECustomSetDriveModeIPC:            
            ret = 0;
            break;

        // Notifications
        case ECustomNotifyRauEventIPC:
        case ECustomNotifyDtmfEventIPC:
        case ECustomNotifyAlsBlockedChangedIPC:
        case ECustomNotifyCipheringInfoChangeIPC:
        case ECustomNotifyNSPSStatusIPC:
        case ECustomSsAdditionalInfoNotificationIPC:
        case ECustomNotifyPndCacheReadyIPC:
        case ECustomNotifySsNetworkEventIPC:
        case ECustomSatRefreshCompleteNotificationIPC:
        case ECustomNotifyNetworkConnectionFailureIPC:
        case ECustomNotifySsRequestCompleteIPC:
        case ECustomNotifyHSxPAStatusIPC:
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
        case ECustomNotifyCellInfoChangeIPC:
        case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
            ret = KReqModeRePostImmediately |
                  KReqModeMultipleCompletionEnabled;
            break;

        // methods that are using DOS but have own flow control system
        case ECustomGetProgrammableOperatorLogoIPC:
        case ECustomTerminateCallIPC:
        case ECustomCheckEmergencyNumberIPC:
        case ECustomCheckAlsPpSupportIPC:
        case ECustomGetAlsBlockedIPC:
        case ECustomGetCipheringInfoIPC:
        case ECustomClearCallBlackListIPC:
        case ECustomGetSystemNetworkModesIPC:
        case ECustomSetSystemNetworkModeIPC:
        case ECustomGetCurrentSystemNetworkModesIPC:
        case ECustomReadHSxPAStatusIPC:
        case ECustomWriteHSxPAStatusIPC:
        case ECustomGetIccCallForwardingStatusIPC:
        case ECustomGetCellInfoIPC:
        case ECustomGetBandSelectionIPC:
        case ECustomSetBandSelectionIPC:

        // methods not using DOS
        case ECustomCallOriginIPC:
        case ECustomGetDiagnosticOctetsIPC:
        case ECustomGetAirTimeDurationIPC:
        case ECustomGetRemoteAlertingToneStatusIPC:
        case ECustomGetPndCacheStatusIPC:
        case ECustomGetLifeTimeIPC:
        case ECustomReadSimFileIPC:
        case EGet3GPBInfoIPC:
        case EMobilePhoneIMSAuthenticate:
        case ECustomGetSimAuthenticationDataIPC:
       	case ECustomGetServiceTableSupportbyApplicationIPC:

        // from Security Tsy
        case ECustomCheckSecurityCodeIPC:
        case ECustomIsBlockedIPC:
        case ECustomDisablePhoneLockIPC:
        case ECustomGetActivePinIPC:
        case ECustomSetSimMessageStatusReadIPC:
            ret = 0;
            break;
        default:
            ret = CMmTsyBase::ReqModeL ( aIpc );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    TInt numberOfSlots ( KMmCustomDefaultSlots );

    switch ( aIpc )
        {
        case ECustomNotifyDtmfEventIPC:
            numberOfSlots = KMmCustomNotifyDtmfEventSlots;
            break;
        case ECustomNotifyRauEventIPC:
        case ECustomNotifyAlsBlockedChangedIPC:
        case ECustomNotifyCipheringInfoChangeIPC:
        case ECustomNotifyNSPSStatusIPC:
        case ECustomSsAdditionalInfoNotificationIPC:
        case ECustomNotifySsRequestCompleteIPC:
        case ECustomNotifyPndCacheReadyIPC:
        case ECustomNotifySsNetworkEventIPC:
        case ECustomSatRefreshCompleteNotificationIPC:
        case ECustomNotifyNetworkConnectionFailureIPC:
        case ECustomNotifyHSxPAStatusIPC:
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
        case ECustomNotifyCellInfoChangeIPC:
        case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
            break;

        default:
            numberOfSlots = CMmTsyBase:: NumberOfSlotsL( aIpc );
            break;
        }

    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetRequiredPlatSecCaps
// ETel server calls this method to find out capabilities of custom IPCs.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TSecurityPolicy CMmCustomTsy::GetRequiredPlatSecCaps(
    const TInt aIpc )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_1,  "TSY: CMmCustomTsy::GetRequiredPlatSecCaps ipc=%{TIPCNamesList}", aIpc );
    // assume fail as default return value
    TSecurityPolicy policy( TSecurityPolicy::EAlwaysFail );

    // deal with cancellations
    TInt ipc ( aIpc );
    if (ipc >= KIpcCustomExt + EMobileCancelOffset)
        {
        ipc  -= EMobileCancelOffset;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_2, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps cancel for ipc=%{TIPCNamesList}", ipc);
        }

    switch ( ipc )
        {
        case ECustomNotifyRauEventIPC:
        case ECustomSatRefreshCompleteNotificationIPC: // To be removed
        case ECustomNotifyPndCacheReadyIPC:
        case ECustomGetPndCacheStatusIPC:
        case EGet3GPBInfoIPC:
        case ECustomNotifyDtmfEventIPC:
        case ECustomGetRemoteAlertingToneStatusIPC:
        case ECustomCallOriginIPC:
        case ECustomGetAlsBlockedIPC:
        case ECustomNotifyAlsBlockedChangedIPC:
        case ECustomCheckAlsPpSupportIPC:
        case ECustomCheckEmergencyNumberIPC:
        case ECustomGetCipheringInfoIPC:
        case ECustomNotifyCipheringInfoChangeIPC:
        case ECustomNotifyNSPSStatusIPC:
        case ECustomNotifyProgrammableOperatorLogoChangeIPC:
        case ECustomNotifyNetworkConnectionFailureIPC:
        case ECustomNotifySsRequestCompleteIPC:
        case ECustomGetActivePinIPC:
        case ECustomGetSystemNetworkModesIPC:
        case ECustomGetCurrentSystemNetworkModesIPC:
        case ECustomGetATRIPC:
        case ECustomNotifySimCardStatusIPC:
        case ECustomGetSimCardReaderStatusIPC:
        case ECustomNotifyHSxPAStatusIPC:
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
        case ECustomNotifyCellInfoChangeIPC:
        case ECustomGetBandSelectionIPC:
        case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_3, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=None");
            policy = TSecurityPolicy( TSecurityPolicy::EAlwaysPass );
            break;

        // NetworkControl
        case ECustomResetNetServerIPC:
        case ECustomSetSystemNetworkModeIPC:
        case ECustomSimWarmResetIPC:
        case ECustomSetBandSelectionIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_4, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=NetworkControl");
            policy = TSecurityPolicy( ECapabilityNetworkControl );
            break;

        // NetworkControl, PowerMgmt
        case ECustomPowerSimOnIPC:
        case ECustomPowerSimOffIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_5, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=NetworkControl, PowerMgmt");
            policy = TSecurityPolicy( ECapabilityNetworkControl,
                ECapabilityPowerMgmt );
            break;

        // NetworkControl, ReadDeviceData
        case ECustomReleaseFileIPC:
        case ECustomRestartFileIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_6, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=NetworkControl, ReadDeviceData");
            policy = TSecurityPolicy( ECapabilityNetworkControl,
                ECapabilityReadDeviceData );
            break;

        // NetworkServices
        case ECustomTerminateCallIPC:
        case ECustomCancelUssdSessionIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_7, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=NetworkServices");
            policy = TSecurityPolicy( ECapabilityNetworkServices );
            break;

        // NetworkServices, WriteDeviceData
        case ECustomSetDriveModeIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_8, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=NetworkServices, WriteDeviceData");
            policy = TSecurityPolicy( ECapabilityNetworkServices,
                ECapabilityWriteDeviceData );
            break;

        // ReadDeviceData
        case EReadViagHomeZoneParamsIPC:
        case ECustomGetNetworkProviderNameIPC:
        case ECustomGetDiagnosticOctetsIPC:
        case ECustomGetLifeTimeIPC:
        case ECustomGetOperatorNameIPC:
        case ECustomGetProgrammableOperatorLogoIPC:
        case ECustomSsAdditionalInfoNotificationIPC:
        case ECustomIsBlockedIPC:
        case ECustomCheckTwoDigitDialSupportIPC:
        case ECustomNotifyEGprsInfoChange:
        case ECustomGetEGprsInfo:
        case EMobilePhoneIMSAuthenticate:
        case ECustomReadHSxPAStatusIPC:
        case ECustomGetIccCallForwardingStatusIPC:
        case ECustomGetCellInfoIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_9, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=ReadDeviceData");
            policy = TSecurityPolicy( ECapabilityReadDeviceData );
            break;

        // ReadDeviceData, Location
        case EReadViagHomeZoneCacheIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_10, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=ReadDeviceData, Location");
            policy = TSecurityPolicy( ECapabilityReadDeviceData,
                ECapabilityLocation );
            break;

        // ReadDeviceData, ReadUserData
        case ECustomNotifySsNetworkEventIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_11, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy= ReadDeviceData, ReadUserData");
            policy = TSecurityPolicy( ECapabilityReadDeviceData,
                ECapabilityReadUserData );
            break;

        // ReadDeviceData, ReadUserData, Location
        case ECustomReadSimFileIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_12, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=ReadDeviceData, ReadUserData, Location");
            policy = TSecurityPolicy( ECapabilityReadDeviceData,
                ECapabilityReadUserData,
                ECapabilityLocation );
            break;

        // ReadDeviceData, ReadUserData, WriteUserData, NetworkControl
        case ECustomGetSimAuthenticationDataIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_13, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=ReadDeviceData, ReadUserData, WriteUserData, NetworkControl");
            policy = TSecurityPolicy( ECapabilityReadDeviceData,
                ECapabilityReadUserData,
                ECapabilityWriteUserData,
                ECapabilityNetworkControl );
            break;

       	case ECustomGetServiceTableSupportbyApplicationIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_14, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=ReadDeviceData");
            policy = TSecurityPolicy( ECapabilityReadDeviceData );
            break;
        // ReadDeviceData, WriteDeviceData, ReadUserData, WriteUserData,
        // Location, NetworkServices, NetworkControl
        case ECustomSendAPDUReqIPC:
        case ECustomSendAPDUReqV2IPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_15, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=ReadDeviceData, WriteDeviceData, ReadUserData, WriteUserData, Location, NetworkServices, NetworkControl");
            policy = TSecurityPolicy( ECapabilityReadDeviceData,
                ECapabilityWriteDeviceData,
                ECapabilityReadUserData,
                ECapabilityWriteUserData,
                ECapabilityLocation,
                ECapabilityNetworkServices,
                ECapabilityNetworkControl  );
            break;

        // ReadUserData
        case ECustomStartSimCbTopicBrowsingIPC:
        case ECustomGetNextSimCbTopicIPC:
        case ECustomGetAirTimeDurationIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_16, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=ReadUserData");
            policy = TSecurityPolicy( ECapabilityReadUserData );
            break;

        // WriteDeviceData
        case ECustomSetAlwaysOnMode:
        case ECustomSimLockActivateIPC:
        case ECustomSimLockDeActivateIPC:
        case ECustomSetAlsBlockedIPC:
        case ECustomClearCallBlackListIPC:
        case ECustomNetWakeupIPC:
        case ECustomCheckSecurityCodeIPC:
        case ECustomDisablePhoneLockIPC:
        case ECustomSetSimMessageStatusReadIPC:
        case ECustomWriteHSxPAStatusIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_17, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=WriteDeviceData");
            policy = TSecurityPolicy( ECapabilityWriteDeviceData );
            break;

        // WriteDeviceData, Location
        case EWriteViagHomeZoneCacheIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_18, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=WriteDeviceData, Location");
            policy = TSecurityPolicy( ECapabilityWriteDeviceData,
                ECapabilityLocation );
            break;

        // WriteUserData
        case ECustomDeleteSimCbTopicIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_19, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=WriteUserData");
            policy = TSecurityPolicy( ECapabilityWriteUserData );
            break;

        // WriteDeviceData
        case EWriteViagHomeZoneUHZIUESettingsIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_20, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=WriteDeviceData");
            policy = TSecurityPolicy( ECapabilityWriteDeviceData );
            break;
        default:
            // if none of the above then we end up here,
            // as a default we already have caps as alwaysfail.
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREQUIREDPLATSECCAPS_21, "TSY: CMmCustomTsy::GetRequiredPlatSecCaps policy=AlwaysFail");
            break;
        }

    return policy;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CancelService
// When the clients close their sub-sessions (eg. by calling RPhone::Close),
// they may not have cancelled all their outstanding asynchronous requests
// before closing. It is up to the ETel server to clean up in this situation,
// so the server will find the list of outstanding requests related to that
// sub-session object and pass these outstanding IPC request numbers, one at
// a time, to the CancelService function in the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::CancelService(
    const TInt aIpc,
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt ret ( KErrGeneral );

    // When the clients close their sub-sessions (eg. by calling
    // RPhone::Close), they may not have cancelled all their outstanding
    // asynchronous requests before closing. It is up to the ETel server to
    // clean up in this situation, so the server will find the list of
    // outstanding requests related to that sub-session object and pass these
    // outstanding IPC request numbers, one at a time, to the CancelService
    // function in the TSY.

    switch ( aIpc )
        {
        case ECustomTerminateCallIPC:
        case ECustomSetAlsBlockedIPC:
        case ECustomNetWakeupIPC:
        case ECustomClearCallBlackListIPC:
        case ECustomResetNetServerIPC:
        case ECustomGetPndCacheStatusIPC:
        case ECustomSetDriveModeIPC:
        case ECustomSetBandSelectionIPC:	
        case ECustomSetSystemNetworkModeIPC:            
            // TSY has started a request and it is not possible to then
            // cancel it. The best thing for the TSY to do in
            // this case is to proceed as though the Cancel never happened.
            // The server's call to the TSY cancel function will return
            // synchronously. The TSY then continues to wait for the methods
            // acknowledgement and when it receives it, the TSY will complete
            // the original request.
            ret = KErrNone;
            break;
        case ECustomNotifyRauEventIPC:
            ret = NotifyRauEventCancel();
            break;
        case ECustomNotifyDtmfEventIPC:
            ret = NotifyDtmfEventCancel( aTsyReqHandle );
            break;
        case ECustomGetAlsBlockedIPC:
            ret = GetAlsBlockedCancel ();
            break;
        case ECustomGetCipheringInfoIPC:
            ret = GetCipheringInfoCancel();
            break;
        case ECustomNotifyCipheringInfoChangeIPC:
            ret = NotifyCipheringInfoChangeCancel( aTsyReqHandle );
            break;
        case ECustomNotifyNSPSStatusIPC:
            ret = NotifyNSPSStatusCancel( aTsyReqHandle );
            break;
        case ECustomNotifyAlsBlockedChangedIPC:
            ret = NotifyAlsBlockedChangedCancel( aTsyReqHandle );
            break;
        case ECustomCheckAlsPpSupportIPC:
            ret = GetAlsPpSupportCancel();
            break;
        case ECustomSsAdditionalInfoNotificationIPC:
            ret = SsAdditionalInfoNotificationCancel( aTsyReqHandle );
            break;
        case ECustomNotifySsRequestCompleteIPC:
            ret = SsRequestCompleteNotificationCancel( aTsyReqHandle );
            break;
        case ECustomNotifyPndCacheReadyIPC:
            ret = NotifyPhoneBookCacheReadyCancel( aTsyReqHandle );
            break;
        case ECustomCheckEmergencyNumberIPC:
            ret = CheckEmergencyNumberCancel();
            break;
        case ECustomGetNetworkProviderNameIPC:
            ret = GetNetworkOperatorNameCancel();
            break;
        case ECustomCheckTwoDigitDialSupportIPC:
            ret = CheckTwoDigitDialSupportCancel();
            break;
        case ECustomGetOperatorNameIPC:
            ret = GetOperatorNameCancel();
            break;
        case ECustomNotifySsNetworkEventIPC:
            ret = NotifySsNetworkEventCancel();
            break;
        case ECustomGetProgrammableOperatorLogoIPC:
            ret = GetProgrammableOperatorLogoCancel();
            break;
        case ECustomSatRefreshCompleteNotificationIPC:
            ret = SatRefreshCompleteNotificationCancel( aTsyReqHandle );
            break;
        case ECustomGetCurrentSystemNetworkModesIPC:
            ret = GetCurrentSystemNetworkModeCancel( aTsyReqHandle );
            break;
        case ECustomNotifyNetworkConnectionFailureIPC:
            ret = NotifyNetworkConnectionFailureCancel();
            break;
        case ECustomReadSimFileIPC:
            ret = GetSimFileInfoCancel( aTsyReqHandle );
            break;
        case ECustomGetLifeTimeIPC:
            ret = GetLifeTimeCancel();
            break;
        case EGet3GPBInfoIPC:
            ret = Get3GPBInfoCancel ();
            break;
        case ECustomGetSystemNetworkModesIPC:
            ret = GetSystemNetworkModesCancel( aTsyReqHandle );
            break;
        case EMobilePhoneIMSAuthenticate:
            ret = ImsAuthenticationCancel();
            break;
        case ECustomGetSimAuthenticationDataIPC:
            ret = SimAuthenticationCancel();
            break;
        case ECustomReadHSxPAStatusIPC:
            ret = ReadHSxPAStatusCancel();
            break;
        case ECustomWriteHSxPAStatusIPC:
            ret = WriteHSxPAStatusCancel();
            break;
        case ECustomNotifyHSxPAStatusIPC:
            ret = NotifyHSxPAStatusCancel();
            break;
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
            ret = NotifyIccCallForwardingStatusChangeCancel( aTsyReqHandle );
            break;
        case ECustomGetIccCallForwardingStatusIPC:
            ret = GetIccCallForwardingStatusCancel();
            break;
        case ECustomNotifyCellInfoChangeIPC:
            ret = NotifyCellInfoChangeCancel();
            break;
        case ECustomGetCellInfoIPC:
            ret = GetCellInfoCancel();
            break;
        case ECustomGetBandSelectionIPC:
            ret = GetSystemNetworkBandCancel( aTsyReqHandle );
            break;
       	case ECustomGetServiceTableSupportbyApplicationIPC:
       		ret = GetUSIMServiceSupportCancel();
       		break;
        case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
            ret = NotifyRemoteAlertingToneStatusChangeCancel();
            break;
        case ECustomTsyGetIccCallForwardingStatus:
          	ret = GetIccCallForwardingStatusCancel( aTsyReqHandle );
          	break;
        default:
        	if ( iMmSecurityTsy->SupportingIPC( aIpc ) ) 
        		{ 
        		ret = iMmSecurityTsy->CancelService ( aIpc, 
        				aTsyReqHandle ); 
        		}
            else
                {
                ret = CMmTsyBase::CancelService ( aIpc, aTsyReqHandle );
                }
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::IMSAuthenticationL
// Requests authentication data generatation from input parameters
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::ImsAuthenticationL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TImsAuthenticateDataV5* aAuthenticationData )
    {
    // Save pointer to client space
    iIMSAuthenticationData = aAuthenticationData;

    CMmDataPackage authenticationDataPackage;
    authenticationDataPackage.PackData( iIMSAuthenticationData );

    TInt ret = ( iMmPhoneTsy->MessageManager()->HandleRequestL(
        EMobilePhoneIMSAuthenticate, &authenticationDataPackage ) );

    // Check response of the extension
    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // Save the req handle type
        iReqHandleType = ECustomTsyIMSAuthenticate;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteImsAuthentication
// Requests authentication data generatation from input parameters
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteImsAuthentication(
    CMmDataPackage* aDataPackage,
    TInt aResult )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyIMSAuthenticate );

    if ( KErrNone == aResult )
        {
        if ( ECustomTsyReqHandleUnknown != reqHandle )
            {
            RMobilePhone::TImsAuthenticateDataV5* authenticationData;
            aDataPackage->UnPackData( &authenticationData );

            // Copy data to client
            iIMSAuthenticationData->iRES.Copy( authenticationData->iRES );
            iIMSAuthenticationData->iCK.Copy( authenticationData->iCK );
            iIMSAuthenticationData->iIK.Copy( authenticationData->iIK );

            iIMSAuthenticationData = NULL;

            ReqCompleted( reqHandle, aResult );
            }
        }
    else if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrTimedOut == aResult )
            {
            iIMSAuthenticationData = NULL;
            }
        else
            {
            RMobilePhone::TImsAuthenticateDataV5* authenticationData;
            aDataPackage->UnPackData( &authenticationData );

            // It is possible that request is completed whithout any data
            // inside datapackage
            if ( NULL != authenticationData )
                {
                iIMSAuthenticationData->iAUTS.Copy(
                  authenticationData->iAUTS );

                iIMSAuthenticationData = NULL;
                }
            }

        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::ImsAuthenticationCancel
// Cancels sim authentication request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::ImsAuthenticationCancel()
    {
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyIMSAuthenticate );

    // reset pointer to client data
    iIMSAuthenticationData = NULL;

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SimAuthenticationL
// Gets authentication parameters from sim
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SimAuthenticationL(
    const TTsyReqHandle aTsyReqHandle,
    const TDataPackage& aAuthData )
    {
    TInt ret ( KErrNone );
    TInt rfStateInfo ( ERfsStateInfoNormal );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_1,  "CMmCustomTSY: CMmCustomTsy::SimAuthentication" );

    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(ECustomTsyGetSimAuthenticationData);
    if(0 < reqHandle)
        {
        return KErrServerBusy;
        }



    RMmCustomAPI::TSimAuthenticationBase* basePtr =
        REINTERPRET_CAST( RMmCustomAPI::TSimAuthenticationBase*,
        aAuthData.Ptr1() );

    CMmDataPackage authenticationDataPackage;

    rfStateInfo = iMmPhoneTsy->GetRfStatus();

    // Declare a RAND sized buffer
    TBuf8<16> rand;

    if ( RMmCustomAPI::TSimAuthenticationBase::EEapSim ==
        basePtr->ExtensionId() )
        {
        iEapSim = REINTERPRET_CAST( RMmCustomAPI::TSimAuthenticationEapSim*,
            basePtr );

        rand.Append( iEapSim->iRandomParameters );

        authenticationDataPackage.PackData( iEapSim, &rfStateInfo );
        }
    else if ( RMmCustomAPI::TSimAuthenticationBase::EEapAka ==
        basePtr->ExtensionId() )
        {
        iEapAka = REINTERPRET_CAST( RMmCustomAPI::TSimAuthenticationEapAka*,
            basePtr );

        rand.Append( iEapAka->iRandomParameters );

        authenticationDataPackage.PackData( iEapAka, &rfStateInfo );
        }
    else if ( RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap ==
        basePtr->ExtensionId() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_2,  "CMmCustomTSY: SimAuthenticationL::in EGbaBootstrap" );
        iGbaBootstrap = REINTERPRET_CAST( 
	        RMmCustomAPI::TSimAuthenticationGbaBootstrap*, basePtr );

        rand.Append( iGbaBootstrap->iRandomParameters );

        authenticationDataPackage.PackData( iGbaBootstrap, &rfStateInfo  ); 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_3,  "CMmCustomTSY: SimAuthenticationL::out EGbaBootstrap" );
        }
    else if ( RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapUpdate ==
        basePtr->ExtensionId() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_4,  "CMmCustomTSY: SimAuthenticationL::in EGbaBootstrapUpdate" );
        iGbaBootstrapUpdate = REINTERPRET_CAST( 
        	RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate*, basePtr );

        authenticationDataPackage.PackData( iGbaBootstrapUpdate, &rfStateInfo  ); 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_5,  "CMmCustomTSY: SimAuthenticationL::out EGbaBootstrapUpdate" );
        }
    else if ( 
        RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapNafDerivation ==
        basePtr->ExtensionId() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_6,  "CMmCustomTSY: SimAuthenticationL::in EGbaBootstrapNafDerivation" );
        iGbaNafDerivation = REINTERPRET_CAST( 
        	RMmCustomAPI::TSimAuthenticationGbaNafDerivation*, basePtr );

        authenticationDataPackage.PackData( iGbaNafDerivation, &rfStateInfo  ); 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_7,  "CMmCustomTSY: SimAuthenticationL::out EGbaBootstrapNafDerivation" );
        }
    else if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMskUpdate ==
        basePtr->ExtensionId() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_8,  "CMmCustomTSY: SimAuthenticationL::in EMgvMskUpdate" );
        iMgvMskUpdate = REINTERPRET_CAST( 
        	RMmCustomAPI::TSimAuthenticationMgvMskUpdate*, basePtr );

        authenticationDataPackage.PackData( iMgvMskUpdate, &rfStateInfo  ); 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_9,  "CMmCustomTSY: SimAuthenticationL::out EMgvMskUpdate" );
        }
    else if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMtkGeneration ==
        basePtr->ExtensionId() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_10,  "CMmCustomTSY: SimAuthenticationL::in EMgvMtkGeneration" );
        iMgvMtkGeneration = REINTERPRET_CAST( 
        	RMmCustomAPI::TSimAuthenticationMgvMtkGeneration*, basePtr );

        authenticationDataPackage.PackData( iMgvMtkGeneration, &rfStateInfo  ); 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_11,  "CMmCustomTSY: SimAuthenticationL::out EMgvMtkGeneration" );
        }
    else if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMskDeletion ==
        basePtr->ExtensionId() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_12,  "CMmCustomTSY: SimAuthenticationL::in EMgvMskDeletion" );
        iMgvMskDeletion = REINTERPRET_CAST( 
        	RMmCustomAPI::TSimAuthenticationMgvMskDeletion*, basePtr );

        authenticationDataPackage.PackData( iMgvMskDeletion, &rfStateInfo  ); 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_13,  "CMmCustomTSY: SimAuthenticationL::out EMgvMskDeletion" );
        }
    else
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SIMAUTHENTICATIONL_14,  "CMmCustomTSY: Invalid authentication type specified: %d", basePtr->ExtensionId() );
        ret = KErrNotSupported;

        iEapSim = NULL;
        iEapAka = NULL;
		iGbaBootstrap = NULL;
		iGbaBootstrapUpdate = NULL;
		iGbaNafDerivation = NULL;
		iMgvMskUpdate = NULL;
		iMgvMtkGeneration = NULL;
		iMgvMskDeletion = NULL;

        //ReqCompleted( aTsyReqHandle, ret );
        }

    // Check if Rand received from network is valid
    // when if basePtr has ExtensionId of EEapSim or EEapAka
    if ( RMmCustomAPI::TSimAuthenticationBase::EEapSim ==
        basePtr->ExtensionId() || RMmCustomAPI::TSimAuthenticationBase::
        EEapAka == basePtr->ExtensionId())/* || RMmCustomAPI::
        TSimAuthenticationBase::EGbaBootstrap == basePtr->ExtensionId())*/
	    {
		ret =  CheckRandValidityL( rand );
	    }
      
    if ( KErrNone == ret )
       {
       ret = ( iMmPhoneTsy->MessageManager()->HandleRequestL(
           ECustomGetSimAuthenticationDataIPC, &authenticationDataPackage ) );

       // check result
       if ( KErrNone == ret )
           {
           // Save the req handle type
           iReqHandleType = ECustomTsyGetSimAuthenticationData;
           }
       else
           {
           iEapSim = NULL;
           iEapAka = NULL;
		   iGbaBootstrap = NULL;
	       iGbaBootstrapUpdate = NULL;
		   iGbaNafDerivation = NULL;
		   iMgvMskUpdate = NULL;
		   iMgvMtkGeneration = NULL;
		   iMgvMskDeletion = NULL;

           ReqCompleted( aTsyReqHandle, ret );
           }
       }
    else
        {
        iEapSim = NULL;
        iEapAka = NULL;
		iGbaBootstrap = NULL;
		iGbaBootstrapUpdate = NULL;
		iGbaNafDerivation = NULL;
		iMgvMskUpdate = NULL;
		iMgvMtkGeneration = NULL;
		iMgvMskDeletion = NULL;

        ReqCompleted( aTsyReqHandle, ret );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteSimAuthentication
// Completes sim authenticaiton request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteSimAuthentication(
    CMmDataPackage* aDataPackage,
    TInt aResult )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_1,  "TSY: CMmCustomTsy::CompleteSimAuthentication" );
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetSimAuthenticationData );

    if ( KErrNone == aResult )
        {
        if ( ECustomTsyReqHandleUnknown != reqHandle )
            {
            RMmCustomAPI::TSimAuthenticationBase* basePtr;
            aDataPackage->UnPackData( &basePtr );

            // if 2G authentication needs to be completed
            if ( RMmCustomAPI::TSimAuthenticationBase::EEapSim ==
                basePtr->ExtensionId() )
                {
                RMmCustomAPI::TSimAuthenticationEapSim* eapSim;

                aDataPackage->UnPackData( &eapSim );

                // Copy data to client
                iEapSim->iSRES.Copy( eapSim->iSRES );
                iEapSim->iKC.Copy( eapSim->iKC );

                iEapSim = NULL;

                ReqCompleted( reqHandle, aResult );
                }
            // if 3G authentication needs to be completed
            else if ( RMmCustomAPI::TSimAuthenticationBase::EEapAka ==
                basePtr->ExtensionId() )
                {
                RMmCustomAPI::TSimAuthenticationEapAka* eapAka;

                aDataPackage->UnPackData( &eapAka );

                // Copy data to client
                iEapAka->iRES.Copy( eapAka->iRES );
                iEapAka->iCK.Copy( eapAka->iCK );
                iEapAka->iIK.Copy( eapAka->iIK );

                iEapAka = NULL;

                ReqCompleted( reqHandle, aResult );
                }
            else if ( RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap ==
                basePtr->ExtensionId() )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_2,  "CMmCustomTSY: CompleteSimAuthentication::in EGbaBootstrap" );
                RMmCustomAPI::TSimAuthenticationGbaBootstrap* gbaBootstrap;

                aDataPackage->UnPackData( &gbaBootstrap );

                // Copy data to client
                iGbaBootstrap->iRES.Copy( gbaBootstrap->iRES );
                iGbaBootstrap->iAUTS.Copy( gbaBootstrap->iAUTS );

                iGbaBootstrap = NULL;

                ReqCompleted( reqHandle, aResult );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_3,  "CMmCustomTSY: CompleteSimAuthentication::out EGbaBootstrap" );
                }
            else if ( 
                RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapUpdate ==
                basePtr->ExtensionId() )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_4,  "CMmCustomTSY: CompleteSimAuthentication::in EGbaBootstrapUpdate" );
                RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate* 
                    gbaBootstrapUpdate;

                aDataPackage->UnPackData( &gbaBootstrapUpdate );

                // Copy data to client
                iGbaBootstrapUpdate->iRandomParameters.Copy( 
                    gbaBootstrapUpdate->iRandomParameters );

                iGbaBootstrapUpdate = NULL;

                ReqCompleted( reqHandle, aResult );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_5,  "CMmCustomTSY: CompleteSimAuthentication::out EGbaBootstrapUpdate" );
                }
            else if ( 
                RMmCustomAPI::TSimAuthenticationBase::
                    EGbaBootstrapNafDerivation == basePtr->ExtensionId() )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_6,  "CMmCustomTSY: CompleteSimAuthentication::in EGbaBootstrapNafDerivation" );
                RMmCustomAPI::TSimAuthenticationGbaNafDerivation* 
                    gbaNafDerivation;

                aDataPackage->UnPackData( &gbaNafDerivation );

                // Copy data to client
                iGbaNafDerivation->iKsExtNaf.Copy( 
                    gbaNafDerivation->iKsExtNaf );

                iGbaNafDerivation = NULL;

                ReqCompleted( reqHandle, aResult );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_7,  "CMmCustomTSY: CompleteSimAuthentication::out EGbaBootstrapNafDerivation" );
                }
            else if ( RMmCustomAPI::TSimAuthenticationBase::EMgvMskUpdate ==
                    basePtr->ExtensionId() )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_8,  "CMmCustomTSY: CompleteSimAuthentication::in EMgvMskUpdate" );
                RMmCustomAPI::TSimAuthenticationMgvMskUpdate* mgvMskUpdate;

                aDataPackage->UnPackData( &mgvMskUpdate );

                // Copy data to client
                iMgvMskUpdate->iMikey.Copy( mgvMskUpdate->iMikey );
                // Smartcard support for MobileTV
                iMgvMskUpdate->iBCASTManagement.Copy( mgvMskUpdate->iBCASTManagement );
                iMgvMskUpdate->iParentalRating.Copy( mgvMskUpdate->iParentalRating );
                iMgvMskUpdate->iSecurityPolicyExt.Copy( mgvMskUpdate->iSecurityPolicyExt );
                               
                iMgvMskUpdate = NULL;

                ReqCompleted( reqHandle, aResult );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_9,  "CMmCustomTSY: CompleteSimAuthentication::out EMgvMskUpdate" );
                }
            else if ( RMmCustomAPI::TSimAuthenticationBase::
                    EMgvMtkGeneration == basePtr->ExtensionId() )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_10,  "CMmCustomTSY: CompleteSimAuthentication::in EMgvMtkGeneration" );
                RMmCustomAPI::TSimAuthenticationMgvMtkGeneration* 
                    mgvMtkGeneration;

                aDataPackage->UnPackData( &mgvMtkGeneration );

                // Copy data to client
                iMgvMtkGeneration->iMtkSalt.Copy( mgvMtkGeneration->iMtkSalt );
                // Smartcard support for MobileTV
                iMgvMtkGeneration->iBCASTManagement.Copy( mgvMtkGeneration->iBCASTManagement );
                iMgvMtkGeneration->iParentalControl.Copy( mgvMtkGeneration->iParentalControl );
                iMgvMtkGeneration->iTrafficEncryptionKey.Copy( mgvMtkGeneration->iTrafficEncryptionKey );
                iMgvMtkGeneration = NULL;

                ReqCompleted( reqHandle, aResult );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_11,  "CMmCustomTSY: CompleteSimAuthentication::out EMgvMtkGeneration" );
                }
            else if ( RMmCustomAPI::TSimAuthenticationBase::
                    EMgvMskDeletion == basePtr->ExtensionId() )
                {
                ReqCompleted( reqHandle, aResult );
                }
            else
                {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_12,  "CMmCustomTSY: Invalid authentication type specified: %d", basePtr->ExtensionId() );
                aResult = KErrArgument;
                ReqCompleted( reqHandle, aResult );
                }
            }
        }
    else if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrTimedOut == aResult )
            {
            iEapSim = NULL;
            iEapAka = NULL;
            }
        else
            {
            RMmCustomAPI::TSimAuthenticationBase* basePtr;
            aDataPackage->UnPackData( &basePtr );

            // if no datapackage is received in error response
            if ( NULL == basePtr )
                {
                iEapAka = NULL;
                iEapSim = NULL;
                }
            else
                {
                // if 3G authentication needs to be completed
                // copy AUTS sequence to client
                if ( RMmCustomAPI::TSimAuthenticationBase::EEapAka ==
                        basePtr->ExtensionId() )
                    {
                    RMmCustomAPI::TSimAuthenticationEapAka* eapAka;
                    aDataPackage->UnPackData( &eapAka );

                    iEapAka->iAUTS.Copy( eapAka->iAUTS );

                    iEapAka = NULL;
                    }
                
                else if( RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap ==
                        basePtr->ExtensionId() )
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESIMAUTHENTICATION_13,  "TSYMH: CompleteSimAuthentication RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap" );
                    RMmCustomAPI::TSimAuthenticationGbaBootstrap* gbabootstrap;
                    aDataPackage->UnPackData( &gbabootstrap );

                    iGbaBootstrap->iAUTS.Copy( gbabootstrap->iAUTS );
                    }
                    
                iEapSim = NULL;
                }
            }

	    // reset pointer to client data
		iGbaBootstrap = NULL;
		iGbaBootstrapUpdate = NULL;
		iMgvMskUpdate = NULL;
		iMgvMtkGeneration = NULL;
		iMgvMskDeletion = NULL;
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SimAuthenticationCancel
// Cancels sim authentication request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SimAuthenticationCancel()
    {
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetSimAuthenticationData );

    if( ECustomTsyReqHandleUnknown != reqHandle )
	    {
	    // reset pointer to client data
	    iEapSim = NULL;
	    iEapAka = NULL;
		iGbaBootstrap = NULL;
		iGbaBootstrapUpdate = NULL;
		iMgvMskUpdate = NULL;
		iMgvMtkGeneration = NULL;
		iMgvMskDeletion = NULL;
		
	    // complete
        ReqCompleted( reqHandle, KErrCancel );

        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CheckRandValidityL
// Checks if rand received from network is valid
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::CheckRandValidityL(
    TBuf8<16> aRand )
    {
    TInt ret( KErrNone );

    TDesC8& rand = aRand;

    CSHA1* sha1 = CSHA1::NewL();

    sha1->Reset();

    TPtrC8 messageDigest = sha1->Final( rand );

    ret = FindSetBitsFromBackup( messageDigest );

    // If bits were not found from rand_db.prv or
    // if rand_db.prv itself was not found
    if ( KErrNone == ret || KErrNotFound == ret )
        {
        ret = InsertBlobsToRandDb( messageDigest );
        }

    delete sha1;
    sha1 = NULL;

    if ( KErrArgument == ret && 0 == iFreshBitCounter )
       {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CHECKRANDVALIDITYL_1,  "CMmCustomTsy: RAND rejected" );
       }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CreateRandDb
// Creates rand_db.cur
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::CreateRandDb()
    {
    TInt ret = ( KErrNone );

    // local variable initialized to zero
    TInt counter = 0;


    // fileSize is initialized to 2 because of filesize calculation method
    TInt fileSize = 2;

    RFs   fs;
    RFile file;
    
    ret = fs.Connect();
    
    TDriveNumber driveNumber;
    driveNumber = fs.GetSystemDrive();
    fs.CreatePrivatePath( driveNumber );
    fs.SetSessionToPrivate( driveNumber );
    
    if ( KErrNone == ret )
        {
        ret = file.Create( fs, KRandDb, EFileShareAny );

        if ( KErrNone != ret )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CREATERANDDB_1,  "TSY: CMmCustomTsy: 'rand_db.cur' creation failed!" );
            }
        else if ( KErrNone == ret )
            {
            ret = file.Open( fs, KRandDb, EFileShareAny | EFileWrite );

            if ( KErrNone == ret )
                {
                // calculates how many bits can be addressed with current
                // BLOB_SIZE
                for ( counter = 1; counter < BLOB_SIZE; counter++ )
                    {
                    fileSize = ( fileSize * 2 );
                    }
                // max number of bits is divided by 8 to get the actual
                // filesize
                fileSize = ( fileSize / 8 );

                ret = file.SetSize( fileSize + METADATA_SIZE );

				// empty tbuf for rand_db initialization
				TBuf8<1024> empty;
				TInt chunkSize = 1024;
				empty.FillZ( chunkSize );
				
                // reset every byte of newly created rand_db to zero
				TInt maxSize = ( fileSize + METADATA_SIZE );
                for ( counter = 0; counter < maxSize;
                    counter+=chunkSize )
                    {
                    TInt writeSize = maxSize - counter;
                    if(writeSize > chunkSize)
                        {
                        writeSize = chunkSize;
                        }
                    ret = file.Write( counter, empty, writeSize );
                    }

                if ( KErrNone != ret )
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CREATERANDDB_2,  "TSY: CMmCustomTsy: 'rand_db.cur' initialization failed!" );
                    file.Close();
                    }
                }
            else
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CREATERANDDB_3,  "TSY: CMmCustomTsy: 'rand_db.cur' open failed!" );
                }
            }

        if ( KErrNone == ret )
            {
            file.Close();
            fs.Close();
            }
        else
            {
            fs.Close();
            }
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CREATERANDDB_4,  "TSY: CMmCustomTsy: Could not connect to file server!" );
        }

    // Give some time for file.Close() and fs.Close() to complete
    User::After( 200000 );

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::InsertBlobsToRandDb
// Inserts blobs to rand_db.cur
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::InsertBlobsToRandDb(
    TPtrC8 aMessageDigest )
    {
    RFs fs;
    RFile file;

    TBuf8<1> byteFromRandDb;

    TInt ret = KErrNone;

    // Many local variables initialized to zero
    TInt8 counter = 0;
    TInt8 counter2 = 0;

    TUint16 hexDigit = 0;
    TUint8 blob = 0;
    TInt byteNumber = 0;
    TInt bit = 0;

    TUint16 assignedBits = 0;
    iFreshBitCounter = 0;

    ret = fs.Connect();
       
    TDriveNumber driveNumber;
    driveNumber = fs.GetSystemDrive();
    fs.CreatePrivatePath( driveNumber );
    fs.SetSessionToPrivate( driveNumber );

    if ( KErrNone == ret )
        {
        ret = file.Open( fs, KRandDb, EFileShareExclusive | EFileWrite );
        if ( KErrNotFound == ret )
            {
            ret = CreateRandDb();

            if ( KErrNone == ret )
                {
                ret = file.Open( fs, KRandDb, EFileShareExclusive |
                    EFileWrite );
                if( KErrNone != ret )
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_INSERTBLOBSTORANDDB_1,  "TSY: CMmCustomTsy: rand_db open failed" );
                    }
                }
            if ( KErrNone != ret )
                {
                fs.Close();

                return ret;
                }
            }

        for ( counter = 0; counter < 20; counter++ )
            {
            // reset loop variables
            hexDigit = 0;
            assignedBits = 0;

            // read one hexDigit from message digest
            hexDigit = static_cast< TUint16 > ( aMessageDigest[counter] );
            hexDigit <<= 8;
            counter++;
            hexDigit |= static_cast< TUint16 > ( aMessageDigest[counter] );

            bit = 1;

            // read as many bits from hexDigit as blobSize indicates
            for ( counter2 = 0; counter2 < BLOB_SIZE; counter2++ )
                {
                assignedBits |= bit & hexDigit;
                bit <<= 1;
                }

            byteNumber = ( assignedBits / 8 );
            bit = ( assignedBits - ( byteNumber * 8 ) );

            ret = file.Read( byteNumber, byteFromRandDb  );
            if( KErrNone != ret )
                {
                break;
                }

            blob = 128;
            blob >>= bit;

            // if bit is not already set
            if ( !blob == ( blob & byteFromRandDb[0] ) )
                {

                byteFromRandDb[0] |= blob;

                ret = file.Write( byteNumber, byteFromRandDb );
                if( KErrNone != ret )
                    {
                    break;
                    }

                iFreshBitCounter++;
                }
            }

        file.Close();
        fs.Close();
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_INSERTBLOBSTORANDDB_2,  "TSY: CMmCustomTsy: Could not connect to file server!" );
        return ret;
        }

    // if no new bits were found e.g. RAND will be rejected
    if ( 0 == iFreshBitCounter )
        {
        ret = KErrArgument;
        }
    else
        {
        ret = UpdateBitCounter();
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::UpdateBitCounter
// Updates bitcounter information on rand_db.cur
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::UpdateBitCounter()
    {
    TInt ret( KErrNone );

    RFs fs;
    RFile file;

    TBuf8<2> bitCounterBytes;

    // local varibales initialized to zero
    TUint16 bitCounter = 0;
    TUint16 temporaryBitCounter = 0;
    TInt8 counter = 0;

    // fileSize is initialized to 2 because of filesize calculation method
    TInt fileSize = 2;

    ret = fs.Connect();
    
    TDriveNumber driveNumber;
    driveNumber = fs.GetSystemDrive();
    fs.CreatePrivatePath( driveNumber );
    fs.SetSessionToPrivate( driveNumber );
    
    if ( KErrNone == ret )
        {
        ret = file.Open( fs, KRandDb, EFileShareExclusive | EFileWrite );
        if ( KErrNone == ret )
            {
            // calculate filesize
            for ( counter = 1; counter < BLOB_SIZE; counter++ )
                {
                fileSize = ( fileSize * 2 );
                }
            fileSize = ( fileSize / 8 );

            ret = file.Read( fileSize, bitCounterBytes, 2 );
            if ( KErrNone == ret  )
                {
                // read bitcounter info from rand_db
                bitCounter = STATIC_CAST( TUint16, bitCounterBytes[0] );
                bitCounter <<= 8;
                bitCounter |= bitCounterBytes[1];

                // add fresh bits to bit counter info
                bitCounter += iFreshBitCounter;

                temporaryBitCounter = bitCounter;
                bitCounterBytes[1] = STATIC_CAST( TUint8,
                  temporaryBitCounter );
                temporaryBitCounter >>= 8;
                bitCounterBytes[0] = STATIC_CAST( TUint8,
                  temporaryBitCounter );

                ret = file.Write( fileSize, bitCounterBytes, 2 );

                // if more than half of the available bits are set
                if ( ( fileSize * 4 ) < bitCounter )
                    {
                    ret = fs.Delete( KRandDbPrev );

                    if ( KErrNotFound == ret  || KErrNone == ret )
                        {
                        ret = file.Rename( KRandDbPrev );
                        }
                    }
                }

            file.Close();
            }

        fs.Close();
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_UPDATEBITCOUNTER_1,  "TSY: CMmCustomTsy: Could not connect to file server!" );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::FindSetBitsFromBackup
// Checks rand_db.prv for set bits
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::FindSetBitsFromBackup(
    TPtrC8 aMessageDigest )
    {
    RFs fs;
    RFile file;
    TBuf8<1> byteFromRandDb;

    TInt ret = KErrNone;

    // Many local variables initialized to zero
    TInt8 counter = 0;
    TInt8 counter2 = 0;

    TUint16 hexDigit = 0;
    TUint8 blob = 0;
    TInt byteNumber = 0;
    TInt bit = 0;

    TUint16 assignedBits = 0;
    iFreshBitCounter = 0;

    ret = fs.Connect();
    
    TDriveNumber driveNumber;
    driveNumber = fs.GetSystemDrive();
    fs.CreatePrivatePath( driveNumber );
    fs.SetSessionToPrivate( driveNumber );
    
    if( KErrNone == ret )
    {
    ret = file.Open( fs, KRandDbPrev, EFileShareAny | EFileRead );

    if ( KErrNone == ret )
        {
        for ( counter = 0; counter < 20; counter++ )
            {
            // reset loop variables
            hexDigit = 0;
            assignedBits = 0;

            // read one hexDigit from message digest
            hexDigit = STATIC_CAST( TUint16, aMessageDigest[counter] );
            hexDigit <<= 8;
            counter++;
            hexDigit |= STATIC_CAST( TUint16, aMessageDigest[counter] );

            // Initialize bit to 1
            bit = 1;

            // read as many bits from hexDigit as blobSize indicates
            for ( counter2 = 0; counter2 < BLOB_SIZE; counter2++ )
                {
                assignedBits |= bit & hexDigit;
                bit <<= 1;
                }

            // calculate the byte and bit to be read from rand_db backup
            byteNumber = ( assignedBits / 8 );
            bit = ( assignedBits - ( byteNumber * 8 ) );

            ret = file.Read( byteNumber, byteFromRandDb );
            if ( KErrNone != ret )
                {
                break;
                }

            // Initialize blob to 10000000
            blob = 128;
            blob >>= bit;

            // if bit is not found from old db
            if ( !blob == ( blob & byteFromRandDb[0] ) )
                {
                iFreshBitCounter++;

                file.Close();
                fs.Close();

                return ret;
                }
            }

        file.Close();
        fs.Close();

        if ( 0 == iFreshBitCounter )
            {
            ret = KErrArgument;
            }
        else
            {
            ret = KErrNone;
            }
        }

    if ( KErrNotFound == ret )
        {
        fs.Close();
        }
    }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::TerminateCallL
// This method is used for terminating a call or all calls.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::TerminateCallL(
    const TTsyReqHandle aTsyReqHandle,
    const TName* aCallName )
    {
    // set return value to KErrNotFound
    TInt ret( KErrNotFound );

    TTsyReqHandle terminateCallHandle =
        iTsyReqHandleStore->GetTsyReqHandle( ECustomTsyTerminateCall );

    if ( 0 < terminateCallHandle )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about
        //the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // Cases are:
        // if parameter is ALL_CALLS
        //     if any active or held call exists
        //         hangup active and held calls
        //     else
        //         complete with KErrNone
        // else (parameter is single call)
        //     if call exists
        //         hangup call
        //     else
        //         complete with KErrNotFound
        //

        // call id
        TInt callId = -1;

        // indicates if this is called in an error situation (ETrue)
        // or by user request (EFalse);
        TBool isError = EFalse;

        // terminate all calls
        // CompareF returns 0 if names are equal
        if ( 0 == aCallName->CompareF( KTerminateAllCalls ) )
            {
            CMmCallTsy* activeCall =
                iMmPhoneTsy->CallList()->GetMmCallByStatus(
                    RMobileCall::EStatusConnected );
            CMmCallTsy* heldCall =
                iMmPhoneTsy->CallList()->GetMmCallByStatus(
                    RMobileCall::EStatusHold );
            CMmCallTsy* connectingCall =
                iMmPhoneTsy->CallList()->GetMmCallByStatus(
                    RMobileCall::EStatusDialling );
            if ( NULL == connectingCall )
                {
                connectingCall =
                iMmPhoneTsy->CallList()->GetMmCallByStatus(
                    RMobileCall::EStatusConnecting );
                }

            if ( activeCall || heldCall || connectingCall )
                {
                // Let's have negative or zero mean "all active and held calls
                // and the call id given by taking absolute value of
                // parameter" as we need to somehow pass alerting call id,
                // which is neither active or held call..
                callId = ( NULL != connectingCall ) ?
                    -TInt( connectingCall->CallId() ) : ( 0 );
                CMmDataPackage dataPackage;
                dataPackage.PackData( &callId, &isError );
                ret = iMmPhoneTsy->MessageManager()->HandleRequestL(
                    ECustomTerminateCallIPC, &dataPackage );
                }
            else
                {
                ReqCompleted( aTsyReqHandle, KErrNone );
                // return immediately, otherwise KErrNone would cause the
                // request handle to be saved!
                return KErrNone;
                }
            }
        // terminate call by name
        else
            {
            // change the cause value, a specific call termination is called
            // when error has ocurred
            isError = ETrue;

            // get call object by name
            CMmCallTsy* namedCall =
                iMmPhoneTsy->CallList()->GetMmCallByName( aCallName );

            // get call ID
            if ( namedCall )
                {
                callId = namedCall->CallId();
                CMmDataPackage dataPackage;
                dataPackage.PackData( &callId, &isError );
                ret = iMmPhoneTsy->MessageManager()->HandleRequestL(
                    ECustomTerminateCallIPC, &dataPackage );
                }
            }

        if ( KErrNone != ret )
            {
            // request failed, complete with error value
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Store the request handle
            iReqHandleType = ECustomTsyTerminateCall;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteTerminateCall
// This method completes a TerminateCall or a DialCancel request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteTerminateCall(
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETETERMINATECALL_1,  "TSY: CMmCustomTsy::CompleteTerminateCall - aError: %{TSymbianErrorCodes}", aError );

    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyTerminateCall );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // complete the request with reqhandle and aError
        ReqCompleted( reqHandle, aError );
        }
    // Set 3rd party Dial check number to EFalse  
    iISVDialNumberCheck = EFalse;    
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyDtmfEvent
// This method allows a client to be notified when the DTMF phase of a call
// has begun and completed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyDtmfEvent(
        RMmCustomAPI::TDtmfInfo* aInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_NOTIFYDTMFEVENT_1, "TSY: CMmCustomTsy::NotifyDtmfEvent");
    // save pointer to client data
    iNotifyInfo = aInfo;

    // save handle type
    iReqHandleType = ECustomTsyNotifyDtmfEvent;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyDtmfEvent
// Completes a NotifyDtmfEvent request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyDtmfEvent(
    RMmCustomAPI::TDtmfInfo aInfo,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYDTMFEVENT_1, "TSY: CMmCustomTsy::CompleteNotifyDtmfEvent");
    // reset request handle. Returns the deleted req handle.
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyDtmfEvent );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // set the information for the client
            *iNotifyInfo= aInfo;
            }

        // reset the internal variable
        iNotifyInfo = NULL;
        // complete
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyDtmfEventCancel
// This method cancels an outstanding notification of dtmf event, placed with
// the NotifyDtmfEvent function.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyDtmfEventCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // reset pointer to client data
    iNotifyInfo = NULL;

    // reset reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle( ECustomTsyNotifyDtmfEvent );

    // complete with KErrCancel
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetDiagnosticInfo
// This function gets the SS diagnostic info (octets) of a call object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetDiagnosticInfo(
    const TTsyReqHandle aTsyReqHandle,
    TName* aCallName )
    {
    TInt errorValue = KErrNone;
    // get call object by name
    CMmCallTsy* mmCall = iMmPhoneTsy->CallList()->GetMmCallByName(
      aCallName );

    // if call exists
    if ( mmCall )
        {
        TUint8 diags = mmCall->GetDiagnostics();
        //gets diagnostic value - check value -  return error code

        if ( 0 != diags )
            {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETDIAGNOSTICINFO_1,  "TSY:CMmCustomTsy::GetDiagnosticInfo: Diagnostic info=%d asked for call id=%d", diags, mmCall->CallId() );
            switch ( diags )
                {                
                case KDiagnosticInfoBarredWithCUG:
                    errorValue = KErrDiagnosticInfoBarredWithCUG;
                    break;
                case KDiagnosticInfoBarredNoCUG:
                    errorValue = KErrDiagnosticInfoBarredNoCUG; 
                    break;
                case KDiagnosticInfoBarredUnknownCUG:
                    errorValue = KErrDiagnosticInfoBarredUnknownCUG;
                    break;
                case KDiagnosticInfoBarredIncompatibleCUG:
                    errorValue = KErrDiagnosticInfoBarredIncompatibleCUG;
                    break;
                case KDiagnosticInfoBarredFailureCUG:
                    errorValue = KErrDiagnosticInfoBarredFailureCUG;
                    break;
                case KDiagnosticInfoBarredClirNotSubscribed:
                    errorValue = KErrDiagnosticInfoBarredClirNotSubscribed;
                    break;   
                case KDiagnosticInfoBarredCCBSPossible:
                    errorValue = KErrDiagnosticInfoBarredCCBSPossible;
                    break; 
                case KDiagnosticInfoBarredCCBSNotPossible:
                    errorValue = KErrDiagnosticInfoBarredCCBSNotPossible;
                    break;
                default:
                    errorValue = KErrNone;
                    break;
                }
            }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETDIAGNOSTICINFO_2,  "TSY: CMmCustomTsy::GetDiagnosticInfo - ReqCompleted - Error code: %{TSymbianErrorCodes}", errorValue );
        // complete errorvalue to client  - inform change  
        ReqCompleted( aTsyReqHandle, errorValue ); 
        }
    else
        {
         // call is not found
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETDIAGNOSTICINFO_3, "TSY: CMmCustomTsy::GetDiagnosticInfo - Call is not found");
        ReqCompleted( aTsyReqHandle, KErrNotFound );
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETDIAGNOSTICINFO_4,  "TSY: CMmCustomTsy::GetDiagnosticInfo - Error code: %{TSymbianErrorCodes}", errorValue );
   
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetDiagnosticOctects
// This function sets the SS diagnostic octets of a call object when received
// from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetDiagnosticOctects(
    TInt aCallId,
    TUint8 aDiags )
    {
    // get call object by id
    CMmCallTsy* mmCall = iMmPhoneTsy->CallList()->GetMmCallById( aCallId );

    // if call exists, set diagnostics
    if ( mmCall )
        {
        mmCall->SetDiagnostics( aDiags );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetRemoteAlertingToneStatus
// This function returns the remote alerting tone status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetRemoteAlertingToneStatus(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TRemoteAlertingToneStatus* aToneStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETREMOTEALERTINGTONESTATUS_1, "TSY: CMmCustomTsy::GetRemoteAlertingToneStatus");
    TInt ret = iMmCustomExtInterface->GetRemoteAlertingToneStatus(
        aToneStatus );

    ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetCallOrigin
// This function returns the origin of the call (ETel or other), defined by
// aCallName parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetCallOrigin(
    const TTsyReqHandle aTsyReqHandle,
    TName* aCallName,
    RMmCustomAPI::TCallOrigin* aOrigin )
    {
    // get call object by name
    CMmCallTsy* mmCall = iMmPhoneTsy->CallList()->GetMmCallByName(
      aCallName );

    // if call exists
    if ( mmCall )
        {
        // get the call origin from Call Object
        // if the call is a ghost call it means that is not made by ETEL
         if ( mmCall->ETelOriginated() )
            {
            *aOrigin = RMmCustomAPI::EETelCall;
            }
         else
            {
            *aOrigin = RMmCustomAPI::EOutsider;
            }
        }
    else // call not found
        {
        *aOrigin = RMmCustomAPI::EUnknown;
        }

    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetAlsBlockedL
// This method gets the alternating line service block status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetAlsBlockedL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TGetAlsBlockStatus* aBlockStatus )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETALSBLOCKEDL_1,  "TSY: CMmCustomTsy::GetAlsBlockedL - Req handle: %d, Block status: %d", aTsyReqHandle, *aBlockStatus );
    TTsyReqHandle getAlsBlockedHandle =
        iTsyReqHandleStore->GetTsyReqHandle( ECustomTsyGetAlsBlocked );

    if ( 0 < getAlsBlockedHandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save pointer to client data
        iRetAlsBlockStatus = aBlockStatus;

        // call DOS (no packed parameters)
        TInt ret = iMmPhoneTsy->MessageManager()->HandleRequestL(
            ECustomGetAlsBlockedIPC );

        // check the result of the call
        if ( KErrNone != ret )
            {
            ReqCompleted ( aTsyReqHandle, ret );
            }
        else
            {
            // Store the request handle
            iReqHandleType = ECustomTsyGetAlsBlocked;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetAlsBlocked
// Completes a GetAlsBlocked operation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetAlsBlocked(
    RMmCustomAPI::TGetAlsBlockStatus  aBlockStatus,
    TInt aErrorCode )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETALSBLOCKED_1,  "TSY: CMmCustomTsy::CompleteGetAlsBlocked - Block status: %d, Error code: %{TSymbianErrorCodes}", aBlockStatus, aErrorCode );
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetAlsBlocked );

    // check if request was called
    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        //check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // set the information for the client
            *iRetAlsBlockStatus = aBlockStatus;
            }
        // reset the internal variable
        iRetAlsBlockStatus = NULL;
        // complete
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetAlsBlockedCancel
// This method cancels getting of alternative line service block status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetAlsBlockedCancel()
    {
    // reset the pointer to client space
    iRetAlsBlockStatus = NULL;

    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetAlsBlocked );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::Get3GPBInfo
// This method gets 3G phonebook info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::Get3GPBInfo(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::T3GPBInfo* aInfo )
    {
    // Fill info struct
    aInfo->iMaxLenAnr = ( TInt16 )iMmPhoneTsy->PhoneBookState()->
        iANRNumLengthMax;
    aInfo->iMaxLenEmail = ( TInt16 )iMmPhoneTsy->PhoneBookState()->
        iEmailTextLengthMax;
    aInfo->iMaxLenSne = ( TInt16 )iMmPhoneTsy->PhoneBookState()->
        iSNETextLengthMax;
    aInfo->iMaxNumAnr = ( TInt8 )iMmPhoneTsy->PhoneBookState()->
        iANRNumOfEntriesPerEntry;
    aInfo->iMaxNumEmail = ( TInt8 )iMmPhoneTsy->PhoneBookState()->
        iEmailNumOfEntriesPerEntry;
    aInfo->iMaxNumGroupName = ( TInt8 )iMmPhoneTsy->PhoneBookState()->
        iGRPNumOfEntriesPerEntry;
    aInfo->iMaxNumSne = ( TInt8 )iMmPhoneTsy->PhoneBookState()->
        iSNENumOfEntriesPerEntry;
    aInfo->iMaxLenGroupName = -1;   // Not known

    // complete with KErrNone
    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::Get3GPBInfoCancel
// This method cancels getting of 3G phonebook info
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::Get3GPBInfoCancel()
    {
    // Get3GPBInfo is completed immediately after the request
    // has been done. No cancelling possible.
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SetAlsBlockedL
// This method sets the alternating line service (ALS) block status using the
// aBlockStatus variable.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SetAlsBlockedL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TSetAlsBlock* aBlockStatus )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SETALSBLOCKEDL_1,  "TSY: CMmCustomTsy::SetAlsBlockedL - Req handle: %d, Block status: %d", aTsyReqHandle, *aBlockStatus );
    // get the handle
    TTsyReqHandle setAlsBlockedHandle =
        iTsyReqHandleStore->GetTsyReqHandle( ECustomTsySetAlsBlocked );

    if ( setAlsBlockedHandle > 0 )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save pointer to aBlockStatus (used for notification)
        iSetBlockStatus = aBlockStatus;

        // call DOS
        // packed parameter: TSetAlsBlock (block status)
        CMmDataPackage dataPackage;
        dataPackage.PackData( aBlockStatus );
        TInt ret = iMmPhoneTsy->MessageManager()->HandleRequestL(
            ECustomSetAlsBlockedIPC, &dataPackage );

        // check success
        if ( KErrNone != ret )
            {
            ReqCompleted ( aTsyReqHandle, ret );
            }
        else
            {
            // Store the request handle
            iReqHandleType = ECustomTsySetAlsBlocked;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteSetAlsBlocked
// Completes a SetAlsBlocked request. If value for completion is KErrNone and
// a NotifyAlsBlockedChanged has been requested this method calls
// CompleteNotifyAlsBlockedChanged.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteSetAlsBlocked(
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESETALSBLOCKED_1,  "TSY: CMmCustomTsy::CompleteSetAlsBlocked - Error code: %{TSymbianErrorCodes}", aErrorCode );
    // get reaq handle for NotifyAlsBlockedChanged
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyNotifyAlsBlockedChanged );

    // check if notification was requested and Set was ok
    if( ECustomTsyReqHandleUnknown != reqHandle && KErrNone == aErrorCode )
        {
        CompleteNotifyAlsBlockedChanged ();
        }

    // reset saved pointer
    iSetBlockStatus = NULL;

    // reset req handle. Returns the deleted req handle
    reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsySetAlsBlocked );

    // check if request was called
    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // complete
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyAlsBlockedChanged
// This method notifies a client of changed ALS blocked status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyAlsBlockedChanged(
    RMmCustomAPI::TGetAlsBlockStatus* aBlockStatus )
    {
    // save pointer to client data
    iRetNotifyAlsBlockStatus = aBlockStatus;

    // save handle type
    iReqHandleType = ECustomTsyNotifyAlsBlockedChanged;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyAlsBlockedChanged
// This method notifies a client of changed ALS blocked status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyAlsBlockedChanged()
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyAlsBlockedChanged );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle && iSetBlockStatus)
        {
        // set the information for the client
#ifndef USING_CTSY_DISPATCHER
        if ( RMmCustomAPI::EActivateBlock == *iSetBlockStatus )
#else	//USING_CTSY_DISPATCHER
        if ( iSetBlockStatus && RMmCustomAPI::EActivateBlock == *iSetBlockStatus )
#endif //USING_CTSY_DISPATCHER
            {
            *iRetNotifyAlsBlockStatus = RMmCustomAPI::EBlockStatusActive;
            }
        else // EDeactivateBlock == *iSetBlockStatus
            {
            *iRetNotifyAlsBlockStatus = RMmCustomAPI::EBlockStatusInactive;
            }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYALSBLOCKEDCHANGED_1,  "TSY: CMmCustomTsy::CompleteNotifyAlsBlockedChanged - Block status: %d", *iSetBlockStatus );

        // reset the varible
        iRetNotifyAlsBlockStatus = NULL;

        // complete
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyAlsBlockedChangedCancel
// Cancels an outstanding NotifyAlsBlockedChanged notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyAlsBlockedChangedCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // reset the pointer to client data
    iRetNotifyAlsBlockStatus = NULL;

    // reset the reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyAlsBlockedChanged );

    // complete
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetAlsPpSupportL
// Gets ALS support information from Product Profile.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetAlsPpSupportL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TAlsSupport* aSupport )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETALSPPSUPPORTL_1,  "TSY: CMmCustomTsy::GetAlsPpSupportL");
    // save pointer to client space
    iAlsSupport = aSupport;
    
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyGetAlsPpSupport );

    if ( 0 < reqHandle )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETALSPPSUPPORTL_2,  "TSY: CMmCustomTsy::GetAlsPpSupportL - Already processing, save handle");
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about
		TCheckAlsPpSupportRequest* req = 
                              new (ELeave) TCheckAlsPpSupportRequest();
		req->iReqHandle = aTsyReqHandle;
		req->iRetSupport = aSupport;
		iCheckAlsPpSupportRequests.AppendL( req );
        }
    else
        {
    // call DOS (no packed parameters)
    TInt ret = Phone()->MessageManager()->HandleRequestL(
        ECustomCheckAlsPpSupportIPC );

    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETALSPPSUPPORTL_3,  "TSY: CMmCustomTsy::GetAlsPpSupportL - Save handle");
        // Save the req handle type
        iReqHandleType = ECustomTsyGetAlsPpSupport;
			// save request in queue for completion
			TCheckAlsPpSupportRequest* req = 
                              new (ELeave) TCheckAlsPpSupportRequest();
			req->iReqHandle = aTsyReqHandle;
			req->iRetSupport = aSupport;
			iCheckAlsPpSupportRequests.AppendL( req );
	        }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetAlsPpSupportCancel
// Cancels an outstanding asynchronous request to get ALS support.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetAlsPpSupportCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETALSPPSUPPORTCANCEL_1,  "TSY: CMmCustomTsy::GetAlsPpSupportCancel");
    // reset the pointer to client data
    iAlsSupport = NULL;

    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetAlsPpSupport );

    // complete
    TInt checkAlsPpSupportRequestsCount = 
        iCheckAlsPpSupportRequests.Count();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETALSPPSUPPORTCANCEL_2,  "TSY: CMmCustomTsy::GetAlsPpSupportCancel - Cancel %d requests", checkAlsPpSupportRequestsCount );

	for ( TInt i=0; i < checkAlsPpSupportRequestsCount ; i++ )
		{
        TCheckAlsPpSupportRequest* req = 
            iCheckAlsPpSupportRequests[ i ];
		//reset the return pointer
        ReqCompleted( req->iReqHandle, KErrCancel );
		}
    // Complete the client request
	// Destroy Array 
 	iCheckAlsPpSupportRequests.ResetAndDestroy();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetAlsPpSupport
// Completes a GetAlsPpSupport request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetAlsPpSupport(
    RMmCustomAPI::TAlsSupport aAlsSupport,
    TInt aErrorCode )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETALSPPSUPPORT_1,  "TSY: CMmCustomTsy::CompleteGetAlsPpSupport - ALS support: %d, Error: %{TSymbianErrorCodes}", aAlsSupport, aErrorCode );
    TBool status = EFalse;
    
    // Check if the get was called internally during boot
    if( iMmPhoneTsy->GetAlsQueryInBoot() )
        {
    	if( RMmCustomAPI::EAlsSupportOn == aAlsSupport )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETALSPPSUPPORT_2,  "TSY: CMmCustomTsy::CompleteGetAlsPpSupport - ALS supported by SIM" );
            status = ETrue;
            }
        //Update ALS status from sim
        iMmPhoneTsy->SetAlsState( status );
    	iMmPhoneTsy->SetAlsQueryInBoot( EFalse );
        }
    
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetAlsPpSupport );

    // check if request was called
    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // set the information for the client
            *iAlsSupport = aAlsSupport;

            if( RMmCustomAPI::EAlsSupportOn == aAlsSupport )
            	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETALSPPSUPPORT_3,  "TSY: CMmCustomTsy::CompleteGetAlsPpSupport - ALS supported by SIM" );
            	status = ETrue;
            	}
            //Update ALS status from sim
            iMmPhoneTsy->SetAlsState( status );
            }

        // reset the internal variable
        iAlsSupport = NULL;
        // complete
        TInt checkAlsPpSupportRequestsCount = 
            iCheckAlsPpSupportRequests.Count();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETALSPPSUPPORT_4,  "TSY: CMmCustomTsy::CompleteGetAlsPpSupport - Complete %d requests", checkAlsPpSupportRequestsCount );

		for ( TInt i=0; i < checkAlsPpSupportRequestsCount ; i++ )
			{
            TCheckAlsPpSupportRequest* req = 
                iCheckAlsPpSupportRequests[ i ];
            
            //set value in client side
            *(req->iRetSupport) = aAlsSupport;
			//reset the return pointer
            ReqCompleted( req->iReqHandle, aErrorCode );
			}
        // Complete the client request
		//Destroy Array 
 		iCheckAlsPpSupportRequests.ResetAndDestroy();
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetCipheringInfoL
// Get Ciphering Info
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetCipheringInfoL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TCipheringInfo* aInfo )
    {
    // this is special case, only called when Ciphering indicator
    // is forced off for some operators
    if ( iMmPhoneTsy->GetNetTsy()->CipheringIndicatorForcedOff() )
        {
        aInfo->iIndStatus = EFalse;
        aInfo->iCiphStatus = EFalse;

        ReqCompleted( aTsyReqHandle, KErrNone );
        }
    // normal case
    else
        {
        TTsyReqHandle getCipheringInfoHandle =
            iTsyReqHandleStore->GetTsyReqHandle( ECustomTsyGetCipheringInfo );

        if ( 0 < getCipheringInfoHandle )
            {
            // The request is already in processing because of previous request
            // Complete request with status value informing the client about
            // the situation.
            ReqCompleted( aTsyReqHandle, KErrServerBusy );
            }
        else
            {
            // save pointer to client space
            iRetCipheringInfo = aInfo;

            // call DOS (no packed parameters)
            TInt ret = Phone()->MessageManager()->HandleRequestL(
                ECustomGetCipheringInfoIPC );

            // check the correct completion of the call to the extension
            if ( KErrNone != ret )
                {
                ReqCompleted( aTsyReqHandle, ret );
                }
            else
                {
                // Save the req handle type
                iReqHandleType = ECustomTsyGetCipheringInfo;
                }
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetCipheringInfo
// Completes a GetCipheringInfo request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetCipheringInfo(
    TBool aCipherIndStatus,
    TInt aErrorCode )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetCipheringInfo );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // set the information for the client
            iRetCipheringInfo->iIndStatus = aCipherIndStatus;
            iRetCipheringInfo->iCiphStatus = iCipheringInfo.iCiphStatus;
            }

        // reset the internal variable
        iRetCipheringInfo = NULL;
        // complete
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetCipheringInfoCancel
// This function cancels an outstanding GetCipheringInfo request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetCipheringInfoCancel()
    {
    // reset the pointer to client space
    iRetCipheringInfo = NULL;

    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetCipheringInfo );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyCipheringInfoChange
// Notifies a client of ciphering info change.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyCipheringInfoChange(
    RMmCustomAPI::TCipheringInfo* aInfo )
    {
    // save pointer to client data
    iRetNotifyCipheringInfoChange = aInfo;

    // set the reqhandle type
    iReqHandleType = ECustomTsyNotifyCipheringInfoChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyCipheringInfoChangeCancel
// Cancels an outstanding NotifyCipheringInfoChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyCipheringInfoChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // reset the pointer to client data
    iRetNotifyCipheringInfoChange = NULL;

    // reset the reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyCipheringInfoChange );

    // complete
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyCipheringInfoChange
// Completes the NotifyCipheringInfoChange method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyCipheringInfoChange(
    TBool aCipherIndStatus,
    TBool aCipherStatus,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYCIPHERINGINFOCHANGE_1, "TSY:CMmCustomTsy::CompleteNotifyCipheringInfoChange entered");
    // this is special case, only called when Ciphering indicator
    // is forced off for some operators
    if ( iMmPhoneTsy->GetNetTsy()->CipheringIndicatorForcedOff() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYCIPHERINGINFOCHANGE_2, "TSY:CMmCustomTsy::CompleteNotifyCipheringInfoChange: Ciphering forced off");
        TTsyReqHandle handle = iTsyReqHandleStore->ResetTsyReqHandle(
            ECustomTsyNotifyCipheringInfoChange );

        if ( ECustomTsyReqHandleUnknown != handle )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYCIPHERINGINFOCHANGE_3, "TSY:CMmCustomTsy::CompleteNotifyCipheringInfoChange: Ciphering forced off, request completed");
            iRetNotifyCipheringInfoChange->iIndStatus = EFalse;
            iRetNotifyCipheringInfoChange->iCiphStatus = ETrue;

            iRetNotifyCipheringInfoChange = NULL;

            ReqCompleted( handle, KErrNone );
            }
        }

     // check if information has changed, continue only in case is changed
    else
        {
        if ( iCipheringInfo.iIndStatus  != aCipherIndStatus ||
            iCipheringInfo.iCiphStatus != aCipherStatus )
            {
            // save the information in the custom TSY
            iCipheringInfo.iIndStatus = aCipherIndStatus;
            iCipheringInfo.iCiphStatus = aCipherStatus;

            // reset req handle. Returns the deleted req handle
            TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                ECustomTsyNotifyCipheringInfoChange );

            // check if handle exists
            if ( ECustomTsyReqHandleUnknown != reqHandle )
                {
                // set the information for the client
                iRetNotifyCipheringInfoChange->iIndStatus =
                    iCipheringInfo.iIndStatus;
                iRetNotifyCipheringInfoChange->iCiphStatus =
                    iCipheringInfo.iCiphStatus;

                // reset the varible
                iRetNotifyCipheringInfoChange = NULL;

                // complete
                ReqCompleted( reqHandle, aErrorCode );

                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyNSPSStatus
// Notifies a client of a change in NSPS (No Service Power Save) status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyNSPSStatus(
    RMmCustomAPI::TNspsStatus* aNspsStatus )
    {
    // save pointer to client data
    iRetNspsStatus = aNspsStatus;

    // set the reqhandle type
    iReqHandleType = ECustomTsyNotifyNSPSStatus;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyNSPSStatus
// Completes the notification of a change in NSPS (No Service Power Save)
// status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyNSPSStatus(
    TBool aNspsStatus )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyNSPSStatus );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // set the information for the client
        if ( aNspsStatus ) // the NSPS mode is ON
            {
            *iRetNspsStatus = RMmCustomAPI::ENspsOn;
            }
        else
            {
            *iRetNspsStatus = RMmCustomAPI::ENspsOff;
            }

        // reset the varible
        iRetNspsStatus = NULL;

        // complete
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyNSPSStatusCancel
// Cancels an outstanding notification of a change in NSPS (No Service Power
// Save) status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyNSPSStatusCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // reset pointer to client data
    iRetNspsStatus = NULL;

    // reset reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle( ECustomTsyNotifyNSPSStatus );

    // complete with KErrCancel
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NetWakeupL
// Send  NetWakeUp request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NetWakeupL(
    const TTsyReqHandle aTsyReqHandle )
    {
    // intialize ret value
    TInt ret ( KErrNone );

    // call DOS (no packed parameters)
    ret = Phone()->MessageManager()->HandleRequestL( ECustomNetWakeupIPC );

    // check result
    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // Save the req handle type
        iReqHandleType = ECustomTsyNetWakeup;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNetWakeup
// Completes a NetWakeup request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNetWakeup(
     TInt aErrorCode )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNetWakeup );

    // check the handle and complete
    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmCustomTsy::SetTypeOfResponse
// Sets the type of response for a given handle.
// Automatic mode includes an automatic response in case of no response from
// the DOS in a specified time.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::SetTypeOfResponse(
    const TInt aReqHandleType,
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );
    TInt ipc( 0 );

    switch ( aReqHandleType )
        {
        case ECustomTsyCallGsmBlackListClear:
            timeOut = KMmCustomCallGsmBlackListClearTimeOut;
            ipc = ECustomClearCallBlackListIPC;
            break;
        case ECustomTsyTerminateCall:
            timeOut = KMmCustomTerminateCallTimeOut;
            ipc = ECustomTerminateCallIPC;
            break;
        case ECustomTsyGetAlsBlocked:
            timeOut = KMmCustomGetAlsBlockedTimeOut;
            ipc = ECustomGetAlsBlockedIPC;
            break;
        case ECustomTsySetAlsBlocked:
            timeOut = KMmCustomSetAlsBlockedTimeOut;
            ipc = ECustomSetAlsBlockedIPC;
            break;
        case ECustomTsyGetCipheringInfo:
            timeOut = KMmCustomGetCipheringInfoTimeOut;
            ipc = ECustomGetCipheringInfoIPC;
            break;
        case ECustomTsyCheckEmergencyNumber:
            timeOut = KMmCustomCheckEmergencyNumberTimeout;
            ipc = ECustomCheckEmergencyNumberIPC;
            break;
        case ECustomTsyGetNetworkOperatorName:
            timeOut = KMmCustomGetNetworkOperatorNameTimeout;
            ipc = ECustomGetNetworkProviderNameIPC;
            break;
        case ECustomTsyCheckTwoDigitDialSupport:
            timeOut = KMmCustomCheckTwoDigitDialTimeout;
            ipc = ECustomCheckTwoDigitDialSupportIPC;
            break;
        case ECustomTsyGetOperatorName:
            timeOut = KMmCustomGetOperatorNameTimeout;
            ipc = ECustomGetOperatorNameIPC;
            break;
        case ECustomTsyGetProgrammableOperatorLogo:
            timeOut = KMmCustomGetProgrammableOperatorLogoTimeout;
            ipc = ECustomGetProgrammableOperatorLogoIPC;
            break;
        case ECustomTsyResetNetServer:
            timeOut = KMmCustomResetNetServer;
            ipc = ECustomResetNetServerIPC;
            break;
        case ECustomTsyGetSimFileInfo:
            timeOut = KMmCustomReadSimFile;
            ipc = ECustomReadSimFileIPC;
            break;
        case ECustomTsyGetLifeTimerInfo:
            timeOut = KMmCustomReadSimFile;
            ipc = ECustomGetLifeTimeIPC;
            break;
        case ECustomTsyIMSAuthenticate:
            timeOut = KMmCustomIMSAuthenticate;
            ipc = EMobilePhoneIMSAuthenticate;
            break;
        case ECustomTsyGetSimAuthenticationData:
            timeOut = KMmCustomGetSimAuthenticationData;
            ipc = ECustomGetSimAuthenticationDataIPC;
            break;
        case ECustomTsySetDriveMode:
            timeOut = KMmCustomSetDriveModeTimeOut;
            ipc = ECustomSetDriveModeIPC;
            break;
        case ECustomTsyReadHSxPAStatus:
            timeOut = KMmCustomTsyReadHSxPAStatusTimeOut;
            ipc = ECustomReadHSxPAStatusIPC;
            break;
        case ECustomTsyWriteHSxPAStatus:
            timeOut = KMmCustomTsyWriteHSxPAStatusTimeOut;
            ipc = ECustomWriteHSxPAStatusIPC;
            break;
        case ECustomTsyGetIccCallForwardingStatus:
        	timeOut = KMmCustomTsyGetIccCallForwardingStatusTimeOut;
        	ipc = ECustomGetIccCallForwardingStatusIPC;
        	break;
        case ECustomTsyGetCellInfo:
            timeOut = KMmCustomTsyGetCellInfoTimeOut;
            ipc = ECustomGetCellInfoIPC;
            break;
       	case ECustomTsyGetUSIMServiceSupport:
            timeOut = KMmCustomGetUSIMServiceSupportTimeOut;
            ipc = ECustomGetServiceTableSupportbyApplicationIPC;
            break;
        case ECustomTsyGetSystemNetworkBand:
            timeOut = KMmCustomTsyGetSystemNetworkBandTimeOut;
            ipc = ECustomGetBandSelectionIPC;
            break;
        default:
            // Does not use timer
            iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType,
                aTsyReqHandle );
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
// CMmCustomTsy::Complete
// Completes the request due the timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::Complete(
    TInt aReqHandleType,
    TInt aError,
    TInt aIPC )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETE_1,  "TSY: CMmCustomTsy::Complete - ReqHandleType: %d Error: %{TSymbianErrorCodes}", aReqHandleType, aError );
    TBool subTsyReqFound = EFalse;
    TInt max = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    // Check Custom TSY's subsystems' req handles first
    for ( TInt i = 0; max > i; i++ )
        {
        // check which custom object handles this IPC
        if ( ( NULL != subTsyPtr[i] ) &&
             ( EFalse != subTsyPtr[i]->SupportingIPC( aIPC ) ) )
            {
            // call complete method of the right custom object
            subTsyPtr[i]->Complete( aReqHandleType, aError );
            i = max;
            subTsyReqFound = ETrue;
            }
        }

    // If the reg handle was not found on Custom TSY's subsystems check the
    // Custom TSY's request handles
    if ( !subTsyReqFound )
        {
        TBuf<1> name(KNullDesC);
        RMmCustomAPI::TOperatorNameInfo operatorNameInfo;
        RMmCustomAPI::TLifeTimeData timeData;

        // All possible Custom TSY req handle types are listed in the
        // switch case below.
        switch ( aReqHandleType )
            {
            case ECustomTsyCallGsmBlackListClear:
                CompleteClearCallBlackList ( aError );
                break;
            case ECustomTsyTerminateCall:
                CompleteTerminateCall( aError );
                break;
            case ECustomTsyGetAlsBlocked:
                CompleteGetAlsBlocked( RMmCustomAPI::EBlockStatusUnknown,
                    aError );
                break;
            case ECustomTsySetAlsBlocked:
                CompleteSetAlsBlocked( aError );
                break;
            case ECustomTsyGetCipheringInfo:
                CompleteGetCipheringInfo( EFalse, aError );
                break;
            case ECustomTsyCheckEmergencyNumber:
                CompleteCheckEmergencyNumber( 0, aError );
                break;
            case ECustomTsyGetNetworkOperatorName:
                CompleteGetNetworkOperatorName( &name, aError );
                break;
            case ECustomTsyCheckTwoDigitDialSupport:
                CompleteCheckTwoDigitDialSupport(
                    RMmCustomAPI::ETwoDigitDialSupportOff, aError );
                break;
            case ECustomTsyGetOperatorName:
                // we don't initialize as this is response to error situation
                CompleteGetOperatorName( operatorNameInfo , aError );
                break;
            case ECustomTsyResetNetServer:
                CompleteResetNetServer( aError );
                break;
            case ECustomTsyGetSimFileInfo:
                CompleteGetSimFileInfo( NULL, aError );
                break;
            case ECustomTsyGetLifeTimerInfo:
                CompleteGetLifeTime( timeData, aError );
                break;
            case ECustomTsyGetProgrammableOperatorLogo:
                CompleteGetProgrammableOperatorLogo( NULL, NULL, aError );
                break;
            case ECustomTsyGetSimAuthenticationData:
                CompleteSimAuthentication( NULL, aError );
                break;
            case ECustomTsyIMSAuthenticate:
                CompleteImsAuthentication( NULL, aError );
                break;
            case ECustomTsySetDriveMode:
                CompleteSetDriveMode( aError );
                break;
            case ECustomTsyReadHSxPAStatus:
                CompleteReadHSxPAStatus( NULL, aError );
                break;
            case ECustomTsyWriteHSxPAStatus:
                CompleteWriteHSxPAStatus( aError );
                break;
            case ECustomTsyGetCellInfo:
                CompleteGetCellInfo( NULL, aError );
                break;
            case ECustomTsyGetUSIMServiceSupport:
                CompleteGetUSIMServiceSupport( NULL, aError );
                break;
            case ECustomTsyGetSystemNetworkBand:
                CompleteGetSystemNetworkBand(
                    RMmCustomAPI::ENetworkBandAny,
                    RMmCustomAPI::KCapsNetworkModeDual,
                    aError );
                break;
            case ECustomTsyGetIccCallForwardingStatus:
                CompleteGetIccCallForwardingStatus( NULL, aError );
                break;                
            default:
                break;
            }
        }
    }
#endif // REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmCustomTsy::ClearCallBlackList
// Clears the Call Blacklist.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::ClearCallBlackListL(
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt retValue ( KErrNone );

    TTsyReqHandle reqhandle =
        iTsyReqHandleStore->GetTsyReqHandle(
            ECustomTsyCallGsmBlackListClear );

    if ( 0 < reqhandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // call DOS and initialize ret value
        // no parameters for DOS call
        retValue = Phone()->MessageManager()->
            HandleRequestL( ECustomClearCallBlackListIPC );

        // check result
        if ( KErrNone != retValue )
            {
            ReqCompleted( aTsyReqHandle, retValue );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = ECustomTsyCallGsmBlackListClear;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteClearCallBlackList
// Completes a ClearCallBlackList request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteClearCallBlackList(
    TInt aErrorCode )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyCallGsmBlackListClear );

    // check the handle and complete
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::ClearCallBlackListCancel
// Cancels an outstanding ClearCallBlackList request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::ClearCallBlackListCancel()
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyCallGsmBlackListClear );
    // complete with KErrCancel
    // check the handle and complete
    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetAirTimeDuration
// Gets the air time duration information from CMmPhoneTsy.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetAirTimeDuration(
    const TTsyReqHandle aTsyReqHandle,
    TTimeIntervalSeconds* aTime )
    {
    // get the air time duration from phone
    Phone()->GetAirTimeDuration( *aTime );

    // complete with KErrNone
    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CheckEmergencyNumberL
// Checks whether given number is emergency number or not.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::CheckEmergencyNumberL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TEmerNumberCheckMode* aNumberMode,
    TBool* aResult )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CHECKEMERGENCYNUMBERL_1, "TSY: CMmCustomTsy::CheckEmergencyNumberL number=%S", (aNumberMode->iNumber) );

    TInt ret(KErrGeneral);

    TTsyReqHandle requestHandle =
        iTsyReqHandleStore->GetTsyReqHandle( ECustomTsyCheckEmergencyNumber );

    // 3rd Dial check number
    if( iISVDialNumberCheck && 0 == requestHandle )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CHECKEMERGENCYNUMBERL_2, "TSY: CMmCustomTsy::CheckEmergencyNumberL - 3rd party client nbr check aResult  = %d" , aResult );
        iEmergencyNumberCheckMode   = aNumberMode;
        iEmergencyNumberCheckResult = aResult;

        // parameter for DOS call: a TEmerNumberCheckMode
        CMmDataPackage dataPackage;
        dataPackage.PackData( aNumberMode );
        ret = Phone()->MessageManager()->HandleRequestL(
            ECustomCheckEmergencyNumberIPC, &dataPackage );
        }

    //Normal case
    else
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CHECKEMERGENCYNUMBERL_3, "TSY: CMmCustomTsy::CheckEmergencyNumberL - aResult  = %d" , aResult );

        if ( ( 0 < requestHandle || iISVDialNumberCheck ) && 0 != aTsyReqHandle )
            {
            // The request is already in processing because of previous request
            ret = KErrServerBusy;
            }
        
        else if( iISVDialNumberCheck && 0 == aTsyReqHandle )
            {
            ret = KErrServerBusy;
            }
        
        else
            {
            // save pointers
            iEmergencyNumberCheckMode   = aNumberMode;
            iEmergencyNumberCheckResult = aResult;

            // parameter for DOS call: a TEmerNumberCheckMode
            CMmDataPackage dataPackage;
            dataPackage.PackData( aNumberMode );
            ret = Phone()->MessageManager()->HandleRequestL(
                    ECustomCheckEmergencyNumberIPC, &dataPackage );

            // check the correct completion of the call
            if ( KErrNone == ret )
                {
                // Save the req handle type
                iReqHandleType = ECustomTsyCheckEmergencyNumber;
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteCheckEmergencyNumber
// Completes checking of emergency number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteCheckEmergencyNumber(
    RMmCustomAPI::TMobileTelNumber* aTelNumber,
    TInt aErrorValue )
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETECHECKEMERGENCYNUMBER_1, "TSY: CMmCustomTsy::CompleteCheckEmergencyNumber" );

    // 3rd party number check completion0
    if( iISVDialNumberCheck )
        {
        //Reset
        TBool isEmergencyNbr = EFalse;

        if ( KErrNone == aErrorValue && NULL == aTelNumber )
            {
            *iEmergencyNumberCheckResult = ETrue;
            isEmergencyNbr = EFalse;
            }
        else
            {
            *iEmergencyNumberCheckResult = ETrue;
            isEmergencyNbr = ETrue;
            }

    // check is the Dial cancel NULL
    if( iISVDialNumberCheckObject )
      {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETECHECKEMERGENCYNUMBER_2, "TSY: CMmCustomTsy::CompleteCheckEmergencyNumber - Complete3rdPartyCallNbrCheck isEmergencyNbr = %d",isEmergencyNbr );
      iISVDialNumberCheckObject->Complete3rdPartyCallNbrCheck(
        isEmergencyNbr );
      }

      //reset pointer
      iISVDialNumberCheckObject = NULL;
      iISVDialNumberCheck = EFalse;
      }

    // Normal case
    else
      {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyCheckEmergencyNumber );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( KErrNone == aErrorValue )
            {
            //set the information for the client
        if ( aTelNumber )
            {
            *iEmergencyNumberCheckResult = ETrue;
            iEmergencyNumberCheckMode->iNumber = *aTelNumber;
            }
        else
            {
            *iEmergencyNumberCheckResult = EFalse;
            }
        }
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETECHECKEMERGENCYNUMBER_3, "TSY: CMmCustomTsy::CompleteCheckEmergencyNumber number=%S", (iEmergencyNumberCheckMode->iNumber) );

        // reset the internal variable
        iEmergencyNumberCheckResult = NULL;
        iEmergencyNumberCheckMode   = NULL;
        // complete
        ReqCompleted( reqHandle, aErrorValue );
        }

        }

    //Reset
    iISVDialNumberCheck = EFalse;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CheckEmergencyNumberCancel
// Cancels checking of emergency number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::CheckEmergencyNumberCancel()
    {
    iEmergencyNumberCheckResult = NULL;

    // reset the req handle.Returns the deleted req handle.
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyCheckEmergencyNumber );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyPhoneBookCacheReady
// Notifies client when phonebookcache is ready.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyPhoneBookCacheReady(
    const TTsyReqHandle,
    TName*  aPndName )
    {
    iPndName = aPndName;

    iReqHandleType = ECustomTsyNotifyCacheReady;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyPhoneBookCacheReady
// Completes notify phonebook cache ready.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyPhoneBookCacheReady(
    TName& aPndName,
    TInt aResult )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyCacheReady );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        *iPndName = aPndName;

        iPndName = NULL;

        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyPhoneBookCacheReadyCancel
// Cancels notify phonebook cache ready notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyPhoneBookCacheReadyCancel(
    const TTsyReqHandle )
    {
    // reset request handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyCacheReady );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetPndCacheStatus
// This function gets current status of phonebook caching.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetPndCacheStatus(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TPndCacheStatus* aPndStatus,
    const TName* aPndName )
    {
    
    TInt error = KErrNone;
    // used phonebook is Adn
    if ( 0 == aPndName->CompareF( KETelIccAdnPhoneBook ) )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETPNDCACHESTATUS_1, "TSY: CMmCustomTsy::GetPndCacheStatus - ADN phonebook status: %d", iAdnCacheStatus);
        *aPndStatus = iAdnCacheStatus;
        }
    // used phonebook is Fdn
    else if ( 0 == aPndName->CompareF( KETelIccFdnPhoneBook ) )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETPNDCACHESTATUS_2, "TSY: CMmCustomTsy::GetPndCacheStatus - FDN phonebook status: %d", iFdnCacheStatus);
        *aPndStatus = iFdnCacheStatus;
        }
    // used phonebook doesn't have cache
    else if ( ( 0 == aPndName->CompareF( KETelIccBdnPhoneBook ) ) ||
              ( 0 == aPndName->CompareF( KETelIccSdnPhoneBook ) ) ||
              ( 0 == aPndName->CompareF( KETelIccVoiceMailBox ) ) )
        {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETPNDCACHESTATUS_3, "TSY: CMmCustomTsy::GetPndCacheStatus - No cache for: %S ", *aPndName);
        *aPndStatus = RMmCustomAPI::ECacheNotUsed;
        }
    else
        {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETPNDCACHESTATUS_4, "TSY: CMmCustomTsy::GetPndCacheStatus - No cache found for: %S ", *aPndName);
        error = KErrArgument;
        }

    ReqCompleted( aTsyReqHandle, error );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::UpdateCacheStatus
// This method updates phonebook caching status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::UpdateCacheStatus(
    RMmCustomAPI::TPndCacheStatus aPndStatus,
    TName& aPndName )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_UPDATECACHESTATUS_1, "TSY: CMmCustomTsy::UpdateCacheStatus - Phonebook: %S status: %d", aPndName, aPndStatus);
    // if used phonebook is Adn
    if ( 0 == aPndName.CompareF( KETelIccAdnPhoneBook ) )
        {
        iAdnCacheStatus = aPndStatus;
        }
    // else if used phonebook is Fdn
    else if ( 0 == aPndName.CompareF( KETelIccFdnPhoneBook ) )
        {
        iFdnCacheStatus = aPndStatus;
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetNetworkOperatorNameL
// Gets the network operator name from the SIM card.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetNetworkOperatorNameL(
    TDes* aNetworkOperatorName )
    {
    // call DOS (no packed parameters)
    TInt ret = Phone()->MessageManager()->HandleRequestL(
        ECustomGetNetworkProviderNameIPC );

    if ( KErrNone == ret )
        {
        // save pointer to client space and req handle type
        iNetworkOperatorName = aNetworkOperatorName;        
        iReqHandleType = ECustomTsyGetNetworkOperatorName;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetNetworkOperatorName
// Completes getting network operator name from SIM card.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetNetworkOperatorName(
    TDes* aName,
    TInt aError )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetNetworkOperatorName );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( ( KErrNone == aError ) && ( NULL != aName ) )
            {
            if ( aName->Length() <= iNetworkOperatorName->MaxLength() )
                {            
                // set the information for the client
                *iNetworkOperatorName = *aName;
                }
            else
                {
                // client delivered too small buffer
                aError = KErrArgument;               
                }
            }

        // reset the internal variable
        iNetworkOperatorName = NULL;
        // complete
        ReqCompleted( reqHandle, aError );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetNetworkOperatorNameCancel
// Cancels getting network operator name from SIM card.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetNetworkOperatorNameCancel()
    {
    // reset the pointer to client space
    iNetworkOperatorName = NULL;

    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetNetworkOperatorName );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SsAdditionalInfoNotification
// Notification request for additional info message.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SsAdditionalInfoNotification(
    RMmCustomAPI::TSsAdditionalInfo* aSsAdditionalInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SSADDITIONALINFONOTIFICATION_1, "TSY: CMmCustomTsy::SsAdditionalInfoNotification");
    iRetSsAdditionalInfo = aSsAdditionalInfo;
    iReqHandleType = ECustomTsyNotifySsAdditionalInfo;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SsAdditionalInfoNotificationCancel
// Cancels notification request for additional info message.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SsAdditionalInfoNotificationCancel(
    const TTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SSADDITIONALINFONOTIFICATIONCANCEL_1, "TSY: CMmCustomTsy::SsAdditionalInfoNotificationCancel");
    iRetSsAdditionalInfo = NULL;

    // reset reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifySsAdditionalInfo );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteSsAdditionalInfoNotification
// Completes SsAdditionalInfo notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteSsAdditionalInfoNotification(
    RMmCustomAPI::TSsAdditionalInfo* aSsAdditionalInfo,
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESSADDITIONALINFONOTIFICATION_1, "TSY: CMmCustomTsy::CompleteSsAdditionalInfoNotification");
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifySsAdditionalInfo );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // copy data to client
        iRetSsAdditionalInfo->iOperationCode =
            aSsAdditionalInfo->iOperationCode;
        iRetSsAdditionalInfo->iAdditionalInfo.Copy(
            aSsAdditionalInfo->iAdditionalInfo );

        // reset pointer to client
        iRetSsAdditionalInfo = NULL;

        // Complete the client request
        ReqCompleted( reqHandle, aError );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CheckTwoDigitDialSupportL
// Checks two digit dialing support from Product Profile.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::CheckTwoDigitDialSupportL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TTwoDigitDialSupport* aSupport )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_CHECKTWODIGITDIALSUPPORTL_1, "TSY: CMmCustomTsy::CheckTwoDigitDialSupportL");
    // save pointer to client space
    iTwoDigitDialSupport = aSupport;

    // call DOS
    TInt error = Phone()->MessageManager()->HandleRequestL(
        ECustomCheckTwoDigitDialSupportIPC );

    if ( KErrNone != error )
        {
        ReqCompleted( aTsyReqHandle, error );
        }
    else
        {
        // Save the req handle type
        iReqHandleType = ECustomTsyCheckTwoDigitDialSupport;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CheckTwoDigitDialSupport
// Completes a CheckTwoDigitDialSupport request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteCheckTwoDigitDialSupport(
     RMmCustomAPI::TTwoDigitDialSupport aTwoDigitDialSupport,
     TInt aErrorCode )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyCheckTwoDigitDialSupport );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // set the information for the client
            *iTwoDigitDialSupport = aTwoDigitDialSupport;
            }

        // reset the internal variable
        iTwoDigitDialSupport = NULL;
        // complete
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CheckTwoDigitDialSupportCancel
// Cancels an outstanding CheckTwoDigitDialSupport request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::CheckTwoDigitDialSupportCancel()
    {
    // reset the pointer to client data
    iTwoDigitDialSupport = NULL;

    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyCheckTwoDigitDialSupport );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetOperatorNameL
// Get the programmable operator name of the current network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetOperatorNameL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TOperatorNameInfo* aOperatorNameInfo )
    {
    // save a pointer to client space
    iRetOperatorNameInfo = aOperatorNameInfo;

    // call DOS (no packed parameters)
    TInt ret = Phone()->MessageManager()->HandleRequestL(
        ECustomGetOperatorNameIPC );

    // check result
    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // Save the req handle type
        iReqHandleType = ECustomTsyGetOperatorName;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetOperatorNameCancel
// Cancels an outstanding GetOperatorName request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetOperatorNameCancel()
    {
    // reset the pointer to client space
    iRetOperatorNameInfo = NULL;

    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetOperatorName );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetOperatorName
// Completes an outstanding GetOperatorName request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetOperatorName(
    RMmCustomAPI::TOperatorNameInfo& aOperatorNameInfo,
    TInt aErrorValue )
    {
    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetOperatorName );

    // check the validity of the handler
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorValue )
            {
            // Copy the programmable operator name info in the client space
            *iRetOperatorNameInfo = aOperatorNameInfo;
            }
        // reset the pointer to client space
        iRetOperatorNameInfo = NULL;

        // complete operation
        ReqCompleted ( reqHandle, aErrorValue );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifySsNetworkEvent
// Notifies the client about a network generated SS event.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifySsNetworkEvent(
    RMmCustomAPI::TSsTypeAndMode* aSsTypeAndMode,
    RMmCustomAPI::TSsInfo* aSsInfo )
    {
    // save pointer to client data
    iSsTypeAndMode = aSsTypeAndMode;
    iSsInfo = aSsInfo;

    // set the reqhandle type
    iReqHandleType = ECustomTsySsNotification;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifySsNetworkEventCancel
// Cancels an outstanding NotifySsNetworkEvent.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifySsNetworkEventCancel()
    {
    // erase pointers to client data
    iSsTypeAndMode = NULL;
    iSsInfo = NULL;

    // reset the reqhandle
    TTsyReqHandle tsyReqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( ECustomTsySsNotification );

    if ( ECustomTsyReqHandleUnknown != tsyReqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( tsyReqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifySsNetworkEvent
// Completes NotifySsNetworkEvent.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifySsNetworkEvent(
   RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode,
   RMmCustomAPI::TSsInfo& aSsInfo,
   TInt aErrorCode )
    {
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( ECustomTsySsNotification );

    if ( ECustomTsyReqHandleUnknown != tsyReqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            // save data in client space
            *iSsTypeAndMode = aSsTypeAndMode;
            *iSsInfo = aSsInfo;
            }
        // reset the pointers to client space
        iSsTypeAndMode = NULL;
        iSsInfo = NULL;
        // complete
        ReqCompleted( tsyReqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetProgrammableOperatorLogoL
// Gets the programmable operator logo.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetProgrammableOperatorLogoL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TOperatorId* aOperatorId,
    RMmCustomAPI::TOperatorLogo* aLogo )
    {
    TTsyReqHandle reqhandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyGetProgrammableOperatorLogo );

    if ( 0 < reqhandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save a pointer to client space
        iRetOperatorLogoInfo = aLogo;
        iRetOperatorIdInfo = aOperatorId;

        // call DOS
        // packed parameter: RMmCustomAPI::TOperatorId
        CMmDataPackage dataPackage;
        dataPackage.PackData( aOperatorId );
        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomGetProgrammableOperatorLogoIPC, &dataPackage );

        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = ECustomTsyGetProgrammableOperatorLogo;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetProgrammableOperatorLogoCancel
// Cancels an outstanding GetProgrammableOperatorLogo request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetProgrammableOperatorLogoCancel()
    {
    // reset the pointers
    iRetOperatorLogoInfo = NULL;
    iRetOperatorIdInfo = NULL;

    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetProgrammableOperatorLogo );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetProgrammableOperatorLogo
// Completes GetProgrammableOperatorLogo.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetProgrammableOperatorLogo(
    RMmCustomAPI::TOperatorId* aOperatorIdInfo,
    RMmCustomAPI::TOperatorLogo* aLogoData,
    TInt aErrorValue )
    {
   
    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetProgrammableOperatorLogo );
    
    // check the validity of the handler
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorValue)
            {
            // if client has allocated big enough space for logo data, logo data
            // is appended into client space pointer
            if ( ( aLogoData->iOperatorLogo ) &&
                ( aLogoData->iOperatorLogo->Size() <=
                iRetOperatorLogoInfo->iOperatorLogo->MaxSize() ) )
                {
                // Copy the logo info in the client space
                iRetOperatorLogoInfo->iLogoInfo = aLogoData->iLogoInfo;
                // Copy the logo in the client space
                iRetOperatorLogoInfo->iOperatorLogo->Copy( *(
                    aLogoData->iOperatorLogo ) );
                // Copy the operator info in the client space
                *iRetOperatorIdInfo = *aOperatorIdInfo;
                }
            else
                {
                // logo will not fit into client space
                aErrorValue = KErrTooBig;
                }
            }

        // reset the pointers
        iRetOperatorLogoInfo = NULL;
        iRetOperatorIdInfo = NULL;

        // complete operation
        ReqCompleted ( reqHandle, aErrorValue );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SatRefreshCompleteNotification
// Notifies the client of SAT refresh.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SatRefreshCompleteNotification()
    {
    iReqHandleType = ECustomTsySatRefreshCompleteNotification;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SatRefreshCompleteNotificationCancel
// Cancels an outstanding asynchronous SatRefreshCompleteNotification request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SatRefreshCompleteNotificationCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // reset reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsySatRefreshCompleteNotification );

    // complete with KErrCancel
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteSatRefreshCompleteNotification
// Completes the notification of SAT refresh to the client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteSatRefreshCompleteNotification()
    {
    //Note iRefresh is never set to ETrue
    
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsySatRefreshCompleteNotification );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // complete
        ReqCompleted( reqHandle, KErrNone );
        // set sat refresh flag off
        SetSatRefreshStatus ( EFalse );
        //Reset ME ALS info
        Phone()->ResetMEAlsInfo();
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SetSatRefreshStatus
// Sets aSatRefreshStatus into internal variable iIsRefresh.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmCustomTsy::SetSatRefreshStatus(
    TBool aSatRefreshStatus )
    {
    iIsRefresh = aSatRefreshStatus;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::ResetNetServerL
// Resets the net server to previous network selection when user doesn't select
// a network from the network list got in a search.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::ResetNetServerL(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_RESETNETSERVERL_1, "TSY: CMmCustomTsy::ResetNetServerL");
    // call DOS (no packed parameters)
    TInt ret = Phone()->MessageManager()->HandleRequestL(
        ECustomResetNetServerIPC );

    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // Save the req handle type
        iReqHandleType = ECustomTsyResetNetServer;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteResetNetServer
// Completes a ResetNetServer request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteResetNetServer(
    TInt aErrorValue )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETERESETNETSERVER_1, "TSY: CMmCustomTsy::CompleteResetNetServer - Error: %{TSymbianErrorCodes}", aErrorValue );
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( ECustomTsyResetNetServer );

    // Check the reqHandle
    if ( ECustomTsyReqHandleUnknown != tsyReqHandle )
        {
        // Check that the error value is not KErrCancel, because this will
        // cause panic on the ETEL side - error: EZGO-6WVD5N  
        if ( KErrCancel == aErrorValue )
            {
            // Convert the error value from KErrCancel to KErrNotReady
            aErrorValue = KErrNotReady; 
            }
            
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETERESETNETSERVER_2, "TSY: CMmCustomTsy::CompleteResetNetServer - complete with error value: %{TSymbianErrorCodes}", aErrorValue );
        ReqCompleted( tsyReqHandle, aErrorValue );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyNetworkConnectionFailure
// Notifies of a network connection failure. This failure is so severe that
// the only way to recover is to restart the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyNetworkConnectionFailure()
    {
    iReqHandleType = ECustomTsyNotifyNetworkConnectionFailure;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyNetworkConnectionFailureCancel
// Cancels a NotifyNetworkConnectionFailure.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyNetworkConnectionFailureCancel()
    {
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyNetworkConnectionFailure );

    if ( ECustomTsyReqHandleUnknown != tsyReqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( tsyReqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyNetworkConnectionFailure
// Completes NotifyNetworkConnectionFailure.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyNetworkConnectionFailure()
    {
    // If SIM is inserted complete network connection failure
    if ( iMmPhoneTsy->NosBootState()->iSIMReady )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYNETWORKCONNECTIONFAILURE_1, "TSY: CMmCustomTsy::CompleteNotifyNetworkConnectionFailure");
        // reset the reqhandle
        TTsyReqHandle tsyReqHandle =
            iTsyReqHandleStore->ResetTsyReqHandle(
            ECustomTsyNotifyNetworkConnectionFailure );

        // Check the reqHandle
        if( ECustomTsyReqHandleUnknown != tsyReqHandle )
            {
            ReqCompleted ( tsyReqHandle, KErrNone );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SsRequestCompleteNotification
// Notification request for "SS request complete".
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SsRequestCompleteNotification(
    TInt* aStatus )
    {
    iRetSsRequestComplete = aStatus;
    iReqHandleType = ECustomTsyNotifySsRequestComplete;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteSsRequestCompleteNotification
// Completes notification request for "SS request complete".
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteSsRequestCompleteNotification(
    TInt aSsStatus,
    TInt aError )
    {
    // Reset the req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifySsRequestComplete );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( NULL == iRetSsRequestComplete )
            {
            ReqCompleted( reqHandle, KErrCorrupt );
            }
        else
            {
            *iRetSsRequestComplete = aSsStatus;
            iRetSsRequestComplete = NULL;
            ReqCompleted( reqHandle, aError );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SsRequestCompleteNotificationCancel
// Cancels notification request for "SS request complete".
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SsRequestCompleteNotificationCancel(
    const TTsyReqHandle )
    {
    iRetSsRequestComplete = NULL;

    // reset reqhandler
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifySsRequestComplete );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel    	
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetSystemNetworkModesL
// This method gets the supported network modes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetSystemNetworkModesL(
    const TTsyReqHandle aTsyReqHandle,
    TUint32* aSystemNetworkModes )
    {
    TTsyReqHandle reqhandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyGetSystemNetworkModes );

    if ( 0 < reqhandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save pointer to the packaged life timer info
        iSystemNetworkModes = aSystemNetworkModes;
        TBool useCurrentRats = EFalse;
        // call DOS
        CMmDataPackage dataPackage;
        dataPackage.PackData( &useCurrentRats );
        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomGetSystemNetworkModesIPC, &dataPackage );

        // Check response of the extension
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = ECustomTsyGetSystemNetworkModes;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetSystemOrCurrentNetworkModes
// Completes a GetSystemNetworkModes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetSystemOrCurrentNetworkModes(
    TUint32 aSystemNetworkModes,
    TInt aResult )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetSystemNetworkModes );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // set the information for the client
        *iSystemNetworkModes = aSystemNetworkModes;

        // complete
        ReqCompleted( reqHandle, aResult );
        }

    // reset req handle. Returns the deleted req handle
    reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetCurrentSystemNetworkMode );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // set the information for the client
        *iCurrentNetworkMode = aSystemNetworkModes;

        // complete
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetSystemNetworkModesCancel
// Cancels a GetSystemNetworkModes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetSystemNetworkModesCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // reset the pointer to client data
    iSystemNetworkModes = NULL;

    // reset the reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetSystemNetworkModes );

    // complete
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SetSystemNetworkModeL
// This method sets the system network mode.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SetSystemNetworkModeL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TNetworkModeCaps* aSystemNetworkModeCaps )
    {
    TTsyReqHandle reqhandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsySetSystemNetworkMode );

    if ( 0 < reqhandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // call DOS
        RMmCustomAPI::TNetworkModeCaps networkModeCaps =
            *aSystemNetworkModeCaps;
        CMmDataPackage dataPackage;
        dataPackage.PackData( &networkModeCaps );
        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomSetSystemNetworkModeIPC, &dataPackage );

        // Check response of the extension
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = ECustomTsySetSystemNetworkMode;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteSetSystemNetworkMode
// Completes a SetSystemNetworkMode.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteSetSystemNetworkMode(
    TInt aResult )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsySetSystemNetworkMode );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // complete
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetCurrentSystemNetworkModeL
// This function gets the current network mode GSM/UMTS/Dual.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetCurrentSystemNetworkModeL(
    const TTsyReqHandle aTsyReqHandle,
    TUint32* aNetworkMode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETCURRENTSYSTEMNETWORKMODEL_1, "CMmCustomTsy::GetCurrentSystemNetworkModeL");
    TTsyReqHandle reqhandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyGetCurrentSystemNetworkMode );

    if ( 0 < reqhandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        iCurrentNetworkMode = aNetworkMode;
        // call DOS
        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomGetCurrentSystemNetworkModesIPC );

        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            iReqHandleType = ECustomTsyGetCurrentSystemNetworkMode;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//  CMmCustomTsy::GetCurrentSystemNetworkModeCancel
//  Cancel outstanding request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetCurrentSystemNetworkModeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETCURRENTSYSTEMNETWORKMODECANCEL_1, "CMmCustomTsy::GetCurrentSystemNetworkModeCancel");

    // reset pointer to client data
    iCurrentNetworkMode = NULL;

    // reset reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetCurrentSystemNetworkMode );

    // complete with KErrCancel
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetSimFileInfoL
// Gets data from a given location of a given sim file determined by the
// aSimFileInfoPckg parameter. Saves the pointer to the client side store
// buffer for further completion.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetSimFileInfoL(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aSimFileInfoPckg,
    TDes8* aSimResponseBuffer )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETSIMFILEINFOL_1,  "TSY: CMmCustomTsy::GetSimFileInfoL" );
    TTsyReqHandle reqHandle =
        iTsyReqHandleStore->GetTsyReqHandle( ECustomTsyGetSimFileInfo );

    // Unpack the TSimFileInfo data
    RMmCustomAPI::TSimFileInfoPckg* simFileInfoPckg =
        ( RMmCustomAPI::TSimFileInfoPckg* ) aSimFileInfoPckg;

    RMmCustomAPI::TSimFileInfo simFileInfo = ( *simFileInfoPckg )();

    if ( 0 < reqHandle )
        {
        //The request is already in processing because of previous request
        //Check if the request is the same than before
        
        //Check the file path in SIM where file is read, the offset from 
        //beginning of the file and amount of data to read
        if ( ( KErrNotFound != simFileInfo.iPath.Match( iGetSimFileInfoReq.iPath ) ) 
        	&& ( iGetSimFileInfoReq.iOffSet == simFileInfo.iOffSet )
        	&& ( iGetSimFileInfoReq.iSize == simFileInfo.iSize ) )
        	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETSIMFILEINFOL_2,  "TSY: CMmCustomTsy::GetSimFileInfoL - Same info already requested" );

			TGetSimFileInfoRequest* req = new ( ELeave ) TGetSimFileInfoRequest();
			req->iReqHandle = aTsyReqHandle;
			req->iSimFileInfo = aSimResponseBuffer;
			iGetSimFileInfoRequests.AppendL( req );
        	}
        else
        	{
            //Complete request with status value informing the client about
            //the situation.
            ReqCompleted( aTsyReqHandle, KErrServerBusy );
        	}
        }
    else
        {
        iGetSimFileInfoReq = simFileInfo;
        // call DOS
        CMmDataPackage dataPackage;
        dataPackage.PackData( &simFileInfo );
        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomReadSimFileIPC, &dataPackage );

        // check the return value of the call to DOS
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETSIMFILEINFOL_3,  "TSY: CMmCustomTsy::GetSimFileInfoL - Save handle" );
	        // Save the req handle type
	        iReqHandleType = ECustomTsyGetSimFileInfo;
			// Save request in queue for completion
	        TGetSimFileInfoRequest* req = new ( ELeave ) TGetSimFileInfoRequest();
			req->iReqHandle = aTsyReqHandle;
			req->iSimFileInfo = aSimResponseBuffer;
			iGetSimFileInfoRequests.AppendL( req );
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetSimFileInfoCancel
// Cancels an outstanding GetSimFileInfo request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetSimFileInfoCancel( TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETSIMFILEINFOCANCEL_1,  "TSY: CMmCustomTsy::GetSimFileInfoCancel" );
    
	for ( TInt i = 0; i < iGetSimFileInfoRequests.Count(); i++ )
		{
		TGetSimFileInfoRequest* req = iGetSimFileInfoRequests[ i ];
		if ( aTsyReqHandle == req->iReqHandle )
			{
			//reset the return pointer
	        ReqCompleted( req->iReqHandle, KErrCancel );
			
	        iGetSimFileInfoRequests.Remove( i );
	        iGetSimFileInfoRequests.Compress();
	        
	        //Delete TGetSimFileInfoRequest object
	        delete req;
			}
		}

	if ( 0 == iGetSimFileInfoRequests.Count() )
		{
	    // reset the reqhandle
	    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
	        ECustomTsyGetSimFileInfo );
	    
		//Destroy Array
		iGetSimFileInfoRequests.ResetAndDestroy();
		}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetSimFileInfo
// Completes an outstanding GetSimFileInfo request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetSimFileInfo(
    TDesC8* aData,
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETSIMFILEINFO_1,  "TSY: CMmCustomTsy::CompleteGetSimFileInfo" );
    
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( ECustomTsyGetSimFileInfo );
    
    // Check the reqHandle
    if ( ECustomTsyReqHandleUnknown != tsyReqHandle )
        {
        // complete
        TInt getSimFileInfoRequestsCount = iGetSimFileInfoRequests.Count();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETSIMFILEINFO_2,  "TSY: CMmCustomTsy::CompleteGetSimFileInfo - Complete %d requests", getSimFileInfoRequestsCount );
        
		for ( TInt i = 0; i < getSimFileInfoRequestsCount; i++ )
		    {
		    TGetSimFileInfoRequest* req = iGetSimFileInfoRequests[ i ];
		
		    if ( ( KErrNone == aError ) && aData )
		        {
		        // check the size....
		        if ( req->iSimFileInfo->MaxLength() >= aData->Length() )
		            {
		            req->iSimFileInfo->Copy( *aData );
		            ReqCompleted( req->iReqHandle, aError );
		            }
		        else // data is too big for client's buffer
		            {
		            req->iSimFileInfo->Zero();
		            ReqCompleted( req->iReqHandle, KErrOverflow );
		            }
		        }
		    else
	            {
	            req->iSimFileInfo->Zero();
	            ReqCompleted( req->iReqHandle, aError );
	            }
		    }
        }
	
    //Destroy Array 
    iGetSimFileInfoRequests.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetLifeTimeL
// Gets the life timer's information and the manufacturing date of the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetLifeTimeL(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aLifeTimerInfoPckg )
    {
    
    TInt ret( KErrNone );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETLIFETIMEL_1, "TSY: CMmCustomTsy::GetLifeTimeL");
    if ( sizeof(RMmCustomAPI::TLifeTimeData) != aLifeTimerInfoPckg->MaxLength() )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETLIFETIMEL_2, "TSY: CMmCustomTsy::GetLifeTimeL bad size argument");
        // Complete the request with appropiate error        
        return KErrArgument;
        }
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
            ECustomTsyGetLifeTimerInfo  );
    if ( ECustomTsyReqHandleUnknown != reqHandle ) 
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETLIFETIMEL_3,  "TSY: CMmCustomTsy::GetLifeTimeL - ECustomTsyReqHandleUnknown" );
        // The request is already in processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save pointer to the packaged life timer info
        iLifeTimerInfoPckg = aLifeTimerInfoPckg;
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETLIFETIMEL_4,  "TSY: CMmCustomTsy::GetLifeTimeL - send request" );
        iTsyReqHandleStore->SetTsyReqHandle( 
                        ECustomTsyGetLifeTimerInfo, aTsyReqHandle );
        ret = Phone()->MessageManager()->HandleRequestL(
                ECustomGetLifeTimeIPC);
        // Check response of the extension
        if ( KErrNone != ret )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETLIFETIMEL_5,  "TSY: CMmCustomTsy::GetLifeTimeL - DOS layer returned error " );
            TTsyReqHandle tsyReqHandle =
                iTsyReqHandleStore->ResetTsyReqHandle( 
                                    ECustomTsyGetLifeTimerInfo );
            if ( ECustomTsyReqHandleUnknown != tsyReqHandle )
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETLIFETIMEL_6,  "TSY: CMmCustomTsy::GetLifeTimeL - DOS error - unknown handle" );
                }
            ReqCompleted( aTsyReqHandle, ret );
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetLifeTimeCancel
// Cancels an ongoing GetLifeTime request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetLifeTimeCancel()
    {
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetLifeTimerInfo );

    // reset pointer to client data
    iLifeTimerInfoPckg = NULL;

    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetLifeTime
// Completes a GetLifeTime request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetLifeTime(
    RMmCustomAPI::TLifeTimeData& aLifeTimerData,
    TInt aErrorValue )
    {
    // reset the reqhandle
    TTsyReqHandle tsyReqHandle =
        iTsyReqHandleStore->ResetTsyReqHandle( ECustomTsyGetLifeTimerInfo );

    // Check the reqHandle
    if ( ECustomTsyReqHandleUnknown != tsyReqHandle )
        {
        // Check if there is error
        if ( KErrNone == aErrorValue )
            {
            // pack the return information
            RMmCustomAPI::TLifeTimeDataPckg aPackg ( aLifeTimerData);
            // Copy the return info into the client side
            (*iLifeTimerInfoPckg) = aPackg;
            }
        // reset the pointer to client space
        iLifeTimerInfoPckg = NULL;
        // complete
        ReqCompleted( tsyReqHandle, aErrorValue );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::DeliverCode
// Deliveres code and unblock code.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::DeliverCodeL(
    RMobilePhone::TCodeAndUnblockCode aCodes )
    {
    iMmSecurityTsy->DeliverCodeL( aCodes );
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SetDriveModeL
// This method sets Drive Mode status using the aModeStatus variable.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SetDriveModeL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TSetDriveMode* aModeStatus )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SETDRIVEMODEL_1, "TSY: CMmCustomTsy::SetDriveModeL - Req handle: %d, Mode status: %d", aTsyReqHandle, *aModeStatus );
	if ( (iUsingFeatureManager) && (iFeatureControl.FeatureSupported(NFeature::KDriveModeCanRestrictMtCalls) == KFeatureSupported) )
        {
        // get the handle
        TTsyReqHandle setDriveModeHandle =
            iTsyReqHandleStore->GetTsyReqHandle( ECustomTsySetDriveMode );

        if ( setDriveModeHandle > 0 )
            {
            // The request is already in processing because of previous
            // request. Complete request with status value informing the
            // client about the situation.
            ReqCompleted( aTsyReqHandle, KErrServerBusy );
            }
        else
            {
            // call DOS
            // packed parameter: TSetDriveMode (mode status)
            CMmDataPackage dataPackage;
            dataPackage.PackData( aModeStatus );
		    // allow for ECustomSetDriveModeIPC immediate completion
#ifdef REQHANDLE_TIMER
            // set timer for the request
            SetTypeOfResponse ( ECustomTsySetDriveMode, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( ECustomTsySetDriveMode,
                    aTsyReqHandle );
#endif // REQHANDLE_TIMER
            TInt ret = iMmPhoneTsy->MessageManager()->HandleRequestL(
                ECustomSetDriveModeIPC, &dataPackage );

            // check success
            if ( (KErrNone != ret) && (iTsyReqHandleStore->ResetTsyReqHandle(ECustomTsySetDriveMode)) )
                {
                ReqCompleted ( aTsyReqHandle, ret );
                }
            }
        }
    else
        {
        ReqCompleted ( aTsyReqHandle, KErrNotSupported );
        }
	
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteSetDriveMode
// Completes a Set Drive Mode request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteSetDriveMode(
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESETDRIVEMODE_1,  "TSY: CMmCustomTsy::CompleteSetDriveMode - Error code: %{TSymbianErrorCodes}", aErrorCode );
	if ( (iUsingFeatureManager) && (iFeatureControl.FeatureSupported(NFeature::KDriveModeCanRestrictMtCalls) == KFeatureSupported) )
        {
        //Reset req handle, returns the deleted req handle
        TTsyReqHandle reqHandle =
            iTsyReqHandleStore->ResetTsyReqHandle( ECustomTsySetDriveMode );

        //check if request was called
        if( ECustomTsyReqHandleUnknown != reqHandle )
            {
            //complete
            ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::IsRequestPossibleInOffLine
// Checks wether a ETel request can be performed or not while offline mode is
// enabled
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCustomTsy::IsRequestPossibleInOffline(
    TInt aIpc )
    {
    // we assume here that most of the requests are possible, thefore
    // we switch for the not possible ones, optimizing a bit the checking.
    TBool isRequestPossible ( ETrue );
    // yet another switching function, yeah!
    switch ( aIpc )
        {
        //case ECustomSsAdditionalInfoNotificationIPC: //ok (but shouldn't be
                                                       //triggered anyways)
        //case ECustomNotifySsRequestCompleteIPC: //ok (but shouldn't be
                                                  //triggered anyways)
        //case ECustomStartSimCbTopicBrowsingIPC: //ok
        //case ECustomGetNextSimCbTopicIPC: //ok
        //case ECustomDeleteSimCbTopicIPC: //ok
        //case ECustomReadSimFileIPC: //ok
        //case ECustomNotifyDtmfEventIPC: //ok (policy is to leave
                                          //the notifications as ok)
        //case ECustomGetDiagnosticOctetsIPC: //ok (NO?, this only applies to
                                              //call so...should be no)
        //case ECustomSetAlsBlockedIPC: //ok
        //case ECustomGetAlsBlockedIPC: //ok
        //case ECustomCheckAlsPpSupportIPC: //ok
        //case ECustomGetRemoteAlertingToneStatusIPC:
        //case ECustomCallOriginIPC: //ok (only for emergency)
        //case ECustomNotifyAlsBlockedChangedIPC: //ok //Notify Als
                                                       //Blocked Change
        //case ECustomTerminateCallIPC: //ok (or no?) //Terminate a Call
        //case ECustomClearCallBlackListIPC: //ok //Clear call blacklist
        //case ECustomSimLockActivateIPC: //ok
        //case ECustomSimLockDeActivateIPC: //ok
        //case EReadViagHomeZoneParamsIPC: //ok
        //case EReadViagHomeZoneCacheIPC: //ok
        //case EWriteViagHomeZoneCacheIPC: //ok
        //case ECustomGetCipheringInfoIPC: //ok
        //case ECustomNotifyCipheringInfoChangeIPC: //ok
        //case ECustomNotifyNSPSStatusIPC: //ok
        case ECustomNetWakeupIPC: //NO
        //case ECustomGetNetworkProviderNameIPC: //ok //(info is got from sim)
        case ECustomGetOperatorNameIPC: //NO //(there is no operator
                                             //available in offline)
        //case ECustomGetProgrammableOperatorLogoIPC: //ok
        case ECustomResetNetServerIPC: //NO (or yes, but this one is sensible)
        //case ECustomNotifyProgrammableOperatorLogoChangeIPC: //ok
        //case ECustomNotifyNetworkConnectionFailureIPC: //ok
        //case ECustomGetAirTimeDurationIPC: //ok (at least for emergency call)
        //case ECustomNotifyPndCacheReadyIPC: //ok
        //case ECustomGetPndCacheStatusIPC: //ok
        //case ECustomNotifySsNetworkEventIPC: //ok //(shouldn't be possible
                                                    //to get any )
        //case ECustomCheckTwoDigitDialSupportIPC: //ok
        //case ECustomSendAPDUReqIPC: //ok
        //case ECustomGetLifeTimeIPC: //ok
        //case ECustomIsBlockedIPC: //ok
        //case ECustomCheckSecurityCodeIPC: //ok
        //case ECustomCheckEmergencyNumberIPC: //ok
        //case ECustomSatRefreshCompleteNotificationIPC: //ok
        //case ECustomDisablePhoneLockIPC: //ok
        //case ECustomGetActivePinIPC: //ok
        //case ECustomReleaseFileIPC: //ok
        //case ECustomRestartFileIPC: //ok
        //case ECustomNotifyEGprsInfoChange: //ok
        //case ECustomGetEGprsInfo: //ok
        //case EGet3GPBInfoIPC: //ok
        //case ECustomSetDriveModeIPC: //ok
        //case ECustomReadHSxPAStatusIPC: //ok
        //case ECustomWriteHSxPAStatusIPC: //ok
        //case ECustomNotifyHSxPAStatusIPC: //ok
        //case ECustomNotifyCellInfoChangeIPC: //ok
        //case ECustomGetCellInfoIPC: //ok

        //case ECustomGetBandSelectionIPC: //ok
        //case ECustomNotifyRemoteAlertingToneStatusChangeIPC: //ok

        case ECustomSetSystemNetworkModeIPC:
        case ECustomSetBandSelectionIPC:
            isRequestPossible = EFalse;
            break;
        default:
            break;
        }

    return isRequestPossible;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetCurrentSystemNetworkMode
// Completes Get Current System Network Mode
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetCurrentSystemNetworkMode(
    TUint32 aCurrentNetwork,
    TInt aResult )
    {
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetCurrentSystemNetworkMode );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // set the information for the client
        *iCurrentNetworkMode = aCurrentNetwork;

        // reset the varible
        iCurrentNetworkMode = NULL;

        // complete
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SetSimMessageStatusReadL
// Sets SIM SMS storage status to "read". Implements
// CustomAPI method SetSimMessageStatusRead.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SetSimMessageStatusReadL(
    const TTsyReqHandle aTsyReqHandle,
    TTime& aTime,
    TInt& aTimezoneDiff )
    {
    // NOTE: This method asynchronously calls DOS, and then immediately
    // completes with KErrNone. Any problems or errors are silently ignored.

    //call DOS
    CMmDataPackage dataPackage;
    dataPackage.PackData( &aTime, &aTimezoneDiff );
    iMmPhoneTsy->MessageManager()->HandleRequestL(
        ECustomSetSimMessageStatusReadIPC, &dataPackage );

    // immediately complete, ignore the result.
    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::ISVDialNumberCheck
// Saves call object for completion
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::SetObjectForISVDialNumberCheck(
    CMmVoiceCallTsy* aCallObject )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SETOBJECTFORISVDIALNUMBERCHECK_1, "TSY: CMmCustomTsy::SetObjectForISVDialNumberCheck aCallObject=0x%08x", aCallObject );
    //Save pointer for completion of check emergency number
    iISVDialNumberCheckObject = aCallObject;
    //ISV dial has been requested
    iISVDialNumberCheck = ETrue;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyRauEvent
// Sets RAU event notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyRauEvent(
    RMmCustomAPI::TRauEventStatus* aEventStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_NOTIFYRAUEVENT_1, "TSY: CMmCustomTsy::NotifyRauEvent");

    // save pointer to client data
    iRauEventStatus = aEventStatus;

    // save handle type
    iReqHandleType = ECustomTsyNotifyRauEvent;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyRauEvent
// Completes a NotifyRauEvent request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyRauEvent(
    RMmCustomAPI::TRauEventStatus aEventStatus,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYRAUEVENT_1, "TSY: CMmCustomTsy::CompleteNotifyRauEvent");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyRauEvent );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // set the information for the client
            *iRauEventStatus = aEventStatus;
            }

        // reset the internal variable
        iRauEventStatus = NULL;
        // complete
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyRauEventCancel
// This method cancels an outstanding notification of RAU event, placed with
// the NotifyRauEvent function.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyRauEventCancel()
    {
    // reset reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        ECustomTsyNotifyRauEvent );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // reset pointer to client data
        iRauEventStatus = NULL;

        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::ReadHSxPAStatusL
// Read high speed channel status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::ReadHSxPAStatusL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::THSxPAStatus* aHSxPAStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_READHSXPASTATUSL_1, "TSY: CMmCustomTsy::ReadHSxPAStatusL");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyReadHSxPAStatus );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save pointer to client data
        iRetHSxPAStatus = aHSxPAStatus;

        // call DOS (no packed parameters)
        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomReadHSxPAStatusIPC );

        // check the result of the call
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Store the request handle
            iReqHandleType = ECustomTsyReadHSxPAStatus;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteReadHSxPAStatus
// Completes a ReadHSxPAStatus request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteReadHSxPAStatus(
    CMmDataPackage* aDataPackage,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEREADHSXPASTATUS_1, "TSY: CMmCustomTsy::CompleteReadHSxPAStatus");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyReadHSxPAStatus );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            RMmCustomAPI::THSxPAStatus status;
            aDataPackage->UnPackData( status );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEREADHSXPASTATUS_2, "TSY: CMmCustomTsy::CompleteReadHSxPAStatus, status = %d", status);

            // Save the status.
            iHSxPAStatus = status;

            // set the information for the client
            *iRetHSxPAStatus = status;

            // reset the internal variable
            iRetHSxPAStatus = NULL;
            //Complete request to success
            ReqCompleted( reqHandle, KErrNone );
            }
        else
            {
            // Complete with error
            ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::ReadHSxPAStatusCancel
// Cancels a ReadHSxPAStatus request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::ReadHSxPAStatusCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_READHSXPASTATUSCANCEL_1, "TSY: CMmCustomTsy::ReadHSxPAStatusCancel");

    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyReadHSxPAStatus );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // reset the pointer to client data
        iRetHSxPAStatus = NULL;

        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::WriteHSxPAStatusL
// Write HSxPAStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::WriteHSxPAStatusL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::THSxPAStatus* aHSxPAStatus )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_WRITEHSXPASTATUSL_1, "TSY: CMmCustomTsy::WriteHSxPAStatusL. New status: %d", *aHSxPAStatus);

    iSetHSxPAStatus = aHSxPAStatus;

    // get the handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyWriteHSxPAStatus );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( iSetHSxPAStatus );

        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomWriteHSxPAStatusIPC, &dataPackage );

        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = ECustomTsyWriteHSxPAStatus;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteWriteHSxPAStatus
// Complete WriteHSxPAStatus request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteWriteHSxPAStatus(
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEWRITEHSXPASTATUS_1, "TSY: CMmCustomTsy::CompleteWriteHSxPAStatus");

    //reset req handle.
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyWriteHSxPAStatus );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            // Save the current status
            iHSxPAStatus = *iSetHSxPAStatus;
            }

        // erase pointer to client data
        iSetHSxPAStatus = NULL;

        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::WriteHSxPAStatusCancel
// Cancel WriteHSxPAStatus request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::WriteHSxPAStatusCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_WRITEHSXPASTATUSCANCEL_1, "TSY: CMmCustomTsy::WriteHSxPAStatusCancel");

    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyWriteHSxPAStatus );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // erase pointer to client data
        iSetHSxPAStatus = NULL;

        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyHSxPAStatus
// Set HSxPA notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyHSxPAStatus(
    RMmCustomAPI::THSxPAStatus* aHSxPAStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_NOTIFYHSXPASTATUS_1, "TSY: CMmCustomTsy::NotifyHSxPAStatus");

    // save pointer to client data
    iRetNotifyHSxPAStatus = aHSxPAStatus;

    // set the reqhandle type
    iReqHandleType = ECustomTsyNotifyHSxPAStatus;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyHSxPAStatus
// Complete HSxPA notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyHSxPAStatus(
    CMmDataPackage* aDataPackage,
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYHSXPASTATUS_1, "TSY: CMmCustomTsy::CompleteNotifyHSxPAStatus, Error: %{TSymbianErrorCodes}", aErrorCode);

    RMmCustomAPI::THSxPAStatus status;
    aDataPackage->UnPackData( status );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYHSXPASTATUS_2, "TSY: CMmCustomTsy::CompleteNotifyHSxPAStatus, old status: %d", iHSxPAStatus);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYHSXPASTATUS_3, "TSY: CMmCustomTsy::CompleteNotifyHSxPAStatus, new status: %d", status);

    // check if status was changed
    if ( iHSxPAStatus != status )
        {
        // Check if notification was requested
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            ECustomTsyNotifyHSxPAStatus );

        if ( ECustomTsyReqHandleUnknown != reqHandle )
            {
            // check if response was ok
            if ( KErrNone == aErrorCode )
                {
                // Save the status
                iHSxPAStatus = status;

                // set the information for the client
                *iRetNotifyHSxPAStatus = status;

                //Complete request to success
                ReqCompleted( reqHandle, KErrNone );
                }
            else
                {
                // Complete error value
                ReqCompleted( reqHandle, aErrorCode );
                }

            // reset the internal variable
            iRetNotifyHSxPAStatus = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyHSxPAStatusCancel
// Cancel HSxPA notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyHSxPAStatusCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_NOTIFYHSXPASTATUSCANCEL_1, "TSY: CMmCustomTsy::NotifyHSxPAStatusCancel");

    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyHSxPAStatus );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // reset the pointer to client data
        iRetNotifyHSxPAStatus = NULL;

        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetCallForwardingStatusL
// Gets  call forwarding status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetIccCallForwardingStatusL(
    TTsyReqHandle /*aTsyReqHandle*/,
    TDes8* aCFIndicators )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETICCCALLFORWARDINGSTATUSL_1, "TSY: CMmCustomTsy::GetIccCallForwardingStatusL");

    TInt ret( KErrGeneral );

    //Get reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyGetIccCallForwardingStatus );
    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        ret = KErrServerBusy;
        }
    else
        {
        if(  sizeof(RMmCustomAPI::TCFIndicators) > aCFIndicators->Size())
            {
            ret = KErrArgument;    
            }
        else
            {
            //Store client side pointer
            iRetCFIndicators =  aCFIndicators;
            
            ret = Phone()->MessageManager()->HandleRequestL(
                ECustomGetIccCallForwardingStatusIPC );
            
            if ( KErrNone == ret )
                {
                // Save the req handle type
                iReqHandleType = ECustomTsyGetIccCallForwardingStatus;
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetCallForwardingStatus
// completes gets  call forwarding status request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetIccCallForwardingStatus(
    CMmDataPackage* aDataPackage,
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETICCCALLFORWARDINGSTATUS_1, "TSY: CMmCustomTsy::CompleteGetIccCallForwardingStatus, aErrorCode=%{TSymbianErrorCodes}", aErrorCode );

    //reset req handle.
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetIccCallForwardingStatus );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if( KErrNone == aErrorCode )
            {
            RMmCustomAPI::TCFIndicatorsPckg* tcfIndicatorPckg = REINTERPRET_CAST(
                    	RMmCustomAPI::TCFIndicatorsPckg*, iRetCFIndicators );
            RMmCustomAPI::TCFIndicators& tcfIndicator = ( *tcfIndicatorPckg )();

            aDataPackage->UnPackData( iCurrentTCFIndicator );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETICCCALLFORWARDINGSTATUS_2, "TSY: CMmCustomTsy::CompleteGetIccCallForwardingStatus, indicator=%u", iCurrentTCFIndicator.iIndicator );

            tcfIndicator.iIndicator = iCurrentTCFIndicator.iIndicator;
                tcfIndicator.iCFNumber.iTypeOfNumber =
            iCurrentTCFIndicator.iCFNumber.iTypeOfNumber;
            tcfIndicator.iCFNumber.iNumberPlan =
                iCurrentTCFIndicator.iCFNumber.iNumberPlan;
            tcfIndicator.iCFNumber.iTelNumber.Copy(
               iCurrentTCFIndicator.iCFNumber.iTelNumber );

	        // Update CenRep with latest valid data
	        if( &iCurrentTCFIndicator )
	            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETICCCALLFORWARDINGSTATUS_3, "TSY: CMmCustomTsy::CompleteGetIccCallForwardingStatus : Update Central Repository");
	            TPckg<TUint32> tempPckg( iCurrentTCFIndicator.iIndicator );
	            iCFISCentRep->Set( KCtsyCallForwardingIndicator, tempPckg );
	
	            iCFISPrivateCentRep->Set( KCtsyCallForwardingNumberPlan,
	                iCurrentTCFIndicator.iCFNumber.iNumberPlan );
	            iCFISCentRep->Set( KCtsyCallForwardingNumber,
	                iCurrentTCFIndicator.iCFNumber.iTelNumber );
	            iCFISPrivateCentRep->Set( KCtsyCallForwardingMspId,
	                iCurrentTCFIndicator.iMultipleSubscriberProfileID );
	            }
            }
        //Erase client side pointer
        iRetCFIndicators = NULL;
        //Complete
        ReqCompleted( reqHandle, aErrorCode );
        }

    //Request is made during boot-up done only once
    else if( iGetIccCfStatusBootUp )
        {
        if( KErrNone == aErrorCode )
            {
            iGetIccCfStatusBootUp = EFalse;
            aDataPackage->UnPackData( iCurrentTCFIndicator );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETICCCALLFORWARDINGSTATUS_4, "TSY: CMmCustomTsy::CompleteGetIccCallForwardingStatus CenRep key UnconditionalCFStatus not RESETED.");

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETICCCALLFORWARDINGSTATUS_5, "TSY: CMmCustomTsy::CompleteGetIccCallForwardingStatus : Bootup case, KCtsyCallForwardingIndicator indicator=%u", iCurrentTCFIndicator.iIndicator );

            TPckg<TUint32> tempPckg( iCurrentTCFIndicator.iIndicator );
            iCFISCentRep->Set( KCtsyCallForwardingIndicator, tempPckg );
            iCFISPrivateCentRep->Set( KCtsyCallForwardingNumberPlan,
            iCurrentTCFIndicator.iCFNumber.iNumberPlan );
            iCFISCentRep->Set( KCtsyCallForwardingNumber,
            iCurrentTCFIndicator.iCFNumber.iTelNumber );
            iCFISPrivateCentRep->Set( KCtsyCallForwardingMspId,
            iCurrentTCFIndicator.iMultipleSubscriberProfileID );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETICCCALLFORWARDINGSTATUS_6, "TSY: CMmCustomTsy::CompleteGetIccCallForwardingStatus CenRep updated.");
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetIccCallForwardingStatusCancel
// Cancels Get icc call forwarding status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetIccCallForwardingStatusCancel(
	const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETICCCALLFORWARDINGSTATUSCANCEL_1, "CMmCustomTsy::GetIccCallForwardingStatusCancel");
	// reset reqhandle
	iTsyReqHandleStore->ResetTsyReqHandle(
			ECustomTsyGetIccCallForwardingStatus );

	// complete with KErrCancel
	ReqCompleted( aTsyReqHandle, KErrCancel );

	return KErrNone;
    }
	 
// ---------------------------------------------------------------------------
// CMmCustomTsy::GetIccCallForwardingStatusCancel
// Cancels an ongoing GetIccCallForwardingStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetIccCallForwardingStatusCancel()
    {
    //Reset req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        ECustomTsyGetIccCallForwardingStatus );
        
    //Erase client side pointer
    iRetCFIndicators = NULL;
    
    if ( ECustomTsyReqHandleUnknown != reqHandle )
    	{
    	//Complete request with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}
    
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyCallForwardingStatusChange
// Set notification on
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyIccCallForwardingStatusChange(
    TDes8* aCFIndicators )
    {

    iNotifyCFIndicator = aCFIndicators;
    // save handle type
    iReqHandleType = ECustomTsyNotifyIccCallForwardingStatusChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyCallForwardingStatusChange
// Notifies client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyIccCallForwardingStatusChange(
    CMmDataPackage* aDataPackage,
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYICCCALLFORWARDINGSTATUSCHANGE_1, "TSY: CMmCustomTsy::CompleteNotifyIccCallForwardingStatusChange, Error: %{TSymbianErrorCodes}", aErrorCode);

    RMmCustomAPI::TCFIndicators tempNotifyCFIndicator;
    aDataPackage->UnPackData( tempNotifyCFIndicator );

    TBool isChanged = IsIccCallForwardingStatusChanged(
        tempNotifyCFIndicator );

    // check if information was changed
    if ( isChanged )
        {
        //Update CTSY internal buffer
        iCurrentTCFIndicator = tempNotifyCFIndicator;

        if ( KErrNone == aErrorCode )
            {  
			//Update CenRep

            TPckg<TUint32> tempPckg( iCurrentTCFIndicator.iIndicator );
			iCFISCentRep->Set( KCtsyCallForwardingIndicator, tempPckg );	
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYICCCALLFORWARDINGSTATUSCHANGE_2, "TSY: CMmCustomTsy::CompleteNotifyIccCallForwardingStatusChange : Update CenRep, KCtsyCallForwardingIndicator indicator=%u", iCurrentTCFIndicator.iIndicator );
				
        	iCFISPrivateCentRep->Set( KCtsyCallForwardingNumberPlan, 
        	    iCurrentTCFIndicator.iCFNumber.iNumberPlan );
			
        	iCFISCentRep->Set( KCtsyCallForwardingNumber,
        	    iCurrentTCFIndicator.iCFNumber.iTelNumber );        
				        		    
        	iCFISPrivateCentRep->Set( KCtsyCallForwardingMspId, 
        	    iCurrentTCFIndicator.iMultipleSubscriberProfileID );
            //CenRep updated
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYICCCALLFORWARDINGSTATUSCHANGE_3, "TSY: CMmCustomTsy::CompleteNotifyIccCallForwardingStatusChange CenRep updated.");
            }

        // Check if notification was requested
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            ECustomTsyNotifyIccCallForwardingStatusChange );

        if ( ECustomTsyReqHandleUnknown != reqHandle )
            {
            // check if response was ok
            if ( KErrNone == aErrorCode )
                {
                RMmCustomAPI::TCFIndicatorsPckg* tcfIndicatorPckg =
                REINTERPRET_CAST(
                RMmCustomAPI::TCFIndicatorsPckg*, iNotifyCFIndicator );
                RMmCustomAPI::TCFIndicators& tcfIndicator =
                ( *tcfIndicatorPckg )();

                if( sizeof(RMmCustomAPI::TCFIndicators) > tcfIndicatorPckg->Size())
                    {
                    ReqCompleted( reqHandle, KErrArgument);
                    }
                else
                    {
                    // Save the new info
                    tcfIndicator = tempNotifyCFIndicator;
                    //Complete request 
                    ReqCompleted( reqHandle, KErrNone );
                    }
                }
            else
                {
                // Complete with error value
                ReqCompleted( reqHandle, aErrorCode );
                }

            // reset the internal variable
            //iNotifyCFIndicator = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyCallForwardingStatusChangeCancel
// Cancels notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyIccCallForwardingStatusChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // reset pointer to client data
    iNotifyCFIndicator = NULL;

    // reset reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyIccCallForwardingStatusChange );

    // complete with KErrCancel
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::IsCallForwardingStatusChanged
// Decides if CTSY internal indicator buffer differs from received buffer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmCustomTsy::IsIccCallForwardingStatusChanged(
    RMmCustomAPI::TCFIndicators& aNotifyCFIndicator )
    {
    TBool changed( EFalse );

    if( ( aNotifyCFIndicator.iIndicator !=
        iCurrentTCFIndicator.iIndicator ) ||
        ( aNotifyCFIndicator.iCFNumber.iTypeOfNumber !=
        iCurrentTCFIndicator.iCFNumber.iTypeOfNumber ) ||
        ( aNotifyCFIndicator.iCFNumber.iNumberPlan !=
        iCurrentTCFIndicator.iCFNumber.iNumberPlan ) ||
        KErrNotFound == ( aNotifyCFIndicator.iCFNumber.iTelNumber.Match(
        iCurrentTCFIndicator.iCFNumber.iTelNumber ) ) )
        {
        changed = ETrue;
        }

    return changed;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetCellInfoL
// Gets GSM/WCDMA cell(s) info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetCellInfoL(
    TDes8* aCellInfoPckg )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETCELLINFOL_1, "TSY: CMmCustomTsy::GetCellInfo");

    TInt ret = KErrServerBusy;

    // Try to get request handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyGetCellInfo );

    if ( ECustomTsyReqHandleUnknown == reqHandle )
        {
        ret = KErrArgument;
        
        if ( sizeof( RMmCustomAPI::TMmCellInfo ) <= 
             aCellInfoPckg->MaxLength() )
            {
            iCellInfoPckg = REINTERPRET_CAST(
                RMmCustomAPI::TMmCellInfoPckg*, aCellInfoPckg );

            ret = Phone()->MessageManager()->HandleRequestL(
                ECustomGetCellInfoIPC  );	
        
    	    if ( KErrNone == ret )
    	        {
    	        // Save request handle in DoExtFuncL()
    	        iReqHandleType = ECustomTsyGetCellInfo;
    	        }
    	    }
		}

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetCellInfoCancel
// Cancels an outstanding GetCellInfo request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetCellInfoCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETCELLINFOCANCEL_1, "TSY: CMmCustomTsy::GetCellInfoCancel");

    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetCellInfo );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
    	// reset pointer to client data
        iCellInfoPckg = NULL;

        // complete with cancel
        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetCellInfo
// Completes an outstanding GetCellInfo request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetCellInfo(
    RMmCustomAPI::TMmCellInfo* aCellInfo,
    TInt aErrorValue )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETCELLINFO_1, "TSY: CMmCustomTsy::CompleteGetCellInfo - aErrorValue %{TSymbianErrorCodes}", aErrorValue);

    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetCellInfo );

    // check the validity of the handler
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorValue )
            {
            RMmCustomAPI::TMmCellInfo& tempCellInfo = ( *iCellInfoPckg )();
            tempCellInfo = *aCellInfo;
            }
        // reset the pointer to client space
        iCellInfoPckg = NULL;

        // complete operation
        ReqCompleted ( reqHandle, aErrorValue );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyCellInfoChange
// Notification cell(s) info changes
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyCellInfoChange(
    TDes8* aCellInfoPckg )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_NOTIFYCELLINFOCHANGE_1, "TSY: CMmCustomTsy::NotifyCellInfoChange");
   
    RMmCustomAPI::TMmCellInfo temp;
	RMmCustomAPI::TMmCellInfoPckg tempPckg(temp);
	
	TInt paramLength = aCellInfoPckg->MaxLength();
	TInt expectedLength = tempPckg.MaxLength();

    TInt ret = KErrArgument;
	if (paramLength == expectedLength)
		{
	    iRetNotifyCellInfoPckg = REINTERPRET_CAST( RMmCustomAPI::TMmCellInfoPckg*, aCellInfoPckg );
	    // set the reqhandle type
	    iReqHandleType = ECustomTsyNotifyCellInfoChange;
	    
	    ret = KErrNone;
		}
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyCellInfoChange
// Complete cell info change notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyCellInfoChange(
    RMmCustomAPI::TMmCellInfo* aCellInfo,
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYCELLINFOCHANGE_1, "TSY: CMmCustomTsy::CompleteNotifyCellInfoChange Error: %{TSymbianErrorCodes}", aErrorCode);

    // Check if notification was requested
	TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyCellInfoChange );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // set the information for the client
            RMmCustomAPI::TMmCellInfo& tempCellInfo = ( *iRetNotifyCellInfoPckg )();
            tempCellInfo = *aCellInfo;
            }
        // reset the internal variable
        iRetNotifyCellInfoPckg = NULL;

        // complete
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyCellInfoChangeCancel
// Cancel cell info change notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyCellInfoChangeCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_NOTIFYCELLINFOCHANGECANCEL_1, "TSY: CMmCustomTsy::NotifyCellInfoChangeCancel");

    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyCellInfoChange );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // reset the pointer to client data
        iRetNotifyCellInfoPckg = NULL;

        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetPhonebookCacheStatus
// Get Phonebook cache status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RMmCustomAPI::TPndCacheStatus CMmCustomTsy::GetPhonebookCacheStatus( 
    TUint8 aPhoneBookType )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETPHONEBOOKCACHESTATUS_1, "TSY: CMmCustomTsy::GetPhonebookCacheStatus");
    RMmCustomAPI::TPndCacheStatus ret( RMmCustomAPI::ECacheNotUsed );

    switch ( aPhoneBookType )
        {
        case KADNPhoneBook:
            ret = iAdnCacheStatus;
            break;
        case KFDNPhoneBook:
            ret = iFdnCacheStatus;
            break;
        case KSDNPhoneBook:
        case KVMBXPhoneBook:
        case KMBDNPhoneBook:
            ret = RMmCustomAPI::ECacheNotUsed;
            break;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::ReqCompleted
// Completes a request back to the client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::ReqCompleted( const TTsyReqHandle aTsyReqHandle,
    const TInt aError )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_REQCOMPLETED_1, "TSY: CMmCustomTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", (TUint)aTsyReqHandle, aError, (TUint)this);

	CTelObject::ReqCompleted ( aTsyReqHandle, aError );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_REQCOMPLETED_2, "<-- TSY: CMmCustomTsy::ReqCompleted");
	}

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetSystemNetworkBandL
// This method gets the supported network band.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetSystemNetworkBandL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TBandSelection* aSystemNetworkBand,
    RMmCustomAPI::TNetworkModeCaps* aSystemNetworkMode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETSYSTEMNETWORKBANDL_1, "TSY: CMmCustomTsy::GetSystemNetworkBandL");
    TTsyReqHandle reqhandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyGetSystemNetworkBand );

    if ( ECustomTsyReqHandleUnknown < reqhandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save pointer to the packaged life timer info
        iAvailableSystemNetworkBand = aSystemNetworkBand;
        iSystemNetworkModeCaps = aSystemNetworkMode;
        // call DOS
        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomGetBandSelectionIPC );

        // Check response of the extension
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = ECustomTsyGetSystemNetworkBand;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetSystemNetworkBand
// Completes a GetSystemNetworkBand.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetSystemNetworkBand(
    RMmCustomAPI::TBandSelection aSystemNetworkBand,
    RMmCustomAPI::TNetworkModeCaps aSystemNetworkMode,
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETSYSTEMNETWORKBAND_1, "TSY: CMmCustomTsy::CompleteGetSystemNetworkBand. Result: %d", aResult);
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetSystemNetworkBand );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        
        if ( KErrNone == aResult )
            {
            // set the information for the client
            *iAvailableSystemNetworkBand = aSystemNetworkBand;
            if ( *iSystemNetworkModeCaps != aSystemNetworkMode )
                {
                *iSystemNetworkModeCaps = aSystemNetworkMode;
                }
            }
        
        // complete
        ReqCompleted( reqHandle, aResult );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmCustomTsy::GetSystemNetworkBandCancel
// Cancels a GetSystemNetworkBand.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetSystemNetworkBandCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETSYSTEMNETWORKBANDCANCEL_1, "TSY: CMmCustomTsy::GetSystemNetworkBandCancel");
    // reset the pointer to client data
    iAvailableSystemNetworkBand = NULL;
    iSystemNetworkModeCaps = NULL;

    // reset the reqhandle
    iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetSystemNetworkBand );

    // complete
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SetSystemNetworkBandL
// This method sets the system network band.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::SetSystemNetworkBandL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TBandSelection* aSystemNetworkBand,
    RMmCustomAPI::TNetworkModeCaps* aSystemNetworkModeCaps )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SETSYSTEMNETWORKBANDL_1, "TSY: CMmCustomTsy::SetSystemNetworkBandL");
    TTsyReqHandle reqhandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsySetSystemNetworkBand );

    if ( ECustomTsyReqHandleUnknown < reqhandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // call DOS
        // The networkband to be set
        RMmCustomAPI::TBandSelection networkBand =
            *aSystemNetworkBand;
        // Used network mode
        RMmCustomAPI::TNetworkModeCaps networkMode =
            *aSystemNetworkModeCaps;
        CMmDataPackage dataPackage;
        // Pack network mode and band
        dataPackage.PackData( &networkBand, &networkMode );
        TInt ret = Phone()->MessageManager()->HandleRequestL(
            ECustomSetBandSelectionIPC, &dataPackage );

        // Check response of the extension
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = ECustomTsySetSystemNetworkBand;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteSetSystemNetworkBand
// Completes a SetSystemNetworkBand.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteSetSystemNetworkBand(
    TInt aResult )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETESETSYSTEMNETWORKBAND_1, "TSY: CMmCustomTsy::CompleteSetSystemNetworkBand");
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsySetSystemNetworkBand );

    // check if handle exists
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // complete
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::SetIccCfBootUpFlag
// Set new value for ICC CF bootup.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::SetIccCfBootUpFlag( TBool aBootUp )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_SETICCCFBOOTUPFLAG_1, "TSY: CMmCustomTsy::SetIccCfBootUpFlag, aBootUp=%d", aBootUp );

    iGetIccCfStatusBootUp = aBootUp;
    }
    
// ---------------------------------------------------------------------------
// CMmCustomTsy::GetUSIMServiceSupportL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetUSIMServiceSupportL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TAppSupport* aAppSupport )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETUSIMSERVICESUPPORTL_1, "TSY: CMmCustomTsy::GetUSIMServiceSupportL");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        ECustomTsyGetUSIMServiceSupport );

    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save pointer to client data
        iAppSupport = aAppSupport;

	    CMmDataPackage appSupportPackage;
	    appSupportPackage.PackData( iAppSupport );

        // call DOS (no packed parameters)
        TInt ret = KErrNone;
        ret = Phone()->MessageManager()->HandleRequestL(
            ECustomGetServiceTableSupportbyApplicationIPC, &appSupportPackage);

        // check the result of the call
        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Store the request handle
            iReqHandleType = ECustomTsyGetUSIMServiceSupport;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteGetUSIMServiceSupport
// Completes a GetUSIMServiceSupport request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteGetUSIMServiceSupport(
    CMmDataPackage* aDataPackage,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETEGETUSIMSERVICESUPPORT_1, "TSY: CMmCustomTsy::CompleteGetUSIMServiceSupport");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetUSIMServiceSupport );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            RMmCustomAPI::TAppSupport *appSupport;
            aDataPackage->UnPackData( &appSupport );

            // set the information for the client
            *iAppSupport = *appSupport;

            // reset the internal variable
			iAppSupport = NULL;
			
            //Complete request to success
            ReqCompleted( reqHandle, KErrNone );
            }
        else
            {
            // Complete with error
            ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::GetUSIMServiceSupportCancel
// Cancels a GetUSIMServiceSupport request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::GetUSIMServiceSupportCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_GETUSIMSERVICESUPPORTCANCEL_1, "TSY: CMmCustomTsy::GetUSIMServiceSupportCancel");

    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyGetUSIMServiceSupport );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // reset the pointer to client data
		iAppSupport = NULL;

        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyRemoteAlertingToneStatusChange
// Sets Remote Alerting Tone Status Change notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyRemoteAlertingToneStatusChange(
    RMmCustomAPI::TRemoteAlertingToneStatus* aToneStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_NOTIFYREMOTEALERTINGTONESTATUSCHANGE_1, "TSY: CMmCustomTsy::NotifyRemoteAlertingToneStatusChange");

    // save pointer to client data
    iToneStatus = aToneStatus;

    // save handle type
    iReqHandleType = ECustomTsyNotifyRemoteAlertingToneStatusChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::CompleteNotifyRemoteAlertingToneStatusChange
// Completes a NotifyRemoteToneAlertingStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCustomTsy::CompleteNotifyRemoteAlertingToneStatusChange(
    RMmCustomAPI::TRemoteAlertingToneStatus aToneStatus,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMTSY_COMPLETENOTIFYREMOTEALERTINGTONESTATUSCHANGE_1, "TSY: CMmCustomTsy::CompleteNotifyRemoteAlertingToneStatusChange");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ECustomTsyNotifyRemoteAlertingToneStatusChange );

    // check if request was called
    if ( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // set the information for the client
            *iToneStatus = aToneStatus;
            }

        // reset the internal variable
        iToneStatus = NULL;
        // complete
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmCustomTsy::NotifyRemoteAlertingToneStatusChangeCancel
// This method cancels an outstanding notification of 
// RemoteAlertingToneStatusChange.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCustomTsy::NotifyRemoteAlertingToneStatusChangeCancel()
    {
    // reset reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        ECustomTsyNotifyRemoteAlertingToneStatusChange );

    if( ECustomTsyReqHandleUnknown != reqHandle )
        {
        // reset pointer to client data
        iToneStatus = NULL;

        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

TBool CMmCustomTsy::ResetReqHandle( const TTsyReqHandle aTsyReqHandle, TInt aIpc )
    {
    TBool ret = CMmTsyBase::ResetReqHandle(aTsyReqHandle, aIpc);
    if(!ret)
        {
        ret = iTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
        }
    return ret;
    }
    

//  End of File
