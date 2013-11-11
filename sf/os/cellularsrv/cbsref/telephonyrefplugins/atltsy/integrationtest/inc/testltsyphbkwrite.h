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
// @file testltsyphbkwrite.h
// 

#ifndef TESTLTSYPHBKWRITE_H
#define TESTLTSYPHBKWRITE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsyphonestorebase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyPhbkWrite
 *
 */
class CTestLtsyPhbkWrite : public CTestLtsyPhoneStoreBase
	{
public:

	~CTestLtsyPhbkWrite();
	static CTestLtsyPhbkWrite* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyPhbkWrite* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyPhbkWrite(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void WriteEntryL(TInt aIndex);
private:
	
	}; // class CTestLtsyPhbkWrite
_LIT(KCTestLtsyPhbkWrite,"CTestLtsyPhbkWrite");

#endif // TESTLTSYPHBKWRITE_H
