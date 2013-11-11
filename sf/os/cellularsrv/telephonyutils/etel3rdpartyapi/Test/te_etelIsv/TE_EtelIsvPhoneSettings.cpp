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
// Tests for ETL 3rd Party Phone APIs  
// 
//

/**
 @file
*/

#include "TE_EtelIsvbase.h"
#include "TE_EtelIsvPhoneSettings.h"
#include "TE_EtelIsvCallFunc.h"

 

CTestGetPhoneId::CTestGetPhoneId()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetPhoneId"));
}

/**
*
@SYMTestCaseID Etel3rdParty 1.1
@SYMTestCaseDesc Test GetPhoneId 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.1.1
*/ 

TVerdict CTestGetPhoneId::doTestStepL()
{
	SetTestNumber(0);

	//expected data
	_LIT16(manufacturerid, "PSION");
	_LIT16(modelid, "666");
	_LIT16(serialid, "1234567890");
	CTelephony::TPhoneIdV1 ExpectedPhoneId;
	ExpectedPhoneId.iManufacturer=manufacturerid;
	ExpectedPhoneId.iModel=modelid;
	ExpectedPhoneId.iSerialNumber=serialid;
	TRequestStatus expStat=KErrNone;
	CTelephony::TPhoneIdV1Pckg ExpectedPhoneIdPckg(ExpectedPhoneId);
	
	CTestGetPhoneIdAct* GetPhoneIdAct = CTestGetPhoneIdAct::NewLC(this, iTelephony);

	#if defined(_DEBUG)
	//Test OOM
		INFO_PRINTF1(_L("Test OOM for GetPhoneId"));
		expStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetPhoneIdAct->GetPhoneId(ExpectedPhoneIdPckg, expStat);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	//Start GetPhoneID AO	
	INFO_PRINTF1(_L("Test GetPhoneId"));
	expStat=KErrNone;
	GetPhoneIdAct->GetPhoneId(ExpectedPhoneIdPckg, expStat, ETrue);
	CActiveScheduler::Start();	
	
	//Test cancel
	INFO_PRINTF1(_L("Test Cancel for GetPhoneId"));
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetPhoneIdCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetPhoneIdAct->GetPhoneId(ExpectedPhoneIdPckg, expStat);	
	CTestCanceller* canceller = GetPhoneIdAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetPhoneIdAct->iStatus.Int() == KErrCancel || GetPhoneIdAct->iStatus.Int() == KErrNone, ETrue);

	CleanupStack::PopAndDestroy(); //GetPhoneIdAct

	return TestStepResult();
}

CTestGetPhoneIdError::CTestGetPhoneIdError()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetPhoneIdError"));
}

/**
*
@SYMTestCaseID Etel3rdParty 1.2
@SYMTestCaseDesc Test GetPhoneId 3rd party API - Error scenario
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc,  REQ 2116.1.1
*/

TVerdict CTestGetPhoneIdError::doTestStepL()
{
	INFO_PRINTF1(_L("Test GetPhoneId with error"));
	SetTestNumber(1);
	CTelephony::TPhoneIdV1 ExpectedPhoneId;
	CTelephony::TPhoneIdV1Pckg ExpectedPhoneIdPckg(ExpectedPhoneId);
	TRequestStatus expStat=KErrNotFound;

	//Start GetPhoneID AO
	CTestGetPhoneIdAct* GetPhoneIdAct = CTestGetPhoneIdAct::NewLC(this, iTelephony);
	GetPhoneIdAct->GetPhoneId(ExpectedPhoneIdPckg, expStat);

	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(); //GetPhoneIdAct
	return TestStepResult();
}

//=====================================================
//
//	Class to get phone ID active object
//
//=====================================================

CTestGetPhoneIdAct* CTestGetPhoneIdAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
//Factory constructor
	{
	CTestGetPhoneIdAct* obj = new(ELeave) CTestGetPhoneIdAct(aTestStep, aTelephony, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
CTestGetPhoneIdAct::CTestGetPhoneIdAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros = ETrue)
	: CEtelIsvActBase(aTestStep, aTelephony), 
	iUseMacros(aUseMacros),
	iPhoneIdBuff(iPhoneId),
	iPhoneIdPckg(iPhoneId)
	{  
	}

void CTestGetPhoneIdAct::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetPhoneIdAct::~CTestGetPhoneIdAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetPhoneIdAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetPhoneIdCancel);
	}
	
void CTestGetPhoneIdAct::GetPhoneId(TDes8& aExpectedPhoneIdPckg, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	iExpectedPhoneIdPckg=&aExpectedPhoneIdPckg;
	iExpStat=aExpStat;
	
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iPhoneIdBuff;
		}
	else
		{
		data = &iPhoneIdPckg;
		}
	
	iTelephony->GetPhoneId(iStatus, *data);
	
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetPhoneId(tmpStatus, *data);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}		
	SetActive();
	}

void CTestGetPhoneIdAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	CTelephony::TPhoneIdV1Pckg& GetPhoneIdV1Pckg = REINTERPRET_CAST(CTelephony::TPhoneIdV1Pckg&, *iExpectedPhoneIdPckg);
	
	//only supports V1 at present
	CTelephony::TPhoneIdV1& expectedPhoneId = (GetPhoneIdV1Pckg)();	 
	
	if(iUseMacros)
		{	
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		}

	if(iStatus.Int() == KErrNone)
		{
		if(iUseMacros)
			{	
			const CTelephony::TPhoneIdV1* data;
			if (iTestStep->iBufferMode)
				{
				data = reinterpret_cast<const CTelephony::TPhoneIdV1*> ( &iPhoneIdBuff() );
				}
			else
				{
				data = reinterpret_cast<const CTelephony::TPhoneIdV1*> ( &iPhoneIdPckg() );
				}
		    
			iTestStep->TEST1(data->iManufacturer == expectedPhoneId.iManufacturer, ETrue);
			iTestStep->TEST1(data->iModel == expectedPhoneId.iModel, ETrue);
			iTestStep->TEST1(data->iSerialNumber == expectedPhoneId.iSerialNumber, ETrue);		
			
			iTestStep->INFO_PRINTF2(_L("Phone manufacturer: %S"), &(data->iManufacturer) );
			iTestStep->INFO_PRINTF2(_L("Phone model: %S"), &(data->iModel) );
			iTestStep->INFO_PRINTF2(_L("Phone serial number: %S"), &(data->iSerialNumber) );
			}
		}
	else
		{
		if(iUseMacros)
			{
			iTestStep->INFO_PRINTF2(_L("Error for GetPhoneId: %d"), iStatus.Int() );
			}
		}

	CActiveScheduler::Stop();
	}

CTestGetSubscriberId::CTestGetSubscriberId()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetSubscriberId"));
}

 
 
 
/**
*
@SYMTestCaseID Etel3rdParty 2.1
@SYMTestCaseDesc Test GetSubscriberId 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.1.2
*
*/ 

TVerdict CTestGetSubscriberId::doTestStepL()
	{
	//Set test number
	SetTestNumber(2);
	
	//Set expected IMSI value
	_LIT16(SubscriberId, "56789012345678");
	CTelephony::TSubscriberIdV1 ExpSubscriberId ;
	ExpSubscriberId.iSubscriberId=SubscriberId;
	CTelephony::TSubscriberIdV1Pckg expSubscriberIdPckg(ExpSubscriberId);
	TRequestStatus expStat=KErrNone;
	
	CTestGetSubscriberIdAct* GetSubscriberIdAct = CTestGetSubscriberIdAct::NewLC(this, iTelephony);

	#if defined(_DEBUG)
	//Test OOM
		INFO_PRINTF1(_L("Test OOM for GetSubscriberId"));
		expStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetSubscriberIdAct->GetSubscriberId(expSubscriberIdPckg, expStat);	
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	//Start GetSubscriberID AO
	INFO_PRINTF1(_L("Test GetSubscriberId"));
	expStat=KErrNone;
	GetSubscriberIdAct->GetSubscriberId(expSubscriberIdPckg, expStat, ETrue);	
	CActiveScheduler::Start();

	//Test cancel
	INFO_PRINTF1(_L("Test Cancel for GetSubscriberId"));
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetSubscriberIdCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	CTestCanceller* canceller = GetSubscriberIdAct->RetrieveCanceller();
	GetSubscriberIdAct->GetSubscriberId(expSubscriberIdPckg, expStat);	
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetSubscriberIdAct->iStatus.Int() == KErrCancel || GetSubscriberIdAct->iStatus.Int() == KErrNone, ETrue);

	CleanupStack::PopAndDestroy(); //GetSubscriberIdAct
	return TestStepResult();
}

CTestGetSubscriberIdError::CTestGetSubscriberIdError()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetSubscriberIdError"));
}

/**
*
@SYMTestCaseID Etel3rdParty 2.2
@SYMTestCaseDesc Test GetSubscriberId 3rd party API - Error scenario
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.1.2
*
*/ 

TVerdict CTestGetSubscriberIdError::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Cancel for GetSubscriberId with error."));

	//Set test number
	SetTestNumber(3);

	//Set expected IMSI value
	_LIT16(SubscriberId, "01234567890123");
	CTelephony::TSubscriberIdV1 ExpSubscriberId ;
	ExpSubscriberId.iSubscriberId=SubscriberId;
	CTelephony::TSubscriberIdV1Pckg expSubscriberIdPckg(ExpSubscriberId);

	//Start GetSubscriberID AO
	TRequestStatus expStat=KErrNotSupported;
	CTestGetSubscriberIdAct* GetSubscriberIdAct = CTestGetSubscriberIdAct::NewLC(this, iTelephony);
	GetSubscriberIdAct->GetSubscriberId(expSubscriberIdPckg, expStat);
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); //GetSubscriberIdAct
	return TestStepResult() ;
}

//=====================================================
//
//	Class to get subscriber ID active object
//
//=====================================================

CTestGetSubscriberIdAct* CTestGetSubscriberIdAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetSubscriberIdAct* obj = new(ELeave) CTestGetSubscriberIdAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestGetSubscriberIdAct::CTestGetSubscriberIdAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iSubscriberIdBuff(iSubscriberId),
	iSubscriberIdPckg(iSubscriberId)
	{  
	}

