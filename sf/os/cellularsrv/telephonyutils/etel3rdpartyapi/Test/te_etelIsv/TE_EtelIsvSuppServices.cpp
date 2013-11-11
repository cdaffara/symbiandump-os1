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

#include "TE_EtelIsvbase.h"
#include "TE_EtelIsvSuppServices.h"

 

/**
@SYMTestCaseID Etel3rdParty 22.1, TestGetSupplementaryServiceStatus
@SYMTestCaseDesc Test GetCallForwardingStatus, GetCallBarringStatus and
* GetCallWaitingStatus  3rd party APIs
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.4.11
*/
 //
// RETRIEVE SUPPLEMENTARY SERVICE INFORMATION //
//
CTestGetSupplementaryServiceStatus::CTestGetSupplementaryServiceStatus()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetSupplementaryService"));
}

enum TVerdict CTestGetSupplementaryServiceStatus::doTestStepL()
{
	INFO_PRINTF1(_L("Test retrieval of supplementary service information"));
	 
	// Set the config.txt test number mapping
	SetTestNumber(13);

	// Set the expected request status
	TRequestStatus ExpStat=KErrNone;
	CTelephony::TCallBarringSupplServicesV1 ExpectedCallBarringStatus; 
	CTelephony::TCallForwardingSupplServicesV1 ExpectedCallForwardingStatus;
	CTelephony::TCallWaitingSupplServicesV1 ExpectedCallWaitingStatus;  

	// Create an active object and push it onto the clean up stack
	CTestGetSupplementaryServiceStatusAct* GetSupplementaryServiceStatusAct = CTestGetSupplementaryServiceStatusAct::NewLC(this, iTelephony);
	
	//
	// Test call barring //
	//
	INFO_PRINTF1(_L("Test retrieval of call barring information"));
	
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	
	//go through each condition, status and service group. 
	
