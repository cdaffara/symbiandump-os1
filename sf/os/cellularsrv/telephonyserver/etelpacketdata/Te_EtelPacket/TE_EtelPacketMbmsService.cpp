// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketMbmsService.cpp
// This contains EtelPacket TestCase 
// 
//

// Symbian OS includes
#include <e32base.h>
#include <etelpckt.h>
#include "pcktretrieve.h"
#include <pcktcs.h>
#include "Te_EtelPacketTestStepBase.h"
#include "testdef.h"
#include "Te_EtelPacketMbmsService.h"
#include "Te_etelpckt_defs.h"

// constructor
CTestMbmsService::CTestMbmsService()
{
	// store the name of this test case
	SetTestStepName(_L("MBMS_SERVICE"));
}

// destructor
CTestMbmsService::~CTestMbmsService()
{
}

TVerdict CTestMbmsService::doTestStepPreambleL()
	{
	CTestStepEtelPacket::doTestStepPreambleL();
	__UHEAP_MARK;
	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);
			
	TInt ret=iPhone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	if(ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone"));
		User::Leave(ret);
		}
	ret=iPcktService.Open(iPhone);
	if(ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open packet service"));
		User::Leave(ret);
		}
	return TestStepResult();
	}
TVerdict CTestMbmsService::doTestStepPostambleL()
	{
	iPcktService.Close();
	iPhone.Close();
	delete iTestScheduler;
	__UHEAP_MARKEND;
	CTestStepEtelPacket::doTestStepPostambleL();
	return TestStepResult();
	}
enum TVerdict CTestMbmsService::doTestStepL( void )
/**
 * Test step to check MBMS service availability functionality
 */
	{
	TestMbmsUpdateServiceL();
	TestMbmsRetrieveServiceListL();
	TestMbmsServicesEnumerationL();
	return TestStepResult();
	}

