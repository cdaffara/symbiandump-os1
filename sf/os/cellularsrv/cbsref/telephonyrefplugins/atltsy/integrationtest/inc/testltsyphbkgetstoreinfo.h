// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file testltsyphbkgetstoreinfo.h
// 

#ifndef TESTLTSYPHBKGETSTOREINFO_H
#define TESTLTSYPHBKGETSTOREINFO_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsyphonestorebase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyPhbkGetStoreInfo
 *
 */
class CTestLtsyPhbkGetStoreInfo : public CTestLtsyPhoneStoreBase
	{
public:

	~CTestLtsyPhbkGetStoreInfo();
	static CTestLtsyPhbkGetStoreInfo* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyPhbkGetStoreInfo* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyPhbkGetStoreInfo(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void GetStoreInfoL();
private:
	
	}; // class CTestLtsyPhbkGetStoreInfo
_LIT(KCTestLtsyPhbkGetStoreInfo,"CTestLtsyPhbkGetStoreInfo");

#endif // TESTLTSYPHBKGETSTOREINFO_H
