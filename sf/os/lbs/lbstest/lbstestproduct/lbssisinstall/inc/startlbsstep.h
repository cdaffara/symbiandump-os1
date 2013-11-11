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
// @file ctlbsstartlbs.h
// This contains the header file for step to start lbs system
// 
//

#ifndef __CT_START_LBS_STEP_H__
#define __CT_START_LBS_STEP_H__

#include <test/TestExecuteStepBase.h>

// Literals used
_LIT(KStartLbsStep, "StartLbsStep");

const TUint KLbsTestDataSrcServerUidValue = 0x10282259;

class CT_StartLbsStep : public CTestStep
	{
public:
	CT_StartLbsStep();
	~CT_StartLbsStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	};

#endif //__CT_START_LBS_STEP_H__