void CTestMbmsService::TestMbmsUpdateServiceL()
	{
	/**
	 * Add new services to be monitored to the service availability list.
	 */
	RPacketService::TMbmsServiceAvailabilityV1 aMbmsService;
	CPcktMbmsMonitoredServiceList* aServiceList= CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(aServiceList);
	TRequestStatus aReqStatusMonitorList;
	TMbmsAction aMbmsAction=EAddEntries;
	//First Service
	aMbmsService.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_ONE); 
	aMbmsService.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_ONE);
	aMbmsService.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_ONE);
	aMbmsService.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_ONE;
	aMbmsService.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_ONE;
	aServiceList->AddEntryL(aMbmsService);
	//Second Service
	aMbmsService.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_TWO); 
	aMbmsService.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_TWO);
	aMbmsService.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_TWO);
	aMbmsService.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_TWO;
	aMbmsService.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_TWO;
	aServiceList->AddEntryL(aMbmsService);	
	//Third Service
	aMbmsService.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_THREE); 
	aMbmsService.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_THREE);
	aMbmsService.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_THREE);
	aMbmsService.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_THREE;
	aMbmsService.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_THREE;
	aServiceList->AddEntryL(aMbmsService);
	
	iPcktService.UpdateMbmsMonitorServiceListL(aReqStatusMonitorList, aMbmsAction, aServiceList);
	User::WaitForRequest(aReqStatusMonitorList);
	TESTL(aReqStatusMonitorList == KErrNone);
	INFO_PRINTF1(_L("Added 3 services to the Monitor list - Completed"));
	
	/**
	 * Cancel UpdateMbmsMonitorServiceListL 
	 */
	TRequestStatus aReqStatusMonitorListCancel;
	iPcktService.UpdateMbmsMonitorServiceListL(aReqStatusMonitorListCancel, aMbmsAction, aServiceList);
	iPcktService.CancelAsyncRequest(EPacketUpdateMbmsMonitorServiceList);
	User::WaitForRequest(aReqStatusMonitorListCancel);
	CleanupStack::PopAndDestroy(aServiceList); 
	TESTL(aReqStatusMonitorListCancel == KErrCancel);
	INFO_PRINTF1(_L("Cancellation of addition to the Monitor list - CANCELLED"));
	
	/**
	 * Remove selected services from the service availability list.
	 */
	TRequestStatus aReqStatusMonitorListRemove;
	aServiceList= CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(aServiceList);
	aMbmsAction=ERemoveEntries;
	aMbmsService.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_TWO); 
	aMbmsService.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_TWO);
	aMbmsService.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_TWO);
	aMbmsService.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_TWO;
	aMbmsService.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_TWO;
	aServiceList->AddEntryL(aMbmsService);
	
	iPcktService.UpdateMbmsMonitorServiceListL (aReqStatusMonitorListRemove, aMbmsAction, aServiceList);
	User::WaitForRequest(aReqStatusMonitorListRemove);
	CleanupStack::PopAndDestroy(aServiceList);
	TESTL(aReqStatusMonitorListRemove == KErrNone);
	INFO_PRINTF1(_L("Removed a service from the Monitor list - Completed"));
	
	/**
	 * Remove an invalid, single, service entry from the service availability list
	 */
	TRequestStatus aReqStatusMonitorListRemoveOneInvalid;
	aServiceList= CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(aServiceList);
	aMbmsAction=ERemoveEntries;
	aMbmsService.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_INVALID); 
	aMbmsService.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_INVALID);
	aMbmsService.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_INVALID);
	aMbmsService.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_INVALID;
	aMbmsService.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_INVALID;
	aServiceList->AddEntryL(aMbmsService);
	
	iPcktService.UpdateMbmsMonitorServiceListL (aReqStatusMonitorListRemoveOneInvalid, aMbmsAction, aServiceList);
	User::WaitForRequest(aReqStatusMonitorListRemoveOneInvalid);
	CleanupStack::PopAndDestroy(aServiceList);
	TESTL(aReqStatusMonitorListRemoveOneInvalid == KErrNotFound);
	INFO_PRINTF1(_L("Removed an invalid service entry from the Monitor list - Completed"));
	
	/**
	 * Remove a list of services from the service availability list. A few of them are invalid services
	 */
	aServiceList= CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(aServiceList);
	//TRequestStatus aReqStatusMonitorList;
	aMbmsAction=ERemoveEntries;
	//First Service
	aMbmsService.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_ONE); 
	aMbmsService.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_ONE);
	aMbmsService.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_ONE);
	aMbmsService.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_ONE;
	aMbmsService.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_ONE;
	aServiceList->AddEntryL(aMbmsService);
	//Second Service
	aMbmsService.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_INVALID); 
	aMbmsService.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_INVALID);
	aMbmsService.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_INVALID);
	aMbmsService.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_INVALID;
	aMbmsService.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_INVALID;
	aServiceList->AddEntryL(aMbmsService);	
	//Third Service
	aMbmsService.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_THREE); 
	aMbmsService.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_THREE);
	aMbmsService.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_THREE);
	aMbmsService.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_THREE;
	aMbmsService.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_THREE;
	aServiceList->AddEntryL(aMbmsService);
	
	iPcktService.UpdateMbmsMonitorServiceListL (aReqStatusMonitorList, aMbmsAction, aServiceList);
	User::WaitForRequest(aReqStatusMonitorList);
	CleanupStack::PopAndDestroy(aServiceList);
	TESTL(aReqStatusMonitorList == KErrMbmsImpreciseServiceEntries);
	INFO_PRINTF1(_L("Removal of list of entries (2nd entry invalid) - Completed"));
		
	/**
	 * Remove all services from the service availability list.
	 */
	TRequestStatus aReqStatusMonitorListRemoveAll;
	aMbmsAction=ERemoveAllEntries;
	iPcktService.UpdateMbmsMonitorServiceListL (aReqStatusMonitorListRemoveAll, aMbmsAction);
	User::WaitForRequest(aReqStatusMonitorListRemoveAll);
	TESTL(aReqStatusMonitorListRemoveAll == KErrNone);
	INFO_PRINTF1(_L("Removed all services from the Monitor list - Completed"));
	}

