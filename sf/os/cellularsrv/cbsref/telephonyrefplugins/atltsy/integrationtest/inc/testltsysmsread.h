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
// @file testltsysmsread.h
// 

#ifndef TESTLTSYSMSREAD_H
#define TESTLTSYSMSREAD_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsRead
 *
 */
class CTestLtsySmsRead : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsRead();
	static CTestLtsySmsRead* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsRead* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsRead(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void ReadSmsL(TInt aIndex);
private:

	RMobileSmsStore::TMobileGsmSmsEntryV1 iEntryV1;
	}; // class CTestLtsySmsRead
_LIT(KCTestLtsySmsRead,"CTestLtsySmsRead");

#endif // TESTLTSYSMSREAD_H
