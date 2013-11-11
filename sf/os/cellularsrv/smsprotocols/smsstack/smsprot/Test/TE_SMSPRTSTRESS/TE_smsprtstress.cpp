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

#include "TE_smsprtstress.h"

#include <e32property.h>
//#include <connect/sbdefs.h>
#include <sacls.h>

#include "TE_smsprtstressutils.h"

//using namespace conn;

TVerdict CTestRxAndRead::doTestStepL()
	{
//	CTestReceiveEnumerateMessages* globalAO = CTestReceiveEnumerateMessages::NewL(KActiveReceiverPriority, iSocketServer, *iSmsStackTestUtils);
//    CleanupStack::PushL(globalAO);
//
//	globalAO->Start(2 /*aEnumExpected*/, 5 /*aRecvExpected*/, 3 /*aRecvInitial*/);
//
//	TEST(globalAO->iStatus.Int() == KErrNone);
//
//   	CleanupStack::PopAndDestroy(globalAO);
	return TestStepResult();
	}

TVerdict CTestMultipleSimultaneousRequests::doTestStepL()
	{
//	INFO_PRINTF1(_L("Test multiple simultaneous requests handling"));
//	TInt ret(KErrNone);
//
//	RPointerArray<CTestSmsActiveBase> testSmsActives;
//	CleanupResetAndDestroyPushL(testSmsActives);
//
//	//Create instance of CTestSmsActiveStop that stops testing activity when tests are completed.
//	CTestSmsActiveStop* testSmsActiveStop = new (ELeave) CTestSmsActiveStop(testSmsActives,ret);
//	CleanupStack::PushL(testSmsActiveStop);
//
//	//Create instance of CTestSmsSendActive that makes send requests to the SMS Stack.
//	CTestSmsSendActive* sendActive = CTestSmsSendActive::NewL(iSocketServer,*testSmsActiveStop, 16, iFs, iSmsStackTestUtils, this);
//	CleanupStack::PushL(sendActive);
//	User::LeaveIfError(testSmsActives.Append(sendActive));
//	CleanupStack::Pop(sendActive);
//
//	//Create instance of CTestSmsParametersActive that makes retrieve and store parameters requests to the SMS Stack.
//	CTestSmsParametersActive* parametersActive = CTestSmsParametersActive::NewL(iSocketServer,*testSmsActiveStop,70, iSmsStackTestUtils, this);
//	CleanupStack::PushL(parametersActive);
//	User::LeaveIfError(testSmsActives.Append(parametersActive));
//	CleanupStack::Pop(parametersActive);
//
//	//Create instance of CTestSmsStorageActive that makes enumerate, write and delete requests to the SMS Stack.
//	CTestSmsStorageActive* storageActive = CTestSmsStorageActive::NewL(iSocketServer,*testSmsActiveStop,70, iFs,iSmsStackTestUtils, this);
//	CleanupStack::PushL(storageActive);
//	User::LeaveIfError(testSmsActives.Append(storageActive));
//	CleanupStack::Pop(storageActive);
//
//	//Create instance of CTestSmsReceiveActive that makes retrieve requests to the SMS Stack.
//	CTestSmsReceiveActive* receiveActive = CTestSmsReceiveActive::NewL(iSocketServer,*testSmsActiveStop,16, iFs, iSmsStackTestUtils, this);
//	CleanupStack::PushL(receiveActive);
//	User::LeaveIfError(testSmsActives.Append(receiveActive));
//	CleanupStack::Pop(receiveActive);
//
//	sendActive->StartL();
//	parametersActive->StartL();
//	storageActive->StartL();
//	receiveActive->StartL();
//
//	//Start active objects
//	CActiveScheduler::Start();
//
//	//Checkpoint is called when test are ran through
//	TEST(ret == KErrNone);
//
//	CleanupStack::PopAndDestroy(testSmsActiveStop);	//stopper class
//	CleanupStack::PopAndDestroy(&testSmsActives);	//active objects
	return TestStepResult();
	}

