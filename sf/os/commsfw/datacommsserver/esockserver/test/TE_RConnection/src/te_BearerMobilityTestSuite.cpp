// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of those test steps for bearermobility covering the AO based API which is not suitable for
// testing under esock test steps
// 
//

/**
 @file te_BearerMobilityTestSuite.cpp
 @file
*/

#include "te_BearerMobilityTestSuite.h"
#include <es_enum_internal.h>
#include <ecom/ecom.h>
#include <comms-infras/mobilitymessagesecom.h>
#include <dummynifvar.h>

using namespace ESock;

// CBearerMobilityTestStep_761402
//-------------------------------
static const TInt KPreferredAccessPointId = 4;
static const TInt KLessPreferredAccessPointId = 5;

static const TInt KSnapTwoPreferences = 15;

enum TVerdict CBearerMobilityTestStep_761402::doTestStepL(void)
	{
	// Create and install an active scheduler as we are testing an AO api
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create and connect the socket server
	TInt err;
	err = iSocketServ.Connect();
	TESTEL(err == KErrNone, err);

	// Create and open the connection
	err = iConnection.Open(iSocketServ);
	TESTEL(err == KErrNone, err);

	// Set the availability of each access point of significance
	SetAvailabilityL(
		KPreferredAccessPointId,
		TAvailabilityStatus(TAvailabilityStatus::EMaxAvailabilityScore));
	SetAvailabilityL(
		KLessPreferredAccessPointId,
		TAvailabilityStatus(TAvailabilityStatus::EMaxAvailabilityScore));
	
	// Start the connection
	TCommSnapPref prefs;
	prefs.SetSnap(KSnapTwoPreferences);
	err = iConnection.Start(prefs);
	TESTEL(err == KErrNone, err);

	// Now create the mobility notification AO
	iMobilityExtension = CActiveCommsMobilityApiExt::NewL(iConnection, *this);

	// Make the preferred become unavailable
	SetAvailabilityL(KPreferredAccessPointId, TAvailabilityStatus(TAvailabilityStatus::EMinAvailabilityScore));

	// Bump the state
	BumpTestStepState();

	// Start the scheduler which will ultimately run the rest of the test
	TRAP_IGNORE(CActiveScheduler::Start())
	
	CleanupStack::PopAndDestroy(scheduler);
	
	iConnection.Close();
	iSocketServ.Close();

	return TestStepResult();
	}

// From MMobilityProtocolResp
void CBearerMobilityTestStep_761402::PreferredCarrierAvailable(TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless)
	{
	switch(GetTestStepState())
		{
		case 1:
			// Check that we got what we expected
			TEST(aOldAP == KPreferredAccessPointId);
			TEST(aNewAP == KLessPreferredAccessPointId);
			TEST(aIsUpgrade == EFalse);
			TEST(aIsSeamless == EFalse);

			// Arrange for migration
			iMobilityExtension->MigrateToPreferredCarrier();

			BumpTestStepState();
			break;

		default:
			INFO_PRINTF1(KErrTestStepAstray);
			SetTestStepResult(EFail);
			break;
		}
	}

void CBearerMobilityTestStep_761402::NewCarrierActive(TAccessPointInfo aNewAP, TBool aIsSeamless)
	{
	switch(GetTestStepState())
		{
		case 2:
			// Check we got what we expected first
			TEST(aNewAP == KLessPreferredAccessPointId);
			TEST(aIsSeamless == EFalse);

			// Accept the new carrier and we are done so stop the scheduler
			iMobilityExtension->NewCarrierAccepted();
			CActiveScheduler::Stop();

			BumpTestStepState();
			break;

		default:
			INFO_PRINTF1(KErrTestStepAstray);
			SetTestStepResult(EFail);
			break;
		}
	}

void CBearerMobilityTestStep_761402::Error(TInt aError)
	{
	// Most likely a cancel but check for any kind of error
	if(aError == KErrCancel)
		{
		INFO_PRINTF1(_L("Mobility extension cancelled"));
		}
	else
		{
		// Some kind of unexpected error during mobility migration
		INFO_PRINTF2(_L("Unexpected error (ordinarily KErrCancel) from mobility extension. error:%d"), aError);
		SetTestStepError(aError);
		}
	}


// CBearerMobilityTestStep_761403
//-------------------------------