void CTestGetSubscriberIdAct::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetSubscriberIdAct::~CTestGetSubscriberIdAct()
	{ 
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetSubscriberIdAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetSubscriberIdCancel);
	}

void CTestGetSubscriberIdAct::GetSubscriberId(TDes8& aExpSubscriberIdPckg, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	iExpStat = aExpStat;
	iExpSubscriberIdPckg=&aExpSubscriberIdPckg;
	
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iSubscriberIdBuff;
		}
	else
		{
		data = &iSubscriberIdPckg;
		}

	iTelephony->GetSubscriberId(iStatus, *data);
	
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetSubscriberId(tmpStatus, *data);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	
	SetActive();
	}

void CTestGetSubscriberIdAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	CTelephony::TSubscriberIdV1Pckg& expSubscriberIdPckg = REINTERPRET_CAST(CTelephony::TSubscriberIdV1Pckg&, *iExpSubscriberIdPckg);
	
	//only supports V1 at present
	CTelephony::TSubscriberIdV1& expSubscriberId = (expSubscriberIdPckg)();	
	
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);

	if(iStatus.Int() == KErrNone)
		{
		CTelephony::TSubscriberIdV1* data;
		if (iTestStep->iBufferMode)
			{
			data = reinterpret_cast<CTelephony::TSubscriberIdV1*> ( &iSubscriberIdBuff() );
			}
		else
			{
			data = reinterpret_cast<CTelephony::TSubscriberIdV1*> ( &iSubscriberIdPckg() );
			}		
	        
		iTestStep->TEST1(data->iSubscriberId == expSubscriberId.iSubscriberId, ETrue);		
		iTestStep->INFO_PRINTF2(_L("SubscriberId: %S"), &(data->iSubscriberId) );
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for GeSubscriberId: %d"), iStatus.Int() );
		}

	CActiveScheduler::Stop();
	}

CTestGetIndicator::CTestGetIndicator()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetIndicator"));
}

/**
*
@SYMTestCaseID Etel3rdParty 4.1
@SYMTestCaseDesc Test GetIndicator 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.1.4
*
*/ 

TVerdict CTestGetIndicator::doTestStepL()
	{
	//Set test number
	SetTestNumber(4);

	//Set expected return values
	TUint32 expIndCaps = CTelephony::KIndChargerConnected | CTelephony::KIndNetworkAvailable | CTelephony::KIndCallInProgress;
	TUint32 expInd = CTelephony::KIndChargerConnected;
	TRequestStatus expStat=KErrNone;
	
	CTestGetIndicatorAct* GetIndicatorAct = CTestGetIndicatorAct::NewLC(this, iTelephony);

	#if defined(_DEBUG)
	//test OOM
		INFO_PRINTF1(_L("Test OOM for GetIndicator"));
		expStat = KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetIndicatorAct->GetPhoneIndicator(expIndCaps, expInd, expStat);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif

	//Start GetIndicator AO
	 expStat=KErrNone;
	INFO_PRINTF1(_L("Test GetIndicator"));	
	GetIndicatorAct->GetPhoneIndicator(expIndCaps, expInd, expStat, ETrue);
	CActiveScheduler::Start();

	//Test cancel
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetIndicatorCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetIndicatorAct->GetPhoneIndicator(expIndCaps, expInd, expStat);
	CTestCanceller* canceller = GetIndicatorAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetIndicatorAct->iStatus.Int() == KErrCancel || GetIndicatorAct->iStatus.Int() == KErrNone, ETrue);

	CleanupStack::PopAndDestroy(); //GetIndicatorAct
	return TestStepResult() ;
}

//=====================================================
//
//	Class to get phone indicator active object
//
//=====================================================

CTestGetIndicatorAct* CTestGetIndicatorAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetIndicatorAct* obj = new(ELeave) CTestGetIndicatorAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestGetIndicatorAct::CTestGetIndicatorAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iIndicatorBuff(iIndicator),
	iIndicatorPckg(iIndicator)
	{  
	}

void CTestGetIndicatorAct::ConstructL()
	{ 
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetIndicatorAct::~CTestGetIndicatorAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetIndicatorAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetIndicatorCancel);
	}

void CTestGetIndicatorAct::GetPhoneIndicator(TUint32 aExpIndCaps, TUint32 aExpInd,TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	iExpIndCaps = aExpIndCaps;
	iExpInd = aExpInd;
	iExpStat = aExpStat;
	
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iIndicatorBuff;
		}
	else
		{
		data = &iIndicatorPckg;
		}
	
	iTelephony->GetIndicator(iStatus, *data);
	
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetIndicator(tmpStatus, *data);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
		
	SetActive();
	}

void CTestGetIndicatorAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().

	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
	
	if(iStatus.Int() == KErrNone)
		{
		CTelephony::TIndicatorV1* data;
		if (iTestStep->iBufferMode)
			{
			data = reinterpret_cast<CTelephony::TIndicatorV1*> ( &iIndicatorBuff() );
			}
		else
			{
			data = reinterpret_cast<CTelephony::TIndicatorV1*> ( &iIndicatorPckg() );
			}
		
		iTestStep->TEST1(iExpIndCaps == data->iCapabilities, ETrue);
		iTestStep->TEST1(iExpInd == data->iIndicator, ETrue);
		iTestStep->PrintIndicatorValues(data->iIndicator, data->iCapabilities);
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for GetIndicators: %d"), iStatus.Int() );
		}

	CActiveScheduler::Stop();
	}

