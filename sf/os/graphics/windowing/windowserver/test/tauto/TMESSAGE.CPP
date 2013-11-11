// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "TMESSAGE.H"

const TInt KTotalNumberOfTestMessages=5;
const TInt KMaxNumOfMessagesInActiveQueue=21;
const TInt KMaxNumOfSyncMessages=25;
const TInt KNumOfTestCases=7;
TInt MsgCountCheck=0;
TInt SyncSendReceiveCount=0;
TUid UidAtFirstLocation;
TUid UidAtLastLocation;
TBool TestNeedsExtraCheck=EFalse;

CTMessage::CTMessage(CTestStep* aStep) : CTWsGraphicsBase(aStep), iSyncSendReceiveFlag(EFalse), iCounter(-1), iState(0)
	{
	}

TInt CTMessage::CreateAndSendMessages(TInt aCount,TInt aNoOfMessages)
	{
	CMessageReceiver* msgReceiver=(*iMessageReceiver)[iState];
	TInt identifer=msgReceiver->GroupWin()->Identifier();
	TInt length=aCount*2;
	TUint8 modificationFactor=(TUint8 )(aCount*3+1);
	TUint8 msgValue=0;
	if(!iSyncSendReceiveFlag && TestNeedsExtraCheck)
		{
		if(aCount==0)
			{
			UidAtFirstLocation=TUid::Uid(length+(modificationFactor<<24));
			}
		else if(aCount==(aNoOfMessages-1))
			{
			UidAtLastLocation=TUid::Uid(length+(modificationFactor<<24));
			}
		}
	iMsg->Des().Zero();
	for(TInt index=0;index<length;index++)
		{
		msgValue=(TUint8)(msgValue+modificationFactor);
		iMsg->Des().Append(msgValue);
		}
	return TheClient->iWs.SendMessageToWindowGroup(identifer, TUid::Uid(length+(modificationFactor<<24)), *iMsg);
	}

TInt CTMessage::CallbackLowPriority(TAny* aMessageTest)
	{
	return static_cast<CTMessage*>(aMessageTest)->SendSyncMessages();
	}

TInt CTMessage::SendSyncMessages()
	{
	if(iSyncSendReceiveFlag && iCounter++<SyncSendReceiveCount)
		{
		TEST(CreateAndSendMessages(iCounter,SyncSendReceiveCount)==KErrNone);
		return ETrue;
		}
	TheClient->iWs.NumWindowGroups();		//Make sure all asyncronus calls have been processed
	CTEventBase& eventHandler=*TheClient->EventHandler();
	if (eventHandler.iStatus.Int()==KRequestPending)
		CActiveScheduler::Stop();
	else
		return ETrue;
	return EFalse;
	}

CTMessage::~CTMessage()
	{
	iMessageReceiver->ResetAndDestroy();
	iMessageReceiver->Close();
	delete iMessageReceiver;
 	iLowPriorityObject->Cancel();
	delete iLowPriorityObject;
	iLowPriorityObject=NULL;
	delete iMsg;
	}

void CTMessage::ConstructL()
	{
	iLowPriorityObject= new(ELeave) CLowPriorityIdle();
	iMessageReceiver=new(ELeave) RPointerArray<CMessageReceiver>(KNumOfTestCases);
	iMsg=HBufC8::NewL(KMaxNumOfSyncMessages*2);
	for(TInt index=0;index<KNumOfTestCases;index++)
		{
		CMessageReceiver* messageReceiver=new(ELeave) CMessageReceiver(TheClient, this);
		messageReceiver->ConstructL();
		iMessageReceiver->AppendL(messageReceiver);
		}
	}


CLowPriorityIdle::CLowPriorityIdle() : CIdle(EPriorityIdle)
	{
	CActiveScheduler::Add(this);
	}

TInt CLowPriorityIdle::RunError(TInt /*aError*/)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}

LOCAL_C TInt DoPanicTest(TInt aInt, TAny *)
	{
	switch(aInt)
		{
		case 1:
			User::Panic(KWSERV,EWservPanicGcActive);
			break;
		}
	return(EWsExitReasonBad);
	}

