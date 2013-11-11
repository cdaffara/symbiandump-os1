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
// EtelPacketMbmsContext.cpp
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
#include "Te_EtelPacketMbmsContext.h"
#include "Te_etelpckt_defs.h"

// constructor
CTestMbmsContext::CTestMbmsContext()
	{
	// store the name of this test case
	SetTestStepName(_L("MBMS_CONTEXT"));
	}

// destructor
CTestMbmsContext::~CTestMbmsContext()
{
}

TVerdict CTestMbmsContext::doTestStepPreambleL()
	{
	CTestStepEtelPacket::doTestStepPreambleL();
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
	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);
	return TestStepResult();
	}
	
TVerdict CTestMbmsContext::doTestStepPostambleL()
	{
	delete iTestScheduler;
	iPcktService.Close();
	iPhone.Close();
	CTestStepEtelPacket::doTestStepPostambleL();
	return TestStepResult();
	}
	
enum TVerdict CTestMbmsContext::doTestStepL( void )
/**
 * Test step to check MBMS service availability functionality
 */
	{
	TestOpenMbmsContextL();
	TestInitialiseMbmsContextL();
	TestMbmsConfigurationL();
	TestActivateMbmsContextL();
	TestMbmsServiceStatusL();
	TestMbmsUpdateSessionL();
	TestMbmsGetSessionListL();
	TestDeactivateMbmsContextL();
	return TestStepResult();
	}

void CTestMbmsContext::TestOpenMbmsContextL()
	{
	/*
	 * Verify whether a MBMS context is opened successfully.
	 */
	TInt retMbmsContext;
	TName contextName;
	
	retMbmsContext = iMbmsContext.OpenNewContext(iPcktService, contextName);
	if(retMbmsContext != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open packet context"));
		User::Leave(retMbmsContext);
		}
	INFO_PRINTF1(_L("Open a MBMS context - Completed"));
	}

void CTestMbmsContext::TestMbmsConfigurationL()
	{
	/**
	 * Set the MBMS context configuration successfully
	 */
	TRequestStatus mbmsConfigStatus;
	RPacketMbmsContext::TContextConfigMbmsV1 mbmsContextConfig;
	mbmsContextConfig.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_ONE);
	mbmsContextConfig.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_ONE);
	mbmsContextConfig.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_ONE);
	mbmsContextConfig.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_ONE;
	mbmsContextConfig.iMbmsServicePriority = DPCKTTSY_MBMS_SERVICEPRIORITY_ONE;
	mbmsContextConfig.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_ONE;
	mbmsContextConfig.iMbmsSessionFlag = DPCKTTSY_MBMS_SESSION_FLAG_TRUE;
	RPacketMbmsContext::TContextConfigMbmsV1Pckg iMbmsContextConfigPckg(mbmsContextConfig);
	
	iMbmsContext.SetConfig(mbmsConfigStatus, iMbmsContextConfigPckg);
	User::WaitForRequest(mbmsConfigStatus);
	TESTL(mbmsConfigStatus == KErrNone);
	INFO_PRINTF1(_L("MBMS context set configurations - Completed"));
	
	/**
	 * Set the MBMS context configuration successfully
	 */
	TRequestStatus mbmsConfigStatusCancel;
	iMbmsContext.SetConfig(mbmsConfigStatusCancel, iMbmsContextConfigPckg);
	iMbmsContext.CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(mbmsConfigStatusCancel);
	TESTL(mbmsConfigStatusCancel == KErrCancel);
	INFO_PRINTF1(_L("Cancel MBMS context set configurations - CANCELLED"));
		
	/**
	 * Set MBMS context configuration with invalid inputs
	 */
	mbmsContextConfig.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_INVALID);
	mbmsContextConfig.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_INVALID);
	mbmsContextConfig.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_INVALID);
	mbmsContextConfig.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_INVALID;
	mbmsContextConfig.iMbmsSessionFlag = DPCKTTSY_MBMS_SESSION_FLAG_TRUE;
	mbmsContextConfig.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_INVALID;
	
	iMbmsContext.SetConfig(mbmsConfigStatus, iMbmsContextConfigPckg);
	User::WaitForRequest(mbmsConfigStatus);
	TESTL(mbmsConfigStatus == KErrCorrupt);
	INFO_PRINTF1(_L("MBMS set context invalid configurations - Completed"));
	
	/**
	 * Get MBMS context configuration.
	 */
	RPacketMbmsContext::TContextConfigMbmsV1 mbmsGetContextConfig;
	RPacketMbmsContext::TContextConfigMbmsV1Pckg mbmsGetContextConfigPckg(mbmsGetContextConfig);
	iMbmsContext.GetConfig(mbmsConfigStatus, mbmsGetContextConfigPckg);
	User::WaitForRequest(mbmsConfigStatus);
	mbmsGetContextConfig = mbmsGetContextConfigPckg();
	TESTL(mbmsGetContextConfig.iTmgi.GetMCC() == DPCKTTSY_MBMS_MCC_ONE);
	TESTL(mbmsGetContextConfig.iTmgi.GetMNC() == DPCKTTSY_MBMS_MNC_ONE);
	TESTL(mbmsGetContextConfig.iTmgi.GetServiceId() == DPCKTTSY_MBMS_SERVICEID_ONE);
	TESTL(mbmsGetContextConfig.iMbmsAccessBearer == DPCKTTSY_MBMS_ACCESSBEARER_ONE);
	TESTL(mbmsGetContextConfig.iMbmsServicePriority == DPCKTTSY_MBMS_SERVICEPRIORITY_ONE);
	TESTL(mbmsGetContextConfig.iMbmsServiceMode == DPCKTTSY_MBMS_SERVICEMODE_ONE);
	TESTL(mbmsGetContextConfig.iMbmsSessionFlag = DPCKTTSY_MBMS_SESSION_FLAG_TRUE);
	TESTL(mbmsConfigStatus == KErrNone);
	INFO_PRINTF1(_L("MBMS get context configurations - Completed"));	
	}

