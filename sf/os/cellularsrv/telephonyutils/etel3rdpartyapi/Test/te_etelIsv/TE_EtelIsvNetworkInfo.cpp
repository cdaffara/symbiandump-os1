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
// Tests for ETEL 3rd Party Call APIs.
// 
//

/**
 @file
*/

#include "TE_EtelIsvbase.h"
#include "TE_EtelIsvNetworkInfo.h"
 

CTestGetCurrentNetworkName::CTestGetCurrentNetworkName()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetCurrentNetworkName"));
	}

/**
*
@SYMTestCaseID Etel3rdParty 17.1
@SYMTestCaseDesc Test GetCurrentNetworkName 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, 
* REQ 2116.4.5
*/ 

enum TVerdict CTestGetCurrentNetworkName::doTestStepL()
{
	INFO_PRINTF1(_L("Test retrieval of current network name"));

	// Set the config.txt test number mapping
	SetTestNumber(9);

	// Set up the expected value to be the same as that provisioned in the SIM TSY config.txt file.
	_LIT16(networkname, "Vodafone_UK");
	CTelephony::TNetworkNameV1 ExpectedNetworkName;
	ExpectedNetworkName.iNetworkName=networkname;
	TRequestStatus ExpStat=KErrNone;
	
	// Create an active object and push it onto the clean up stack
	CTestGetCurrentNetworkNameAct* GetCurrentNetworkNameAct = CTestGetCurrentNetworkNameAct::NewLC(this, iTelephony);

	#if defined(_DEBUG)
	//Test OOM
		INFO_PRINTF1(_L("Test OOM for GetCurrentNetworkName"));
		ExpStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetCurrentNetworkNameAct->TestGetCurrentNetworkName(ExpectedNetworkName,ExpStat);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif

	ExpStat=KErrNone;
	// Attempt to retrieve the network name. Pass the expected values. 
	GetCurrentNetworkNameAct->TestGetCurrentNetworkName(ExpectedNetworkName,ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();
	
	//Test cancel
	INFO_PRINTF1(_L("Test Cancel for GetCurrentNetworkName"));
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetCurrentNetworkNameAct->TestGetCurrentNetworkName(ExpectedNetworkName,ExpStat);
	CTestCanceller* canceller = GetCurrentNetworkNameAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetCurrentNetworkNameAct->iStatus.Int() == KErrCancel || GetCurrentNetworkNameAct->iStatus.Int() == KErrNone, ETrue);

	// Test GetCurrentNetworkName when FlightMode is on
	INFO_PRINTF1(_L("Test GetCurrentNetworkName When Flight Mode is On"));
	SignalSimTsyToReduceTimersL();
	// Wait for 4 sec to make sure that RadioMode is changed to Off
	User::After(4000000);

	// Attempt to retrieve the network name. Pass the expected values.
	GetCurrentNetworkNameAct->TestGetCurrentNetworkName(ExpectedNetworkName,KErrAccessDenied, EFalse);
	// Start the active scheduler
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); //GetCurrentNetworkNameAct

	return TestStepResult();
}

CTestGetCurrentNetworkNameAct* CTestGetCurrentNetworkNameAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetCurrentNetworkNameAct* obj = new(ELeave) CTestGetCurrentNetworkNameAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestGetCurrentNetworkNameAct::CTestGetCurrentNetworkNameAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iNetworkNamePckg(iNetworkName),
	iNetworkNameBuff(iNetworkName)
	{  
	}

void CTestGetCurrentNetworkNameAct::ConstructL()
	{ 
	iCanceller = new(ELeave) CTestCanceller(this); 
	CActiveScheduler::Add(this);
	}

CTestGetCurrentNetworkNameAct::~CTestGetCurrentNetworkNameAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetCurrentNetworkNameAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
	}

void CTestGetCurrentNetworkNameAct::TestGetCurrentNetworkName(CTelephony::TNetworkNameV1 aNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkName=aNetworkName;
	iExpStat=aExpStat;
	  
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iNetworkNameBuff;
		}
	else
		{
		data = &iNetworkNamePckg;
		}

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkName(iStatus, *data );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkName(tmpStatus, *data );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}

void CTestGetCurrentNetworkNameAct::RunL()
	{ 
	iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
	
	if(iStatus.Int() == KErrNone)
		{
		 CTelephony::TNetworkNameV1* data;
		 if (iTestStep->iBufferMode)
		 	{
			data = reinterpret_cast<CTelephony::TNetworkNameV1*> ( &iNetworkNameBuff() );
			}
		 else
			{
			data = reinterpret_cast<CTelephony::TNetworkNameV1*> ( &iNetworkNamePckg() );
			}
		// Print off the values retrieved and those expected
		iTestStep->INFO_PRINTF2(_L("Network Name expected: %S"), &iExpectedNetworkName.iNetworkName) ;
		iTestStep->INFO_PRINTF2(_L("Network Name retrieved: %S"),  &data->iNetworkName);
		
		// Check that the retrieved values match those expected.
		iTestStep->TEST1(data->iNetworkName == iExpectedNetworkName.iNetworkName, ETrue);
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
		}
	CActiveScheduler::Stop();
	}

/**
*
@SYMTestCaseID Etel3rdParty 18.1
@SYMTestCaseDesc Test GetNetworkRegistrationStatus 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.4.4
*/

//////////////////////////////////////////////////////////////////
/// RETRIEVE NETWORK REGISTRATION ////////////////////////////////
//////////////////////////////////////////////////////////////////
CTestGetNetworkRegistrationStatus::CTestGetNetworkRegistrationStatus()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetNetworkRegistration"));
}

enum TVerdict CTestGetNetworkRegistrationStatus::doTestStepL()
{
	INFO_PRINTF1(_L("Test retrieval of current network registration"));

	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	CTelephony::TNetworkRegistrationV1 ExpectedNetworkRegistration;	 
	ExpectedNetworkRegistration.iRegStatus=CTelephony::ENotRegisteredSearching;
	
	// Set the config.txt test number mapping
	SetTestNumber(10);
	
	// Set the expected request status
	TRequestStatus ExpStat=KErrNone;

	// Create an active object and push it onto the clean up stack
	CTestGetNetworkRegistrationAct* GetNetworkRegistrationAct = CTestGetNetworkRegistrationAct::NewLC(this, iTelephony);

	#if defined(_DEBUG)
	//Test OOM
		INFO_PRINTF1(_L("Test OOM for GetNetworkRegistrationStatus"));
		ExpStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetNetworkRegistrationAct->TestGetNetworkRegistration(ExpectedNetworkRegistration,ExpStat);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
		
	// Attempt to retrieve the network registration. Pass the expected values. 
	ExpStat=KErrNone;
	GetNetworkRegistrationAct->TestGetNetworkRegistration(ExpectedNetworkRegistration,ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();
	
	//Test cancel
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetNetworkRegistrationAct->TestGetNetworkRegistration(ExpectedNetworkRegistration,ExpStat);
	CTestCanceller* canceller = GetNetworkRegistrationAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetNetworkRegistrationAct->iStatus.Int() == KErrCancel || GetNetworkRegistrationAct->iStatus.Int() == KErrNone, ETrue);

	CleanupStack::PopAndDestroy(); //GetNetworkRegistrationAct
	return TestStepResult();
}

CTestGetNetworkRegistrationAct* CTestGetNetworkRegistrationAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetNetworkRegistrationAct* obj = new(ELeave) CTestGetNetworkRegistrationAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestGetNetworkRegistrationAct::CTestGetNetworkRegistrationAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iNetworkRegistrationPckg(iNetworkRegistration),
	iNetworkRegistrationBuff(iNetworkRegistration)
	{  
	}

void CTestGetNetworkRegistrationAct::ConstructL()
	{ 
	iCanceller = new(ELeave) CTestCanceller(this); 
	CActiveScheduler::Add(this);
	}

CTestGetNetworkRegistrationAct::~CTestGetNetworkRegistrationAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetNetworkRegistrationAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
	}

void CTestGetNetworkRegistrationAct::TestGetNetworkRegistration(CTelephony::TNetworkRegistrationV1 aNetworkRegistration, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkRegistration=aNetworkRegistration;
	iExpStat=aExpStat;
	  
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iNetworkRegistrationBuff;
		}
	else
		{
		data = &iNetworkRegistrationPckg;
		}
        
	// Make a call to the 3rd party API
	iTelephony->GetNetworkRegistrationStatus(iStatus, *data );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetNetworkRegistrationStatus(tmpStatus, *data );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();
	}

void CTestGetNetworkRegistrationAct::RunL()
	{ 
	iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		
	if(iStatus.Int() == KErrNone)
		{
		CTelephony::TNetworkRegistrationV1* data;
		if (iTestStep->iBufferMode)
			{
			data = reinterpret_cast<CTelephony::TNetworkRegistrationV1*> ( &iNetworkRegistrationBuff() );
			}
		else
			{
			data = reinterpret_cast<CTelephony::TNetworkRegistrationV1*> ( &iNetworkRegistrationPckg() );
			}
		    
		// Print off the values retrieved and those expected
		iTestStep->INFO_PRINTF2(_L("Network registration expected: %d"), iExpectedNetworkRegistration.iRegStatus );
		iTestStep->INFO_PRINTF2(_L("Network registration retrieved: %d"),  data->iRegStatus);
		
		// Check that the retrieved values match those expected. 
		iTestStep->TEST1(data->iRegStatus == iExpectedNetworkRegistration.iRegStatus, ETrue);
	 	 
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
		}

	CActiveScheduler::Stop();
	}

/**
*
@SYMTestCaseID Etel3rdParty 19.1
@SYMTestCaseDesc Test GetCurrentNetworkInfo 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.4.1
*/ 

//////////////////////////////////////////////////////////////////
/// RETRIEVE NETWORK INFORMATION  ////////////////////////////////
//////////////////////////////////////////////////////////////////
CTestGetCurrentNetworkInfo::CTestGetCurrentNetworkInfo()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetNetworkInfo"));
}

enum TVerdict CTestGetCurrentNetworkInfo::doTestStepL()
{
	INFO_PRINTF1(_L("Test retrieval of current network information"));
	
	// Set the config.txt test number mapping
	SetTestNumber(11);

	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(KCountrycode, "ABCD");
	_LIT16(KNetworkid, "Network0");
	
	// Need to test display tag
	_LIT16(KDisplaytag, "Vodafone");
	_LIT16(KShortname, "Voda");
	_LIT16(KLongname, "Vodafone_UK");
	
	CTelephony::TNetworkInfoV1 ExpectedNetworkInfo;
	ExpectedNetworkInfo.iCountryCode=KCountrycode;
	ExpectedNetworkInfo.iNetworkId=KNetworkid;
	ExpectedNetworkInfo.iDisplayTag=KDisplaytag;
	ExpectedNetworkInfo.iShortName=KShortname;
	ExpectedNetworkInfo.iLongName=KLongname;	
	ExpectedNetworkInfo.iMode=CTelephony::ENetworkModeGsm;
	ExpectedNetworkInfo.iStatus = CTelephony::ENetworkStatusCurrent;
	ExpectedNetworkInfo.iBandInfo=CTelephony::EBandUnknown;
	ExpectedNetworkInfo.iCdmaSID.SetLength(0);
	ExpectedNetworkInfo.iAccess = CTelephony::ENetworkAccessGsm; //If v1=ENetworkAccessUnknown, if V2=ENetworkAccessGsm
	ExpectedNetworkInfo.iAreaKnown = EFalse;
	ExpectedNetworkInfo.iLocationAreaCode = 0;
	ExpectedNetworkInfo.iCellId = 0;
	