CMessageReceiver::CMessageReceiver(CTClient *aClient, CTMessage *aTMessage) : CTWindowGroup(aClient), iMsgCount(0)
	{
	iTMessage = aTMessage;
	}

#define MESSAGES_READ 1		//Change to 2 to test the 16-bit functions too.
void CMessageReceiver::MessageReady(const TWsEvent& aEvent)
	{
	iTMessage->OnMessageReceived(aEvent, iMsgCount);
	}

void CTMessage::OnMessageReceived(const TWsEvent& aEvent, TInt& aMsgCount)
	{
	aMsgCount++;
	if(aMsgCount>MsgCountCheck)
		{
		return;
		}
	if (MESSAGES_READ*(aMsgCount/MESSAGES_READ)<aMsgCount)
		{
		return;
		}
	TPtr8 msgPtr(NULL,0);
	//TPtr16 msgPtr2(NULL,0);
	TUid uid;
	TInt retValue=TheClient->iWs.FetchMessage(uid, msgPtr, aEvent);
	TEST(retValue==KErrNone);
	//GroupWin()->FetchMessage(uid, msgPtr2);
	if(TestNeedsExtraCheck)
		{
		TEST(uid==(aMsgCount==1?UidAtFirstLocation:UidAtLastLocation));
		}
	TInt msgLength=(uid.iUid)&0xFFFFFF;
	TUint8 uidFactor=(TUint8)((uid.iUid&0xFF000000)>>24);
	TBool retVal = msgPtr.Length()==msgLength;
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("msgPtr.Length()==msgLength  - Expected: %d, Actual: %d"), msgLength, msgPtr.Length());

	TUint8 check=0;
	for(TInt index=0;index<msgLength;index++)
		{
		check=(TUint8)(check+uidFactor);
		retVal = msgPtr[index]==check;
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("msgPtr[index]==check  - Expected: %d, Actual: %d"), check, msgPtr[index]);

		}
	/*CTestBase::Test(msgPtr.Length()/2==msgPtr2.Length());
	TPtrC8 data2((const TUint8*) msgPtr2.Ptr(),msgPtr2.Size());
	CTestBase::Test(data2==msgPtr.Left(msgPtr2.Size()));*/
	User::Free((TAny *)msgPtr.Ptr());
	//User::Free((TAny *)msgPtr2.Ptr());
	}

void CTMessage::TestPanicsL()
	{	
	TEST(iTest->TestWsPanicL(&DoPanicTest,EWservPanicGcActive,1));
	iTest->CloseAllPanicWindows();
	}

void CTMessage::MessageTests(TInt aNoOfMessages,TInt aMsgCountCheck,TBool aSyncSendReceiveFlag,TBool aTestNeedsExtraCheck)
	{
	iSyncSendReceiveFlag=aSyncSendReceiveFlag;
	TestNeedsExtraCheck=aTestNeedsExtraCheck;
	MsgCountCheck=aMsgCountCheck;
	iLowPriorityObject->Start(TCallBack(CallbackLowPriority,this));

	if(iSyncSendReceiveFlag)
		{
		//Messages will be sent and received one by one using the function SendSyncMessages().
		CActiveScheduler::Start();
		}
	else
		{
		for(TInt count=0;count<aNoOfMessages;count++)
			{
			TInt err=CreateAndSendMessages(count,aNoOfMessages);
			TEST(err==KErrNone);
			if (err!=KErrNone)
				{
				_LIT(KLog,"Error %d when trying to send Messages");
				LOG_MESSAGE2(KLog,err);
				}
			}
		CActiveScheduler::Start();
		TInt msgCount=(*iMessageReceiver)[iState]->MessageCount();
		TEST(msgCount==MsgCountCheck);
		if (msgCount!=MsgCountCheck)
			{
			_LIT(KLog,"Number of messages recieved=%d expected=%d");
			LOG_MESSAGE3(KLog,msgCount,MsgCountCheck);
			}
		}
	}

/**
 Test delivery group messages in case when event queue is overflow
 */