void CTestMbmsContext::TestActivateMbmsContextL()
	{
	/**
	 * Activate MBMS context.
	 */
	TRequestStatus activateStatus;
	iMbmsContext.Activate(activateStatus);
	User::WaitForRequest(activateStatus);
	TESTL(activateStatus == KErrNone);
	INFO_PRINTF1(_L("MBMS context activation - Completed"));
	}
	
void CTestMbmsContext::TestMbmsServiceStatusL()
	{
	/**
	 * Verify the status of the service being received
	 */
	TRequestStatus activateStatus;
	RPacketMbmsContext::TContextStatus contextStatus;
	iMbmsContext.NotifyStatusChange(activateStatus,contextStatus);
	User::WaitForRequest(activateStatus);
	TESTL(contextStatus == DPCKTTSY_CONTEXT_STATUS2);
	TESTL(activateStatus == KErrNone);
	INFO_PRINTF1(_L("MBMS context status notification - Completed"));
	
	/**
	 * Receive notification on status change of a MBMS service
	 */
	TInt retStatus = iMbmsContext.GetStatus(contextStatus);
	TESTL(contextStatus == DPCKTTSY_CONTEXT_STATUS1);
	TESTL(retStatus == KErrNone);
	INFO_PRINTF1(_L("Get MBMS context status - Completed"));
	
	/**
	 * Cancel MBMS status notification on status change.
	 */
	iMbmsContext.NotifyStatusChange(activateStatus,contextStatus);
	iMbmsContext.CancelAsyncRequest(EPacketContextNotifyStatusChange);
	User::WaitForRequest(activateStatus);
	TESTL( activateStatus == KErrCancel);
	INFO_PRINTF1(_L("MBMS context status notification cancellation - CANCELLED"));
	}
	
