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
// @file testltsyphbkdeleteinvalidindex.h
// 

#ifndef TESTLTSYPHBKDELETEINVALIDINDEX_H
#define TESTLTSYPHBKDELETEINVALIDINDEX_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsyphonestorebase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyPhbkDeleteInvalidIndex
 *
 */
class CTestLtsyPhbkDeleteInvalidIndex : public CTestLtsyPhoneStoreBase
	{
public:

	~CTestLtsyPhbkDeleteInvalidIndex();
	static CTestLtsyPhbkDeleteInvalidIndex* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyPhbkDeleteInvalidIndex* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyPhbkDeleteInvalidIndex(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void DeletePhonebookL(TInt aIndex);
private:
	
	}; // class CTestLtsyPhbkDeleteInvalidIndex
_LIT(KCTestLtsyPhbkDeleteInvalidIndex,"CTestLtsyPhbkDeleteInvalidIndex");

#endif // TESTLTSYPHBKDELETEINVALIDINDEX_H
