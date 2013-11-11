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

#ifndef __TSTARTUPPROPERTIES_STEPINTERNALEXTERNAL_H__
#define __TSTARTUPPROPERTIES_STEPINTERNALEXTERNAL_H__

#include <test/testexecutestepbase.h>

_LIT(KCTestCaseInternalExternal, "InternalExternal");


//APPFWK-STARTUPPROPERTIES-0002

class CStartupPropertiesTestStepInternalExternal : public CTestStep
	{
public:
CStartupPropertiesTestStepInternalExternal();
	
	//from CTestStep
	TVerdict doTestStepL();

private:
	void DoTestWithDataL();
	void DoTestWithNullPointersL();
	void DoTestResetL();
	};

#endif
