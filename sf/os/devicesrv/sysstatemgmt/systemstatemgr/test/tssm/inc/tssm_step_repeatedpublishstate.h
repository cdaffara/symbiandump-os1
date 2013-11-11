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
 

#ifndef __TSSM_STEP_REPEATEDPUBLISHSTATE_H__
#define __TSSM_STEP_REPEATEDPUBLISHSTATE_H__

#include <test/testexecutestepbase.h>

#include "tssm_ssmclient.h"

_LIT(KTSsmRepeatedPublishStateStep,"SsmRepeatedPublishStateStep");



//---------------- CSsmRepeatedPublishStateTest step --------------------------------

class CSsmRepeatedPublishStateTest : public CTestStep
	{
public:
	CSsmRepeatedPublishStateTest();
	~CSsmRepeatedPublishStateTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void DoRepeatedPublishStateL();
	
	// Utility methods
	void RequestAndWaitForStateTransitionL(TSsmStateTransition& aState);
	
private:
	RSsmStateManagerTestClient iClient;
	};


#endif
