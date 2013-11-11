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

#ifndef __CPHONEBOOKONDISPATCHER_H_
#define __CPHONEBOOKONDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>

#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchPhonebookOnStoreRead;
class MLtsyDispatchPhonebookOnStoreDeleteAll;
class MLtsyDispatchPhonebookOnStoreReadEntry;
class MLtsyDispatchPhonebookOnStoreGetInfo;
class MLtsyDispatchPhonebookOnStoreGetReadStoreSize;
class MLtsyDispatchPhonebookOnStoreDeleteEntry;
class MLtsyDispatchPhonebookOnStoreWriteEntry;
class MLtsyDispatchPhonebookOnStoreWrite;
class MLtsyDispatchPhonebookOnStoreGetStoreSize;



// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to PhonebookOn related requests to the Licensee LTSY.
 */
class CPhonebookOnDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CPhonebookOnDispatcher();
	
	static CPhonebookOnDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CPhonebookOnDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchStoreReadL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreDeleteAllL();
	TInt DispatchStoreReadEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreGetInfoL();
	TInt DispatchStoreGetReadStoreSizeL();
	TInt DispatchStoreDeleteEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreWriteEntryL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreWriteL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreGetStoreSizeL();
	
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackStoreReadAll(TInt aError);
	void CallbackStoreRead(TInt aError, TInt aIndex, const TDesC& aName, const TDesC& aNumber);
	void CallbackStoreDeleteAll(TInt aError);
	void CallbackStoreReadEntry(TInt aError, TInt aIndex, const TDesC& aName, const TDesC& aNumber);
	void CallbackStoreGetInfo(TInt aError, TInt aNumOfEntries, TInt aUsedEntries, TInt aNameLen, TInt aNumLen);
	void CallbackStoreGetReadStoreSize(TInt aError, TInt aSize);
	void CallbackStoreDeleteEntry(TInt aError);
	void CallbackStoreWriteEntry(TInt aError);
	void CallbackStoreWrite(TInt aError, TInt aLocation);
	void CallbackStoreGetStoreSize(TInt aError, TInt aSize);
	
	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);
	
	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);
	
private:

	CPhonebookOnDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();
	
private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchPhonebookOnStoreRead* iLtsyDispatchPhonebookOnStoreRead;
    MLtsyDispatchPhonebookOnStoreDeleteAll* iLtsyDispatchPhonebookOnStoreDeleteAll;
    MLtsyDispatchPhonebookOnStoreReadEntry* iLtsyDispatchPhonebookOnStoreReadEntry;
    MLtsyDispatchPhonebookOnStoreGetInfo* iLtsyDispatchPhonebookOnStoreGetInfo;
    MLtsyDispatchPhonebookOnStoreGetReadStoreSize* iLtsyDispatchPhonebookOnStoreGetReadStoreSize;
    MLtsyDispatchPhonebookOnStoreDeleteEntry* iLtsyDispatchPhonebookOnStoreDeleteEntry;
    MLtsyDispatchPhonebookOnStoreWriteEntry* iLtsyDispatchPhonebookOnStoreWriteEntry;
    MLtsyDispatchPhonebookOnStoreWrite* iLtsyDispatchPhonebookOnStoreWrite;
    MLtsyDispatchPhonebookOnStoreGetStoreSize* iLtsyDispatchPhonebookOnStoreGetStoreSize;
    
	}; // class CPhonebookOnDispatcher

#endif // __CPHONEBOOKONDISPATCHER_H_
	
