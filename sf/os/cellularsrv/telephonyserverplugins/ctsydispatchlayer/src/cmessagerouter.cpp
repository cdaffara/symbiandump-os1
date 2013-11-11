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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmessagerouterTraces.h"
#endif

#include <e32def.h>
#include <satcs.h>
#include <pcktcs.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/rmmcustomapi.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>

#include "cmessagerouter.h"
#include <ctsy/ltsy/sat_defs.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/ltsy/ltsylogger.h>
#include "tdispatcherholder.h"
#include "requestqueueoneshot.h"
#include "ctsydispatcherpanic.h"

#include "ccallcontroldispatcher.h"
#include "cphonedispatcher.h"
#include "csecuritydispatcher.h"
#include "cphonebookendispatcher.h"
#include "ccellbroadcastdispatcher.h"
#include "cphonebookondispatcher.h"
#include "cphonebookdispatcher.h"
#include "csatdispatcher.h"
#include "csimdispatcher.h"
#include "csmsdispatcher.h"
#include "ccallcontrolmultipartydispatcher.h"
#include "csupplementaryservicesdispatcher.h"
#include "cpacketservicesdispatcher.h"



TMessageManagerCallbackProxy::TMessageManagerCallbackProxy(MmMessageManagerCallback& aCtsyMessageManagerCallback)
: iCtsyMessageManagerCallback(&aCtsyMessageManagerCallback)
	{
	} // TMessageManagerCallbackProxy::TMessageManagerCallbackProxy

TMessageManagerCallbackProxy::TMessageManagerCallbackProxy()
: iCtsyMessageManagerCallback(NULL)
	{
	}

void TMessageManagerCallbackProxy::SetMessageManagerCallbackProxy(MmMessageManagerCallback& aCtsyMessageManagerCallback)
	{
	iCtsyMessageManagerCallback = &aCtsyMessageManagerCallback;
	}

void TMessageManagerCallbackProxy::Complete( TInt aIpc, TInt aResult )
/**
 * Completes requests / indications back to the Common TSY.
 *
 * @param aIpc The IPC to complete.
 * @param aResult The result code of the request / indication.
 */
	{
	TSYLOGENTRYEXITARGS(_L8(" [IPC=%d, result=%d]"), aIpc, aResult);
	ASSERT(iCtsyMessageManagerCallback);
	iCtsyMessageManagerCallback->Complete(aIpc, aResult);
	} // TMessageManagerCallbackProxy::Complete


void TMessageManagerCallbackProxy::Complete( TInt aIpc, CMmDataPackage* aData, TInt aResult )
/**
 * Completes requests / indications back to the Common TSY.
 *
 * @param aIpc The IPC to complete.
 * @param aData Package encapulating the data relating to the request / indication.
 * @param aResult The result code of the request / indication.
 */
	{
	TSYLOGENTRYEXITARGS(_L8(" [IPC=%d, result=%d, aData=0x%x]"), aIpc, aResult, aData);
	ASSERT(iCtsyMessageManagerCallback);
	iCtsyMessageManagerCallback->Complete(aIpc, aData, aResult);
	} // TMessageManagerCallbackProxy::Complete


CMessageRouter* CMessageRouter::NewL(MmMessageManagerCallback& aMessageManagerCallback)
    {
    TSYLOGENTRYEXIT;

    CMessageRouter* const self = new(ELeave) CMessageRouter(aMessageManagerCallback);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CMessageRouter::CMessageRouter(MmMessageManagerCallback& aMessageManagerCallback)
	: 	iMessageManagerCallbackProxy(aMessageManagerCallback),
		iSatMessageManagerCallbackProxy()
    {
    }


void CMessageRouter::ConstructL()
/**
 * Second phase constructor.
 */
    {
    TSYLOGENTRYEXIT;

    iRequestQueueOneShot = CRequestQueueOneShot::NewL();

    // Create callback object
    iCtsyDispatcherCallback = CCtsyDispatcherCallback::NewL();
    
    
    // The following code loads the LicenseeSkeletonLTSY.dll
    // This LTSY is for testing purposes only. In a real
    // environment, the licensee is expected to provide their own 
    // LTSY, which should adhere to the  MLtsyDispatchFactoryV1 interface
    
	#define TEST_CODE
	    
    _LIT(KLicenseeLTSYDllName, "licenseeskeletontsy.dll");    
    
    User::LeaveIfError(iLoadedLib.Load(KLicenseeLTSYDllName));
    typedef MLtsyFactoryBase* (*LTsyFactoryL)();
    LTsyFactoryL libEntry = (LTsyFactoryL)iLoadedLib.Lookup(1);
    MLtsyFactoryBase* ltsyfactory = (MLtsyFactoryBase*)(*libEntry)();	// libEntry may leave.
    iLtsyFactoryV1 = static_cast<MLtsyDispatchFactoryV1*>(ltsyfactory);
    
    iLtsyFactoryV1->ConstructL(*iCtsyDispatcherCallback);

    // Create the dispatchers

	iCtsyDispatcherCallControlDispatcher = CCallControlDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherPhoneDispatcher = CPhoneDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, iSatMessageManagerCallbackProxy,
			*iRequestQueueOneShot, *this);


	iCtsyDispatcherSecurityDispatcher = CSecurityDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherPhonebookEnDispatcher = CPhonebookEnDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherCellBroadcastDispatcher = CCellBroadcastDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherPhonebookOnDispatcher = CPhonebookOnDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherPhonebookDispatcher = CPhonebookDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherSimDispatcher = CSimDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherSmsDispatcher = CSmsDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherCallControlMultipartyDispatcher = CCallControlMultipartyDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherSupplementaryServicesDispatcher = CSupplementaryServicesDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherPacketServicesDispatcher = CPacketServicesDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, *iRequestQueueOneShot);


	iCtsyDispatcherSatDispatcher = CSatDispatcher::NewL(
			*iLtsyFactoryV1, iMessageManagerCallbackProxy, iSatMessageManagerCallbackProxy, *iRequestQueueOneShot);



	iDispatcherHolder = new (ELeave) TDispatcherHolder(
			*iCtsyDispatcherCallControlDispatcher,
			*iCtsyDispatcherPhoneDispatcher,
			*iCtsyDispatcherSecurityDispatcher,
			*iCtsyDispatcherPhonebookEnDispatcher,
			*iCtsyDispatcherCellBroadcastDispatcher,
			*iCtsyDispatcherPhonebookOnDispatcher,
			*iCtsyDispatcherPhonebookDispatcher,
			*iCtsyDispatcherSimDispatcher,
			*iCtsyDispatcherSmsDispatcher,
			*iCtsyDispatcherCallControlMultipartyDispatcher,
			*iCtsyDispatcherSupplementaryServicesDispatcher,
			*iCtsyDispatcherPacketServicesDispatcher,
			*iCtsyDispatcherSatDispatcher);

    iCtsyDispatcherCallback->SetDispatcherHolder(*iDispatcherHolder);

	iCtsyDispatcherCallControlDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherPhoneDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherSecurityDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherPhonebookEnDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherCellBroadcastDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherPhonebookOnDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherPhonebookDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherSimDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherSmsDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherCallControlMultipartyDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherSupplementaryServicesDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherPacketServicesDispatcher->SetDispatcherHolder(*iDispatcherHolder);
	iCtsyDispatcherSatDispatcher->SetDispatcherHolder(*iDispatcherHolder);

    StartBootSequenceL();
    } // CMessageRouter::ConstructL

