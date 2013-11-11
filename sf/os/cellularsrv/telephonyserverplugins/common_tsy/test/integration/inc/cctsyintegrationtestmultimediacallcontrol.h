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
// Test step declarations for the MultimediaCallControl functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTMULTIMEDIACALLCONTROL_H__
#define __CCTSYINTEGRATIONTESTMULTIMEDIACALLCONTROL_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"
#include "tcallcontroltsytesthelper.h" 

class CEtelSessionMgr;


class CCTSYIntegrationTestMultimediaCallControlBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControlBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestMultimediaCallControlBase();

protected:

		TNetworkTsyTestHelper iNetworkTestHelper;
		TCallControlTsyTestHelper iCallControlTestHelper;

	}; // class CCTSYIntegrationTestMultimediaCallControlBase


class CCTSYIntegrationTestMultimediaCallControl0001 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0001


class CCTSYIntegrationTestMultimediaCallControl0002 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0002


class CCTSYIntegrationTestMultimediaCallControl0003 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0003


class CCTSYIntegrationTestMultimediaCallControl0004 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0004


class CCTSYIntegrationTestMultimediaCallControl0005 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0005


class CCTSYIntegrationTestMultimediaCallControl0006 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0006


class CCTSYIntegrationTestMultimediaCallControl0007 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0007


class CCTSYIntegrationTestMultimediaCallControl0008 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0008


class CCTSYIntegrationTestMultimediaCallControl0009 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0009


class CCTSYIntegrationTestMultimediaCallControl0010 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0010


class CCTSYIntegrationTestMultimediaCallControl0011 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0011


class CCTSYIntegrationTestMultimediaCallControl0012 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0012


class CCTSYIntegrationTestMultimediaCallControl0013 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0013


class CCTSYIntegrationTestMultimediaCallControl0014 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0014


class CCTSYIntegrationTestMultimediaCallControl0015 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0015


class CCTSYIntegrationTestMultimediaCallControl0016 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0016


class CCTSYIntegrationTestMultimediaCallControl0017 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0017


class CCTSYIntegrationTestMultimediaCallControl0018 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0018


class CCTSYIntegrationTestMultimediaCallControl0019 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0019


class CCTSYIntegrationTestMultimediaCallControl0020 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0020


class CCTSYIntegrationTestMultimediaCallControl0021 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0021


class CCTSYIntegrationTestMultimediaCallControl0022 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0022


class CCTSYIntegrationTestMultimediaCallControl0023 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0023


class CCTSYIntegrationTestMultimediaCallControl0024 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0024


class CCTSYIntegrationTestMultimediaCallControl0025 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0025


class CCTSYIntegrationTestMultimediaCallControl0026 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0026(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0026();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0026


class CCTSYIntegrationTestMultimediaCallControl0027 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0027(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0027();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0027


class CCTSYIntegrationTestMultimediaCallControl0028 : public CCTSYIntegrationTestMultimediaCallControlBase
	{
public:

	CCTSYIntegrationTestMultimediaCallControl0028(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultimediaCallControl0028();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultimediaCallControl0028



#endif // __CCTSYINTEGRATIONTESTMULTIMEDIACALLCONTROL_H__
