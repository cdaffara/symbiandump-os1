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
#include "ccellbroadcastdispatcherTraces.h"
#endif

#include "ccellbroadcastdispatcher.h"

#include <ctsy/ltsy/mltsydispatchcellbroadcastinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>


#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"



CCellBroadcastDispatcher::CCellBroadcastDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CCellBroadcastDispatcher::CCellBroadcastDispatcher

	  
CCellBroadcastDispatcher::~CCellBroadcastDispatcher()
	{
	if(iCbsMsg)
		{
		iCbsMsg->ResetAndDestroy();
		delete iCbsMsg;
		}
	} // CCellBroadcastDispatcher::~CCellBroadcastDispatcher


CCellBroadcastDispatcher* CCellBroadcastDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CCellBroadcastDispatcher* self =
		new (ELeave) CCellBroadcastDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CCellBroadcastDispatcher::NewLC


CCellBroadcastDispatcher* CCellBroadcastDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CCellBroadcastDispatcher* self =
		CCellBroadcastDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CCellBroadcastDispatcher::NewL


void CCellBroadcastDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to CellBroadcast functionality
	// from the factory
	
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCellBroadcastFuncUnitId, MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId))
		{
		TAny* setBroadcastFilterSettingInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId,
		       	setBroadcastFilterSettingInterface);
		iLtsyDispatchCellBroadcastSetBroadcastFilterSetting =
				static_cast<MLtsyDispatchCellBroadcastSetBroadcastFilterSetting*>(setBroadcastFilterSettingInterface);
        __ASSERT_DEBUG(iLtsyDispatchCellBroadcastSetBroadcastFilterSetting, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCellBroadcastFuncUnitId, MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId))
		{
		TAny* activateBroadcastReceiveMessageInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId,
		       	activateBroadcastReceiveMessageInterface);
		iLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage =
				static_cast<MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage*>(activateBroadcastReceiveMessageInterface);
        __ASSERT_DEBUG(iLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCellBroadcastFuncUnitId, MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId))
		{
		TAny* receiveMessageCancelInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId,
		       	receiveMessageCancelInterface);
		iLtsyDispatchCellBroadcastReceiveMessageCancel =
				static_cast<MLtsyDispatchCellBroadcastReceiveMessageCancel*>(receiveMessageCancelInterface);
        __ASSERT_DEBUG(iLtsyDispatchCellBroadcastReceiveMessageCancel, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
  		
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCellBroadcastFuncUnitId, MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId))
		{
		TAny* startSimCbTopicBrowsingInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId,
		       	startSimCbTopicBrowsingInterface);
		iLtsyDispatchCellBroadcastStartSimCbTopicBrowsing =
				static_cast<MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing*>(startSimCbTopicBrowsingInterface);
        __ASSERT_DEBUG(iLtsyDispatchCellBroadcastStartSimCbTopicBrowsing, CtsyDispatcherPanic(EInvalidNullPtr));
  		}  		
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCellBroadcastFuncUnitId, MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId))
		{
		TAny* deleteSimCbTopicInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId,
		       	deleteSimCbTopicInterface);
		iLtsyDispatchCellBroadcastDeleteSimCbTopic =
				static_cast<MLtsyDispatchCellBroadcastDeleteSimCbTopic*>(deleteSimCbTopicInterface);
        __ASSERT_DEBUG(iLtsyDispatchCellBroadcastDeleteSimCbTopic, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
  		
	iCbsMsg = new( ELeave ) CArrayPtrFlat< TWcdmaCbsMsg >( 10 );  		  		
	
	} // CCellBroadcastDispatcher::ConstructL

void CCellBroadcastDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CCellBroadcastDispatcher::SetDispatcherHolder

TInt CCellBroadcastDispatcher::DispatchSetBroadcastFilterSettingL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileBroadcastMessagingSetFilterSetting
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
	if (iLtsyDispatchCellBroadcastSetBroadcastFilterSetting)
		{
		TCbsCbmiAndLangAndFilter* cbsFilter = NULL;
		aDataPackage->UnPackData(&cbsFilter);
			
		__ASSERT_DEBUG(cbsFilter, CtsyDispatcherPanic(EInvalidNullPtr));
			
		ret = iLtsyDispatchCellBroadcastSetBroadcastFilterSetting->HandleSetBroadcastFilterSettingReqL(cbsFilter->iSetting);
		}	

	return TSYLOGSETEXITERR(ret);
	} // CCellBroadcastDispatcher::DispatchSetBroadcastFilterSettingL

