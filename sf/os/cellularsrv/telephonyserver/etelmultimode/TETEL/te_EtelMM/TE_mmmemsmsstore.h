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

#ifndef _TE_MMMEMSMSSTORE_H_
#define _TE_MMMEMSMSSTORE_H_

#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"
#include "TE_mmsmsstore.h"

class CTestMemSmsStore : public CTestSmsStore
	{
public:

	TInt iTestCount;
	CTestMemSmsStore() ;
	~CTestMemSmsStore(){} ;
	virtual enum TVerdict doTestStepL();

private:

	class CMemTestGetSmsList : public CTestSmsStore::CTestGetSmsList
		{
	public:
		static CMemTestGetSmsList* NewLC(RMobilePhoneStore& aStore, TBool aBatch, CTestMemSmsStore* aTestMemSmsStore);
		void Start();
		CTestMemSmsStore* iTestMemSmsStore;
	protected:
		CMemTestGetSmsList(RMobilePhoneStore& aStore, TBool aBatch, CTestMemSmsStore* aTestMemSmsStore);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};

friend class CMemTestGetSmsList;

	};

#endif