void CMessageRouter::SetSatMessageManager(MmMessageManagerCallback& aMessageManagerCallback)
	{
	TSYLOGENTRYEXIT;
	iSatMessageManagerCallbackProxy.SetMessageManagerCallbackProxy(aMessageManagerCallback);
	}

void CMessageRouter::SetSatCallOriginInterface(MStkTsyCallOrigin& aSatCallOrigin)
	{
	TSYLOGENTRYEXIT;
	//this pointer is initialised in ConstructL
	iCtsyDispatcherSatDispatcher->SetSatCallOriginInterface(aSatCallOrigin);
	}

CMessageRouter::~CMessageRouter()
    {
    TSYLOGENTRYEXIT;

    if (iLtsyFactoryV1)
    	{
    	iLtsyFactoryV1->Release();
    	iLtsyFactoryV1 = NULL;
    	}

    iLoadedLib.Close();
    
    // Delete dispatchers

	delete iCtsyDispatcherCallControlDispatcher;
    iCtsyDispatcherCallControlDispatcher = NULL;

	delete iCtsyDispatcherPhoneDispatcher;
    iCtsyDispatcherPhoneDispatcher = NULL;

	delete iCtsyDispatcherSecurityDispatcher;
    iCtsyDispatcherSecurityDispatcher = NULL;

	delete iCtsyDispatcherPhonebookEnDispatcher;
    iCtsyDispatcherPhonebookEnDispatcher = NULL;

	delete iCtsyDispatcherCellBroadcastDispatcher;
    iCtsyDispatcherCellBroadcastDispatcher = NULL;

	delete iCtsyDispatcherPhonebookOnDispatcher;
    iCtsyDispatcherPhonebookOnDispatcher = NULL;

	delete iCtsyDispatcherPhonebookDispatcher;
    iCtsyDispatcherPhonebookDispatcher = NULL;

	delete iCtsyDispatcherSimDispatcher;
    iCtsyDispatcherSimDispatcher = NULL;

	delete iCtsyDispatcherSmsDispatcher;
    iCtsyDispatcherSmsDispatcher = NULL;

	delete iCtsyDispatcherCallControlMultipartyDispatcher;
    iCtsyDispatcherCallControlMultipartyDispatcher = NULL;

	delete iCtsyDispatcherSupplementaryServicesDispatcher;
    iCtsyDispatcherSupplementaryServicesDispatcher = NULL;

	delete iCtsyDispatcherPacketServicesDispatcher;
    iCtsyDispatcherPacketServicesDispatcher = NULL;

	delete iCtsyDispatcherSatDispatcher;
    iCtsyDispatcherSatDispatcher = NULL;

    delete iCtsyDispatcherCallback;
    iCtsyDispatcherCallback = NULL;

    delete iDispatcherHolder;
    iDispatcherHolder = NULL;

    delete iRequestQueueOneShot;
    iRequestQueueOneShot = NULL;

    } // CMessageRouter::~CMessageRouter


void CMessageRouter::StartBootSequenceL()
/**
 * Second initialisation phase.
 */
    {
    TSYLOGENTRYEXIT;

    // Kick off boot sequence
    TInt ret = ExtFuncL(EMmTsyBootNotifyModemStatusReadyIPC, NULL);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_STARTBOOTSEQUENCEL_1, "ExtFuncL(EMmTsyBootNotifyModemStatusReadyIPC returned %d", ret);
    User::LeaveIfError(ret);
    } // CMessageRouter::StartBootSequenceL