TInt CCellBroadcastDispatcher::DispatchActivateBroadcastReceiveMessageL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileBroadcastMessagingReceiveMessage
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
	if (iLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage)
		{
		TCbsCbmiAndLangAndFilter* cbsFilter = NULL;
		aDataPackage->UnPackData(&cbsFilter);
			
		__ASSERT_DEBUG(cbsFilter, CtsyDispatcherPanic(EInvalidNullPtr));			
			
		ret = iLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage->HandleActivateBroadcastReceiveMessageReqL(cbsFilter->iSetting);
		}
		

	return TSYLOGSETEXITERR(ret);
	} // CCellBroadcastDispatcher::DispatchActivateBroadcastReceiveMessageL

TInt CCellBroadcastDispatcher::DispatchReceiveMessageCancelL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileBroadcastMessagingReceiveMessageCancel
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
	if (iLtsyDispatchCellBroadcastReceiveMessageCancel)
		{
		TCbsCbmiAndLangAndFilter* cbsFilter = NULL;
		aDataPackage->UnPackData(&cbsFilter);
			
		__ASSERT_DEBUG(cbsFilter, CtsyDispatcherPanic(EInvalidNullPtr));
			
		ret = iLtsyDispatchCellBroadcastReceiveMessageCancel->HandleReceiveMessageCancelReqL(cbsFilter->iSetting);
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CCellBroadcastDispatcher::DispatchReceiveMessageCancelL

TInt CCellBroadcastDispatcher::DispatchStartSimCbTopicBrowsingL()
/**
 * Pass ECustomStartSimCbTopicBrowsingIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCellBroadcastStartSimCbTopicBrowsing)
		{						
		ret = iLtsyDispatchCellBroadcastStartSimCbTopicBrowsing->HandleStartSimCbTopicBrowsingReqL();			
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CCellBroadcastDispatcher::DispatchReceiveMessageCancelL
	
TInt CCellBroadcastDispatcher::DispatchDeleteSimCbTopicL(const CMmDataPackage* aDataPackage)
/**
 * Pass ECustomDeleteSimCbTopicIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));	

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCellBroadcastDeleteSimCbTopic)
		{
		TUint expIndex;
		TBool deleteFlag;
		aDataPackage->UnPackData(expIndex, deleteFlag);						
		ret = iLtsyDispatchCellBroadcastDeleteSimCbTopic->HandleDeleteSimCbTopicReqL(expIndex, deleteFlag);
		}
		

	return TSYLOGSETEXITERR(ret);
	} // CCellBroadcastDispatcher::DispatchActivateBroadcastReceiveMessageL
	

//
// Callback handlers follow
//



void CCellBroadcastDispatcher::CallbackGsmBroadcastNotifyMessageReceived(TInt aError, const TDesC8& aCbsMsg)		
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCellBroadcastGsmBroadcastNotifyMessageReceivedInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCbsMsg CB message.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	__ASSERT_DEBUG( (aCbsMsg.Length() < RMobileBroadcastMessaging::KBroadcastPageSize) , CtsyDispatcherPanic(EBadLength));	

	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	TGsmCbsMsg gsmCbsMsg;
	gsmCbsMsg.iCbsMsg.Copy(aCbsMsg.Left(RMobileBroadcastMessaging::KBroadcastPageSize));
	data.PackData(&gsmCbsMsg);
		
	iMessageManagerCallback.Complete(EMmTsyGsmBroadcastNotifyMessageReceived, &data, aError);
	
	} // CCellBroadcastDispatcher::CallbackGsmBroadcastNotifyMessageReceived

void CCellBroadcastDispatcher::CallbackWcdmaBroadcastMessageReceived(TInt aError, const TDesC8& aWcdmaCbsData, 
     const DispatcherCellBroadcast::TWcdmaCbsMsgBase& aWcdmaCbsMsgBase, TBool aMoreToCome)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCellBroadcastWcdmaBroadcastMessageReceivedInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aWcdmaCbsData Broadcast message WCDMA.
 * @param aWcdmaCbsMsgBase base wcdma cbs data structure.
 * @param aIsLast Is it the last WCDMA message to pass to CTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMoreToCome=%d"), aError, aMoreToCome);
	if(aError != KErrNone)
		{
    	__ASSERT_DEBUG(aMoreToCome==EFalse, CtsyDispatcherPanic(EInvalidParameter));			
		}
	
	//A TRAP here is expensive, do a non overloaded new operator and check for NULL.
	TWcdmaCbsMsg* wcdmaCbsMsgPtr = new TWcdmaCbsMsg();
	if (!wcdmaCbsMsgPtr)
		{
		// not proceeding
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EInvalidNullPtr));
		return;	
		}
	// allocation ok:
	__ASSERT_DEBUG( (aWcdmaCbsData.Length() < RMobileBroadcastMessaging::KBroadcastPageSize) , CtsyDispatcherPanic(EBadLength));		
	wcdmaCbsMsgPtr->iWcdmaCbsData.Copy(aWcdmaCbsData);
	wcdmaCbsMsgPtr->iSbNumber = 0; // not used
	wcdmaCbsMsgPtr->iNumberOfPages = aWcdmaCbsMsgBase.iNumberOfPages;
	wcdmaCbsMsgPtr->iMessageType = aWcdmaCbsMsgBase.iMessageType;
	wcdmaCbsMsgPtr->iMessageId = aWcdmaCbsMsgBase.iMessageId;
	wcdmaCbsMsgPtr->iSerialNum = aWcdmaCbsMsgBase.iSerialNum;
	wcdmaCbsMsgPtr->iDCS = aWcdmaCbsMsgBase.iDCS;
	wcdmaCbsMsgPtr->iInfoLength = aWcdmaCbsMsgBase.iInfoLength;
	
	TRAPD(err,iCbsMsg->AppendL(wcdmaCbsMsgPtr));
	if(err != KErrNone)
		{
		delete wcdmaCbsMsgPtr;
		wcdmaCbsMsgPtr = NULL;
		}
	
	if (!aMoreToCome)
		{
		TUint8 wcdmaPageNumber = (TUint8)iCbsMsg->Count();
		// Pack the data to return to the Common TSY
		CMmDataPackage dataPackage;
		dataPackage.PackData(&iCbsMsg, &wcdmaPageNumber);
		
		iMessageManagerCallback.Complete(EMmTsyWcdmaBroadcastNotifyMessageReceived, &dataPackage, aError);

		// handle case, when error on completion:
		if(aError != KErrNone)
			{
			iCbsMsg->ResetAndDestroy();	
			}
		else
			{
			iCbsMsg->Reset();				
			}												
		}
					
	} // CCellBroadcastDispatcher::CallbackWcdmaBroadcastMessageReceived	
	
void CCellBroadcastDispatcher::CallbackStartSimCbTopicBrowsing(TInt aError,
		const CArrayFixFlat<RMmCustomAPI::TSimCbTopic>& aSimTopicArray )
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCellBroadcastStartSimCbTopicBrowsingComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSimTopicArray array of TSimCbTopic.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSimTopicArray.Count()=%d"), aError, aSimTopicArray.Count());

	CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* simTopicArrayPtr = &const_cast<CArrayFixFlat<RMmCustomAPI::TSimCbTopic>&>(aSimTopicArray);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&simTopicArrayPtr);			   
		
	iMessageManagerCallback.Complete(ECustomStartSimCbTopicBrowsingIPC, &dataPackage, aError);	
	
	} // CCellBroadcastDispatcher::CallbackWcdmaBroadcastMessageReceived

void CCellBroadcastDispatcher::CallbackDeleteSimCbTopic(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCellBroadcastDeleteSimCbTopicComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
		
	iMessageManagerCallback.Complete(ECustomDeleteSimCbTopicIPC, aError);	
	
	} // CCellBroadcastDispatcher::CallbackWcdmaBroadcastMessageReceived

void CCellBroadcastDispatcher::CallbackSetBroadcastFilterSetting(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCellBroadcastSetBroadcastFilterSettingComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobileBroadcastMessagingSetFilterSetting, aError);
	
	} // CCellBroadcastDispatcher::CallbackSetBroadcastFilterSetting

void CCellBroadcastDispatcher::CallbackActivateBroadcastReceiveMessage(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCellBroadcastActivateBroadcastReceiveMessageComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMobileBroadcastMessagingReceiveMessage, aError);
	
	} // CCellBroadcastDispatcher::CallbackActivateBroadcastReceiveMessage

void CCellBroadcastDispatcher::CallbackReceiveMessageCancel(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCellBroadcastReceiveMessageCancelComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMobileBroadcastMessagingReceiveMessageCancel, aError);
	
	} // CCellBroadcastDispatcher::CallbackReceiveMessageCancel

void CCellBroadcastDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
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
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCELLBROADCASTDISPATCHER_CALLBACKSYNC_1, "WARNING: CCellBroadcastDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CCellBroadcastDispatcher::CallbackSync



