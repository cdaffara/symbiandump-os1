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
//

#include "TE_smsinter.h"
#include "TE_smsinterserver.h"


_LIT(KServerName, "TE_SMSINTER");


/**
 *  @return - Instance of the test server
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
CSmsInterTestServer* CSmsInterTestServer::NewL()
	{
	CSmsInterTestServer*  server = new (ELeave) CSmsInterTestServer();

	CleanupStack::PushL(server);
	server->StartL(KServerName);
	CleanupStack::Pop(server);

	return server;
	} // CSmsInterTestServer::NewL


/**
 *  Much simpler, uses the new Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSmsInterTestServer*  server = NULL;

	// Create the CTestServer derived server
	TRAPD(err, server = CSmsInterTestServer::NewL());
	if (err == KErrNone)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	} // MainL


/**
 *  @return - Standard Epoc error code on exit
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err, MainL());
	delete cleanup;

	return err;
    } // E32Main


/**
 *  @return - A CTestStep derived instance
 *  Implementation of CTestServer pure virtual
 */
CTestStep* CSmsInterTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep*  testStep = NULL;

	//
	// Here the test step is created when it is needed. Note that this
	// function is non-leaving so we cannot use new(ELeave). Therefore
	// the new could return NULL, but that is not a problem as it implies
	// the test step is missing and this will be marked in the log file.
	//
	if (aStepName == _L("SendRecvTextFormated7BitEMS"))
		{
		testStep = new CSmsSendRecvTextFormated7BitEMS;
		}
	else if (aStepName == _L("SendRecvTextFormated8BitEMS"))
		{
		testStep = new CSmsSendRecvTextFormated8BitEMS;
		}
	else if (aStepName == _L("SendRecvTextFormated16BitEMS"))
		{
		testStep = new CSmsSendRecvTextFormated16BitEMS;
		}
	else if (aStepName == _L("EnumerateReadWriteAndDeleteEMSOnSIM"))
		{
		testStep = new CSmsEnumerateReadWriteAndDeleteEMSOnSIM;
		}
	else if (aStepName == _L("SendRecvPreDefSoundEMS"))
		{
		testStep = new CSmsSendRecvPreDefSoundEMS;
		}
	else if (aStepName == _L("SendRecvUserDefSoundEMS"))
		{
		testStep = new CSmsSendRecvUserDefSoundEMS;
		}
	else if (aStepName == _L("EnumerateReadWriteAndDeleteEMSOnPhone"))
		{
		testStep = new CSmsEnumerateReadWriteAndDeleteEMSOnPhone;
		}
	else if (aStepName == _L("SendRecvPicturesEMS"))
		{
		testStep = new CSmsSendRecvPicturesEMS;
		}
	else if (aStepName == _L("SendRecvPreDefAnimation"))
		{
		testStep = new CSmsSendRecvPreDefAnimation;
		}
	else if (aStepName == _L("SendRecvUserDefAnimation"))
		{
		testStep = new CSmsSendRecvUserDefAnimation;
		}
	else if (aStepName == _L("SendRecvUserPromptEMS"))
		{
		testStep = new CSmsSendRecvUserPromptEMS;
		}
	else if (aStepName == _L("SendRecvObjectDistributionEMS"))
		{
		testStep = new CSmsSendRecvObjectDistributionEMS;
		}
	else if (aStepName == _L("SendRecvManyEMSObjects"))
		{
		testStep = new CSmsSendRecvManyEMSObjects;
		}
	else if (aStepName == _L("SendRecvEMSOn8BitPort"))
		{
		testStep = new CSmsSendRecvEMSOn8BitPort;
		}
	else if (aStepName == _L("SendRecvEMSOn16BitPort"))
		{
		testStep = new CSmsSendRecvEMSOn16BitPort;
		}
	else if (aStepName == _L("TestFormattingSegmentation"))
		{
		testStep = new CSmsTestFormattingSegmentation;
		}
	else if (aStepName == _L("TestNumberOfMessages"))
		{
		testStep = new CSmsTestNumberOfMessages;
		}
	else if (aStepName == _L("TestCreateSocket"))
		{
		testStep = new CSmsTestCreateSocket;
		}
	else if (aStepName == _L("PublicMethods"))
		{
		testStep = new CSmsPublicMethods;
		}
	else if (aStepName == _L("SendAndReceive7bitMsgs"))
		{
		testStep = new CSmsSendAndReceive7bitMsgs;
		}
	else if (aStepName == _L("SendAndReceive8bitMsgs"))
		{
		testStep = new CSmsSendAndReceive8bitMsgs;
		}
	else if (aStepName == _L("SendAndReceive16bitMsgs"))
		{
		testStep = new CSmsSendAndReceive16bitMsgs;
		}
	else if (aStepName == _L("SendAndReceiveConcatenatedMsgs"))
		{
		testStep = new CSmsSendAndReceiveConcatenatedMsgs;
		}
	else if (aStepName == _L("TestStringMatchingIncomingMsg"))
		{
		testStep = new CSmsTestStringMatchingIncomingMsg;
		}
	else if (aStepName == _L("SendAndReceiveDifferentIEIs"))
		{
		testStep = new CSmsSendAndReceiveDifferentIEIs;
		}
	else if (aStepName == _L("SendAndReceiveBinaryData"))
		{
		testStep = new CSmsSendAndReceiveBinaryData;
		}
	else if (aStepName == _L("TestOOM"))
		{
		testStep = new CSmsTestOOM;
		}
	else if (aStepName == _L("TestSendAndReceive1byteMsg"))
		{
		testStep = new CSmsTestSendAndReceive1byteMsg;
		}
	else if (aStepName == _L("TestSmsWapEmailNotification"))
		{
		testStep = new CSmsTestSmsWapEmailNotification;
		}
	else if (aStepName == _L("TestSendAndReceive159bytesMsg"))
		{
		testStep = new CSmsTestSendAndReceive159bytesMsg;
		}
	else if (aStepName == _L("TestSocketConnectTwice"))
		{
		testStep = new CSmsTestSocketConnectTwice;
		}
	else if (aStepName == _L("TestSocketBinding"))
		{
		testStep = new CSmsTestSocketBinding;
		}
	else if (aStepName == _L("TestEnumerateAndReceiveMessages"))
		{
		testStep = new CSmsTestEnumerateAndReceiveMessages;
		}
	else if (aStepName == _L("TestEnumerateAndDeleteMessages"))
		{
		testStep = new CSmsTestEnumerateAndDeleteMessages;
		}
	else if (aStepName == _L("EnumerateAndWriteAndDeleteMessages"))
		{
		testStep = new CSmsEnumerateAndWriteAndDeleteMessages;
		}
	else if (aStepName == _L("PDUSlotStoreWithEnumeration"))
		{
		testStep = new CSmsPDUSlotStoreWithEnumeration;
		}
	else if (aStepName == _L("DeleteMessagesWithoutEnumeration"))
		{
		testStep = new CSmsDeleteMessagesWithoutEnumeration;
		}
	else if (aStepName == _L("SmartMessaging"))
		{
		testStep = new CSmsSmartMessaging;
		}
	else if (aStepName == _L("RetrieveAndStoreSmsParams"))
		{
		testStep = new CSmsRetrieveAndStoreSmsParams;
		}
	else if (aStepName == _L("TestRetrieveAndStoreSmsParamsOOM"))
		{
		testStep = new CSmsTestRetrieveAndStoreSmsParamsOOM;
		}
	else if (aStepName == _L("SmsStoreMsgOfDifferentClasses"))
		{
		testStep = new CSmsSmsStoreMsgOfDifferentClasses;
		}
	else if (aStepName == _L("DuplicatePdusOnStore"))
		{
		testStep = new CSmsDuplicatePdusOnStore;
		}
	else if (aStepName == _L("TestMaxNumberOfPdusOnStore"))
		{
		testStep = new CSmsTestMaxNumberOfPdusOnStore;
		}
	else if (aStepName == _L("DeletingMessageInEmptyStore"))
		{
		testStep = new CSmsDeletingMessageInEmptyStore;
		}
	else if (aStepName == _L("ConcatenatedMessages"))
		{
		testStep = new CSmsConcatenatedMessages;
		}
	else if (aStepName == _L("OutOfMemoryWrite"))
		{
		testStep = new CSmsOutOfMemoryWrite;
		}
	else if (aStepName == _L("TestOutOfMemoryRead"))
		{
		testStep = new CSmsTestOutOfMemoryRead;
		}
	else if (aStepName == _L("OutOfMemoryDelete"))
		{
		testStep = new CSmsOutOfMemoryDelete;
		}
	else if (aStepName == _L("SendSMS2Port0000"))
		{
		testStep = new CSmsSendSMS2Port0000;
		}
	else if (aStepName == _L("TestReceiveLongClass2"))
		{
		testStep = new CSmsTestReceiveLongClass2;
		}
	else if (aStepName == _L("TestSendAndReceiveConcatenated1"))
		{
		testStep = new CSmsTestSendAndReceiveConcatenated1;
		}
	else if (aStepName == _L("TestSendAndReceiveConcatenated2"))
		{
		testStep = new CSmsTestSendAndReceiveConcatenated2;
		}
	else if (aStepName == _L("TestBindingToApplicationPort"))
		{
		testStep = new CSmsTestBindingToApplicationPort;
		}
	else if (aStepName == _L("TestSendAndReceiveAndDeleteClass2Msgs"))
		{
		testStep = new CSmsTestSendAndReceiveAndDeleteClass2Msgs;
		}
	else if (aStepName == _L("PublishandSubscribeTest"))
		{
		testStep = new CSmsPublishandSubscribeTest;
		}
	else if (aStepName == _L("TestSendingReceivingViaAppPortPublishAndSubscribe"))
		{
		testStep = new CSmsTestSendingReceivingViaAppPortPublishAndSubscribe;
		}
	
	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}
	
	return testStep;
	} // CSmsInterTestServer::CreateTestStep