	//Test retrieving condition which is out of range
	ExpStat=KErrNotSupported;
	ExpectedCallBarringStatus.iCallBarring=CTelephony::EStatusActive;
	GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,(CTelephony::TCallBarringCondition)7, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	//Test OOM
	#if defined(_DEBUG)
		ExpStat=KErrNoMemory;
		__UHEAP_FAILNEXT(1);
		ExpectedCallBarringStatus.iCallBarring=CTelephony::EStatusActive;
		GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarAllIncoming, CTelephony::EVoiceService, ExpStat);
		CActiveScheduler::Start();
	__UHEAP_RESET;
	#endif
   
    ExpStat=KErrNone;
	ExpectedCallBarringStatus.iCallBarring=CTelephony::EStatusActive;
	GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarAllIncoming, CTelephony::EVoiceService, ExpStat, ETrue);
	CActiveScheduler::Start();
	
	// INC085718: SimTSY is configured to have EBarIncomingRoaming for EVoiceService. A query of the status for an other service must return ENotActive
	ExpectedCallBarringStatus.iCallBarring=CTelephony::ENotActive;
	GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarIncomingRoaming, CTelephony::EDataService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallBarringStatus.iCallBarring=CTelephony::ENotProvisioned;
	GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarAllOutgoing, CTelephony::EFaxService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallBarringStatus.iCallBarring=CTelephony::ENotAvailable;
	GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarOutgoingInternational, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallBarringStatus.iCallBarring=CTelephony::EUnknown;
	GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarOutgoingInternationalExHC, CTelephony::EDataService, ExpStat);
	CActiveScheduler::Start();
	
	//Test cancel
	INFO_PRINTF1(_L("Test cancel of call barring information"));
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetCallBarringStatusCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarAllIncoming, CTelephony::EDataService, ExpStat);
	CTestCanceller* canceller = GetSupplementaryServiceStatusAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetSupplementaryServiceStatusAct->iStatus.Int() == KErrCancel, ETrue);
	
	//
	// Test call forwarding //
	//
	INFO_PRINTF1(_L("Test retrieval of call forwarding information"));
	
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	 
	//Test retrieving condition which is out of range
	ExpStat=KErrNotSupported;
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::EStatusActive;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, (CTelephony::TCallForwardingCondition)7, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	ExpStat=KErrNone;
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::EStatusActive;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingUnconditional, CTelephony::EVoiceService, ExpStat, ETrue);
	CActiveScheduler::Start();
	
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::ENotActive;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingBusy, CTelephony::EDataService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::EUnknown;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingNoReply, CTelephony::EDataService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::ENotProvisioned;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingNotReachable, CTelephony::EDataService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::ENotAvailable;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingBusy, CTelephony::EFaxService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::EUnknown;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingNoReply, CTelephony::EFaxService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::EUnknown;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingNotReachable, CTelephony::EFaxService, ExpStat);
	CActiveScheduler::Start();
	
	//Test cancel
	INFO_PRINTF1(_L("Test cancel of call forwarding information"));
	cancelerr = iTelephony->CancelAsync(CTelephony::EGetCallForwardingStatusCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingNoReply, CTelephony::EVoiceService, ExpStat);
	canceller = GetSupplementaryServiceStatusAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetSupplementaryServiceStatusAct->iStatus.Int() == KErrCancel, ETrue);
	
	//
	// Test call waiting //
	//
	INFO_PRINTF1(_L("Test retrieval of call waiting information"));
	
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	 	
	ExpStat=KErrNone;
	ExpectedCallWaitingStatus.iCallWaiting=CTelephony::EStatusActive;
	GetSupplementaryServiceStatusAct->TestGetCWSupplementaryServiceStatus(ExpectedCallWaitingStatus, CTelephony::EVoiceService, ExpStat, ETrue);
	CActiveScheduler::Start();
	
	ExpectedCallWaitingStatus.iCallWaiting=CTelephony::ENotActive;
	GetSupplementaryServiceStatusAct->TestGetCWSupplementaryServiceStatus(ExpectedCallWaitingStatus, CTelephony::EDataService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallWaitingStatus.iCallWaiting=CTelephony::ENotProvisioned;
	GetSupplementaryServiceStatusAct->TestGetCWSupplementaryServiceStatus(ExpectedCallWaitingStatus, CTelephony::EFaxService, ExpStat);
	CActiveScheduler::Start();
	
	//Test cancel
	INFO_PRINTF1(_L("Test cancel of call waiting information"));
	cancelerr = iTelephony->CancelAsync(CTelephony::EGetCallWaitingStatusCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetSupplementaryServiceStatusAct->TestGetCWSupplementaryServiceStatus(ExpectedCallWaitingStatus,CTelephony::EVoiceService, ExpStat);
	canceller = GetSupplementaryServiceStatusAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetSupplementaryServiceStatusAct->iStatus.Int() == KErrCancel, ETrue);
    // Clean up
	CleanupStack::PopAndDestroy(); //GetSupplementaryServiceStatusAct
	
	//Part 2 - Open a new test which tests EAllServices responses 
	
	// Set the config.txt test number mapping
	SetTestNumber(33);	
	
	GetSupplementaryServiceStatusAct = CTestGetSupplementaryServiceStatusAct::NewLC(this, iTelephony);
	
	//Call barring
	ExpStat=KErrNone;
	ExpectedCallBarringStatus.iCallBarring=CTelephony::EStatusActive;
	GetSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarAllIncoming, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	//Call forwarding
	ExpStat=KErrNone;
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::EUnknown;
	GetSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingUnconditional, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	ExpectedCallWaitingStatus.iCallWaiting=CTelephony::EStatusActive;
	GetSupplementaryServiceStatusAct->TestGetCWSupplementaryServiceStatus(ExpectedCallWaitingStatus, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	// Clean up
	CleanupStack::PopAndDestroy(); //GetSupplementaryServiceStatusAct
	return TestStepResult();
}

CTestGetSupplementaryServiceStatusAct* CTestGetSupplementaryServiceStatusAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetSupplementaryServiceStatusAct* obj = new(ELeave) CTestGetSupplementaryServiceStatusAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestGetSupplementaryServiceStatusAct::CTestGetSupplementaryServiceStatusAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	  iCallBarringStatusPckg(iCallBarringStatus),
	  iCallBarringStatusBuff(iCallBarringStatus),
	  iCallWaitingStatusPckg(iCallWaitingStatus),
	  iCallWaitingStatusBuff(iCallWaitingStatus),
	  iCallForwardingStatusPckg(iCallForwardingStatus),
	  iCallForwardingStatusBuff(iCallForwardingStatus)
	{  
	}

void CTestGetSupplementaryServiceStatusAct::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetSupplementaryServiceStatusAct::~CTestGetSupplementaryServiceStatusAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetSupplementaryServiceStatusAct::DoCancel()
	{
	if(iSupplServiceType == ETestSupplServiceCB)
		{
		iTelephony->CancelAsync(CTelephony::EGetCallBarringStatusCancel);
		}
	else if(iSupplServiceType == ETestSupplServiceCF)
		{
		iTelephony->CancelAsync(CTelephony::EGetCallForwardingStatusCancel);
		}
	else if(iSupplServiceType == ETestSupplServiceCW)
		{
		iTelephony->CancelAsync(CTelephony::EGetCallWaitingStatusCancel);
		}
	}

void CTestGetSupplementaryServiceStatusAct::TestGetCFSupplementaryServiceStatus(CTelephony::TCallForwardingSupplServicesV1& aExpSupplementaryServiceStatus, CTelephony::TCallForwardingCondition aSSCondition, CTelephony::TServiceGroup aServiceGroup, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{	
	// Set the service type requested
	iSupplServiceType=ETestSupplServiceCF;	
	// Set the expected return value
	iExpStat=aExpStat;	
	iExpectedCallForwardingStatus.iCallForwardingCondition = aSSCondition;
	iServiceGroup = aServiceGroup;
 	// Set the expected return values for call forwarding
	iExpectedCallForwardingStatus.iCallForwarding= aExpSupplementaryServiceStatus.iCallForwarding;		
	// Make a call to the 3rd party API
	
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iCallForwardingStatusBuff;
		}
	else
		{
		data = &iCallForwardingStatusPckg;
		}
	    
	iTelephony->GetCallForwardingStatus(iStatus, aSSCondition, *data, aServiceGroup );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCallForwardingStatus(tmpStatus, aSSCondition, *data, aServiceGroup );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();			
	}
	
void CTestGetSupplementaryServiceStatusAct::TestGetCBSupplementaryServiceStatus(CTelephony::TCallBarringSupplServicesV1& aExpSupplementaryServiceStatus, CTelephony::TCallBarringCondition aSSCondition, CTelephony::TServiceGroup aServiceGroup, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{	
	// Set the service type requested
	iSupplServiceType=ETestSupplServiceCB;	
	// Set the expected return value
	iExpStat=aExpStat;	
	iExpectedCallBarringStatus.iCallBarringCondition = aSSCondition;
	iServiceGroup = aServiceGroup;
 	// Set the expected return values for call forwarding
	iExpectedCallBarringStatus.iCallBarring= aExpSupplementaryServiceStatus.iCallBarring;		
	// Make a call to the 3rd party API
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iCallBarringStatusBuff;
		}
	else
		{
		data = &iCallBarringStatusPckg;
		}
	    
	iTelephony->GetCallBarringStatus(iStatus, aSSCondition, *data, aServiceGroup );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCallBarringStatus(tmpStatus, aSSCondition, *data, aServiceGroup );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();			
	}
	
void CTestGetSupplementaryServiceStatusAct::TestGetCWSupplementaryServiceStatus(CTelephony::TCallWaitingSupplServicesV1& aExpSupplementaryServiceStatus, CTelephony::TServiceGroup aServiceGroup, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{	
	// Set the service type requested
	iSupplServiceType=ETestSupplServiceCW;	
	// Set the expected return value
	iExpStat=aExpStat;	
	iServiceGroup = aServiceGroup;
 	// Set the expected return values for call forwarding
	iExpectedCallWaitingStatus.iCallWaiting= aExpSupplementaryServiceStatus.iCallWaiting;		
	// Make a call to the 3rd party API
	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iCallWaitingStatusBuff;
		}
	else
		{
		data = &iCallWaitingStatusPckg;
		}
	    
	iTelephony->GetCallWaitingStatus(iStatus, *data, aServiceGroup );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetCallWaitingStatus(tmpStatus, *data, aServiceGroup );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();			
	}

void CTestGetSupplementaryServiceStatusAct::RunL()
	{ 
	iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
	
	if(iStatus.Int() == KErrNone)
	{
		iTestStep->INFO_PRINTF2(_L("Service Group = : %d"),iServiceGroup );		
		if (iSupplServiceType == ETestSupplServiceCB)
		{		
			CTelephony::TCallBarringSupplServicesV1* cbData;
			if (iTestStep->iBufferMode)
				{
				cbData = reinterpret_cast<CTelephony::TCallBarringSupplServicesV1*> ( &iCallBarringStatusBuff() );
				}
			else
				{
				cbData = reinterpret_cast<CTelephony::TCallBarringSupplServicesV1*> ( &iCallBarringStatusPckg() );
				}
		        
			// Print off the call barring values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Call barring condition expected: %d"),iExpectedCallBarringStatus.iCallBarringCondition );
			iTestStep->INFO_PRINTF2(_L("Call barring condition: %d"),cbData->iCallBarringCondition );
			iTestStep->INFO_PRINTF2(_L("Call Barring status  expected: %d"), iExpectedCallBarringStatus.iCallBarring );
			iTestStep->INFO_PRINTF2(_L("Call barring status retrieved: %d"), cbData->iCallBarring );
				
			// Check that the retrieved values match those expected.
			iTestStep->TEST1( cbData->iCallBarringCondition ==  iExpectedCallBarringStatus.iCallBarringCondition, ETrue );
			iTestStep->TEST1( cbData->iCallBarring ==  iExpectedCallBarringStatus.iCallBarring, ETrue );
	 	}
	 	else 
	 	if (iSupplServiceType == ETestSupplServiceCF)
		{
			CTelephony::TCallForwardingSupplServicesV1* cfData;
			if (iTestStep->iBufferMode)
				{
				cfData = reinterpret_cast<CTelephony::TCallForwardingSupplServicesV1*> ( &iCallForwardingStatusBuff() );
				}
			else
				{
				cfData = reinterpret_cast<CTelephony::TCallForwardingSupplServicesV1*> ( &iCallForwardingStatusPckg() );
				}
		        
			// Print off the call forwarding values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Call forwarding expected condition: %d"), iExpectedCallForwardingStatus.iCallForwardingCondition );
			iTestStep->INFO_PRINTF2(_L("Call forwarding condition: %d"), cfData->iCallForwardingCondition );
			iTestStep->INFO_PRINTF2(_L("Call Forwarding status  expected: %d"), iExpectedCallForwardingStatus.iCallForwarding );
			iTestStep->INFO_PRINTF2(_L("Call forwarding status retrieved: %d"), cfData->iCallForwarding );
					
			// Check that the retrieved values match those expected.
			iTestStep->TEST1( cfData->iCallForwardingCondition ==  iExpectedCallForwardingStatus.iCallForwardingCondition, ETrue );
			iTestStep->TEST1( cfData->iCallForwarding ==  iExpectedCallForwardingStatus.iCallForwarding, ETrue );
		}
		else 
	 	if (iSupplServiceType == ETestSupplServiceCW)
		{
			CTelephony::TCallWaitingSupplServicesV1* cwData;
			if (iTestStep->iBufferMode)
				{
				cwData = reinterpret_cast<CTelephony::TCallWaitingSupplServicesV1*> ( &iCallWaitingStatusBuff() );
				}
			else
				{
				cwData = reinterpret_cast<CTelephony::TCallWaitingSupplServicesV1*> ( &iCallWaitingStatusPckg() );
				}
			// Print off the call waiting values retrieved and those expected
			iTestStep->INFO_PRINTF2(_L("Call Waiting status  expected: %d"), iExpectedCallWaitingStatus.iCallWaiting );
			iTestStep->INFO_PRINTF2(_L("Call Waiting status retrieved: %d"), cwData->iCallWaiting );
			 
			// Check that the retrieved values match those expected.
			iTestStep->TEST1( cwData->iCallWaiting ==  iExpectedCallWaitingStatus.iCallWaiting , ETrue);
		}
	}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
		}

	CActiveScheduler::Stop();
	}

