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

#ifndef _TE_MMONSTORE_H_
#define _TE_MMONSTORE_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"

class CTestOnStore : public CEtelMMTestStepBase
	{
public:

	TInt iTestCount;
	CTestOnStore() ;
	~CTestOnStore(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidateONInfo(const RMobileONStore::TMobileONStoreInfoV1 &aInfo);
	/*static*/ void ValidateONEntry(const RMobileONStore::TMobileONEntryV1 &aEntry);
	/*static*/ void ValidateONRead(const RMobileONStore::TMobileONEntryV1 &aEntry);
	static void InitONEntry(RMobileONStore::TMobileONEntryV1& aEntry);
	
	class CTestGetONList : public CActive
		{
	public:
		static CTestGetONList* NewLC(RMobilePhoneStore& aStore, CTestOnStore* aTestOnStore);
		~CTestGetONList();
		void Start();
		CTestCanceller* RetrieveCancelPtr();
	protected:
		CTestGetONList(RMobilePhoneStore& aStore, CTestOnStore* aTestOnStore);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt CheckResults();
	protected:
		CRetrieveMobilePhoneONList* iRetrieve;
		CTestCanceller* iCanceller;
		RMobilePhoneStore& iStore;
		CTestOnStore* iTestOnStore;
		};

friend class CTestGetONList;

	};

#endif
