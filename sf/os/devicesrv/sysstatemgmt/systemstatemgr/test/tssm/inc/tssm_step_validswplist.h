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
 

#ifndef __TSSM_STEP_VALIDSWPLIST_H__
#define __TSSM_STEP_VALIDSWPLIST_H__

#include <test/testexecutestepbase.h>

_LIT(KTSsmValidSwpListStep,"SsmValidSwpListStep");


class CSsmValidSwpListTest;
//--------------------- Thread creation framework ---------------------

TInt StartSwpInvalidListInThreadL(CSsmValidSwpListTest* aSsmValidSwpListTest);
TInt ThreadStartSwpInvalidListFn(TAny* aPtr);
void ThreadDispatchSwpInvalidListFunctionL(CSsmValidSwpListTest* aSsmValidSwpListTest);


//---------------- CSsmValidSwpListTest step ---------------------------------------

class CSsmValidSwpListTest : public CTestStep
	{
public:
	CSsmValidSwpListTest();
	~CSsmValidSwpListTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	void GetSsmSwpPolicyServerThreadIdL();
	TInt Function();

private:
	void SetFunction(TInt aFunction);

private:
	RThread iThread;
	TInt iFunction;
	};


#endif
