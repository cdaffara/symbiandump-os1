
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

#ifndef TE_SimMisc_H__
#define TE_SimMisc_H__

#include "Te_SimMiscTestStepBase.h"

const TInt KLocalTel=32;
const TInt KRemoteTel=32;

//------------------------------------------------------------------ 

/**
 * This class substitutes function SignalAndBatteryStrengthTestL() from T_SimMisc
 * 
*/
class CSignalAndBatteryStrengthTest : public CSimMiscTestStepBase
	{
public:
	CSignalAndBatteryStrengthTest() ;
	~CSignalAndBatteryStrengthTest(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function SubscriberIdTestL() from T_SimMisc
 * 
*/
class CSubscriberIdTest : public CSimMiscTestStepBase
	{
public:
	CSubscriberIdTest() ;
	~CSubscriberIdTest(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function PhoneStoreTestL() from T_SimMisc
 * 
*/
class CPhoneStoreTest : public CSimMiscTestStepBase
	{
public:
	CPhoneStoreTest() ;
	~CPhoneStoreTest(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function GetCapsL() from T_SimMisc
 * 
*/
class CGetCaps : public CSimMiscTestStepBase
	{
public:
	CGetCaps() ;
	~CGetCaps(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function GetLinesStatusL() from T_SimMisc
 * 
*/
class CGetLinesStatus : public CSimMiscTestStepBase
	{
public:
	CGetLinesStatus() ;
	~CGetLinesStatus(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function GetMultimodeCapsL() from T_SimMisc
 * 
*/
class CGetMultimodeCaps : public CSimMiscTestStepBase
	{
public:
	CGetMultimodeCaps() ;
	~CGetMultimodeCaps(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function GetNetworkCapsL() from T_SimMisc
 * 
*/
class CGetNetworkCaps : public CSimMiscTestStepBase
	{
public:
	CGetNetworkCaps() ;
	~CGetNetworkCaps(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function GetCurrentModeL() from T_SimMisc
 * 
*/
class CGetCurrentMode : public CSimMiscTestStepBase
	{
public:
	CGetCurrentMode() ;
	~CGetCurrentMode(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;
	
//------------------------------------------------------------------ 

/**
 * This class tests GetPhoneId and GetIdentityCaps
 * 
*/
class CGetPhoneId : public CSimMiscTestStepBase
	{
public:
	CGetPhoneId() ;
	~CGetPhoneId(){} ;
	virtual TVerdict doTestStepL( void );	
	} ;


/**
 * This class tests Publish & Subscribe support
 * 
*/
class CPubSubMode : public CSimMiscTestStepBase
	{
public:
	CPubSubMode() ;
	~CPubSubMode(){} ;
	virtual TVerdict doTestStepL( void );
	
private:
	void DisconnectFromEtel();
	TInt ConnectToEtelL();	
	void TestCallInitiationL(const TDesC& aLineName, TUid aPSCallInitCategory, const TInt aPSCallInitKey);
	} ;

#endif// TE_SimMisc_H__

