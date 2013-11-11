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
// @file ctlbsclockstep.h
// This contains the header file for Clock Test Step Base
// 
//

#ifndef __CT_LBS_CLOCK_STEP_H__
#define __CT_LBS_CLOCK_STEP_H__

// EPOC includes.
#include <test/testexecutestepbase.h>

// LBS test includes.
#include <lbs/test/lbsnetsimtest.h>
#include "ctlbsclockserver.h"


class CT_LbsClockStep : public CTestStep
{

public:
	virtual ~CT_LbsClockStep();

protected:
	CT_LbsClockStep(CT_LbsClockServer& aParent);

	void ConstructL();

	TInt OpenNetSim();
	void CloseNetSim();


	/** Server parent class, generally used to access shared data. */
	CT_LbsClockServer& iParent;

	/** Net sim required for tests which invoke assistance data via AGPS module. */
	RLbsNetSimTest iNetSim;
	
	/** Reference position used to setup Net sim and later verification. */
	TPosition iRefPos;

};

#endif // __CT_LBS_CLOCK_STEP_H__
