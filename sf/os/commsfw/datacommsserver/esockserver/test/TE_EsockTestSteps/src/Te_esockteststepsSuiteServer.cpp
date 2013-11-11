// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file Te_esockteststepsSuiteServer.cpp
*/

#include <e32base.h>
#include <c32comm.h>
#include <c32root.h>
#include <ecom/ecom.h>

#include "SocketServer.TestSteps.h"
#include "Connections.TestSteps.h"
#include "SubConnections.TestSteps.h"
#include "Sockets.TestSteps.h"

#include "BearerMobility.TestSteps.h"

#include "DataMonitoring.TestSteps.h"
#include "ESockDebug.TestSteps.h"
#include "ConnectionServer.TestSteps.h"
#include "Commsdat.TestSteps.h"

#include "Qos.TestSteps.h"
#include "cancelconnectrsocketStep.h"
#include "Te_esockteststepsSuiteServer.h"

CTe_esockteststepsSuite* CTe_esockteststepsSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_esockteststepsSuite * server = new (ELeave) CTe_esockteststepsSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(server->ServerName());


	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_esockteststepsSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_esockteststepsSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	return err;
    }




CTe_esockteststepsSuite::CTe_esockteststepsSuite()
	: CTestServer()
	, iEsockTest(NULL)
	{
	}

CTe_esockteststepsSuite::~CTe_esockteststepsSuite()
	{
	if( iEsockTest != NULL )
		{
		delete iEsockTest;
		iEsockTest = NULL;
		}
	}

