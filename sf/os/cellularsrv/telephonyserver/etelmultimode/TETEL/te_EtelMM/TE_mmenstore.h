// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_MMENSTORE_H_
#define _TE_MMENSTORE_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"

class CTestEnStore : public CEtelMMTestStepBase
	{
public:
	TInt iTestCount;
	CTestEnStore() ;
	~CTestEnStore(){} ;
	virtual enum TVerdict doTestStepL();

	/*static*/ void ValidateENInfo(const RMobilePhoneStore::TMobilePhoneStoreInfoV1 &aInfo);
	/*static*/ void ValidateENEntry(const RMobileENStore::TMobileENEntryV1 &aEntry);
	/*static*/ void ValidateENRead(const RMobileENStore::TMobileENEntryV1 &aEntry);
	/*static*/ void InitENEntry(RMobileENStore::TMobileENEntryV1& aEntry);
	
class CTestGetENList : public CActive
		{
	public:
		static CTestGetENList* NewLC(RMobilePhoneStore& aStore, CTestEnStore* aTestEnStore);
		~CTestGetENList();
		void Start();
		CTestCanceller* RetrieveCancelPtr();
	protected:
		CTestGetENList(RMobilePhoneStore& aStore, CTestEnStore* aTestEnStore);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt CheckResults();
	protected:
		CRetrieveMobilePhoneENList* iRetrieve;
		CTestCanceller* iCanceller;
		RMobilePhoneStore& iStore;
		CTestEnStore* iTestEnStore;
		};

friend class CTestGetENList;

	};

#endif
