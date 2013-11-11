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
// Test step declarations for the ConferenceCallControl functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTCONFERENCECALLCONTROL_H__
#define __CCTSYINTEGRATIONTESTCONFERENCECALLCONTROL_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcallcontroltsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestConferenceCallControlBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestConferenceCallControlBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestConferenceCallControlBase();

protected:

	RMobileCall::TMobileCallStatus MakeCallL( RMobileCall& aCall, TPtrC& aNumber );
	RMobileCall::TMobileCallStatus HoldCallL( RMobileCall& aCall );
	void AddCallToConferenceL( RMobileConferenceCall& aConfCall, RMobileCall& aCall, TPtrC& aNumber, TInt index );
	void CheckConfCallInfo( RMobileConferenceCall& aConfCall, TPtrC& aNumber, TInt aCallCount );
	
protected:

	TCallControlTsyTestHelper iCallControlTsyTestHelper;
	TNetworkTsyTestHelper iNetworkTsyTestHelper;

	}; // class CCTSYIntegrationTestConferenceCallControlBase


class CCTSYIntegrationTestConferenceCallControl0001 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0001


class CCTSYIntegrationTestConferenceCallControl0002 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0002


class CCTSYIntegrationTestConferenceCallControl0003 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0003


class CCTSYIntegrationTestConferenceCallControl0004 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0004


class CCTSYIntegrationTestConferenceCallControl0005 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0005


class CCTSYIntegrationTestConferenceCallControl0006 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0006


class CCTSYIntegrationTestConferenceCallControl0007 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0007


class CCTSYIntegrationTestConferenceCallControl0008 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0008


class CCTSYIntegrationTestConferenceCallControl0009 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0009


class CCTSYIntegrationTestConferenceCallControl0010 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0010


class CCTSYIntegrationTestConferenceCallControl0011 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0011


class CCTSYIntegrationTestConferenceCallControl0012 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0012


class CCTSYIntegrationTestConferenceCallControl0013 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	void AddCallTestStepL( RMobileConferenceCall& aConfCall, 
													RMobileCall& aCall, 
													TPtrC& aNumber,
													TInt aCallCount );
																										

	}; // class CCTSYIntegrationTestConferenceCallControl0013


class CCTSYIntegrationTestConferenceCallControl0014 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0014


class CCTSYIntegrationTestConferenceCallControl0015 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0015


class CCTSYIntegrationTestConferenceCallControl0016 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0016


class CCTSYIntegrationTestConferenceCallControl0017 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0017


class CCTSYIntegrationTestConferenceCallControl0018 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0018


class CCTSYIntegrationTestConferenceCallControl0019 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0019


class CCTSYIntegrationTestConferenceCallControl0020 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0020


class CCTSYIntegrationTestConferenceCallControl0021 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0021


class CCTSYIntegrationTestConferenceCallControl0022 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0022


class CCTSYIntegrationTestConferenceCallControl0023 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0023


class CCTSYIntegrationTestConferenceCallControl0024 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0024


class CCTSYIntegrationTestConferenceCallControl0025 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0025


class CCTSYIntegrationTestConferenceCallControl0026 : public CCTSYIntegrationTestConferenceCallControlBase
	{
public:

	CCTSYIntegrationTestConferenceCallControl0026(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestConferenceCallControl0026();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestConferenceCallControl0026



#endif // __CCTSYINTEGRATIONTESTCONFERENCECALLCONTROL_H__
