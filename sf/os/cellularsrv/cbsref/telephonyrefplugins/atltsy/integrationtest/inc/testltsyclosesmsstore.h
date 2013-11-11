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
// @file testltsyclosesmsstore.h
// 

#ifndef TESTLTSYCLOSESMSSTORE_H
#define TESTLTSYCLOSESMSSTORE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyCloseSmsStore
 *
 */
class CTestLtsyCloseSmsStore : public CTestLtsySmsBase
	{
public:

	~CTestLtsyCloseSmsStore();
	static CTestLtsyCloseSmsStore* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyCloseSmsStore* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyCloseSmsStore(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:

	}; // class CTestLtsyCloseSmsStore
_LIT(KCTestLtsyCloseSmsStore,"CTestLtsyCloseSmsStore");

#endif // TESTLTSYCLOSESMSSTORE_H