void CTMessage::TestMessageQueueOverflowL()
	{
	const TInt numMaxMessages = 60;
	TInt numWasteMessages = 0;
	TInt numWgMessages = 5;

	TInt gotRealWasteMessages = 0;
	TInt gotRealWgMessages = 0;

	const TInt oneSecond = 1000000;
	const TInt allPossibleMessages = 500;

	RWsSession senderWsSession;
	RWindowGroup senderGroupWin;
	TInt sndrHanGrpWin = 7777;

	RWsSession receiverWsSession;
	RWindowGroup receiverGroupWin;
	TInt rcvrHanGrpWin = 8888;

	TRequestStatus testStatus;
	TRequestStatus timerStatus;
	TWsEvent wasteEvent;
	TWsEvent event;

	RTimer timer;
	timer.CreateLocal();
	CleanupClosePushL(timer);

	// Create sender WsSession and the window group
	User::LeaveIfError(senderWsSession.Connect());
	CleanupClosePushL(senderWsSession);

	senderGroupWin = RWindowGroup(senderWsSession);
	User::LeaveIfError(senderGroupWin.Construct(sndrHanGrpWin));
	CleanupClosePushL(senderGroupWin);

	// Create reciever WsSession and the window group
	User::LeaveIfError(receiverWsSession.Connect());
	CleanupClosePushL(receiverWsSession);

	receiverGroupWin = RWindowGroup(receiverWsSession);
	User::LeaveIfError(receiverGroupWin.Construct(rcvrHanGrpWin));
	CleanupClosePushL(receiverGroupWin);

	TInt err = KErrNone;

	// Send waste events to the receiver and overflow the event queue
	for (TInt ind = 0; ind < numMaxMessages; ind++)
		{
		wasteEvent.SetType(sndrHanGrpWin);
		err = senderWsSession.SendEventToWindowGroup(receiverGroupWin.Identifier(), wasteEvent);
		if (err != KErrNone)
			{
			numWasteMessages = ind; // Real number waste messages that was sent
			break;
			}
		}

	// Send messages to the receiver
	for (TInt ind = 0; ind < numWgMessages; ind++)
		{
		TInt uidData = ind + sndrHanGrpWin;
		TPtr8 msg((unsigned char*) &uidData, sizeof(uidData));
		err = senderWsSession.SendMessageToWindowGroup(receiverGroupWin.Identifier(), TUid::Uid(uidData), msg);
		if (err != KErrNone)
			{
			LOG_MESSAGE2(_L("UnExpected Error Code = %d"),err);
			numWgMessages = ind; // Real number window group messages that was sent
			}
		}

	// Check and count sent messages 
	for (TInt ind = 0; ind < allPossibleMessages; ind++)
		{
		receiverWsSession.EventReady(&testStatus);
		timer.After(timerStatus, oneSecond);
		User::WaitForRequest(testStatus, timerStatus);
		if (testStatus == 0)
			{
			// Test incoming events
			receiverWsSession.GetEvent(event);
			if (event.Type() == sndrHanGrpWin)
				{
				++gotRealWasteMessages;
				}
			else if (event.Type() == EEventMessageReady)
				{
				if (gotRealWgMessages == 0)
					{
					for (TInt ind = 0; ind < numMaxMessages; ind++)
						{
						wasteEvent.SetType(sndrHanGrpWin);
						err = senderWsSession.SendEventToWindowGroup(receiverGroupWin.Identifier(), wasteEvent);
						if (err == KErrNone)
							{
							++numWasteMessages;
							}
						else
							{
							break;
							}
						}
					}
				++gotRealWgMessages;
				TUid uid;
				TPtr8 msgPtr(NULL,0);
				err = receiverWsSession.FetchMessage(uid, msgPtr, event);
				User::Free((TAny *) msgPtr.Ptr());
				}
			// testStatus has been completed. Hence, Cancel the timer.
			timer.Cancel();
			User::WaitForRequest(timerStatus);
			}
		else
			{
			// Times out, cancel the event notification
			receiverWsSession.EventReadyCancel();
			User::WaitForRequest(testStatus);
			// All events were recieved
			break;
			}
		}

	LOG_MESSAGE3(_L("Got Waste message = %d expected =%d"),gotRealWasteMessages,numWasteMessages);
	LOG_MESSAGE3(_L("Got Group message = %d expected =%d"),gotRealWgMessages,numWgMessages);
	TEST(gotRealWasteMessages == numWasteMessages);
	TEST(gotRealWgMessages == numWgMessages);

	CleanupStack::PopAndDestroy(&receiverGroupWin);
	CleanupStack::PopAndDestroy(&receiverWsSession);
	CleanupStack::PopAndDestroy(&senderGroupWin);
	CleanupStack::PopAndDestroy(&senderWsSession);
	CleanupStack::PopAndDestroy(&timer);

	TheClient->iWs.Flush();

	}

