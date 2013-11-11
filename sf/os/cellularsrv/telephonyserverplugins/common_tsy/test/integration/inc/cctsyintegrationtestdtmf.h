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
// Test step declarations for the DTMF functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTDTMF_H__
#define __CCTSYINTEGRATIONTESTDTMF_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcallcontroltsytesthelper.h"
#include "tnetworktsytesthelper.h"


class CEtelSessionMgr;


class CCTSYIntegrationTestDTMFBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestDTMFBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestDTMFBase();

protected:

	void AddCallToConferenceL( RMobileConferenceCall& aConfCall, RMobileCall& aCall, TPtrC& aNumber, TInt index );

	// Declare a TSY test helper here if required
	
	TCallControlTsyTestHelper iCallControlTestHelper;
	TNetworkTsyTestHelper iNetworkTestHelper;
	
	}; // class CCTSYIntegrationTestDTMFBase


class CCTSYIntegrationTestDTMF0001 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0001


class CCTSYIntegrationTestDTMF0002 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0002


class CCTSYIntegrationTestDTMF0003 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0003


class CCTSYIntegrationTestDTMF0004 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0004


class CCTSYIntegrationTestDTMF0005 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0005


class CCTSYIntegrationTestDTMF0006 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0006


class CCTSYIntegrationTestDTMF0007 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0007


class CCTSYIntegrationTestDTMF0008 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0008


class CCTSYIntegrationTestDTMF0009 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0009


class CCTSYIntegrationTestDTMF0010 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDTMF0010


class CCTSYIntegrationTestDTMF0011 : public CCTSYIntegrationTestDTMFBase
	{
public:

	CCTSYIntegrationTestDTMF0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDTMF0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	void CCTSYIntegrationTestDTMF0011::AddCallToConferenceL(RMobileConferenceCall& aConfCall, 
															RMobileCall& aCall,
															TPtrC& aNumber,
															TInt index);

	}; // class CCTSYIntegrationTestDTMF0011



#endif // __CCTSYINTEGRATIONTESTDTMF_H__
