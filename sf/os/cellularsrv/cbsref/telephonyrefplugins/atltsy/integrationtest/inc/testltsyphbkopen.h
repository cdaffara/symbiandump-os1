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
// @file testltsyphbkopen.h
// 

#ifndef TESTLTSYPHBKOPEN_H
#define TESTLTSYPHBKOPEN_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsyphonestorebase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyPhbkOpen
 *
 */
class CTestLtsyPhbkOpen : public CTestLtsyPhoneStoreBase
	{
public:

	~CTestLtsyPhbkOpen();
	static CTestLtsyPhbkOpen* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyPhbkOpen* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyPhbkOpen(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void OpenPhonebookL();
private:
	
	}; // class CTestLtsyPhbkOpen
_LIT(KCTestLtsyPhbkOpen,"CTestLtsyPhbkOpen");

#endif // TESTLTSYPHBKOPEN_H