#define NEW_ESOCK_TESTSTEP(TEST_STEP_NAME) \
	else if(aStepName.CompareF (K##TEST_STEP_NAME) == 0)\
		testStep = new C##TEST_STEP_NAME(iEsockTest);

#define FIRST_NEW_ESOCK_TESTSTEP(TEST_STEP_NAME) \
	if(aStepName.CompareF (K##TEST_STEP_NAME) == 0) \
				testStep = new C##TEST_STEP_NAME(iEsockTest);



CTestStep* CTe_esockteststepsSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	// They are created "just in time" when the worker thread is created
	// More test steps can be added below
	if(aStepName.CompareF (KAddRSocketToRSubConnectionStep) == 0)
				testStep = new CAddRSocketToRSubConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KAcceptIncomingConnectionStep) == 0)
				testStep = new CAcceptIncomingConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KAsyncAcceptIncomingConnectionStep) == 0)
                testStep = new CAsyncAcceptIncomingConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KAddRSocketToRSubConnectionOOMStep) == 0)
				testStep = new CAddRSocketToRSubConnectionOOMStep(iEsockTest);
	else if(aStepName.CompareF (KBackupRestoreStep) == 0)
				testStep = new CBackupRestoreStep(iEsockTest);
	else if(aStepName.CompareF (KCheckMobilityNotificationStep) == 0)
				testStep = new CCheckMobilityNotificationStep(iEsockTest);
	else if(aStepName.CompareF (KCheckRSubConnectionEventsStep) == 0)
				testStep = new CCheckRSubConnectionEventsStep(iEsockTest);
	else if(aStepName.CompareF (KCloseRCommsMobilityAPIExtStep) == 0)
				testStep = new CCloseRCommsMobilityAPIExtStep(iEsockTest);

	else if(aStepName.CompareF (KCloseRConnectionStep) == 0)
				testStep = new CCloseRConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KCloseRSocketServStep) == 0)
				testStep = new CCloseRSocketServStep(iEsockTest);
	else if(aStepName.CompareF (KCloseRSocketStep) == 0)
				testStep = new CCloseRSocketStep(iEsockTest);
	else if(aStepName.CompareF (KCloseRSubConnectionStep) == 0)
				testStep = new CCloseRSubConnectionStep(iEsockTest);

	else if(aStepName.CompareF (KCloseRSubConParameterBundleStep) == 0)
				testStep = new CCloseRSubConParameterBundleStep(iEsockTest);
	else if(aStepName.CompareF (KCompareExtIPLink99ParameterSetStep) == 0)
				testStep = new CCompareExtIPLink99ParameterSetStep(iEsockTest);
	else if(aStepName.CompareF (KCompareExtSBLPParameterSetStep) == 0)
				testStep = new CCompareExtSBLPParameterSetStep(iEsockTest);
	else if(aStepName.CompareF (KCompareGenQosParameterSetStep) == 0)
				testStep = new CCompareGenQosParameterSetStep(iEsockTest);
	else if(aStepName.CompareF (KCompareGenSBLPParameterSetStep) == 0)
				testStep = new CCompareGenSBLPParameterSetStep(iEsockTest);
	else if(aStepName.CompareF (KinitialiseExtMbmsParameterSetStep) == 0)
				testStep = new CinitialiseExtMbmsParameterSetStep(iEsockTest);
	else if(aStepName.CompareF (KinitialiseGenMbmsParameterSetStep) == 0)
				testStep = new CinitialiseGenMbmsParameterStep(iEsockTest);

	else if(aStepName.CompareF (KConnectRSocketServStep) == 0)
				testStep = new CConnectRSocketServStep(iEsockTest);
	else if(aStepName.CompareF (KConnectRSocketStep) == 0)
				testStep = new CConnectRSocketStep(iEsockTest);
    else if(aStepName.CompareF (KBindRSocketStep) == 0)
                testStep = new CBindRSocketStep(iEsockTest);	
	else if(aStepName.CompareF (KControlRSubConnectionStep) == 0)
				testStep = new CControlRSubConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KCreateRConnectionStep) == 0)
				testStep = new CCreateRConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KCreateRSocketServStep) == 0)
				testStep = new CCreateRSocketServStep(iEsockTest);
	else if(aStepName.CompareF (KCreateRSocketStep) == 0)
				testStep = new CCreateRSocketStep(iEsockTest);

	else if(aStepName.CompareF (KCreateSBLPFlowIdStep) == 0)
				testStep = new CCreateSBLPFlowIdStep(iEsockTest);
	else if(aStepName.CompareF (KSetSBLPFlowIdStep) == 0)
				testStep = new CSetSBLPFlowIdStep(iEsockTest);

	else if(aStepName.CompareF (KCreateRSubConnectionStep) == 0)
				testStep = new CCreateRSubConnectionStep(iEsockTest);

	else if(aStepName.CompareF (KCreateRSubConParameterBundleStep) == 0)
				testStep = new CCreateRSubConParameterBundleStep(iEsockTest);
	else if(aStepName.CompareF (KCreateRSubConParameterFamilyStep) == 0)
				testStep = new CCreateRSubConParameterFamilyStep(iEsockTest);
	else if(aStepName.CompareF (KGetRSubConParametersStep) == 0)
				testStep = new CGetRSubConParametersStep(iEsockTest);
	else if(aStepName.CompareF (KinitialiseExtIPLink99ParameterSetStep) == 0)
				testStep = new CinitialiseExtIPLink99ParameterSetStep(iEsockTest);
              else if(aStepName.CompareF (KinitialiseExtIPLinkR5ParameterSetStep) == 0)
                            testStep = new CinitialiseExtIPLinkR5ParameterSetStep(iEsockTest);
              else if (aStepName.CompareF(KCompareExtIPLinkR5ParameterSetStep) == 0)
            	  testStep = new CCompareExtIPLinkR5ParameterSetStep(iEsockTest);
              else if(aStepName.CompareF (KinitialiseIMSExtParameterSetStep) == 0)
                            testStep = new CinitialiseIMSExtParameterSetStep(iEsockTest);
