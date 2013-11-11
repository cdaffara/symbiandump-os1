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
 

#ifndef __TSSM_STEP_SWPCOMMONPLUGIN_H__
#define __TSSM_STEP_SWPCOMMONPLUGIN_H__

#include <test/testexecutestepbase.h>

_LIT(KTSsmSwpCommonPluginStep,"SsmSwpCommonPluginStep");



//---------------- CSsmSwpCommonPluginTest step ---------------------------------------

class CSsmSwpCommonPluginTest : public CTestStep
	{
public:
	CSsmSwpCommonPluginTest();
	~CSsmSwpCommonPluginTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void RequestSingleSwpChange();
	void RequestMultipleSwpChangeSamePlugin();
	void RequestMultipleSwpChange();
	void RequestCancelQueuedSwpChange();
	void RequestCancelRunningSwpChange();
	void StressSwpChange();
	void RequestMultipleSwpChangeWithPanicL();

private:
	};


#endif
