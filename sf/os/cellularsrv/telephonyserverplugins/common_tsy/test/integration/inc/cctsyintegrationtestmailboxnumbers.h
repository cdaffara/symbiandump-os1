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
// Test step declarations for the MailboxNumbers functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTMAILBOXNUMBERS_H__
#define __CCTSYINTEGRATIONTESTMAILBOXNUMBERS_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestMailboxNumbersBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestMailboxNumbersBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestMailboxNumbersBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestMailboxNumbersBase


class CCTSYIntegrationTestMailboxNumbers0001 : public CCTSYIntegrationTestMailboxNumbersBase
	{
public:

	CCTSYIntegrationTestMailboxNumbers0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMailboxNumbers0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMailboxNumbers0001



#endif // __CCTSYINTEGRATIONTESTMAILBOXNUMBERS_H__
