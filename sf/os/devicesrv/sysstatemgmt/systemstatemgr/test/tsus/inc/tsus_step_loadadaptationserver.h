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


#ifndef TSUS_STEP_LOADADAPTATIONSERVER_H_
#define TSUS_STEP_LOADADAPTATIONSERVER_H_

#include <test/testexecutestepbase.h>
#include "ssmadaptationcli.h"
#include "susadaptionserver.h"

_LIT(KTSusLoadAdaptationServerStep,"SusLoadAdaptationServerStep");


class CTestSusLoadAdaptationServer  : public CTestStep
	{
public:
	CTestSusLoadAdaptationServer  ();
	~CTestSusLoadAdaptationServer  ();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void doTestLoadStateAdaptationOOML();
	void TestLoadStateAdaptationL();
	void doTestLoadStateAdaptationObserversOOML();
	void TestLoadStateAdaptationObserversL();
	void doTestLoadSimAdaptationOOML();
	void TestLoadSimAdaptationL();
	void doTestLoadSimAdaptationObserversOOML();
	void TestLoadSimAdaptationObserversL();
	void doTestLoadRtcAdaptationOOML();
	void TestLoadRtcAdaptationL();
	void doTestLoadMiscAdaptationOOML();
	void TestLoadMiscAdaptationL();
	void doTestLoadEmergencyCallRfAdaptationOOML();
	void TestLoadEmergencyCallRfAdaptationL();
	};

#endif /*TSUS_STEP_LOADADAPTATIONSERVER_H_*/

