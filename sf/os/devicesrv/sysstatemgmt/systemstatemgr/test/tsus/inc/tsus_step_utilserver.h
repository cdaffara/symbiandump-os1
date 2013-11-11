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
 

#ifndef __TSUS_STEP_UTILSERVER_H__
#define __TSUS_STEP_UTILSERVER_H__

#include <test/testexecutestepbase.h>

_LIT(KTSusUtilServerStep,"SusUtilServerStep");

class RSsmSusCli;

class CSusUtilServerTest : public CTestStep
	{
public:
	CSusUtilServerTest ();
	~CSusUtilServerTest ();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void doTestForMemoryLeaksL();
	void doTestForMemoryLeaksDuringHeapFailureL();
	void doTestProcessCriticalL();
	void doTestUnconnected();
	void doTestPositiveConnectCloseL();
	void doTestNegativeConnectCloseL();
	void doTestLoadSupL();
	void TestLoadMissing(RSsmSusCli& aSession);
	void TestLoadOkL(RSsmSusCli& aSession);
	void TestLoadDuplicate(RSsmSusCli& aSession);
	void TestLoadWrongType(RSsmSusCli& aSession);
	void TestLoadWrongIdentity(RSsmSusCli& aSession);
	void TestUnloadSup(RSsmSusCli& aSession);
	};

#endif
