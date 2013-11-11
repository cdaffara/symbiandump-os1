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
// @file testltsysmsatutil.h
// 

#ifndef TESTLTSYSMSACK_H
#define TESTLTSYSMSACK_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsAck
 *
 */
class CTestLtsySmsAck : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsAck();
	static CTestLtsySmsAck* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsAck* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsAck(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void AckSmsStoredL();
private:

	}; // class CTestLtsySmsAck
_LIT(KCTestLtsySmsAck,"CTestLtsySmsAck");

#endif // TESTLTSYSMSACK_H
