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
// @file testltsyopensmsstore.h
// 

#ifndef TESTLTSYOPENSMSSTORE_H
#define TESTLTSYOPENSMSSTORE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyOpenSmsStore
 *
 */
class CTestLtsyOpenSmsStore : public CTestLtsySmsBase
	{
public:

	~CTestLtsyOpenSmsStore();
	static CTestLtsyOpenSmsStore* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyOpenSmsStore* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyOpenSmsStore(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
	
	}; // class CTestLtsyOpenSmsStore
_LIT(KCTestLtsyOpenSmsStore,"CTestLtsyOpenSmsStore");

#endif // TESTLTSYOPENSMSSTORE_H
