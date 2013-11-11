// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//  Test steps
//

/**
 @file
 @internalComponent
*/
 
#ifndef __MBUFGOBBLER_TEST_H__
#define __MBUFGOBBLER_TEST_H__

#include <test/testexecutestepbase.h>
#include "te_mbufgobblerserver.h"

class CMbufGobblerStartupTest : public CTestStep
	{
public:
	CMbufGobblerStartupTest(CMbufGobblerTestServer* aOwner);
	~CMbufGobblerStartupTest();
	virtual TVerdict doTestStepL();
	};

_LIT(KTestMbufGobblerStartupTest,"MbufGobblerStartupTest");

#endif // __MBUFGOBBLER_TEST_H__