/**
@SYMTestCaseID Etel3rdParty 23.1, TestGetIdentityServiceStatus
@SYMTestCaseDesc Test GetIdentityServiceStatus  3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.4.12
*/
//
// RETRIEVE IDENTITY SERVICE INFORMATION //
//
CTestGetIdentityServiceStatus::CTestGetIdentityServiceStatus()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetIdentityService"));
}

enum TVerdict CTestGetIdentityServiceStatus::doTestStepL()
{
	INFO_PRINTF1(_L("Test retrieval of identity service information"));

	 
	// Set the config.txt test number mapping
	SetTestNumber(14);
	
	// Set the expected request status
	TRequestStatus ExpStat=KErrNone;

	// Create an active object and push it onto the clean up stack
	CTestGetIdentityServiceStatusAct* GetIdentityServiceStatusAct = CTestGetIdentityServiceStatusAct::NewLC(this, iTelephony);

	CTelephony::TIdentityServiceV1 ExpectedIdentityStatus;
	CTelephony::TIdentityService IdentityServiceTypeRequired; 
	
	//Test OOM
	#if defined(_DEBUG)
		INFO_PRINTF1(_L("Test GetIdentityServiceStatus OOM"));
		ExpStat=KErrNoMemory;
		IdentityServiceTypeRequired=CTelephony::EIdServiceCallerPresentation;
		ExpectedIdentityStatus.iIdentityStatus=CTelephony::EIdServiceActiveDefaultRestricted;
		__UHEAP_FAILNEXT(1);
		GetIdentityServiceStatusAct->TestGetIdentityServiceStatus(ExpectedIdentityStatus,IdentityServiceTypeRequired, ExpStat);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	ExpStat=KErrNone;
	// Test EIdServiceCallerPresentation
	INFO_PRINTF1(_L("Test retrieval of EIdServiceCallerPresentation status"));
	// Set the type of supplementary service info required
	IdentityServiceTypeRequired=CTelephony::EIdServiceCallerPresentation;
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.	
	ExpectedIdentityStatus.iIdentityStatus=CTelephony::EIdServiceActiveDefaultRestricted;
	// Attempt to retrieve the service information. Pass the expected values. 
	GetIdentityServiceStatusAct->TestGetIdentityServiceStatus(ExpectedIdentityStatus,IdentityServiceTypeRequired, ExpStat, ETrue);
	// Start the active scheduler
	CActiveScheduler::Start();
	