void CTestMbmsService::TestMbmsRetrieveServiceListL()
	{
	/**
	 * Receive notification on MBMS service availability
	 */
	TRequestStatus reqStatusMbmsService;
	iPcktService.NotifyMbmsServiceAvailabilityChange(reqStatusMbmsService);
	User::WaitForRequest(reqStatusMbmsService);
	TESTL(reqStatusMbmsService == KErrNone);
	INFO_PRINTF1(_L("Receive notification about a change in monitor service list - Completed"));
	
	/**
	 * Cancel the service availability notification
	 */
	iPcktService.NotifyMbmsServiceAvailabilityChange(reqStatusMbmsService);
	iPcktService.CancelAsyncRequest(EPacketNotifyMbmsServiceAvailabilityChange);
	User::WaitForRequest(reqStatusMbmsService);
	TESTL(reqStatusMbmsService == KErrCancel);
	INFO_PRINTF1(_L("Cancellation of notification about a change in monitor service list - CANCELLED"));
	
	/**
	 * Retrieve MBMS service list.
	 */
	CTestRetrieveEtelPacketMonitorServices *pRetrieve = CTestRetrieveEtelPacketMonitorServices::NewLC(iPcktService,this);
  	TBool startReqd = ETrue;
  	// To test the 2 phase retrieval of CRetrievePcktMbmsMonitoredServices 
  	pRetrieve->TestRetrieve(startReqd);
   	//Delete test object
  	CleanupStack::PopAndDestroy();
  	INFO_PRINTF1(_L("Retreive MBMS monitor list - Completed"));
  	
  	/**
	 * Cancel retrieval of the MBMS service list
	 */
  	pRetrieve = CTestRetrieveEtelPacketMonitorServices::NewLC(iPcktService,this);
  	pRetrieve->TestRetrieve(startReqd);
  	pRetrieve->TestCancel();
  	//Delete test object
  	CleanupStack::PopAndDestroy(); //pRetrieve
  	INFO_PRINTF1(_L("Cancel retreival of MBMS monitor list - Completed"));
  	}

//

CTestMbmsService::CTestRetrieveEtelPacketMonitorServices* CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::NewLC(RPacketService& aService, CTestMbmsService* aTestService)
	{
	CTestRetrieveEtelPacketMonitorServices* monitorService=new(ELeave) CTestRetrieveEtelPacketMonitorServices(aService,aTestService);
	CleanupStack::PushL(monitorService);
	monitorService->ConstructL();
	return monitorService;
	}
	  
CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::CTestRetrieveEtelPacketMonitorServices(RPacketService& aService,CTestMbmsService* aTestService)
: CActive(EPriorityNormal), iService(aService),iTestService(aTestService)
{}
	 
void CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::ConstructL()
	{
	CActiveScheduler::Add(this);
	iRetrieve=CRetrievePcktMbmsMonitoredServices::NewL(iService);
	iCanceller=new (ELeave) CTestCanceller(this);
	}
    
CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::~CTestRetrieveEtelPacketMonitorServices()
	{
	Cancel();
	delete iRetrieve;
	delete iCanceller;
	}

void CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::Start()
	{
	iRetrieve->Start(iStatus);
	//Test the CompleteIfInUse case of Start API
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus);
	User::WaitForRequest(tmpStatus);
	iTestService->TEST(tmpStatus.Int() == KErrInUse);
	SetActive();
	}       

void CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::RunL()
	{
	TInt ret=iStatus.Int();
	User::LeaveIfError(ret);
	ret=CheckMonitorServicesResultsL();
	User::LeaveIfError(ret);
	ret=CheckMonitorServicesErrorResultsL();
	User::LeaveIfError(ret);
	CActiveScheduler::Stop();
	}

void CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::TestCancel()
	{
	Start();
	CTestCanceller* cancelMbmsService = RetrieveCancelPtr();
	cancelMbmsService->Call();

	CActiveScheduler::Start();
	TBool cancelled = (iStatus == KErrCancel);

	// Ensure that AO can be used after its previous transaction was cancelled
	Start();
	CActiveScheduler::Start();

	if (cancelled)
		{
		iTestService->INFO_PRINTF1(_L("Cancelled retrieval of MBMS service List"));
		iTestService->INFO_PRINTF1(_L("CRetrievePcktMbmsMonitoredServices (async & cancel (Cancelled Request)) - CANCELLED"));
		}
	else
		{
		iTestService->INFO_PRINTF1(_L("CRetrievePcktMbmsMonitoredServices (async & cancel (Request Not Cancelled))- COMPLETED"));
		}	
	} 
	
CTestCanceller* CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::RetrieveCancelPtr()
	{
	return iCanceller;
	}

void CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::DoCancel()
	{
	iRetrieve->Cancel();
	}       
  
TInt CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::CheckMonitorServicesResultsL()
	{
	TInt ret=KErrNone;

	CPcktMbmsMonitoredServiceList* mbmsList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, mbmsList=iRetrieve->RetrieveListL(););

	//Check that a list is returned back to the test
	iTestService->TESTL(mbmsList!=NULL);

if (mbmsList)
	{
	//Check that the number of entries in the list is as expected
	iTestService->TESTL(mbmsList->Enumerate()==DPCKTTSY_MBMS_SERVICELIST_COUNT_ADD);
	// Get each entry
	RPacketService::TMbmsServiceAvailabilityV1 entry;
	for (TInt index=0; index < DPCKTTSY_MBMS_SERVICELIST_COUNT_ADD; index++)
			{
			TRAPD(ret,entry=mbmsList->GetEntryL(index));
				if (ret != KErrNone)
					break;
			switch (index)
				{
			case 0:
				iTestService->TESTL(entry.iMbmsServiceMode == DPCKTTSY_MBMS_SERVICEMODE_ONE);
				iTestService->TESTL(entry.iMbmsAccessBearer == DPCKTTSY_MBMS_ACCESSBEARER_ONE);
				iTestService->TESTL(entry.iMbmsAvailabilityStatus == DPCKTTSY_MBMS_AVAILABILITYSTATUS_ONE);
				iTestService->TESTL(entry.iTmgi.GetMCC() == DPCKTTSY_MBMS_MCC_ONE);
				iTestService->TESTL(entry.iTmgi.GetMNC() == DPCKTTSY_MBMS_MNC_ONE);
				iTestService->TESTL(entry.iTmgi.GetServiceId() == DPCKTTSY_MBMS_SERVICEID_ONE);
				break;
				
			case 1:
			iTestService->TESTL(entry.iMbmsServiceMode == DPCKTTSY_MBMS_SERVICEMODE_TWO);
			iTestService->TESTL(entry.iMbmsAccessBearer == DPCKTTSY_MBMS_ACCESSBEARER_TWO);
			iTestService->TESTL(entry.iMbmsAvailabilityStatus == DPCKTTSY_MBMS_AVAILABILITYSTATUS_TWO);
			iTestService->TESTL(entry.iTmgi.GetMCC() == DPCKTTSY_MBMS_MCC_TWO);
			iTestService->TESTL(entry.iTmgi.GetMNC() == DPCKTTSY_MBMS_MNC_TWO);
			iTestService->TESTL(entry.iTmgi.GetServiceId() == DPCKTTSY_MBMS_SERVICEID_TWO);
				break;
			
			case 2:
			default:
			iTestService->TESTL(entry.iMbmsServiceMode == DPCKTTSY_MBMS_SERVICEMODE_THREE);
			iTestService->TESTL(entry.iMbmsAccessBearer == DPCKTTSY_MBMS_ACCESSBEARER_THREE);
			iTestService->TESTL(entry.iMbmsAvailabilityStatus == DPCKTTSY_MBMS_AVAILABILITYSTATUS_THREE);
			iTestService->TESTL(entry.iTmgi.GetMCC() == DPCKTTSY_MBMS_MCC_THREE);
			iTestService->TESTL(entry.iTmgi.GetMNC() == DPCKTTSY_MBMS_MNC_THREE);
			iTestService->TESTL(entry.iTmgi.GetServiceId() == DPCKTTSY_MBMS_SERVICEID_THREE);
				break;
				}
		}
	delete mbmsList;
	}
