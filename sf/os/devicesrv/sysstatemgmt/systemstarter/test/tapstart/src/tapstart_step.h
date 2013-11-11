// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/
 
#ifndef __APSTART_STEP_H__
#define __APSTART_STEP_H__

#include <test/testexecutestepbase.h>

class CApaStarter;
class RWsSession;

_LIT(KCTestCaseApStart, "CApStartTestStep");

class TApaTask;

class CApStartTestStep : public CTestStep
	{	
public:
	CApStartTestStep();
	
	// from CTestStep
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
private:
	void ExecuteL();
	void TestInitApparcL();
	void TestStartAndForgetL(RWsSession& aWsSession);
	void TestStartAndFollowUpL(RWsSession& aWsSession);
	TApaTask TestStartL(TRequestStatus* aRequestStatus, RWsSession& aWsSession);
	
private:
	CApaStarter* iAppStart;
	};
	
#endif