TInt CMessageRouter::ExtFuncL(TInt aIpc, const CMmDataPackage* aDataPackage)
/**
 * Part of the MMessageRouter interface which is part of the LTSY plugin
 * interface.
 *
 * This function is the entry point for a request from the CTSY into the
 * LTSY.
 *
 * @param aIpc The IPC of the request.
 *
 * @param aDataPackage The data package containing the parameters of the
 * request.
 *
 * @return Error code to propagate back up to the CTSY relating to making
 * this request. Returns KErrNone on success, KErrNotSupported if the LTSY
 * does not support this request (it could not find a message handler to
 * handle this request).
 */
	{
	TSYLOGENTRYEXITARGS(_L8(" [IPC=%d]"), aIpc);

	TInt ret = KErrNotSupported;

	// Dispatch to Licensee LTSY
	switch (aIpc)
		{

		// CallControl Functional Unit IPCs

	case EEtelCallAnswer:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchAnswerL(aDataPackage);
		break;
	case EMobileCallAnswerISV:
		ret = iCtsyDispatcherCallControlDispatcher->DispatchAnswerIsvL(aDataPackage);
		break;
	case EMobileCallHold:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchHoldL(aDataPackage);
		break;
	case EMobileCallDialEmergencyCall:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchDialEmergencyL(aDataPackage);
		break;
	case EMobilePhoneStopDTMFTone:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchStopDtmfToneL(aDataPackage);
		break;
	case EMobileCallDialISV:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchDialIsvL(aDataPackage);
		break;
	case EMobilePhoneSetALSLine:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchSetActiveAlsLineL(aDataPackage);
		break;
	case EMobilePhoneSendDTMFTonesCancel:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchSendDtmfTonesCancelL(aDataPackage);
		break;
	case EEtelCallHangUp:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchHangUpL(aDataPackage);
		break;
	case EMobileCallResume:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchResumeL(aDataPackage);
		break;
	case EMobileCallSetDynamicHscsdParams:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchSetDynamicHscsdParamsL(aDataPackage);
		break;
	case EEtelCallDial:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchDialL(aDataPackage);
		break;
	case EMobileCallDialNoFdnCheck:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchDialNoFdnCheckL(aDataPackage);
		break;
	case EMobileCallTransfer:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchTransferL(aDataPackage);
		break;
	case EMobilePhoneSendDTMFTones:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchSendDtmfTonesL(aDataPackage);
		break;
	case EMobilePhoneGetIdentityServiceStatus:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchGetIdentityServiceStatusL(aDataPackage);
		break;
	case EMobileCallSwap:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchSwapL(aDataPackage);
		break;
	case EMobilePhoneContinueDTMFStringSending:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchContinueDtmfStringSendingL(aDataPackage);
		break;
	case EEtelCallLoanDataPort:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchLoanDataPortL(aDataPackage);
		break;
	case EEtelCallRecoverDataPort:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchRecoverDataPortL(aDataPackage);
		break;
	case EMobilePhoneStartDTMFTone:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchStartDtmfToneL(aDataPackage);
		break;
	case EMobilePhoneGetALSLine:
		ret = iCtsyDispatcherCallControlDispatcher->DispatchGetActiveAlsLineL();
		break;
	case ECustomCheckEmergencyNumberIPC:
		ret = iCtsyDispatcherCallControlDispatcher->DispatchQueryIsEmergencyNumberL(aDataPackage);
		break;
	case ECustomCheckAlsPpSupportIPC:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchGetAlsPpSupportL();
		break;
	case ECustomGetAlsBlockedIPC:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchGetAlsBlockedStatusL();
		break;
	case ECustomSetAlsBlockedIPC:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchSetAlsBlockedL(aDataPackage);
		break;
	case ECustomGetLifeTimeIPC:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchGetLifeTimeL();
		break;
	case ECustomTerminateCallIPC:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchTerminateCallL(aDataPackage);
		break;
	case ECustomGetIccCallForwardingStatusIPC:
	    ret = iCtsyDispatcherCallControlDispatcher->DispatchGetCallForwardingIndicatorL();
		break;
	case ECtsyUpdateLifeTimeReq:
		ret = iCtsyDispatcherCallControlDispatcher->DispatchUpdateLifeTimeL(aDataPackage);
		break;

		// Phone functionality

	case EMobilePhoneGetFdnStatus:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetFdnStatusL();
		break;
	case EMobilePhoneGetNetworkRegistrationStatus:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetNetworkRegistrationStatusL();
		break;
	case EMmTsyBootNotifyModemStatusReadyIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchBootNotifyModemStatusReadyL();
		break;
	case EMmTsyBootNotifySimStatusReadyIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchBootNotifySimStatusReadyL();
		break;
	case EMobilePhoneGetHomeNetwork:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetHomeNetworkL();
		break;
	case EMmTsySimRefreshRegisterIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchSimRefreshRegisterL(aDataPackage);
		break;
	case EMmTsyGetCallForwardingNumberIPC: 		// Deprecated parts of the boot sequence,
	                                            // deliberately not passed to Licensee LTSY

	case EMmTsyGetCustomVendorExtPtrIPC:		// Part of deprecated custom API initialisation
												// deliberately not passed to Licensee LTSY
		break;
	case EReadViagHomeZoneCacheIPC:				// Deliberated not passed to LTSY
												// Leave like this unless this custom functionality
												// is needed
		break;
	case ECustomNetWakeupIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchNspsWakeupL();
		break;
	case ECustomSetSystemNetworkModeIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchSetSystemNetworkModeL(aDataPackage);
		break;
	case ECustomGetCurrentSystemNetworkModesIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetCurrentSystemNetworkModesL();
		break;
	case ECustomResetNetServerIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchResetNetServerL();
		break;
	case ECustomSetAlwaysOnMode:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchSetAlwaysOnModeL(aDataPackage);
		break;
	case ECustomSetDriveModeIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchSetDriveModeL(aDataPackage);
		break;
	case ECustomReadHSxPAStatusIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetHspaStatusL();
		break;
	case ECustomWriteHSxPAStatusIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchSetHspaStatusL(aDataPackage);
		break;
	case ECustomGetNetworkProviderNameIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetNetworkProviderNameL();
		break;
	case ECustomGetOperatorNameIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetOperatorNameL();
		break;
	case ECustomGetCellInfoIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetCellInfoL();
		break;
	case ECtsyPhoneTerminateAllCallsReq:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchTerminateAllCallsL(aDataPackage);
		break;

		// Phone Functional Unit IPCs

	case EMobilePhoneGetServiceProviderName:
	    ret = iCtsyDispatcherPhoneDispatcher->DispatchGetServiceProviderNameL();
		break;
	case EMobilePhoneGetPhoneId:
	    ret = iCtsyDispatcherPhoneDispatcher->DispatchGetPhoneIdL(KPhoneIdReqOriginPhoneFu);
		break;
	case EMobilePhoneGetDetectedNetworksV2Phase1:
	    ret = iCtsyDispatcherPhoneDispatcher->DispatchGetDetectedNetworksL();
		break;
	case EMobilePhoneGetDetectedNetworksCancel:
	    ret = iCtsyDispatcherPhoneDispatcher->DispatchGetDetectedNetworksCancelL();
		break;
	case EMobilePhoneSelectNetwork:
	    ret = iCtsyDispatcherPhoneDispatcher->DispatchSelectNetworkL(aDataPackage);
		break;
	case EMobilePhoneSelectNetworkCancel:
	    ret = iCtsyDispatcherPhoneDispatcher->DispatchSelectNetworkCancelL();
		break;
	case EMobilePhoneSetNetworkSelectionSetting:
	    ret = iCtsyDispatcherPhoneDispatcher->DispatchSetNetworkSelectionSettingL(aDataPackage);
		break;
	case ECtsyPhoneCellInfoIndReq:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchRegisterCellInfoChangeNotificationL();
		break;
	case ECtsyPhoneCellInfoReq:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetPhoneCellInfoL();
		break;
	case ECustomGetServiceTableSupportbyApplicationIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetUsimServiceSupportL(aDataPackage);
		break;
	case EMobilePhoneGetCurrentActiveUSimApplication:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetCurrentActiveUsimApplicationL();
		break;
	case ECustomGetBandSelectionIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetSystemNetworkBandL();
		break;
	case ECustomSetBandSelectionIPC:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchSetSystemNetworkBandL(aDataPackage);
		break;

		// Security Functional Unit IPCs

	case EMobilePhoneGetSecurityCodeInfo:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchGetSecurityCodeInfoL(aDataPackage);
		break;
	case EMobilePhoneGetLockInfo:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchGetLockInfoL(aDataPackage);
		break;
	case EMobilePhoneAbortSecurityCode:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchAbortSecurityCodeL(aDataPackage);
		break;
	case EMmTsySecurityGetSimActivePinStateIPC:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchGetCurrentActivePinL();
		break;
	case EMobilePhoneSetLockSetting:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchSetLockSettingL(aDataPackage);
		break;
	case EMobilePhoneVerifySecurityCode:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchVerifySecurityCodeL(aDataPackage);
		break;
	case EMmTsyPhoneGetPin1DisableSupportedIPC:
		ret = iCtsyDispatcherSecurityDispatcher->DispatchGetPin1DisableSupportedL();
		break;		
	case ECustomCheckSecurityCodeIPC:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchCheckSecurityCodeL(aDataPackage);
		break;
	case ECustomDisablePhoneLockIPC:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchDisablePhoneLockL(aDataPackage);
		break;
	case ECustomGetCipheringInfoIPC:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchGetCipheringInfoL();
		break;
	case ECustomIsBlockedIPC:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchIsSecurityCodeBlockedL(aDataPackage);
		break;
	case ECustomCheckSecurityCodeCancelIPC:
	    ret = iCtsyDispatcherSecurityDispatcher->DispatchCheckSecurityCodeCancelL(aDataPackage);
		break;		

		// PhonebookEn Functional Unit IPCs

	case EMmTsyENStoreGetInfoIPC:
	    ret = iCtsyDispatcherPhonebookEnDispatcher->DispatchStoreGetInfoL();
		break;
	case EMmTsyENStoreReadAllPhase1IPC:
	    ret = iCtsyDispatcherPhonebookEnDispatcher->DispatchStoreReadAllL();
		break;
	case EMmTsyENStoreReadIPC:
	    ret = iCtsyDispatcherPhonebookEnDispatcher->DispatchStoreReadEntryL(aDataPackage);
		break;

		// CellBroadcast Functional Unit IPCs

	case EMobileBroadcastMessagingSetFilterSetting:
	    ret = iCtsyDispatcherCellBroadcastDispatcher->DispatchSetBroadcastFilterSettingL(aDataPackage);
		break;
	case EMobileBroadcastMessagingReceiveMessage:
	    ret = iCtsyDispatcherCellBroadcastDispatcher->DispatchActivateBroadcastReceiveMessageL(aDataPackage);
		break;
	case EMobileBroadcastMessagingReceiveMessageCancel:
	    ret = iCtsyDispatcherCellBroadcastDispatcher->DispatchReceiveMessageCancelL(aDataPackage);
		break;
	case ECustomStartSimCbTopicBrowsingIPC:
	    ret = iCtsyDispatcherCellBroadcastDispatcher->DispatchStartSimCbTopicBrowsingL();
		break;	
	case ECustomDeleteSimCbTopicIPC:
	    ret = iCtsyDispatcherCellBroadcastDispatcher->DispatchDeleteSimCbTopicL(aDataPackage);
		break;	

		// PhonebookOn Functional Unit IPCs

	case EMmTsyONStoreReadIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreReadL(aDataPackage);
		break;
	case EMmTsyONStoreDeleteAllIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreDeleteAllL();
		break;
	case EMmTsyONStoreReadEntryIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreReadEntryL(aDataPackage);
		break;
	case EMmTsyONStoreGetInfoIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreGetInfoL();
		break;
	case EMmTsyONStoreReadSizeIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreGetReadStoreSizeL();
		break;
	case EMmTsyONStoreDeleteIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreDeleteEntryL(aDataPackage);
		break;
	case EMmTsyONStoreWriteEntryIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreWriteEntryL(aDataPackage);
		break;
	case EMmTsyONStoreWriteIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreWriteL(aDataPackage);
		break;
	case EMmTsyONStoreWriteSizeIPC:
	    ret = iCtsyDispatcherPhonebookOnDispatcher->DispatchStoreGetStoreSizeL();
		break;

		// Phonebook Functional Unit IPCs

	case EMmTsyPhoneBookStoreReadIPC:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchStoreReadEntryL(aDataPackage);
		break;
	case EMmTsyPhoneBookStoreDeleteIPC:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchStoreDeleteEntryL(aDataPackage);
		break;
	case EMmTsyPhoneBookStoreCacheIPC:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchStoreCacheL(aDataPackage);
		break;
	case EMmTsyPhoneBookStoreGetInfoIPC:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchStoreGetInfoL(aDataPackage);
		break;
	case EMmTsyPhoneBookStoreInitIPC:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchStoreInitialiseL(aDataPackage);
		break;
	case EMmTsyPhoneBookStoreDeleteAllIPC:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchStoreDeleteAllL(aDataPackage);
		break;
	case EMobilePhoneStoreReadAllPhase1:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchSmsStoreReadAllL();
		break;
	case EMmTsyPhoneBookStoreWriteIPC:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchStoreWriteEntryL(aDataPackage);
		break;
	case EMobilePhoneGetPhoneStoreInfo:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchGetPhoneStoreInfoL(aDataPackage);
		break;
	case EMobilePhoneStoreDelete:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchSmsStoreDeleteEntryL(aDataPackage);
		break;
	case EMobilePhoneStoreDeleteAll:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchSmsStoreDeleteAllL();
		break;
	case EMobilePhoneStoreGetInfo:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchSmsStoreGetInfoL();
		break;
	case EMobilePhoneStoreRead:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchSmsStoreReadEntryL(aDataPackage);
		break;
	case EMobilePhoneStoreWrite:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchSmsStoreWriteEntryL(aDataPackage);
		break;
	case EMmTsyPhoneBookStoreCacheCancelIPC:
	    ret = iCtsyDispatcherPhonebookDispatcher->DispatchStoreCacheCancelL(aDataPackage);
		break;

		// Sim Functional Unit IPCs

	case EMobilePhoneGetAPNControlListServiceStatus:
	    ret = iCtsyDispatcherSimDispatcher->DispatchGetApnControlListServiceStatusL();
		break;
	case EMobilePhoneDeleteAPNName:
	    ret = iCtsyDispatcherSimDispatcher->DispatchDeleteApnNameL(aDataPackage);
		break;
	case EMobilePhoneEnumerateAPNEntries:
	    ret = iCtsyDispatcherSimDispatcher->DispatchEnumerateApnEntriesL();
		break;
	case EMobilePhoneChangeSecurityCode:
	    ret = iCtsyDispatcherSimDispatcher->DispatchChangeSecurityCodeL(aDataPackage);
		break;
	case EMobilePhoneSetFdnSetting:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSetFdnSettingL(aDataPackage);
		break;
	case EMobilePhoneGetCustomerServiceProfile:
	    ret = iCtsyDispatcherSimDispatcher->DispatchGetCustomerServiceProfileL();
		break;
	case EMobilePhoneGetSubscriberId:
	    ret = iCtsyDispatcherSimDispatcher->DispatchGetSubscriberIdL();
		break;
	case EMobilePhoneAppendAPNName:
	    ret = iCtsyDispatcherSimDispatcher->DispatchAppendApnNameL(aDataPackage);
		break;
	case EMmTsySimGetICCType:
	    ret = iCtsyDispatcherSimDispatcher->DispatchGetIccTypeL();
		break;
	case EMobilePhoneSetIccMessageWaitingIndicators:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSetIccMessageWaitingIndicatorsL(aDataPackage);
		break;
	case EMobilePhoneSetAPNControlListServiceStatus:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSetApnControlListServiceStatusL(aDataPackage);
		break;
	case EMobilePhoneGetAPNname:
	    ret = iCtsyDispatcherSimDispatcher->DispatchGetApnNameL(aDataPackage);
		break;
	case EMmTsySimRefreshDoneIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSimRefreshDoneL(aDataPackage);
		break;
	case EMobilePhoneGetServiceTable:
		ret = iCtsyDispatcherSimDispatcher->DispatchGetServiceTableL(aDataPackage);
		break;
	case EMobilePhoneGetIccMessageWaitingIndicators:
		ret = iCtsyDispatcherSimDispatcher->DispatchGetIccMessageWaitingIndicatorsL();
		break;
	case ECustomSimLockActivateIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSimLockActivateL(aDataPackage);
		break;
	case ECustomSimLockDeActivateIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSimLockDeActivateL(aDataPackage);
		break;
	case ECustomGetATRIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchGetAnswerToResetL(aDataPackage);
		break;
	case ECustomGetSimCardReaderStatusIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchGetSimCardReaderStatusL();
		break;
	case ECustomGetSimAuthenticationDataIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchGetSimAuthenticationDataL(aDataPackage);
		break;
	case ECustomPowerSimOffIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchPowerSimOffL();
		break;
	case ECustomPowerSimOnIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchPowerSimOnL();
		break;
	case ECustomReadSimFileIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchReadSimFileL(aDataPackage);
		break;
	case ECustomSendAPDUReqIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSendApduRequestL(aDataPackage);
		break;
	case ECustomSendAPDUReqV2IPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSendApduRequestV2L(aDataPackage);
		break;
	case ECustomSimWarmResetIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSimWarmResetL();
		break;
	case ECustomSetSimMessageStatusReadIPC:
	    ret = iCtsyDispatcherSimDispatcher->DispatchSetSimMessageStatusReadL(aDataPackage);
		break;

	// Sms Functional Unit IPCs

	case EMmTsyActivateSmsRouting:
		ret = iCtsyDispatcherSmsDispatcher->DispatchActivateSmsRoutingL();
		break;
	case EMmTsyDeactivateSmsRouting:
		ret = KErrNone; //do nothing for deactivating sms routing
		break;
	case EMmTsySmsSendSatMessage:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchSendSatSmsL(aDataPackage);
		break;
	case EMobileSmsMessagingGetMessageStoreInfo:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchGetSmsStoreInfoL();
		break;
	case EMobileSmsMessagingGetSmspListPhase1:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchGetSmspListL();
		break;
	case EMobileSmsMessagingNackSmsStored:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchNackSmsStoredL(aDataPackage);
		break;
	case EMobileSmsMessagingAckSmsStored:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchAckSmsStoredL(aDataPackage);
		break;
	case EMobileSmsMessagingResumeSmsReception:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchResumeSmsReceptionL();
		break;
	case EMobileSmsMessagingSendMessage:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchSendSmsMessageL(aDataPackage);
		break;
	case EMobileSmsMessagingSendMessageNoFdnCheck:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchSendSmsMessageNoFdnCheckL(aDataPackage);
		break;
	case EMobileSmsMessagingSetMoSmsBearer:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchSetMoSmsBearerL(aDataPackage);
		break;
	case EMobileSmsMessagingStoreSmspList:
	    ret = iCtsyDispatcherSmsDispatcher->DispatchStoreSmspListEntryL(aDataPackage);
		break;

		// CallControlMultiparty Functional Unit IPCs

	case EMobileConferenceCallHangUp:
	    ret = iCtsyDispatcherCallControlMultipartyDispatcher->DispatchConferenceHangUpL(aDataPackage);
		break;
	case EMobileConferenceCallAddCall:
	    ret = iCtsyDispatcherCallControlMultipartyDispatcher->DispatchConferenceAddCallL(aDataPackage);
		break;
	case EMobileConferenceCallCreateConference:
	    ret = iCtsyDispatcherCallControlMultipartyDispatcher->DispatchCreateConferenceL(aDataPackage);
		break;
	case EMobileConferenceCallSwap:
	    ret = iCtsyDispatcherCallControlMultipartyDispatcher->DispatchConferenceSwapL(aDataPackage);
		break;
	case EMobileCallGoOneToOne:
		ret = iCtsyDispatcherCallControlMultipartyDispatcher->DispatchConferenceGoOneToOneL(aDataPackage);
		break;

		// SupplementaryServices Functional Unit IPCs

	case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSendNetworkServiceRequestNoFdnCheckL(aDataPackage);
		break;
	case EMobilePhoneGetWaitingStatusPhase1:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchGetCallWaitingStatusL();
		break;
	case EMobilePhoneSendNetworkServiceRequest:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSendNetworkServiceRequestL(aDataPackage);
		break;
	case EMobilePhoneGetBarringStatusPhase1:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchGetCallBarringStatusL(aDataPackage);
		break;
	case EMobilePhoneSetCallBarringStatus:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSetCallBarringStatusL(aDataPackage);
		break;
	case EMobilePhoneSetCallForwardingStatus:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSetCallForwardingStatusL(aDataPackage);
		break;
	case EMobilePhoneSetCallWaitingStatus:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSetCallWaitingStatusL(aDataPackage);
		break;
	case EMobilePhoneSetSSPassword:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSetSsPasswordL(aDataPackage);
		break;
	case EMobilePhoneGetCallForwardingStatusPhase1:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchGetCallForwardingStatusL(aDataPackage);
		break;
	case EMobileUssdMessagingSendMessage:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSendUssdMessageL(aDataPackage);
	    break;
	case EMobileUssdMessagingSendMessageNoFdnCheck:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSendUssdMessageNoFdnCheckL(aDataPackage);
		break;
	case EMobileUssdMessagingSendRelease:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchSendUssdReleaseL();
		break;
	case ECustomClearCallBlackListIPC:
	    ret = iCtsyDispatcherSupplementaryServicesDispatcher->DispatchClearBlacklistL();
		break;

		// PacketServices Functional Unit IPCs

	case EPacketAttach:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchPacketAttachL();
		break;
	case EPacketGetAttachMode:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchGetPacketAttachModeL(aDataPackage);
		break;
	case EPacketGetNtwkRegStatus:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchGetPacketNetworkRegistrationStatusL();
		break;
	case EPacketDetach:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchPacketDetachL();
		break;
	case EPacketContextSetConfig:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchSetPdpContextConfigL(aDataPackage);
		break;
	case EPacketContextModifyActiveContext:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchModifyActivePdpContextL(aDataPackage);
		break;
	case EPacketContextInitialiseContext:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchInitialisePdpContextL(aDataPackage);
		break;
	case EPacketContextDelete:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchDeletePdpContextL(aDataPackage);
		break;
	case EPacketSetAttachMode:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchSetPacketAttachModeL(aDataPackage);
		break;
	case EPacketNotifyStatusChange:
		ret = iCtsyDispatcherPacketServicesDispatcher->DispatchGetStatusL();
		break;
	case EPacketSetDefaultContextParams:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchSetDefaultPdpContextParamsL(aDataPackage);
		break;
	case EPacketContextActivate:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchActivatePdpContextL(aDataPackage);
		break;
	case EPacketQoSSetProfileParams:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchSetPdpContextQosL(aDataPackage);
		break;
	case EPacketRejectActivationRequest:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchRejectNetworkInitiatedContextActivationRequestL();
		break;
	case EPacketContextDeactivate:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchDeactivatePdpContextL(aDataPackage);
		break;
	case EPacketContextAddPacketFilter:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchAddPacketFilterL(aDataPackage);
		break;
	case EPacketContextRemovePacketFilter:
		ret = iCtsyDispatcherPacketServicesDispatcher->DispatchRemovePacketFilterL(aDataPackage);
		break;
	case EPacketInitProxiesIPC:
		// deliberately not passed to Licensee LTSY 
		ret = KErrNone;	
		break;
	case EPacketGetStaticCaps:
	    ret = iCtsyDispatcherPacketServicesDispatcher->DispatchGetStaticCapabilitiesL();
		break;
	case EPacketEnumerateMbmsMonitorServiceList:
		ret = iCtsyDispatcherPacketServicesDispatcher->DispatchGetMaxNoMonitoredServiceListsL();
		break;
	case EPacketEnumerateMbmsActiveServiceList:
		ret = iCtsyDispatcherPacketServicesDispatcher->DispatchGetMaxNoActiveServicesL();
		break;
	case ECtsyPacketMbmsInitialiseContextReq:
		ret = iCtsyDispatcherPacketServicesDispatcher->DispatchInitialiseMbmsContextL(aDataPackage);
		break;
	case EPacketGetMbmsNetworkServiceStatus:
		ret = iCtsyDispatcherPacketServicesDispatcher->DispatchGetMbmsNetworkServiceStatusL(aDataPackage);
		break;
	case EPacketUpdateMbmsMonitorServiceList:
		ret = iCtsyDispatcherPacketServicesDispatcher->DispatchUpdateMbmsMonitorServiceListL(aDataPackage);
		break;
	case EPacketContextUpdateMbmsSessionList:
		ret = iCtsyDispatcherPacketServicesDispatcher->DispatchUpdateMbmsSessionListL(aDataPackage);
		break;

		// Sat Functional Unit IPCs

	case ESatTsySmsDeliverReport:
	    ret = iCtsyDispatcherSatDispatcher->DispatchSmsDeliverReportL(aDataPackage);
		break;
	case ESatTsyGetImageInstance:
	    ret = iCtsyDispatcherSatDispatcher->DispatchGetImageInstanceL(aDataPackage);
		break;
	case ESatTsyGetIMEI:
		ret = iCtsyDispatcherPhoneDispatcher->DispatchGetPhoneIdL(KPhoneIdReqOriginSatFu);
		break;
	case ESatTsyGetIconData:
	    ret = iCtsyDispatcherSatDispatcher->DispatchGetIconDataL(aDataPackage);
		break;
	case ESatTsyGetClut:
	    ret = iCtsyDispatcherSatDispatcher->DispatchGetClutL(aDataPackage);
		break;
	case ESatTsyGetBearerCapability:
	    ret = iCtsyDispatcherSatDispatcher->DispatchGetDefaultBearerCapabilityL();
		break;
	case ESatTsySmsPpDdlStatus:
	    ret = iCtsyDispatcherSatDispatcher->DispatchSmsPpDdlStatusL();
		break;
	case ESatTsyMoSmsControlActivation:
	    ret = iCtsyDispatcherSatDispatcher->DispatchGetSmsControlActivatedL();
		break;
	case ESatTsyCellBroadcast:
	    ret = iCtsyDispatcherSatDispatcher->DispatchCellBroadcastL(aDataPackage);
		break;
	case ESatTsyAccTech:
	    ret = iCtsyDispatcherSatDispatcher->DispatchAccessTechnologyL();
		break;
	case ESatTsyTimingAdvance:
	    ret = iCtsyDispatcherSatDispatcher->DispatchTimingAdvanceL();
		break;
	case ESatNotifyMoSmControlRequest:
	    ret = iCtsyDispatcherSatDispatcher->DispatchNotifySmControlRequestL(aDataPackage);
		break;
	case ESatTsyProvideLocalInfo:
	    ret = iCtsyDispatcherSatDispatcher->DispatchProvideLocationInfoL();
		break;
	case ESatTsySetPolling:
	    ret = iCtsyDispatcherSatDispatcher->DispatchSetPollingIntervalL(aDataPackage);
		break;
	case ESatTsySmsPpDdl:
	    ret = iCtsyDispatcherSatDispatcher->DispatchSmsPpDdlL(aDataPackage);
		break;
	case ESatTsyLocalInformationNmr:
	    ret = iCtsyDispatcherSatDispatcher->DispatchLocalInformationNmrL();
		break;
	case ESatTsySendEnvelope:
	    ret = iCtsyDispatcherSatDispatcher->DispatchSendEnvelopeL(aDataPackage);
		break;
	case ESatTsyServiceRequest:
	    ret = iCtsyDispatcherSatDispatcher->DispatchServiceRequestL(aDataPackage);
		break;
	case ESatTsyReady:
	    ret = iCtsyDispatcherSatDispatcher->DispatchReadyL();
		break;
	case ESatTsyGetIfPendingNotification:
	    ret = iCtsyDispatcherSatDispatcher->DispatchPCmdNotificationL(aDataPackage);
		break;
	case ESatTsyEnvelopeError:
	    ret = iCtsyDispatcherSatDispatcher->DispatchUssdControlEnvelopeErrorL();
		break;
	case ESatTsyTimerExpiredIPC:
	    ret = iCtsyDispatcherSatDispatcher->DispatchTimerExpiredL(aDataPackage);
		break;
	case ESatTerminalRsp:
	    ret = iCtsyDispatcherSatDispatcher->DispatchTerminalRspL(aDataPackage);
		break;
	case ESatNotifySetUpCallPCmd:
		//This IPC is called when a call connected envelope event is delayed because the terminal response to
		//the set up call proactive command must be sent before the event download envelope.
		//so this internal ipc just forwards the saved envelope to the event download api as normal.
		ret = iCtsyDispatcherSatDispatcher->DispatchEventDownloadL(aDataPackage);
	    //ret = iCtsyDispatcherSatDispatcher->DispatchNotifySetUpCallPcmdL(aDataPackage);
		break;
	case ESatMenuSelection:
	    ret = iCtsyDispatcherSatDispatcher->DispatchMenuSelectionL(aDataPackage);
		break;
	case ESatEventDownload:
	    ret = iCtsyDispatcherSatDispatcher->DispatchEventDownloadL(aDataPackage);
		break;
	case ESatTsyUssdStatus:
		ret = iCtsyDispatcherSatDispatcher->DispatchGetUssdControlSupportedL();
		break;
	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_EXTFUNCL_1, "WARNING: CMessageRouter::ExtFuncL unhandled IPC=%d", aIpc);
	 	break;
		} // switch (aIpc)

	return TSYLOGSETEXITERR(ret);
	} // CMessageRouter::ExtFuncL