return ret;
}

TInt CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::CheckMonitorServicesErrorResultsL()
	{
	TInt ret=KErrGeneral;
	
	CPcktMbmsMonitoredServiceList* mbmsList =NULL;
	TInt leaveCode=KErrNone;
	CRetrievePcktMbmsMonitoredServices* pRetrieveMbmsMonitorList = CRetrievePcktMbmsMonitoredServices::NewL(iService);
	CleanupStack::PushL(pRetrieveMbmsMonitorList);
	TRAP(leaveCode, mbmsList=iRetrieve->RetrieveListL(););
	CleanupStack::PopAndDestroy();
	
	//Check that a list is returned back to the test
	if((leaveCode == KErrNotFound) && (mbmsList==NULL))
		{
		ret = KErrNone;
		}
	
	return ret;
	}


void CTestMbmsService::CTestRetrieveEtelPacketMonitorServices::TestRetrieve(TBool aStartReqd)
	{
	if(aStartReqd)
		{
		Start();
		}
	CActiveScheduler::Start();
	}
void CTestMbmsService::TestMbmsServicesEnumerationL()
	{
	/**
	 * Get the current and max number of active services.
	 */
	TRequestStatus activeServicesStatus;
	TInt currrentActiveServices, maxActiveServices;
	iPcktService.EnumerateMbmsActiveServiceList(activeServicesStatus, currrentActiveServices, maxActiveServices);
	User::WaitForRequest(activeServicesStatus);
	TESTL(currrentActiveServices == DPCKTTSY_MBMS_ACTIVESERVICE_NUMBER_OF_CONTEXTS);
	TESTL(maxActiveServices == DPCKTTSY_MBMS_ACTIVESERVICE_MAX_NUMBER_OF_CONTEXTS);
	TESTL(activeServicesStatus == KErrNone);
	INFO_PRINTF1(_L("Enumerate MBMS active services - Completed"));
	
	/**
	 * Test to cancel EnumerateMbmsActiveServiceList API
	 */
	TRequestStatus activeServicesStatusCancel;
	iPcktService.EnumerateMbmsActiveServiceList(activeServicesStatusCancel, currrentActiveServices, maxActiveServices);
	iPcktService.CancelAsyncRequest(EPacketEnumerateMbmsActiveServiceList); //EPacketEnumerateMbmsActiveServiceList
	User::WaitForRequest(activeServicesStatusCancel);
	TESTL(activeServicesStatusCancel == KErrCancel);
	INFO_PRINTF1(_L("Cancel Enumerate MBMS active services - CANCELLED"));
		
	/**
	 * Get the current and max number of monitor services.
	 */
	TRequestStatus monitorServicesStatus;
	iPcktService.EnumerateMbmsMonitorServiceList(monitorServicesStatus, currrentActiveServices, maxActiveServices);
	User::WaitForRequest(monitorServicesStatus);
	TESTL(currrentActiveServices == DPCKTTSY_MBMS_MONITORSERVICE_NUMBER_OF_CONTEXTS);
	TESTL(maxActiveServices == DPCKTTSY_MBMS_MONITORSERVICE_MAX_NUMBER_OF_CONTEXTS);
	TESTL(monitorServicesStatus == KErrNone);
	INFO_PRINTF1(_L("Enumerate MBMS monitor services - Completed"));
	
	/**
	 * Test to cancel EnumerateMbmsMonitorServiceList API
	 */
	TRequestStatus monitorServicesStatusCancel;
	iPcktService.EnumerateMbmsMonitorServiceList(monitorServicesStatusCancel, currrentActiveServices, maxActiveServices);
	iPcktService.CancelAsyncRequest(EPacketEnumerateMbmsMonitorServiceList);
	User::WaitForRequest(monitorServicesStatusCancel);
	TESTL(monitorServicesStatusCancel == KErrCancel);
	INFO_PRINTF1(_L("Cancel Enumerate MBMS monitor services - CANCELLED"));
	}