void CTestMbmsContext::TestMbmsUpdateSessionL()
	{
	/**
	 * Add MBMS session
	 */
	TRequestStatus sessionStatus;
	TUint sessionId = DPCKTTSY_MBMS_SESSIONID_ONE;
	iMbmsContext.UpdateMbmsSessionList(sessionStatus, EAddEntries, sessionId);
	User::WaitForRequest(sessionStatus);
	TESTL(sessionStatus == KErrNone);
	INFO_PRINTF1(_L("Add MBMS session - Completed"));

	/**
	 * Test cancelling the UpdateMbmsSessionList API
	 */
	TRequestStatus sessionStatusCancel;
	sessionId = DPCKTTSY_MBMS_SESSIONID_ONE;
	iMbmsContext.UpdateMbmsSessionList(sessionStatusCancel, EAddEntries, sessionId);
	iMbmsContext.CancelAsyncRequest(EPacketContextUpdateMbmsSessionList);
	User::WaitForRequest(sessionStatusCancel);
	TESTL(sessionStatusCancel == KErrCancel);
	INFO_PRINTF1(_L("Cancel UpdateMbmsSessionList - CANCELLED"));
	
	/**
	 * Remove MBMS session
	 */
	iMbmsContext.UpdateMbmsSessionList(sessionStatus, ERemoveEntries, sessionId);
	User::WaitForRequest(sessionStatus);
	TESTL(sessionStatus == KErrNone);
	INFO_PRINTF1(_L("Remove MBMS session - Completed"));
	
	/**
	 * Remove an invalid MBMS session
	 */
	sessionId = DPCKTTSY_MBMS_SESSIONID_INVALID;
	iMbmsContext.UpdateMbmsSessionList(sessionStatus, ERemoveEntries, sessionId);
	User::WaitForRequest(sessionStatus);
	TESTL(sessionStatus == KErrNotFound);
	INFO_PRINTF1(_L("Remove invalid MBMS session - Completed"));
	
	/**
	 * Remove all MBMS sessions
	 */
	iMbmsContext.UpdateMbmsSessionList(sessionStatus, ERemoveAllEntries);
	User::WaitForRequest(sessionStatus);
	TESTL(sessionStatus == KErrNone);
	INFO_PRINTF1(_L("Remove all MBMS sessions - Completed"));
	
	/**
	 * Remove all MBMS sessions completes with error.
	 */
	sessionId = DPCKTTSY_MBMS_SESSIONID_ONE;
	iMbmsContext.UpdateMbmsSessionList(sessionStatus, ERemoveAllEntries, sessionId);
	User::WaitForRequest(sessionStatus);
	TESTL(sessionStatus == KErrMbmsImpreciseServiceEntries);
	INFO_PRINTF1(_L("Failure scenario - Remove all MBMS sessions - Completed"));
	}

void CTestMbmsContext::TestDeactivateMbmsContextL()
	{
	/**
	 * Deactivate a MBMS context.
	 */
	TRequestStatus mbmsDeactivateStatus;
	iMbmsContext.Deactivate(mbmsDeactivateStatus);
	User::WaitForRequest(mbmsDeactivateStatus);
	TESTL(mbmsDeactivateStatus == KErrNone);
	INFO_PRINTF1(_L("MBMS context deactivation - Completed"));
	
	/**
	 * Delete the context
	 */
	TRequestStatus mbmsDeleteStatus;
	iMbmsContext.Delete(mbmsDeleteStatus);
	User::WaitForRequest(mbmsDeleteStatus);
	TESTL(mbmsDeleteStatus == KErrNone);
	INFO_PRINTF1(_L("MBMS context deleted - Completed"));
	
	/**
	 * Close the context
	 */
	iMbmsContext.Close();
	}

void CTestMbmsContext::TestInitialiseMbmsContextL()
	{
	/**
	 * Initialise MBMS context.
	 */
	RPacketMbmsContext::TDataChannelV2 dataChannel;
	RPacketMbmsContext::TDataChannelV2Pckg dataChannelPckg(dataChannel);
	TRequestStatus mbmsInitContextStatus;
	iMbmsContext.InitialiseContext(mbmsInitContextStatus, dataChannelPckg);
	User::WaitForRequest(mbmsInitContextStatus);
	TESTL(dataChannel.iCsy == DPCKTTSY_CSY_ASYNC);
	TESTL(dataChannel.iPort == DPCKTTSY_PORT_ASYNC);
	TESTL(dataChannel.iChannelId == DPCKTTSY_CHANNEL_ID);
	TESTL(mbmsInitContextStatus == KErrNone);
	INFO_PRINTF1(_L("MBMS Initialise context - Completed"));
	}

