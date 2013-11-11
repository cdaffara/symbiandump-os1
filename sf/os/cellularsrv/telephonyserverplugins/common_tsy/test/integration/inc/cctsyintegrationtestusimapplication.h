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
 @file	Test step declarations for the USimApplication functional unit.
 @internalTechnology
*/

#ifndef __CCTSYINTEGRATIONTESTUSIMAPPLICATION_H__
#define __CCTSYINTEGRATIONTESTUSIMAPPLICATION_H__

#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcallcontroltsytesthelper.h"
#include "tsupplementaltsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;

class CCTSYIntegrationTestUSimApplicationBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestUSimApplicationBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestUSimApplicationBase();
	
	}; // class CCTSYIntegrationTestUSimApplicationBase

class CCTSYIntegrationTestUSimApplication0001 : public CCTSYIntegrationTestUSimApplicationBase
	{
public:

	CCTSYIntegrationTestUSimApplication0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUSimApplication0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUSimApplication0001

#endif // __CCTSYINTEGRATIONTESTUSIMAPPLICATION_H__