enum TVerdict CBearerMobilityTestStep_761403::doTestStepL(void)
	{
	// Fail by default
	SetTestStepResult(EFail);
	
	// Create and connect the socket server
	TInt err;
	RSocketServ sockServ;
	err = sockServ.Connect();
	TESTEL(err == KErrNone, err);

	// Create and open the connection
	RConnection conn;
	err = conn.Open(sockServ);
	TESTEL(err == KErrNone, err);

	// Set the availability of each access point of significance
	SetAvailabilityL(KPreferredAccessPointId, TAvailabilityStatus(TAvailabilityStatus::EMaxAvailabilityScore));
	SetAvailabilityL(KLessPreferredAccessPointId, TAvailabilityStatus(TAvailabilityStatus::EMaxAvailabilityScore));
	
	// Start the connection
	TCommSnapPref prefs;
	prefs.SetSnap(KSnapTwoPreferences);
	err = conn.Start(prefs);
	TESTEL(err == KErrNone, err);

	// Attach a subconn to the default
	RSubConnection subConn;
	err = subConn.Open(sockServ, RSubConnection::EAttachToDefault, conn);
	TESTEL(err == KErrNone, err);

	// Now create the mobility notification and try to open on the subconnection!
	RCommsMobilityApiExt mobExtension;
	err = mobExtension.Open(subConn);
	if(err == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Received KErrNotSupported as expected"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Negative test: Expect KErrNotSupported. error:%d"), err);
		SetTestStepError(err);
		}
	
	// Close the resources we opened and out of here
	subConn.Close();
	conn.Close();
	sockServ.Close();

	return TestStepResult();
	}

// CBearerMobilityTestStepBase
//----------------------------
const TUid KAvailabilityTestingPubSubUid = {0x10272F42};

void CBearerMobilityTestStepBase::SetAvailabilityL(TInt aAccessPointId, ESock::TAvailabilityStatus aStatus)
	{
	RProperty a;

	// Set the availability of the access point as prescribed
	TInt error = a.Define(KAvailabilityTestingPubSubUid, aAccessPointId, RProperty::EInt);
	if((error != KErrAlreadyExists) && (error != KErrNone))
		{
		INFO_PRINTF1(_L("Could not define property used for availability simulation"));
		User::Leave(KErrGeneral);
		}
	
	// Successfully created the property so attach to it for use
	error = a.Attach(KAvailabilityTestingPubSubUid, aAccessPointId);
	TESTEL(error == KErrNone, error);

	// Now set the availability as prescribed
	a.Set(aStatus.Score());
	}


// Entry point
//------------

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CBearerMobilitySuiteTestServer* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err, server = CBearerMobilitySuiteTestServer::NewL());
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




// Test suite server
//------------------

_LIT(KServerName, "te_BearerMobilityTestSuite");

CBearerMobilitySuiteTestServer* CBearerMobilitySuiteTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CBearerMobilitySuiteTestServer* server = new (ELeave) CBearerMobilitySuiteTestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop();
	return server;
	}
	
CBearerMobilitySuiteTestServer::~CBearerMobilitySuiteTestServer()
	{
	}
	
CTestStep* CBearerMobilitySuiteTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName.Compare(_L("NET_BMOBILITY_761402")) == 0)
		testStep = new CBearerMobilityTestStep_761402();
	else if(aStepName.Compare(_L("NET_BMOBILITY_761403")) == 0)
		testStep = new CBearerMobilityTestStep_761403();
	else if(aStepName.Compare(_L("NET_BMOBILITY_761404")) == 0)
		testStep = new CBearerMobilityTestStep_761404();
	else if(aStepName.Compare(_L("NET_BMOBILITY_761405")) == 0)
		testStep = new CBearerMobilityTestStep_761405();
	
	return testStep;
	}

