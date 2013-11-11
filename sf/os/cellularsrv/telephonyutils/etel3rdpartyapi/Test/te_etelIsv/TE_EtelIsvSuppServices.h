// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TE_ETELISVSUPPSERVICES_H_
#define __TE_ETELISVSUPPSERVICES_H_


class CTestGetSupplementaryServiceStatus : public CEtelIsvTestStep
	{
public:
	CTestGetSupplementaryServiceStatus() ;
	~CTestGetSupplementaryServiceStatus(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetSupplementaryServiceStatusAct : public CEtelIsvActBase
	{
public:	// functions
	enum TTestSupplServiceType
		{
		ETestSupplServiceCB,
		ETestSupplServiceCF,
		ETestSupplServiceCW,
		};
	static CTestGetSupplementaryServiceStatusAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetSupplementaryServiceStatusAct();
	void TestGetCFSupplementaryServiceStatus(CTelephony::TCallForwardingSupplServicesV1& aExpSupplementaryServiceStatus, CTelephony::TCallForwardingCondition aSSCondition, CTelephony::TServiceGroup aServiceGroup, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void TestGetCBSupplementaryServiceStatus(CTelephony::TCallBarringSupplServicesV1& aExpSupplementaryServiceStatus, CTelephony::TCallBarringCondition aSSCondition, CTelephony::TServiceGroup aServiceGroup, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void TestGetCWSupplementaryServiceStatus(CTelephony::TCallWaitingSupplServicesV1& aExpSupplementaryServiceStatus, CTelephony::TServiceGroup aServiceGroup, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetSupplementaryServiceStatusAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	TTestSupplServiceType iSupplServiceType;
	CTelephony::TCallBarringSupplServicesV1 iCallBarringStatus;
	CTelephony::TCallBarringSupplServicesV1Pckg iCallBarringStatusPckg;
	TPckgBuf<CTelephony::TCallBarringSupplServicesV1> iCallBarringStatusBuff;
	CTelephony::TCallBarringSupplServicesV1 iExpectedCallBarringStatus;
	
	CTelephony::TCallWaitingSupplServicesV1 iCallWaitingStatus;
	CTelephony::TCallWaitingSupplServicesV1Pckg iCallWaitingStatusPckg;
	TPckgBuf<CTelephony::TCallWaitingSupplServicesV1> iCallWaitingStatusBuff;
	CTelephony::TCallWaitingSupplServicesV1 iExpectedCallWaitingStatus;
	
	CTelephony::TCallForwardingSupplServicesV1 iCallForwardingStatus;
	CTelephony::TCallForwardingSupplServicesV1Pckg iCallForwardingStatusPckg;
	TPckgBuf<CTelephony::TCallForwardingSupplServicesV1> iCallForwardingStatusBuff;
	CTelephony::TCallForwardingSupplServicesV1 iExpectedCallForwardingStatus;
	CTelephony::TServiceGroup iServiceGroup;
	};


class CTestGetIdentityServiceStatus : public CEtelIsvTestStep
	{
public:
	CTestGetIdentityServiceStatus() ;
	~CTestGetIdentityServiceStatus(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetIdentityServiceStatusAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetIdentityServiceStatusAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetIdentityServiceStatusAct();
	void TestGetIdentityServiceStatus( CTelephony::TIdentityServiceV1& aIdentityServiceStatus, CTelephony::TIdentityService aIdentityServiceType, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetIdentityServiceStatusAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TIdentityService iIdentityServiceType;
	CTelephony::TIdentityServiceV1 iIdentityServiceStatus;
	CTelephony::TIdentityServiceV1 iExpectedIdentityServiceStatus;
	
	CTelephony::TIdentityServiceV1Pckg iIdentityServiceStatusPckg;
	TPckgBuf<CTelephony::TIdentityServiceV1> iIdentityServiceStatusBuff;
	};

class CTestSuppServFuncFlightModeOnWithPublishSubscribe : public CEtelIsvTestStep
	{	
public:
	CTestSuppServFuncFlightModeOnWithPublishSubscribe() ;
	~CTestSuppServFuncFlightModeOnWithPublishSubscribe(){} ;
	virtual enum TVerdict doTestStepL();
	};


#endif