	// Test EIdServiceCallerRestriction
	INFO_PRINTF1(_L("Test retrieval of EIdServiceCallerRestriction status"));
	// Set the type of supplementary service info required
	IdentityServiceTypeRequired=CTelephony::EIdServiceCallerRestriction;
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.	
	ExpectedIdentityStatus.iIdentityStatus=CTelephony::EIdServiceNotProvisioned;
	// Attempt to retrieve the service information. Pass the expected values. 
	GetIdentityServiceStatusAct->TestGetIdentityServiceStatus(ExpectedIdentityStatus,IdentityServiceTypeRequired, ExpStat);
	// Start the active scheduler
	CActiveScheduler::Start();
	
	//Test cancel
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EGetIdentityServiceStatusCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	GetIdentityServiceStatusAct->TestGetIdentityServiceStatus(ExpectedIdentityStatus,IdentityServiceTypeRequired, ExpStat);
	CTestCanceller* canceller = GetIdentityServiceStatusAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(GetIdentityServiceStatusAct->iStatus.Int() == KErrCancel || GetIdentityServiceStatusAct->iStatus.Int() == KErrNone, ETrue);	 
	
    // Clean up
	CleanupStack::PopAndDestroy(); //GetIdentityServiceStatusAct
	return TestStepResult();
}

