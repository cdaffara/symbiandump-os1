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

#ifndef __TSTARTUPPROPERTIES_STEPDEFVAL_H__
#define __TSTARTUPPROPERTIES_STEPDEFVAL_H__

#include <test/testexecutestepbase.h>

class CStartupProperties;
class RResourceFile;

_LIT(KCTestCaseDefVal, "DefVal");	

class CStartupPropertiesTestStepDefVal : public CTestStep
	{
public:
	CStartupPropertiesTestStepDefVal();
	
	//from CTestStep
	TVerdict doTestStepL();
	
private:
	CStartupProperties* CreateFromResourceL(TInt aResourceId);
	void TestDefaultValues_CtorL();
	void TestDefaultValues_START_APP_INFOL(RResourceFile& aRFile);
	void TestDefaultValues_START_APP_INFO2L(RResourceFile& aRFile);
	void TestDefaultValues_START_PROCESS_INFOL(RResourceFile& aRFile);
	void TestDefaultValues_START_PROCESS_INFO2L(RResourceFile& aRFile);
	CStartupProperties* CreateFromResourceLC(RResourceFile& aRFile, TInt aResourceId);
	};

#endif
