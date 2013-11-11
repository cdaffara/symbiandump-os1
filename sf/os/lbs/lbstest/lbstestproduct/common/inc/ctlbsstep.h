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
// @file CT_LbsServer.h
// This contains the header file for LBS  Test Step Base
// 
//

#ifndef __CT_LBS_STEP_H__
#define __CT_LBS_STEP_H__


#include <test/testexecutestepbase.h>

#include "ctlbsserver.h"


class CT_LbsStep : public CTestStep
{

public:
	virtual ~CT_LbsStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsStep(CT_LbsServer& aParent);

	// Class members which will be common to all individual test steps:
	const CT_LbsServer& iParent;
	

private:
};

#endif // __CT_LBS_STEP_H__