	TRequestStatus ExpStat=KErrNone;

	// Create an active object and push it onto the clean up stack
	CTestGetCurrentNetworkInfoAct* GetCurrentNetworkInfoAct = CTestGetCurrentNetworkInfoAct::NewLC(this, iTelephony);
	
	#if defined(_DEBUG)
	//Test OOM
		INFO_PRINTF1(_L("Test OOM for GetCurrentNetworkInfo"));
		ExpStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetCurrentNetworkInfoAct->TestGetCurrentNetworkInfo(ExpectedNetworkInfo, ExpStat);
		CActiveScheduler::Start();	
		__UHEAP_RESET;
	#endif
	
	// Attempt to retrieve the network info. Pass the expected values. 
	ExpStat=KErrNone;
	GetCurrentNetworkInfoAct->TestGetCurrentNetworkInfo(ExpectedNetworkInfo, ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();
	
	//Test cancel
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetCurrentNetworkInfoAct->TestGetCurrentNetworkInfo(ExpectedNetworkInfo, ExpStat);
	CTestCanceller* canceller = GetCurrentNetworkInfoAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetCurrentNetworkInfoAct->iStatus.Int() == KErrCancel || GetCurrentNetworkInfoAct->iStatus.Int() == KErrNone, ETrue);

	CleanupStack::PopAndDestroy(); //GetCurrentNetworkInfoAct
	return TestStepResult();
}

CTestGetCurrentNetworkInfoAct* CTestGetCurrentNetworkInfoAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
//Factory constructor
	{
	CTestGetCurrentNetworkInfoAct* obj = new(ELeave) CTestGetCurrentNetworkInfoAct(aTestStep, aTelephony, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestGetCurrentNetworkInfoAct::CTestGetCurrentNetworkInfoAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iUseMacros(aUseMacros),
	iNetworkInfoPckg(iNetworkInfo),
	iNetworkInfoBuff(iNetworkInfo)	 
	{  
	}

void CTestGetCurrentNetworkInfoAct::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetCurrentNetworkInfoAct::~CTestGetCurrentNetworkInfoAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetCurrentNetworkInfoAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
	}

void CTestGetCurrentNetworkInfoAct::TestGetCurrentNetworkInfo(const CTelephony::TNetworkInfoV1& aNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkInfo=aNetworkInfo;
	iExpStat=aExpStat;
	  
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iNetworkInfoBuff;
		}
	else
		{
		data = &iNetworkInfoPckg;
		}

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkInfo(iStatus, *data );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkInfo(tmpStatus, *data );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();
	}

void CTestGetCurrentNetworkInfoAct::RunL()
	{ 
	if(iUseMacros)
		{ 
		iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
		}
	
	if(iStatus.Int() == KErrNone)
		{
		 
		if(iUseMacros)
			{  
			CTelephony::TNetworkInfoV1* data;
			if (iTestStep->iBufferMode)
				{
				data = reinterpret_cast<CTelephony::TNetworkInfoV1*> ( &iNetworkInfoBuff() );
				}
			else
				{
				data = reinterpret_cast<CTelephony::TNetworkInfoV1*> ( &iNetworkInfoPckg() );
				}
			
			// Print off the values retrieved and those expected
			iTestStep->INFO_PRINTF1(_L("Network Info.........."));
			iTestStep->INFO_PRINTF2(_L("iCountryCode retrieved: %S"), &data->iCountryCode );
			iTestStep->INFO_PRINTF2(_L("iCountryCode expected: %S"), &iExpectedNetworkInfo.iCountryCode );
			iTestStep->INFO_PRINTF2(_L("iNetworkId retrieved: %S"), &data->iNetworkId );
			iTestStep->INFO_PRINTF2(_L("iNetworkId xpected: %S"), &iExpectedNetworkInfo.iNetworkId );
			iTestStep->INFO_PRINTF2(_L("iDisplayTag retrieved: %S"), &data->iDisplayTag );
			iTestStep->INFO_PRINTF2(_L("iDisplayTag xpected: %S"), &iExpectedNetworkInfo.iDisplayTag );
			iTestStep->INFO_PRINTF2(_L("iShortName retrieved: %S"), &data->iShortName );
			iTestStep->INFO_PRINTF2(_L("iShortName xpected: %S"), &iExpectedNetworkInfo.iShortName );
			iTestStep->INFO_PRINTF2(_L("iLongName retrieved: %S"), &data->iLongName );
			iTestStep->INFO_PRINTF2(_L("iLongName xpected: %S"), &iExpectedNetworkInfo.iLongName );		
			iTestStep->INFO_PRINTF2(_L("iMode retrieved: %d"), data->iMode);
			iTestStep->INFO_PRINTF2(_L("iMode xpected: %d"), iExpectedNetworkInfo.iMode);
			iTestStep->INFO_PRINTF2(_L("iStatus retrieved: %d"), data->iStatus);
			iTestStep->INFO_PRINTF2(_L("iStatus expected: %d"), iExpectedNetworkInfo.iStatus);
			iTestStep->INFO_PRINTF2(_L("iBandInfo retrieved: %d"), data->iBandInfo);
			iTestStep->INFO_PRINTF2(_L("iBandInfo expected: %d"), iExpectedNetworkInfo.iBandInfo);
			iTestStep->INFO_PRINTF2(_L("iCdmaSID retrieved: %S"), &data->iCdmaSID);
			iTestStep->INFO_PRINTF2(_L("iCdmaSID expected: %S"), &iExpectedNetworkInfo.iCdmaSID);
			iTestStep->INFO_PRINTF2(_L("iAccess retrieved: %d"), data->iAccess);
			iTestStep->INFO_PRINTF2(_L("iAccess expected: %d"), iExpectedNetworkInfo.iAccess);
			iTestStep->INFO_PRINTF2(_L("iAreaKnown retrieved: %d"), data->iAreaKnown);
			iTestStep->INFO_PRINTF2(_L("iAreaKnown expected: %d"), iExpectedNetworkInfo.iAreaKnown);
			iTestStep->INFO_PRINTF2(_L("iLocationAreaCode retrieved: %d"), data->iLocationAreaCode);
			iTestStep->INFO_PRINTF2(_L("iLocationAreaCode expected: %d"), iExpectedNetworkInfo.iLocationAreaCode);
			iTestStep->INFO_PRINTF2(_L("iCellId retrieved: %d"), data->iCellId);
			iTestStep->INFO_PRINTF2(_L("iCellId expected: %d"), iExpectedNetworkInfo.iCellId);
		
			
			// Check that the retrieved values match those expected.
			iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			iTestStep->TEST1(data->iCountryCode == iExpectedNetworkInfo.iCountryCode, ETrue);
			iTestStep->TEST1(data->iNetworkId == iExpectedNetworkInfo.iNetworkId, ETrue);
			iTestStep->TEST1(data->iDisplayTag == iExpectedNetworkInfo.iDisplayTag, ETrue);
			iTestStep->TEST1(data->iShortName == iExpectedNetworkInfo.iShortName, ETrue);
			iTestStep->TEST1(data->iLongName == iExpectedNetworkInfo.iLongName, ETrue);		
			iTestStep->TEST1(data->iMode = iExpectedNetworkInfo.iMode, ETrue);
			iTestStep->TEST1(data->iStatus == iExpectedNetworkInfo.iStatus, ETrue);
			iTestStep->TEST1(data->iBandInfo == iExpectedNetworkInfo.iBandInfo, ETrue);
			iTestStep->TEST1(data->iCdmaSID == iExpectedNetworkInfo.iCdmaSID, ETrue);
			iTestStep->TEST1(data->iAccess == iExpectedNetworkInfo.iAccess, ETrue);
			iTestStep->TEST1(data->iAreaKnown == iExpectedNetworkInfo.iAreaKnown, ETrue);
			iTestStep->TEST1(data->iLocationAreaCode == iExpectedNetworkInfo.iLocationAreaCode, ETrue);
			iTestStep->TEST1(data->iCellId == iExpectedNetworkInfo.iCellId, ETrue);
			}	 	 
		}
	else
		{
		if(iUseMacros)
			{ 
			iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
			iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			}
		}
	CActiveScheduler::Stop();
	}

/**
*
@SYMTestCaseID Etel3rdParty 20.1, TestGetOperatorName
@SYMTestCaseDesc Test GetOperatorName 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.4.7
*/ 

//////////////////////////////////////////////////////////////////
/// RETRIEVE SERVICE PROVIDER NAME ///////////////////////////////
//////////////////////////////////////////////////////////////////
CTestGetOperatorName::CTestGetOperatorName()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetOperatorName"));
}

enum TVerdict CTestGetOperatorName::doTestStepL()
{
	INFO_PRINTF1(_L("Test retrieval of current service provider name"));

	// Set the config.txt test number mapping
	SetTestNumber(12);
	
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(operatorName, "Voda");
	CTelephony::TOperatorNameV1 ExpectedOperatorName;
	ExpectedOperatorName.iOperatorName=operatorName;
	TRequestStatus ExpStat=KErrNone;
	
	// Create an active object and push it onto the clean up stack
	CTestGetOperatorNameAct* GetOperatorNameAct = CTestGetOperatorNameAct::NewLC(this, iTelephony);

	#if defined(_DEBUG)
	//Test OOM
		INFO_PRINTF1(_L("Test OOM for GetOperatorName"));
		ExpStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetOperatorNameAct->TestGetOperatorName(ExpectedOperatorName,ExpStat);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif	
	
	// Normal scenario
	// Attempt to retrieve the network name. Pass the expected values. 
	ExpStat=KErrNone;
	GetOperatorNameAct->TestGetOperatorName(ExpectedOperatorName,ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();
	
	//Test cancel
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetOperatorNameCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetOperatorNameAct->TestGetOperatorName(ExpectedOperatorName,ExpStat);
	CTestCanceller* canceller = GetOperatorNameAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetOperatorNameAct->iStatus.Int() == KErrCancel || GetOperatorNameAct->iStatus.Int() == KErrNone, ETrue);

	// Test GetOpeartorName when FlightMode is on
	INFO_PRINTF1(_L("Test GetOpeartorName When Flight Mode is On"));
	SignalSimTsyToReduceTimersL();
	// Wait for 4 sec to make sure that RadioMode is changed to Off
	User::After(4000000);

	// Attempt to retrieve the Opeartor name. Pass the expected values.
	GetOperatorNameAct->TestGetOperatorName(ExpectedOperatorName, KErrAccessDenied, EFalse);
	// Start the active scheduler
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); //GetOperatorNameAct

	return TestStepResult();
}

