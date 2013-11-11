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


#ifndef __TE_ETELISVSTRESS_H_
#define __TE_ETELISVSTRESS_H_

enum TApiToTest
	{
	EGetPhoneIdAPI,
	EGetSignalStrengthAPI,	
	EDialNewCallAPI,
	EGetNetworkInfoAPI
	};
struct TTestData
	{
	TBool iTestSuccess;
	TInt iClientId;
	TInt iRepCount;	
	CEtelIsvTestStep* iTestStep;
	TApiToTest iApiToTest;	
	};

class CTestStress : public CEtelIsvTestStep	//Stress Tests
	{
public:
	CTestStress(TInt& aCallCount) ;
	~CTestStress(){} ;
	virtual enum TVerdict doTestStepL();
	static TInt StartThreadFunction(TAny* aThreadData);	
	
	static TInt GetPhoneIdThreadFunctionL(TTestData* aTestData);
	static TInt GetSignalStrengthThreadFunctionL(TTestData* aTestData);
	static TInt MakeCallHangupThreadFunctionL(TTestData* aTestData);
	static TInt GetCurrentNetworkInfoThreadFunctionL(TTestData* aTestData);
	
	void RunTestL(TTestData& aTestData1, TTestData& aTestData2, const TDes& aTestName1, const TDes& aTestName2);
private:
	TInt& iCallCount;
	};
	
class CTestFailedOpen : public CEtelIsvTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};
	
#endif
