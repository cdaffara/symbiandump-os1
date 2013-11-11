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

#ifndef __TSTARTSAFE_APPSTARTRVERROR_H__
#define __TSTARTSAFE_APPSTARTRVERROR_H__


#include <test/testexecutestepbase.h>

#include "tstartsafe_asyncnotifier.h"

//APPFWK-STARTSAFE-0016
_LIT(KCTestCaseAppStartRvError, "AppStartRvError");

class CAppFwkStartSafeTestStepAppStartRvError : public CTestStep, public MSsTestAsyncNotifier
	{
	virtual void SsTestNotify( TInt aNotify );
	
public:
	CAppFwkStartSafeTestStepAppStartRvError();
	~CAppFwkStartSafeTestStepAppStartRvError();

public:
	// From CTestStep
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
private:
	CActiveScheduler* iActiveScheduler;
	RProcess iProc;
	TInt iTried;
	
	};

#endif // __TSTARTSAFE_APPSTARTRVERROR_H__
