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
// @file ctlbsx3pserver.h
// This contains the header file for LBS Assistance GPS Data Test Step Base
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_H__
#define __CT_LBS_PRIVFW_STEP_H__

#include <test/TestExecuteStepBase.h>
//#include <lbs.h>

#include "ctlbsprivfwserver.h"
//#include "ctlbsnetsimstep.h"


//const TLbsCallbackFlags KLbsCallback_Got_NotifyDoTransmitPos					= 0x1000;
//const TLbsCallbackFlags KLbsCallback_Got_DoPosUpdate							= 0x2000;
//const TLbsCallbackFlags KLbsCallback_Got_NotifyDoTransmitReferencePos			= 0x4000;
//#define X3P_PRIORITY_PUSH			0x02
//#define X3P_PRIORITY_MENU			0x08
//#define X3P_PRIORITY_TIMER			0x10

class CT_LbsPrivFwStep : public CTestStep
{

public:
	virtual ~CT_LbsPrivFwStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:

	// From MLbsNetSimTestObserver
	//NOTE: The functions are defined here to avoid unnecessary implementation in derived class.
	//The derived class has to implement required functions

	void ConstructL();
	CT_LbsPrivFwStep(CT_LbsPrivFwServer& aParent);
	
protected:
	//ROomRequest		iOomRequest;
	CT_LbsPrivFwServer& iParent;
		
};

#endif // __CT_LBS_PRIVFW_STEP_H__