CTestGetBatteryInfo::CTestGetBatteryInfo()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetBatteryInfo"));
}

/**
*
@SYMTestCaseID Etel3rdParty 5.1
@SYMTestCaseDesc Test GetBatteryInfo 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.1.3
*
*/ 

TVerdict CTestGetBatteryInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("Test retrieval of battery information status"));

	//Set test number
	SetTestNumber(5);
	
	//Set expected result data
	TRequestStatus expStat=KErrNone;
	CTelephony::TBatteryInfoV1 BatteryExpInfo;
	BatteryExpInfo.iStatus=CTelephony::EPowerStatusUnknown;
	BatteryExpInfo.iChargeLevel=0;
	CTelephony::TBatteryInfoV1Pckg batteryExpInfoPckg(BatteryExpInfo);
	CTestGetBatteryInfoAct* GetBatteryInfoAct = CTestGetBatteryInfoAct::NewLC(this, iTelephony);
	
	#if defined(_DEBUG)
	//test OOM
		INFO_PRINTF1(_L("Test OOM for GetBatteryInfo"));
		expStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetBatteryInfoAct->GetBatteryInfo(batteryExpInfoPckg, expStat);	
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	//test GetBatteryInfo
	INFO_PRINTF1(_L("Test GetBatteryInfo"));
	expStat=KErrNone;
	GetBatteryInfoAct->GetBatteryInfo(batteryExpInfoPckg, expStat, ETrue);
	CActiveScheduler::Start();
		
	//wait for 3 secs then re-test.  State should have changed.	
	User::After(3000000);

	expStat=KErrNone;
	BatteryExpInfo.iStatus=CTelephony::EPoweredByBattery;
	BatteryExpInfo.iChargeLevel=2;
	GetBatteryInfoAct->GetBatteryInfo(batteryExpInfoPckg, expStat);	
	CActiveScheduler::Start();
	
	//wait for 3 secs then re-test.  State should have changed.
	User::After(3000000);

	expStat=KErrGeneral;
	BatteryExpInfo.iStatus=CTelephony::EPoweredByBattery;
	BatteryExpInfo.iChargeLevel=20;
	GetBatteryInfoAct->GetBatteryInfo(batteryExpInfoPckg, expStat);	
	CActiveScheduler::Start();
	
	//Test cancel
	User::After(3000000);
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetBatteryInfoCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	INFO_PRINTF1(_L("Test Cancel for GetBatteryInfo"));
	GetBatteryInfoAct->GetBatteryInfo(batteryExpInfoPckg, expStat);
	CTestCanceller* canceller = GetBatteryInfoAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetBatteryInfoAct->iStatus.Int() == KErrCancel || GetBatteryInfoAct->iStatus.Int() == KErrNone, ETrue);

	CleanupStack::PopAndDestroy(); // GetBatteryInfoAct
	return TestStepResult() ;
}

//=====================================================
//
//	Class to get battery info active object
//
//=====================================================

CTestGetBatteryInfoAct* CTestGetBatteryInfoAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetBatteryInfoAct* obj = new(ELeave) CTestGetBatteryInfoAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestGetBatteryInfoAct::CTestGetBatteryInfoAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	  iBatteryInfoBuff(iBatteryInfo),
	  iBatteryInfoPckg(iBatteryInfo)
	{  
	}

void CTestGetBatteryInfoAct::ConstructL()
	{
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetBatteryInfoAct::~CTestGetBatteryInfoAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetBatteryInfoAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetBatteryInfoCancel);
	}

void CTestGetBatteryInfoAct::GetBatteryInfo(TDes8& aBatteryExpInfoPckg, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	iBatteryExpInfoPckg=&aBatteryExpInfoPckg;
	iExpStat = aExpStat;
	
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iBatteryInfoBuff;
		}
	else
		{
		data = &iBatteryInfoPckg;
		}
	
	iTelephony->GetBatteryInfo(iStatus, *data);
	
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetBatteryInfo(tmpStatus, *data);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
		
	SetActive();
	}

void CTestGetBatteryInfoAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	CTelephony::TBatteryInfoV1Pckg& batteryExpInfoPckg = REINTERPRET_CAST(CTelephony::TBatteryInfoV1Pckg&, *iBatteryExpInfoPckg);
	
	//only supports V1 at present
	CTelephony::TBatteryInfoV1& batteryExpInfo = (batteryExpInfoPckg)();	

	iTestStep->TEST1(iStatus.Int()==iExpStat.Int(), ETrue);
	
	if(iStatus.Int() == KErrNone)
		{
		CTelephony::TBatteryInfoV1* data;
		if (iTestStep->iBufferMode)
			{
			data = reinterpret_cast<CTelephony::TBatteryInfoV1*> ( &iBatteryInfoBuff() );
			}
		else
			{
			data = reinterpret_cast<CTelephony::TBatteryInfoV1*> ( &iBatteryInfoPckg() );
			}
		    
		iTestStep->TEST1(data->iStatus==batteryExpInfo.iStatus, ETrue);
		iTestStep->TEST1(data->iChargeLevel==batteryExpInfo.iChargeLevel, ETrue);
		iTestStep->PrintBatteryInfo(*data);
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for GetBatteryInfo: %d"), iStatus.Int() );
		}

	iTestStep->INFO_PRINTF1(_L(""));
	CActiveScheduler::Stop();
	}

