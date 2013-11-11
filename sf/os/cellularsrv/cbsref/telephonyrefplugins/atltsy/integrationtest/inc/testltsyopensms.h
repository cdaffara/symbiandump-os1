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
// @file testltsyopensmsstore.h
// 

#ifndef TESTLTSYOPENSMS_H
#define TESTLTSYOPENSMS_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyOpenSms
 *
 */
class CTestLtsyOpenSms : public CTestLtsySmsBase
	{
public:

	~CTestLtsyOpenSms();
	static CTestLtsyOpenSms* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyOpenSms* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyOpenSms(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
	
	}; // class CTestLtsyOpenSms
_LIT(KCTestLtsyOpenSms,"CTestLtsyOpenSms");

#endif // TESTLTSYOPENSMS_H
