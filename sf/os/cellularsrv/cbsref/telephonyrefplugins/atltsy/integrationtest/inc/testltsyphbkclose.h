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
// @file testltsyphbkclose.h
// 

#ifndef TESTLTSYPHBKCLOSE_H
#define TESTLTSYPHBKCLOSE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsyphonestorebase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyPhbkClose
 *
 */
class CTestLtsyPhbkClose : public CTestLtsyPhoneStoreBase
	{
public:

	~CTestLtsyPhbkClose();
	static CTestLtsyPhbkClose* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyPhbkClose* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyPhbkClose(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void ClosePhonebookL();
private:
	
	}; // class CTestLtsyPhbkClose
_LIT(KCTestLtsyPhbkClose,"CTestLtsyPhbkClose");

#endif // TESTLTSYPHBKCLOSE_H