CTestGetSignalStrength::CTestGetSignalStrength()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetSignalStrength"));
}

/**
*
@SYMTestCaseID Etel3rdParty 6.1
@SYMTestCaseDesc Test GetSignalStrength 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.??
*
*/ 

TVerdict CTestGetSignalStrength::doTestStepL()
	{
	INFO_PRINTF1(_L("Test retrieval of Signal Strength information "));

	//Set test number
	SetTestNumber(6);
	//Set expected result values
	TRequestStatus expStat=KErrNone;
	CTelephony::TSignalStrengthV1 ExpSignalStrength;
	ExpSignalStrength.iBar=3;
	ExpSignalStrength.iSignalStrength=-51;
	CTelephony::TSignalStrengthV1Pckg expSignalStrengthPckg(ExpSignalStrength);
	CTestGetSignalStrengthAct* GetSignalStrengthAct = CTestGetSignalStrengthAct::NewLC(this, iTelephony);

	#if defined(_DEBUG)
	//test OOM
		INFO_PRINTF1(_L("Test OOM for GetSignalStrength"));
		expStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		GetSignalStrengthAct->GetSignalStrength(expSignalStrengthPckg, expStat);	
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif

	//Start GetSignalStrength AO
	INFO_PRINTF1(_L("Test GetSignalStrength"));
	expStat=KErrNone;
	GetSignalStrengthAct->GetSignalStrength(expSignalStrengthPckg, expStat, ETrue);	
	CActiveScheduler::Start();

	User::After(3000000);
	//wait for 3 secs and re-test
	expStat=KErrNotSupported;
	GetSignalStrengthAct->GetSignalStrength(expSignalStrengthPckg, expStat);	
	CActiveScheduler::Start();
	
	//Test cancel
	User::After(3000000);
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetSignalStrengthCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	INFO_PRINTF1(_L("Test Cancel for GetSignalStrength"));
	GetSignalStrengthAct->GetSignalStrength(expSignalStrengthPckg, expStat);
	CTestCanceller* canceller = GetSignalStrengthAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetSignalStrengthAct->iStatus.Int() == KErrCancel || GetSignalStrengthAct->iStatus.Int() == KErrNone, ETrue);

	CleanupStack::PopAndDestroy(); //GetSignalStrengthAct 
	return TestStepResult() ;
}

//=====================================================
//
//	Class to get signal strength active object
//
//=====================================================
CTestGetSignalStrengthAct* CTestGetSignalStrengthAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
//Factory constructor
	{
	CTestGetSignalStrengthAct* obj = new(ELeave) CTestGetSignalStrengthAct(aTestStep, aTelephony, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestGetSignalStrengthAct::CTestGetSignalStrengthAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iUseMacros(aUseMacros),
	iSignalStrengthBuff(iSignalStrength),
	iSignalStrengthPckg(iSignalStrength)
	{  
	}

void CTestGetSignalStrengthAct::ConstructL()
	{ 
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetSignalStrengthAct::~CTestGetSignalStrengthAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetSignalStrengthAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetSignalStrengthCancel);
	}

void CTestGetSignalStrengthAct::GetSignalStrength(TDes8& aExpSignalStrengthPckg, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	iExpStat = aExpStat;
	iExpSignalStrengthPckg=&aExpSignalStrengthPckg;
	
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iSignalStrengthBuff;
		}
	else
		{
		data = &iSignalStrengthPckg;
		}
	
	iTelephony->GetSignalStrength(iStatus, *data);
	
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetSignalStrength(tmpStatus, *data);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}		
		
	SetActive();
	}

void CTestGetSignalStrengthAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	CTelephony::TSignalStrengthV1Pckg& expSignalStrengthPckg = REINTERPRET_CAST(CTelephony::TSignalStrengthV1Pckg&, *iExpSignalStrengthPckg);
	
	//only supports V1 at present
	CTelephony::TSignalStrengthV1& expSignalStrength = (expSignalStrengthPckg)();	

	if(iUseMacros)
		{
		iTestStep->TEST1(iStatus.Int()==iExpStat.Int(), ETrue);
		}
	
	if(iStatus.Int() == KErrNone)
		{
		if(iUseMacros)
			{
			CTelephony::TSignalStrengthV1* data;
			if (iTestStep->iBufferMode)
				{
				data = reinterpret_cast<CTelephony::TSignalStrengthV1*> ( &iSignalStrengthBuff() );
				}
			else
				{
				data = reinterpret_cast<CTelephony::TSignalStrengthV1*> ( &iSignalStrengthPckg() );
				}
			    
			iTestStep->TEST1(data->iBar==expSignalStrength.iBar, ETrue);
			iTestStep->TEST1(data->iSignalStrength==expSignalStrength.iSignalStrength, ETrue);
			iTestStep->INFO_PRINTF2(_L("Signal strength (in display bars) : %d"), expSignalStrength.iBar);
			iTestStep->INFO_PRINTF2(_L("Signal strength (in dBm) : %d"), expSignalStrength.iSignalStrength);
			}
		}
	else
		{
		if(iUseMacros)
			{
			iTestStep->INFO_PRINTF2(_L("Error for GetSignalStrength: %d"), iStatus.Int() );
			}
		}

	CActiveScheduler::Stop();
	if(iUseMacros)
		{
		iTestStep->INFO_PRINTF1(_L(""));
		}
	}

