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
// functional unit of the Common TSY Dispatcher.
//



/**
 @file The TEFUnit header file which tests the Phonebook
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYPHONEBOOKFU_H
#define CCTSYPHONEBOOKFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>

#include <ctsy/rmmcustomapi.h>

class CPhoneBookEntry;

class CCTsyPhonebookFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// use-case tests
	void TestUseCase0001L();
	void TestUseCase0002L();
	void TestUseCase0003L();
	void TestUseCase0004L();
	void TestUseCase0005L();
	void TestUseCase0006L();
	void TestUseCase0007L();
	void TestUseCase0008L();
	void TestUseCase0009L();
	void TestUseCase0010L();
	void TestUseCase0011L();
	void TestUseCase0012L();
	void TestUseCase0013L();
	void TestUseCase0014L();
	
	//other unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	void TestUnit0008L();
	void TestUnit0009L();
	void TestUnit0010L();
	void TestUnit0011L();
	void TestUnit0012L();
	
protected:
    virtual void OpenNonCachingPhoneBookL(DispatcherPhonebook::TPhonebook aPhonebook, RMobilePhoneBookStore& aPhoneBookStore, TInt aError);
    virtual void OpenCachingPhoneBookL(DispatcherPhonebook::TPhonebook aPhonebook, RMobilePhoneBookStore& aPhoneBookStore, TInt aError);
    virtual void DoCleanup();
	
private:

	void CreateTlvFromEntryL(const CPhoneBookEntry& aPhoneBookEntry, RBuf8& aBuf);
	void CreateTlvFromPhonebookL(const RPointerArray<CPhoneBookEntry>& aPhonebook, RBuf8& aBuf);

	void FillPhonebookEntryArrayFromTlvL(const TDesC8& aEntry, RPointerArray<CPhoneBookEntry>& aPhoneBook);

	void TestTlvWriteReadL(const CPhoneBookEntry& aPhoneBookEntry);

	void CreateEntriesAndStoreDataL();

	void OpenPhoneBookL(DispatcherPhonebook::TPhonebook aPhonebook, RMobilePhoneBookStore& aPhoneBookStore, RMmCustomAPI& aCustomApi);
	void OpenPhoneBookWithInitialisationL(DispatcherPhonebook::TPhonebook aPhonebook, RMobilePhoneBookStore& aPhoneBookStore, TInt aError);
	void CompleteCacheL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aError, TBool aIsRefresh);
	TBool InitialisationRequired(DispatcherPhonebook::TPhonebook aPhonebook) const;
	TBool CachingPhonebook(DispatcherPhonebook::TPhonebook aPhonebook) const;
	
	void ReadL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aError);
	void ReadL(DispatcherPhonebook::TPhonebook aPhonebook, const RMobilePhoneBookStore& aPhonebookStore, TInt aError);

	void CacheReadL(const RMobilePhoneBookStore& aPhoneBookStore, TInt aIndex, TInt aNumberOfSlots);
	void NonCacheReadL(DispatcherPhonebook::TPhonebook aPhonebook, const RMobilePhoneBookStore& aPhoneBookStore, TInt aIndex, TInt aNumberOfSlots, TInt aError);
	void ExpectNonCacheReadL(DispatcherPhonebook::TPhonebook aPhonebook, const RMobilePhoneBookStore& aPhoneBookStore, TInt aIndex, TInt aNumberOfSlots, TRequestStatus& aRequestStatus, TDes8& aReadData);
	void CompleteNonCacheReadL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndex, TInt aNumberOfSlots, TRequestStatus& aRequestStatus, const TDesC8& aReadData, TInt aError);
	
	void DeleteL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndexToDelete, TInt aError);
	void DeleteAllL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aError);

	void WriteL(DispatcherPhonebook::TPhonebook aPhonebook, const CPhoneBookEntry& aPhoneBookEntry, TInt aError);
	void WriteL(DispatcherPhonebook::TPhonebook aPhonebook, const RMobilePhoneBookStore& aPhonebookStore, const CPhoneBookEntry& aPhoneBookEntry, TInt aError);
	
	void CheckPhonebookDetailsL(DispatcherPhonebook::TPhonebook aPhonebook,const RMobilePhoneBookStore& aPhonebookStore, TInt aEntries, TInt aError);
	
	void GetInfoL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aError);
	void GetInfoFromCtsy(DispatcherPhonebook::TPhonebook aPhonebook, const RMobilePhoneBookStore& aPhonebookStore, TInt aUsedEntries);
	void GetInfoFromLtsyL(DispatcherPhonebook::TPhonebook aPhonebook, const RMobilePhoneBookStore& aPhonebookStore, TInt aUsedEntries, TInt aError);

	void SetFdnInfoL(RMobilePhoneBookStore& aPhoneBookStore, TInt aError);

	void RefreshL(DispatcherPhonebook::TPhonebook aPhonebook);

	TName PhonebookName(DispatcherPhonebook::TPhonebook aPhonebook) const;

	static void PhoneBookEntryArrayCleanup(TAny* aArray);

	void OpenSmsStoreL(RMobileSmsMessaging& aSmsMessaging, RMobileSmsStore& aSmsStore, const TDesC& aSmsStoreName);
	
private:
	
	DispatcherPhonebook::TPhonebookStoreInfoV1 iPhonebookStoreData;

	RPointerArray<CPhoneBookEntry> iEntries;
	
	TBool iInititalised;

	}; // class CCTsyPhonebookFU

#endif // CCTSYPHONEBOOKENFU_H

