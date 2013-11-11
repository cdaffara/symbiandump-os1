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
// @file testltsysmsnack.h
// 

#ifndef TESTLTSYSMSNACK_H
#define TESTLTSYSMSNACK_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsNack
 *
 */
class CTestLtsySmsNack : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsNack();
	static CTestLtsySmsNack* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsNack* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsNack(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void NackSmsStoredL();
private:

	}; // class CTestLtsySmsNack
_LIT(KCTestLtsySmsNack,"CTestLtsySmsNack");

#endif // TESTLTSYSMSNACK_H
