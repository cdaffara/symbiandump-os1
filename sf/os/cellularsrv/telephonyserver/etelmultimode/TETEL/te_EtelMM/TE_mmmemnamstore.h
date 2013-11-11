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

#ifndef _TE_MMMEMNAMSTORE_H_
#define _TE_MMMEMNAMSTORE_H_

#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"
#include "TE_mmnamstore.h"

class CTestMemNamStore : public CTestNamStore
	{
public:
	
	TInt iTestCount;
	CTestMemNamStore() ;
	~CTestMemNamStore(){} ;
	virtual enum TVerdict doTestStepL();

private:

class CMemTestGetNamList : public CTestNamStore::CTestGetNamList
		{
	public:
		static CMemTestGetNamList* NewLC(RMobilePhoneStore& aStore, CTestMemNamStore* aTestMemNamStore);
		void Start();
		CTestMemNamStore* iTestMemNamStore;
	protected:
		CMemTestGetNamList(RMobilePhoneStore& aStore, CTestMemNamStore* aTestMemNamStore);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};

friend class CMemTestGetNamList;

	};

#endif
