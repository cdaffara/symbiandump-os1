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
// This contains CAsyncHelperRequest which is a helper request which callback to ctsy asynchronously 
// 

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <rmmcustomapi.h> 
#include "asynchelperrequest.h"
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include "globalphonemanager.h"
#include "tsyconfg.h"
#include "mslogger.h"

// Class CAsyncHelperRequest
// ---------------------------------------------------------------------------
// CAsyncHelperRequest::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAsyncHelperRequest* CAsyncHelperRequest::NewL(CGlobalPhonemanager& aGloblePhone,
		                           CCtsyDispatcherCallback& aCallBack,
		                           TInt aPriority )
	{
	CAsyncHelperRequest* newTimer=new(ELeave) CAsyncHelperRequest(aGloblePhone,aCallBack,aPriority);
	CleanupStack::PushL(newTimer);
	newTimer->ConstructL();
	CleanupStack::Pop();
	return newTimer;
	}
// ---------------------------------------------------------------------------
// CAsyncHelperRequest::CAsyncHelperRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
CAsyncHelperRequest ::CAsyncHelperRequest(CGlobalPhonemanager& aGloblePhone,
		                      CCtsyDispatcherCallback& aCallBack,
		                      TInt aPriority)
	: CTimer(aPriority),iCallBack(aCallBack),iGloblePhone(aGloblePhone)
	{
	CActiveScheduler::Add(this);
	iRequestId = 0;
	iReqArray.Reset();
	}
// ---------------------------------------------------------------------------
// CAsyncHelperRequest::~CAsyncHelperRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
CAsyncHelperRequest::~CAsyncHelperRequest()
	{
	Cancel();
	}
// ---------------------------------------------------------------------------
// CAsyncHelperRequest::SetRequestId
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAsyncHelperRequest::SetRequestId(const TInt aIpc)
	{
	TInt count = iReqArray.Count();
	if((count > 0)||IsActive())
		{
		iReqArray.Append(aIpc);
		}
	else
		{
		iRequestId = aIpc;
		StartTimer();
		}
	}
// ---------------------------------------------------------------------------
// CAsyncHelperRequest::RunL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAsyncHelperRequest::RunL()
	{
	switch(iRequestId)
		{
		case MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId:
			iCallBack.CallbackPhoneGetNetworkModeComp(KErrNone,RMobilePhone::ENetworkModeGsm);
			break;
		case MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId:
			iCallBack.CallbackPhoneGetCurrentSystemNetworkModesComp(KErrNone,RMmCustomAPI::KCapsNetworkModeGsm);
			break;
		case MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId:
			iCallBack.CallbackPhoneGetNitzInfoComp(KErrNone,iGloblePhone.iPhoneStatus.iMobilePhoneNITZ);
			break;
		case MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId:
			iCallBack.CallbackPhoneSimRefreshRegisterComp(KErrNone);
			break;
		case MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId:
			iCallBack.CallbackPacketServicesSetDefaultPdpContextParamsComp(KErrNone);
			break;
		case MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId:
			TUint aCaps ;
			aCaps=RPacketService::KCapsSetDefaultContextSupported;
			aCaps|=RPacketService::KCapsMSClassSupported;
			iCallBack.CallbackPacketServicesGetStaticCapabilitiesComp(KErrNone,aCaps);
			break;
		case MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId:
			iCallBack.CallbackPacketServicesGetMaxNoMonitoredServiceListsComp(KErrNone,1);
			break;
		case MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId:
			iCallBack.CallbackPacketServicesGetMaxNoActiveServicesComp(KErrNone,1);
			break;
		case MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId:
			iCallBack.CallbackPacketServicesGetStatusComp(KErrNone,iGloblePhone.iPhoneStatus.iPacketStatus,EFalse);
			break;
		case MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId:
			iCallBack.CallbackPhoneGetDetectedNetworksCancelComp(KErrNone);
			break;
		case MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId:
			iCallBack.CallbackPhoneSelectNetworkCancelComp(KErrGeneral);
			break;
		case MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId:
			iCallBack.CallbackSmsSetMoSmsBearerComp(KErrNotSupported);
			break;	
		case MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId:
		    iCallBack.CallbackPacketServicesGetPacketAttachModeComp(KErrNone, RPacketService::EAttachWhenNeeded);
		    break;
		default:
			break;
		}
	TInt count = iReqArray.Count();
	if(count > 0)
		{
		iRequestId = iReqArray[0];
		iReqArray.Remove(0);
		StartTimer();
		}
	}
// ---------------------------------------------------------------------------
// CAsyncHelperRequest::StartTimer
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAsyncHelperRequest::StartTimer(const TTimeIntervalMicroSeconds32 aTimeout)
	{
	if (IsActive())
		Cancel();
	After(aTimeout);
	}
// ---------------------------------------------------------------------------
// CAsyncHelperRequest::StopTimer
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAsyncHelperRequest::StopTimer()
	{
	Cancel();
	}

//
// End of file