CTestGetOperatorNameAct* CTestGetOperatorNameAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetOperatorNameAct* obj = new(ELeave) CTestGetOperatorNameAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestGetOperatorNameAct::CTestGetOperatorNameAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iOperatorNamePckg(iOperatorName),
	iOperatorNameBuff(iOperatorName)	 
	{  
	}

void CTestGetOperatorNameAct::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetOperatorNameAct::~CTestGetOperatorNameAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetOperatorNameAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetOperatorNameCancel);
	}

void CTestGetOperatorNameAct::TestGetOperatorName(CTelephony::TOperatorNameV1 aOperatorName, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedOperatorName=aOperatorName;
	iExpStat=aExpStat;
	  
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iOperatorNameBuff;
		}
	else
		{
		data = &iOperatorNamePckg;
		}

	// Make a call to the 3rd party API
	iTelephony->GetOperatorName(iStatus, *data ); 
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetOperatorName(tmpStatus, *data ); 
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();
	}

void CTestGetOperatorNameAct::RunL()
	{ 
	iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
	
	if(iStatus.Int() == KErrNone)
		{
		CTelephony::TOperatorNameV1* data;
		if (iTestStep->iBufferMode)
			{
			data = reinterpret_cast<CTelephony::TOperatorNameV1*> ( &iOperatorNameBuff() );
			} 
		else
			{
			data = reinterpret_cast<CTelephony::TOperatorNameV1*> ( &iOperatorNamePckg() );
			} 
		    
		// Print off the values retrieved and those expected
		iTestStep->INFO_PRINTF2(_L("Service provider name expected: %S"), &iExpectedOperatorName.iOperatorName);
		iTestStep->INFO_PRINTF2(_L("Service provider name retrieved: %S"), &data->iOperatorName);
		
		// Check that the retrieved values match those expected.
		iTestStep->TEST1(data->iOperatorName == iExpectedOperatorName.iOperatorName, ETrue);
	 	 
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
		}

	CActiveScheduler::Stop();
	}

CTestNetworkFuncFlightModeOnWithPublishSubscribe::CTestNetworkFuncFlightModeOnWithPublishSubscribe()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestNetworkFuncFlightModeOnWithPublishSubscribe"));
}

/**
@SYMTestCaseID Etel3rdParty 21.1, TestNetworkFuncFlightModeOn
@SYMTestCaseDesc Test GetCurrentNetworkInfo 3rd party API with flight mode on
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.4.1
*/

enum TVerdict CTestNetworkFuncFlightModeOnWithPublishSubscribe::doTestStepL()
{
	INFO_PRINTF1(_L("START TEST 21.1 "));	
	
	INFO_PRINTF1(_L("Test retrieval of current network information (flight mode on) with publish and subscribe"));
	
	// Set the config.txt test number mapping
	SetTestNumber(11);
	
	// Set up phonePowerProperty variable so that flight mode can be manipulated.
	RProperty phonePowerProperty;
	User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
	CleanupClosePushL(phonePowerProperty);

	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(KCountrycode, "ABCD");
	_LIT16(KNetworkid, "Network0");
	
	// Need to test display tag
	_LIT16(KDisplaytag, "Vodafone");
	_LIT16(KShortname, "Voda");
	_LIT16(KLongname, "Vodafone_UK");
	
	CTelephony::TNetworkInfoV1 ExpectedNetworkInfo;
	ExpectedNetworkInfo.iCountryCode=KCountrycode;
	ExpectedNetworkInfo.iNetworkId=KNetworkid;
	ExpectedNetworkInfo.iDisplayTag=KDisplaytag;
	ExpectedNetworkInfo.iShortName=KShortname;
	ExpectedNetworkInfo.iLongName=KLongname;	
	ExpectedNetworkInfo.iMode=CTelephony::ENetworkModeGsm;
	ExpectedNetworkInfo.iStatus = CTelephony::ENetworkStatusCurrent;
	ExpectedNetworkInfo.iBandInfo=CTelephony::EBandUnknown;
	ExpectedNetworkInfo.iCdmaSID.SetLength(0);
	ExpectedNetworkInfo.iAccess = CTelephony::ENetworkAccessGsm; //If v1=ENetworkAccessUnknown, if V2=ENetworkAccessGsm
	ExpectedNetworkInfo.iAreaKnown = EFalse;
	ExpectedNetworkInfo.iLocationAreaCode = 0;
	ExpectedNetworkInfo.iCellId = 0;
	
	  
	// Create an active object and push it onto the clean up stack
	CTestGetCurrentNetworkInfoAct* getCurrentNetworkInfoAct = CTestGetCurrentNetworkInfoAct::NewLC(this, iTelephony);
	
	//Switch flight mode on (ie radio mode off)
	TRequestStatus status;
	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);	
	
	// Set expected status to KErrAccessDenied as this should be returned if
	// flight mode is on.
	// Attempt to retrieve the network info.  
	TRequestStatus expStat=KErrAccessDenied;
	getCurrentNetworkInfoAct->TestGetCurrentNetworkInfo(ExpectedNetworkInfo, expStat);
	// Start the active scheduler
	CActiveScheduler::Start(); 

	INFO_PRINTF1(_L("END TEST 21.1"));
	  
 
	CleanupStack::PopAndDestroy(getCurrentNetworkInfoAct);  
	CleanupStack::PopAndDestroy(&phonePowerProperty);	 
	return TestStepResult() ;	  
	}



//////////////////////////////////////////////////////////////////
/// RETRIEVE NETWORK INFORMATION  ////////////////////////////////
//////////////////////////////////////////////////////////////////

/** 
Each test step initialises it's own name.
*/
CTestGetCurrentNetworkInfoEighteenChars::CTestGetCurrentNetworkInfoEighteenChars()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetNetworkInfoEighteenChars"));
	}

