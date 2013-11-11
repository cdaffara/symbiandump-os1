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
// tsysmon_stepselfmoncancel.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/
 
#ifndef __TSYSMON_STEPSELFMONCANCEL2_H__
#define __TSYSMON_STEPSELFMONCANCEL2_H__


#include "tsysmon_stepbase.h"

//APPFWK-SYSMON-0009

_LIT(KCTestCaseSelfMonCancel2, "SelfMonCancel2");

class CStepSelfMonCancel2 : public CTestStepBase
	{
public:
	CStepSelfMonCancel2();
	
	//from CTestStep
	TVerdict doTestStepL();
	};

#endif
