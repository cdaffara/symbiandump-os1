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

#ifndef _TE_MMMEMONSTORE_H_
#define _TE_MMMEMONSTORE_H_

#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"
#include "TE_mmonstore.h"

class CTestMemOnStore : public CTestOnStore
	{
public:

	TInt iTestCount;
	CTestMemOnStore() ;
	~CTestMemOnStore(){} ;
	virtual enum TVerdict doTestStepL();

	private:

		class CMemTestGetONList : public CTestOnStore::CTestGetONList
			{
		public:
			static CMemTestGetONList* NewLC(RMobilePhoneStore& aStore, CTestMemOnStore* aTestMemOnStore);
			void Start();
			CTestMemOnStore* iTestMemOnStore;
		protected:
			CMemTestGetONList(RMobilePhoneStore& aStore, CTestMemOnStore* aTestMemOnStore);
			TInt RunError(TInt aError);
			void StartMemTest();
			virtual void RunL();
		protected:
			TInt iFailEvery;
			};

	friend class CMemTestGetONList;

	};

#endif