/**
This test consists of three stages:
1. Test GetCurrentNetworkInfo truncates the network name when its length is
greater than the length of TNetworkInfoV1::iShortName and TNetworkInfoV1::iLongName.
2. Test GetOperatorName truncates the network name when its length is greater than
the length of TOperatorNameV1::iOperatorName.
3. Test GetCurrentNetworkName truncates the network name when its length is greater
than the length of TNetworkInfoV1::iNetworkName.
CTestGetCurrentNetworkInfoActEighteenChars::SetTestStage() sets the test stage.

@return The verdict of the test step
*/
enum TVerdict CTestGetCurrentNetworkInfoEighteenChars::doTestStepL()
	{
	INFO_PRINTF1(_L("Test retrieval of current network information, operator name and network name when the long network name is eighteen Chars long"));
	
	// Set the config.txt test number mapping
	SetTestNumber(36);

	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(KCountrycode, "ABCD");
	_LIT16(KNetworkid, "Network0");
	
	// Need to test display tag
	_LIT16(KDisplaytag, "Vodafone");
	_LIT16(KShortname, "SymbianN");
	_LIT16(KLongname, "SymbianNetworkAB");
	
	CTelephony::TNetworkInfoV1 ExpectedNetworkInfo;
	ExpectedNetworkInfo.iCountryCode=KCountrycode;
	ExpectedNetworkInfo.iNetworkId=KNetworkid;
	ExpectedNetworkInfo.iDisplayTag=KDisplaytag;
	ExpectedNetworkInfo.iShortName=KShortname;
	ExpectedNetworkInfo.iLongName=KLongname;	
	ExpectedNetworkInfo.iMode=CTelephony::ENetworkModeGsm;
	ExpectedNetworkInfo.iStatus = CTelephony::ENetworkStatusCurrent;
	ExpectedNetworkInfo.iBandInfo=CTelephony::EBandUnknown;
	ExpectedNetworkInfo.iCdmaSID.Zero();
	ExpectedNetworkInfo.iAccess = CTelephony::ENetworkAccessGsm; //If v1=ENetworkAccessUnknown, if V2=ENetworkAccessGsm
	ExpectedNetworkInfo.iAreaKnown = EFalse;
	ExpectedNetworkInfo.iLocationAreaCode = 0;
	ExpectedNetworkInfo.iCellId = 0;
	
	TRequestStatus ExpStat=KErrNone;

	// Create an active object and push it onto the clean up stack
	CTestGetCurrentNetworkInfoActEighteenChars* GetCurrentNetworkInfoActEighteenChars = CTestGetCurrentNetworkInfoActEighteenChars::NewLC(this, iTelephony);
	
	// Set testStage to 1 to indicate testing GetNetworkInfo
	GetCurrentNetworkInfoActEighteenChars->SetTestStage(1);
	
	// Attempt to retrieve the network info. Pass the expected values. 
	ExpStat=KErrNone;
	GetCurrentNetworkInfoActEighteenChars->TestGetCurrentNetworkInfoEighteenChars(ExpectedNetworkInfo, ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	//
	// Test GetOperator
	//
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(operatorNameSixteenChars, "SymbianOperatorA");
	CTelephony::TOperatorNameV1 ExpectedOperatorName;
	ExpectedOperatorName.iOperatorName=operatorNameSixteenChars;
	ExpStat=KErrNone;
	
	// Set testStage to 2 to indicate testing GetOperatorName
	GetCurrentNetworkInfoActEighteenChars->SetTestStage(2);	
	
	// Normal scenario
	// Attempt to retrieve the network name. Pass the expected values. 
	// EtelMM already returns the same 16 character operator name that ETel3rdParty expects, therefore this test does nothing.
	ExpStat=KErrNone;
	GetCurrentNetworkInfoActEighteenChars->TestGetOperatorNameSixteenChars(ExpectedOperatorName,ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	//
	// Test GetCurrentNetworkName
	//
	INFO_PRINTF1(_L("Test retrieval of current network name when long network name passed in is 18 Chars long"));

	// Set up the expected value to be the same as that provisioned in the SIM TSY config.txt file.
	_LIT16(networkname, "SymbianNetworkAB");
	CTelephony::TNetworkNameV1 ExpectedNetworkName;
	ExpectedNetworkName.iNetworkName=networkname;
	ExpStat=KErrNone;
	
	// Set testStage to 3 to indicate testing GetNetworkInfo
	GetCurrentNetworkInfoActEighteenChars->SetTestStage(3);

	ExpStat=KErrNone;
	// Attempt to retrieve the network name. Pass the expected values. 
	GetCurrentNetworkInfoActEighteenChars->TestGetCurrentNetworkNameEighteenChars(ExpectedNetworkName,ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); //GetCurrentNetworkInfoActEighteenChars
	return TestStepResult();
	}

/** 
Sets the test stage.
1. Tests GetCurrentNetworkInfo
2. Tests GetOperatorName
3. Tests GetCurrentNetworkName

@param aTestStage The stage of the test step to be executed
*/
void CTestGetCurrentNetworkInfoActEighteenChars::SetTestStage(TInt aTestStage)
	{
	iTestStage = aTestStage;
	}

/**
@param aTestStep Pointer to the test step that owns this object
@param aTelephony Pointer to CTelephony object whose functions are being tested
@param aUseMacros Specifies whether to use TestExecute logger macros
@return Pointer to instance of CTestGetCurrentNetworkInfoActEighteenChars
*/
CTestGetCurrentNetworkInfoActEighteenChars* CTestGetCurrentNetworkInfoActEighteenChars::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	{
	CTestGetCurrentNetworkInfoActEighteenChars* obj = new(ELeave) CTestGetCurrentNetworkInfoActEighteenChars(aTestStep, aTelephony, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

/**
Default constructor.

@param aTestStep Pointer to the test step that owns this object
@param aTelephony Pointer to CTelephony object whose functions are being tested
@param aUseMacros Specifies whether to use TestExecute logger macros
@return Pointer to instance of CTestGetCurrentNetworkInfoActEighteenChars
*/
CTestGetCurrentNetworkInfoActEighteenChars::CTestGetCurrentNetworkInfoActEighteenChars(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iUseMacros(aUseMacros)	 
	{  
	}

/**
Second phase constructor.
*/
void CTestGetCurrentNetworkInfoActEighteenChars::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

/**
Destructor calls Cancel() to clean up.
*/
CTestGetCurrentNetworkInfoActEighteenChars::~CTestGetCurrentNetworkInfoActEighteenChars()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

/**
Calls CTelephony::CancelAsync with the relevant parameter depending on the current test stage.
*/
void CTestGetCurrentNetworkInfoActEighteenChars::DoCancel()
	{
	if(iTestStage==1)
		{
		iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
		}
	else if (iTestStage==2)
		{
		iTelephony->CancelAsync(CTelephony::EGetOperatorNameCancel);
		}
	else if (iTestStage==3)
		{
		iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
		}
	}

/**
Makes the call to 3rd party API function CTelephony::GetCurrentNetworkInfo for testing.

@param aNetworkInfo The CTelephony::TNetworkInfoV1 object filled by the CTelephony::GetCurrentNetworkInfo function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specifies whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActEighteenChars::TestGetCurrentNetworkInfoEighteenChars(const CTelephony::TNetworkInfoV1& aNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkInfo=aNetworkInfo;
	iExpStat=aExpStat;
	  
	CTelephony::TNetworkInfoV1Pckg networkInfoPckg(iNetworkInfo);

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkInfo(iStatus, networkInfoPckg );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkInfo(tmpStatus, networkInfoPckg );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}

/**
Makes the call to 3rd party API function CTelephony::GetOperatorName for testing.

@param aOperatorName The CTelephony::TOperatorNameV1 object filled by the CTelephony::GetOperatorName function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specifies whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActEighteenChars::TestGetOperatorNameSixteenChars(CTelephony::TOperatorNameV1 aOperatorName, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedOperatorName=aOperatorName;
	iExpStat=aExpStat;
	  
	CTelephony::TOperatorNameV1Pckg operatorNamePckg(iOperatorName);

	// Make a call to the 3rd party API
	iTelephony->GetOperatorName(iStatus, operatorNamePckg ); 
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetOperatorName(tmpStatus, operatorNamePckg ); 
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();
	}

/**
Makes the call to 3rd party API function CTelephony::GetCurrentNetworkName for testing.

@param aNetworkName The CTelephony::TNetworkNameV1 object filled by the CTelephony::GetCurrentNetworkName function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specifies whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActEighteenChars::TestGetCurrentNetworkNameEighteenChars(CTelephony::TNetworkNameV1 aNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkName=aNetworkName;
	iExpStat=aExpStat;
	  
	CTelephony::TNetworkNameV1Pckg networkNamePckg(iNetworkName);

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkName(iStatus, networkNamePckg );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkName(tmpStatus, networkNamePckg );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}

/**
The part of RunL executed is determined by iTestStage.

iTestStage = 1 tests GetCurrentNetworkInfo
iTestStage = 2 tests GetOperatorName
iTestStage = 3 tests GetNetworkName
*/
void CTestGetCurrentNetworkInfoActEighteenChars::RunL()
	{ 
    if (iTestStage==1)
    	{
	    if(iUseMacros)
			{ 
			iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
			}

		if(iStatus.Int() == KErrNone)
			{
			if(iUseMacros)
				{  
				// Print off the values retrieved and those expected
			
				iTestStep->INFO_PRINTF1(_L("Network Info.........."));
				iTestStep->INFO_PRINTF2(_L("iCountryCode retrieved: %S"), &iNetworkInfo.iCountryCode );
				iTestStep->INFO_PRINTF2(_L("iCountryCode expected: %S"), &iExpectedNetworkInfo.iCountryCode );
				iTestStep->INFO_PRINTF2(_L("iNetworkId retrieved: %S"), &iNetworkInfo.iNetworkId );
				iTestStep->INFO_PRINTF2(_L("iNetworkId expected: %S"), &iExpectedNetworkInfo.iNetworkId );
				iTestStep->INFO_PRINTF2(_L("iDisplayTag retrieved: %S"), &iNetworkInfo.iDisplayTag );
				iTestStep->INFO_PRINTF2(_L("iDisplayTag expected: %S"), &iExpectedNetworkInfo.iDisplayTag );
				iTestStep->INFO_PRINTF2(_L("iShortName retrieved: %S"), &iNetworkInfo.iShortName );
				iTestStep->INFO_PRINTF2(_L("iShortName expected: %S"), &iExpectedNetworkInfo.iShortName );
				iTestStep->INFO_PRINTF2(_L("iLongName retrieved: %S"), &iNetworkInfo.iLongName );
				iTestStep->INFO_PRINTF2(_L("iLongName expected: %S"), &iExpectedNetworkInfo.iLongName );		
				iTestStep->INFO_PRINTF2(_L("iMode retrieved: %d"), iNetworkInfo.iMode);
				iTestStep->INFO_PRINTF2(_L("iMode expected: %d"), iExpectedNetworkInfo.iMode);
				iTestStep->INFO_PRINTF2(_L("iStatus retrieved: %d"), iNetworkInfo.iStatus);
				iTestStep->INFO_PRINTF2(_L("iStatus expected: %d"), iExpectedNetworkInfo.iStatus);
				iTestStep->INFO_PRINTF2(_L("iBandInfo retrieved: %d"), iNetworkInfo.iBandInfo);
				iTestStep->INFO_PRINTF2(_L("iBandInfo expected: %d"), iExpectedNetworkInfo.iBandInfo);
				iTestStep->INFO_PRINTF2(_L("iCdmaSID retrieved: %S"), &iNetworkInfo.iCdmaSID);
				iTestStep->INFO_PRINTF2(_L("iCdmaSID expected: %S"), &iExpectedNetworkInfo.iCdmaSID);
				iTestStep->INFO_PRINTF2(_L("iAccess retrieved: %d"), iNetworkInfo.iAccess);
				iTestStep->INFO_PRINTF2(_L("iAccess expected: %d"), iExpectedNetworkInfo.iAccess);
				iTestStep->INFO_PRINTF2(_L("iAreaKnown retrieved: %d"), iNetworkInfo.iAreaKnown);
				iTestStep->INFO_PRINTF2(_L("iAreaKnown expected: %d"), iExpectedNetworkInfo.iAreaKnown);
				iTestStep->INFO_PRINTF2(_L("iLocationAreaCode retrieved: %d"), iNetworkInfo.iLocationAreaCode);
				iTestStep->INFO_PRINTF2(_L("iLocationAreaCode expected: %d"), iExpectedNetworkInfo.iLocationAreaCode);
				iTestStep->INFO_PRINTF2(_L("iCellId retrieved: %d"), iNetworkInfo.iCellId);
				iTestStep->INFO_PRINTF2(_L("iCellId expected: %d"), iExpectedNetworkInfo.iCellId);

				// Check that the retrieved values match those expected.
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				iTestStep->TEST1(iNetworkInfo.iCountryCode == iExpectedNetworkInfo.iCountryCode, ETrue);
				iTestStep->TEST1(iNetworkInfo.iNetworkId == iExpectedNetworkInfo.iNetworkId, ETrue);
				iTestStep->TEST1(iNetworkInfo.iDisplayTag == iExpectedNetworkInfo.iDisplayTag, ETrue);
				iTestStep->TEST1(iNetworkInfo.iShortName == iExpectedNetworkInfo.iShortName, ETrue);
				iTestStep->TEST1(iNetworkInfo.iLongName == iExpectedNetworkInfo.iLongName, ETrue);		
				iTestStep->TEST1(iNetworkInfo.iMode = iExpectedNetworkInfo.iMode, ETrue);
				iTestStep->TEST1(iNetworkInfo.iStatus == iExpectedNetworkInfo.iStatus, ETrue);
				iTestStep->TEST1(iNetworkInfo.iBandInfo == iExpectedNetworkInfo.iBandInfo, ETrue);
				iTestStep->TEST1(iNetworkInfo.iCdmaSID == iExpectedNetworkInfo.iCdmaSID, ETrue);
				iTestStep->TEST1(iNetworkInfo.iAccess == iExpectedNetworkInfo.iAccess, ETrue);
				iTestStep->TEST1(iNetworkInfo.iAreaKnown == iExpectedNetworkInfo.iAreaKnown, ETrue);
				iTestStep->TEST1(iNetworkInfo.iLocationAreaCode == iExpectedNetworkInfo.iLocationAreaCode, ETrue);
				iTestStep->TEST1(iNetworkInfo.iCellId == iExpectedNetworkInfo.iCellId, ETrue);
				}	 	 
			}
		else
			{
			if(iUseMacros)
				{ 
				iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				}
			else
				{
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);					
				}
			}
    	}
	else if (iTestStage==2)	
		{
		// GetOperator
		iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		
		if(iStatus.Int() == KErrNone)
			{
			// Print off the values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Service provider name expected: %S"), &iExpectedOperatorName.iOperatorName);
			iTestStep->INFO_PRINTF2(_L("Service provider name retrieved: %S"), &iOperatorName.iOperatorName);
			
			// Check that the retrieved values match those expected.
			iTestStep->TEST1(iOperatorName.iOperatorName == iExpectedOperatorName.iOperatorName, ETrue);
		 	 
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
			iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			}
		}
	else if (iTestStage==3)
		{
		iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		
		if(iStatus.Int() == KErrNone)
			{
			 
			// Print off the values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Network Name expected: %S"), &iExpectedNetworkName.iNetworkName) ;
			iTestStep->INFO_PRINTF2(_L("Network Name retrieved: %S"),  &iNetworkName.iNetworkName);
			
			// Check that the retrieved values match those expected.
			iTestStep->TEST1(iNetworkName.iNetworkName == iExpectedNetworkName.iNetworkName, ETrue);
		 	 
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
			iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			}
		}
	CActiveScheduler::Stop();
	}


//////////////////////////////////////////////////////////////////
/// RETRIEVE NETWORK INFORMATION  ////////////////////////////////
//////////////////////////////////////////////////////////////////

/**
Each test step initialises it's own name.
*/
CTestGetCurrentNetworkInfoTwentyThreeChars::CTestGetCurrentNetworkInfoTwentyThreeChars()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetNetworkInfoTwentyThreeChars"));
	}

/**
This test consists of three stages:
1. Test GetCurrentNetworkInfo truncates the network name when its length is
greater than the length of TNetworkInfoV1::iShortName, TNetworkInfoV1::iLongName
and TMobilePhoneNetworkInfoV1::iShortName and iLongName.
2. Test GetOperatorName truncates the network name when its length is greater than
the length of TOperatorNameV1::iOperatorName.
3. Test GetCurrentNetworkName truncates the network name when its length is greater
than the length of TNetworkInfoV1::iNetworkName.
CTestGetCurrentNetworkInfoActTwentyThreeChars::SetTestStage() sets the test stage.

@return The verdict of the test step
*/
enum TVerdict CTestGetCurrentNetworkInfoTwentyThreeChars::doTestStepL()
	{
	INFO_PRINTF1(_L("Test retrieval of current network information, operator name and network name when the long network name is twenty three Chars long"));
	
	// Set the config.txt test number mapping
	SetTestNumber(37);

	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(KCountrycode, "ABCD");
	_LIT16(KNetworkid, "Network0");
	
	// Need to test display tag
	_LIT16(KDisplaytag, "Vodafone");
	_LIT16(KShortname, "SymbianN");
	_LIT16(KLongname, "SymbianNetworkAB");
	
	CTelephony::TNetworkInfoV1 ExpectedNetworkInfo;
	ExpectedNetworkInfo.iCountryCode=KCountrycode;
	ExpectedNetworkInfo.iNetworkId=KNetworkid;
	ExpectedNetworkInfo.iDisplayTag=KDisplaytag;
	ExpectedNetworkInfo.iShortName=KShortname;
	ExpectedNetworkInfo.iLongName=KLongname;	
	ExpectedNetworkInfo.iMode=CTelephony::ENetworkModeGsm;
	ExpectedNetworkInfo.iStatus = CTelephony::ENetworkStatusCurrent;
	ExpectedNetworkInfo.iBandInfo=CTelephony::EBandUnknown;
	ExpectedNetworkInfo.iCdmaSID.SetLength(0);
	ExpectedNetworkInfo.iAccess = CTelephony::ENetworkAccessGsm; //If v1=ENetworkAccessUnknown, if V2=ENetworkAccessGsm
	ExpectedNetworkInfo.iAreaKnown = EFalse;
	ExpectedNetworkInfo.iLocationAreaCode = 0;
	ExpectedNetworkInfo.iCellId = 0;
	
	TRequestStatus ExpStat=KErrNone;

	// Create an active object and push it onto the clean up stack
	CTestGetCurrentNetworkInfoActTwentyThreeChars* GetCurrentNetworkInfoActTwentyThreeChars = CTestGetCurrentNetworkInfoActTwentyThreeChars::NewLC(this, iTelephony);

 	// Set testStage to 1 to indicate testing GetNetworkInfo
	GetCurrentNetworkInfoActTwentyThreeChars->SetTestStage(1);
	
	// Attempt to retrieve the network info. Pass the expected values. 
	ExpStat=KErrNone;
	GetCurrentNetworkInfoActTwentyThreeChars->TestGetCurrentNetworkInfoTwentyThreeChars(ExpectedNetworkInfo, ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	//
	// Test GetOperator
	//
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(operatorNameSixteenChars, "SymbianOperatorA");
	CTelephony::TOperatorNameV1 ExpectedOperatorName;
	ExpectedOperatorName.iOperatorName=operatorNameSixteenChars;
	ExpStat=KErrNone;
	
	// Set testStage to 2 to indicate testing GetOperatorName
	GetCurrentNetworkInfoActTwentyThreeChars->SetTestStage(2);
	
	// Normal scenario
	// Attempt to retrieve the network name. Pass the expected values. 
	ExpStat=KErrNone;
	GetCurrentNetworkInfoActTwentyThreeChars->TestGetOperatorNameSixteenChars(ExpectedOperatorName,ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	//
	// Test GetCurrentNetworkName
	//
	INFO_PRINTF1(_L("Test retrieval of current network name when long network name passed in is 18 Chars long"));

	// Set up the expected value to be the same as that provisioned in the SIM TSY config.txt file.
	_LIT16(networkname, "SymbianNetworkAB");
	CTelephony::TNetworkNameV1 ExpectedNetworkName;
	ExpectedNetworkName.iNetworkName=networkname;
	ExpStat=KErrNone;
	
	// Set testStage to 3 to indicate testing GetNetworkInfo
	GetCurrentNetworkInfoActTwentyThreeChars->SetTestStage(3);

	ExpStat=KErrNone;
	// Attempt to retrieve the network name. Pass the expected values. 
	GetCurrentNetworkInfoActTwentyThreeChars->TestGetCurrentNetworkNameTwentyThreeChars(ExpectedNetworkName,ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); //GetCurrentNetworkInfoActTwentyThreeChars
	return TestStepResult();
	}

/**
@param The stage of the test step to be executed
*/
void CTestGetCurrentNetworkInfoActTwentyThreeChars::SetTestStage(TInt aTestStage)
	{
	iTestStage = aTestStage;
	}

/**
Factory constructor.

@param aTestStep Pointer to the test step that owns this object
@param aTelephony Pointer to CTelephony object whose functions are being tested
@param aUseMacros Specifies whether to use TestExecute logger macros
@return Pointer to instance of CTestGetCurrentNetworkInfoActTwentyThreeChars
*/
CTestGetCurrentNetworkInfoActTwentyThreeChars* CTestGetCurrentNetworkInfoActTwentyThreeChars::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	{
	CTestGetCurrentNetworkInfoActTwentyThreeChars* obj = new(ELeave) CTestGetCurrentNetworkInfoActTwentyThreeChars(aTestStep, aTelephony, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

/**
Default onstructor.

@param aTestStep Pointer to the test step that owns this object
@param aTelephony Pointer to CTelephony object whose functions are being tested
@param aUseMacros Specifies whether to use TestExecute logger macros
@return Pointer to instance of CTestGetCurrentNetworkInfoActTwentyThreeChars
*/
CTestGetCurrentNetworkInfoActTwentyThreeChars::CTestGetCurrentNetworkInfoActTwentyThreeChars(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iUseMacros(aUseMacros)	 
	{  
	}

/**
Second phase constructor.
*/
void CTestGetCurrentNetworkInfoActTwentyThreeChars::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

/**
Destructor calls Cancel() to clean up.
*/
CTestGetCurrentNetworkInfoActTwentyThreeChars::~CTestGetCurrentNetworkInfoActTwentyThreeChars()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

/**
Calls CTelephony::CancelAsync with the relevant parameter depending on the current test stage.
*/
void CTestGetCurrentNetworkInfoActTwentyThreeChars::DoCancel()
	{
	if(iTestStage==1)
		{
		iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
		}
	else if (iTestStage==2)
		{
		iTelephony->CancelAsync(CTelephony::EGetOperatorNameCancel);
		}
	else if (iTestStage==3)
		{
		iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
		}
	}

/**
Makes the call to 3rd party API function CTelephony::GetCurrentNetworkInfo for testing.

@param aNetworkInfo The CTelephony::TNetworkInfoV1 object filled by the CTelephony::GetCurrentNetworkInfo function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specifies whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActTwentyThreeChars::TestGetCurrentNetworkInfoTwentyThreeChars(const CTelephony::TNetworkInfoV1& aNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkInfo=aNetworkInfo;
	iExpStat=aExpStat;
	  
	CTelephony::TNetworkInfoV1Pckg NetworkInfoPckg(iNetworkInfo);

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkInfo(iStatus, NetworkInfoPckg );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkInfo(tmpStatus, NetworkInfoPckg );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}

/**
Makes the call to 3rd party API function CTelephony::GetOperatorName for testing.

@param aOperatorName The CTelephony::TOperatorNameV1 object filled by the CTelephony::GetCurrentNetworkInfo function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specifies whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActTwentyThreeChars::TestGetOperatorNameSixteenChars(CTelephony::TOperatorNameV1 aOperatorName, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedOperatorName=aOperatorName;
	iExpStat=aExpStat;
	  
	CTelephony::TOperatorNameV1Pckg OperatorNamePckg(iOperatorName);

	// Make a call to the 3rd party API
	iTelephony->GetOperatorName(iStatus, OperatorNamePckg ); 
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetOperatorName(tmpStatus, OperatorNamePckg ); 
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();
	}

/**
Makes the call to 3rd party API function CTelephony::GetCurrentNetworkName for testing.

@param aNetworkName The CTelephony::TNetworkNameV1 object filled by the CTelephony::GetCurrentNetworkName function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specifies whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActTwentyThreeChars::TestGetCurrentNetworkNameTwentyThreeChars(CTelephony::TNetworkNameV1 aNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkName=aNetworkName;
	iExpStat=aExpStat;
	  
	CTelephony::TNetworkNameV1Pckg NetworkNamePckg(iNetworkName);

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkName(iStatus, NetworkNamePckg );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkName(tmpStatus, NetworkNamePckg );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}

/**
The part of RunL executed is determined by iTestStage.
iTestStage = 1 tests GetCurrentNetworkInfo
iTestStage = 2 tests GetOperatorName
iTestStage = 3 tests GetNetworkName
*/
void CTestGetCurrentNetworkInfoActTwentyThreeChars::RunL()
	{ 
    if (iTestStage==1)
    	{
	    if(iUseMacros)
			{ 
			iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
			}
	
		if(iStatus.Int() == KErrNone)
			{
		 
			if(iUseMacros)
				{  
				// Print off the values retrieved and those expected
			
				iTestStep->INFO_PRINTF1(_L("Network Info.........."));
				iTestStep->INFO_PRINTF2(_L("iCountryCode retrieved: %S"), &iNetworkInfo.iCountryCode );
				iTestStep->INFO_PRINTF2(_L("iCountryCode expected: %S"), &iExpectedNetworkInfo.iCountryCode );
				iTestStep->INFO_PRINTF2(_L("iNetworkId retrieved: %S"), &iNetworkInfo.iNetworkId );
				iTestStep->INFO_PRINTF2(_L("iNetworkId expected: %S"), &iExpectedNetworkInfo.iNetworkId );
				iTestStep->INFO_PRINTF2(_L("iDisplayTag retrieved: %S"), &iNetworkInfo.iDisplayTag );
				iTestStep->INFO_PRINTF2(_L("iDisplayTag expected: %S"), &iExpectedNetworkInfo.iDisplayTag );
				iTestStep->INFO_PRINTF2(_L("iShortName retrieved: %S"), &iNetworkInfo.iShortName );
				iTestStep->INFO_PRINTF2(_L("iShortName expected: %S"), &iExpectedNetworkInfo.iShortName );
				iTestStep->INFO_PRINTF2(_L("iLongName retrieved: %S"), &iNetworkInfo.iLongName );
				iTestStep->INFO_PRINTF2(_L("iLongName expected: %S"), &iExpectedNetworkInfo.iLongName );		
				iTestStep->INFO_PRINTF2(_L("iMode retrieved: %d"), iNetworkInfo.iMode);
				iTestStep->INFO_PRINTF2(_L("iMode expected: %d"), iExpectedNetworkInfo.iMode);
				iTestStep->INFO_PRINTF2(_L("iStatus retrieved: %d"), iNetworkInfo.iStatus);
				iTestStep->INFO_PRINTF2(_L("iStatus expected: %d"), iExpectedNetworkInfo.iStatus);
				iTestStep->INFO_PRINTF2(_L("iBandInfo retrieved: %d"), iNetworkInfo.iBandInfo);
				iTestStep->INFO_PRINTF2(_L("iBandInfo expected: %d"), iExpectedNetworkInfo.iBandInfo);
				iTestStep->INFO_PRINTF2(_L("iCdmaSID retrieved: %S"), &iNetworkInfo.iCdmaSID);
				iTestStep->INFO_PRINTF2(_L("iCdmaSID expected: %S"), &iExpectedNetworkInfo.iCdmaSID);
				iTestStep->INFO_PRINTF2(_L("iAccess retrieved: %d"), iNetworkInfo.iAccess);
				iTestStep->INFO_PRINTF2(_L("iAccess expected: %d"), iExpectedNetworkInfo.iAccess);
				iTestStep->INFO_PRINTF2(_L("iAreaKnown retrieved: %d"), iNetworkInfo.iAreaKnown);
				iTestStep->INFO_PRINTF2(_L("iAreaKnown expected: %d"), iExpectedNetworkInfo.iAreaKnown);
				iTestStep->INFO_PRINTF2(_L("iLocationAreaCode retrieved: %d"), iNetworkInfo.iLocationAreaCode);
				iTestStep->INFO_PRINTF2(_L("iLocationAreaCode expected: %d"), iExpectedNetworkInfo.iLocationAreaCode);
				iTestStep->INFO_PRINTF2(_L("iCellId retrieved: %d"), iNetworkInfo.iCellId);
				iTestStep->INFO_PRINTF2(_L("iCellId expected: %d"), iExpectedNetworkInfo.iCellId);
			
				
				// Check that the retrieved values match those expected.
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				iTestStep->TEST1(iNetworkInfo.iCountryCode == iExpectedNetworkInfo.iCountryCode, ETrue);
				iTestStep->TEST1(iNetworkInfo.iNetworkId == iExpectedNetworkInfo.iNetworkId, ETrue);
				iTestStep->TEST1(iNetworkInfo.iDisplayTag == iExpectedNetworkInfo.iDisplayTag, ETrue);
				iTestStep->TEST1(iNetworkInfo.iShortName == iExpectedNetworkInfo.iShortName, ETrue);
				iTestStep->TEST1(iNetworkInfo.iLongName == iExpectedNetworkInfo.iLongName, ETrue);		
				iTestStep->TEST1(iNetworkInfo.iMode = iExpectedNetworkInfo.iMode, ETrue);
				iTestStep->TEST1(iNetworkInfo.iStatus == iExpectedNetworkInfo.iStatus, ETrue);
				iTestStep->TEST1(iNetworkInfo.iBandInfo == iExpectedNetworkInfo.iBandInfo, ETrue);
				iTestStep->TEST1(iNetworkInfo.iCdmaSID == iExpectedNetworkInfo.iCdmaSID, ETrue);
				iTestStep->TEST1(iNetworkInfo.iAccess == iExpectedNetworkInfo.iAccess, ETrue);
				iTestStep->TEST1(iNetworkInfo.iAreaKnown == iExpectedNetworkInfo.iAreaKnown, ETrue);
				iTestStep->TEST1(iNetworkInfo.iLocationAreaCode == iExpectedNetworkInfo.iLocationAreaCode, ETrue);
				iTestStep->TEST1(iNetworkInfo.iCellId == iExpectedNetworkInfo.iCellId, ETrue);
				}	 	 
			}
		else
			{
			if(iUseMacros)
				{ 
				iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				}
			else
				{
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);					
				}				
			}
	
    	}
	else if (iTestStage==2)	
		{
		// GetOperator
		iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		
		if(iStatus.Int() == KErrNone)
			{
			 
			// Print off the values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Service provider name expected: %S"), &iExpectedOperatorName.iOperatorName);
			iTestStep->INFO_PRINTF2(_L("Service provider name retrieved: %S"), &iOperatorName.iOperatorName);
			
			// Check that the retrieved values match those expected.
			iTestStep->TEST1(iOperatorName.iOperatorName == iExpectedOperatorName.iOperatorName, ETrue);
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
			iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			}
		}
	else if (iTestStage==3)
		{
		iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		
		if(iStatus.Int() == KErrNone)
			{
			 
			// Print off the values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Network Name expected: %S"), &iExpectedNetworkName.iNetworkName) ;
			iTestStep->INFO_PRINTF2(_L("Network Name retrieved: %S"),  &iNetworkName.iNetworkName);
			
			// Check that the retrieved values match those expected.
			iTestStep->TEST1(iNetworkName.iNetworkName == iExpectedNetworkName.iNetworkName, ETrue);
		 	 
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
			iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			}
		}
	CActiveScheduler::Stop();
	}


//////////////////////////////////////////////////////////////////
/// RETRIEVE NETWORK INFORMATION  ////////////////////////////////
//////////////////////////////////////////////////////////////////

/** 
Each test step initialises it's own name
*/
CTestGetCurrentNetworkInfoEighteenCharsV2::CTestGetCurrentNetworkInfoEighteenCharsV2()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetNetworkInfoEighteenCharsV2"));
	}

/**
The test consists of two stages with the stage determined by the
iTestStage variable.
1. Tests GetCurrentNetworkInfo when TNetworkInfoV2 is used and the 
network long name is 18 characters long. The larger buffer in
TNetworkInfoV2 compared to TNetworkInfoV1 means that the network
name is not truncated.
2. Tests GetCurrentNetworkName when TNetworkNameV2 is used and the
network long name is 18 characters long. The larger buffer in
TNetworkInfoV2 compared to TNetworkInfoV1 means that the network
name is not truncated.

@return The verdict of the test step
*/
enum TVerdict CTestGetCurrentNetworkInfoEighteenCharsV2::doTestStepL()
	{
	INFO_PRINTF1(_L("Test retrieval of current network information, operator name and network name when the long network name is eighteen Chars long using TNetworkInfoV2 and TNetworkNameV2"));
	
	// Set the config.txt test number mapping
	SetTestNumber(36);

	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(KCountrycode, "ABCD");
	_LIT16(KNetworkid, "Network0");
	
	// Need to test display tag
	_LIT16(KDisplaytag, "Vodafone");
	_LIT16(KShortnameV2, "SymbianNe"); // Up to first 10 Chars from test data
	_LIT16(KLongnameV2, "SymbianNetworkABCD"); // Up to first 20 Chars from test data
	
	CTelephony::TNetworkInfoV2 ExpectedNetworkInfoV2;
	ExpectedNetworkInfoV2.iCountryCode=KCountrycode;
	ExpectedNetworkInfoV2.iNetworkId=KNetworkid;
	ExpectedNetworkInfoV2.iDisplayTag=KDisplaytag;
	ExpectedNetworkInfoV2.iShortName=KShortnameV2;
	ExpectedNetworkInfoV2.iLongName=KLongnameV2;	
	ExpectedNetworkInfoV2.iMode=CTelephony::ENetworkModeGsm;
	ExpectedNetworkInfoV2.iStatus = CTelephony::ENetworkStatusCurrent;
	ExpectedNetworkInfoV2.iBandInfo=CTelephony::EBandUnknown;
	ExpectedNetworkInfoV2.iCdmaSID.SetLength(0);
	ExpectedNetworkInfoV2.iAccess = CTelephony::ENetworkAccessGsm; //If v1=ENetworkAccessUnknown, if V2=ENetworkAccessGsm
	ExpectedNetworkInfoV2.iAreaKnown = EFalse;
	ExpectedNetworkInfoV2.iLocationAreaCode = 0;
	ExpectedNetworkInfoV2.iCellId = 0;
	
	TRequestStatus ExpStat=KErrNone;
	// Create an active object and push it onto the clean up stack
	CTestGetCurrentNetworkInfoActEighteenCharsV2* GetCurrentNetworkInfoActEighteenCharsV2 = CTestGetCurrentNetworkInfoActEighteenCharsV2::NewLC(this, iTelephony);
	
	// Set testStage to 1 to indicate testing GetNetworkInfo with TNetworkInfoV2
	GetCurrentNetworkInfoActEighteenCharsV2->SetTestStage(1);
	
	// Attempt to retrieve the network info. Pass the expected values. 
	ExpStat=KErrNone;
	GetCurrentNetworkInfoActEighteenCharsV2->TestGetCurrentNetworkInfoEighteenCharsV2(ExpectedNetworkInfoV2, ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	//
	// Test GetCurrentNetworkName with V2
	//
	INFO_PRINTF1(_L("Test retrieval of current network name when long network name passed in is 18 Chars long"));

	// Set up the expected value to be the same as that provisioned in the SIM TSY config.txt file.
	_LIT16(networkNameV2, "SymbianNetworkABCD"); 
	CTelephony::TNetworkNameV2 ExpectedNetworkNameV2;
	ExpectedNetworkNameV2.iNetworkName=networkNameV2;
	ExpStat=KErrNone;
	
	// Set testStage to 2 to indicate testing GetNetworkName with TNetworkNameV2
	GetCurrentNetworkInfoActEighteenCharsV2->SetTestStage(2);

	ExpStat=KErrNone;
	// Attempt to retrieve the network name. Pass the expected values. 
	GetCurrentNetworkInfoActEighteenCharsV2->TestGetCurrentNetworkNameEighteenCharsV2(ExpectedNetworkNameV2,ExpStat,ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(); //GetCurrentNetworkInfoActEighteenCharsV2
	return TestStepResult();
	}	

/**
Default constructor.

@param aTestStep Pointer to the test step that owns this object
@param aTelephony Pointer to CTelephony object whose functions are being tested
@param aUseMacros Specifies whether to use TestExecute logger macros
@return Pointer to instance of CTestGetCurrentNetworkInfoActEighteenCharsV2
*/
CTestGetCurrentNetworkInfoActEighteenCharsV2::CTestGetCurrentNetworkInfoActEighteenCharsV2(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iUseMacros(aUseMacros)	 
	{  
	}

/**
Factory constructor.

@param aTestStep Pointer to the test step that owns this object
@param aTelephony Pointer to CTelephony object whose functions are being tested
@param aUseMacros Specifies whether to use TestExecute logger macros
@return Pointer to instance of CTestGetCurrentNetworkInfoActEighteenCharsV2
*/
CTestGetCurrentNetworkInfoActEighteenCharsV2* CTestGetCurrentNetworkInfoActEighteenCharsV2::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	{
	CTestGetCurrentNetworkInfoActEighteenCharsV2* obj = new(ELeave) CTestGetCurrentNetworkInfoActEighteenCharsV2(aTestStep, aTelephony, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

void CTestGetCurrentNetworkInfoActEighteenCharsV2::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

/**
Destructor calls Cancel() to clean up.
*/
CTestGetCurrentNetworkInfoActEighteenCharsV2::~CTestGetCurrentNetworkInfoActEighteenCharsV2()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

/** 
Calls CTelephony::CancelAsync with relevant parameter depending on the current test stage
*/
void CTestGetCurrentNetworkInfoActEighteenCharsV2::DoCancel()
	{
	if(iTestStage==1)
		{
		iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
		}
	else if (iTestStage==2)
		{
		iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
		}
	}

/**
When iTestStage = 1 tests GetCurrentNetworkInfo with TNetworkInfoV2
When iTestStage = 2 tests GetNetworkName with TNetworkNameV2
*/
void CTestGetCurrentNetworkInfoActEighteenCharsV2::RunL()
	{
	
	if (iTestStage==1)
		{
	    if(iUseMacros)
			{ 
			iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
			}

		if(iStatus.Int() == KErrNone)
			{
		 
			if(iUseMacros)
				{  
				// Print off the values retrieved and those expected
			
				iTestStep->INFO_PRINTF1(_L("Network Info.........."));
				iTestStep->INFO_PRINTF2(_L("iCountryCode retrieved: %S"), &iNetworkInfoV2.iCountryCode );
				iTestStep->INFO_PRINTF2(_L("iCountryCode expected: %S"), &iExpectedNetworkInfoV2.iCountryCode );
				iTestStep->INFO_PRINTF2(_L("iNetworkId retrieved: %S"), &iNetworkInfoV2.iNetworkId );
				iTestStep->INFO_PRINTF2(_L("iNetworkId expected: %S"), &iExpectedNetworkInfoV2.iNetworkId );
				iTestStep->INFO_PRINTF2(_L("iDisplayTag retrieved: %S"), &iNetworkInfoV2.iDisplayTag );
				iTestStep->INFO_PRINTF2(_L("iDisplayTag expected: %S"), &iExpectedNetworkInfoV2.iDisplayTag );
				iTestStep->INFO_PRINTF2(_L("iShortName retrieved: %S"), &iNetworkInfoV2.iShortName );
				iTestStep->INFO_PRINTF2(_L("iShortName expected: %S"), &iExpectedNetworkInfoV2.iShortName );
				iTestStep->INFO_PRINTF2(_L("iLongName retrieved: %S"), &iNetworkInfoV2.iLongName );
				iTestStep->INFO_PRINTF2(_L("iLongName expected: %S"), &iExpectedNetworkInfoV2.iLongName );		
				iTestStep->INFO_PRINTF2(_L("iMode retrieved: %d"), iNetworkInfoV2.iMode);
				iTestStep->INFO_PRINTF2(_L("iMode expected: %d"), iExpectedNetworkInfoV2.iMode);
				iTestStep->INFO_PRINTF2(_L("iStatus retrieved: %d"), iNetworkInfoV2.iStatus);
				iTestStep->INFO_PRINTF2(_L("iStatus expected: %d"), iExpectedNetworkInfoV2.iStatus);
				iTestStep->INFO_PRINTF2(_L("iBandInfo retrieved: %d"), iNetworkInfoV2.iBandInfo);
				iTestStep->INFO_PRINTF2(_L("iBandInfo expected: %d"), iExpectedNetworkInfoV2.iBandInfo);
				iTestStep->INFO_PRINTF2(_L("iCdmaSID retrieved: %S"), &iNetworkInfoV2.iCdmaSID);
				iTestStep->INFO_PRINTF2(_L("iCdmaSID expected: %S"), &iExpectedNetworkInfoV2.iCdmaSID);
				iTestStep->INFO_PRINTF2(_L("iAccess retrieved: %d"), iNetworkInfoV2.iAccess);
				iTestStep->INFO_PRINTF2(_L("iAccess expected: %d"), iExpectedNetworkInfoV2.iAccess);
				iTestStep->INFO_PRINTF2(_L("iAreaKnown retrieved: %d"), iNetworkInfoV2.iAreaKnown);
				iTestStep->INFO_PRINTF2(_L("iAreaKnown expected: %d"), iExpectedNetworkInfoV2.iAreaKnown);
				iTestStep->INFO_PRINTF2(_L("iLocationAreaCode retrieved: %d"), iNetworkInfoV2.iLocationAreaCode);
				iTestStep->INFO_PRINTF2(_L("iLocationAreaCode expected: %d"), iExpectedNetworkInfoV2.iLocationAreaCode);
				iTestStep->INFO_PRINTF2(_L("iCellId retrieved: %d"), iNetworkInfoV2.iCellId);
				iTestStep->INFO_PRINTF2(_L("iCellId expected: %d"), iExpectedNetworkInfoV2.iCellId);
			
				
				// Check that the retrieved values match those expected.
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iCountryCode == iExpectedNetworkInfoV2.iCountryCode, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iNetworkId == iExpectedNetworkInfoV2.iNetworkId, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iDisplayTag == iExpectedNetworkInfoV2.iDisplayTag, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iShortName == iExpectedNetworkInfoV2.iShortName, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iLongName == iExpectedNetworkInfoV2.iLongName, ETrue);		
				iTestStep->TEST1(iNetworkInfoV2.iMode = iExpectedNetworkInfoV2.iMode, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iStatus == iExpectedNetworkInfoV2.iStatus, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iBandInfo == iExpectedNetworkInfoV2.iBandInfo, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iCdmaSID == iExpectedNetworkInfoV2.iCdmaSID, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iAccess == iExpectedNetworkInfoV2.iAccess, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iAreaKnown == iExpectedNetworkInfoV2.iAreaKnown, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iLocationAreaCode == iExpectedNetworkInfoV2.iLocationAreaCode, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iCellId == iExpectedNetworkInfoV2.iCellId, ETrue);
				}	 	 
			}
		else
			{
			if(iUseMacros)
				{ 
				iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				}
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			}


		}
	else if (iTestStage==2)
		{
		iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		
		if(iStatus.Int() == KErrNone)
			{
			 
			// Print off the values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Network Name expected: %S"), &iExpectedNetworkNameV2.iNetworkName) ;
			iTestStep->INFO_PRINTF2(_L("Network Name retrieved: %S"),  &iNetworkNameV2.iNetworkName);
			
			// Check that the retrieved values match those expected.
			iTestStep->TEST1(iNetworkNameV2.iNetworkName == iExpectedNetworkNameV2.iNetworkName, ETrue);
		 	 
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
			iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			}
		}
	
	CActiveScheduler::Stop();
	}

/** 
Sets the test stage
1. Tests GetCurrentNetworkInfo using TNetworkInfoV2
2. Tests GetCurrentNetworkName using TNetworkNameV2
@param The stage of the test step to be executed
*/
void CTestGetCurrentNetworkInfoActEighteenCharsV2::SetTestStage(TInt aTestStage)
	{
	iTestStage = aTestStage;
	}

/**
Makes the call to 3rd party API function CTelephony::GetCurrentNetworkInfo for testing
@param aNetworkInfo The CTelephony::TNetworkInfoV2 object filled by the CTelephony::GetCurrentNetworkInfo function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specify whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActEighteenCharsV2::TestGetCurrentNetworkInfoEighteenCharsV2(const CTelephony::TNetworkInfoV2& aNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkInfoV2=aNetworkInfo;
	iExpStat=aExpStat;
	  
	CTelephony::TNetworkInfoV2Pckg NetworkInfoPckg(iNetworkInfoV2);

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkInfo(iStatus, NetworkInfoPckg );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkInfo(tmpStatus, NetworkInfoPckg );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}
	
/**
Makes the call to 3rd party API function CTelephony::GetCurrentNetworkName for testing
@param aNetworkName The CTelephony::TNetworkInfoV2 object filled by the CTelephony::GetCurrentNetworkName function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specify whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActEighteenCharsV2::TestGetCurrentNetworkNameEighteenCharsV2(CTelephony::TNetworkNameV2 aNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkNameV2=aNetworkName;
	iExpStat=aExpStat;
	  
	CTelephony::TNetworkNameV2Pckg NetworkNamePckg(iNetworkNameV2);

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkName(iStatus, NetworkNamePckg );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkName(tmpStatus, NetworkNamePckg );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}
	
	
	
//////////////////////////////////////////////////////////////////
/// RETRIEVE NETWORK INFORMATION  ////////////////////////////////
//////////////////////////////////////////////////////////////////
/**
Each test step initialises it's own name.
*/
CTestGetCurrentNetworkInfoTwentyThreeCharsV2::CTestGetCurrentNetworkInfoTwentyThreeCharsV2()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetNetworkInfoTwentyThreeCharsV2"));
	}

/**
Test consists of two stages:
1. Test GetCurrentNetworkInfo truncates the network name when its length is
greater than the length of TNetworkInfoV2::iShortName, TNetworkInfoV2::iLongName
and TMobilePhoneNetworkInfoV1::iShortName and iLongName
2. Test GetCurrentNetworkName truncates the network name when its length is greater
than the length of TNetworkInfoV2::iNetworkName

@return The verdict of the test step
*/
enum TVerdict CTestGetCurrentNetworkInfoTwentyThreeCharsV2::doTestStepL()
	{
	INFO_PRINTF1(_L("Test retrieval of current network information, operator name and network name when the long network name is twenty three characters long"));
	
	// Set the config.txt test number mapping
	SetTestNumber(37);

	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(KCountrycode, "ABCD");
	_LIT16(KNetworkid, "Network0");

	//
	// Test GetCurrentNetworkInfoV2
	//
	_LIT16(KShortnameV2, "SymbianNet"); // Up to first 10 characters from test data
	_LIT16(KLongnameV2, "SymbianNetworkABCDEF"); // Up to first 20 characters from test data
	// Need to test display tag
	_LIT16(KDisplaytag, "Vodafone");
	
	CTelephony::TNetworkInfoV2 ExpectedNetworkInfoV2;
	ExpectedNetworkInfoV2.iCountryCode=KCountrycode;
	ExpectedNetworkInfoV2.iNetworkId=KNetworkid;
	ExpectedNetworkInfoV2.iDisplayTag=KDisplaytag;
	ExpectedNetworkInfoV2.iShortName=KShortnameV2;
	ExpectedNetworkInfoV2.iLongName=KLongnameV2;	
	ExpectedNetworkInfoV2.iMode=CTelephony::ENetworkModeGsm;
	ExpectedNetworkInfoV2.iStatus = CTelephony::ENetworkStatusCurrent;
	ExpectedNetworkInfoV2.iBandInfo=CTelephony::EBandUnknown;
	ExpectedNetworkInfoV2.iCdmaSID.SetLength(0);
	ExpectedNetworkInfoV2.iAccess = CTelephony::ENetworkAccessGsm; //If v1=ENetworkAccessUnknown, if V2=ENetworkAccessGsm
	ExpectedNetworkInfoV2.iAreaKnown = EFalse;
	ExpectedNetworkInfoV2.iLocationAreaCode = 0;
	ExpectedNetworkInfoV2.iCellId = 0;
	
	TRequestStatus ExpStat=KErrNone;

	// Create an active object and push it onto the clean up stack
	CTestGetCurrentNetworkInfoActTwentyThreeCharsV2* GetCurrentNetworkInfoActTwentyThreeCharsV2 = CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::NewLC(this, iTelephony);
	
	// Set testStage to 1 to indicate testing GetNetworkInfo with TNetworkInfoV2
	GetCurrentNetworkInfoActTwentyThreeCharsV2->SetTestStage(1);
	
	// Attempt to retrieve the network info. Pass the expected values. 
	ExpStat=KErrNone;
	GetCurrentNetworkInfoActTwentyThreeCharsV2->TestGetCurrentNetworkInfoTwentyThreeCharsV2(ExpectedNetworkInfoV2, ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	//
	// Test GetCurrentNetworkName with V2
	//
	INFO_PRINTF1(_L("Test retrieval of current network name when long network name passed in is 23 characters long"));

	// Set up the expected value to be the same as that provisioned in the SIM TSY config.txt file.
	_LIT16(networkNameV2, "SymbianNetworkABCDEF"); 
	CTelephony::TNetworkNameV2 ExpectedNetworkNameV2;
	ExpectedNetworkNameV2.iNetworkName=networkNameV2;
	ExpStat=KErrNone;
	
	// Set testStage to 2 to indicate testing GetNetworkName with TNetworkNameV2
	GetCurrentNetworkInfoActTwentyThreeCharsV2->SetTestStage(2);

	ExpStat=KErrNone;
	// Attempt to retrieve the network name. Pass the expected values. 
	GetCurrentNetworkInfoActTwentyThreeCharsV2->TestGetCurrentNetworkNameTwentyThreeCharsV2(ExpectedNetworkNameV2,ExpStat,ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); //GetCurrentNetworkInfoActTwentyThreeChars
	return TestStepResult();
	}

/**
Sets the stage of the test step
@param The stage of the test step to be executed
*/
void CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::SetTestStage(TInt aTestStage)
	{
	iTestStage = aTestStage;
	}

/**
Factory constructor
@param aTestStep Pointer to the test step that owns this object
@param aTelephony Pointer to CTelephony object whose functions are being tested
@param aUseMacros Specifies whether to use TestExecute logger macros
@return Pointer to instance of CTestGetCurrentNetworkInfoActTwentyThreeCharsV2
*/
CTestGetCurrentNetworkInfoActTwentyThreeCharsV2* CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	{
	CTestGetCurrentNetworkInfoActTwentyThreeCharsV2* obj = new(ELeave) CTestGetCurrentNetworkInfoActTwentyThreeCharsV2(aTestStep, aTelephony, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

/**
Constructor called by NewLC
@param aTestStep Pointer to the test step that owns this object
@param aTelephony Pointer to CTelephony object whose functions are being tested
@param aUseMacros Specifies whether to use TestExecute logger macros
@return Pointer to instance of CTestGetCurrentNetworkInfoActTwentyThreeCharsV2
*/
CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::CTestGetCurrentNetworkInfoActTwentyThreeCharsV2(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iUseMacros(aUseMacros)	 
	{  
	}

void CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

/**
Destructor calls Cancel() to clean up
*/	
CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::~CTestGetCurrentNetworkInfoActTwentyThreeCharsV2()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

/**
Calls CTelephony::CancelAsync with the relevant parameter depending on the current test stage
*/
void CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::DoCancel()
	{
	if(iTestStage==1)
		iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
	else if (iTestStage==2)
		iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
	}	

/**
Makes the call to 3rd party API function CTelephony::GetCurrentNetworkInfo for testing
@param aNetworkInfo The CTelephony::TNetworkInfoV2 object filled by the CTelephony::GetCurrentNetworkInfo function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specifies whether to test when a simultaneous function call is made
*/	
void CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::TestGetCurrentNetworkInfoTwentyThreeCharsV2(const CTelephony::TNetworkInfoV2& aNetworkInfo, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkInfoV2=aNetworkInfo;
	iExpStat=aExpStat;
	  
	CTelephony::TNetworkInfoV2Pckg NetworkInfoPckg(iNetworkInfoV2);

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkInfo(iStatus, NetworkInfoPckg );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkInfo(tmpStatus, NetworkInfoPckg );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}

/**
Makes the call to 3rd party API function CTelephony::GetCurrentNetworkName for testing
@param aNetworkName The CTelephony::TNetworkInfoV2 object filled by the CTelephony::GetCurrentNetworkName function
@param aExpStat The result code after the asynchronous call completes
@param aCheckConcurrency Specifies whether to test when a simultaneous function call is made
*/
void CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::TestGetCurrentNetworkNameTwentyThreeCharsV2(const CTelephony::TNetworkNameV2& aNetworkName, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	// Set the expected return values
	iExpectedNetworkNameV2=aNetworkName;
	iExpStat=aExpStat;
	  
	CTelephony::TNetworkNameV2Pckg NetworkNamePckg(iNetworkNameV2);

	// Make a call to the 3rd party API
	iTelephony->GetCurrentNetworkName(iStatus, NetworkNamePckg );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCurrentNetworkName(tmpStatus, NetworkNamePckg );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}
	
/**
When iTestStage = 1 tests GetCurrentNetworkInfo with TNetworkInfoV2
When iTestStage = 2 tests GetNetworkName with TNetworkNameV2
*/	
void CTestGetCurrentNetworkInfoActTwentyThreeCharsV2::RunL()
	{ 
    if (iTestStage==1)
		{
	    if(iUseMacros)
			{ 
			iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
			}
	
		if(iStatus.Int() == KErrNone)
			{
		 
			if(iUseMacros)
				{  
				// Print off the values retrieved and those expected
			
				iTestStep->INFO_PRINTF1(_L("Network Info.........."));
				iTestStep->INFO_PRINTF2(_L("iCountryCode retrieved: %S"), &iNetworkInfoV2.iCountryCode );
				iTestStep->INFO_PRINTF2(_L("iCountryCode expected: %S"), &iExpectedNetworkInfoV2.iCountryCode );
				iTestStep->INFO_PRINTF2(_L("iNetworkId retrieved: %S"), &iNetworkInfoV2.iNetworkId );
				iTestStep->INFO_PRINTF2(_L("iNetworkId expected: %S"), &iExpectedNetworkInfoV2.iNetworkId );
				iTestStep->INFO_PRINTF2(_L("iDisplayTag retrieved: %S"), &iNetworkInfoV2.iDisplayTag );
				iTestStep->INFO_PRINTF2(_L("iDisplayTag expected: %S"), &iExpectedNetworkInfoV2.iDisplayTag );
				iTestStep->INFO_PRINTF2(_L("iShortName retrieved: %S"), &iNetworkInfoV2.iShortName );
				iTestStep->INFO_PRINTF2(_L("iShortName expected: %S"), &iExpectedNetworkInfoV2.iShortName );
				iTestStep->INFO_PRINTF2(_L("iLongName retrieved: %S"), &iNetworkInfoV2.iLongName );
				iTestStep->INFO_PRINTF2(_L("iLongName expected: %S"), &iExpectedNetworkInfoV2.iLongName );		
				iTestStep->INFO_PRINTF2(_L("iMode retrieved: %d"), iNetworkInfoV2.iMode);
				iTestStep->INFO_PRINTF2(_L("iMode expected: %d"), iExpectedNetworkInfoV2.iMode);
				iTestStep->INFO_PRINTF2(_L("iStatus retrieved: %d"), iNetworkInfoV2.iStatus);
				iTestStep->INFO_PRINTF2(_L("iStatus expected: %d"), iExpectedNetworkInfoV2.iStatus);
				iTestStep->INFO_PRINTF2(_L("iBandInfo retrieved: %d"), iNetworkInfoV2.iBandInfo);
				iTestStep->INFO_PRINTF2(_L("iBandInfo expected: %d"), iExpectedNetworkInfoV2.iBandInfo);
				iTestStep->INFO_PRINTF2(_L("iCdmaSID retrieved: %S"), &iNetworkInfoV2.iCdmaSID);
				iTestStep->INFO_PRINTF2(_L("iCdmaSID expected: %S"), &iExpectedNetworkInfoV2.iCdmaSID);
				iTestStep->INFO_PRINTF2(_L("iAccess retrieved: %d"), iNetworkInfoV2.iAccess);
				iTestStep->INFO_PRINTF2(_L("iAccess expected: %d"), iExpectedNetworkInfoV2.iAccess);
				iTestStep->INFO_PRINTF2(_L("iAreaKnown retrieved: %d"), iNetworkInfoV2.iAreaKnown);
				iTestStep->INFO_PRINTF2(_L("iAreaKnown expected: %d"), iExpectedNetworkInfoV2.iAreaKnown);
				iTestStep->INFO_PRINTF2(_L("iLocationAreaCode retrieved: %d"), iNetworkInfoV2.iLocationAreaCode);
				iTestStep->INFO_PRINTF2(_L("iLocationAreaCode expected: %d"), iExpectedNetworkInfoV2.iLocationAreaCode);
				iTestStep->INFO_PRINTF2(_L("iCellId retrieved: %d"), iNetworkInfoV2.iCellId);
				iTestStep->INFO_PRINTF2(_L("iCellId expected: %d"), iExpectedNetworkInfoV2.iCellId);
			
				
				// Check that the retrieved values match those expected.
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iCountryCode == iExpectedNetworkInfoV2.iCountryCode, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iNetworkId == iExpectedNetworkInfoV2.iNetworkId, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iDisplayTag == iExpectedNetworkInfoV2.iDisplayTag, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iShortName == iExpectedNetworkInfoV2.iShortName, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iLongName == iExpectedNetworkInfoV2.iLongName, ETrue);		
				iTestStep->TEST1(iNetworkInfoV2.iMode = iExpectedNetworkInfoV2.iMode, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iStatus == iExpectedNetworkInfoV2.iStatus, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iBandInfo == iExpectedNetworkInfoV2.iBandInfo, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iCdmaSID == iExpectedNetworkInfoV2.iCdmaSID, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iAccess == iExpectedNetworkInfoV2.iAccess, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iAreaKnown == iExpectedNetworkInfoV2.iAreaKnown, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iLocationAreaCode == iExpectedNetworkInfoV2.iLocationAreaCode, ETrue);
				iTestStep->TEST1(iNetworkInfoV2.iCellId == iExpectedNetworkInfoV2.iCellId, ETrue);
				}	 	 
			}
		else
			{
			if(iUseMacros)
				{ 
				iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				}
			else
				{
				iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
				}
			}
		} // End iTestStage==2
	else if (iTestStage==2)
		{
		iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		
		if(iStatus.Int() == KErrNone)
			{
			 
			// Print off the values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Network Name expected: %S"), &iExpectedNetworkNameV2.iNetworkName) ;
			iTestStep->INFO_PRINTF2(_L("Network Name retrieved: %S"),  &iNetworkNameV2.iNetworkName);
			
			// Check that the retrieved values match those expected.
			iTestStep->TEST1(iNetworkNameV2.iNetworkName == iExpectedNetworkNameV2.iNetworkName, ETrue);
		 	 
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
			iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
			}
		}

	CActiveScheduler::Stop();
	}