CTestGetIdentityServiceStatusAct* CTestGetIdentityServiceStatusAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestGetIdentityServiceStatusAct* obj = new(ELeave) CTestGetIdentityServiceStatusAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestGetIdentityServiceStatusAct::CTestGetIdentityServiceStatusAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony),
	iIdentityServiceStatusPckg(iIdentityServiceStatus),
	iIdentityServiceStatusBuff(iIdentityServiceStatus)
	{  
	}

void CTestGetIdentityServiceStatusAct::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestGetIdentityServiceStatusAct::~CTestGetIdentityServiceStatusAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestGetIdentityServiceStatusAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetIdentityServiceStatusCancel);
	}

void CTestGetIdentityServiceStatusAct::TestGetIdentityServiceStatus(CTelephony::TIdentityServiceV1& aIdentityServiceStatus, CTelephony::TIdentityService aIdentityServiceType, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	
	// Set the service type requested
	iIdentityServiceType=aIdentityServiceType;
	
	// Set the expected return value
	iExpStat=aExpStat; 
	CTelephony::TIdentityServiceV1* IdentityServiceStatus = &aIdentityServiceStatus;
	iExpectedIdentityServiceStatus.iIdentityStatus= IdentityServiceStatus->iIdentityStatus;

	TDes8* data;
	if (iTestStep->iBufferMode)
		{
		data = &iIdentityServiceStatusBuff;
		}
	else
		{
		data = &iIdentityServiceStatusPckg;
		}
		
	// Make a call to the 3rd party API
	iTelephony->GetIdentityServiceStatus(iStatus, aIdentityServiceType, *data);
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->GetIdentityServiceStatus(tmpStatus, aIdentityServiceType, *data);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive();
	}

