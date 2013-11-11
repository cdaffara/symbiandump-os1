// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalAll 
*/

#ifndef __CPHONEBOOKDISPATCHER_H_
#define __CPHONEBOOKDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>

#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;
class CPhoneBookEntry;
class CPhoneBookBuffer;
class CPhoneBookStoreEntry;
class CStorageInfoData;
class TPhonebookInfoV1;
class DispatcherPhonebook::TPhonebookStoreInfoV1;

class MLtsyDispatchPhonebookStoreReadEntry;
class MLtsyDispatchPhonebookStoreDeleteEntry;
class MLtsyDispatchPhonebookStoreCache;
class MLtsyDispatchPhonebookStoreGetPhonebookInfo;
class MLtsyDispatchPhonebookStoreInitialise;
class MLtsyDispatchPhonebookStoreDeleteAll;
class MLtsyDispatchPhonebookSmsStoreReadAll;
class MLtsyDispatchPhonebookStoreWriteEntry;
class MLtsyDispatchPhonebookGetPhoneStoreInfo;
class MLtsyDispatchPhonebookSmsStoreDeleteEntry;
class MLtsyDispatchPhonebookSmsStoreDeleteAll;
class MLtsyDispatchPhonebookSmsStoreGetInfo;
class MLtsyDispatchPhonebookSmsStoreReadEntry;
class MLtsyDispatchPhonebookSmsStoreWriteEntry;
class MLtsyDispatchPhonebookStoreCacheCancel;



// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to Phonebook related requests to the Licensee LTSY.
 */
class CPhonebookDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CPhonebookDispatcher();
	
	static CPhonebookDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CPhonebookDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchStoreReadEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreDeleteEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreCacheL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreGetInfoL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreInitialiseL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreDeleteAllL(const CMmDataPackage* aDataPackage);
	TInt DispatchSmsStoreReadAllL();
	TInt DispatchStoreWriteEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetPhoneStoreInfoL(const CMmDataPackage* aDataPackage);
	TInt DispatchSmsStoreDeleteEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchSmsStoreDeleteAllL();
	TInt DispatchSmsStoreGetInfoL();
	TInt DispatchSmsStoreReadEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchSmsStoreWriteEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreCacheCancelL(const CMmDataPackage* aDataPackage);
	
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackStoreResetCache(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook);
	void CallbackStoreSetFdnPhonebookInfoInd(TInt aError, TInt aTotalEntries , TInt aMaxTextLength , TInt aMaxNumberLength);
	void CallbackStoreReadEntry(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aPhonebookEntries);
	void CallbackStoreDeleteEntry(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aMaxNumberLength);
	void CallbackStoreCache(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aPhonebookEntries);
	void CallbackStoreGetPhonebookInfo(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aUsedEntries);
	void CallbackStoreInitialise(TInt aError, const DispatcherPhonebook::TPhonebookStoreInfoV1& aPhonebookStoreInfo);
	void CallbackStoreDeleteAll(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aMaxNumberLength);
	void CallbackStoreWriteEntry(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TUint16 aIndex, TInt aMaxNumberLength);
	void CallbackGetPhoneStoreInfo(TInt aError, const RMobilePhoneStore::TMobilePhoneStoreInfoV1& aPhoneStoreInfo);
	void CallbackSmsStoreReadAll(TInt aError, const DispatcherPhonebook::TSmsData& aSmsData, TInt aIndex, TBool aMoreToCome, TBool aReceivedClass2ToBeResent);
	void CallbackSmsStoreDeleteEntry(TInt aError);
	void CallbackSmsStoreDeleteAll(TInt aError);
	void CallbackSmsStoreGetInfo(TInt aError, TInt aTotalEntries, TInt aUsedEntries);
	void CallbackSmsStoreReadEntry(TInt aError, const DispatcherPhonebook::TSmsData& aSmsData);
	void CallbackSmsStoreWriteEntry(TInt aError, TInt aLocation, TBool aReceivedClass2ToBeResent);
	
	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);

