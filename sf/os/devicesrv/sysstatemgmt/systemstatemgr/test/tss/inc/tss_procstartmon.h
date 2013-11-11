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
 
#ifndef __TSS_PROCSTARTMON_H__
#define __TSS_PROCSTARTMON_H__


#include "tss_step_base.h"
#include "testandstopper.h"
#include "ssmtestapps.h"
#include "ssmtestprocgood.h"
#include "ssmtestprocgoodsession.h"

//APPFWK-SSS-0006

_LIT(KCTestCaseProcStartMon, "ProcStartMon");

class CSsTestStepProcStartMon : public CSsStepBase
	{
	
public:
	CSsTestStepProcStartMon();
	~CSsTestStepProcStartMon();
	void SsTestNotify( TInt aNotify );
	//from CTestStep
	TVerdict doTestStepPostambleL();
	TVerdict doTestStepPreambleL();	
	TVerdict doTestStepL();
	
public:
	void DoTestMonSyncL();
	void DoTestMonForAlreadyStartedProcL();
	
private:
	CActiveScheduler* iActiveScheduler;
	RSemaphore iSem;
	};

#endif // __TSS_PROCSTARTMON_H__
