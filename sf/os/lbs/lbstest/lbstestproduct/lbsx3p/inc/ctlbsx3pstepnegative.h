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
// @file ctlbsx3pstepnegative.h
// This contains the header file for LBS Assistance GPS Data Test Step Base
// 
//

#ifndef __CT_LBS_X3P_STEP_NEGATIVE_H__
#define __CT_LBS_X3P_STEP_NEGATIVE_H__

#include <test/testexecutestepbase.h>


#include "ctlbsx3pserver.h"

const TUint KBadPriority = 0;
_LIT(KLbsX3P_Step_Negative, "LbsX3P_Step_Negative");
_LIT(KBadDestination, "07712345678");


//#define X3P_PRIORITY_PUSH			0x02
//#define X3P_PRIORITY_MENU			0x08
//#define X3P_PRIORITY_TIMER			0x10


class CT_LbsX3PStep_Negative :	public CTestStep
{

public:

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	virtual ~CT_LbsX3PStep_Negative();
	static CT_LbsX3PStep_Negative* New(CT_LbsX3PServer& aParent);
	
private:
	TInt iTestCaseId;

	void ConstructL();
	virtual TVerdict doTestStepL();
		
		
protected:
	CT_LbsX3PStep_Negative(CT_LbsX3PServer& aParent);

	// Class members which will be common to all individual test steps:
	const CT_LbsX3PServer& iParent;
		
};

#endif // __CT_LBS_X3P_STEP_NEGATIVE_H__