CTestGetLockInfo::CTestGetLockInfo()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetLockInfo"));
}

/**
*
@SYMTestCaseID Etel3rdParty 7.1
@SYMTestCaseDesc Test GetLockInfo 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.1.8
*
*/ 

TVerdict CTestGetLockInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("Test retrieval of current status of PIN1/PIN2 "));
	//Set test number
	SetTestNumber(7);
	
	//Set expected result values
	TRequestStatus expStat=KErrNone;
	CTelephony::TIccLockInfoV1 ExpLockInfo;
	ExpLockInfo.iSetting = CTelephony::ELockSetDisabled;
	ExpLockInfo.iStatus = CTelephony::EStatusUnlocked;
	CTelephony::TIccLockInfoV1Pckg expLockInfoPckg(ExpLockInfo);
	CTestGetLockInfoAct* GetLockInfoAct = CTestGetLockInfoAct::NewLC(this, iTelephony);

	CTelephony::TIccLock Lock;
	#if defined(_DEBUG)
	//test OOM
		INFO_PRINTF1(_L("Test OOM for GetLockInfo"));
		expStat=KErrNoMemory;
		Lock=CTelephony::ELockPin1;
		__UHEAP_FAILNEXT(1);
		GetLockInfoAct->GetLockInfo(Lock, expLockInfoPckg, expStat);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	//Start GetLockinfo AO for pin1
	expStat=KErrNone;
	INFO_PRINTF1(_L("Test GetLockInfo"));
	Lock=CTelephony::ELockPin1;
	INFO_PRINTF1(_L("current status of PIN1 "));
	GetLockInfoAct->GetLockInfo(Lock, expLockInfoPckg, expStat, ETrue);
	CActiveScheduler::Start();

	//Start GetLockinfo AO for pin2
	Lock=CTelephony::ELockPin2;
	INFO_PRINTF1(_L("current status of PIN2 "));
	ExpLockInfo.iSetting = CTelephony::ELockSetEnabled;
	ExpLockInfo.iStatus = CTelephony::EStatusLocked;
	GetLockInfoAct->GetLockInfo(Lock, expLockInfoPckg, expStat);
	CActiveScheduler::Start();
	
	//Test cancel
	INFO_PRINTF1(_L("Test Cancel for GetLockInfo"));
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetLockInfoCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetLockInfoAct->GetLockInfo(Lock, expLockInfoPckg, expStat);
	CTestCanceller* canceller = GetLockInfoAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();
	TEST1(GetLockInfoAct->iStatus.Int() == KErrCancel || GetLockInfoAct->iStatus.Int() == KErrNone, ETrue);

	CleanupStack::PopAndDestroy(); //GetLockInfoAct 
	return TestStepResult() ;
}

//=====================================================
//
//	Class to get lock info active object
//
//=====================================================
CTestGetLockInfoAct* CTestGetLockInfoAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetLockInfoAct* obj = new(ELeave) CTestGetLockInfoAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestGetLockInfoAct::CTestGetLockInfoAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iLockInfoBuff(iLockInfo),
	iLockInfoPckg(iLockInfo)
	{  
	}

void CTestGetLockInfoAct::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetLockInfoAct::~CTestGetLockInfoAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetLockInfoAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetLockInfoCancel);
	}

void CTestGetLockInfoAct::GetLockInfo(CTelephony::TIccLock aLock, TDes8& aExpLockInfoPckg, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	iExpStat = aExpStat;
	iExpLockInfoPckg=&aExpLockInfoPckg;
	iLock = aLock;

	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iLockInfoBuff;
		}
	else
		{
		data = &iLockInfoPckg;
		}

	iTelephony->GetLockInfo(iStatus ,aLock ,*data);
	
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetLockInfo(tmpStatus ,aLock ,*data);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
		
	SetActive();
	}

void CTestGetLockInfoAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	CTelephony::TIccLockInfoV1Pckg& expLockInfoPckg = REINTERPRET_CAST(CTelephony::TIccLockInfoV1Pckg&, *iExpLockInfoPckg);
	
	//only supports V1 at present
	CTelephony::TIccLockInfoV1& expLockInfo = (expLockInfoPckg)();	

	iTestStep->TEST1(iStatus.Int()==iExpStat.Int(),1);
	
	if(iStatus.Int() == KErrNone)
		{
		CTelephony::TIccLockInfoV1* data;
		if (iTestStep->iBufferMode)
			{
			data = reinterpret_cast<CTelephony::TIccLockInfoV1*> ( &iLockInfoBuff() );
			}
		else
			{
			data = reinterpret_cast<CTelephony::TIccLockInfoV1*> ( &iLockInfoPckg() );
			}
		    
		iTestStep->TEST1(data->iSetting==expLockInfo.iSetting,1);
		iTestStep->TEST1(data->iStatus==expLockInfo.iStatus,1);
		iTestStep->PrintLockInfo(iLock, data->iSetting, data->iStatus);
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for GetLockInfo: %d"), iStatus.Int() );
		}

	CActiveScheduler::Stop();
	iTestStep->INFO_PRINTF1(_L(""));
	}

