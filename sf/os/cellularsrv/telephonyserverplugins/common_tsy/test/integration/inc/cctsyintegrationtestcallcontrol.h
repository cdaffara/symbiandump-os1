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
// Test step declarations for the CallControl functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTCALLCONTROL_H__
#define __CCTSYINTEGRATIONTESTCALLCONTROL_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcallcontroltsytesthelper.h" 
#include "tnetworktsytesthelper.h"
#include "tpacketservicetsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestCallControlBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestCallControlBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestCallControlBase();
	
protected:
	
	TCallControlTsyTestHelper iCallControlTestHelper;
	TNetworkTsyTestHelper iNetworkTestHelper;
	TPacketServiceTsyTestHelper iPacketServiceTestHelper;
	
	}; // class CCTSYIntegrationTestCallControlBase


class CCTSYIntegrationTestCallControl0001 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0001


class CCTSYIntegrationTestCallControl0002 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0002


class CCTSYIntegrationTestCallControl0003 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0003


class CCTSYIntegrationTestCallControl0004 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0004


class CCTSYIntegrationTestCallControl0005 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0005


class CCTSYIntegrationTestCallControl0006 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0006


class CCTSYIntegrationTestCallControl0007 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0007


class CCTSYIntegrationTestCallControl0008 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0008


class CCTSYIntegrationTestCallControl0009 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0009


class CCTSYIntegrationTestCallControl0010 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0010


class CCTSYIntegrationTestCallControl0011 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0011


class CCTSYIntegrationTestCallControl0012 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0012


class CCTSYIntegrationTestCallControl0013 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0013


class CCTSYIntegrationTestCallControl0014 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0014


class CCTSYIntegrationTestCallControl0015 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0015


class CCTSYIntegrationTestCallControl0016 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0016


class CCTSYIntegrationTestCallControl0017 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0017


class CCTSYIntegrationTestCallControl0018 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0018


class CCTSYIntegrationTestCallControl0019 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0019


class CCTSYIntegrationTestCallControl0020 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0020


class CCTSYIntegrationTestCallControl0021 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0021


class CCTSYIntegrationTestCallControl0022 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0022


class CCTSYIntegrationTestCallControl0023 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0023


class CCTSYIntegrationTestCallControl0024 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0024


class CCTSYIntegrationTestCallControl0025 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0025


class CCTSYIntegrationTestCallControl0026 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0026(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0026();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0026


class CCTSYIntegrationTestCallControl0027 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0027(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0027();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0027


class CCTSYIntegrationTestCallControl0028 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0028(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0028();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0028


class CCTSYIntegrationTestCallControl0029 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0029(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0029();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0029


class CCTSYIntegrationTestCallControl0030 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0030(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0030();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0030


class CCTSYIntegrationTestCallControl0031 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0031(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0031();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0031


class CCTSYIntegrationTestCallControl0032 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0032(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0032();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0032


class CCTSYIntegrationTestCallControl0033 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0033(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0033();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0033


class CCTSYIntegrationTestCallControl0034 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0034(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0034();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0034


class CCTSYIntegrationTestCallControl0035 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0035(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0035();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0035


class CCTSYIntegrationTestCallControl0036 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0036(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0036();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:
		
	}; // class CCTSYIntegrationTestCallControl0036


class CCTSYIntegrationTestCallControl0037 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0037(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0037();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0037


class CCTSYIntegrationTestCallControl0038 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0038(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0038();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0038


class CCTSYIntegrationTestCallControl0039 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0039(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0039();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0039


class CCTSYIntegrationTestCallControl0040 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0040(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0040();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0040


class CCTSYIntegrationTestCallControl0041 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0041(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0041();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

protected:

	void AnswerIncomingCallCancelL(	RMobileLine& aVoiceLine, TInt aDelay);

private:

	}; // class CCTSYIntegrationTestCallControl0041


class CCTSYIntegrationTestCallControl0042 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0042(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0042();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0042


class CCTSYIntegrationTestCallControl0043 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0043(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0043();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0043


class CCTSYIntegrationTestCallControl0044 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0044(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0044();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0044


class CCTSYIntegrationTestCallControl0045 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0045(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0045();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0045


class CCTSYIntegrationTestCallControl0046 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0046(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0046();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0046


class CCTSYIntegrationTestCallControl0047 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0047(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0047();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0047


class CCTSYIntegrationTestCallControl0048 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0048(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0048();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0048


class CCTSYIntegrationTestCallControl0049 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0049(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0049();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0049


class CCTSYIntegrationTestCallControl0050 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0050(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0050();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0050


class CCTSYIntegrationTestCallControl0051 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0051(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0051();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0051


class CCTSYIntegrationTestCallControl0052 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0052(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0052();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0052


class CCTSYIntegrationTestCallControl0053 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0053(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0053();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0053


class CCTSYIntegrationTestCallControl0054 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0054(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0054();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0054


class CCTSYIntegrationTestCallControl0055 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0055(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0055();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0055


class CCTSYIntegrationTestCallControl0056 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0056(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0056();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0056


class CCTSYIntegrationTestCallControl0057 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0057(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0057();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0057


class CCTSYIntegrationTestCallControl0058 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0058(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0058();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0058

class CCTSYIntegrationTestCallControl0059 : public CCTSYIntegrationTestCallControlBase
	{
public:

	CCTSYIntegrationTestCallControl0059(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallControl0059();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallControl0059

class CCTSYIntegrationTestCallControl0060 : public CCTSYIntegrationTestCallControlBase
    {
public:

    CCTSYIntegrationTestCallControl0060(CEtelSessionMgr& aEtelSessionMgr);
    ~CCTSYIntegrationTestCallControl0060();
    virtual TVerdict doTestStepL();
    static TPtrC GetTestStepName();

private:

    }; // class CCTSYIntegrationTestCallControl0060

#endif // __CCTSYINTEGRATIONTESTCALLCONTROL_H__
