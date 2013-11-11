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
// @file testltsyphbkdelete.h
// 

#ifndef TESTLTSYPHBKDELETE_H
#define TESTLTSYPHBKDELETE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsyphonestorebase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyPhbkDelete
 *
 */
class CTestLtsyPhbkDelete : public CTestLtsyPhoneStoreBase
	{
public:

	~CTestLtsyPhbkDelete();
	static CTestLtsyPhbkDelete* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyPhbkDelete* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyPhbkDelete(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void DeletePhonebookL(TInt aIndex);
private:
	
	}; // class CTestLtsyPhbkDelete
_LIT(KCTestLtsyPhbkDelete,"CTestLtsyPhbkDelete");

#endif // TESTLTSYPHBKDELETE_H