CTestSendDTMFTones::CTestSendDTMFTones()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSendDTMFTones"));
}

/**
*
@SYMTestCaseID Etel3rdParty 8.1
@SYMTestCaseDesc Test SendDTMFTones 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.1.9
*
*/ 


TVerdict CTestSendDTMFTones::doTestStepL()
	{
	SetTestNumber(8);

	INFO_PRINTF1(_L("Test sending of DTMF tones "));
	
	//set test data to transmit
	_LIT(KDtmfString0,"45p6789A");
	
	//make a call to send the DTMF through
	
	_LIT(KGoodNumber,"1789");	
	//Set the call parameters for call 
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams;
	testCallParams.iLine = CTelephony::EVoiceLine;
	testCallParams.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams.iExpStat = KErrNone;
		
	//make the call
	CTestDialNewCallAct* testDialNewCallAct = CTestDialNewCallAct::NewLC(this, iTelephony);	
	testCallParams.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct->TestDialNewCall(testCallParams);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(testDialNewCallAct);	
	
	//wait for call status to be connected
	CTelephony::TCallStatusV1 CallStatusV1;
	CTelephony::TCallStatusV1Pckg CallStatusPckg(CallStatusV1);
	iTelephony->GetCallStatus(testCallParams.iCallId, CallStatusPckg);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
 
	TRequestStatus expStat=KErrNone;	
	CTestSendDTMFTonesAct*SendDTMFAct = CTestSendDTMFTonesAct::NewLC(this, iTelephony);

	#if defined(_DEBUG)
	//test OOM
		INFO_PRINTF1(_L("Test OOM for SendDTMFTones")); 
		expStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		SendDTMFAct->SendDTMFTone(KDtmfString0, expStat);
		CActiveScheduler::Start();	
		__UHEAP_RESET;
	#endif
		
	//set expected result values for DTMF	
	expStat=KErrNone;
	SendDTMFAct->SendDTMFTone(KDtmfString0, expStat, ETrue);
	CActiveScheduler::Start();

	//Test cancel
	INFO_PRINTF1(_L("Test Cancel for SendDTMFTones"));
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::ESendDTMFTonesCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	SendDTMFAct->SendDTMFTone(KDtmfString0, expStat);
	CTestCanceller* canceller = SendDTMFAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(SendDTMFAct->iStatus.Int() == KErrCancel, ETrue);

	CTestHangupAct* testHangupAct = CTestHangupAct::NewLC(this, iTelephony);	
	//Hangup call
	testHangupAct->TestHangup(testCallParams);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(testHangupAct);

	
	CleanupStack::PopAndDestroy(SendDTMFAct); //SendDTMFAct 
	return TestStepResult() ;
}

CTestSendDTMFTonesAct* CTestSendDTMFTonesAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestSendDTMFTonesAct* obj = new(ELeave) CTestSendDTMFTonesAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestSendDTMFTonesAct::CTestSendDTMFTonesAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony)
	{  
	}

void CTestSendDTMFTonesAct::ConstructL()
	{ 
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestSendDTMFTonesAct::~CTestSendDTMFTonesAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestSendDTMFTonesAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::ESendDTMFTonesCancel);
	}

void CTestSendDTMFTonesAct::SendDTMFTone(const TDesC& aTones, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	iExpStat = aExpStat;
	
	iTelephony->SendDTMFTones(iStatus, aTones);
	
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->SendDTMFTones(tmpStatus, aTones);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
		
	SetActive();
	}

void CTestSendDTMFTonesAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().

	iTestStep->TEST1(iStatus.Int()==iExpStat.Int(), 1);
	
	CActiveScheduler::Stop();
	iTestStep->INFO_PRINTF1(_L(""));
	}

CTestFlightModeWithPublishSubscribe::CTestFlightModeWithPublishSubscribe()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestFlightModeWithPublishSubscribe"));
}

/**
*
@SYMTestCaseID Etel3rdParty 3.1
@SYMTestCaseDesc Test GetFlightMode 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.4.3
*
*/ 


