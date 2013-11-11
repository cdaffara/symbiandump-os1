// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent 
*/

#ifndef _TE_MMNAMSTORE_H_
#define _TE_MMNAMSTORE_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"

class CTestNamStore : public CEtelMMTestStepBase
	{
public:

	TInt iTestCount;
	CTestNamStore() ;
	~CTestNamStore(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidateNamInfo(const RMobileNamStore::TMobileNamStoreInfoV1 &aInfo);
	/*static*/ void ValidateNamEntry(const RMobileNamStore::TMobileNamEntryV1 &aEntry);
	/*static*/ void ValidateNamEntry(const RMobileNamStore::TMobileNamEntryV4 &aEntry);
	/*static*/ void ValidateNamRead(const RMobileNamStore::TMobileNamEntryV1 &aEntry);
	/*static*/ void ValidateNamRead(const RMobileNamStore::TMobileNamEntryV4 &aEntry);
	static void InitNamEntry(RMobileNamStore::TMobileNamEntryV1& aEntry);
	static void InitNamEntry(RMobileNamStore::TMobileNamEntryV4& aEntry);

class CTestGetNamList : public CActive
		{
	public:
		static CTestGetNamList* NewLC(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore);
		~CTestGetNamList();
		void Start();
		CTestCanceller* RetrieveCancelPtr();
	protected:
		CTestGetNamList(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt CheckResults();
	protected:
		CRetrieveMobilePhoneNamList* iRetrieve;
		CTestCanceller* iCanceller;
		RMobilePhoneStore& iStore;
		CTestNamStore* iTestNamStore;
		};

class CTestGetNamListV4 : public CActive
		{
	public:
		static CTestGetNamListV4* NewLC(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore);
		~CTestGetNamListV4();
		void Start();
		CTestCanceller* RetrieveCancelPtr();
	protected:
		CTestGetNamListV4(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt CheckResults();
	protected:
		CRetrieveMobilePhoneNamList* iRetrieve;
		CTestCanceller* iCanceller;
		RMobilePhoneStore& iStore;
		CTestNamStore* iTestNamStore;
		};

class CTestGetNamListSimultaneous : public CActive
		{
	public:
		static CTestGetNamListSimultaneous* NewLC(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore);
		~CTestGetNamListSimultaneous();
		CTestGetNamListSimultaneous();
		void Start();
	protected:
		CTestGetNamListSimultaneous(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt CheckResults();
	protected:
		CRetrieveMobilePhoneNamList* iRetrieve;
		CTestCanceller* iCanceller;
		RMobilePhoneStore& iStore;
		CTestNamStore* iTestNamStore;
		};

friend class CTestGetNamList;
friend class CTestGetNamListV4;
friend class CTestGetNamListSimultaneous;

	};

#endif
