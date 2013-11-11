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

#ifndef __TSS_APPSTARTAPPARCINIT_H__
#define __TSS_APPSTARTAPPARCINIT_H__


#include "tss_step_base.h"


//APPFWK-SSS-0003

_LIT(KCTestCaseAppStartAppArcInit, "AppStartAppArcInit");


class CSsTestStepAppStartAppArcInit : public CSsStepBase
	{
	
public:
	CSsTestStepAppStartAppArcInit();
	~CSsTestStepAppStartAppArcInit();

	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
public:
	// Frpm CTestStep
	TVerdict doTestStepL();
	
private:
	CActiveScheduler* iActiveScheduler;
	};

#endif // __TSS_APPSTARTAPPARCINIT_H__
