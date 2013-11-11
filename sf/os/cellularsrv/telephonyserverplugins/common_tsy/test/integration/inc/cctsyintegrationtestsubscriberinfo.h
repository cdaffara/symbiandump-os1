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
// Test step declarations for the SubscriberInfo functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTSUBSCRIBERINFO_H__
#define __CCTSYINTEGRATIONTESTSUBSCRIBERINFO_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestSubscriberInfoBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestSubscriberInfoBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestSubscriberInfoBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestSubscriberInfoBase


class CCTSYIntegrationTestSubscriberInfo0001 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0001


class CCTSYIntegrationTestSubscriberInfo0002 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0002


class CCTSYIntegrationTestSubscriberInfo0003 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0003


class CCTSYIntegrationTestSubscriberInfo0004 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0004


class CCTSYIntegrationTestSubscriberInfo0005 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0005


class CCTSYIntegrationTestSubscriberInfo0006 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0006


class CCTSYIntegrationTestSubscriberInfo0007 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0007


class CCTSYIntegrationTestSubscriberInfo0008 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0008


class CCTSYIntegrationTestSubscriberInfo0009 : public CCTSYIntegrationTestSubscriberInfoBase
	{
public:

	CCTSYIntegrationTestSubscriberInfo0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSubscriberInfo0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSubscriberInfo0009



#endif // __CCTSYINTEGRATIONTESTSUBSCRIBERINFO_H__
