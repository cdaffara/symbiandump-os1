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
#ifndef _TE_MMPHONEBOOKSTORE_H_
#define _TE_MMPHONEBOOKSTORE_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include <e32base.h>
#include <mpbutil.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"

typedef TBuf8<KMaxName>                 TMobilePhoneBookData;

class CTestPhonebookStore : public CEtelMMTestStepBase
	{
public:
	TInt iTestCount;
	CTestPhonebookStore() ;
	~CTestPhonebookStore(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidatePhoneBookInfo(const RMobilePhoneBookStore::TMobilePhoneBookInfoV1 &aInfo);
	/*static*/ void ValidatePhoneBookInfo(const RMobilePhoneBookStore::TMobilePhoneBookInfoV2 &aInfo);
	/*static*/ void ValidatePhoneBookInfo(const RMobilePhoneBookStore::TMobilePhoneBookInfoV5 &aInfo);
	/*static*/ void ValidatePhoneBookRead(const TInt aIndex, const TInt aNumEntries, const TMobilePhoneBookData &aPBData);
	/*static*/ void ValidatePhoneBookMultipleRead(const TInt aIndex, const TInt aNumEntries, const TMobilePhoneBookData &aPBData);
	static void InitPhoneBookEntry(TInt &aNumEntries, TMobilePhoneBookData &aPBData);
	};

#endif