const TInt KBufferLength = 512;
const TInt KEchoPortNum = 8;
TInt RequestInterfaceDownL(RConnection& conn, RSocketServ& ss)
/**
 * Sends a special packet to the dummy nif telling it to signal that the interface is down
 * @param conn The connection to signal
 * @param ss The socket server that the connection is within
 * @return system wide error code
 */
{
	TRequestStatus status;

	TBuf8<KBufferLength> buffer;

	RSocket sock;
	TInetAddr dest;

	User::LeaveIfError(sock.Open(ss, KAfInet, KSockDatagram, KProtocolInetUdp, conn));
	CleanupClosePushL(sock);

	User::LeaveIfError(sock.SetOpt(KSoReuseAddr, KSolInetIp, 1)); 
	User::LeaveIfError(sock.SetLocalPort(KEchoPortNum)); 

	dest.SetAddress(KDummyNifLocalAddressBase + 4);
	dest.SetPort(KDummyNifCmdPort);

	buffer.SetMax();
	buffer.FillZ();
	buffer[0] = (TUint8) KForceDisconnect;

	sock.SendTo(buffer, dest, 0, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	buffer.Zero();
	// I expect some sort of response
	// this fails - possibly ok as the interface might have gone already??
	//sock.RecvFrom(buffer, dest, 0, status);
	//User::WaitForRequest(status);
	//TESTEL(KErrNone == status.Int(), status.Int());

	CleanupStack::PopAndDestroy(&sock);

	return KErrNone;
}

/**
   Bring up a snap with two bearers, both on.
   Kill the top priority.
   Accept the migration.
   Reject the new carrier.
   Verify that the new carrier has gone away.
   The new carrier should go away because of the short idle timer. The bearers in this test have only short idle timer to verify.

   This test verifies DEF143172
*/   
enum TVerdict CBearerMobilityTestStep_761404::doTestStepL()
	{
	const TInt KTestSnap = 20;
	const TInt KPriority1BearerAP = 7;
	const TInt KPriority2BearerAP = 8;

	TInt error;
	TInterfaceNotificationBuf info1;
	TNifProgress progress;
	TNifProgressBuf progbuf(progress);
	TBuf8<KNotificationEventMaxSize> eventnotbuf;
	TRequestStatus status1, status2, status3;
	
	RSocketServ ss;
	RConnection connS, connAIN;
	RCommsMobilityApiExt mobility;
	TConnSnapPref p;
	p.SetSnap(KTestSnap);

	INFO_PRINTF1(_L("Description: "));
	INFO_PRINTF1(_L("Bring up a snap with two bearers, both on."));
   	INFO_PRINTF1(_L("Kill the top priority."));
	INFO_PRINTF1(_L("Accept the migration."));
	INFO_PRINTF1(_L("Reject the new carrier."));
	INFO_PRINTF1(_L("Verify that the new carrier has goes away."));
   	INFO_PRINTF1(_L("The new carrier should go away because of the short idle timer. The bearers in this test have only short idle timer to verify."));
	INFO_PRINTF1(_L("============================================================================="));
	
	// Set both bearer to be up
	RProperty a1;
	error = a1.Define(KAvailabilityTestingPubSubUid, KPriority1BearerAP, RProperty::EInt);
	if((error == KErrAlreadyExists) || (error == KErrNone))
		{
		error = a1.Attach(KAvailabilityTestingPubSubUid, KPriority1BearerAP);
		ASSERT(error == KErrNone);
		
		a1.Set(ESock::TAvailabilityStatus::EMaxAvailabilityScore);
		}
	
	RProperty a2;
	error = a2.Define(KAvailabilityTestingPubSubUid, KPriority1BearerAP, RProperty::EInt);
	if((error == KErrAlreadyExists) || (error == KErrNone))
		{
		error = a2.Attach(KAvailabilityTestingPubSubUid, KPriority1BearerAP);
		ASSERT(error == KErrNone);
		
		a2.Set(ESock::TAvailabilityStatus::EMaxAvailabilityScore);
		}

	// Open session and subsessions
	User::LeaveIfError(ss.Connect());
	CleanupClosePushL(ss);

	User::LeaveIfError(connAIN.Open(ss));
	CleanupClosePushL(connAIN);

	User::LeaveIfError(connS.Open(ss));
	CleanupClosePushL(connS);

	// Register for notification of connections going up/down
	INFO_PRINTF1(_L("Register for notification of connections going up/down"));
	connAIN.AllInterfaceNotification(info1, status1);

	// Start the connection
	INFO_PRINTF2(_L("Start connection (%d)"), p.Snap());
	User::LeaveIfError(connS.Start(p));

	// Check all interface notification
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTL(info1().iConnectionInfo.iIapId == KPriority1BearerAP && info1().iState == 0);

	// Register for notification again
	connAIN.AllInterfaceNotification(info1, status1);

	// Check the current progress
	error = connS.Progress(progress);
	INFO_PRINTF3(_L("Current progress iStage(%d) iError(%d)"), progress.iStage, progress.iError);
	TESTL(error == KErrNone);
	TESTL(progress.iStage == KLinkLayerOpen && progress.iError == KErrNone);

	// open mobility
	INFO_PRINTF1(_L("Open mobility on connection"));
	error = mobility.Open(connS);
	TESTL(error == KErrNone);
	CleanupClosePushL(mobility);

	// register for mobility notification
	eventnotbuf.Zero();
	INFO_PRINTF1(_L("Register for notification"));
	mobility.RegisterForMobilityNotification(eventnotbuf, status2);

	// Turn first priority off
	INFO_PRINTF1(_L("Turn off highest priority bearer"));
	a1.Set(ESock::TAvailabilityStatus::EMinAvailabilityScore);
	RequestInterfaceDownL(connS, ss);
	
	// wait for all interface notification to report bearer down
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTL(info1().iConnectionInfo.iIapId == KPriority1BearerAP && info1().iState == 1);
	
	// register for all interface notification again
	connAIN.AllInterfaceNotification(info1, status1);

	// wait for mobility notification
	User::WaitForRequest(status2);	
	CCommsApiExtRespMsg * msg = CCommsApiExtRespMsg::NewL(eventnotbuf);
	CleanupStack::PushL(msg);
	
	ESock::CPreferredCarrierAvailable* pca = static_cast<ESock::CPreferredCarrierAvailable*>(msg);
	INFO_PRINTF3(_L("Preferred carrier available NewAP(%d) OldAP(%d)"), pca->NewAPInfo().AccessPoint(), pca->OldAPInfo().AccessPoint());
	TESTL(KPriority2BearerAP == pca->NewAPInfo().AccessPoint() && KPriority1BearerAP ==  pca->OldAPInfo().AccessPoint());

	// register for mobility and migrate
	mobility.RegisterForMobilityNotification(eventnotbuf, status2);
	INFO_PRINTF1(_L("Migrate to preferred carrier"));
	mobility.MigrateToPreferredCarrier();

	// check that the expected bearer has come up
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTEL(info1().iConnectionInfo.iIapId == KPriority2BearerAP && info1().iState == 0, KErrCorrupt);

	// register for it again
	connAIN.AllInterfaceNotification(info1, status1);

	// register for progress notification, for KLinkLayerClosed.
	connS.ProgressNotification(progbuf, status3, KLinkLayerClosed);

	// wait for new carrier active
	User::WaitForRequest(status2);
	
	CleanupStack::PopAndDestroy(msg);
	// verify the new carrier is as expected and then reject it
	msg = CCommsApiExtRespMsg::NewL(eventnotbuf);
	CleanupStack::PushL(msg);
	ESock::CNewCarrierActive* nca = static_cast<ESock::CNewCarrierActive*>(msg);
	INFO_PRINTF2(_L("NewCarrierActive NewAP(%d)"), nca->NewAPInfo().AccessPoint());
	TESTL(nca->NewAPInfo().AccessPoint() == KPriority2BearerAP);
	CleanupStack::PopAndDestroy(msg);

	INFO_PRINTF1(_L("Reject new carrier"));
	mobility.NewCarrierRejected();

	// wait for all interface notification to tell us connection has gone down.
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTL(info1().iConnectionInfo.iIapId == KPriority2BearerAP && info1().iState == 1);

	User::WaitForRequest(status3);
	INFO_PRINTF3(_L("Progress notification received iStage(%d) iError(%d)"), progbuf().iStage, progbuf().iError);
	TESTL(progbuf().iStage == KLinkLayerClosed);

	CleanupStack::PopAndDestroy(&mobility);
	CleanupStack::PopAndDestroy(&connS);
	CleanupStack::PopAndDestroy(&connAIN);
	CleanupStack::PopAndDestroy(&ss);
	
	return EPass;
	}

/**
   Bring up a snap with three bearers, all on.
   Kill the top priority.
   Accept the migration.
   Reject the new carrier.
   Verify that the new carrier has gone away.
   Get offered the third priority.
   Accept third priority
   Verify KLinkLayerClosed is never received.

   This test verifies DEF143172
*/
enum TVerdict CBearerMobilityTestStep_761405::doTestStepL()
	{
	const TInt KTestSnap = 23;
	const TInt KPriority1BearerAP = 7;
	const TInt KPriority2BearerAP = 8;
	const TInt KPriority3BearerAP = 4;

	TInt error;
	TInterfaceNotificationBuf info1;
	TNifProgress progress;
	TNifProgressBuf progbuf(progress);
	TBuf8<KNotificationEventMaxSize> eventnotbuf;
	TRequestStatus status1, status2, status3;
	
	RSocketServ ss;
	RConnection connS, connAIN;
	RCommsMobilityApiExt mobility;
	TConnSnapPref p;
	p.SetSnap(KTestSnap);

	INFO_PRINTF1(_L("Description: "));
	INFO_PRINTF1(_L("Bring up a snap with three bearers, all on."));
   	INFO_PRINTF1(_L("Kill the top priority."));
	INFO_PRINTF1(_L("Accept the migration."));
	INFO_PRINTF1(_L("Reject the new carrier."));
	INFO_PRINTF1(_L("Verify that the new carrier has goes away."));
	INFO_PRINTF1(_L("Accept the migration to the third priority."));
	INFO_PRINTF1(_L("Accept the new carrier."));
   	INFO_PRINTF1(_L("The new carrier should go away because of the short idle timer. The bearers in this test have only short idle timer to verify."));
	INFO_PRINTF1(_L("============================================================================="));
	
	// Set both bearer to be up
	RProperty a1;
	error = a1.Define(KAvailabilityTestingPubSubUid, KPriority1BearerAP, RProperty::EInt);
	if((error == KErrAlreadyExists) || (error == KErrNone))
		{
		error = a1.Attach(KAvailabilityTestingPubSubUid, KPriority1BearerAP);
		ASSERT(error == KErrNone);
		
		a1.Set(ESock::TAvailabilityStatus::EMaxAvailabilityScore);
		}
	
	RProperty a2;
	error = a2.Define(KAvailabilityTestingPubSubUid, KPriority1BearerAP, RProperty::EInt);
	if((error == KErrAlreadyExists) || (error == KErrNone))
		{
		error = a2.Attach(KAvailabilityTestingPubSubUid, KPriority1BearerAP);
		ASSERT(error == KErrNone);
		
		a2.Set(ESock::TAvailabilityStatus::EMaxAvailabilityScore);
		}

	RProperty a3;
	error = a3.Define(KAvailabilityTestingPubSubUid, KPriority3BearerAP, RProperty::EInt);
	if((error == KErrAlreadyExists) || (error == KErrNone))
		{
		error = a3.Attach(KAvailabilityTestingPubSubUid, KPriority3BearerAP);
		ASSERT(error == KErrNone);
		
		a3.Set(ESock::TAvailabilityStatus::EMaxAvailabilityScore);
		}
	
	// Open session and subsessions
	User::LeaveIfError(ss.Connect());
	CleanupClosePushL(ss);

	User::LeaveIfError(connAIN.Open(ss));
	CleanupClosePushL(connAIN);

	User::LeaveIfError(connS.Open(ss));
	CleanupClosePushL(connS);

	// Register for notification of connections going up/down
	INFO_PRINTF1(_L("Register for notification of connections going up/down"));
	connAIN.AllInterfaceNotification(info1, status1);

	// Start the connection
	INFO_PRINTF2(_L("Start connection (%d)"), p.Snap());
	User::LeaveIfError(connS.Start(p));

	// Check all interface notification
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTL(info1().iConnectionInfo.iIapId == KPriority1BearerAP && info1().iState == 0);

	// Register for notification again
	connAIN.AllInterfaceNotification(info1, status1);

	// Check the current progress
	error = connS.Progress(progress);
	INFO_PRINTF3(_L("Current progress iStage(%d) iError(%d)"), progress.iStage, progress.iError);
	TESTL(error == KErrNone);
	TESTL(progress.iStage == KLinkLayerOpen && progress.iError == KErrNone);

	// open mobility
	INFO_PRINTF1(_L("Open mobility on connection"));
	error = mobility.Open(connS);
	TESTL(error == KErrNone);
	CleanupClosePushL(mobility);

	// register for mobility notification
	eventnotbuf.Zero();
	INFO_PRINTF1(_L("Register for notification"));
	mobility.RegisterForMobilityNotification(eventnotbuf, status2);

	// Turn first priority off
	INFO_PRINTF1(_L("Turn off highest priority bearer"));
	a1.Set(ESock::TAvailabilityStatus::EMinAvailabilityScore);
	RequestInterfaceDownL(connS, ss);
	
	// wait for all interface notification to report bearer down
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTL(info1().iConnectionInfo.iIapId == KPriority1BearerAP && info1().iState == 1);
	
	// register for all interface notification again
	connAIN.AllInterfaceNotification(info1, status1);

	// wait for mobility notification
	User::WaitForRequest(status2);	
	CCommsApiExtRespMsg * msg = CCommsApiExtRespMsg::NewL(eventnotbuf);
	CleanupStack::PushL(msg);
	
	ESock::CPreferredCarrierAvailable* pca = static_cast<ESock::CPreferredCarrierAvailable*>(msg);
	INFO_PRINTF3(_L("Preferred carrier available NewAP(%d) OldAP(%d)"), pca->NewAPInfo().AccessPoint(), pca->OldAPInfo().AccessPoint());
	TESTL(KPriority2BearerAP == pca->NewAPInfo().AccessPoint() && KPriority1BearerAP ==  pca->OldAPInfo().AccessPoint());

	// register for mobility and migrate
	mobility.RegisterForMobilityNotification(eventnotbuf, status2);
	INFO_PRINTF1(_L("Migrate to preferred carrier"));
	mobility.MigrateToPreferredCarrier();

	// check that the expected bearer has come up
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTEL(info1().iConnectionInfo.iIapId == KPriority2BearerAP && info1().iState == 0, KErrCorrupt);

	// register for it again
	connAIN.AllInterfaceNotification(info1, status1);

	// register for progress notification, for KLinkLayerClosed.
	connS.ProgressNotification(progbuf, status3, KLinkLayerClosed);

	// wait for new carrier active
	User::WaitForRequest(status2);
	
	CleanupStack::PopAndDestroy(msg);
	// verify the new carrier is as expected and then reject it
	msg = CCommsApiExtRespMsg::NewL(eventnotbuf);
	CleanupStack::PushL(msg);
	ESock::CNewCarrierActive* nca = static_cast<ESock::CNewCarrierActive*>(msg);
	INFO_PRINTF2(_L("NewCarrierActive NewAP(%d)"), nca->NewAPInfo().AccessPoint());
	TESTL(nca->NewAPInfo().AccessPoint() == KPriority2BearerAP);
	CleanupStack::PopAndDestroy(msg);

	mobility.RegisterForMobilityNotification(eventnotbuf, status2);
	INFO_PRINTF1(_L("Reject new carrier"));
	mobility.NewCarrierRejected();

	// wait for mobility notification
	User::WaitForRequest(status2);	
	msg = CCommsApiExtRespMsg::NewL(eventnotbuf);
	CleanupStack::PushL(msg);
	
	pca = static_cast<ESock::CPreferredCarrierAvailable*>(msg);
	INFO_PRINTF3(_L("Preferred carrier available NewAP(%d) OldAP(%d)"), pca->NewAPInfo().AccessPoint(), pca->OldAPInfo().AccessPoint());
	TESTL(KPriority3BearerAP == pca->NewAPInfo().AccessPoint() && KPriority2BearerAP == pca->OldAPInfo().AccessPoint());

	// register for mobility and migrate
	mobility.RegisterForMobilityNotification(eventnotbuf, status2);
	INFO_PRINTF1(_L("Migrate to preferred carrier"));
	mobility.MigrateToPreferredCarrier();

	// check that the expected bearer has come up
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTL(info1().iConnectionInfo.iIapId == KPriority3BearerAP && info1().iState == 0);
	
	connAIN.AllInterfaceNotification(info1, status1);

	// wait for all interface notification to tell us connection has gone down.
	User::WaitForRequest(status1);
	INFO_PRINTF3(_L("Interface changed IAP (%d) is in state (%d)"), info1().iConnectionInfo.iIapId, info1().iState);
	TESTL(info1().iConnectionInfo.iIapId == KPriority2BearerAP && info1().iState == 1);

	CleanupStack::PopAndDestroy(msg);
	// verify the new carrier is as expected and then reject it
	msg = CCommsApiExtRespMsg::NewL(eventnotbuf);
	CleanupStack::PushL(msg);
	nca = static_cast<ESock::CNewCarrierActive*>(msg);
	INFO_PRINTF2(_L("NewCarrierActive NewAP(%d)"), nca->NewAPInfo().AccessPoint());
	TESTL(nca->NewAPInfo().AccessPoint() == KPriority3BearerAP);
	CleanupStack::PopAndDestroy(msg);

	INFO_PRINTF1(_L("Accept migration"));
	mobility.NewCarrierAccepted();

	// Ensure we're never got KLinkLayerClosed
	TESTL(status3.Int() == KRequestPending);
	connS.CancelProgressNotification();
	
	CleanupStack::PopAndDestroy(&mobility);
	CleanupStack::PopAndDestroy(&connS);
	CleanupStack::PopAndDestroy(&connAIN);
	CleanupStack::PopAndDestroy(&ss);
	
	return EPass;
	}
