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

#ifndef __TRESTARTSYS_WITHMODE_H__
#define __TRESTARTSYS_WITHMODE_H__

#include <test/testexecutestepbase.h>
#include <savenotf.h>

_LIT(KRestartSysWithMode, "RestartSysWithMode");

/**
Only 4 bits are used to persist a startup mode on the H4 reference hardware, 
which results in 2^4 = 16 startup modes available in total.

from omap_hrp/assp/omap24xx/inc/omap24xxconst.h

// Restart startup Mode (bits 19-16)
const TUint KmRestartStartupModes			= 0x000F0000; // Bits 19 to 16
const TUint KsRestartStartupModes			= 16;
const TUint KRestartStartupModesSize		= 4; // 4 bits
*/
const TInt KDefaultBootMode = 0;
const TInt KNonDefaultBootMode = 15;

/**
This is the test step class for a restartsys test where a restart actually takes place,
and in a different startup-configuration than the default. The test used the last used 
startup-mode to determine in which phase  it is, i.e. before or after the restart.

Only one test of this kind can be run by DABS, and it should be the first test listed
in autoexec.bat.
*/
class CRestartSysWithMode : public CTestStep, public MSaveObserver
	{
public:
	CRestartSysWithMode(const TDesC& aStepName);
	~CRestartSysWithMode();

	// from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();

	// from MSaveObserver
	void SaveL(TSaveType aSaveType);

private:
	void InitRebootL();
	void SaveVariablesToFileL();
	void AssertExpectationsL();
	void AssertRestartIsPreemptiveL();	

private:
	CActiveScheduler* iScheduler;
	CSaveNotifier* iNotifier;
	TTime iShutdownInitiated;
	TTime iShutdownPerformed;
	TSaveType iSaveType;
	};

#endif
