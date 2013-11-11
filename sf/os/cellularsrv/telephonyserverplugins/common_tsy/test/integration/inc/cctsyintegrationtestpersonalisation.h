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
// Test step declarations for the Personalisation functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPERSONALISATION_H__
#define __CCTSYINTEGRATIONTESTPERSONALISATION_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestPersonalisationBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPersonalisationBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPersonalisationBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestPersonalisationBase


class CCTSYIntegrationTestPersonalisation0001 : public CCTSYIntegrationTestPersonalisationBase
	{
public:

	CCTSYIntegrationTestPersonalisation0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPersonalisation0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPersonalisation0001



#endif // __CCTSYINTEGRATIONTESTPERSONALISATION_H__
