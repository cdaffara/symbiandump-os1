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

#ifndef __CPHONEBOOKENDISPATCHER_H_
#define __CPHONEBOOKENDISPATCHER_H_

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

class MLtsyDispatchPhonebookEnStoreGetInfo;
class MLtsyDispatchPhonebookEnStoreReadAll;
class MLtsyDispatchPhonebookEnStoreReadEntry;



// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to PhonebookEn related requests to the Licensee LTSY.
 */
class CPhonebookEnDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CPhonebookEnDispatcher();
	
	static CPhonebookEnDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CPhonebookEnDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchStoreGetInfoL();
	TInt DispatchStoreReadAllL();
	TInt DispatchStoreReadEntryL(const CMmDataPackage* aDataPackage);
	
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackStoreGetInfo(TInt aError, TInt aUsedEntries);
	void CallbackStoreReadAll(TInt aError, TInt aIndex, const TDesC& aNumber, TBool aMoreToCome);
	void CallbackStoreReadEntry(TInt aError, TInt aIndex, const TDesC& aNumber);
	
	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);	
	
private:

	CPhonebookEnDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();
	
	void FillEnStoreResponseListL(TInt aIndex, const TDesC& aNumber);
private:	// Not owned
	static const TInt KMaxTotalEntries = 255;
	
	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchPhonebookEnStoreGetInfo* iLtsyDispatchPhonebookEnStoreGetInfo;
    MLtsyDispatchPhonebookEnStoreReadAll* iLtsyDispatchPhonebookEnStoreReadAll;
    MLtsyDispatchPhonebookEnStoreReadEntry* iLtsyDispatchPhonebookEnStoreReadEntry;
    
    CArrayPtrFlat<TENStoreResponse>* iEnEntriesArray;

	}; // class CPhonebookEnDispatcher

#endif // __CPHONEBOOKENDISPATCHER_H_
	