// SYMBIAN_NETWORKING_UMTSR5
	else if(aStepName.CompareF (KinitialiseExtSblpParameterSetStep) == 0)
				testStep = new CinitialiseExtSblpParameterSetStep(iEsockTest);
	else if(aStepName.CompareF (KinitialiseGenQosParameterSetStep) == 0)
				testStep = new CinitialiseGenQosParameterSetStep(iEsockTest);
	else if(aStepName.CompareF (KinitialiseGenSblpParameterSetStep) == 0)
				testStep = new CinitialiseGenSblpParameterSetStep(iEsockTest);
	else if(aStepName.CompareF (KMultipleOpenRSubConnectionStep) == 0)
				testStep = new CMultipleOpenRSubConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KMultipleAddRSubConnectionStep) == 0)
				testStep = new CMultipleAddRSubConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KAddRSocketToRSubConNoWaitStep) == 0)
	                testStep = new CAddRSocketToRSubConNoWaitStep(iEsockTest);
	else if(aStepName.CompareF (KMultipleRemoveRSubConnectionStep) == 0)
				testStep = new CMultipleRemoveRSubConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KOpenRCommsMobilityAPIExtStep) == 0)
				testStep = new COpenRCommsMobilityAPIExtStep(iEsockTest);

	else if(aStepName.CompareF (KOpenRConnectionStep) == 0)
				testStep = new COpenRConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KOpenRSocketStep) == 0)
				testStep = new COpenRSocketStep(iEsockTest);
	else if(aStepName.CompareF (KOpenRSubConnectionOOMStep) == 0)
				testStep = new COpenRSubConnectionOOMStep(iEsockTest);
	else if(aStepName.CompareF (KOpenRSubConnectionStep) == 0)
				testStep = new COpenRSubConnectionStep(iEsockTest);

	else if(aStepName.CompareF (KWaitForIncomingConnectionStep) == 0)
				testStep = new CWaitForIncomingConnectionStep(iEsockTest);

	else if(aStepName.CompareF (KRegisterForRSubConnectionEventsStep) == 0)
				testStep = new CRegisterForRSubConnectionEventsStep(iEsockTest);
	else if(aStepName.CompareF (KRemoveRSocketFromRSubConnectionOOMStep) == 0)
				testStep = new CRemoveRSocketFromRSubConnectionOOMStep(iEsockTest);
	else if(aStepName.CompareF (KRemoveRSocketFromRSubConnectionStep) == 0)
				testStep = new CRemoveRSocketFromRSubConnectionStep(iEsockTest);
	else if(aStepName.CompareF (KRejectIncomingConnectionStep) == 0)
				testStep = new CRejectIncomingConnectionStep(iEsockTest);

	else if(aStepName.CompareF (KSendReceiveRSocketStep) == 0)
				testStep = new CSendReceiveRSocketStep(iEsockTest);

	else if(aStepName.CompareF (KSetRSubConParametersStep) == 0)
				testStep = new CSetRSubConParametersStep(iEsockTest);

	else if(aStepName.CompareF (KShutdownRSocketStep) == 0)
				testStep = new CShutdownRSocketStep(iEsockTest);
	else if(aStepName.CompareF (KStartRConnectionStep) == 0)
				testStep = new CStartRConnectionStep(iEsockTest);
    else if(aStepName.CompareF (KStartStopCrazyLoopRConnectionStep) == 0)
                testStep = new CStartStopCrazyLoopRConnectionStep(iEsockTest);
    else if(aStepName.CompareF (KStartCloseCrazyLoopRConnectionStep) == 0)
                testStep = new CStartCloseCrazyLoopRConnectionStep(iEsockTest);  
	else if(aStepName.CompareF (KAwaitRConnectionStartComplete) == 0)
				testStep = new CAwaitRConnectionStartComplete(iEsockTest);
	
	else if(aStepName.CompareF (KStartSimTSYStep) == 0)
				testStep = new CStartSimTSYStep(iEsockTest);

	else if(aStepName.CompareF (KStopRConnectionStep) == 0)
				testStep = new CStopRConnectionStep(iEsockTest);

	else if(aStepName.CompareF (KStopSimTSYStep) == 0)
				testStep = new CStopSimTSYStep(iEsockTest);
	else if(aStepName.CompareF (KSimTSYTriggerEventStep) == 0)
				testStep = new CSimTSYTriggerEventStep(iEsockTest);

	else if(aStepName.CompareF (KCleanAllStep) == 0)
				testStep = new CCleanAllStep(iEsockTest);

	else if(aStepName.CompareF (KTe_CreatePublishSubscribeVar) == 0)
					testStep = new CCreatePublishSubscribeVar(iEsockTest);
	else if(aStepName.CompareF (KTe_CheckPublishSubscribeVar) == 0)
						testStep = new CCheckPublishSubscribeVar(iEsockTest);

	else if(aStepName.CompareF (KInitialiseRSubConParameterFamilyStep) == 0)
				testStep = new CInitialiseRSubConParameterFamilyStep(iEsockTest);
    else if(aStepName.CompareF(KmoveConnectedSocketToSubconnectionStep)==0)
              	testStep = new CmoveConnectedSocketToSubconnectionStep(iEsockTest);
    else if(aStepName.CompareF(KCancelConnectrsocketStep)==0)
                testStep = new CCancelConnectrsocketStep(iEsockTest);

    else if(aStepName.CompareF(Kacceptcarrierrcommsmobilityapiext)==0)
  				testStep = new Cacceptcarrierrcommsmobilityapiext(iEsockTest);
    else if(aStepName.CompareF(Krejectcarrierrcommsmobilityapiext)==0)
  				testStep = new Crejectcarrierrcommsmobilityapiext(iEsockTest);
    else if(aStepName.CompareF(KCancelMobilityNotificationStep)==0)
  				testStep = new CCancelMobilityNotificationStep(iEsockTest);
    else if(aStepName.CompareF(Kmigratercommsmobilityapiext)==0)
  				testStep = new Cmigratercommsmobilityapiext(iEsockTest);
    else if(aStepName.CompareF(Kignorercommsmobilityapiext)==0)
  				testStep = new Cignorercommsmobilityapiext(iEsockTest);
    else if(aStepName.CompareF(KRegisterForMobilityNotificationStep) == 0)
  				testStep = new CRegisterForMobilityNotificationStep(iEsockTest);

    else if(aStepName.CompareF (KattachtorconnectionStep) == 0)
                testStep = new CattachtorconnectionStep(iEsockTest);
    else if(aStepName.CompareF(Kmobilitynotifyrcommsmobilityapiext)==0)
  				testStep = new Cmobilitynotifyrcommsmobilityapiext(iEsockTest);
    else if(aStepName.CompareF(Kcancelnotifyrcommsmobilityapiext)==0)
  				testStep = new Ccancelnotifyrcommsmobilityapiext(iEsockTest);

	// Data monitoring
	else if(aStepName.CompareF (KOpenRCommsDataMonitoringApiExtStep) == 0)
				testStep = new COpenRCommsDataMonitoringApiExtStep(iEsockTest);
	else if(aStepName.CompareF (KRequestDataTransferredStep) == 0)
				testStep = new CRequestDataTransferredStep(iEsockTest);
	else if(aStepName.CompareF (KCloseRCommsDataMonitoringApiExtStep) == 0)
				testStep = new CCloseRCommsDataMonitoringApiExtStep(iEsockTest);
	else if(aStepName.CompareF (KReceiveDataReceivedNotificationStep) == 0)
				testStep = new CReceiveDataReceivedNotificationStep(iEsockTest);
	else if(aStepName.CompareF (KReceiveDataSentNotificationStep) == 0)
				testStep = new CReceiveDataSentNotificationStep(iEsockTest);
	else if(aStepName.CompareF (KCancelDataReceivedNotificationStep) == 0)
				testStep = new CCancelDataReceivedNotificationStep(iEsockTest);
	else if(aStepName.CompareF (KCancelDataSentNotificationStep) == 0)
				testStep = new CCancelDataSentNotificationStep(iEsockTest);
	else if(aStepName.CompareF (KRequestDataReceivedNotificationStep) == 0)
				testStep = new CRequestDataReceivedNotificationStep(iEsockTest);
	else if(aStepName.CompareF (KRequestDataSentNotificationStep) == 0)
				testStep = new CRequestDataSentNotificationStep(iEsockTest);