void CTestMbmsContext::TestMbmsGetSessionListL()
	{
	__UHEAP_MARK;
	TInt ret(KErrNone);
	TRequestStatus reqStatus;
	TUint sessionId1=1000, sessionId2=2000, sessionId3=3000;
	
	/**
	 * Test serialization
	 */
	RPacketMbmsContext::CMbmsSession* mbmsSession = RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(mbmsSession);
	TESTL(mbmsSession->ExtensionId() == KETelExtPcktV1);
	mbmsSession->iSessionIdList.Append(sessionId1);
	mbmsSession->iSessionIdList.Append(sessionId2);
	mbmsSession->iSessionIdList.Append(sessionId3);
	HBufC8* buff  = HBufC8::NewL(15);
	mbmsSession->ExternalizeL(buff);	
	CleanupStack::PushL(buff);
	RPacketMbmsContext::CMbmsSession* mbmsSessionCopy = RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(mbmsSessionCopy);
	
	TBuf8<256> buffBig;
	buffBig=*buff;

	/**
	 * Memory overflow test for Internalize function
	 */
	TInt err=KErrNone;
	TInt len=0;
	do
		{
		buffBig.SetLength(len);
		TRAP(err, mbmsSessionCopy->InternalizeL(buffBig));
		if(err == KErrOverflow)
			{
			len++;	
			}
		else
			{
			TESTL(err == KErrNone);
			break;
			}		
		}
	while(len < 100); //just in case, set a limit to finish the loop.
	TESTL(len<100);
	TRAP(ret,mbmsSessionCopy->InternalizeL(buffBig));
	TEST(ret==KErrNone);
	TESTL(mbmsSessionCopy->iSessionIdList.Count() == mbmsSession->iSessionIdList.Count());
	TESTL(mbmsSessionCopy->iSessionIdList[0] == mbmsSession->iSessionIdList[0]);
	TESTL(mbmsSessionCopy->iSessionIdList[1] == mbmsSession->iSessionIdList[1]);
	TESTL(mbmsSessionCopy->iSessionIdList[2] == mbmsSession->iSessionIdList[2]);
	CleanupStack::PopAndDestroy(mbmsSessionCopy);
	CleanupStack::PopAndDestroy(buff);
	CleanupStack::PopAndDestroy(mbmsSession);
	INFO_PRINTF1(_L("Test serialization of sessions - Completed"));
	INFO_PRINTF1(_L("OOM test for Internalize API - Completed"));
	
	/**
	 * Retrieve a list of session ids of a MBMS service
	 */
	RPacketMbmsContext::CMbmsSession* mbmsSession1 = RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(mbmsSession1);
	CTestMbmsSession* testMbmsSession = CTestMbmsSession::NewLC(iMbmsContext,*mbmsSession1, this);
	testMbmsSession->Start();
	CActiveScheduler::Start();
	TEST(testMbmsSession->iRetrieve != NULL);
	if(testMbmsSession->iRetrieve )
		{
		TESTL(mbmsSession1->iSessionIdList.Count() == DPCKTTSY_MBMS_CONTEXT_SESSION_ID_COUNT);
		TESTL(mbmsSession1->iSessionIdList[0] == DPCKTTSY_MBMS_CONTEXT_SESSION_ID_ONE);
		TESTL(mbmsSession1->iSessionIdList[1] == DPCKTTSY_MBMS_CONTEXT_SESSION_ID_TWO);
		TESTL(mbmsSession1->iSessionIdList[2] == DPCKTTSY_MBMS_CONTEXT_SESSION_ID_THREE);
		}
	INFO_PRINTF1(_L("Retrieve a list of sessions IDs of a MBMS service - Completed"));
    
	/**
     * Test cancellation of retrieval of sessions
     */	
    testMbmsSession->Start();	
    testMbmsSession->iCanceller->Call();
    CActiveScheduler::Start();
    INFO_PRINTF1(_L("Cancel retrieval of MBMS sessions - Completed"));
    
	CleanupStack::PopAndDestroy(testMbmsSession);
	CleanupStack::PopAndDestroy(mbmsSession1);
	iPcktService.Close();
	iPhone.Close();
	__UHEAP_MARKEND;
	}
//
CTestMbmsSession* CTestMbmsSession::NewLC(RPacketMbmsContext& aPcktMbmsContext,RPacketMbmsContext::CMbmsSession& aSessionIdList, CTestMbmsContext* aTestMbmsContext)
	{
    CTestMbmsSession* self = new(ELeave) CTestMbmsSession(aPcktMbmsContext, aSessionIdList, aTestMbmsContext);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CTestMbmsSession::CTestMbmsSession(RPacketMbmsContext& aPcktMbmsContext, RPacketMbmsContext::CMbmsSession& aSessionIdList, CTestMbmsContext* aTestMbmsContext)
    : CActive(EPriorityHigh), iSessionIdList(aSessionIdList), iPcktMbmsContext(aPcktMbmsContext), iTestMbmsContext(aTestMbmsContext)
    {
    }

TInt CTestMbmsSession::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

void CTestMbmsSession::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrievePcktMbmsSessionList::NewL(iPcktMbmsContext, iSessionIdList);
	iCanceller=new (ELeave) CTestCanceller(this);
    }

CTestMbmsSession::~CTestMbmsSession()
    {
    delete iCanceller;
	delete iRetrieve;
	}

void CTestMbmsSession::Start()
    {
    iRetrieve->Start(iStatus);
    //Test the CompleteIfInUse case of Start API
    TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus);
	User::WaitForRequest(tmpStatus);
	iTestMbmsContext->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();    
    }

void CTestMbmsSession::RunL()
    {    
    CActiveScheduler::Stop();
	}

void CTestMbmsSession::DoCancel()
    {
    iRetrieve->Cancel();    
    }


