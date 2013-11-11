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
// @file testltsyclosesms.h
// 

#ifndef TESTLTSYCLOSESMS_H
#define TESTLTSYCLOSESMS_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyCloseSms
 *
 */
class CTestLtsyCloseSms : public CTestLtsySmsBase
	{
public:

	~CTestLtsyCloseSms();
	static CTestLtsyCloseSms* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyCloseSms* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyCloseSms(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:

	}; // class CTestLtsyCloseSms
_LIT(KCTestLtsyCloseSms,"CTestLtsyCloseSms");

#endif // TESTLTSYCLOSESMS_H
