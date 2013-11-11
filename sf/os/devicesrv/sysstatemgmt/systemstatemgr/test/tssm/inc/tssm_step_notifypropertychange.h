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
 

#ifndef __TSSM_STEP_SYSTEMWIDEPROPERTY_H__
#define __TSSM_STEP_SYSTEMWIDEPROPERTY_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmsystemwideproperty.h>

_LIT(KTSsmNotifyPropertyChangeStep,"SsmNotifyPropertyChangeStep");

class CSsmNotifyPropertyChangeTest : public CTestStep, public MSwpChangeNotificationSubscriber
	{
public:
	CSsmNotifyPropertyChangeTest();
	~CSsmNotifyPropertyChangeTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
	//from MSwpChangeNotificationSubscriber
	void SwpChanged(TSsmSwp aSwp);
	
private:
	void doTestSwpChangeL();
	
private:
	RArray<TSsmSwp> iSwpChanges;
	};

#endif
