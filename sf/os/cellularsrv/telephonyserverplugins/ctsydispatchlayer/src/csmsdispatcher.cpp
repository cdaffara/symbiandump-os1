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
#include "csmsdispatcherTraces.h"
#endif

#include <ctsy/serviceapi/cmmsmsutility.h>
#include "csmsdispatcher.h"

#include <ctsy/ltsy/mltsydispatchsmsinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"

CSmsDispatcher::CSmsDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CSmsDispatcher::CSmsDispatcher

	  
CSmsDispatcher::~CSmsDispatcher()
	{
	if(iLtsyDataList)
		{
		iLtsyDataList->ResetAndDestroy();
		delete iLtsyDataList;	
		}
	} // CSmsDispatcher::~CSmsDispatcher


CSmsDispatcher* CSmsDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSmsDispatcher* self =
		new (ELeave) CSmsDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CSmsDispatcher::NewLC


CSmsDispatcher* CSmsDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSmsDispatcher* self =
		CSmsDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CSmsDispatcher::NewL


void CSmsDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to Sms functionality
	// from the factory
	
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId))
		{
		TAny* sendSatSmsInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId,
		       	sendSatSmsInterface);
		iLtsyDispatchSmsSendSatSms =
				static_cast<MLtsyDispatchSmsSendSatSms*>(sendSatSmsInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsSendSatSms, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId))
		{
		TAny* getSmsStoreInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId,
		       	getSmsStoreInfoInterface);
		iLtsyDispatchSmsGetSmsStoreInfo =
				static_cast<MLtsyDispatchSmsGetSmsStoreInfo*>(getSmsStoreInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsGetSmsStoreInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId))
		{
		TAny* getSmspListInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId,
		       	getSmspListInterface);
		iLtsyDispatchSmsGetSmspList =
				static_cast<MLtsyDispatchSmsGetSmspList*>(getSmspListInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsGetSmspList, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId))
		{
		TAny* nackSmsStoredInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId,
		       	nackSmsStoredInterface);
		iLtsyDispatchSmsNackSmsStored =
				static_cast<MLtsyDispatchSmsNackSmsStored*>(nackSmsStoredInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsNackSmsStored, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId))
		{
		TAny* ackSmsStoredInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId,
		       	ackSmsStoredInterface);
		iLtsyDispatchSmsAckSmsStored =
				static_cast<MLtsyDispatchSmsAckSmsStored*>(ackSmsStoredInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsAckSmsStored, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId))
		{
		TAny* resumeSmsReceptionInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId,
		       	resumeSmsReceptionInterface);
		iLtsyDispatchSmsResumeSmsReception =
				static_cast<MLtsyDispatchSmsResumeSmsReception*>(resumeSmsReceptionInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsResumeSmsReception, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId))
		{
		TAny* sendSmsMessageInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId,
		       	sendSmsMessageInterface);
		iLtsyDispatchSmsSendSmsMessage =
				static_cast<MLtsyDispatchSmsSendSmsMessage*>(sendSmsMessageInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsSendSmsMessage, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId))
		{
		TAny* sendSmsMessageNoFdnCheckInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId,
		       	sendSmsMessageNoFdnCheckInterface);
		iLtsyDispatchSmsSendSmsMessageNoFdnCheck =
				static_cast<MLtsyDispatchSmsSendSmsMessageNoFdnCheck*>(sendSmsMessageNoFdnCheckInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsSendSmsMessageNoFdnCheck, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId))
		{
		TAny* setMoSmsBearerInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId,
		       	setMoSmsBearerInterface);
		iLtsyDispatchSmsSetMoSmsBearer =
				static_cast<MLtsyDispatchSmsSetMoSmsBearer*>(setMoSmsBearerInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsSetMoSmsBearer, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSmsFuncUnitId, MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId))
		{
		TAny* storeSmspListInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId,
		       	storeSmspListInterface);
		iLtsyDispatchSmsStoreSmspListEntry =
				static_cast<MLtsyDispatchSmsStoreSmspListEntry*>(storeSmspListInterface);
        __ASSERT_DEBUG(iLtsyDispatchSmsStoreSmspListEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
  		
	iLtsyDataList = new( ELeave ) CArrayPtrFlat< TSmsParameters >( 10 );  		  		  		
	
	} // CSmsDispatcher::ConstructL

void CSmsDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CSmsDispatcher::SetDispatcherHolder

TInt CSmsDispatcher::DispatchActivateSmsRoutingL()
/**
 * Unpack data related to EMmTsyActivateSmsRouting
 * This is not passed to LTSY but just completed by dispatch layer
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	// Trigger async one shot completer 

	CRequestQueueOneShot::TIpcDataPackage* dataPackage = new (ELeave) CRequestQueueOneShot::TIpcDataPackage;
	dataPackage->iIpc = EMmTsyActivateSmsRouting;
	dataPackage->iResultCode = KErrNone;
	dataPackage->iDataPackage = NULL;
	dataPackage->iDispatcherCallback = this;
	iRequestAsyncOneShot.QueueRequest(*dataPackage);		

	return TSYLOGSETEXITERR(KErrNone);
	} // CSmsDispatcher::DispatchActivateSmsRoutingL


TInt CSmsDispatcher::DispatchSendSatSmsL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsySmsSendSatMessage
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TSendSmsDataAndAttributes* data = NULL;
	aDataPackage->UnPackData(&data);

	__ASSERT_DEBUG(data, CtsyDispatcherPanic(EInvalidNullPtr));
	__ASSERT_DEBUG(data->iAttributes, CtsyDispatcherPanic(EInvalidNullPtr));
	__ASSERT_DEBUG(data->iMsgData, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSmsSendSatSms)
		{
		ret = iLtsyDispatchSmsSendSatSms->HandleSendSatSmsReqL(	*(data->iMsgData),
												(data->iAttributes->iDataFormat),
												(data->iAttributes->iGsmServiceCentre),
												(data->iAttributes->iMore)
												);
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchSendSatSmsL

TInt CSmsDispatcher::DispatchGetSmsStoreInfoL()
/**
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSmsGetSmsStoreInfo)
		{
		ret = iLtsyDispatchSmsGetSmsStoreInfo->HandleGetSmsStoreInfoReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchGetSmsStoreInfoL

TInt CSmsDispatcher::DispatchGetSmspListL()
/**
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSmsGetSmspList)
		{
		ret = iLtsyDispatchSmsGetSmspList->HandleGetSmspListReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchGetSmspListL

TInt CSmsDispatcher::DispatchNackSmsStoredL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileSmsMessagingNackSmsStored
 * and pass request on to Licensee LTSY.
 *
 * @param aDataPackage Contains packed pointer to TDesC8 (TPDU data) and TInt (RP cause)
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

    __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSmsNackSmsStored)
		{
		TDesC8* msgPtr = NULL;
		TInt rpCause;
		aDataPackage->UnPackData(msgPtr, rpCause);

		if(msgPtr)
			{
			ret = iLtsyDispatchSmsNackSmsStored->HandleNackSmsStoredReqL(*msgPtr, rpCause);
			}
		else
			{
			ret = iLtsyDispatchSmsNackSmsStored->HandleNackSmsStoredReqL(rpCause);
			}
		
		}

	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchNackSmsStoredL

TInt CSmsDispatcher::DispatchAckSmsStoredL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileSmsMessagingAckSmsStored
 * and pass request on to Licensee LTSY.
 * 
 * @param aDataPackage contains the TPDU defined for a SMS-DELIVER-REPORT
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSmsAckSmsStored)
		{
		TDesC8* msgPtr = NULL;
		aDataPackage->UnPackData(msgPtr);
		
		if(msgPtr)
			{
			ret = iLtsyDispatchSmsAckSmsStored->HandleAckSmsStoredReqL(*msgPtr);			
			}
		else
			{
			ret = iLtsyDispatchSmsAckSmsStored->HandleAckSmsStoredReqL();			
			}
		}

	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchAckSmsStoredL

TInt CSmsDispatcher::DispatchResumeSmsReceptionL()
/**
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;	

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSmsResumeSmsReception)
		{
		ret = iLtsyDispatchSmsResumeSmsReception->HandleResumeSmsReceptionReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchResumeSmsReceptionL

TInt CSmsDispatcher::DispatchSendSmsMessageL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileSmsMessagingSendMessage
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
	if (iLtsyDispatchSmsSendSmsMessage)
		{						   			  		
		TSendSmsDataAndAttributes* data = NULL;	
		aDataPackage->UnPackData(&data);
				
		__ASSERT_DEBUG(data, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(data->iAttributes, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(data->iMsgData, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchSmsSendSmsMessage->HandleSendSmsMessageReqL( (data->iAttributes->iDestination), 
																		*(data->iMsgData),
																		(data->iAttributes->iDataFormat),
																		(data->iAttributes->iGsmServiceCentre),
																		(data->iAttributes->iMore)
				  														  );
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchSendSmsMessageL

TInt CSmsDispatcher::DispatchSendSmsMessageNoFdnCheckL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileSmsMessagingSendMessageNoFdnCheck
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
	if (iLtsyDispatchSmsSendSmsMessageNoFdnCheck)
		{						   			  		
		TSendSmsDataAndAttributes* data = NULL;	
		aDataPackage->UnPackData(&data);	
		
		__ASSERT_DEBUG(data, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(data->iAttributes, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(data->iMsgData, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchSmsSendSmsMessageNoFdnCheck->HandleSendSmsMessageNoFdnCheckReqL( (data->iAttributes->iDestination), 
																									*(data->iMsgData),
																									(data->iAttributes->iDataFormat),
																									(data->iAttributes->iGsmServiceCentre),
																									(data->iAttributes->iMore)
																								  );
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchSendSmsMessageNoFdnCheckL

TInt CSmsDispatcher::DispatchSetMoSmsBearerL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileSmsMessagingSetMoSmsBearer
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
	if (iLtsyDispatchSmsSendSmsMessageNoFdnCheck)
		{						   			  		
		RMobileSmsMessaging::TMobileSmsBearer* data = NULL;	
		aDataPackage->UnPackData(&data);		
		__ASSERT_DEBUG(data, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchSmsSetMoSmsBearer->HandleSetMoSmsBearerReqL(*data);
		}
	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchSetMoSmsBearerL

TInt CSmsDispatcher::DispatchStoreSmspListEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileSmsMessagingStoreSmspList
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
   __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
   
   if (iLtsyDispatchSmsStoreSmspListEntry)
		{						   			  		
		RMobileSmsMessaging::TMobileSmspEntryV1* data = NULL; 	
		aDataPackage->UnPackData(&data);		

	   __ASSERT_DEBUG(data, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchSmsStoreSmspListEntry->HandleStoreSmspListEntryReqL(*data);
		}
   
	return TSYLOGSETEXITERR(ret);
	} // CSmsDispatcher::DispatchStoreSmspListL


//
// Callback handlers follow
//

void CSmsDispatcher::CallbackActivateSmsRouting(CRequestQueueOneShot::TIpcDataPackage* aDataPackage)
/**
 * Callback function to be used by the request to complete .
 *
 * @param aDataPackage Package containing the IPC, return data and result code to be completed to CTSY
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aDataPackage->iResultCode);
	
	if (!aDataPackage->iCleanupOnly)
		{
		CMmDataPackage dataPackage;
		TUint8 smsRouting = KSmsRoutingActivated; 
		dataPackage.PackData(static_cast<TUint8*>(&smsRouting));
		iMessageManagerCallback.Complete(EMmTsyActivateSmsRouting, &dataPackage, aDataPackage->iResultCode);
		}		
	delete aDataPackage;	
	} // CSmsDispatcher::CallbackActivateSmsRouting


void CSmsDispatcher::CallbackNotifyReceiveSmsMessage(TInt aError, TBool aInd, const TSmsMsg& aSmsMessage)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsNotifyReceiveSmsMessageInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aInd Set routing activity on/off.
 * @param aSmsMessage Used to buffer incoming SMSes to the TSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aInd=%d"), aError, aInd);

	TSmsMsg* smsMessagePtr = &const_cast<TSmsMsg&>(aSmsMessage);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aInd, &smsMessagePtr);
		
	iMessageManagerCallback.Complete(EMobileSmsMessagingReceiveMessage, &dataPackage, aError);
	
	} // CSmsDispatcher::CallbackNotifyReceiveSmsMessage

void CSmsDispatcher::CallbackSendSatSms(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsSendSatSmsComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMmTsySmsSendSatMessage, aError);
	
	} // CSmsDispatcher::CallbackSendSatSms

void CSmsDispatcher::CallbackGetSmsStoreInfo(TInt aError, TInt aTotalEntries, TInt aUsedEntries)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsGetSmsStoreInfoComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aTotalEntries Indicates the total number of entries that may be held in this store.
 * @param aUsedEntries Indicates the current number of entries held in this store.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aTotalEntries=%d, aUsedEntries=%d"), aError, aTotalEntries, aUsedEntries);

	__ASSERT_DEBUG(aTotalEntries >= 0 && aTotalEntries <= 255, CtsyDispatcherPanic(EBadLength));
	
	// Pack the data to return to the Common TSY
	TUint8 tempVar = static_cast<TUint8>(aTotalEntries);
	CMmDataPackage dataPackage;
	dataPackage.PackData(&tempVar, &aUsedEntries);	
	
	iMessageManagerCallback.Complete(EMobileSmsMessagingGetMessageStoreInfo, &dataPackage, aError);
	
	} // CSmsDispatcher::CallbackGetSmsStoreInfo

void CSmsDispatcher::CallbackGetSmspList(TInt aError, const TDesC& aServiceCenterAddress, const TDesC& aDestinationAddress,
              const TDesC& aAlphaTagData, const DispatcherSim::TSmsParameters& aSmsParameters, TBool aMoreToCome)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsGetSmspListComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aServiceCenterAddress Service center address.
 * @param aDestinationAddress Destination address.
 * @param aAlphaTagData Alpha tag data.
 * @param aSmsParameters Sms parametets.
 * @param aMoreToCome Are more elements to come. 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMoreToCome=%d"), aError, aMoreToCome);

	if(aError != KErrNone)
		{
    	__ASSERT_DEBUG(aMoreToCome==EFalse, CtsyDispatcherPanic(EInvalidParameter));			
		}
	
	//A TRAP new (Eleave) here is expensive, do a non overloaded new operator and check for NULL.
	TSmsParameters* smsParamsPtr = new TSmsParameters();
	if (!smsParamsPtr)
		{
		// not proceeding
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EInvalidNullPtr));
		return;	
		}
	// allocation ok:
	
	__ASSERT_DEBUG( (aServiceCenterAddress.Length() < KMaxAddressBufferSize) , CtsyDispatcherPanic(EBadLength));		
	smsParamsPtr->iServiceCenterAddress.Copy(aServiceCenterAddress);

	__ASSERT_DEBUG( (aDestinationAddress.Length() < KMaxAddressBufferSize) , CtsyDispatcherPanic(EBadLength));		
	smsParamsPtr->iDestinationAddress.Copy(aDestinationAddress);
	
	__ASSERT_DEBUG( (aAlphaTagData.Length() < RMobileSmsMessaging::KMaxSmspTextSize) , CtsyDispatcherPanic(EBadLength));		
	smsParamsPtr->iAlphaTagData.Copy(aAlphaTagData);
	
	smsParamsPtr->iLocationNumber = aSmsParameters.iLocationNumber;
	smsParamsPtr->iParameterIndicator = aSmsParameters.iParameterIndicator;
	smsParamsPtr->iProtocolId = aSmsParameters.iProtocolId;
	smsParamsPtr->iDataCodingScheme = aSmsParameters.iDataCodingScheme;
	smsParamsPtr->iValidityPeriod = aSmsParameters.iValidityPeriod;
	smsParamsPtr->iMobileScTON = aSmsParameters.iMobileScTON;
	smsParamsPtr->iMobileScNPI = aSmsParameters.iMobileScNPI;
	smsParamsPtr->iMobileDeTON = aSmsParameters.iMobileDeTON;
	smsParamsPtr->iMobileDeNPI = aSmsParameters.iMobileDeNPI;
	smsParamsPtr->iAlphaTagPresent = aSmsParameters.iAlphaTagPresent;
	
	TRAPD(err,iLtsyDataList->AppendL(smsParamsPtr));
	if(err != KErrNone)
		{
		delete smsParamsPtr;
		smsParamsPtr = NULL;
		}
	
	if (!aMoreToCome)
		{
		// Pack the data to return to the Common TSY
		CMmDataPackage dataPackage;
		dataPackage.PackData(iLtsyDataList);
		
		iMessageManagerCallback.Complete(EMobileSmsMessagingGetSmspListPhase1, &dataPackage, aError);
		
		// handle case, when error on completion:
		if(aError != KErrNone)
			{
			iLtsyDataList->ResetAndDestroy();	
			}
		else
			{
			iLtsyDataList->Reset();				
			}								
		}
				
	} // CSmsDispatcher::CallbackGetSmspList

void CSmsDispatcher::CallbackNackSmsStored(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMobileSmsMessagingNackSmsStored, aError);
	
	} // CSmsDispatcher::CallbackNackSmsStored

void CSmsDispatcher::CallbackAckSmsStored(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMobileSmsMessagingAckSmsStored, aError);
	
	} // CSmsDispatcher::CallbackAckSmsStored

void CSmsDispatcher::CallbackResumeSmsReception(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsResumeSmsReceptionComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobileSmsMessagingResumeSmsReception, aError);	
	} // CSmsDispatcher::CallbackResumeSmsReception

void CSmsDispatcher::CallbackSendSmsMessage(TInt aError,
		TInt aMsgRef, const TDesC8& aSmsSubmitReport)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSmsSendSmsMessageComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aMsgRef the message reference assigned to a sent message.
 * @param aSmsSubmitReport message string, , which is expected to be a buffer with
 *        a maximum size of RMobileSmsMessaging::KGsmTpduSize
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	__ASSERT_DEBUG( (aSmsSubmitReport.Length() < RMobileSmsMessaging::KGsmTpduSize) , CtsyDispatcherPanic(EBadLength));
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;	
	TPtrC8 submitReport(aSmsSubmitReport);			
	
	dataPackage.PackData(&aMsgRef, &submitReport );	
	iMessageManagerCallback.Complete(EMobileSmsMessagingSendMessage, &dataPackage, aError);
	} // CSmsDispatcher::CallbackSendSmsMessage

void CSmsDispatcher::CallbackSendSmsMessageNoFdnCheck(TInt aError,
		TInt aMsgRef, const TDesC8& aSmsSubmitReport)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSmsSendSmsMessageNoFdnCheckComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aMsgRef the message reference assigned to a sent message.
 * @param aSmsSubmitReport message string, , which is expected to be a buffer with
 *        a maximum size of RMobileSmsMessaging::KGsmTpduSize
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	__ASSERT_DEBUG( (aSmsSubmitReport.Length() < RMobileSmsMessaging::KGsmTpduSize) , CtsyDispatcherPanic(EBadLength));
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;	
	TBuf8<RMobileSmsMessaging::KGsmTpduSize> submitReport = aSmsSubmitReport;	
	
	dataPackage.PackData(&aMsgRef, &submitReport  );
	iMessageManagerCallback.Complete(EMobileSmsMessagingSendMessageNoFdnCheck, &dataPackage, aError);		
	} // CSmsDispatcher::CallbackSendSmsMessageNoFdnCheck

void CSmsDispatcher::CallbackSetMoSmsBearer(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsSetMoSmsBearerComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
		
	iMessageManagerCallback.Complete(EMobileSmsMessagingSetMoSmsBearer, aError);
	
	} // CSmsDispatcher::CallbackSetMoSmsBearer

void CSmsDispatcher::CallbackStoreSmspListEntry(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSmsStoreSmspListComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);		
	iMessageManagerCallback.Complete(EMobileSmsMessagingStoreSmspList, aError);
	} // CSmsDispatcher::CallbackStoreSmspList

void CSmsDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
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
	case EMmTsyActivateSmsRouting:
		CallbackActivateSmsRouting(&aIpcDataPackage);
		break;
	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDISPATCHER_CALLBACKSYNC_1, "WARNING: CSmsDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CSmsDispatcher::CallbackSync