private:
	
	static const TInt KPhonebookTypes = DispatcherPhonebook::EUnknown;

	static const TInt KMaxTotalEntries = 255;
	
private:

	CPhonebookDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();

	//helper functions
	CPhoneBookStoreEntry* CreatePhoneBookStoreEntryLC(const CPhoneBookEntry& aPhoneBookEntry) const;
	TInt FillPhoneBookEntry(const CPhoneBookStoreEntry& aPhoneBookStoreEntry, CPhoneBookEntry& aPhoneBookEntry) const;
	void FillCtsyPhoneBookStoreL(const TDesC8& aPhonebookEntries, CArrayPtrSeg<CPhoneBookStoreEntry>& aCtsyPhoneBook);
	const TDesC& PhonebookName(DispatcherPhonebook::TPhonebook aPhonebookType) const;
	DispatcherPhonebook::TPhonebook Phonebook(const TDesC& aPhonebookName) const;
	void PhonebookStoreToStorageInfo(const DispatcherPhonebook::TPhonebookStoreInfoV1& aPhonebookStoreInfo, CStorageInfoData& aStorageInfoData) const;

	void FillSmsStoreListL(const DispatcherPhonebook::TSmsData& aSmsData, TInt aIndex, CArrayPtrFlat< TSmsMsg >& aSmsListArray);
	
private:	// Not owned


	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchPhonebookStoreReadEntry* iLtsyDispatchPhonebookStoreReadEntry;
    MLtsyDispatchPhonebookStoreDeleteEntry* iLtsyDispatchPhonebookStoreDeleteEntry;
    MLtsyDispatchPhonebookStoreCache* iLtsyDispatchPhonebookStoreCache;
    MLtsyDispatchPhonebookStoreGetPhonebookInfo* iLtsyDispatchPhonebookStoreGetPhonebookInfo;
    MLtsyDispatchPhonebookStoreInitialise* iLtsyDispatchPhonebookStoreInitialise;
    MLtsyDispatchPhonebookStoreDeleteAll* iLtsyDispatchPhonebookStoreDeleteAll;
    MLtsyDispatchPhonebookSmsStoreReadAll* iLtsyDispatchPhonebookSmsStoreReadAll;
    MLtsyDispatchPhonebookStoreWriteEntry* iLtsyDispatchPhonebookStoreWriteEntry;
    MLtsyDispatchPhonebookGetPhoneStoreInfo* iLtsyDispatchPhonebookGetPhoneStoreInfo;
    MLtsyDispatchPhonebookSmsStoreDeleteEntry* iLtsyDispatchPhonebookSmsStoreDeleteEntry;
    MLtsyDispatchPhonebookSmsStoreDeleteAll* iLtsyDispatchPhonebookSmsStoreDeleteAll;
    MLtsyDispatchPhonebookSmsStoreGetInfo* iLtsyDispatchPhonebookSmsStoreGetInfo;
    MLtsyDispatchPhonebookSmsStoreReadEntry* iLtsyDispatchPhonebookSmsStoreReadEntry;
    MLtsyDispatchPhonebookSmsStoreWriteEntry* iLtsyDispatchPhonebookSmsStoreWriteEntry;
    MLtsyDispatchPhonebookStoreCacheCancel* iLtsyDispatchPhonebookStoreCacheCancel;

 
	//stored pointers sent from the CTSY between Req Comp calls 
	RPointerArray<CArrayPtrSeg<CPhoneBookStoreEntry> > iPhonebookCachePtrs;
	RPointerArray<CArrayPtrSeg<CPhoneBookStoreEntry> > iPhonebookReadPtrs;
	CArrayPtrFlat< TSmsMsg >* iSmsListArray;
	DispatcherPhonebook::TPhonebook iInitialisePhonebook;	
	}; // class CPhonebookDispatcher

#endif // __CPHONEBOOKDISPATCHER_H_
	
