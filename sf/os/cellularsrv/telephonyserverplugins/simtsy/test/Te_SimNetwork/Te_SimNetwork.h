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

#ifndef TE_SimNetwork_H__
#define TE_SimNetwork_H__

#include "Te_SimNetworkTestStepBase.h"

const TInt KLocalTel=32;
const TInt KRemoteTel=32;

//for retrive network information
struct KnownNetwork
	{
	TUint iMCC;
	TUint iMNC;
	const TText* iShortName;
	const TText* iLongName;
	};

class CTestGetDetectedNetworks : public CActive
	{
public:
	static CTestGetDetectedNetworks* NewLC(RMobilePhone& aPhone);
	~CTestGetDetectedNetworks();
	void Start();
	CMobilePhoneNetworkList *RetrieveList();
protected:
	CTestGetDetectedNetworks(RMobilePhone& aPhone);
	void ConstructL();
	void RunL();
	void DoCancel();
protected:
	CRetrieveMobilePhoneDetectedNetworks* iRetrieve;
	RMobilePhone& iPhone;
	};

//------------------------------------------------------------------ 

/**
 * This class substitutes function GetNetworkCapsTestsL() from T_SimNetwork
 * 
*/
class CGetNetworkCapsTests : public CSimNetworkTestStepBase
	{
public:
	CGetNetworkCapsTests() ;
	~CGetNetworkCapsTests(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NetworkRegistrationStatusTestsL() from T_SimNetwork
 * 
*/
class CNetworkRegistrationStatusTests : public CSimNetworkTestStepBase
	{
public:
	CNetworkRegistrationStatusTests() ;
	~CNetworkRegistrationStatusTests(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void GetNetworkRegistrationStatusL() ;
	void GetNetworkRegistrationStatusCancelL() ;
	void NotifyNetworkRegistrationStatusChangeL() ;
	void NotifyNetworkRegistrationStatusChangeCancelL() ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function ModeTestsL() from T_SimNetwork
 * 
*/
class CModeTests : public CSimNetworkTestStepBase
	{
public:
	CModeTests() ;
	~CModeTests(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void GetCurrentModeL() ;
	void NotifyModeChangeL() ;
	void NotifyModeChangeCancelL() ;
	void PrintNetworkMode(RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode) ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function CurrentNetworkTestsL() from T_SimNetwork
 * 
*/
class CCurrentNetworkTests : public CSimNetworkTestStepBase
	{
public:
	CCurrentNetworkTests() ;
	~CCurrentNetworkTests(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void GetCurrentNetworkL();
	void GetCurrentNetworkCancelL() ;
	void NotifyCurrentNetworkChangeL() ;
	void NotifyCurrentNetworkChangeCancelL() ;
	void GetServiceProviderL();
	void GetCurrentNetworkNameL();
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function HomeNeworkTestsL() from T_SimNetwork
 * 
*/
class CHomeNeworkTests : public CSimNetworkTestStepBase
	{
public:
	CHomeNeworkTests() ;
	~CHomeNeworkTests(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void GetHomeNetworkL() ;
	void GetHomeNetworkCancelL() ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NetworkSelectionSettingTestsL() from T_SimNetwork
 * 
*/
class CNetworkSelectionSettingTests : public CSimNetworkTestStepBase
	{
public:
	CNetworkSelectionSettingTests() ;
	~CNetworkSelectionSettingTests(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void GetNetworkSelectionSettingL() ;
	void SetNetworkSelectionSettingL() ;
	void SetNetworkSelectionSettingCancelL() ;
	void NotifyNetworkSelectionSettingChangeL() ;
	void NotifyNetworkSelectionSettingChangeCancelL() ;
	void PrintSelectionSettings(RMobilePhone::TMobilePhoneNetworkSelectionV1& aSetting) ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function NetworkTimeDateTestsL() from T_SimNetwork
 * 
*/
class CNetworkTimeDateTests : public CSimNetworkTestStepBase
	{
public:
	CNetworkTimeDateTests() ;
	~CNetworkTimeDateTests(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void GetNITZInfoL() ;
	void NotifyNITZInfoChangeL() ;
	void NotifyNITZInfoChangeCancelL() ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function GetCellInfo() from T_SimNetwork
 * 
*/
class CNetworkCellInfoTests : public CSimNetworkTestStepBase
	{
public:
	CNetworkCellInfoTests() ;
	~CNetworkCellInfoTests(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void GetCellInfoL() ;
	void GetCellInfoCancelL() ;
	void NotifyCellInfoChangeL() ;
	void NotifyCellInfoChangeCancelL() ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function DetectedNetworkTestL() from T_SimNetwork
 * 
*/
class CDetectedNetworkTest : public CSimNetworkTestStepBase
	{
public:
	CDetectedNetworkTest() ;
	~CDetectedNetworkTest(){} ;
	virtual TVerdict doTestStepL( void );
	// tests may optionaly implement pre and post ambles
	//we need them to start Active Sheduleer
private:	
	void GetNITZInfo() ;
	void NotifyNITZInfoChange() ;
	void NotifyNITZInfoChangeCancel() ;
private:
	CActiveScheduler* ipActiveScheduler ;
	} ;

//------------------------------------------------------------------ 
/**
 * This class substitutes function ServerInfoL() from T_SimNetwork
 * 
*/
class CServerInfo : public CSimNetworkTestStepBase
	{
public:
	CServerInfo() ;
	~CServerInfo(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	
	} ;
//------------------------------------------------------------------ 

#endif// TE_SimNetwork_H__

