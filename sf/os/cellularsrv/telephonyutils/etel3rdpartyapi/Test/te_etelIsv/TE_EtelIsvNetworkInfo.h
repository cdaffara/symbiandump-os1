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
// ETelISV test's header file, declaring classes for Network Information
// retrieval.
// 
//

/**
 @file
*/


#ifndef __TE_ETELISVNETWORKINFO_H_
#define __TE_ETELISVNETWORKINFO_H_

class CTestGetCurrentNetworkName : public CEtelIsvTestStep
	{
public:
	CTestGetCurrentNetworkName() ;
	~CTestGetCurrentNetworkName(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetCurrentNetworkNameAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetCurrentNetworkNameAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetCurrentNetworkNameAct();
	void TestGetCurrentNetworkName(CTelephony::TNetworkNameV1 aExpectedNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void RunL();
protected:
	void DoCancel();
	CTestGetCurrentNetworkNameAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TNetworkNameV1 iNetworkName;
	CTelephony::TNetworkNameV1 iExpectedNetworkName;
	CTelephony::TNetworkNameV1Pckg iNetworkNamePckg;
	TPckgBuf<CTelephony::TNetworkNameV1> iNetworkNameBuff;
	};

class CTestGetNetworkRegistrationStatus : public CEtelIsvTestStep
	{
public:
	CTestGetNetworkRegistrationStatus() ;
	~CTestGetNetworkRegistrationStatus(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetNetworkRegistrationAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetNetworkRegistrationAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetNetworkRegistrationAct();
	void TestGetNetworkRegistration(CTelephony::TNetworkRegistrationV1 aExpectedNetworkRegistration, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetNetworkRegistrationAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TNetworkRegistrationV1 iNetworkRegistration;
	CTelephony::TNetworkRegistrationV1 iExpectedNetworkRegistration;
	
	CTelephony::TNetworkRegistrationV1Pckg iNetworkRegistrationPckg;
	TPckgBuf<CTelephony::TNetworkRegistrationV1> iNetworkRegistrationBuff;
	};


class CTestGetCurrentNetworkInfo : public CEtelIsvTestStep
	{
public:
	CTestGetCurrentNetworkInfo() ;
	~CTestGetCurrentNetworkInfo(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetCurrentNetworkInfoAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetCurrentNetworkInfoAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros = ETrue);
	void ConstructL();
	~CTestGetCurrentNetworkInfoAct();
	void TestGetCurrentNetworkInfo(const CTelephony::TNetworkInfoV1& aExpectedNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetCurrentNetworkInfoAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros);
public:
	CTelephony::TNetworkInfoV1 iNetworkInfo;
private:	// data	
	CTelephony::TNetworkInfoV1 iExpectedNetworkInfo;
	TBool iUseMacros;
	CTelephony::TNetworkInfoV1Pckg iNetworkInfoPckg;
	TPckgBuf<CTelephony::TNetworkInfoV1> iNetworkInfoBuff;
	};


 
class CTestGetOperatorName : public CEtelIsvTestStep
	{
public:
	CTestGetOperatorName() ;
	~CTestGetOperatorName(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetOperatorNameAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetOperatorNameAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetOperatorNameAct();
	void TestGetOperatorName(CTelephony::TOperatorNameV1 aExpectedOperatorName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetOperatorNameAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TOperatorNameV1 iOperatorName;
	CTelephony::TOperatorNameV1 iExpectedOperatorName;
	
	CTelephony::TOperatorNameV1Pckg iOperatorNamePckg;
	TPckgBuf<CTelephony::TOperatorNameV1> iOperatorNameBuff;
	};
	
class CTestNetworkFuncFlightModeOnWithPublishSubscribe : public CEtelIsvTestStep
	{
public:
	CTestNetworkFuncFlightModeOnWithPublishSubscribe() ;
	~CTestNetworkFuncFlightModeOnWithPublishSubscribe(){} ;
	virtual enum TVerdict doTestStepL();
	};



/**
This class tests three ETel3rdParty methods GetCurrentNetworkInfo,
GetOperator and GetCurrentNetworkName when the network information
and network names are stored in TNetworkInfoV1 and TNetworkNameV1
respectively and the long network name is eighteen characters long.
The network name should be truncated to the first sixteen characters.
*/
class CTestGetCurrentNetworkInfoEighteenChars : public CEtelIsvTestStep
	{
public:
	CTestGetCurrentNetworkInfoEighteenChars() ;
	~CTestGetCurrentNetworkInfoEighteenChars(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetCurrentNetworkInfoActEighteenChars : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetCurrentNetworkInfoActEighteenChars* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros = ETrue);
	void ConstructL();
	~CTestGetCurrentNetworkInfoActEighteenChars();
	void TestGetCurrentNetworkInfoEighteenChars(const CTelephony::TNetworkInfoV1& aExpectedNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
	void SetTestStage(TInt aTestStage);
private:
	TBool iUseMacros;
	/**
	The testStage that is currently being executed.  When testStage=1, 
	tests GetCurrentNetworkInfo, when testStage=2, tests GetOperator, 
	when testStage=3 tests GetCurrentNetworkName.
	*/
	TInt iTestStage;

protected:
	CTestGetCurrentNetworkInfoActEighteenChars(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros);
private:	// data	
	CTelephony::TNetworkInfoV1 iNetworkInfo;
	CTelephony::TNetworkInfoV1 iExpectedNetworkInfo;
	
// To test GetOperatorName
public:
	void TestGetOperatorNameSixteenChars(CTelephony::TOperatorNameV1 aExpectedOperatorName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);	
private:	// data
	CTelephony::TOperatorNameV1 iOperatorName;
	CTelephony::TOperatorNameV1 iExpectedOperatorName;
	
// To test GetCurrentNetworkName
public:
	void TestGetCurrentNetworkNameEighteenChars(CTelephony::TNetworkNameV1 aExpectedNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
private:	// data
	CTelephony::TNetworkNameV1 iNetworkName;
	CTelephony::TNetworkNameV1 iExpectedNetworkName;
	};


/**
This class tests three ETel3rdParty methods GetCurrentNetworkInfo,
GetOperator and GetCurrentNetworkName when the network information
and network names are stored in TNetworkInfoV1 and TNetworkNameV1
respectively and the long network name is 23 characters long.
The network name should be truncated to the first sixteen characters.
*/
class CTestGetCurrentNetworkInfoTwentyThreeChars : public CEtelIsvTestStep
	{
public:
	CTestGetCurrentNetworkInfoTwentyThreeChars() ;
	~CTestGetCurrentNetworkInfoTwentyThreeChars(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetCurrentNetworkInfoActTwentyThreeChars : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetCurrentNetworkInfoActTwentyThreeChars* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros = ETrue);
	void ConstructL();
	~CTestGetCurrentNetworkInfoActTwentyThreeChars();
	void TestGetCurrentNetworkInfoTwentyThreeChars(const CTelephony::TNetworkInfoV1& aExpectedNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
	void SetTestStage(TInt aTestStage); // testStage=1 GetCurrentNetworkInfo, testStage=2 GetOperator, testStage=3 GetNetworkName, testStage=4 GetCurrentNetworkInfo using TNetworkInfoV2, testStage=5 GetNetworkName using TNetworkNameV2
protected:
	CTestGetCurrentNetworkInfoActTwentyThreeChars(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros);
private:	// data	
	CTelephony::TNetworkInfoV1 iNetworkInfo;
	CTelephony::TNetworkInfoV1 iExpectedNetworkInfo;
	TBool iUseMacros;
	TInt iTestStage; // testStage=1 GetCurrentNetworkInfo, testStage=2 GetOperator, testStage=3 GetNetworkName
	
// To test GetOperatorName
public:
	void TestGetOperatorNameSixteenChars(CTelephony::TOperatorNameV1 aExpectedOperatorName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);	
private:	// data
	CTelephony::TOperatorNameV1 iOperatorName;
	CTelephony::TOperatorNameV1 iExpectedOperatorName;
	
// To test GetCurrentNetworkName
public:
	void TestGetCurrentNetworkNameTwentyThreeChars(CTelephony::TNetworkNameV1 aExpectedNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
private:	// data
	CTelephony::TNetworkNameV1 iNetworkName;
	CTelephony::TNetworkNameV1 iExpectedNetworkName;

// To test GetCurrentNetworkInfo using TNetworkInfoV2 type
public:
	void TestGetCurrentNetworkInfoTwentyThreeCharsV2(CTelephony::TNetworkInfoV2 aExpectedNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
private:	// data	
	CTelephony::TNetworkInfoV2 iNetworkInfoV2;
	CTelephony::TNetworkInfoV2 iExpectedNetworkInfoV2;

// To test GetCurrentNetworkName using TNetworkNameV2 type
public:
	void TestGetCurrentNetworkNameTwentyThreeCharsV2(const CTelephony::TNetworkNameV2& aExpectedNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
private:	// data
	CTelephony::TNetworkNameV2 iNetworkNameV2;
	CTelephony::TNetworkNameV2 iExpectedNetworkNameV2;

	};


/**
This class tests two ETel3rdParty methods GetCurrentNetworkInfo and 
GetCurrentNetworkName when the network information
and network names are stored in TNetworkInfoV2 and TNetworkNameV2
respectively and the long network name is eighteen characters long.
The network name should be truncated to the first 20 characters.
*/
class CTestGetCurrentNetworkInfoEighteenCharsV2 : public CEtelIsvTestStep
	{
public:
	CTestGetCurrentNetworkInfoEighteenCharsV2() ;
	~CTestGetCurrentNetworkInfoEighteenCharsV2(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetCurrentNetworkInfoActEighteenCharsV2 : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetCurrentNetworkInfoActEighteenCharsV2* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros = ETrue);
	void ConstructL();
	~CTestGetCurrentNetworkInfoActEighteenCharsV2();
	void DoCancel();
	void RunL();
	void SetTestStage(TInt aTestStage);
protected:
	CTestGetCurrentNetworkInfoActEighteenCharsV2(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros);
private:
	TBool iUseMacros;
	/**
	The testStage that is currently being executed.  When testStage=1, 
	tests GetCurrentNetworkInfo, when testStage=2, tests 
	GetCurrentNetworkName.
	*/
	TInt iTestStage;

// To test GetCurrentNetworkInfo using TNetworkInfoV2 type
public:
	void TestGetCurrentNetworkInfoEighteenCharsV2(const CTelephony::TNetworkInfoV2& aExpectedNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
private:	// data	
	CTelephony::TNetworkInfoV2 iNetworkInfoV2;
	CTelephony::TNetworkInfoV2 iExpectedNetworkInfoV2;

// To test GetCurrentNetworkName using TNetworkNameV2 type
public:
	void TestGetCurrentNetworkNameEighteenCharsV2(CTelephony::TNetworkNameV2 aExpectedNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
private:	// data
	CTelephony::TNetworkNameV2 iNetworkNameV2;
	CTelephony::TNetworkNameV2 iExpectedNetworkNameV2;
	};


/**
This class tests two ETel3rdParty methods GetCurrentNetworkInfo and 
GetCurrentNetworkName when the network information
and network names are stored in TNetworkInfoV2 and TNetworkNameV2
respectively and the long network name is 23 characters long.
The network name should be truncated to the first 20 characters.
*/
class CTestGetCurrentNetworkInfoTwentyThreeCharsV2 : public CEtelIsvTestStep
	{
public:
	CTestGetCurrentNetworkInfoTwentyThreeCharsV2() ;
	~CTestGetCurrentNetworkInfoTwentyThreeCharsV2(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestGetCurrentNetworkInfoActTwentyThreeCharsV2 : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetCurrentNetworkInfoActTwentyThreeCharsV2* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros = ETrue);
	void ConstructL();
	~CTestGetCurrentNetworkInfoActTwentyThreeCharsV2();
	void DoCancel();
	void RunL();
	void SetTestStage(TInt aTestStage); // testStage=1 GetCurrentNetworkInfo, testStage=2 GetOperator, testStage=3 GetNetworkName, testStage=4 GetCurrentNetworkInfo using TNetworkInfoV2, testStage=5 GetNetworkName using TNetworkNameV2
protected:
	CTestGetCurrentNetworkInfoActTwentyThreeCharsV2(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros);
private:	// data	
	TBool iUseMacros;
	/**
	The testStage that is currently being executed.  When testStage=1, 
	tests GetCurrentNetworkInfo, when testStage=2, tests 
	GetCurrentNetworkName.
	*/
	TInt iTestStage; 

// To test GetCurrentNetworkInfo using TNetworkInfoV2 type
public:
	void TestGetCurrentNetworkInfoTwentyThreeCharsV2(const CTelephony::TNetworkInfoV2& aExpectedNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
private:	// data	
	CTelephony::TNetworkInfoV2 iNetworkInfoV2;
	CTelephony::TNetworkInfoV2 iExpectedNetworkInfoV2;

// To test GetCurrentNetworkName using TNetworkNameV2 type
public:
	void TestGetCurrentNetworkNameTwentyThreeCharsV2(const CTelephony::TNetworkNameV2& aExpectedNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
private:	// data
	CTelephony::TNetworkNameV2 iNetworkNameV2;
	CTelephony::TNetworkNameV2 iExpectedNetworkNameV2;

	};

#endif // __TE_ETELISVNETWORKINFO_H_
