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
 

#ifndef __TSUS_STEP_SUPINFO_H__
#define __TSUS_STEP_SUPINFO_H__

#include <test/testexecutestepbase.h>

_LIT(KTSsmSupInfoStep,"SsmSupInfoStep");


class CSsmSupInfoTest : public CTestStep
	{
public:
	CSsmSupInfoTest();
	~CSsmSupInfoTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void doTestStructL();
	void doTestSerializationL();
	};

#endif
