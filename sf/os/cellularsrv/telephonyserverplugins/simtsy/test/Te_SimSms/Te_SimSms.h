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

#ifndef TE_SimSms_H__
#define TE_SimSms_H__

#include "Te_SimSmsTestStepBase.h"

class CTestGetSmspList : public CActive
		{
	public:
		static CTestGetSmspList* NewLC(RMobileSmsMessaging& aSmsMessaging);
		~CTestGetSmspList();
		void Start();
		CMobilePhoneSmspList* RetrieveListL();
	protected:
		CTestGetSmspList(RMobileSmsMessaging& aSmsMessaging);
		void ConstructL();
		void RunL();
		void DoCancel();
	protected:
		CRetrieveMobilePhoneSmspList* iRetrieve;
		RMobileSmsMessaging& iSmsMessaging;
		};


//------------------------------------------------------------------ 


//------------------------------------------------------------------ 

/**
 * This class substitutes function TestUtilClassL() from T_SimSms
 * 
*/
class CTestUtilClass : public CSimSmsTestStepBase
	{
public:
	CTestUtilClass() ;
	~CTestUtilClass(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void TestUtilClass_Step1L();
	void TestUtilClass_Step2L();
	void TestUtilClass_Step3L();
	void TestUtilClass_Step3point1L();
	void TestUtilClass_Step4L();
	void TestUtilClass_Step5L();
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function GetCapsL() from T_SimSms
 * 
*/
class CGetCaps : public CSimSmsTestStepBase
	{
public:
	CGetCaps() ;
	~CGetCaps(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestSmspListL() from T_SimSms
 * 
*/
class CTestSmspList : public CSimSmsTestStepBase
	{
public:
	CTestSmspList() ;
	~CTestSmspList(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void SetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry) ;
	void GetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry) ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function MessageStoreInfoL() from T_SimSms
 * 
*/
class CMessageStoreInfo : public CSimSmsTestStepBase
	{
public:
	CMessageStoreInfo() ;
	~CMessageStoreInfo(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function MessageStoreOpsL() from T_SimSms
 * 
*/
class CMessageStoreOps : public CSimSmsTestStepBase
	{
public:
	CMessageStoreOps() ;
	~CMessageStoreOps(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function SendSmsL() from T_SimSms
 * 
*/
class CSendSms : public CSimSmsTestStepBase
	{
public:
	CSendSms() ;
	~CSendSms(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function ReceiveUnstoredSmsL() from T_SimSms
 * 
*/
class CReceiveUnstoredSms : public CSimSmsTestStepBase
	{
public:
	CReceiveUnstoredSms() ;
	~CReceiveUnstoredSms(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function ReceiveStoredSmsL() from T_SimSms
 * 
*/
class CReceiveStoredSms : public CSimSmsTestStepBase
	{
public:
	CReceiveStoredSms() ;
	~CReceiveStoredSms(){} ;
	virtual TVerdict doTestStepL( void );
	TInt SendSms(TBool aQuiet, RMobileSmsMessaging& aSms);
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function ReceiveStoredSmsAndCancelL() from T_SimSms
 * 
*/
class CReceiveStoredSmsAndCancel : public CSimSmsTestStepBase
	{
public:
	CReceiveStoredSmsAndCancel() ;
	~CReceiveStoredSmsAndCancel(){} ;
	virtual TVerdict doTestStepL( void );
	TInt SendSms(TBool aQuiet, RMobileSmsMessaging& aSms);
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function SendSmsAndCancelL() from T_SimSms
 * 
*/
class CSendSmsAndCancel : public CSimSmsTestStepBase
	{
public:
	CSendSmsAndCancel() ;
	~CSendSmsAndCancel(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function AckSmsStoredL() from T_SimSms
 * 
*/
class CAckSmsStored : public CSimSmsTestStepBase
	{
public:
	CAckSmsStored() ;
	~CAckSmsStored(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function AckSmsStoredCancelL() from T_SimSms
 * 
*/
class CAckSmsStoredCancel : public CSimSmsTestStepBase
	{
public:
	CAckSmsStoredCancel() ;
	~CAckSmsStoredCancel(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NackSmsStoredL() from T_SimSms
 * 
*/
class CNackSmsStored : public CSimSmsTestStepBase
	{
public:
	CNackSmsStored() ;
	~CNackSmsStored(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NackSmsStoredCancelL() from T_SimSms
 * 
*/
class CNackSmsStoredCancel : public CSimSmsTestStepBase
	{
public:
	CNackSmsStoredCancel() ;
	~CNackSmsStoredCancel(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function AckSmsStoredWithResponseL() from T_SimSms
 * 
*/
class CAckSmsStoredWithResponse : public CSimSmsTestStepBase
	{
public:
	CAckSmsStoredWithResponse() ;
	~CAckSmsStoredWithResponse(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function AckSmsStoredWithResponseCancelL() from T_SimSms
 * 
*/
class CAckSmsStoredWithResponseCancel : public CSimSmsTestStepBase
	{
public:
	CAckSmsStoredWithResponseCancel() ;
	~CAckSmsStoredWithResponseCancel(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NackSmsStoredWithResponseL() from T_SimSms
 * 
*/
class CNackSmsStoredWithResponse : public CSimSmsTestStepBase
	{
public:
	CNackSmsStoredWithResponse() ;
	~CNackSmsStoredWithResponse(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NackSmsStoredWithResponseCancelL() from T_SimSms
 * 
*/
class CNackSmsStoredWithResponseCancel : public CSimSmsTestStepBase
	{
public:
	CNackSmsStoredWithResponseCancel() ;
	~CNackSmsStoredWithResponseCancel(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function ResumeSmsReceptionL() from T_SimSms
 * 
*/
class CResumeSmsReception : public CSimSmsTestStepBase
	{
public:
	CResumeSmsReception() ;
	~CResumeSmsReception(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NotifySmsEventL() from T_SimSms
 * 
*/
class CNotifySmsEvent : public CSimSmsTestStepBase
	{
public:
	CNotifySmsEvent() ;
	~CNotifySmsEvent(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NotifySmsEventCancelL() from T_SimSms
 * 
*/
class CNotifySmsEventCancel : public CSimSmsTestStepBase
	{
public:
	CNotifySmsEventCancel() ;
	~CNotifySmsEventCancel(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

#endif// TE_SimSms_H__

