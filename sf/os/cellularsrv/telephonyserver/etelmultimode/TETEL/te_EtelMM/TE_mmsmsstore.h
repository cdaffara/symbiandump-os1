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

/**@file
   @internalComponent */
#ifndef _TE_MMSMSSTORE_H_
#define _TE_MMSMSSTORE_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"

class CTestSmsStore : public CEtelMMTestStepBase
	{
public:

	TInt iTestCount;
	CTestSmsStore() ;
	~CTestSmsStore(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidateSmsStoreInfo(const RMobilePhoneStore::TMobilePhoneStoreInfoV1 &aSyncInfo);
	/*static*/ void ValidateSmsRead(const RMobileSmsStore::TMobileGsmSmsEntryV1 &aSyncEntry);



	class CTestGetSmsList : public CActive
		{
	public:
		static CTestGetSmsList* NewLC(RMobilePhoneStore& aStore, TBool aBatch,RMobileSmsStore::TMobileSmsEntryExtensionId aEntryType, CTestSmsStore* aTestSmsStore);
		~CTestGetSmsList();
		void Start();
		CTestCanceller* RetrieveCancelPtr();
	protected:
		CTestGetSmsList(RMobilePhoneStore& aStore, TBool aBatch,RMobileSmsStore::TMobileSmsEntryExtensionId aEntryType, CTestSmsStore* aTestSmsStore);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt CheckGsmResults();
		TInt CheckCdmaResults();
	protected:
		CRetrieveMobilePhoneSmsList* iRetrieve;
		CTestCanceller* iCanceller;
		RMobilePhoneStore& iStore;
		TBool iBatch;
		RMobileSmsStore::TMobileSmsEntryExtensionId iEntryType;
		CTestSmsStore* iTestSmsStore;
		};
friend class CTestGetSmsList;

	};

#endif
