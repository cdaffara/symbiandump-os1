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
// @file testltsysmsdeleteinvalidindex.h
// 

#ifndef TESTLTSYSMSDELETEINVALIDINDEX_H
#define TESTLTSYSMSDELETEINVALIDINDEX_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsDeleteInvalidIndex
 *
 */
class CTestLtsySmsDeleteInvalidIndex : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsDeleteInvalidIndex();
	static CTestLtsySmsDeleteInvalidIndex* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsDeleteInvalidIndex* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsDeleteInvalidIndex(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void DeleteSmsL(TInt aIndex);
private:

	}; // class CTestLtsySmsDeleteInvalidIndex
_LIT(KCTestLtsySmsDeleteInvalidIndex,"CTestLtsySmsDeleteInvalidIndex");

#endif // TESTLTSYSMSDELETEINVALIDINDEX_H