void CTestGetIdentityServiceStatusAct::RunL()
	{ 
	iTestStep->INFO_PRINTF2(_L("Request status set =  %d"), iStatus.Int() ); 
	
	if(iStatus.Int() == KErrNone)
	{
		CTelephony::TIdentityServiceV1* data;
		if (iTestStep->iBufferMode)	
			{
			data = reinterpret_cast<CTelephony::TIdentityServiceV1*> ( const_cast<TUint8*> (iIdentityServiceStatusBuff.Ptr()) );
			}
		else	
			{
			data = reinterpret_cast<CTelephony::TIdentityServiceV1*> ( const_cast<TUint8*> (iIdentityServiceStatusPckg.Ptr()) );
			}
				
		// Print of the service type requested
		iTestStep->INFO_PRINTF2(_L("Service type requested was: %d"), iIdentityServiceType );
		 
		// Print off the service values retrieved and those expected
		iTestStep->INFO_PRINTF2(_L("Status  expected: %d"), iExpectedIdentityServiceStatus.iIdentityStatus  );
		iTestStep->INFO_PRINTF2(_L("Status retrieved: %d"), data->iIdentityStatus );
		  
		// Check that the retrieved values match those expected.
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		iTestStep->TEST1( data->iIdentityStatus  == iExpectedIdentityServiceStatus.iIdentityStatus, ETrue   );
	 }
	else
		{
		iTestStep->INFO_PRINTF2(_L("Unexpected request status received:: %d"), iStatus.Int() );
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		}

	CActiveScheduler::Stop();
	}
	
CTestSuppServFuncFlightModeOnWithPublishSubscribe::CTestSuppServFuncFlightModeOnWithPublishSubscribe()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSuppServFuncFlightModeOnWithPublishSubscribe"));
}

/**
@SYMTestCaseID Etel3rdParty 24.1, TestSuppServiveFuncFlightModeOn
@SYMTestCaseDesc Test GetCallForwardingStatus, GetCallBarringStatus 
* GetCallWaitingStatus and GetIdentityServiceStatus 3rd party APIs
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc,
*  REQ 2116.4.11/12
*/

