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

#if (!defined __TAMASTARTER_STEP_CANCEL_H__)
#define __TAMASTARTER_STEP_CANCEL_H__

#include <startupproperties.h>
#include <startup.hrh>
#include <dscstoredefs.h>
#include <dscstore.h>
#include <dscitem.h>
#include <sysmonclisess.h>

#include <test/testexecutestepbase.h>
#include <cinidata.h>

class CAppfwkAmaStarterTestStepCancel : public CTestStep
	{
public:	
	CAppfwkAmaStarterTestStepCancel(const TDesC& aStepName);
	virtual ~CAppfwkAmaStarterTestStepCancel();
	
	//from CTestStep	
	TVerdict doTestStepL();	
	
private:
	void TestCase1L();
	void TestCase2L();
	};

_LIT(KAppfwkAmaStarterCancel,"AmaStarterCancel");

#endif