void CTMessage::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KMsgTest0,"Message test 1");
	_LIT(KMsgTest1,"Message test 2");
	_LIT(KMsgTest2,"Message test 3");
	_LIT(KMsgTest3,"Message test 4");
	_LIT(KMsgTest4,"Message test 5");
	_LIT(KMsgTest5,"Message test 6");
	_LIT(KMsgTest6,"Message test 7");
	_LIT(KMsgTest7,"Message test 8 - when event queue is overflow");

	iState=iTest->iState; // used by iMessageReceiver array 
	((CTMessageStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0084

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test message sending and receiving

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Generates 5 short messages and sends them to window group, window group receives them

@SYMTestExpectedResults Expects that received messages have the same length and the same content as those sent
*/		
		case 1:
			((CTMessageStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0084"));
			iTest->LogSubTest(KMsgTest0);
			MessageTests(KTotalNumberOfTestMessages,KTotalNumberOfTestMessages,EFalse,EFalse);
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0406

@SYMDEF             DEF102201,DEF141447

@SYMTestCaseDesc    Test message sending and receiving

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Generates some diffrent size of messages and sends them to window group, window group receives them

@SYMTestExpectedResults Expects that received messages have the same length and the same content as those sent
*/		
		case 2:
			((CTMessageStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0406"));
			iTest->LogSubTest(KMsgTest1);
			MessageTests(KMaxNumOfMessagesInActiveQueue-1,KMaxNumOfMessagesInActiveQueue-1,EFalse,EFalse);
			break;
		case 3:
			((CTMessageStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0406"));
			iTest->LogSubTest(KMsgTest2);
			MessageTests(KMaxNumOfMessagesInActiveQueue,2,EFalse,ETrue);
			break;
		case 4:
			((CTMessageStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0406"));
			iTest->LogSubTest(KMsgTest3);
			MessageTests(KMaxNumOfMessagesInActiveQueue+1,2,EFalse,ETrue);
			break;
		case 5:
			((CTMessageStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0406"));
			iTest->LogSubTest(KMsgTest4);
			MessageTests(KMaxNumOfMessagesInActiveQueue+2,2,EFalse,ETrue);
			break;
		case 6:
			((CTMessageStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0406"));
			iTest->LogSubTest(KMsgTest5);
			SyncSendReceiveCount=KMaxNumOfSyncMessages;
			MessageTests(KMaxNumOfSyncMessages,KMaxNumOfSyncMessages,ETrue,EFalse);
			break;
		case 7:
			((CTMessageStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0406"));
			iTest->LogSubTest(KMsgTest6);
			MessageTests(KMaxNumOfMessagesInActiveQueue+1,2,EFalse,ETrue);
			((*iMessageReceiver)[iState])->ResetMessageCount();
			SyncSendReceiveCount=KTotalNumberOfTestMessages;
			MessageTests(KTotalNumberOfTestMessages,KTotalNumberOfTestMessages,ETrue,EFalse);
			break;
		case 8:
			((CTMessageStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0406"));
			iTest->LogSubTest(KMsgTest7);
			TestMessageQueueOverflowL();
			((CTMessageStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTMessageStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			return;
		}
	((*iMessageReceiver)[iState])->ResetMessageCount();
	((CTMessageStep*)iStep)->RecordTestResultL();
	}
	

__WS_CONSTRUCT_STEP__(Message)