void CMessageRouter::HandleNotifyModemReadyReceivedL()
	{
	/*
	 * This is the first IPC that is sent from CtsyDispatcher->CTSY indicating modem
	 * is ready. We piggy-back on the back of this to now query what callback indicators
	 * are supported.  We do it here rather than at construction because for some basebands
	 * it is possible this information must be read from configuration or even queried asynchronously.
	 * If it is async they can do it before invoking the CallbackBootNotifyModemStatusReady
	 *
	 */
	QuerySupporterCallbackIndicators();

	// Method to retrieve network related information which will get stored in various Ctsy caches.
	RetrieveNetworkInfoL();
	}

void CMessageRouter::QuerySupporterCallbackIndicators()
	{

	// Query the support for all the callback indicators
	// At present we do not do anything with this information (other than log it)
	// but in the future we could link this up to
	// registering for notifiers.  So if a baseband does not support the notifier
	// then client can be returned
	// KErrNotSUpported rather than just never completing.

	TUint32 indmask;
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchCallControlFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_1, "CallControl indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchPhoneFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_2, "Security indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchSecurityFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_3, "CallControl indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchPhonebookEnFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_4, "Phonebook EN indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchCellBroadcastFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_5, "Cell Broadcast indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchPhonebookOnFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_6, "Phonebook ON indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchPhonebookFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_7, "Phonebook indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchSimFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_8, "SIM indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchSmsFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_9, "Sms indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchCallControlMultipartyFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_10, "Call Control Multiparty indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchSupplementaryServicesFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_11, "Supplementary Services indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchPacketServicesFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_12, "Packet Services indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	iLtsyFactoryV1->IsCallbackIndicatorSupported(KDispatchSatFuncUnitId, EIndIdGroup1, indmask);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMESSAGEROUTER_QUERYSUPPORTERCALLBACKINDICATORS_13, "SAT indicator support. GroupID=%d, bitmask = 0x%x", EIndIdGroup1, (TUint)indmask);
	}

void CMessageRouter::RetrieveNetworkInfoL()
	{
	// Call methods to get various network information from the Ltsy
	iCtsyDispatcherPhoneDispatcher->DispatchGetCurrentNetworkInfoL();
	iCtsyDispatcherPhoneDispatcher->DispatchGetNetworkModeL();
	iCtsyDispatcherPhoneDispatcher->DispatchGetNitzInfoL();
	iCtsyDispatcherPhoneDispatcher->DispatchGetSignalStrengthL();
	iCtsyDispatcherPhoneDispatcher->DispatchGetBatteryInfoL();
	}
