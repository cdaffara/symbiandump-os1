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

#ifndef TESTSTEPCIPLAYBACKSTATUS_H
#define TESTSTEPCIPLAYBACKSTATUS_H

#include <testframework.h>

// Custom interface definitions
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>

#include "devsoundciutestdevices.hrh"

// Test system includes
#include "tsu_mmf_devsound_ciu_step.h"
#include "tsu_mmf_devsound_ciu_suite.h"


class CTestStepCIPlaybackStatus : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIPlaybackStatus();
	TVerdict DoTestStepL();
	};

class CTestStepCIPlaybackStatusInfo : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIPlaybackStatusInfo();
	TVerdict DoTestStepL();
	};

class CTestStepCIPlaybackStatusRequestNotification : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIPlaybackStatusRequestNotification(TBool aIsCancelTest);
	TVerdict DoTestStepL();
private:
	void DoReadFromFileL(TBool& aFlag);
	void DoRequestTestL(MMMFPlaybackStatus& aInterface);
	void DoCancelTestL(MMMFPlaybackStatus& aInterface);
private:
	TBool iIsCancelTest;
	};

#endif
