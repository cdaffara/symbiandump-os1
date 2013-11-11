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

#if (!defined __TAMASTARTER_STEP_STARTPENDING_H__)
#define __TAMASTARTER_STEP_STARTPENDING_H__

#include <startupproperties.h>
#include <startup.hrh>
#include <dscstoredefs.h>
#include <dscstore.h>
#include <dscitem.h>
#include <sysmonclisess.h>

#include <test/testexecutestepbase.h>
#include <cinidata.h>

/**
CAppfwkAmaStarterTestStepStartLAsync encapsulates the functionality required 
when test AMAs are launched and compare it against the expected result file.
*/
class CAppfwkAmaStarterTestStepStartPending : public CTestStep
	{
public:	
	CAppfwkAmaStarterTestStepStartPending(const TDesC& aStepName);
	virtual ~CAppfwkAmaStarterTestStepStartPending();
	
	//from CTestStep	
	TVerdict doTestStepL();	

private:
	void TestCase1L();

	};

_LIT(KAppfwkAmaStarterStartPending,"AmaStarterStartPending");

#endif


