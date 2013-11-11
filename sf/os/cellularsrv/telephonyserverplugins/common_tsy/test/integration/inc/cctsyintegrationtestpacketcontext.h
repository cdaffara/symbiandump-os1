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
// Test step declarations for the PacketContext functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPACKETCONTEXT_H__
#define __CCTSYINTEGRATIONTESTPACKETCONTEXT_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tpacketservicetsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestPacketContextBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPacketContextBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPacketContextBase();

protected:

    void SetAttachModeToWhenPossibleAndWaitForAttachL(RPacketService& aPacketService);

	TInt AddPacketFilterV2ToContextL(RPacketContext& aPacketContext, TInt aId, TInt aErrCode = KErrNone);

    void FillPacketFilter(RPacketContext::TPacketFilterV2 &aFilter, TInt aId);

    template<class PARAMETERS,class CAPABILITIES>
      void FillParametersFromCapabilities(PARAMETERS &aParameters,const CAPABILITIES &aCapabilities);

    TBool CompareFilters(const RPacketContext::TPacketFilterV2 &aFilterOne,
                         const RPacketContext::TPacketFilterV2 &aFilterTwo);
    
	TBool ActivateSecondaryPacketContextL(RPacketContext& aSecondaryPacketContext, TPrimaryPacketContextId aPrimaryPacketContextId=KPrimaryPacketContext1, TSecondaryPacketContextId aSecondaryPacketContextId=KSecondaryPacketContext1);
    TUint FindMSB(TUint aNumber);
    TUint FindLSB(TUint aNumber);
    template<class QOSCAPS> void GetProfileCapsL(RPacketQoS& aContextQoS, QOSCAPS& aCapabilities);
    enum TQoSCapType
        {
		EQoSGPRS = 1,
		EQoSR4,
		EQoSR5	
        };
	
protected:
	
	TPacketServiceTsyTestHelper iPacketServiceTestHelper;
    TNetworkTsyTestHelper iNetworkTestHelper;

	}; // class CCTSYIntegrationTestPacketContextBase


