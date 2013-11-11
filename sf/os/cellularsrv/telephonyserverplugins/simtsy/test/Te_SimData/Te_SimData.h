
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_SimData_H__
#define TE_SimData_H__

#include "Te_SimDataTestStepBase.h"

#define SIM_TSY

#define CHECK_POINT(code,expected,msg){\
	TInt ret(0);\
	ret = (code)==(expected);\
	TEST(ret);\
	if (ret ) INFO_PRINTF1(msg);\
	}

class CTestSuite ;
const TInt KLocalTel=32;
const TInt KRemoteTel=32;


/**
 * Test class that enables tests.
 * It's a base class for all test steps.
 */
class CTestSimDataStep : public CSimDataTestStepBase
	{
protected:
	//Utils
	void GetGoodNumber(TDes& aNum) ;
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds) ;
	TInt CallMeDear() ;
	TInt HangMeUpDear() ;
	void Print_RCall_TStatus(RCall::TStatus& aArg) ;
	void Print_TCapsEnum(RCall::TCaps& aArg) ;
	void TestLineCapabilitiesL() ;
	void Print_TMobilePhoneModeCaps(RMobilePhone::TMobilePhoneModeCaps& aArg) ;

protected:
	TBuf8<KLocalTel> iLocalNumber;		//< Local telephone number
	TBuf8<KRemoteTel> iRemoteNumber;	//< Remote telephone number (modem)
	TInt iState  ;

	TRequestStatus iStatus;				//< Request status used by asynchronous functions
	TRequestStatus iStatus2;
	TRequestStatus iStatus3;
	TRequestStatus iStatus4;
	TRequestStatus iStatus5;
	TRequestStatus iStatus6;
	TRequestStatus iStatus7;


	RMobileLine iLine;				  //< Telephone line
	RMobileCall iDataCall;			  //< Voice call object
	RCall::TCallInfo iCallInfo;
	RPhone::TLineInfo iLineInfo;
	RPhone::TCaps iPhoneCaps;		  //< Phone capabalities
	TInt iLineIndex;
	};

//------------------------------------------------------------------ 


//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineStatusIncommingCallL() from T_SimData
 * 
*/
class CTestLineStatusIncommingCall : public CTestSimDataStep
	{
public:
	CTestLineStatusIncommingCall() ;
	~CTestLineStatusIncommingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineStatusOutgoingCallL() from T_SimData
 * 
*/
class CTestLineStatusOutgoingCall : public CTestSimDataStep
	{
public:
	CTestLineStatusOutgoingCall() ;
	~CTestLineStatusOutgoingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineInfoL() from T_SimData
 * 
*/
class CTestLineInfo : public CTestSimDataStep
	{
public:
	CTestLineInfo() ;
	~CTestLineInfo(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineNotificationsIncommingCallL() from T_SimData
 * 
*/
class CTestLineNotificationsIncommingCall : public CTestSimDataStep
	{
public:
	CTestLineNotificationsIncommingCall() ;
	~CTestLineNotificationsIncommingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineNotificationsOutgoingCallL() from T_SimData
 * 
*/
class CTestLineNotificationsOutgoingCall : public CTestSimDataStep
	{
public:
	CTestLineNotificationsOutgoingCall() ;
	~CTestLineNotificationsOutgoingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineCancelsL() from T_SimData
 * 
*/
class CTestLineCancels : public CTestSimDataStep
	{
public:
	CTestLineCancels() ;
	~CTestLineCancels(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallInfoL() from T_SimData
 * 
*/
class CTestCallInfo : public CTestSimDataStep
	{
public:
	CTestCallInfo() ;
	~CTestCallInfo(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallNotificationsL() from T_SimData
 * 
*/
class CTestCallNotifications : public CTestSimDataStep
	{
public:
	CTestCallNotifications() ;
	~CTestCallNotifications(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallStatusIncommingCallL() from T_SimData
 * 
*/
class CTestCallStatusIncommingCall : public CTestSimDataStep
	{
public:
	CTestCallStatusIncommingCall() ;
	~CTestCallStatusIncommingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallStatusOutgoingCallL() from T_SimData
 * 
*/
class CTestCallStatusOutgoingCall : public CTestSimDataStep
	{
public:
	CTestCallStatusOutgoingCall() ;
	~CTestCallStatusOutgoingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallCancelsL() from T_SimData
 * 
*/
class CTestCallCancels : public CTestSimDataStep
	{
public:
	CTestCallCancels() ;
	~CTestCallCancels(){} ;
	virtual TVerdict doTestStepL();
	
	} ;


class CTestCallDuration : public CTestSimDataStep  
{
public:
	virtual TVerdict doTestStepL();
	TVerdict RunTestL(TBool aBRingOut);
	CTestCallDuration();
	virtual ~CTestCallDuration();

private:
	TBool RingOrDialL(TBool aBRingOut);
};

//------------------------------------------------------------------ 

class CTestRemoteHangup : public CTestSimDataStep
	{
public:
	CTestRemoteHangup() ;
	virtual TVerdict doTestStepL();
	} ;

#endif// TE_SimData_H__