#ifdef __1060_TESTCODE__
	// Mobility
	else if(aStepName.CompareF (KregisterformobilitynotificationStep) == 0)
				testStep = new CregisterformobilitynotificationStep(iEsockTest);

	else if(aStepName.CompareF (KChangeBearerAvailabilityStep) == 0)
				testStep = new CChangeBearerAvailabilityStep(iEsockTest);
	else if(aStepName.CompareF (KCSelectRConnectionStep) == 0)
				testStep = new CSelectRConnectionStep(iEsockTest);

	else if(aStepName.CompareF (KGetAvailabilityAPIStep) == 0)
				testStep = new CGetAvailabilityAPIStep(iEsockTest);
	else if(aStepName.CompareF (KRegisterForAvailabilityNotificationStep) == 0)
				testStep = new CRegisterForAvailabilityNotificationStep(iEsockTest);
	else if(aStepName.CompareF (KCheckForAvailabilityStep) == 0)
				testStep = new CCheckForAvailabilityStep(iEsockTest);

	else if(aStepName.CompareF (KConfirmSocketDataTransferStep) == 0)
				testStep = new CConfirmSocketDataTransferStep(iEsockTest);
	else if(aStepName.CompareF (KCreateBearerAvailabilitySimulatorStep) == 0)
				testStep = new CCreateBearerAvailabilitySimulatorStep(iEsockTest);
