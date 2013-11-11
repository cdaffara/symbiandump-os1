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

#ifndef __TSTARTSAFE_PROCMONSYSRESTART_H__
#define __TSTARTSAFE_PROCMONSYSRESTART_H__


#include <savenotf.h>
#include <test/testexecutestepbase.h>

class CStartSafe;


//APPFWK-STARTSAFE-0013

_LIT(KCTestCaseProcMonSysRestart, "ProcMonSysRestart");

class CAppFwkStartSafeTestStepProcMonSysRestart : public CTestStep, public MSaveObserver
	{
	
public:
	CAppFwkStartSafeTestStepProcMonSysRestart();
	~CAppFwkStartSafeTestStepProcMonSysRestart();
		
	//from MSaveObserver
	void SaveL(TSaveType aSaveType);
	
	//from CTestStep
	TVerdict doTestStepL();
	
private:
	CSaveNotifier *iNotifier;
	TBool iCallbackReceived;
	TTime iShutdownPerformed;
	};

#endif
