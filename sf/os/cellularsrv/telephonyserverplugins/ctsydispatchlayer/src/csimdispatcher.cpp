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
#include "csimdispatcherTraces.h"
#endif

#include "csimdispatcher.h"

#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

#include <ctsy/rmmcustomapi.h>
#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"

static const TInt KApduInfoLength = 3;

CSimDispatcher::CSimDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CSimDispatcher::CSimDispatcher

	  
CSimDispatcher::~CSimDispatcher()
	{
	} // CSimDispatcher::~CSimDispatcher


CSimDispatcher* CSimDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSimDispatcher* self =
		new (ELeave) CSimDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CSimDispatcher::NewLC


CSimDispatcher* CSimDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSimDispatcher* self =
		CSimDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CSimDispatcher::NewL


void CSimDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to Sim functionality
	// from the factory
	
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId))
		{
		TAny* getApnControlListServiceStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId,
		       	getApnControlListServiceStatusInterface);
		iLtsyDispatchSimGetApnControlListServiceStatus =
				static_cast<MLtsyDispatchSimGetApnControlListServiceStatus*>(getApnControlListServiceStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetApnControlListServiceStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId))
		{
		TAny* deleteApnNameInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId,
		       	deleteApnNameInterface);
		iLtsyDispatchSimDeleteApnName =
				static_cast<MLtsyDispatchSimDeleteApnName*>(deleteApnNameInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimDeleteApnName, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId))
		{
		TAny* enumerateApnEntriesInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId,
		       	enumerateApnEntriesInterface);
		iLtsyDispatchSimEnumerateApnEntries =
				static_cast<MLtsyDispatchSimEnumerateApnEntries*>(enumerateApnEntriesInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimEnumerateApnEntries, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId))
		{
		TAny* changeSecurityCodeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId,
		       	changeSecurityCodeInterface);
		iLtsyDispatchSimChangeSecurityCode =
				static_cast<MLtsyDispatchSimChangeSecurityCode*>(changeSecurityCodeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimChangeSecurityCode, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId))
		{
		TAny* setFdnSettingInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId,
		       	setFdnSettingInterface);
		iLtsyDispatchSimSetFdnSetting =
				static_cast<MLtsyDispatchSimSetFdnSetting*>(setFdnSettingInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSetFdnSetting, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId))
		{
		TAny* getCustomerServiceProfileInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId,
		       	getCustomerServiceProfileInterface);
		iLtsyDispatchSimGetCustomerServiceProfile =
				static_cast<MLtsyDispatchSimGetCustomerServiceProfile*>(getCustomerServiceProfileInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetCustomerServiceProfile, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId))
		{
		TAny* getSubscriberIdInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId,
		       	getSubscriberIdInterface);
		iLtsyDispatchSimGetSubscriberId =
				static_cast<MLtsyDispatchSimGetSubscriberId*>(getSubscriberIdInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetSubscriberId, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId))
		{
		TAny* appendApnNameInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId,
		       	appendApnNameInterface);
		iLtsyDispatchSimAppendApnName =
				static_cast<MLtsyDispatchSimAppendApnName*>(appendApnNameInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimAppendApnName, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId))
		{
		TAny* getIccTypeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId,
		       	getIccTypeInterface);
		iLtsyDispatchSimGetIccType =
				static_cast<MLtsyDispatchSimGetActiveIccApplicationType*>(getIccTypeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetIccType, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId))
		{
		TAny* setIccMessageWaitingIndicatorsInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId,
		       	setIccMessageWaitingIndicatorsInterface);
		iLtsyDispatchSimSetIccMessageWaitingIndicators =
				static_cast<MLtsyDispatchSimSetIccMessageWaitingIndicators*>(setIccMessageWaitingIndicatorsInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSetIccMessageWaitingIndicators, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId))
		{
		TAny* setApnControlListServiceStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId,
		       	setApnControlListServiceStatusInterface);
		iLtsyDispatchSimSetApnControlListServiceStatus =
				static_cast<MLtsyDispatchSimSetApnControlListServiceStatus*>(setApnControlListServiceStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSetApnControlListServiceStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId))
		{
		TAny* getApnNameInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId,
		       	getApnNameInterface);
		iLtsyDispatchSimGetApnName =
				static_cast<MLtsyDispatchSimGetApnName*>(getApnNameInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetApnName, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId))
		{
		TAny* simRefreshDoneInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId,
		       	simRefreshDoneInterface);
		iLtsyDispatchSimSimRefreshDone =
				static_cast<MLtsyDispatchSimSimRefreshDone*>(simRefreshDoneInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSimRefreshDone, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId))
		{
		TAny* getServiceTableInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId,
		       	getServiceTableInterface);
		iLtsyDispatchSimGetServiceTable =
			static_cast<MLtsyDispatchSimGetServiceTable*>(getServiceTableInterface);
  		__ASSERT_DEBUG(iLtsyDispatchSimGetServiceTable, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId))
		{
	    TAny* getIccMessageWaitingIndicatorsInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId,
		       	getIccMessageWaitingIndicatorsInterface);
		iLtsyDispatchSimGetIccMessageWaitingIndicators =
			static_cast<MLtsyDispatchSimGetIccMessageWaitingIndicators*>(getIccMessageWaitingIndicatorsInterface);
		__ASSERT_DEBUG(iLtsyDispatchSimGetIccMessageWaitingIndicators, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId))
		{
		TAny* simLockActivateInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId,
		       	simLockActivateInterface);
		iLtsyDispatchSimSimLockActivate =
				static_cast<MLtsyDispatchSimSimLockActivate*>(simLockActivateInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSimLockActivate, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId))
		{
		TAny* simLockDeActivateInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId,
		       	simLockDeActivateInterface);
		iLtsyDispatchSimSimLockDeActivate =
				static_cast<MLtsyDispatchSimSimLockDeActivate*>(simLockDeActivateInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSimLockDeActivate, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId))
		{
		TAny* getAnswerToResetInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId,
		       	getAnswerToResetInterface);
		iLtsyDispatchSimGetAnswerToReset =
				static_cast<MLtsyDispatchSimGetAnswerToReset*>(getAnswerToResetInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetAnswerToReset, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId))
		{
		TAny* getSimCardReaderStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId,
		       	getSimCardReaderStatusInterface);
		iLtsyDispatchSimGetSimCardReaderStatus =
				static_cast<MLtsyDispatchSimGetSimCardReaderStatus*>(getSimCardReaderStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetSimCardReaderStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId))
		{
		TAny* getSimAuthenticationEapSimDataInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId,
				getSimAuthenticationEapSimDataInterface);
		iLtsyDispatchSimGetSimAuthenticationEapSimData =
				static_cast<MLtsyDispatchSimGetSimAuthenticationEapSimData*>(getSimAuthenticationEapSimDataInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetSimAuthenticationEapSimData, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId))
		{
		TAny* getSimAuthenticationEapAkaDataInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId,
		       	getSimAuthenticationEapAkaDataInterface);
		iLtsyDispatchSimGetSimAuthenticationEapAkaData =
				static_cast<MLtsyDispatchSimGetSimAuthenticationEapAkaData*>(getSimAuthenticationEapAkaDataInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimGetSimAuthenticationEapAkaData, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId))
		{
		TAny* powerSimOffInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId,
		       	powerSimOffInterface);
		iLtsyDispatchSimPowerSimOff =
				static_cast<MLtsyDispatchSimPowerSimOff*>(powerSimOffInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimPowerSimOff, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId))
		{
		TAny* powerSimOnInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId,
		       	powerSimOnInterface);
		iLtsyDispatchSimPowerSimOn =
				static_cast<MLtsyDispatchSimPowerSimOn*>(powerSimOnInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimPowerSimOn, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId))
		{
		TAny* readSimFileInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId,
		       	readSimFileInterface);
		iLtsyDispatchSimReadSimFile =
				static_cast<MLtsyDispatchSimReadSimFile*>(readSimFileInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimReadSimFile, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId))
		{
		TAny* sendApduRequestInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId,
				sendApduRequestInterface);
		iLtsyDispatchSimSendApduRequest =
				static_cast<MLtsyDispatchSimSendApduRequest*>(sendApduRequestInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSendApduRequest, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId))
		{
		TAny* sendApduRequestV2Interface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId,
				sendApduRequestV2Interface);
		iLtsyDispatchSimSendApduRequestV2 =
				static_cast<MLtsyDispatchSimSendApduRequestV2*>(sendApduRequestV2Interface);
        __ASSERT_DEBUG(iLtsyDispatchSimSendApduRequestV2, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId))
		{
		TAny* simWarmResetInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId,
		       	simWarmResetInterface);
		iLtsyDispatchSimSimWarmReset =
				static_cast<MLtsyDispatchSimSimWarmReset*>(simWarmResetInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSimWarmReset, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSimFuncUnitId, MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId))
		{
		TAny* setSimMessageStatusReadInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId,
		       	setSimMessageStatusReadInterface);
		iLtsyDispatchSimSetSimMessageStatusRead =
				static_cast<MLtsyDispatchSimSetSimMessageStatusRead*>(setSimMessageStatusReadInterface);
        __ASSERT_DEBUG(iLtsyDispatchSimSetSimMessageStatusRead, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	} // CSimDispatcher::ConstructL

void CSimDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CSimDispatcher::SetDispatcherHolder

TInt CSimDispatcher::DispatchGetApnControlListServiceStatusL()
/**
 * Unpack data related to EMobilePhoneGetAPNControlListServiceStatus
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSimGetApnControlListServiceStatus)
		{
		ret = iLtsyDispatchSimGetApnControlListServiceStatus->HandleGetApnControlListServiceStatusReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchGetApnControlListServiceStatusL

TInt CSimDispatcher::DispatchDeleteApnNameL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneDeleteAPNName
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call HandleDeleteApnNameReqL method in Licensee LTSY
	if (iLtsyDispatchSimDeleteApnName)
		{
		TUint32 index;
		aDataPackage->UnPackData(index);
		ret = iLtsyDispatchSimDeleteApnName->HandleDeleteApnNameReqL(index);
		}
	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchDeleteApnNameL

TInt CSimDispatcher::DispatchEnumerateApnEntriesL()
/**
 * Unpack data related to EMobilePhoneEnumerateAPNEntries
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSimEnumerateApnEntries)
		{
		ret = iLtsyDispatchSimEnumerateApnEntries->HandleEnumerateApnEntriesReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchEnumerateApnEntriesL

TInt CSimDispatcher::DispatchChangeSecurityCodeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneChangeSecurityCode
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
	if(iLtsyDispatchSimChangeSecurityCode)
		{
		RMobilePhone::TMobilePhoneSecurityCode* secCode = NULL;
		RMobilePhone::TMobilePhonePasswordChangeV1* passwds = NULL;
		aDataPackage->UnPackData(&secCode, &passwds);
		
		__ASSERT_DEBUG(secCode, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(passwds, CtsyDispatcherPanic(EInvalidNullPtr));			
		
		ret = iLtsyDispatchSimChangeSecurityCode->HandleChangeSecurityCodeReqL(*secCode, *passwds);
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchChangeSecurityCodeL

TInt CSimDispatcher::DispatchSetFdnSettingL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetFdnSetting
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
	if (iLtsyDispatchSimSetFdnSetting)
		{
		RMobilePhone::TMobilePhoneFdnSetting fdnSetting;
		aDataPackage->UnPackData(fdnSetting);
		ret = iLtsyDispatchSimSetFdnSetting->HandleSetFdnSettingReqL(fdnSetting);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSetFdnSettingL

TInt CSimDispatcher::DispatchGetCustomerServiceProfileL()
/**
 * Unpack data related to EMobilePhoneGetCustomerServiceProfile
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSimGetCustomerServiceProfile)
		{
		ret = iLtsyDispatchSimGetCustomerServiceProfile->HandleGetCustomerServiceProfileReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchGetCustomerServiceProfileL

TInt CSimDispatcher::DispatchGetSubscriberIdL()
/**
 * Unpack data related to EMobilePhoneGetSubscriberId
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSimGetSubscriberId)
		{
		ret = iLtsyDispatchSimGetSubscriberId->HandleGetSubscriberIdReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchGetSubscriberIdL

TInt CSimDispatcher::DispatchAppendApnNameL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneAppendAPNName
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call HandleAppendApnNameReqL method in Licensee LTSY
	if (iLtsyDispatchSimAppendApnName)
		{
		RMobilePhone::TAPNEntryV3* entry = NULL;
		aDataPackage->UnPackData(&entry);

		__ASSERT_DEBUG(entry, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchSimAppendApnName->HandleAppendApnNameReqL(*entry);
		}
	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchAppendApnNameL

TInt CSimDispatcher::DispatchGetIccTypeL()
/**
 * Unpack data related to EMmTsySimGetICCType
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSimGetIccType)
		{
		ret = iLtsyDispatchSimGetIccType->HandleGetActiveIccApplicationTypeReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchGetIccTypeL

TInt CSimDispatcher::DispatchSetIccMessageWaitingIndicatorsL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetIccMessageWaitingIndicators
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call HandleSetIccMessageWaitingIndicatorsReqL method in Licensee LTSY
	if (iLtsyDispatchSimSetIccMessageWaitingIndicators)
		{
		RMobilePhone::TMobilePhoneMessageWaitingV1* indicators = NULL;
		aDataPackage->UnPackData(&indicators);

		__ASSERT_DEBUG(indicators, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchSimSetIccMessageWaitingIndicators->HandleSetIccMessageWaitingIndicatorsReqL(*indicators);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSetIccMessageWaitingIndicatorsL

TInt CSimDispatcher::DispatchSetApnControlListServiceStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetAPNControlListServiceStatus
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call HandleSetApnControlListServiceStatusReqL method in Licensee LTSY
	if (iLtsyDispatchSimSetApnControlListServiceStatus)
		{
		RMobilePhone::TAPNControlListServiceStatus status;
		aDataPackage->UnPackData(status); 
		ret = iLtsyDispatchSimSetApnControlListServiceStatus->HandleSetApnControlListServiceStatusReqL(status);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSetApnControlListServiceStatusL

TInt CSimDispatcher::DispatchGetApnNameL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneGetAPNname
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call HandleGetApnNameReqL method in Licensee LTSY
	if (iLtsyDispatchSimGetApnName)
		{
		TUint32 index;
		aDataPackage->UnPackData(index);
		ret = iLtsyDispatchSimGetApnName->HandleGetApnNameReqL(index);
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchGetApnNameL

TInt CSimDispatcher::DispatchSimRefreshDoneL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsySimRefreshDoneIPC
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
	if (iLtsyDispatchSimSimRefreshDone)
		{
		//packed as a TBool in the CTSY really a TInt containing KErrXXX
		TBool error;
		aDataPackage->UnPackData(error);
			
		ret = iLtsyDispatchSimSimRefreshDone->HandleSimRefreshDoneReqL(static_cast<TInt>(error));
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSimRefreshDoneL

TInt CSimDispatcher::DispatchGetServiceTableL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to RMobilePhone::GetServiceTable
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
	if (iLtsyDispatchSimGetServiceTable)
		{		
		RMobilePhone::TMobilePhoneServiceTable serviceTable; 
		aDataPackage->UnPackData(serviceTable);  		
		ret = iLtsyDispatchSimGetServiceTable->HandleGetServiceTableReqL(serviceTable);
		}
	
	return TSYLOGSETEXITERR(ret);	
	} // CSimDispatcher::DispatchGetServiceTableL
	
TInt CSimDispatcher::DispatchGetIccMessageWaitingIndicatorsL()
/**
 * Unpack data related to EMobilePhoneGetIccMessageWaitingIndicators
 * and pass request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSimGetIccMessageWaitingIndicators)
		{
		ret = iLtsyDispatchSimGetIccMessageWaitingIndicators->HandleGetIccMessageWaitingIndicatorsReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CSimDispatcher::DispatchGetIccMessageWaitingIndicatorsL
	
TInt CSimDispatcher::DispatchSimLockActivateL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSimLockActivateIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSimSimLockActivate)
		{		
		RMmCustomAPI::TSimLockPassword* password = NULL; 
		RMmCustomAPI::TLockNumber* lockNumber = NULL;
		aDataPackage->UnPackData(&password, &lockNumber);
		__ASSERT_DEBUG(password, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(lockNumber, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchSimSimLockActivate->HandleSimLockActivateReqL(*password, *lockNumber);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSimLockActivateL

TInt CSimDispatcher::DispatchSimLockDeActivateL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSimLockDeActivateIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSimSimLockDeActivate)
		{
		RMmCustomAPI::TSimLockPassword* password = NULL; 
		RMmCustomAPI::TLockNumber* lockNumber = NULL;
		aDataPackage->UnPackData(&password, &lockNumber);
		__ASSERT_DEBUG(password, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(lockNumber, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchSimSimLockDeActivate->HandleSimLockDeActivateReqL(*password, *lockNumber);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSimLockDeActivateL

TInt CSimDispatcher::DispatchGetAnswerToResetL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomGetATRIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	if (iLtsyDispatchSimGetAnswerToReset)
		{
		TDes8* answerToReset = NULL;
		aDataPackage->UnPackData(answerToReset);
		__ASSERT_DEBUG(answerToReset, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchSimGetAnswerToReset->HandleGetAnswerToResetReqL(*answerToReset);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchGetAnswerToResetL

TInt CSimDispatcher::DispatchGetSimCardReaderStatusL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	if (iLtsyDispatchSimGetSimCardReaderStatus)
		{
		ret = iLtsyDispatchSimGetSimCardReaderStatus->HandleGetSimCardReaderStatusReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchGetSimCardReaderStatusL

TInt CSimDispatcher::DispatchGetSimAuthenticationDataL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomGetSimAuthenticationDataIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	RMmCustomAPI::TSimAuthenticationBase* basePtr = NULL;
	TInt* rfStateInfo = 0;
	aDataPackage->UnPackData(&basePtr, &rfStateInfo);
	__ASSERT_DEBUG(basePtr, CtsyDispatcherPanic(EInvalidNullPtr));
	
	if(basePtr->ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EEapSim)
		{
		RMmCustomAPI::TSimAuthenticationEapSim* eapSim = static_cast<RMmCustomAPI::TSimAuthenticationEapSim*> (basePtr);
		
		if (iLtsyDispatchSimGetSimAuthenticationEapSimData)
			{
			ret = iLtsyDispatchSimGetSimAuthenticationEapSimData->HandleGetSimAuthenticationEapSimDataReqL(eapSim->iRandomParameters,
																										   *rfStateInfo);
			}
		}
	else
	if(basePtr->ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EEapAka)
		{	
		RMmCustomAPI::TSimAuthenticationEapAka* eapAka = static_cast<RMmCustomAPI::TSimAuthenticationEapAka*>(basePtr);
		
		if (iLtsyDispatchSimGetSimAuthenticationEapAkaData)
			{
			ret = iLtsyDispatchSimGetSimAuthenticationEapAkaData->HandleGetSimAuthenticationEapAkaDataReqL(eapAka->iRandomParameters, 
																										   eapAka->iAUTN,
																										   *rfStateInfo);
			}
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchGetSimAuthenticationDataL

TInt CSimDispatcher::DispatchPowerSimOffL()
/**
 * Pass ECustomPowerSimOffIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSimPowerSimOff)
		{
		ret = iLtsyDispatchSimPowerSimOff->HandlePowerSimOffReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchPowerSimOffL

TInt CSimDispatcher::DispatchPowerSimOnL()
/**
 * Pass ECustomPowerSimOnIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSimPowerSimOn)
		{
		ret = iLtsyDispatchSimPowerSimOn->HandlePowerSimOnReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchPowerSimOnL

TInt CSimDispatcher::DispatchReadSimFileL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomReadSimFileIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSimReadSimFile)
		{
		
		RMmCustomAPI::TSimFileInfo* simFileInfo = NULL;
		aDataPackage->UnPackData(&simFileInfo);
		__ASSERT_DEBUG(simFileInfo, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchSimReadSimFile->HandleReadSimFileReqL(simFileInfo->iPath,simFileInfo->iOffSet,simFileInfo->iSize);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchReadSimFileL

TInt CSimDispatcher::DispatchSendApduRequestL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSendAPDUReqIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSimSendApduRequest)
		{
		RMmCustomAPI::TApdu* apduReq = NULL;
		aDataPackage->UnPackData(&apduReq);

		__ASSERT_DEBUG(apduReq, CtsyDispatcherPanic(EInvalidNullPtr));
		//will cause a ETel panic 1 if either iInfo or iData is missing
		__ASSERT_DEBUG(apduReq->iInfo, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(apduReq->iData, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(apduReq->iInfo->Length() >= KApduInfoLength, CtsyDispatcherPanic(EBadLength));

		const TDesC8& info = *(apduReq->iInfo);
		
		ret = iLtsyDispatchSimSendApduRequest->HandleSimSendApduRequestReqL(info[0],info[1],info[2],*(apduReq->iData));
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSendApduRequestL

TInt CSimDispatcher::DispatchSendApduRequestV2L(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSendAPDUReqV2IPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSimSendApduRequestV2)
		{
		
		RMmCustomAPI::TApduParameters* apduParameters = NULL;
		aDataPackage->UnPackData(apduParameters);
		__ASSERT_DEBUG(apduParameters, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchSimSendApduRequestV2->HandleSimSendApduRequestV2ReqL(apduParameters->iCardReaderId,apduParameters->iCmdData);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSendApduRequestV2L

TInt CSimDispatcher::DispatchSimWarmResetL()
/**
 * Pass ECustomSimWarmResetIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSimSimWarmReset)
		{
		ret = iLtsyDispatchSimSimWarmReset->HandleSimWarmResetReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSimWarmResetL

TInt CSimDispatcher::DispatchSetSimMessageStatusReadL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSetSimMessageStatusReadIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Unpack data for this request here if required.
	TTime* timestamp = NULL;
	TInt* timezone = NULL;
	aDataPackage->UnPackData(&timestamp, &timezone);

	if (!timestamp || !timezone)
		{
		ret = KErrCorrupt;
		}
	// Can Handle... method in Licensee LTSY
	else if (iLtsyDispatchSimSetSimMessageStatusRead) 
		{
		ret = iLtsyDispatchSimSetSimMessageStatusRead->HandleSetSimMessageStatusReadReqL(*timestamp,*timezone);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSimDispatcher::DispatchSetSimMessageStatusReadL

//
// Callback handlers follow
//



void CSimDispatcher::CallbackRefreshSimFiles(TInt aError, TUint16 aRefreshFileList)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param @param aRefreshFileList Bitmask containing list of files to refresh. The list of possible files is specified in TCacheFileTypes.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRefreshFileList=%d"), aError, aRefreshFileList);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aRefreshFileList);
	
	iMessageManagerCallback.Complete(EMmTsySimRefreshNowIPC, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackRefreshSimFiles

void CSimDispatcher::CallbackNotifyIccMessageWaitingIndicatorsChange(TInt aError,
		const RMobilePhone::TMobilePhoneMessageWaitingV1& aIndicators)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aIndicators The ICC message waiting indicators to return.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneMessageWaitingV1*>(&aIndicators));

	iMessageManagerCallback.Complete(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackNotifyIccMessageWaitingIndicatorsChange

void CSimDispatcher::CallbackNotifyApnListChange(TInt aError)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneNotifyAPNListChanged, aError);
	
	} // CSimDispatcher::CallbackNotifyApnListChange

void CSimDispatcher::CallbackNotifyApnControlListServiceStatusChange(TInt aError,
		RMobilePhone::TAPNControlListServiceStatus aStatus)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aStatus The new service status to return.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aStatus);

	iMessageManagerCallback.Complete(EMobilePhoneNotifyAPNControlListServiceStatusChange, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackNotifyApnControlListServiceStatusChange

void CSimDispatcher::CallbackGetApnControlListServiceStatus(TInt aError,
		RMobilePhone::TAPNControlListServiceStatus aStatus)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aStatus The APN Control List Service status stored on the USIM.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aStatus);
	
	iMessageManagerCallback.Complete(EMobilePhoneGetAPNControlListServiceStatus, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackGetApnControlListServiceStatus

void CSimDispatcher::CallbackDeleteApnName(TInt aError)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneDeleteAPNName, aError);
	
	} // CSimDispatcher::CallbackDeleteApnName

void CSimDispatcher::CallbackEnumerateApnEntries(TInt aError,
		TUint32 aNumEntries)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aNumEntries The total number of available entries.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aNumEntries=%d"), aError, aNumEntries);

	// Pack the data to return to the Common TSY

	CMmDataPackage dataPackage;
	dataPackage.PackData(&aNumEntries);
	
	iMessageManagerCallback.Complete(EMobilePhoneEnumerateAPNEntries, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackEnumerateApnEntries

void CSimDispatcher::CallbackChangeSecurityCode(TInt aError)
/**
 * Callback function to be used by the request to complete the EMobilePhoneChangeSecurityCode
 * request back to the Common TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY

	iMessageManagerCallback.Complete(EMobilePhoneChangeSecurityCode, aError);
	} // CSimDispatcher::CallbackChangeSecurityCode

void CSimDispatcher::CallbackSetFdnSetting(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimSetFdnSettingComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSetFdnSetting, aError);
	
	} // CSimDispatcher::CallbackSetFdnSetting

void CSimDispatcher::CallbackGetCustomerServiceProfile(TInt aError,
		const RMobilePhone::TMobilePhoneCspFileV1& aCsp)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCsp The Customer Service Profile file that is stored on the SIM.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneCspFileV1*>(&aCsp));

	iMessageManagerCallback.Complete(EMobilePhoneGetCustomerServiceProfile, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackGetCustomerServiceProfile

void CSimDispatcher::CallbackGetSubscriberId(TInt aError,
		const TDesC8& aId)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimGetSubscriberIdComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aId The subscriber id to be returned.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	__ASSERT_DEBUG(aId.Length() <= RMobilePhone::KIMSISize, CtsyDispatcherPanic(EBadLength));

	//the CTSY unpack ad does a TDesC copy to a TBuf8 of size KGsmUssdDataSize
	TPtrC8 imsi = aId.Left(RMobilePhone::KIMSISize);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&imsi);

	iMessageManagerCallback.Complete(EMobilePhoneGetSubscriberId, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackGetSubscriberId

void CSimDispatcher::CallbackAppendApnName(TInt aError)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneAppendAPNName, aError);
	
	} // CSimDispatcher::CallbackAppendApnName

void CSimDispatcher::CallbackGetActiveIccApplicationType(TInt aError, MLtsyDispatchSimGetActiveIccApplicationType::TIccType aIccType)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aIccType active application type
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIccType=%d"), aError, aIccType);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	
	//effectively a static cast from MLtsyDispatchSimGetActiveIccApplicationType::TIccType to
	//a CMmPhoneGsmWcdmaExt::TICCType when the data is unpacked.
	//This is only correct if the two enums match, unfortunately CMmPhoneGsmWcdmaExt::TICCType is 
	//not an exported header file.
	dataPackage.PackData(&aIccType);
	
	iMessageManagerCallback.Complete(EMmTsySimGetICCType, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackGetIccType

void CSimDispatcher::CallbackSetIccMessageWaitingIndicators(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimSetIccMessageWaitingIndicatorsComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSetIccMessageWaitingIndicators, aError);
	
	} // CSimDispatcher::CallbackSetIccMessageWaitingIndicators

void CSimDispatcher::CallbackSetApnControlListServiceStatus(TInt aError)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSetAPNControlListServiceStatus, aError);
	
	} // CSimDispatcher::CallbackSetApnControlListServiceStatus

void CSimDispatcher::CallbackGetApnName(TInt aError,
			const RMobilePhone::TAPNEntryV3& aEntry)
/**
 * Callback function to be used by the request to complete.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aEntry The Access Point Name (APN) entry.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TAPNEntryV3*>(&aEntry));
	
	iMessageManagerCallback.Complete(EMobilePhoneGetAPNname, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackGetApnName

void CSimDispatcher::CallbackGetServiceTable(
				TInt aError,
				const RMobilePhone::TMobilePhoneServiceTableV1& aServiceTable)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * RMobilePhone::GetServiceTable back to the Common TSY.
 * 
 * @param aServiceTable The service table data to return back to Common TSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Package up data to return to Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(
		const_cast<RMobilePhone::TMobilePhoneServiceTableV1*>(&aServiceTable));

	// Now complete the actual get request
	iMessageManagerCallback.Complete(EMobilePhoneGetServiceTable, &dataPackage, aError);	
	} // CSimDispatcher::CallbackGetServiceTable

void CSimDispatcher::CallbackGetIccMessageWaitingIndicators(
		TInt aError, const RMobilePhone::TMobilePhoneMessageWaitingV1& aMessageWaitingIndicator)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * RMobilePhone::GetIccMessageWaitingIndicators back to the Common TSY.
 * 
 * @param aError The error code to complete back.
 * @param aMessageWaitingIndicator The message waiting indicators to return.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Package up data to return to Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(
			const_cast<RMobilePhone::TMobilePhoneMessageWaitingV1*>
											(&aMessageWaitingIndicator));

	// Now complete the actual get request
	iMessageManagerCallback.Complete(
			EMobilePhoneGetIccMessageWaitingIndicators, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackGetIccMessageWaitingIndicators

void CSimDispatcher::CallbackNotifySimCardStatus(TInt aError,
		RMmCustomAPI::TSIMCardStatus aSimCardStatus)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSimNotifySimCardStatusInd().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSimCardStatus whether the sim card is enabled/disabled etc.
 * @see RMmCustomAPI::NotifySimCardStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aSimCardStatus);
	
	iMessageManagerCallback.Complete(ECustomNotifySimCardStatusIPC, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackNotifySimCardStatus

void CSimDispatcher::CallbackSimLockActivate(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimSimLockActivateComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	iMessageManagerCallback.Complete(ECustomSimLockActivateIPC, aError);
	
	} // CSimDispatcher::CallbackSimLockActivate

void CSimDispatcher::CallbackSimLockDeActivate(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimSimLockDeActivateComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	iMessageManagerCallback.Complete(ECustomSimLockDeActivateIPC, aError);
	
	} // CSimDispatcher::CallbackSimLockDeActivate

void CSimDispatcher::CallbackGetAnswerToReset(TInt aError, const TDesC8& aAnswerToReset)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimGetAnswerToResetComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aAnswerToReset The answer to reset.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAnswerToReset=%S"), aError, &aAnswerToReset);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&const_cast<TDesC8&>(aAnswerToReset));

	iMessageManagerCallback.Complete(ECustomGetATRIPC, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackGetAnswerToReset

void CSimDispatcher::CallbackGetSimCardReaderStatus(TInt aError, TUint8 aSimCardReaderStatus)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimGetSimCardReaderStatusComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSimCardReaderStatus The sim card reader's status.
 * 							   Bits: 
 *								1-3  Identity of card reader,
 *								4    0=Card reader is not removable,
 *								     1=Card reader is removable,
 *								5    0=Card reader is not present,
 * 									 1=Card reader is present,
 *								6    0=Card reader present is not ID-1 size,
 *									 1=Card reader present is ID-1 size,
 *								7    0=No card present,
 *									 1=Card is present in reader,
 *								8    0=No card powered,
 *									 1=Card in reader is powered.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSimCardReaderStatus=%d"), aError, aSimCardReaderStatus);

	TBuf8<1> status;
	status.Append(aSimCardReaderStatus);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&status);
	
	iMessageManagerCallback.Complete(ECustomGetSimCardReaderStatusIPC, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackGetSimCardReaderStatus

void CSimDispatcher::CallbackGetSimAuthenticationData(
		TInt aError, const TDesC8& aSignedResponse, const TDesC8& aCipheringKey)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationDataComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSignedResponse The signed response. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * @param aCipheringKey The ciphering key. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSignedResponse=%S, aCipheringKey=%S"), aError, &aSignedResponse, &aCipheringKey);
	
	RMmCustomAPI::TSimAuthenticationEapSim authenticationEapSim;
	TInt error = aError;
	if ( error == KErrNone)
		{
		if (RMmCustomAPI::KMaxParametersLength < aSignedResponse.Length() 
			|| RMmCustomAPI::KMaxParametersLength < aCipheringKey.Length())
			{
			error = KErrCorrupt;
			}
		else
			{
			authenticationEapSim.iSRES.Copy(aSignedResponse);
			authenticationEapSim.iKC.Copy(aCipheringKey);
			}
		}
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&authenticationEapSim);
	
	iMessageManagerCallback.Complete(ECustomGetSimAuthenticationDataIPC, &dataPackage, error);
	
	} // CSimDispatcher::CallbackGetSimAuthenticationData

void CSimDispatcher::CallbackGetSimAuthenticationData(
		TInt aError, const TDesC8& aResponse, const TDesC8& aCipheringKey, const TDesC8& aIntegrityKey, const TDesC8& aAUTS)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationDataComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponse The response. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * @param aCipheringKey The ciphering key. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * @param aIntegrityKey The integrity key. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * @param aAUTS The AUTS value. A value generated by the peer upon experiencing a synchronization failure, 112 bits. 
 * 		  This is needed only in error case. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aResponse=%S, aCipheringKey=%S, aIntegrityKey=%S, aAUTS=%S"), aError, &aResponse, &aCipheringKey, &aIntegrityKey, &aAUTS);

	RMmCustomAPI::TSimAuthenticationEapAka authenticationEapAka;
	TInt error = aError;
	
	if ( error == KErrNone)
		{
		if (RMmCustomAPI::KMaxParametersLength < aResponse.Length() 
			|| RMmCustomAPI::KMaxParametersLength < aCipheringKey.Length() 
			|| RMmCustomAPI::KMaxParametersLength < aIntegrityKey.Length()) 
			{
			error = KErrCorrupt;
			}
		else
			{
			authenticationEapAka.iRES.Copy(aResponse);
			authenticationEapAka.iCK.Copy(aCipheringKey);
			authenticationEapAka.iIK.Copy(aIntegrityKey);
			}
		}
	else
		{
		if (RMmCustomAPI::KMaxParametersLength < aAUTS.Length()) 
			{
			error = KErrCorrupt;
			}
		else
			{
			authenticationEapAka.iAUTS.Copy(aAUTS);
			}
		}
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&authenticationEapAka);
	
	iMessageManagerCallback.Complete(ECustomGetSimAuthenticationDataIPC, &dataPackage, error);
	
	} // CSimDispatcher::CallbackGetSimAuthenticationData

void CSimDispatcher::CallbackPowerSimOff(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSimPowerSimOffComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECustomPowerSimOffIPC, aError);
	
	} // CSimDispatcher::CallbackPowerSimOff

void CSimDispatcher::CallbackPowerSimOn(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSimPowerSimOnComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECustomPowerSimOnIPC, aError);
	
	} // CSimDispatcher::CallbackPowerSimOn

void CSimDispatcher::CallbackReadSimFile(TInt aError, const TDesC8& aResponseBytes)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSimReadSimFileComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponseBytes The bytes read from the SIM.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	//local need so will compile for ARM	
	TDesC8* responseBytesPtr = &const_cast<TDesC8&>(aResponseBytes);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&responseBytesPtr);
	
	iMessageManagerCallback.Complete(ECustomReadSimFileIPC, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackReadSimFile

void CSimDispatcher::CallbackSendApduRequest(
		TInt aError, TUint8 aServiceType, TUint8 aCardReaderNumber, TUint8 aApplicationType, const TDesC8& aResponseData)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSimSendApduRequestComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aServiceType The value for the service type of the request to be returned to the client.
 * @param aCardReaderNumber The value for the card reader number of the request to be returned to the client.
 * @param aApplicationType The value for the application type of the request to be returned to the client.
 * @param aResponseData The transparent response data conveyed from the baseband to be returned to the client.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aServiceType=%d,aCardReaderNumber=%d,aApplicationType=%d"), aError, aServiceType, aCardReaderNumber, aApplicationType);

	TBuf8<KApduInfoLength> info;
	info.Append(aServiceType);
	info.Append(aCardReaderNumber);
	info.Append(aApplicationType);

	TPtrC8 infoPtr(info);
	TPtrC8 responsePtr(aResponseData);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&infoPtr,&responsePtr);
	
	iMessageManagerCallback.Complete(ECustomSendAPDUReqIPC, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackSendApduRequest

void CSimDispatcher::CallbackSendApduRequestV2(TInt aError, const TDesC8& aResponseData)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSimSendApduRequestV2Comp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponseData The transparent response data conveyed from the baseband to be returned to the client.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&const_cast<TDesC8&>(aResponseData));
	
	iMessageManagerCallback.Complete(ECustomSendAPDUReqV2IPC, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackSendApduRequestV2

void CSimDispatcher::CallbackSimWarmReset(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSimSimWarmResetComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECustomSimWarmResetIPC, aError);
	
	} // CSimDispatcher::CallbackSimWarmReset

void CSimDispatcher::CallbackSetSimMessageStatusRead(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSimSetMessageStatusReadComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECustomSetSimMessageStatusReadIPC, aError);
	
	} // CSimDispatcher::CallbackSetSimMessageStatusRead

void CSimDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
/**
 * Part of the MDispatcherCallback interface. Used to complete requests handled
 * synchronously by the Licensee LTSY asynchronously back to the Common TSY.
 *  
 * @param aIpcDataPackage Package encapsulating the request.
 * 
 * @see MDispatcherCallback::CallbackSync()
 */
	{
	TSYLOGENTRYEXIT;
	
	switch (aIpcDataPackage.iIpc)
		{
	
	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDISPATCHER_CALLBACKSYNC_1, "WARNING: CSimDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CSimDispatcher::CallbackSync



