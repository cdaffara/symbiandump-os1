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
// Test step declarations for the CustomAPI functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTCUSTOMAPI_H__
#define __CCTSYINTEGRATIONTESTCUSTOMAPI_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcustomapitsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestCustomAPIBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestCustomAPIBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestCustomAPIBase();

protected:

	// Declare a TSY test helper here if required
    TCustomApiTsyTestHelper iCustomApiTsyTestHelper;

	}; // class CCTSYIntegrationTestCustomAPIBase


class CCTSYIntegrationTestCustomAPI0001 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0001


class CCTSYIntegrationTestCustomAPI0002 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0002


class CCTSYIntegrationTestCustomAPI0003 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0003


class CCTSYIntegrationTestCustomAPI0004 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0004


class CCTSYIntegrationTestCustomAPI0005 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0005


class CCTSYIntegrationTestCustomAPI0006 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0006


class CCTSYIntegrationTestCustomAPI0007 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0007


class CCTSYIntegrationTestCustomAPI0008 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0008


class CCTSYIntegrationTestCustomAPI0009 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0009


class CCTSYIntegrationTestCustomAPI0010 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0010


class CCTSYIntegrationTestCustomAPI0011 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0011


class CCTSYIntegrationTestCustomAPI0012 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0012


class CCTSYIntegrationTestCustomAPI0013 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0013


class CCTSYIntegrationTestCustomAPI0014 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0014


class CCTSYIntegrationTestCustomAPI0015 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0015


class CCTSYIntegrationTestCustomAPI0016 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0016


class CCTSYIntegrationTestCustomAPI0017 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0017


class CCTSYIntegrationTestCustomAPI0018 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0018


class CCTSYIntegrationTestCustomAPI0019 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0019


class CCTSYIntegrationTestCustomAPI0020 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0020


class CCTSYIntegrationTestCustomAPI0021 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0021


class CCTSYIntegrationTestCustomAPI0022 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0022


class CCTSYIntegrationTestCustomAPI0023 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0023


class CCTSYIntegrationTestCustomAPI0024 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0024


class CCTSYIntegrationTestCustomAPI0025 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0025


class CCTSYIntegrationTestCustomAPI0026 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0026(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0026();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0026


class CCTSYIntegrationTestCustomAPI0027 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0027(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0027();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0027


class CCTSYIntegrationTestCustomAPI0028 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0028(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0028();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0028


class CCTSYIntegrationTestCustomAPI0029 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0029(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0029();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0029


class CCTSYIntegrationTestCustomAPI0030 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0030(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0030();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0030


class CCTSYIntegrationTestCustomAPI0031 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0031(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0031();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0031


class CCTSYIntegrationTestCustomAPI0032 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0032(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0032();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0032


class CCTSYIntegrationTestCustomAPI0033 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0033(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0033();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0033


class CCTSYIntegrationTestCustomAPI0034 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0034(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0034();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0034


class CCTSYIntegrationTestCustomAPI0035 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0035(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0035();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0035


class CCTSYIntegrationTestCustomAPI0036 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0036(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0036();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0036


class CCTSYIntegrationTestCustomAPI0037 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0037(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0037();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0037


class CCTSYIntegrationTestCustomAPI0038 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0038(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0038();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0038


class CCTSYIntegrationTestCustomAPI0039 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0039(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0039();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0039


class CCTSYIntegrationTestCustomAPI0040 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0040(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0040();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0040


class CCTSYIntegrationTestCustomAPI0041 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0041(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0041();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0041


class CCTSYIntegrationTestCustomAPI0042 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0042(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0042();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0042


class CCTSYIntegrationTestCustomAPI0043 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0043(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0043();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0043


class CCTSYIntegrationTestCustomAPI0044 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0044(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0044();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0044


class CCTSYIntegrationTestCustomAPI0045 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0045(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0045();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0045


class CCTSYIntegrationTestCustomAPI0046 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0046(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0046();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0046


class CCTSYIntegrationTestCustomAPI0047 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0047(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0047();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0047


class CCTSYIntegrationTestCustomAPI0048 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0048(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0048();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0048


class CCTSYIntegrationTestCustomAPI0049 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0049(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0049();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0049


class CCTSYIntegrationTestCustomAPI0050 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0050(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0050();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0050


class CCTSYIntegrationTestCustomAPI0051 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0051(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0051();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0051


class CCTSYIntegrationTestCustomAPI0052 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0052(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0052();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0052


class CCTSYIntegrationTestCustomAPI0053 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0053(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0053();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0053


class CCTSYIntegrationTestCustomAPI0054 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0054(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0054();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0054


class CCTSYIntegrationTestCustomAPI0055 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0055(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0055();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0055


class CCTSYIntegrationTestCustomAPI0056 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0056(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0056();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0056


class CCTSYIntegrationTestCustomAPI0057 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0057(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0057();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0057


class CCTSYIntegrationTestCustomAPI0058 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0058(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0058();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0058


class CCTSYIntegrationTestCustomAPI0059 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0059(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0059();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0059


class CCTSYIntegrationTestCustomAPI0060 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0060(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0060();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0060


class CCTSYIntegrationTestCustomAPI0061 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0061(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0061();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0061


class CCTSYIntegrationTestCustomAPI0062 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0062(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0062();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0062


class CCTSYIntegrationTestCustomAPI0063 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0063(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0063();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0063


class CCTSYIntegrationTestCustomAPI0064 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0064(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0064();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0064


class CCTSYIntegrationTestCustomAPI0065 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0065(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0065();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0065


class CCTSYIntegrationTestCustomAPI0066 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0066(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0066();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0066


class CCTSYIntegrationTestCustomAPI0067 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0067(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0067();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0067


class CCTSYIntegrationTestCustomAPI0068 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0068(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0068();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0068


class CCTSYIntegrationTestCustomAPI0069 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0069(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0069();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0069


class CCTSYIntegrationTestCustomAPI0070 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0070(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0070();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0070


class CCTSYIntegrationTestCustomAPI0071 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0071(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0071();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0071


class CCTSYIntegrationTestCustomAPI0072 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0072(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0072();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0072


class CCTSYIntegrationTestCustomAPI0073 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0073(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0073();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0073


class CCTSYIntegrationTestCustomAPI0074 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0074(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0074();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0074


class CCTSYIntegrationTestCustomAPI0075 : public CCTSYIntegrationTestCustomAPIBase
	{
public:

	CCTSYIntegrationTestCustomAPI0075(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCustomAPI0075();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCustomAPI0075



#endif // __CCTSYINTEGRATIONTESTCUSTOMAPI_H__
