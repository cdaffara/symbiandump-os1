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
// @file testltsyphbkwriteinvalidindex.h
// 

#ifndef TESTLTSYPHBKWRITEINVALIDINDEX_H
#define TESTLTSYPHBKWRITEINVALIDINDEX_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsyphonestorebase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyPhbkWriteInvalidIndex
 *
 */
class CTestLtsyPhbkWriteInvalidIndex : public CTestLtsyPhoneStoreBase
	{
public:

	~CTestLtsyPhbkWriteInvalidIndex();
	static CTestLtsyPhbkWriteInvalidIndex* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyPhbkWriteInvalidIndex* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyPhbkWriteInvalidIndex(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void WriteEntryL(TInt aIndex);
private:
	
	}; // class CTestLtsyPhbkWriteInvalidIndex
_LIT(KCTestLtsyPhbkWriteInvalidIndex,"CTestLtsyPhbkWriteInvalidIndex");

#endif // TESTLTSYPHBKWRITEINVALIDINDEX_H