TVerdict CTestFlightModeWithPublishSubscribe::doTestStepL()
	{
	INFO_PRINTF1(_L("Test retrieval of flight mode with publish and subscribe"));

	RProperty phonePowerProperty;
	User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
	CleanupClosePushL(phonePowerProperty);	
			
	TRequestStatus status;
	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);	

	//Set up the data
	CTelephony::TFlightModeV1 FlightMode;
	CTelephony::TFlightModeV1Pckg FlightModePckg(FlightMode);
	TPckgBuf<CTelephony::TFlightModeV1> FlightModeBuff(FlightMode);
	TDes8* FlightModeData;
	if (iBufferMode)
		{
		FlightModeData = &FlightModeBuff;
		}
	else
		{
		FlightModeData = &FlightModePckg;
		}
	CTelephony::TFlightModeV1& FlightModeResult = 
		reinterpret_cast<CTelephony::TFlightModeV1&> ( const_cast<TUint8&> ( *FlightModeData->Ptr() ) );
	
	//Get current flight mode
	TRequestStatus reqStat=KErrNone;
	iTelephony->GetFlightMode(reqStat, *FlightModeData);
	User::WaitForRequest(reqStat);

	//check results
	TEST1(reqStat.Int() == KErrNone, ETrue);
	TEST1(FlightModeResult.iFlightModeStatus == CTelephony::EFlightModeOff, ETrue);
	if (FlightModeResult.iFlightModeStatus == CTelephony::EFlightModeOff)
		INFO_PRINTF1(_L("The mobile radio interface and bluetooth is currently on"));

	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);	
	
	//Get current flight mode
	iTelephony->GetFlightMode(reqStat, *FlightModeData);
	User::WaitForRequest(reqStat);

	//check results
	TEST1(reqStat.Int() == KErrNone, ETrue);
	TEST1(FlightModeResult.iFlightModeStatus == CTelephony::EFlightModeOn, ETrue);
	if (FlightModeResult.iFlightModeStatus == CTelephony::EFlightModeOn)
		INFO_PRINTF1(_L("The mobile radio interface and bluetooth is currently off"));		
	CleanupStack::PopAndDestroy(&phonePowerProperty);
	return TestStepResult() ;
}

CTestPhoneFuncFlightModeOnWithPublishSubscribe::CTestPhoneFuncFlightModeOnWithPublishSubscribe()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestPhoneFuncFlightModeOnWithPublishSubscribe"));
}

/**
@SYMTestCaseID Etel3rdParty 8.2, TestPhoneFuncFlightModeOn
@SYMTestCaseDesc Test GetSignalStrength and SendDTMFTone 3rd party APIs
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.1.?/9
*/

enum TVerdict CTestPhoneFuncFlightModeOnWithPublishSubscribe::doTestStepL()
{
	INFO_PRINTF1(_L("START TEST 8.2"));	
	 
	INFO_PRINTF1(_L(" TEST: Signal Strength information not retrievable when flight mode on with publish and subscribe "));

	//Set test number
	SetTestNumber(34);
	
	// Set expected status to KErrAccessDenied as this should be returned in
	// the case where flight mode is on.
	TRequestStatus expStat=KErrAccessDenied;
	CTelephony::TSignalStrengthV1 ExpSignalStrength;
	ExpSignalStrength.iBar=3;
	ExpSignalStrength.iSignalStrength=-51;
	CTelephony::TSignalStrengthV1Pckg expSignalStrengthPckg(ExpSignalStrength);
	CTestGetSignalStrengthAct* GetSignalStrengthAct = CTestGetSignalStrengthAct::NewLC(this, iTelephony);

	// Set up phonePowerProperty variable so that flight mode can be manipulated.
	RProperty phonePowerProperty;
	User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
	CleanupClosePushL(phonePowerProperty);	

	//Switch flight mode on (ie radio mode off)
	TRequestStatus status;
	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);	

	// Make a call to the 3rd Party API.
	GetSignalStrengthAct->GetSignalStrength(expSignalStrengthPckg, expStat);	
	CActiveScheduler::Start();

	
	INFO_PRINTF1(_L("Test sending of DTMF tones disallowed when flight mode on "));
	
	//set test data to transmit
	_LIT(KDtmfString0,"45p6789A");
	
	// Switch flight mode off so that a call can be set up. 
	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);	
 
 	//Get current flight mode
	TRequestStatus reqStat=KErrNone;
	CTelephony::TFlightModeV1 FlightMode;
	CTelephony::TFlightModeV1Pckg FlightModePckg(FlightMode);	
	iTelephony->GetFlightMode(reqStat, FlightModePckg);
	User::WaitForRequest(reqStat);

	//check results
	TEST1(reqStat.Int() == KErrNone, ETrue);
	TEST1(FlightMode.iFlightModeStatus == CTelephony::EFlightModeOff, ETrue);
	if (FlightMode.iFlightModeStatus == CTelephony::EFlightModeOff)
		INFO_PRINTF1(_L("The mobile radio interface and bluetooth is currently on"));
	
 	// Switch flight mode on (ie radio mode off) to test that 
 	// DTMF Tone sending fails is disallowed in flight mode. 
	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);	

 
	//set expected result values for DTMF	
	CTestSendDTMFTonesAct*SendDTMFAct = CTestSendDTMFTonesAct::NewLC(this, iTelephony);
	expStat=KErrAccessDenied;
	SendDTMFAct->SendDTMFTone(KDtmfString0, expStat);
	CActiveScheduler::Start();
 
 	// Switch flight mode off (ie radio mode on)  
 	// so that the call can be hung up.
	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);	

	
	INFO_PRINTF1(_L("END TEST 8.2"));
	
	 
	//CleanupStack::PopAndDestroy(testHangupAct);
	CleanupStack::PopAndDestroy(SendDTMFAct); //SendDTMFAct 
	//CleanupStack::PopAndDestroy(testDialNewCallAct);
	CleanupStack::PopAndDestroy(&phonePowerProperty);	 
	CleanupStack::PopAndDestroy(GetSignalStrengthAct);
	return TestStepResult() ;	  
	 
}