class CCTSYIntegrationTestPacketContext0001 : public CCTSYIntegrationTestPacketContextBase
	{
public:

    CCTSYIntegrationTestPacketContext0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0001


class CCTSYIntegrationTestPacketContext0002 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0002


class CCTSYIntegrationTestPacketContext0003 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0003


class CCTSYIntegrationTestPacketContext0004 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0004


class CCTSYIntegrationTestPacketContext0005 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0005


class CCTSYIntegrationTestPacketContext0006 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0006


class CCTSYIntegrationTestPacketContext0007 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0007


class CCTSYIntegrationTestPacketContext0008 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0008


class CCTSYIntegrationTestPacketContext0009 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0009


class CCTSYIntegrationTestPacketContext0010 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0010


class CCTSYIntegrationTestPacketContext0011 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0011


class CCTSYIntegrationTestPacketContext0012 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0012


class CCTSYIntegrationTestPacketContext0013 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0013


class CCTSYIntegrationTestPacketContext0014 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0014


class CCTSYIntegrationTestPacketContext0015 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0015


class CCTSYIntegrationTestPacketContext0016 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0016


class CCTSYIntegrationTestPacketContext0017 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0017


class CCTSYIntegrationTestPacketContext0018 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0018


class CCTSYIntegrationTestPacketContext0019 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0019


class CCTSYIntegrationTestPacketContext0020 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0020


class CCTSYIntegrationTestPacketContext0021 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0021


class CCTSYIntegrationTestPacketContext0022 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0022


class CCTSYIntegrationTestPacketContext0023 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0023


class CCTSYIntegrationTestPacketContext0024 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0024


class CCTSYIntegrationTestPacketContext0025 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0025


class CCTSYIntegrationTestPacketContext0026 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0026(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0026();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0026


class CCTSYIntegrationTestPacketContext0027 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0027(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0027();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0027


class CCTSYIntegrationTestPacketContext0028 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0028(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0028();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

protected:

	void DoTest0028L(RPacketService& aPacketService, TPtrC aApn, TPtrC aUser, TPtrC aPwd, TInt aNWType);

private:

	}; // class CCTSYIntegrationTestPacketContext0028


class CCTSYIntegrationTestPacketContext0029 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0029(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0029();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0029


class CCTSYIntegrationTestPacketContext0030 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0030(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0030();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0030


class CCTSYIntegrationTestPacketContext0031 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0031(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0031();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0031


class CCTSYIntegrationTestPacketContext0032 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0032(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0032();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0032


class CCTSYIntegrationTestPacketContext0033 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0033(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0033();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0033


class CCTSYIntegrationTestPacketContext0034 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0034(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0034();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0034


class CCTSYIntegrationTestPacketContext0035 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0035(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0035();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0035


class CCTSYIntegrationTestPacketContext0036 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0036(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0036();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0036


class CCTSYIntegrationTestPacketContext0037 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0037(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0037();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0037


class CCTSYIntegrationTestPacketContext0038 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0038(CEtelSessionMgr& aEtelSessionMgr, const TDesC& aName);
	~CCTSYIntegrationTestPacketContext0038();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:
	template<class PARAMETERS, class CAPABILITIES, class NEGOTIATED>
		TBool ActiveContextL(RPacketContext &aPacketContext,
                            RPacketQoS &aPacketQoS,
                            PARAMETERS &aParameters,
                            CAPABILITIES &aCapabilities,
                            NEGOTIATED &aNegotiated);
	
	TQoSCapType iTestType;

	}; // class CCTSYIntegrationTestPacketContext0038


class CCTSYIntegrationTestPacketContext0039 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0039(CEtelSessionMgr& aEtelSessionMgr, const TDesC& aName);
	~CCTSYIntegrationTestPacketContext0039();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:
    template<class PARAMETERS,class CAPABILITIES,class NEGOTIATED>
      TBool ActivateAndModifyContextL(RPacketContext &aPacketContext,
                                     RPacketQoS &aPacketQoS,
                                     PARAMETERS &aParameters,
                                     CAPABILITIES &aCapabilities,
                                     NEGOTIATED &aNegotiated);

    TQoSCapType iTestType;

	}; // class CCTSYIntegrationTestPacketContext0039


class CCTSYIntegrationTestPacketContext0040 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0040(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0040();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0040


class CCTSYIntegrationTestPacketContext0041 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0041(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0041();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0041


class CCTSYIntegrationTestPacketContext0042 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0042(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0042();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0042


class CCTSYIntegrationTestPacketContext0043 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0043(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0043();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0043


class CCTSYIntegrationTestPacketContext0044 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0044(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0044();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0044


class CCTSYIntegrationTestPacketContext0045 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0045(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0045();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0045


class CCTSYIntegrationTestPacketContext0046 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0046(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0046();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0046


class CCTSYIntegrationTestPacketContext0047 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0047(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0047();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0047


class CCTSYIntegrationTestPacketContext0048 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0048(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0048();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0048


class CCTSYIntegrationTestPacketContext0049 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0049(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0049();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0049


class CCTSYIntegrationTestPacketContext0050 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0050(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0050();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0050


class CCTSYIntegrationTestPacketContext0051 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0051(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0051();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0051


class CCTSYIntegrationTestPacketContext0052 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0052(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0052();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0052


class CCTSYIntegrationTestPacketContext0053 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0053(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0053();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0053


class CCTSYIntegrationTestPacketContext0054 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0054(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0054();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0054


class CCTSYIntegrationTestPacketContext0055 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0055(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0055();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0055


class CCTSYIntegrationTestPacketContext0056 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0056(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0056();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0056


class CCTSYIntegrationTestPacketContext0057 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0057(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0057();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0057


class CCTSYIntegrationTestPacketContext0058 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0058(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0058();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

protected:

	void DoTest0058L(TPtrC& aApn, TPtrC& aUser, TPtrC& aPwd, TInt aQoSCapType);

private:

	}; // class CCTSYIntegrationTestPacketContext0058


class CCTSYIntegrationTestPacketContext0059 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0059(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0059();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

protected:

	void DoTest0059L(TPtrC& aApn, TPtrC& aUser, TPtrC& aPwd, TInt aQoSCapType);

private:

	}; // class CCTSYIntegrationTestPacketContext0059


class CCTSYIntegrationTestPacketContext0060 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0060(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0060();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

protected:

	void DoTest0060L(TPtrC& aApn, TPtrC& aUser, TPtrC& aPwd, TInt aQoSCapType);

private:

	}; // class CCTSYIntegrationTestPacketContext0060


class CCTSYIntegrationTestPacketContext0061 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0061(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0061();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0061


class CCTSYIntegrationTestPacketContext0062 : public CCTSYIntegrationTestPacketContextBase
	{
public:

	CCTSYIntegrationTestPacketContext0062(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketContext0062();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketContext0062



#endif // __CCTSYINTEGRATIONTESTPACKETCONTEXT_H__
