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
// @file testltsysmsdeleteall.h
// 

#ifndef TESTLTSYSMSDELETE_H
#define TESTLTSYSMSDELETE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsDelete
 *
 */
class CTestLtsySmsDelete : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsDelete();
	static CTestLtsySmsDelete* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsDelete* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsDelete(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void DeleteSmsL(TInt aIndex);
private:

	}; // class CTestLtsySmsDelete
_LIT(KCTestLtsySmsDelete,"CTestLtsySmsDelete");

#endif // TESTLTSYSMSDELETE_H