#endif

	// ESock debug control based test steps
	NEW_ESOCK_TESTSTEP(AppendMessageInterceptPattern)
	NEW_ESOCK_TESTSTEP(EnableMessageInterceptRegister)
	NEW_ESOCK_TESTSTEP(DisableMessageInterceptRegister)
	NEW_ESOCK_TESTSTEP(ResetMessageInterceptRegister)
	NEW_ESOCK_TESTSTEP(QueryMsgInterceptComplete)
	// RConnectionServ API
	NEW_ESOCK_TESTSTEP(CreateRConnectionServStep)
	NEW_ESOCK_TESTSTEP(ConnectRConnectionServStep)
	NEW_ESOCK_TESTSTEP(RConnectionServApiExtStep)
	NEW_ESOCK_TESTSTEP(CloseRConnectionServStep)
	NEW_ESOCK_TESTSTEP(AccessPointStatusQueryStep)
	NEW_ESOCK_TESTSTEP(CancelAccessPointStatusQueryStep)
	NEW_ESOCK_TESTSTEP(RequestAccessPointNotificationStep)
	NEW_ESOCK_TESTSTEP(ReceiveAccessPointNotificationStep)
	NEW_ESOCK_TESTSTEP(CancelAccessPointNotificationStep)
	NEW_ESOCK_TESTSTEP(RequestMBMSNtfnStep)
	NEW_ESOCK_TESTSTEP(ReceiveMBMSNtfnStep)
	NEW_ESOCK_TESTSTEP(CancelMBMSNtfnStep)
	// Availability control
	NEW_ESOCK_TESTSTEP(SetAccessPointAvailabilityStep)
    NEW_ESOCK_TESTSTEP(SetPppAccessPointAvailabilityStep)
    NEW_ESOCK_TESTSTEP(SetPacketDataAccessPointAvailabilityStep)
	NEW_ESOCK_TESTSTEP(SetWifiAccessPointAvailabilityStep)

	NEW_ESOCK_TESTSTEP(EnumerateConnectionsStep)
	// Commsdat helper tests
	NEW_ESOCK_TESTSTEP(DeleteCommsdatRecordStep)
	NEW_ESOCK_TESTSTEP(CreateCommsdatRecordStep)
	NEW_ESOCK_TESTSTEP(UpdateCommsdatRecordStep)

	NEW_ESOCK_TESTSTEP(InitialiseDummyProtocolParameterSetStep)

	// Sockets
	NEW_ESOCK_TESTSTEP(SendReceiveIoctlStep)
	NEW_ESOCK_TESTSTEP(SocketProtocolDescriptionStep)
	NEW_ESOCK_TESTSTEP(SocketGetOptionStep)
	NEW_ESOCK_TESTSTEP(SocketSetOptionStep)

	// Socket server
	NEW_ESOCK_TESTSTEP(SocketServerNumProtocolsStep)
	NEW_ESOCK_TESTSTEP(SocketServerFindProtocolStep)
	NEW_ESOCK_TESTSTEP(SocketServerGetProtocolInfoStep)
	NEW_ESOCK_TESTSTEP(SocketServerStartProtocolStep)
	NEW_ESOCK_TESTSTEP(SocketServerStopProtocolStep)

	// General
	NEW_ESOCK_TESTSTEP(CompareIntegerValuesStep)

	NEW_ESOCK_TESTSTEP(RegisterProgressNotificationStep)
	NEW_ESOCK_TESTSTEP(CheckProgressNotificationStep)
	NEW_ESOCK_TESTSTEP(CheckNegativeProgressNotificationStep)
	NEW_ESOCK_TESTSTEP(GetParameters_IntStep)
	NEW_ESOCK_TESTSTEP(WaitStep)
	
	return testStep;
	}

/**
Returns server name based on the EKA version
@internalTechnology
@test
@return		Descriptor containing the servername
*/
const TPtrC CTe_esockteststepsSuite::ServerName() const
	{
	// On EKA2, test server runs in its own process.
	// So we arrive at the server name using the exe from which it is loaded.
	// This is useful when doing cap tests, as we create duplicate exe's using setcap then.
	TParsePtrC serverName(RProcess().FileName());
	return serverName.Name();
	}

