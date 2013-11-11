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
// @file testltsyphbkreadinvalidindex.h
// 

#ifndef TESTLTSYPHBKREADINVALIDINDEX_H
#define TESTLTSYPHBKREADINVALIDINDEX_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsyphonestorebase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyPhbkReadInvalidIndex
 *
 */
class CTestLtsyPhbkReadInvalidIndex : public CTestLtsyPhoneStoreBase
	{
public:

	~CTestLtsyPhbkReadInvalidIndex();
	static CTestLtsyPhbkReadInvalidIndex* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyPhbkReadInvalidIndex* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyPhbkReadInvalidIndex(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void ReadEntryL(TInt aIndex);
private:
	
	}; // class CTestLtsyPhbkReadInvalidIndex
_LIT(KCTestLtsyPhbkReadInvalidIndex,"CTestLtsyPhbkReadInvalidIndex");

#endif // TESTLTSYPHBKREADINVALIDINDEX_H
