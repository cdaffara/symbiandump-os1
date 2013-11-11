// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TRESTARTSYS_ABORT_H__
#define __TRESTARTSYS_ABORT_H__

#include <test/testexecutestepbase.h>
#include <savenotf.h>

_LIT(KRestartSysAbort, "RestartSysAbort");

/**
This is the test step class for the simpler restartsys test (in which the restart is aborted as
soon  we detect it is going to take place).This class inherit from MSaveObserver to detect 
that the restart is successfully initiated.
*/
class CRestartSysAbort : public CTestStep, public MSaveObserver
	{
public:
	CRestartSysAbort(const TDesC& aStepName);
	~CRestartSysAbort();

	// from CTestStep
	TVerdict doTestStepL();

	// from MSaveObserver
	void SaveL(TSaveType aSaveType);

private:
	CSaveNotifier* iNotifier;
	TTime iShutdownInitiated;
	TBool iShutdownPerformed;
	};

#endif
