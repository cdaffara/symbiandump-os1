
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

#ifndef __TE_DATA_H__
#define __TE_DATA_H__

#include "Te_DataTestStepBase.h"

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineStatusIncommingCallL() from T_Data
 * 
*/
class CTestLineStatusIncommingCall : public CTestDataStep
	{
public:
	CTestLineStatusIncommingCall() ;
	~CTestLineStatusIncommingCall(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineStatusOutgoingCallL() from T_Data
 * 
*/
class CTestLineStatusOutgoingCall : public CTestDataStep
	{
public:
	CTestLineStatusOutgoingCall() ;
	~CTestLineStatusOutgoingCall(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineInfoL() from T_Data
 * 
*/
class CTestLineInfo : public CTestDataStep
	{
public:
	CTestLineInfo() ;
	~CTestLineInfo(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineNotificationsIncommingCallL() from T_Data
 * 
*/
class CTestLineNotificationsIncommingCall : public CTestDataStep
	{
public:
	CTestLineNotificationsIncommingCall() ;
	~CTestLineNotificationsIncommingCall(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineNotificationsOutgoingCallL() from T_Data
 * 
*/
class CTestLineNotificationsOutgoingCall : public CTestDataStep
	{
public:
	CTestLineNotificationsOutgoingCall() ;
	~CTestLineNotificationsOutgoingCall(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineCancelsL() from T_Data
 * 
*/
class CTestLineCancels : public CTestDataStep
	{
public:
	CTestLineCancels() ;
	~CTestLineCancels(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallInfoL() from T_Data
 * 
*/
class CTestCallInfo : public CTestDataStep
	{
public:
	CTestCallInfo() ;
	~CTestCallInfo(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallNotificationsL() from T_Data
 * 
*/
class CTestCallNotifications : public CTestDataStep
	{
public:
	CTestCallNotifications() ;
	~CTestCallNotifications(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallStatusIncommingCallL() from T_Data
 * 
*/
class CTestCallStatusIncommingCall : public CTestDataStep
	{
public:
	CTestCallStatusIncommingCall() ;
	~CTestCallStatusIncommingCall(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallStatusOutgoingCallL() from T_Data
 * 
*/
class CTestCallStatusOutgoingCall : public CTestDataStep
	{
public:
	CTestCallStatusOutgoingCall() ;
	~CTestCallStatusOutgoingCall(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallCancelsL() from T_Data
 * 
*/
class CTestCallCancels : public CTestDataStep
	{
public:
	CTestCallCancels() ;
	~CTestCallCancels(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestHandShakeL() from T_Data
 * 
*/
class CTestHandShake : public CTestDataStep
	{
public:
	CTestHandShake() ;
	~CTestHandShake(){} ;
	virtual TVerdict doTestStepL( void );
private:
	TInt DialRemoteModemTestL() ;
	void AnswerIncomingCallTestL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestGetMobileDataCallCapsLL() from T_Data
 * 
*/
class CTestGetMobileDataCallCaps : public CTestDataStep
	{
public:
	CTestGetMobileDataCallCaps() ;
	~CTestGetMobileDataCallCaps(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------

/**
 * This class reads a config file and extract phone numbers
 * 
*/
class CConfigParams : public CBase
	{
public:
	static CConfigParams* NewL(const TDesC &aCfgFileName);
	const TPtrC FindAlphaVar(const TDesC &aVarName,const TDesC &aDefault);
	TInt	FindNumVar(const TDesC &aVarName, const TInt aDefault);
	TBool ConfigFileExist();
protected:
	void ConstructL(const TDesC &aCfgFileName);
	CConfigParams(){};
private:
	TBool iFileExist;
	TBuf8<500> iConfigParamBuf8;
	TBuf<1000> iConfigParamBuf;
	};

#endif// __TE_DATA_H__

