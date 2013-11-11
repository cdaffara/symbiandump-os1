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
#include "cphonebookondispatcherTraces.h"
#endif

#include "cphonebookondispatcher.h"

#include <ctsy/ltsy/mltsydispatchphonebookoninterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"

CPhonebookOnDispatcher::CPhonebookOnDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CPhonebookOnDispatcher::CPhonebookOnDispatcher

	  
CPhonebookOnDispatcher::~CPhonebookOnDispatcher()
	{
	} // CPhonebookOnDispatcher::~CPhonebookOnDispatcher


CPhonebookOnDispatcher* CPhonebookOnDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CPhonebookOnDispatcher* self =
		new (ELeave) CPhonebookOnDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CPhonebookOnDispatcher::NewLC


CPhonebookOnDispatcher* CPhonebookOnDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CPhonebookOnDispatcher* self =
		CPhonebookOnDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CPhonebookOnDispatcher::NewL


void CPhonebookOnDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to PhonebookOn functionality
	// from the factory
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId))
		{
		TAny* storeReadInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId,
		       	storeReadInterface);
		iLtsyDispatchPhonebookOnStoreRead =
				static_cast<MLtsyDispatchPhonebookOnStoreRead*>(storeReadInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreRead, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId))
		{
		TAny* storeDeleteAllInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId,
		       	storeDeleteAllInterface);
		iLtsyDispatchPhonebookOnStoreDeleteAll =
				static_cast<MLtsyDispatchPhonebookOnStoreDeleteAll*>(storeDeleteAllInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreDeleteAll, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId))
		{
		TAny* storeReadEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId,
		       	storeReadEntryInterface);
		iLtsyDispatchPhonebookOnStoreReadEntry =
				static_cast<MLtsyDispatchPhonebookOnStoreReadEntry*>(storeReadEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreReadEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId))
		{
		TAny* storeGetInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId,
		       	storeGetInfoInterface);
		iLtsyDispatchPhonebookOnStoreGetInfo =
				static_cast<MLtsyDispatchPhonebookOnStoreGetInfo*>(storeGetInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreGetInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId))
		{
		TAny* storeGetReadStoreSizeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId,
		       	storeGetReadStoreSizeInterface);
		iLtsyDispatchPhonebookOnStoreGetReadStoreSize =
				static_cast<MLtsyDispatchPhonebookOnStoreGetReadStoreSize*>(storeGetReadStoreSizeInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreGetReadStoreSize, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId))
		{
		TAny* storeDeleteEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId,
		       	storeDeleteEntryInterface);
		iLtsyDispatchPhonebookOnStoreDeleteEntry =
				static_cast<MLtsyDispatchPhonebookOnStoreDeleteEntry*>(storeDeleteEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreDeleteEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId))
		{
		TAny* storeWriteEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId,
		       	storeWriteEntryInterface);
		iLtsyDispatchPhonebookOnStoreWriteEntry =
				static_cast<MLtsyDispatchPhonebookOnStoreWriteEntry*>(storeWriteEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreWriteEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId))
		{
		TAny* storeWriteInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId,
		       	storeWriteInterface);
		iLtsyDispatchPhonebookOnStoreWrite =
				static_cast<MLtsyDispatchPhonebookOnStoreWrite*>(storeWriteInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreWrite, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookOnFuncUnitId, MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId))
		{
		TAny* storeGetStoreSizeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId,
		       	storeGetStoreSizeInterface);
		iLtsyDispatchPhonebookOnStoreGetStoreSize =
				static_cast<MLtsyDispatchPhonebookOnStoreGetStoreSize*>(storeGetStoreSizeInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookOnStoreGetStoreSize, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	} // CPhonebookOnDispatcher::ConstructL

void CPhonebookOnDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CPhonebookOnDispatcher::SetDispatcherHolder

