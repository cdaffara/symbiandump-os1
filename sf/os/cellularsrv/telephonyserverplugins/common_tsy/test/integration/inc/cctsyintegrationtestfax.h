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
// Test step declarations for the Fax functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTFAX_H__
#define __CCTSYINTEGRATIONTESTFAX_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestFaxBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestFaxBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestFaxBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestFaxBase


class CCTSYIntegrationTestFax0001 : public CCTSYIntegrationTestFaxBase
	{
public:

	CCTSYIntegrationTestFax0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestFax0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestFax0001


class CCTSYIntegrationTestFax0002 : public CCTSYIntegrationTestFaxBase
	{
public:

	CCTSYIntegrationTestFax0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestFax0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestFax0002



#endif // __CCTSYINTEGRATIONTESTFAX_H__