enum TVerdict CTestSuppServFuncFlightModeOnWithPublishSubscribe::doTestStepL()
{
	INFO_PRINTF1(_L("START TEST 24.1"));	
	 
	INFO_PRINTF1(_L(" TEST: Supp service retrieval with flight mode on with publish and subscribe"));

	// Set the config.txt test number mapping
	SetTestNumber(13);
	
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

	// Set the expected request status
	TRequestStatus ExpStat=KErrNone;
	CTelephony::TCallBarringSupplServicesV1 ExpectedCallBarringStatus; 
	CTelephony::TCallForwardingSupplServicesV1 ExpectedCallForwardingStatus;
	CTelephony::TCallWaitingSupplServicesV1 ExpectedCallWaitingStatus;  

	// Create an active object and push it onto the clean up stack
	CTestGetSupplementaryServiceStatusAct* getSupplementaryServiceStatusAct = CTestGetSupplementaryServiceStatusAct::NewLC(this, iTelephony);
	
	//
	// Test call barring //
	//
	INFO_PRINTF1(_L("Test retrieval of call barring information"));
	
	ExpStat=KErrAccessDenied;
	ExpectedCallBarringStatus.iCallBarring=CTelephony::EStatusActive;
	getSupplementaryServiceStatusAct->TestGetCBSupplementaryServiceStatus(ExpectedCallBarringStatus,CTelephony::EBarAllIncoming, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	INFO_PRINTF1(_L("Test retrieval of call forwarding information"));
	
	ExpStat=KErrAccessDenied;
	ExpectedCallForwardingStatus.iCallForwarding=CTelephony::EStatusActive;
	getSupplementaryServiceStatusAct->TestGetCFSupplementaryServiceStatus(ExpectedCallForwardingStatus, CTelephony::ECallForwardingUnconditional, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	INFO_PRINTF1(_L("Test retrieval of call waiting information"));
	
	ExpStat=KErrAccessDenied;
	ExpectedCallWaitingStatus.iCallWaiting=CTelephony::EStatusActive;
	getSupplementaryServiceStatusAct->TestGetCWSupplementaryServiceStatus(ExpectedCallWaitingStatus, CTelephony::EVoiceService, ExpStat);
	CActiveScheduler::Start();
	
	
	//
	// Test identity service
	//
	
	// Set the config.txt test number mapping
	SetTestNumber(14);
	
	INFO_PRINTF1(_L(" TEST: Identity service retrieval with flight mode on "));
	
	// Create an active object and push it onto the clean up stack
	CTestGetIdentityServiceStatusAct* getIdentityServiceStatusAct = CTestGetIdentityServiceStatusAct::NewLC(this, iTelephony);

	CTelephony::TIdentityServiceV1 ExpectedIdentityStatus;
	CTelephony::TIdentityService IdentityServiceTypeRequired; 
	
	//Switch flight mode on (ie radio mode off)
	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	
	// Set expected status to access denied as flight mode is on.
	ExpStat=KErrAccessDenied;
	// Test EIdServiceCallerPresentation
	INFO_PRINTF1(_L("Test retrieval of EIdServiceCallerPresentation status"));
	// Set the type of supplementary service info required
	IdentityServiceTypeRequired=CTelephony::EIdServiceCallerPresentation;
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.	
	ExpectedIdentityStatus.iIdentityStatus=CTelephony::EIdServiceActiveDefaultRestricted;
	// Attempt to retrieve the service information. Pass the expected values. 
	getIdentityServiceStatusAct->TestGetIdentityServiceStatus(ExpectedIdentityStatus,IdentityServiceTypeRequired, ExpStat);
	// Start the active scheduler
	CActiveScheduler::Start();
	
	INFO_PRINTF1(_L("END TEST 24.1"));
	
	CleanupStack::PopAndDestroy(getIdentityServiceStatusAct);	 
	CleanupStack::PopAndDestroy(getSupplementaryServiceStatusAct);   
	CleanupStack::PopAndDestroy(&phonePowerProperty);	 
	 
	return TestStepResult() ;	  
	 
}