// TODO: replace with one from CSmsBaseTestStep
TInt CTestSmsPrtBackup::SendSmsL(RSocket& aSocket)
	{
	INFO_PRINTF1(_L("Attempting to send an sms to +358405202000"));
	TBuf<32> buf;
	buf.Append(_L("TestSmsPrtBackup message"));

	CSmsMessage* message = iSmsStackTestUtils->CreateSmsMessageLC(CSmsPDU::ESmsSubmit, _L("+358405202000"), buf);

	RSmsSocketWriteStream writestream(aSocket);
	writestream << *message;
	writestream.CommitL();

 	//Send message
	TPckgBuf<TUint> sbuf;
	TRequestStatus status;
	aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
    INFO_PRINTF2(_L("SendSmsL returning %d"), status.Int());
    
	CleanupStack::PopAndDestroy(message);
	return (status.Int());
	}

/* Test sending sms messages under backup and restore situations
	1 - Sending of an sms message under normal circumstances succeeds
	2 - Sending of an sms message fails when a backup is in progress
	3 - Sending of an sms message succeeds when a backup is complete
	4 - Sending of an sms message fails when a restore is in progress
	5 - Sending of an sms message succeeds when a restore is complete
	The test case is intended to validate the behaviour of the SMS Stack whilst a
	backup and restore is in progress, rather than testing the behaviour of the
	server during backup and restore. A test precondition is that the client
	already has a socket open prior to a backup and restore commencing,
	thus avoiding the errors that can be generated when opening the socket
	during backup and restore.*/
TVerdict CTestSmsPrtBackup::doTestStepL()
	{
//	RSocket socket;
//	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrSendOnly);
//
//	INFO_PRINTF1(_L("Expecting SendSmsL to return KErrNone"));
//	TEST(SendSmsL(socket) == KErrNone);
//
//	// Notify the SMSPROT server that a backup is about to take place and
//	// that the server should powerdown
//	INFO_PRINTF1(_L("Simulating a backup notification"));
//	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
//									 EBURBackupFull, EBackupBase);
//
//	INFO_PRINTF1(_L("Expecting SendSmsL to return KErrDisconnected"));
//	TEST(SendSmsL(socket) == KErrDisconnected);
//
//
//	// Notify the SMSPROT server that a backup has completed
//	// that the server should powerup
//	INFO_PRINTF1(_L("Simulating a backup complete notification"));
//	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
//									 EBURNormal, ENoBackup);
//
//	// Required Pause to Allow SMSStack to hande notification and restart
//	User::After(5*1000*1000);
//
//	INFO_PRINTF1(_L("Expecting SendSmsL to return KErrNone"));
//	TEST(SendSmsL(socket) == KErrNone);
//
//	// Notify the SMSPROT server that a restore is about to take place and
//	// that the server should powerdown
//	INFO_PRINTF1(_L("Simulating a restore notification"));
//	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
//									 EBURRestorePartial, EBackupIncrement);
//
//	INFO_PRINTF1(_L("Expecting SendSmsL to return KErrDisconnected"));
//	TEST(SendSmsL(socket) == KErrDisconnected);
//
//	// Notify the SMSPROT server that a restore has completed
//	// that the server should powerup
//	INFO_PRINTF1(_L("Simulating a restore complete notification"));
//	
//	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
//									 EBURNormal, ENoBackup);
//
//	// Required Pause to Allow SMSStack to hande notification and restart
//	User::After(5*1000*1000);
//	INFO_PRINTF1(_L("Expecting SendSmsL to return KErrNone"));
//	TEST(SendSmsL(socket) == KErrNone);
//
//	INFO_PRINTF1(_L("Closing SocketServer ..."));
//	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/**
 *  Tests the Backup and Restore operation when the phone is powered off.
 */
TVerdict CTestSmsPrtBackupWhenPhoneIsOff::doTestStepL()
	{
//	RSocket socket;
//	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrSendOnly);
//
//	//
//	// Perform backup abnd restore when the phone is powered down...
//	//
//	INFO_PRINTF1(_L("Simulating a backup and restore while the phone is off"));
//	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid,
//									  ESAPhoneOff));
//	
//	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
//									 EBURBackupFull, EBackupBase);
//
//	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
//									 EBURNormal, ENoBackup);
//
//	User::After(5*1000*1000);
//
//	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid,
//									  ESAPhoneOn));
//
//	INFO_PRINTF1(_L("Closing SocketServer..."));
//	CleanupStack::PopAndDestroy(&socket);
//	iSocketServer.Close();
//
//	// TODO: check if this part is still valid
//	//
//	// Ensure the SMS Stack has been reloaded, prior to performing the ESock
//	// memory leak check.
//	//
//	ConnectSocketServerL(iSocketServer);
//	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrSendOnly);
//
//	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}
