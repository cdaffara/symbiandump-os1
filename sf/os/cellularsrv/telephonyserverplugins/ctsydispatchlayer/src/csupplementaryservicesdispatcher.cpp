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
#include "csupplementaryservicesdispatcherTraces.h"
#endif

#include "csupplementaryservicesdispatcher.h"

#include <ctsy/ltsy/mltsydispatchsupplementaryservicesinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <mmlist.h>
#include <ctsy/rmmcustomapi.h>


#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"

CSupplementaryServicesDispatcher::CSupplementaryServicesDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CSupplementaryServicesDispatcher::CSupplementaryServicesDispatcher

	  
CSupplementaryServicesDispatcher::~CSupplementaryServicesDispatcher()
	{
	} // CSupplementaryServicesDispatcher::~CSupplementaryServicesDispatcher


CSupplementaryServicesDispatcher* CSupplementaryServicesDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSupplementaryServicesDispatcher* self =
		new (ELeave) CSupplementaryServicesDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CSupplementaryServicesDispatcher::NewLC


CSupplementaryServicesDispatcher* CSupplementaryServicesDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSupplementaryServicesDispatcher* self =
		CSupplementaryServicesDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CSupplementaryServicesDispatcher::NewL


void CSupplementaryServicesDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to SupplementaryServices functionality
	// from the factory
	
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId))
		{
		TAny* sendNetworkServiceRequestNoFdnCheckInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId,
		       	sendNetworkServiceRequestNoFdnCheckInterface);
		iLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck =
				static_cast<MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck*>(sendNetworkServiceRequestNoFdnCheckInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId))
		{
		TAny* getCallWaitingStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId,
		       	getCallWaitingStatusInterface);
		iLtsyDispatchSupplementaryServicesGetCallWaitingStatus =
				static_cast<MLtsyDispatchSupplementaryServicesGetCallWaitingStatus*>(getCallWaitingStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesGetCallWaitingStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId))
		{
		TAny* sendNetworkServiceRequestInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId,
		       	sendNetworkServiceRequestInterface);
		iLtsyDispatchSupplementaryServicesSendNetworkServiceRequest =
				static_cast<MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest*>(sendNetworkServiceRequestInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSendNetworkServiceRequest, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId))
		{
		TAny* getCallBarringStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId,
		       	getCallBarringStatusInterface);
		iLtsyDispatchSupplementaryServicesGetCallBarringStatus =
				static_cast<MLtsyDispatchSupplementaryServicesGetCallBarringStatus*>(getCallBarringStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesGetCallBarringStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId))
		{
		TAny* setCallBarringStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId,
		       	setCallBarringStatusInterface);
		iLtsyDispatchSupplementaryServicesSetCallBarringStatus =
				static_cast<MLtsyDispatchSupplementaryServicesSetCallBarringStatus*>(setCallBarringStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSetCallBarringStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId))
		{
		TAny* setCallForwardingStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId,
		       	setCallForwardingStatusInterface);
		iLtsyDispatchSupplementaryServicesSetCallForwardingStatus =
				static_cast<MLtsyDispatchSupplementaryServicesSetCallForwardingStatus*>(setCallForwardingStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSetCallForwardingStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId))
		{
		TAny* setCallWaitingStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId,
		       	setCallWaitingStatusInterface);
		iLtsyDispatchSupplementaryServicesSetCallWaitingStatus =
				static_cast<MLtsyDispatchSupplementaryServicesSetCallWaitingStatus*>(setCallWaitingStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSetCallWaitingStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId))
		{
		TAny* setSsPasswordInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId,
		       	setSsPasswordInterface);
		iLtsyDispatchSupplementaryServicesSetSsPassword =
				static_cast<MLtsyDispatchSupplementaryServicesSetSsPassword*>(setSsPasswordInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSetSsPassword, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId))
		{
		TAny* getCallForwardingStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId,
		       	getCallForwardingStatusInterface);
		iLtsyDispatchSupplementaryServicesGetCallForwardingStatus =
				static_cast<MLtsyDispatchSupplementaryServicesGetCallForwardingStatus*>(getCallForwardingStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesGetCallForwardingStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId))
		{
		TAny* sendUssdMessageInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId,
		       	sendUssdMessageInterface);
		iLtsyDispatchSupplementaryServicesSendUssdMessage =
				static_cast<MLtsyDispatchSupplementaryServicesSendUssdMessage*>(sendUssdMessageInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSendUssdMessage, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId))
		{
		TAny* sendUssdMessageNoFdnCheckInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId,
		       	sendUssdMessageNoFdnCheckInterface);
		iLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck =
				static_cast<MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck*>(sendUssdMessageNoFdnCheckInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId))
		{
		TAny* sendUssdReleaseInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId,
		       	sendUssdReleaseInterface);
		iLtsyDispatchSupplementaryServicesSendUssdRelease =
				static_cast<MLtsyDispatchSupplementaryServicesSendUssdRelease*>(sendUssdReleaseInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesSendUssdRelease, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSupplementaryServicesFuncUnitId, MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId))
		{
		TAny* clearBlacklistInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId,
		       	clearBlacklistInterface);
		iLtsyDispatchSupplementaryServicesClearBlacklist =
				static_cast<MLtsyDispatchSupplementaryServicesClearBlacklist*>(clearBlacklistInterface);
        __ASSERT_DEBUG(iLtsyDispatchSupplementaryServicesClearBlacklist, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	} // CSupplementaryServicesDispatcher::ConstructL

void CSupplementaryServicesDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CSupplementaryServicesDispatcher::SetDispatcherHolder

TInt CSupplementaryServicesDispatcher::DispatchSendNetworkServiceRequestNoFdnCheckL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSendNetworkServiceRequestNoFdnCheck
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck)
		{
		const TDesC* request;
		aDataPackage->UnPackData(request);
		ret = iLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck->HandleSendNetworkServiceRequestNoFdnCheckReqL(*request);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSendNetworkServiceRequestNoFdnCheckL

TInt CSupplementaryServicesDispatcher::DispatchGetCallWaitingStatusL()
/**
 * Pass EMobilePhoneGetWaitingStatusPhase1 request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesGetCallWaitingStatus)
		{
		ret = iLtsyDispatchSupplementaryServicesGetCallWaitingStatus->HandleGetCallWaitingStatusReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchGetCallWaitingStatusL

TInt CSupplementaryServicesDispatcher::DispatchSendNetworkServiceRequestL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSendNetworkServiceRequest
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSendNetworkServiceRequest)
		{
		const TDesC* request;
		aDataPackage->UnPackData(request);
		ret = iLtsyDispatchSupplementaryServicesSendNetworkServiceRequest->HandleSendNetworkServiceRequestReqL(*request);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSendNetworkServiceRequestL

TInt CSupplementaryServicesDispatcher::DispatchGetCallBarringStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneGetBarringStatusPhase1
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

    __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesGetCallBarringStatus)
		{
		RMobilePhone::TMobilePhoneCBCondition condition;
		aDataPackage->UnPackData(condition);
		ret = iLtsyDispatchSupplementaryServicesGetCallBarringStatus->HandleGetCallBarringStatusReqL(condition);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchGetCallBarringStatusL

TInt CSupplementaryServicesDispatcher::DispatchSetCallBarringStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetCallBarringStatus
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

    __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSetCallBarringStatus)
		{
		RMobilePhone::TMobilePhoneCBCondition condition;
		RMobilePhone::TMobilePhoneCBChangeV1* info;
		aDataPackage->UnPackData(condition, info);
		ret = iLtsyDispatchSupplementaryServicesSetCallBarringStatus->HandleSetCallBarringStatusReqL(condition,
				info->iAction, info->iServiceGroup, info->iPassword);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSetCallBarringStatusL

TInt CSupplementaryServicesDispatcher::DispatchSetCallForwardingStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetCallForwardingStatus
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSetCallForwardingStatus)
		{
		RMobilePhone::TMobilePhoneCFCondition condition;
		RMobilePhone::TMobilePhoneCFChangeV1* info;
		aDataPackage->UnPackData(condition, info);
		ret = iLtsyDispatchSupplementaryServicesSetCallForwardingStatus->HandleSetCallForwardingStatusReqL(condition,
				info->iServiceGroup,info->iAction,
				info->iNumber.iTelNumber,info->iTimeout);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSetCallForwardingStatusL

TInt CSupplementaryServicesDispatcher::DispatchSetCallWaitingStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetCallWaitingStatus
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

    __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSetCallWaitingStatus)
		{
		RMobilePhone::TMobileService service;
		RMobilePhone::TMobilePhoneServiceAction action;
		aDataPackage->UnPackData(service, action);
		ret = iLtsyDispatchSupplementaryServicesSetCallWaitingStatus->HandleSetCallWaitingStatusReqL(service, action);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSetCallWaitingStatusL

TInt CSupplementaryServicesDispatcher::DispatchSetSsPasswordL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetSSPassword
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSetSsPassword)
		{
		RMobilePhone::TMobilePhonePasswordChangeV2* password = NULL;
		TUint16* services = NULL;
		aDataPackage->UnPackData(&password, &services);
			
		__ASSERT_DEBUG(password, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(services, CtsyDispatcherPanic(EInvalidNullPtr));
		
		//may only come down from the CTSY as hardcoded values of 0 or 330
		//"Password for Barring Services" = 330 
	    //"Common Password for All Appropriate Services" = 0
		__ASSERT_DEBUG(*services == 0 || *services == 330, CtsyDispatcherPanic(EInvalidNullPtr));
			
		MLtsyDispatchSupplementaryServicesSetSsPassword::TServiceType serviceType 
			= static_cast<MLtsyDispatchSupplementaryServicesSetSsPassword::TServiceType>(*services);

		if(password->iNewPassword != password->iVerifiedPassword)
			{
			ret = KErrArgument;
			}
		else
			{
			ret = iLtsyDispatchSupplementaryServicesSetSsPassword->HandleSetSsPasswordReqL(password->iOldPassword, password->iNewPassword, serviceType);
			}
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSetSsPasswordL

TInt CSupplementaryServicesDispatcher::DispatchGetCallForwardingStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneGetCallForwardingStatusPhase1
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesGetCallForwardingStatus)
		{
		RMobilePhone::TMobilePhoneCFCondition condition;
		RMobilePhone::TMobileService service;
		aDataPackage->UnPackData(condition, service);
		ret = iLtsyDispatchSupplementaryServicesGetCallForwardingStatus->HandleGetCallForwardingStatusReqL(condition, service);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchGetCallForwardingStatusL

TInt CSupplementaryServicesDispatcher::DispatchSendUssdMessageL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileUssdMessagingSendMessage
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSendUssdMessage)
		{
		RMobileUssdMessaging::TMobileUssdAttributesV1Pckg** attributesPckg = NULL;
		RMobileUssdMessaging::TGsmUssdMessageData* data = NULL;
		aDataPackage->UnPackData(&data, &attributesPckg);
			
		__ASSERT_DEBUG(attributesPckg, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(data, CtsyDispatcherPanic(EInvalidNullPtr));
			
		const RMobileUssdMessaging::TMobileUssdAttributesV1& attributes =(**attributesPckg)();			
				
		ret = iLtsyDispatchSupplementaryServicesSendUssdMessage->HandleSendUssdMessageReqL(*data,
				attributes.iDcs,
				attributes.iFormat,
				attributes.iType);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSendUssdMessageL

TInt CSupplementaryServicesDispatcher::DispatchSendUssdMessageNoFdnCheckL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileUssdMessagingSendMessageNoFdnCheck
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck)
		{
		RMobileUssdMessaging::TMobileUssdAttributesV1Pckg** attributesPckg = NULL;
		RMobileUssdMessaging::TGsmUssdMessageData* data = NULL;
		aDataPackage->UnPackData(&data, &attributesPckg);
			
		__ASSERT_DEBUG(attributesPckg, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(data, CtsyDispatcherPanic(EInvalidNullPtr));
			
		const RMobileUssdMessaging::TMobileUssdAttributesV1& attributes =(**attributesPckg)();			
		
		ret = iLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck->HandleSendUssdMessageNoFdnCheckReqL(*data,
				attributes.iDcs,
				attributes.iFormat,
				attributes.iType);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSendUssdMessageNoFdnCheckL

TInt CSupplementaryServicesDispatcher::DispatchSendUssdReleaseL()
/**
 * Pass EMobileUssdMessagingSendRelease request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesSendUssdRelease)
		{
		ret = iLtsyDispatchSupplementaryServicesSendUssdRelease->HandleSendUssdReleaseReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchSendUssdReleaseL

TInt CSupplementaryServicesDispatcher::DispatchClearBlacklistL()
/**
 * Pass ECustomClearCallBlackListIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSupplementaryServicesClearBlacklist)
		{
		ret = iLtsyDispatchSupplementaryServicesClearBlacklist->HandleClearBlacklistReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSupplementaryServicesDispatcher::DispatchClearBlacklistL


//
// Callback handlers follow
//



void CSupplementaryServicesDispatcher::CallbackNotifyCallBarringStatusChange(TInt aError,
		RMobilePhone::TMobilePhoneCBCondition aCallBarringCondition)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallBarringStatusChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallBarringCondition The call barring condition that have changed
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallBarringCondition=%d"), aError, aCallBarringCondition);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aCallBarringCondition);
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyCallBarringStatusChange, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyCallBarringStatusChange

void CSupplementaryServicesDispatcher::CallbackReceiveUssdMessage(TInt aError,
				const TDesC8& aMessage, TUint8 aDcs, 
				RMobileUssdMessaging::TMobileUssdMessageType aType, 
				RMobileUssdMessaging::TMobileUssdDataFormat aFormat)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesReceiveUssdMessageInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aMessage The message that was received.
 * @param aDcs The Data Coding Scheme of the received USSD message.
 * @param aType The message type (depend on the invoke part of the message:
 * 									UnstructuredSS-Notify - RMobileUssdMessaging::EUssdMTNotify;
 * 									UnstructuredSS-Request - RMobileUssdMessaging::EUssdMTRequest;
 * 									ProcessUnstructuredSS-Request - RMobileUssdMessaging::EUssdMTReply).
 * @param aFormat The message format.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aDcs=%d, aType=%d, aFormat=%d"), aError, aDcs, aType, aFormat);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	
    RMobileUssdMessaging::TMobileUssdAttributesV1 ussdAttributes;
    ussdAttributes.iDcs = aDcs;
    ussdAttributes.iType = aType;
    ussdAttributes.iFormat = aFormat;
    ussdAttributes.iFlags = RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageType | RMobileUssdMessaging::KUssdMessageDcs;
	
	__ASSERT_DEBUG(aMessage.Length() <= RMobileUssdMessaging::KGsmUssdDataSize, CtsyDispatcherPanic(EBadLength));
	TPtrC8 gsmUssdMessageData = aMessage.Left(RMobileUssdMessaging::KGsmUssdDataSize);
	
	dataPackage.PackData(&gsmUssdMessageData, &ussdAttributes);
	
	iMessageManagerCallback.Complete(EMobileUssdMessagingReceiveMessage, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackReceiveUssdMessage

void CSupplementaryServicesDispatcher::CallbackNotifyCallWaitingStatusChange(TInt aError,
				RMobilePhone::TMobileService aServiceGroup,
				RMobilePhone::TMobilePhoneCWStatus aStatus)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallWaitingStatusChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aServiceGroup The relevant service group.
 * @param aStatus The new call waiting status.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aServiceGroup=%d, aStatus=%d"), aError, aServiceGroup, aStatus);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMobilePhone::TMobilePhoneCWInfoEntryV1 info;
    info.iServiceGroup = aServiceGroup;
    info.iStatus = aStatus;
    dataPackage.PackData(&info);
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyCallWaitingStatusChange, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyCallWaitingStatusChange

void CSupplementaryServicesDispatcher::CallbackNotifyCallForwardingStatusChange(TInt aError,
		RMobilePhone::TMobilePhoneCFCondition aCallForwardingCondition)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallForwardingStatusChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallForwardingCondition The new call forwarding condition. 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallForwardingCondition=%d"), aError,aCallForwardingCondition);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aCallForwardingCondition);
	
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyCallForwardingStatusChange, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyCallForwardingStatusChange

void CSupplementaryServicesDispatcher::CallbackNotifyCallForwardingActive(TInt aError,
		RMobilePhone::TMobileService aServiceGroup, RMobilePhone::TMobilePhoneCFActive aActiveType)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallForwardingActiveInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aServiceGroup The basic service group the call forwarding is active.
 * @param aActiveType Specifies whether CF unconditional or one of the conditional CF services is active. 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aService=%d, aActiveType=%d"), aError, aServiceGroup, aActiveType);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aServiceGroup,&aActiveType);
	
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyCallForwardingActive, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyCallForwardingActive

void CSupplementaryServicesDispatcher::CallbackSendNetworkServiceRequestNoFdnCheck(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestNoFdnCheckComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	
	iMessageManagerCallback.Complete(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSendNetworkServiceRequestNoFdnCheck

void CSupplementaryServicesDispatcher::CallbackGetCallWaitingStatus(TInt aError,
		const CMobilePhoneCWList& aCallWaitingList)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallWaitingStatusComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallWaitingList The list af the call waiting statuses.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallWaitingList.Enumerate=%d"), aError, aCallWaitingList.Enumerate());

	// Pack the data to return to the Common TSY
    CMmDataPackage dataPackage;
    dataPackage.PackData(&const_cast<CMobilePhoneCWList&>(aCallWaitingList));
	
	iMessageManagerCallback.Complete(EMobilePhoneGetWaitingStatusPhase1, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackGetCallWaitingStatus

void CSupplementaryServicesDispatcher::CallbackSendNetworkServiceRequest(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMobilePhoneSendNetworkServiceRequest, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSendNetworkServiceRequest

void CSupplementaryServicesDispatcher::CallbackGetCallBarringStatus(TInt aError,
		const CMobilePhoneCBList& aCallBarringList)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallBarringStatusComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallBarringList The call barring list.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallBarringList.Enumerate()=%d"), aError, aCallBarringList.Enumerate());

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&const_cast<CMobilePhoneCBList&>(aCallBarringList));
	
	iMessageManagerCallback.Complete(EMobilePhoneGetBarringStatusPhase1, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackGetCallBarringStatus

void CSupplementaryServicesDispatcher::CallbackSetCallBarringStatus(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallBarringStatusComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSetCallBarringStatus, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSetCallBarringStatus

void CSupplementaryServicesDispatcher::CallbackSetCallForwardingStatus(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallForwardingStatusComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	
	iMessageManagerCallback.Complete(EMobilePhoneSetCallForwardingStatus, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSetCallForwardingStatus

void CSupplementaryServicesDispatcher::CallbackSetCallWaitingStatus(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallWaitingStatusComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSetCallWaitingStatus, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSetCallWaitingStatus

void CSupplementaryServicesDispatcher::CallbackSetSsPassword(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetSsPasswordComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSetSSPassword, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSetSsPassword

void CSupplementaryServicesDispatcher::CallbackGetCallForwardingStatus(TInt aError,
		const CMobilePhoneCFList& aCallForwardingList)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallForwardingStatusComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallForwardingList The list of all the call forwarding definitions.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallForwardingList.Enumerate()=%d"), aError, aCallForwardingList.Enumerate());

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&const_cast<CMobilePhoneCFList&>(aCallForwardingList));
	
	iMessageManagerCallback.Complete(EMobilePhoneGetCallForwardingStatusPhase1, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackGetCallForwardingStatus

void CSupplementaryServicesDispatcher::CallbackSendUssdMessage(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMobileUssdMessagingSendMessage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSendUssdMessage

void CSupplementaryServicesDispatcher::CallbackSendUssdMessageNoFdnCheck(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageNoFdnCheckComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMobileUssdMessagingSendMessageNoFdnCheck, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSendUssdMessageNoFdnCheck

void CSupplementaryServicesDispatcher::CallbackSendUssdRelease(TInt aError, 
		TUint8 aOpCode, const TDesC& aAdditionalInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdReleaseComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aOpCode The Operation Code.
 * @param aAdditionalInfo Any additional information.
 */
	{ 
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aOpCode=%d"), aError, aOpCode);

	CMmDataPackage dataPackage;
	RMobilePhone::TMobilePhoneSendSSRequestV3 result;
	result.iOpCode = aOpCode;
	result.iAdditionalInfo = aAdditionalInfo;
	dataPackage.PackData(&result);
	
	iMessageManagerCallback.Complete(EMobileUssdMessagingSendRelease, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackSendUssdRelease

void CSupplementaryServicesDispatcher::CallbackClearBlacklist(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesClearBlacklistComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{ 
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECustomClearCallBlackListIPC, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackClearBlacklist

void CSupplementaryServicesDispatcher::CallbackGetDiagnosticOctets(TInt aError,
		TInt aCallId, TUint8 aOctet)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetDiagnosticOctetsInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId The Call ID of the call whose cause is set.
 * @param aOctet The octet to set.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d, aOctet=0x%x"), aError, aCallId, aOctet);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aCallId, &aOctet);
	
	
	iMessageManagerCallback.Complete(ECustomGetDiagnosticOctetsIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackGetDiagnosticOctets

void CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventForwardMode(TInt aError,
		RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsForwMode aForwardMode)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventForwardModeInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aForwardMode The forward mode (SS-Notification).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d, aForwardMode=%d") ,
      aError, aType, aMode, aForwardMode);
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsTypeAndMode typeAndMode;
    typeAndMode.iSsType = aType;
    typeAndMode.iSsMode = aMode;
    RMmCustomAPI::TSsInfo ssInfo;
    ssInfo.iForwMode = aForwardMode;
	dataPackage.PackData(&typeAndMode, &ssInfo);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsNetworkEventIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventForwardMode


void CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventCallWaiting(TInt aError,
		RMmCustomAPI::TSsMode aMode, TBool aCallIsWaiting)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventCallWaitingInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aMode The mode of the service.
 * @param aCallIsWaiting An indicator of the call waiting state of the call (callIsWaiting-Indicator).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMode=%d, aCallIsWaiting=%d") ,
      aError, aMode, aCallIsWaiting);
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsTypeAndMode typeAndMode;
    typeAndMode.iSsType = RMmCustomAPI::ESsCallWaiting;
    typeAndMode.iSsMode = aMode;
    RMmCustomAPI::TSsInfo ssInfo;
    ssInfo.iCallWait = aCallIsWaiting;
	dataPackage.PackData(&typeAndMode, &ssInfo);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsNetworkEventIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventCallWaiting


void CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventHoldMode(TInt aError,
		RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsHoldMode aHoldMode)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventHoldModeInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aHoldMode The hold mode (callOnHold-Indicator).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d, aHoldMode=%d") ,
      aError, aType, aMode, aHoldMode);
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsTypeAndMode typeAndMode;
    typeAndMode.iSsType = aType;
    typeAndMode.iSsMode = aMode;
    RMmCustomAPI::TSsInfo ssInfo;
    ssInfo.iCallHold = aHoldMode;
	dataPackage.PackData(&typeAndMode, &ssInfo);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsNetworkEventIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventHoldMode


void CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventConfrence(TInt aError,
		RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, TBool aConferenceIndicator)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventConfrenceInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aConferenceIndicator The confrence call indicator (mpty-Indicator).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d, aConferenceIndicator=%d") ,
      aError, aType, aMode, aConferenceIndicator);
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsTypeAndMode typeAndMode;
    typeAndMode.iSsType = aType;
    typeAndMode.iSsMode = aMode;
    RMmCustomAPI::TSsInfo ssInfo;
    ssInfo.iConfInd = aConferenceIndicator;
	dataPackage.PackData(&typeAndMode, &ssInfo);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsNetworkEventIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventConfrence


void CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventCug(TInt aError,
		RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, TUint16 aCugIndex)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventCugInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aCugIndex The Index associated with the invoked CUG (cug-Index).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d, aCugIndex=%d") ,
      aError, aType, aMode, aCugIndex);
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsTypeAndMode typeAndMode;
    typeAndMode.iSsType = aType;
    typeAndMode.iSsMode = aMode;
    RMmCustomAPI::TSsInfo ssInfo;
    ssInfo.iCugIndex = aCugIndex;
	dataPackage.PackData(&typeAndMode, &ssInfo);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsNetworkEventIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventCug


void CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventClirSuppression(TInt aError,
		RMmCustomAPI::TSsMode aMode, TBool aClirSuppressionRejected)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventClirSuppressionInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aMode The mode of the service.
 * @param aClirSuppressionRejected An indicator that indicate if the CLIR override has not been performed (clirSuppressionRejected).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMode=%d, aClirSuppressionRejected=%d") ,
      aError, aMode, aClirSuppressionRejected);
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsTypeAndMode typeAndMode;
    typeAndMode.iSsType = RMmCustomAPI::ESsClir;
    typeAndMode.iSsMode = aMode;
    RMmCustomAPI::TSsInfo ssInfo;
    ssInfo.iClirSuppReject = aClirSuppressionRejected;
	dataPackage.PackData(&typeAndMode, &ssInfo);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsNetworkEventIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventClirSuppression


void CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventEctCallState(TInt aError,
		RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsEctState aEctCallState,
        RMmCustomAPI::TSsChoice aEctChoice, const TDesC& aRemotePartyNumber)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventEctCallStateInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aEctCallState The ECT state (ect-Indicator.ect-CallState).
 * @param aEctChoice The ECT presentation (ect-Indicator.rdn).
 * @param aRemotePartyNumber The remote party number (ect-Indicator.rdn).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("err=%d, type=%d ") ,
      aError, aType);
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsTypeAndMode typeAndMode;
    typeAndMode.iSsType = aType;
    typeAndMode.iSsMode = aMode;
    RMmCustomAPI::TSsInfo ssInfo;
    ssInfo.iEctCallState = aEctCallState;
    ssInfo.iChoice = aEctChoice;
    ssInfo.iRemoteAddress = aRemotePartyNumber;
	dataPackage.PackData(&typeAndMode, &ssInfo);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsNetworkEventIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyNetworkEventEctCallState


void CSupplementaryServicesDispatcher::CallbackNotifyNetworkEvent(TInt aError,
		RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d") ,
      aError, aType, aMode);
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsTypeAndMode typeAndMode;
    typeAndMode.iSsType = aType;
    typeAndMode.iSsMode = aMode;
    RMmCustomAPI::TSsInfo ssInfo;
	dataPackage.PackData(&typeAndMode, &ssInfo);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsNetworkEventIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyNetworkEvent

void CSupplementaryServicesDispatcher::CallbackNotifyAdditionalInfo(TInt aError,
		TUint8 aOperationCode, const TDesC8& aAdditionalInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyAdditionalInfoInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aOperationCode The operational code.
 * @param aAdditionalInfo Any additional information.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aOperationCode=%d"), aError, aOperationCode);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
    RMmCustomAPI::TSsAdditionalInfo info;
    info.iOperationCode = aOperationCode;
    info.iAdditionalInfo = aAdditionalInfo;
    RMmCustomAPI::TSsAdditionalInfo* infoPtr = &info;
	dataPackage.PackData(&infoPtr);
	
	
	iMessageManagerCallback.Complete(ECustomSsAdditionalInfoNotificationIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyAdditionalInfo

void CSupplementaryServicesDispatcher::CallbackNotifyRequestComplete(TInt aError,
		 TInt aStatus)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyRequestCompleteInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatus The completion status.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d"), aError, aStatus);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aStatus);
	
	
	iMessageManagerCallback.Complete(ECustomNotifySsRequestCompleteIPC, &dataPackage, aError);
	
	} // CSupplementaryServicesDispatcher::CallbackNotifyRequestComplete

void CSupplementaryServicesDispatcher::CallbackNotifySendNetworkServiceRequest(TInt aError, RMobilePhone::TMobilePhoneNotifySendSSOperation aOperationCode, const TDesC& aAdditionalInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifySendNetworkServiceRequestInd()
 *
 * @param The Operation Code for a Send SS Invoke or Return result events, or the Error 
 * or Problem Code for a Send SS Return Error or Reject events.
 * @param The additional information (parameters) for a Send SS Invoke or Return result or Return Error events.
 * This is not used for a Send SS Reject event
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aOperationCode=%d"), aError, aOperationCode);
	
	__ASSERT_DEBUG(aAdditionalInfo.Length() <= RMobilePhone::KAdditionalInfoSize, CtsyDispatcherPanic(EBadLength));
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendSsRequest;
	sendSsRequest.iOpCode = aOperationCode;
	sendSsRequest.iAdditionalInfo = aAdditionalInfo.Left(RMobilePhone::KAdditionalInfoSize);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	data.PackData(&sendSsRequest);
	
	iMessageManagerCallback.Complete(ECtsyPhoneSendNetworkServiceRequestInd, &data, aError);
	} //CSupplementaryServicesDispatcher::CallbackNotifySendNetworkServiceRequest

void CSupplementaryServicesDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
/**
 * Part of the MDispatcherCallback interface. Used to complete requests handled
 * synchronously by the Licensee LTSY asynchronously back to the Common TSY.
 *  
 * @param aIpcDataPackage Package encapsulating the request.
 * 
 * @see MDispatcherCallback::CallbackSync
 */
	{
	TSYLOGENTRYEXIT;
	
	switch (aIpcDataPackage.iIpc)
		{
	
	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSUPPLEMENTARYSERVICESDISPATCHER_CALLBACKSYNC_1, "WARNING: CSupplementaryServicesDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CSupplementaryServicesDispatcher::CallbackSync



