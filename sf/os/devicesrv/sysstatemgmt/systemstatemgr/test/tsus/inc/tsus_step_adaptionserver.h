// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal test code 
*/
 

#ifndef __TSUS_STEP_ADAPTIONSERVER_H__
#define __TSUS_STEP_ADAPTIONSERVER_H__


#include <test/testexecutestepbase.h>
#include "ssmadaptationcli.h"


_LIT(KTSusAdaptionServerStep,"SusAdaptionServerStep");

class RTestSusAdaptationCli : public RSsmEmergencyCallRfAdaptation
    {
public:
#ifdef TEST_SSM_SERVER
    TInt Connect(const TDesC& aServerName);
#endif 
    TInt SetHeapFailure(TInt aFailureRate);
    TInt UnSetHeapFailure();
    TInt SetHeapMark();
    TInt UnSetHeapMark();
    
    };

class CSusAdaptionServerTest : public CTestStep
	{
public:
	CSusAdaptionServerTest ();
	~CSusAdaptionServerTest ();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void doTestForMemoryLeaksL();
	void doTestProcessCriticalL();
	void doTestOOML();	
	void StartAndDestroyServerL();
#ifdef TEST_SSM_SERVER
	void doTestForEmergencyCallOOM();
	void doTestForSettingPriorityClient();
	void doTestForNormalEmergencyCallinOOM();
	void doTestForMultipleClientinOOMcondition();
	void doTestForEmergencyCallOOMIterative();
#endif
	};

#endif
