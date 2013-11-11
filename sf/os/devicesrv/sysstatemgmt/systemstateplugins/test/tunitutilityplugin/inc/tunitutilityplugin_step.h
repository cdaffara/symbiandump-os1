// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __TUNITUTILITYPLUGIN_TEST_H__
#define __TUNITUTILITYPLUGIN_TEST_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmstateawaresession.h>
#include <e32event.h>
#include <shutdownsrv.h>
_LIT(KTUnitUtilityPluginStep,"UnitUtilityPluginStep");

class CUnitUtilityPluginTest : public CTestStep
	{
public:
	typedef void (CUnitUtilityPluginTest::*ClassFuncPtrL) ();
	CUnitUtilityPluginTest(const TDesC& aStepName);
	~ CUnitUtilityPluginTest();
public:
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

	void TestClientArrayL();
	void TestGetShutdownStateL();
	void TestIsClientHungL();
private:
    CActiveScheduler* iScheduler;
	};

#endif / /__TUNITUTILITYPLUGIN_TEST_H__