TInt CPhonebookOnDispatcher::DispatchStoreReadL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyONStoreReadIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	TInt index = 0; 
	aDataPackage->UnPackData(index); 
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookOnStoreRead)
		{
		ret = iLtsyDispatchPhonebookOnStoreRead->HandleStoreReadReqL(index);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreReadL

TInt CPhonebookOnDispatcher::DispatchStoreDeleteAllL()
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
	if (iLtsyDispatchPhonebookOnStoreDeleteAll)
		{
		ret = iLtsyDispatchPhonebookOnStoreDeleteAll->HandleStoreDeleteAllReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreDeleteAllL

TInt CPhonebookOnDispatcher::DispatchStoreReadEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyONStoreReadEntryIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
		
	TInt index = 0; 
	aDataPackage->UnPackData(index); 
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookOnStoreReadEntry)
		{
		ret = iLtsyDispatchPhonebookOnStoreReadEntry->HandleStoreReadEntryReqL(index);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreReadEntryL

TInt CPhonebookOnDispatcher::DispatchStoreGetInfoL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 * 
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookOnStoreGetInfo)
		{
		ret = iLtsyDispatchPhonebookOnStoreGetInfo->HandleStoreGetInfoReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreGetInfoL

TInt CPhonebookOnDispatcher::DispatchStoreGetReadStoreSizeL()
/**
 * Pass request on to Licensee LTSY to retrieve the number of entries that are stored in the storage.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookOnStoreGetReadStoreSize)
		{
		ret = iLtsyDispatchPhonebookOnStoreGetReadStoreSize->HandleStoreGetReadStoreSizeReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreGetReadStoreSizeL

TInt CPhonebookOnDispatcher::DispatchStoreDeleteEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyONStoreDeleteIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TInt index = 0;
	aDataPackage->UnPackData(index);
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookOnStoreDeleteEntry)
		{
		ret = iLtsyDispatchPhonebookOnStoreDeleteEntry->HandleStoreDeleteEntryReqL(index);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreDeleteEntryL

TInt CPhonebookOnDispatcher::DispatchStoreWriteEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyONStoreWriteEntryIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	RMobileONStore::TMobileONEntryV1* mobileOnEntry = NULL;
	aDataPackage->UnPackData(&mobileOnEntry);
	
	__ASSERT_DEBUG(mobileOnEntry, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookOnStoreWriteEntry)
		{
		ret = iLtsyDispatchPhonebookOnStoreWriteEntry->HandleStoreWriteEntryReqL(*mobileOnEntry);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreWriteEntryL

TInt CPhonebookOnDispatcher::DispatchStoreWriteL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyONStoreWriteIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	RMobileONStore::TMobileONEntryV1Pckg* data = NULL;
	aDataPackage->UnPackData(data);
	RMobileONStore::TMobileONEntryV1& mobileOnEntry = (*data)();
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookOnStoreWrite)
		{
		ret = iLtsyDispatchPhonebookOnStoreWrite->HandleStoreWriteReqL(mobileOnEntry);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreWriteL

TInt CPhonebookOnDispatcher::DispatchStoreGetStoreSizeL()
/**
 * Pass request on to Licensee LTSY to retrieve the number of entries that can be stored in the storage.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookOnStoreGetStoreSize)
		{
		ret = iLtsyDispatchPhonebookOnStoreGetStoreSize->HandleStoreGetStoreSizeReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookOnDispatcher::DispatchStoreGetStoreSizeL


//
// Callback handlers follow
//

void CPhonebookOnDispatcher::CallbackStoreRead(TInt aError, TInt aIndex, const TDesC& aName, const TDesC& aNumber)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndex The index of the entry.
 * @param aName The name stored in the entry. The length of this should not be more than 20.
 * @param aNumber The number stored in the ON entry. The length of this should not be more than 100.
 *
 * @see RMobileONStore::Read()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d"), aError, aIndex);

	// Pack the data to return to the Common TSY
	TONStoreMsg onStoreMsg;

	onStoreMsg.iLocation = aIndex;
	onStoreMsg.iName.Copy(aName);
	onStoreMsg.iTelNumber.Copy(aNumber);
	
	TONStoreMsg* onStoreMsgPtr = &onStoreMsg;
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&onStoreMsgPtr);
	
	iMessageManagerCallback.Complete(EMmTsyONStoreReadIPC, &dataPackage, aError);
	} // CPhonebookOnDispatcher::CallbackStoreRead

void CPhonebookOnDispatcher::CallbackStoreDeleteAll(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteAllComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMobileONStore::DeleteAll()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMmTsyONStoreDeleteAllIPC, aError);
	
	} // CPhonebookOnDispatcher::CallbackStoreDeleteAll

void CPhonebookOnDispatcher::CallbackStoreReadEntry(TInt aError, TInt aIndex, const TDesC& aName, const TDesC& aNumber)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadEntryComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndex The index of the entry.
 * @param aName The name stored in the entry. The length of this should not be more than 20.
 * @param aNumber The number stored in the ON entry. The length of this should not be more than 100.
 *  
 * @see RMobileONStore::Read()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d"), aError, aIndex);

	TONStoreMsg onStoreMsg;
	
	// Pack the data to return to the Common TSY
	onStoreMsg.iLocation = aIndex;
	onStoreMsg.iName.Copy(aName);
	onStoreMsg.iTelNumber.Copy(aNumber);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&onStoreMsg);
	
	iMessageManagerCallback.Complete(EMmTsyONStoreReadEntryIPC, &dataPackage, aError);
	
	} // CPhonebookOnDispatcher::CallbackStoreReadEntry

void CPhonebookOnDispatcher::CallbackStoreGetInfo(
		TInt aError, TInt aNumOfEntries, TInt aUsedEntries, TInt aMaxNameLength, TInt aMaxNumberLength)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetInfoComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNumOfEntries The number of entries.
 * @param aUsedEntries The number of used entries.
 * @param aMaxNameLength The length of a name.
 * @param aMaxNumberLength The length of a number.
 * 
 * @see RMobileONStore::GetInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aNumOfEntries=%d, aUsedEntries=%d, aMaxNameLength=%d, aMaxNumberLength=%d"), aError, aNumOfEntries, aUsedEntries, aMaxNameLength, aMaxNumberLength);

	// Pack the data to return to the Common TSY
	TServiceType serviceType;
	serviceType.iNumOfEntries = aNumOfEntries;
	serviceType.iUsedEntries = aUsedEntries;
	serviceType.iNameLen = aMaxNameLength;
	serviceType.iNumLen = aMaxNumberLength;

	CMmDataPackage dataPackage;
	dataPackage.PackData(&serviceType);
	
	iMessageManagerCallback.Complete(EMmTsyONStoreGetInfoIPC, &dataPackage, aError);
	
	} // CPhonebookOnDispatcher::CallbackStoreGetInfo

void CPhonebookOnDispatcher::CallbackStoreGetReadStoreSize(TInt aError, TInt aSize)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetReadStoreSizeComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSize The number of entries stored in the storage.
 *
 * @see CRetrieveMobilePhoneONList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSize=%d"), aError, aSize);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aSize);
	
	iMessageManagerCallback.Complete(EMmTsyONStoreReadSizeIPC, &dataPackage, aError);
	
	} // CPhonebookOnDispatcher::CallbackStoreGetReadStoreSize

void CPhonebookOnDispatcher::CallbackStoreDeleteEntry(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteEntryComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMobileONStore::Delete()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMmTsyONStoreDeleteIPC, aError);
	
	} // CPhonebookOnDispatcher::CallbackStoreDeleteEntry

void CPhonebookOnDispatcher::CallbackStoreWriteEntry(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteEntryComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMobileONStore::StoreAllL()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMmTsyONStoreWriteEntryIPC, aError);
	
	} // CPhonebookOnDispatcher::CallbackStoreWriteEntry

void CPhonebookOnDispatcher::CallbackStoreWrite(TInt aError, TInt aLocation)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aLocation The location where the ON entry was stored.
 * 
 * @see RMobileONStore::Write()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aLocation=%d"), aError, aLocation);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aLocation);
	
	iMessageManagerCallback.Complete(EMmTsyONStoreWriteIPC, &dataPackage, aError);
	
	} // CPhonebookOnDispatcher::CallbackStoreWrite

void CPhonebookOnDispatcher::CallbackStoreGetStoreSize(TInt aError, TInt aSize)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetStoreSizeComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSize The number of entries that can be stored in the storage.
 *
 * @see RMobileONStore::StoreAllL()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSize=%d"), aError, aSize);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&aSize);
	
	iMessageManagerCallback.Complete(EMmTsyONStoreWriteSizeIPC, &dataPackage, aError);
	
	} // CPhonebookOnDispatcher::CallbackStoreGetStoreSize

void CPhonebookOnDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
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
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEBOOKONDISPATCHER_CALLBACKSYNC_1, "WARNING: CPhonebookOnDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CPhonebookOnDispatcher::CallbackSync



