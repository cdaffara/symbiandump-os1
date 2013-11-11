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
#include "cphonebookendispatcherTraces.h"
#endif

#include "cphonebookendispatcher.h"

#include <ctsy/ltsy/mltsydispatchphonebookeninterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>


#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"

CPhonebookEnDispatcher::CPhonebookEnDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CPhonebookEnDispatcher::CPhonebookEnDispatcher

	  
CPhonebookEnDispatcher::~CPhonebookEnDispatcher()
	{
	if (iEnEntriesArray)
		{
		iEnEntriesArray->ResetAndDestroy();
		}
	delete iEnEntriesArray;
	} // CPhonebookEnDispatcher::~CPhonebookEnDispatcher


CPhonebookEnDispatcher* CPhonebookEnDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CPhonebookEnDispatcher* self =
		new (ELeave) CPhonebookEnDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CPhonebookEnDispatcher::NewLC


CPhonebookEnDispatcher* CPhonebookEnDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CPhonebookEnDispatcher* self =
		CPhonebookEnDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CPhonebookEnDispatcher::NewL


void CPhonebookEnDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to PhonebookEn functionality
	// from the factory

	iEnEntriesArray = new (ELeave) CArrayPtrFlat<TENStoreResponse> (1);
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookEnFuncUnitId, MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId))
		{
		TAny* storeGetInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId,
		       	storeGetInfoInterface);
		iLtsyDispatchPhonebookEnStoreGetInfo =
				static_cast<MLtsyDispatchPhonebookEnStoreGetInfo*>(storeGetInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookEnStoreGetInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookEnFuncUnitId, MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId))
		{
		TAny* storeReadAllInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId,
		       	storeReadAllInterface);
		iLtsyDispatchPhonebookEnStoreReadAll =
				static_cast<MLtsyDispatchPhonebookEnStoreReadAll*>(storeReadAllInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookEnStoreReadAll, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookEnFuncUnitId, MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId))
		{
		TAny* storeReadEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId,
		       	storeReadEntryInterface);
		iLtsyDispatchPhonebookEnStoreReadEntry =
				static_cast<MLtsyDispatchPhonebookEnStoreReadEntry*>(storeReadEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookEnStoreReadEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	} // CPhonebookEnDispatcher::ConstructL

void CPhonebookEnDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CPhonebookEnDispatcher::SetDispatcherHolder


void CPhonebookEnDispatcher::FillEnStoreResponseListL(TInt aIndex, const TDesC& aNumber)
/*
 * Append a TENStoreResponse entry to the iEnEntriesArray. 
 * 
 * @param aIndex The index of the entry.
 * @param aNumber The number of the entry.
 * 
 */	
	{
	TENStoreResponse* tempENStoreResponse = new (ELeave) TENStoreResponse();
	CleanupStack::PushL(tempENStoreResponse);
			
	tempENStoreResponse->iECCNumber.Copy(aNumber);
	tempENStoreResponse->iLocation = aIndex;
	iEnEntriesArray->AppendL(tempENStoreResponse); 	//iEnEntriesArray has taken the ownership of the object

	CleanupStack::Pop(tempENStoreResponse);
	}

TInt CPhonebookEnDispatcher::DispatchStoreGetInfoL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookEnStoreGetInfo)
		{
		ret = iLtsyDispatchPhonebookEnStoreGetInfo->HandleStoreGetInfoReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookEnDispatcher::DispatchStoreGetInfoL

TInt CPhonebookEnDispatcher::DispatchStoreReadAllL()
/**
 * Unpack data related to EMmTsyENStoreReadAllPhase1IPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	if (iLtsyDispatchPhonebookEnStoreReadAll)
		{
		ret = iLtsyDispatchPhonebookEnStoreReadAll->HandleStoreReadAllReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookEnDispatcher::DispatchStoreReadAllL

TInt CPhonebookEnDispatcher::DispatchStoreReadEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyENStoreReadIPC
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
	TInt index = 0;
	aDataPackage->UnPackData(index);
	
	if (iLtsyDispatchPhonebookEnStoreReadEntry)
		{
		ret = iLtsyDispatchPhonebookEnStoreReadEntry->HandleStoreReadEntryReqL(index);
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPhonebookEnDispatcher::DispatchStoreReadEntryL


//
// Callback handlers follow
//



void CPhonebookEnDispatcher::CallbackStoreGetInfo(TInt aError, TInt aUsedEntries)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackPhonebookEnStoreGetInfoComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aUsedEntries The number of used entries in the EN storage.
 *
 * @see RMobileENStore::GetInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aUsedEntries=%d"), aError, aUsedEntries);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aUsedEntries);

	iMessageManagerCallback.Complete(EMmTsyENStoreGetInfoIPC, &dataPackage, aError);
	
	} // CPhonebookEnDispatcher::CallbackStoreGetInfo

void CPhonebookEnDispatcher::CallbackStoreReadAll(TInt aError, TInt aIndex, const TDesC& aNumber, TBool aMoreToCome)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadAllComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndex The index of the entry.
 * @param aNumber The number stored in the EN entry. The length of this should not be more than 6.
 * @param aMoreToCome ETrue if there are more entries to be read, EFalse otherwise.
 *
 * @see CRetrieveMobilePhoneENList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d, aMoreToCome=%d"), aError, aIndex, aMoreToCome);
	
	TInt errorCode = aError;
	CMmDataPackage dataPackage;
	
	if (errorCode == KErrNone)
		{
		if ( aIndex < 0 || aIndex > KMaxTotalEntries ) 
			{
			errorCode = KErrCorrupt;
			}
		else
			{
			TRAP(errorCode, FillEnStoreResponseListL(aIndex, aNumber));
			if (errorCode == KErrNone)
				{
				dataPackage.PackData(&iEnEntriesArray);
				}
			}
		}
	
	if (!aMoreToCome || errorCode != KErrNone) 
		{
		iMessageManagerCallback.Complete(EMmTsyENStoreReadAllPhase1IPC, &dataPackage, errorCode);
		iEnEntriesArray->ResetAndDestroy(); 
		}
	
	} // CPhonebookEnDispatcher::CallbackStoreReadAll

void CPhonebookEnDispatcher::CallbackStoreReadEntry(TInt aError, TInt aIndex, const TDesC& aNumber)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadEntryComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndex The index of the entry.
 * @param aNumber The number stored in the EN entry. The length of this should not be more than 6.
 *
 * @see RMobileENStore::Read()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d"), aError, aIndex);
	
	TENStoreResponse storeResponse;
	storeResponse.iLocation = aIndex;
	storeResponse.iECCNumber.Copy(aNumber);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&storeResponse);
	
	if(aIndex >= KMaxTotalEntries)
		{
		iMessageManagerCallback.Complete(EMmTsyENStoreReadIPC, &dataPackage, KErrCorrupt);
		}
	else
		{
		iMessageManagerCallback.Complete(EMmTsyENStoreReadIPC, &dataPackage, aError);
		}
	} // CPhonebookEnDispatcher::CallbackStoreReadEntry

void CPhonebookEnDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
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
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEBOOKENDISPATCHER_CALLBACKSYNC_1, "WARNING: CPhonebookEnDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CPhonebookEnDispatcher::CallbackSync



