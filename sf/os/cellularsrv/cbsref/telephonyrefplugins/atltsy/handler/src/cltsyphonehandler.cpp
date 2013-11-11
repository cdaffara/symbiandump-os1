// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CLtsyPhoneHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>
#include <etelmm.h>
#include <ctsy/ltsy/ltsylogger.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>

#include "cltsyphonehandler.h"
#include "ltsymacros.h"
#include "atgetmodemstatus.h"
#include "atgetnetworkregistrationstatus.h"
#include "atgetsimstatusready.h"
#include "atgetnetworkinfo.h"
#include "atgetsignalstrength.h"
#include "atgetbatteryinfo.h"
#include "atdetectnetwork.h"
#include "getphoneid.h"
#include "initializephone.h"
#include "mslogger.h"
#include "athangup.h"
#include "getoperatorname.h"
#include "atselectnetworkmode.h"
#include "atselectnetwork.h"
#include "atgetphonecellinfo.h"
#include "atgetcellinfo.h"
#include "atsetnotifycell.h"
#include "asynchelperrequest.h"
#include "atnetworkregstatuschange.h"
#include "atnotifycellinfo.h"
#include "notifybatteryandsignal.h"
#include "getfdnstatus.h"
#include "getcurrentnetworkinfo.h"
#include "atgetphonecellinfo.h"

CLtsyPhoneHandler::CLtsyPhoneHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback),iGloblePhone(aGloblePhone)
	{
	}

	
CLtsyPhoneHandler::~CLtsyPhoneHandler()
	{
	if(iAsyncHelperRequest)
		{
		delete iAsyncHelperRequest;
		iAsyncHelperRequest = NULL;
		}	
	if(iNetworkRegStatusChange)
		{
		delete iNetworkRegStatusChange;
		iNetworkRegStatusChange = NULL;
		}
	if(iNotifyCellInfo)
		{
		delete iNotifyCellInfo;
		iNotifyCellInfo = NULL;
		}
	if(iNotifyBatteryAndSignal)
		{
		delete iNotifyBatteryAndSignal;
		iNotifyBatteryAndSignal = NULL;
		}
	if(iInitializePhone)
		{
		delete iInitializePhone;
		iInitializePhone = NULL;
		}
	if(iATDetectNetwork)
		{
		delete iATDetectNetwork;
		iATDetectNetwork = NULL;
		}
	}


CLtsyPhoneHandler* CLtsyPhoneHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhoneHandler* self = new (ELeave) CLtsyPhoneHandler(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyPhoneHandler* CLtsyPhoneHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback,
		CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhoneHandler* self=
			CLtsyPhoneHandler::NewLC(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyPhoneHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	//for temp
	iNetworkRegStatusChange = CATNetworkRegStatusChange::NewL((*iGloblePhone), iCtsyDispatcherCallback);
	iAsyncHelperRequest = CAsyncHelperRequest::NewL((*iGloblePhone), iCtsyDispatcherCallback);
	iNotifyCellInfo = CATNotifyCellInfo::NewL((*iGloblePhone), iCtsyDispatcherCallback);
	iNotifyBatteryAndSignal = CNotifyBatteryAndSignal::NewL((*iGloblePhone), iCtsyDispatcherCallback);
	iInitializePhone = CInitializePhone::NewL(*iGloblePhone,
			                                  iCtsyDispatcherCallback,
			                                  *iNetworkRegStatusChange,
			                                  *iNotifyBatteryAndSignal);
	iATDetectNetwork  = CATDetectNetwork::NewL(*iGloblePhone,iCtsyDispatcherCallback);
	iATSelectNetwork  = CATSelectNetwork::NewL(*iGloblePhone,iCtsyDispatcherCallback);
	} // CLtsyPhoneHandler::ConstructL

TBool CLtsyPhoneHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		case KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId: 
			return ETrue;
		case KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetFdnStatusApiId: 
			return ETrue; 
		case KLtsyDispatchPhoneGetHomeNetworkApiId: 
			return ETrue;	
		case KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId: 
			return ETrue;
		case KLtsyDispatchPhoneSimRefreshRegisterApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetServiceProviderNameApiId: 
			return ETrue; //Not finished
		case KLtsyDispatchPhoneGetPhoneIdApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetDetectedNetworksApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetDetectedNetworksCancelApiId: 
			return ETrue; //
		case KLtsyDispatchPhoneSelectNetworkApiId: 
			return ETrue;
		case KLtsyDispatchPhoneSelectNetworkCancelApiId: 
			return ETrue; //KErrNotSupported
		case KLtsyDispatchPhoneSetNetworkSelectionSettingApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetCurrentNetworkInfoApiId: 
			return ETrue; //Finished ?
		case KLtsyDispatchPhoneGetNetworkModeApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetNitzInfoApiId: 
			return ETrue; 
		case KLtsyDispatchPhoneGetSignalStrengthApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetBatteryInfoApiId: 
			return ETrue;
		case KLtsyDispatchPhoneNspsWakeupApiId: 
			return EFalse;
		case KLtsyDispatchPhoneSetSystemNetworkModeApiId: 
			return EFalse;
		case KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId: 
			return ETrue;
		case KLtsyDispatchPhoneResetNetServerApiId: 
			return EFalse;
		case KLtsyDispatchPhoneSetAlwaysOnModeApiId: 
			return EFalse;
		case KLtsyDispatchPhoneSetDriveModeApiId: 
			return EFalse;
		case KLtsyDispatchPhoneGetHspaStatusApiId: 
			return EFalse;
		case KLtsyDispatchPhoneSetHspaStatusApiId: 
			return EFalse;
		case KLtsyDispatchPhoneGetNetworkProviderNameApiId: 
			return ETrue;  
		case KLtsyDispatchPhoneGetOperatorNameApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetCellInfoApiId: 
			return ETrue;  
		case KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetPhoneCellInfoApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetUsimServiceSupportApiId: 
			return EFalse;
		case KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId: 
			return EFalse;
		case KLtsyDispatchPhoneTerminateAllCallsApiId: 
			return ETrue;
		case KLtsyDispatchPhoneGetSystemNetworkBandApiId: 
			return EFalse;
		case KLtsyDispatchPhoneSetSystemNetworkBandApiId: 
			return EFalse;
		default:
			return EFalse;
		}
	}

void CLtsyPhoneHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1.
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask =   KLtsyDispatchPhoneNotifyNetworkModeChangeIndId |
						  KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId |
						  KLtsyDispatchPhoneNotifySignalStrengthChangeIndId |
						  KLtsyDispatchPhoneNotifyNetworkRegistrationStatusChangeIndId |
						  KLtsyDispatchPhoneNotifyNitzInfoChangeIndId |
						  KLtsyDispatchPhoneNotifyBatteryInfoChangeIndId |
						  KLtsyDispatchPhoneNotifyCurrentNetworkChangeIndId |
						  KLtsyDispatchPhoneNotifyRfStatusChangeIndId |
						  KLtsyDispatchPhoneNotifyCellInfoChangeIndId ;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	
	}

TInt CLtsyPhoneHandler::HandleGetFdnStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetFdnStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CGetFdnStatus* GetFdnStatus  = CGetFdnStatus::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(GetFdnStatus);
		GetFdnStatus->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetFdnStatusReqL


TInt CLtsyPhoneHandler::HandleGetNetworkRegistrationStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetNetworkRegistrationStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId);
		iInitializePhone->ExecutePendingRequest();   
		ret = KErrNone;
		}
	else if(ret == KErrNone)
		{
		CATNetworkRegistration* atNetworkRegStatus  = CATNetworkRegistration::NewL(*iGloblePhone,
				                                                                   iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atNetworkRegStatus);
		atNetworkRegStatus->SetIpcRequestCompleteObserver(this);
		atNetworkRegStatus->SetIpcId(KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId);
		iNetworkRegStatusChange->Disable();
		atNetworkRegStatus->StartRequest();
		}
	 
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetNetworkRegistrationStatusReqL


TInt CLtsyPhoneHandler::HandleGetHomeNetworkReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetHomeNetworkComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneGetHomeNetworkApiId);
        iInitializePhone->ExecutePendingRequest();   
		ret = KErrNone;
		}
	else if(ret == KErrNone)
		{
		CATNetworkInfo* ATNetworkInfo  = CATNetworkInfo::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATNetworkInfo);
		ATNetworkInfo->SetToGetHomeNwk();
		ATNetworkInfo->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetHomeNetworkReqL


TInt CLtsyPhoneHandler::HandleBootNotifyModemStatusReadyReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneBootNotifyModemStatusReadyComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;
    // This function is the entry of phone bootup
	// Let's get the phone bootup
	TInt ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneNotInitialised)
		{
		iGloblePhone->iPhoneStatus.iInitStatus = EPhoneInitialising;
		iGloblePhone->iEventSignalActive = ETrue;
		iInitializePhone->StartRequest();
		ret = KErrNone;
		}
	else if(ret == KErrNone)
		{
		CATGetModemStatus* ATGetModemStatus = CATGetModemStatus::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATGetModemStatus);
		ATGetModemStatus->StartRequest();
		}	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleBootNotifyModemStatusReadyReqL


TInt CLtsyPhoneHandler::HandleBootNotifySimStatusReadyReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneBootNotifySimStatusReadyComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;
	
	TInt ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneBootNotifySimStatusReadyApiId);
        iInitializePhone->ExecutePendingRequest();   
		ret = KErrNone;
		}
	else if(ret == KErrNone)
		{
		CATSIMCard* ATSIMCard  = CATSIMCard::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATSIMCard);
		ATSIMCard->StartRequest();
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleBootNotifySimStatusReadyReqL

TInt CLtsyPhoneHandler::HandleSimRefreshRegisterReqL(TUint16 /*aFilesToRegister*/)
/**
 * @param aFilesToRegister  Bitmask containing list of files to register for refresh indications.
 * The list of possible files is specified in TCacheFileTypes.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;
	
	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneSimRefreshRegisterApiId);
        iInitializePhone->ExecutePendingRequest();   
        ret = KErrNone;
		}
	else if(ret == EPhoneInitialised)
		{
		iAsyncHelperRequest->SetRequestId(KLtsyDispatchPhoneSimRefreshRegisterApiId);
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSimRefreshRegisterReqL

TInt CLtsyPhoneHandler::HandleGetServiceProviderNameReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetServiceProviderNameComp()
 *
 = * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	//TInt ret = KErrNotSupported;
	TInt ret = 0;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CGetOperatorNmae* GetOperatorName  = CGetOperatorNmae::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		GetOperatorName->SetRequestTypeL(KLtsyDispatchPhoneGetServiceProviderNameApiId);
		iGloblePhone->SetActiveRequest(GetOperatorName);
		GetOperatorName->StartRequest();
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetServiceProviderNameReqL


TInt CLtsyPhoneHandler::HandleGetPhoneIdReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetPhoneIdComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	//TInt ret = KErrNotSupported;
	TInt ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CGetPhoneID* GetPhoneID  = CGetPhoneID::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(GetPhoneID);
		GetPhoneID->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetPhoneIdReqL


TInt CLtsyPhoneHandler::HandleGetDetectedNetworksReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	//TInt ret = KErrNotSupported;
	TInt ret = 0;
	
	ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		iGloblePhone->iEventSignalActive = ETrue;
		iATDetectNetwork->StartRequest();
		}
	
	return TSYLOGSETEXITERR(ret);
	
	} // CLtsyPhoneHandler::HandleGetDetectedNetworksReqL


TInt CLtsyPhoneHandler::HandleGetDetectedNetworksCancelReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksCancelComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	//TInt ret = KErrNotSupported;
	TInt ret = 0;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if((ret == KErrInUse)&&(iATDetectNetwork->iCommandRunning))
		{
		if(iATDetectNetwork->iCommandCancel)
			{
			ret = KErrNotSupported;
			}
		else
			{
		    iATDetectNetwork->CancelCommand();
		    ret = KErrNone;
			}
		}
	else if(ret == KErrNone)
		{
		iAsyncHelperRequest->SetRequestId(KLtsyDispatchPhoneGetDetectedNetworksCancelApiId);
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetDetectedNetworksCancelReqL


TInt CLtsyPhoneHandler::HandleSelectNetworkReqL(TBool aIsManual, const RMobilePhone::TMobilePhoneNetworkManualSelection& aNetworkManualSelection)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSelectNetworkComp()
 *
 * @param aIsManual Specifies whether phone should use a manual or automatic network selection method.
 * @param aNetworkManualSelection If aIsManual==ETrue, then this parameter contain the user's manually selected network.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	if(!aIsManual)
		{
		return KErrNotSupported;
		}
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		iGloblePhone->iEventSignalActive = ETrue;
		iATSelectNetwork->SetNetworkSelection(aNetworkManualSelection);
		iATSelectNetwork->StartRequest();
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSelectNetworkReqL


TInt CLtsyPhoneHandler::HandleSelectNetworkCancelReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSelectNetworkCancelComp()
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = 0;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if((ret == KErrInUse)&&(iATSelectNetwork->iCommandRunning))
		{
		if(iATSelectNetwork->iCommandCancel)
			{
			ret = KErrNotSupported;
			}
		else
			{
			iATSelectNetwork->CancelCommand();
		    ret = KErrNone;
			}
		}
	else if(ret == KErrNone)
		{
		iAsyncHelperRequest->SetRequestId(KLtsyDispatchPhoneSelectNetworkCancelApiId);
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSelectNetworkCancelReqL


TInt CLtsyPhoneHandler::HandleSetNetworkSelectionSettingReqL(const RMobilePhone::TMobilePhoneNetworkSelectionV1& aPhoneNetworkSelection)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetNetworkSelectionSettingComp()
 *
 * @param aPhoneNetworkSelection The network selection to be set.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	TInt networkSelection = -1;
	switch (aPhoneNetworkSelection.iMethod)
		{
	case RMobilePhone::ENetworkSelectionAutomatic:
	    networkSelection = 0;
		break;
	case RMobilePhone::ENetworkSelectionManual:
	    networkSelection = 1;
		break;
	default:
	    networkSelection = 0;
		break;
		}
	
	if(networkSelection != -1)
		{
		ret = iGloblePhone->CheckGlobalPhoneStatus();
		if(ret == KErrNone)
			{
			CATSelectNetworkMode* ATSelectNetworkMode  = CATSelectNetworkMode::NewL(*iGloblePhone,iCtsyDispatcherCallback);
			iGloblePhone->iEventSignalActive = ETrue;
			iGloblePhone->SetActiveRequest(ATSelectNetworkMode);
			ATSelectNetworkMode->SetNetworkSelection(networkSelection);
			ATSelectNetworkMode->StartRequest();
			ret = KErrNone;
			}
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetNetworkSelectionSettingReqL


TInt CLtsyPhoneHandler::HandleGetCurrentNetworkInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentNetworkInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneGetCurrentNetworkInfoApiId);
        iInitializePhone->ExecutePendingRequest();   
		ret = KErrNone;
		}
	else if(ret == KErrNone)
		{
		CGetCurrentNetworkInfo* ATNetworkInfo  = CGetCurrentNetworkInfo::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATNetworkInfo);
		ATNetworkInfo->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetCurrentNetworkInfoReqL


TInt CLtsyPhoneHandler::HandleGetNetworkModeReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetNetworkModeComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneGetNetworkModeApiId);
        iInitializePhone->ExecutePendingRequest();   	
		ret = KErrNone;
		}
	else if(ret == EPhoneInitialised)
		{
		iAsyncHelperRequest->SetRequestId(KLtsyDispatchPhoneGetNetworkModeApiId);
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetNetworkModeReqL


TInt CLtsyPhoneHandler::HandleGetNitzInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetNitzInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneGetNitzInfoApiId);
        iInitializePhone->ExecutePendingRequest();   
		ret = KErrNone;
		}
	else if(ret == EPhoneInitialised)
		{
		iAsyncHelperRequest->SetRequestId(KLtsyDispatchPhoneGetNitzInfoApiId);
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetNITZInfoReqL()


TInt CLtsyPhoneHandler::HandleGetSignalStrengthReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetSignalStrengthComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneGetSignalStrengthApiId);
        iInitializePhone->ExecutePendingRequest();   
		ret = KErrNone;
		}
	else if(ret == KErrNone)
		{
		CATGetSignal* ATGetSignal  = CATGetSignal::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATGetSignal);
		ATGetSignal->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetSignalStrengthReqL()


TInt CLtsyPhoneHandler::HandleGetBatteryInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetBatteryInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == EPhoneInitialising)
		{
		iInitializePhone->AppendIpcRequestToPendingListL(KLtsyDispatchPhoneGetBatteryInfoApiId);
        iInitializePhone->ExecutePendingRequest();   
		ret = KErrNone;
		}
	else if(ret == KErrNone)
		{
		CATGetBattery* ATGetBattery  = CATGetBattery::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATGetBattery);
		ATGetBattery->StartRequest();
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetBatteryInfoReqL


TInt CLtsyPhoneHandler::HandleNspsWakeupReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneNspsWakeupComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleNspsWakeupReqL


TInt CLtsyPhoneHandler::HandleSetSystemNetworkModeReqL(RMmCustomAPI::TNetworkModeCaps /*aMode*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkModeComp()
 *
 * @param aMode Specifies the mode to set the system network to.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetSystemNetworkModeReqL


TInt CLtsyPhoneHandler::HandleGetCurrentSystemNetworkModesReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentSystemNetworkModesComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = 0;
	iAsyncHelperRequest->SetRequestId(KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId);
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetCurrentSystemNetworkModesReqL


TInt CLtsyPhoneHandler::HandleResetNetServerReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneResetNetServerComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleResetNetServerReqL


TInt CLtsyPhoneHandler::HandleSetAlwaysOnModeReqL(RMmCustomAPI::TSetAlwaysOnMode /*aMode*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetAlwaysOnModeComp()
 *
 * @param aMode The new "always on" mode to set, can be
 *               RMmCustomAPI::EAlwaysModeVPLMN for VPLMN (Visited Public Land Mobile Network) always on,
 *               RMmCustomAPI::EAlwaysModeHPLMN for HPLMN (Home Public Land Mobile Network) always on,
 *               RMmCustomAPI::EAlwaysModeBoth for VPLMN and HPLMN always on,
 *               RMmCustomAPI::EAlwaysModeNeither for neither VPLMN mor HPLMN always on.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetAlwaysOnModeReqL


TInt CLtsyPhoneHandler::HandleSetDriveModeReqL(RMmCustomAPI::TSetDriveMode /*aMode*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetDriveModeComp()
 *
 * @param aMode The new mode to set (RMmCustomAPI::EDeactivateDriveMode for deactivate drive mode
 *               or RMmCustomAPI::EActivateDriveMode to activate drive mode).
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetDriveModeReqL


TInt CLtsyPhoneHandler::HandleGetHspaStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetHspaStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetHspaStatusReqL


TInt CLtsyPhoneHandler::HandleSetHspaStatusReqL(RMmCustomAPI::THSxPAStatus /*aStatus*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetHspaStatusComp()
 *
 * @param aStatus The new HSxPA status (enable or disable).
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetHspaStatusReqL


TInt CLtsyPhoneHandler::HandleGetNetworkProviderNameReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetNetworkProviderNameComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CGetOperatorNmae* GetOperatorName  = CGetOperatorNmae::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		GetOperatorName->SetRequestTypeL(KLtsyDispatchPhoneGetNetworkProviderNameApiId);
		iGloblePhone->SetActiveRequest(GetOperatorName);
		GetOperatorName->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetNetworkProviderNameReqL


TInt CLtsyPhoneHandler::HandleGetOperatorNameReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetOperatorNameComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CGetOperatorNmae* GetOperatorName  = CGetOperatorNmae::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		GetOperatorName->SetRequestTypeL(KLtsyDispatchPhoneGetOperatorNameApiId);
		iGloblePhone->SetActiveRequest(GetOperatorName);
		GetOperatorName->StartRequest();
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetOperatorNameReqL


TInt CLtsyPhoneHandler::HandleGetCellInfoReqL()
/**
 * This request is completed by invoking one of the 
 * CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp().
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CATGetCellInfo* atGetCellInfo  = CATGetCellInfo::NewL(*iGloblePhone,
				                                              iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atGetCellInfo);
		atGetCellInfo->SetIpcId(KLtsyDispatchPhoneGetCellInfoApiId);
		atGetCellInfo->SetIpcRequestCompleteObserver(this);
		iNotifyCellInfo->Disable();
		atGetCellInfo->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetCellInfoReqL

TInt CLtsyPhoneHandler::HandleRegisterCellInfoChangeNotificationReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneRegisterCellInfoChangeNotificationComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	//TInt ret = KErrNotSupported;
	TInt ret = KErrNone;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CATSetNotifyCellInfo* atSetNotifyCellInfo  = CATSetNotifyCellInfo::NewL(*iGloblePhone,
				                                                                iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSetNotifyCellInfo);
		atSetNotifyCellInfo->SetIpcId(KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId);
		atSetNotifyCellInfo->SetIpcRequestCompleteObserver(this);
		atSetNotifyCellInfo->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleRegisterCellInfoChangeNotificationReqL 

TInt CLtsyPhoneHandler::HandleGetPhoneCellInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetPhoneCellInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CATGetPhoneCellInfo* atGetPhoneCellInfo  = CATGetPhoneCellInfo::NewL(*iGloblePhone,
				                                                             iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atGetPhoneCellInfo);
		atGetPhoneCellInfo->SetIpcRequestCompleteObserver(this);
		atGetPhoneCellInfo->SetIpcId(KLtsyDispatchPhoneGetPhoneCellInfoApiId);
		// disable to get the notification as we may get the incorrect response after CATGetPhoneCellInfo command was issued
		iNotifyCellInfo->Disable();
		atGetPhoneCellInfo->StartRequest();
		}
	else if (ret == KErrInUse)
		{
		// as dispather will post two IPC requests(KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId and KLtsyDispatchPhoneGetPhoneCellInfoApiId)
		// together, so we need store one and then execute this AT command after previous one was complete
		iPendingIpcId = KLtsyDispatchPhoneGetPhoneCellInfoApiId;
		CRequestBase* activeReq = iGloblePhone->CurrentActiveRequest();
		if(activeReq)
		    {
		    if(activeReq->IpcId() == KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId)
		        {
		        ret = KErrNone;
		        }
		    }
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetPhoneCellInfoReqL

TInt CLtsyPhoneHandler::HandleGetGetUsimServiceSupportReqL(TInt /*aApplicationNumber*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetUsimServiceSupportComp()
 * 
 * @param aApplicationNumber The application number to check support for in the USIM.
 * 
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetGetUsimServiceSupportReqL

TInt CLtsyPhoneHandler::HandleGetCurrentActiveUsimApplicationReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentActiveUsimApplicationComp()
 * 
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetCurrentActiveUsimApplicationReqL
  
TInt CLtsyPhoneHandler::HandleTerminateAllCallsReqL(TInt /*aCallId*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneTerminateAllCallsComp()
 * 
 * @param aCallId if a call is in the connecting state then this parameter will contain the id of that call, else 0.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy Phone]  Starting CLtsyPhoneHandler::HandleTerminateAllCallsReqL()"));
	
	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(ret == KErrNone)
		{
		CATHangUp* cHangUp = CATHangUp::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(cHangUp);
		cHangUp->SetHangupOperator(CATHangUp::ERMobilePhoneAPI);
		cHangUp->StartRequest();
		}
	return ret;
	} // CLtsyPhoneHandler::HandleTerminateAllCallsReqL

TInt CLtsyPhoneHandler::HandleGetSystemNetworkBandReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetSystemNetworkBandComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetSystemNetworkBandReqL

TInt CLtsyPhoneHandler::HandleSetSystemNetworkBandReqL(RMmCustomAPI::TBandSelection /*aBand*/, RMmCustomAPI::TNetworkModeCaps /*aMode*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkBandComp()
 * 
 * @param aBand the requested band
 * @param aMode the requested mode
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetSystemNetworkBandReqL

void CLtsyPhoneHandler::NotifyIpcRequestComplete(TInt aIpcId)
/**
 * This function was called by some IPC request after it complete the request 
 * 
 * If there are one more AT commands which need some interactions among them, the logic control should be put here.
 * 
 * @param aIpcId The IPC request Id
 * 
 * @return void
 */
    {
    if(aIpcId == KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId)
        {
        if(iPendingIpcId == KLtsyDispatchPhoneGetPhoneCellInfoApiId)
            {
            iGloblePhone->DeleteLastActiveRequest();
            CATGetPhoneCellInfo* getPhoneCellInfo = NULL;
            TRAPD(err,getPhoneCellInfo  = CATGetPhoneCellInfo::NewL(*iGloblePhone,iCtsyDispatcherCallback));
            if(err != KErrNone)
                {
                RMobilePhone::TMobilePhoneCellInfoV9 cellInfo; 
                iCtsyDispatcherCallback.CallbackPhoneGetPhoneCellInfoComp(err,cellInfo);
                }
            else
                {
                iGloblePhone->iEventSignalActive = ETrue;
                iGloblePhone->SetActiveRequest(getPhoneCellInfo);
                getPhoneCellInfo->SetIpcRequestCompleteObserver(this);
                getPhoneCellInfo->SetIpcId(KLtsyDispatchPhoneGetPhoneCellInfoApiId);
                getPhoneCellInfo->StartRequest();
                iPendingIpcId = -1;
                }
            }
        }
    else if(aIpcId == KLtsyDispatchPhoneGetPhoneCellInfoApiId)
        {
        // enable to receive the notificaiton of cell info changed
        iNotifyCellInfo->Enable();
        }
    else if(aIpcId == KLtsyDispatchPhoneGetCellInfoApiId)
        {
        iNotifyCellInfo->Enable();
        }
    else if(aIpcId == KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId)
        {
        iNetworkRegStatusChange->Enable();
        }
    }

// End of file
