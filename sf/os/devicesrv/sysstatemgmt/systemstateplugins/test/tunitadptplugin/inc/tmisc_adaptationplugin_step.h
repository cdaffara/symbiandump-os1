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


#ifndef __TMISC_ADAPTATIONPLUGIN_STEP_H__
#define __TMISC_ADAPTATIONPLUGIN_STEP_H__


#include "miscadaptationref.h"
#include "tadaptation_step_base.h"
#include <ssm/ssmstate.h>
#include <ssm/ssmstates.hrh>

#include <ssm/ssmadaptationcli.h>

_LIT(KTCTestMiscAdaptationPlugin, "CTestMiscAdaptationPlugin");


class CTestMiscAdaptationPlugin : public CAdaptationTestBase
	{
public:
	CTestMiscAdaptationPlugin();
	~CTestMiscAdaptationPlugin();

	//from CAdaptationTestBase
	TVerdict doTestStepL();

private:

    void DoTestPrepareSimLanguages(TSsmLanguageListPriority aPriority);
	void TestSecurityStateChange();
	void TestGetGlobalStartupMode();
	void TestPrepareSimLanguages();
	void TestGetSimLanguages();
	void TestGetHiddenReset();
	void TestCancel();


private:

	RSsmMiscAdaptation iSsmMiscAdaptation;
	};

#endif	// __TMISC_ADAPTATIONPLUGIN_STEP_H__
