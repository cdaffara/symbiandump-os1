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
 

#ifndef __TCMN_STEP_SWPVALUE_H__
#define __TCMN_STEP_SWPVALUE_H__

#include <test/testexecutestepbase.h>

_LIT(KTCmnSwpValueStep,"CmnSwpValueStep");

class CCmnSwpValueTest : public CTestStep
	{
public:
	CCmnSwpValueTest();
	~CCmnSwpValueTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void doTestConstructors();
	void doTestOperators();
	void doTestOtherMethods();
	};

#endif
