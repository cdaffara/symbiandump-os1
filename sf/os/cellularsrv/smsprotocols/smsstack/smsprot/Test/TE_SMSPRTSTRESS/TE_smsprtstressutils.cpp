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
// *AOs***
// 
//

/**
 @file
*/

#include "TE_smsprtstressutils.h"

/*
-------------------------------------------------------------------------------

	CTestSmsActiveBase

	Base class for test active objects

-------------------------------------------------------------------------------
*/
CTestSmsActiveBase::CTestSmsActiveBase(TInt aHowManyRequests):
/**
 *  Constructor
 *  
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	CActive(0),
	iHowManyRequests(aHowManyRequests)
	{
	CActiveScheduler::Add(this);
	}

/*
-------------------------------------------------------------------------------

	CTestSmsActiveStop

	"Stopper" class for test active objects. This class stops all testing
	activity if tests are ran through or if an error is occurred.

-------------------------------------------------------------------------------
*/

CTestSmsActiveStop::CTestSmsActiveStop(RPointerArray<CTestSmsActiveBase>& aTestSmsActives,TInt& aRet):
/**
 *  Constructor
 *  
 *  @param aTestSmsActives a reference to the active objects
 *  @param aRet a reference to cause value
 */
	iTestSmsActives(aTestSmsActives),
	iRet(aRet)
	{
	}

void CTestSmsActiveStop::StopTests(TInt aStatus)
/**
 *  Stop all tests by calling active objects cancel. After that stops active scheduler.
 *  
 *  @param aTestSmsActives a reference to the active objects
 *  @param aRet a reference to cause value
 */
	{
	TInt count = iTestSmsActives.Count();
	// Set error value
	iRet = aStatus;
	while(count--)
		iTestSmsActives[count]->Cancel();
	User::After(4000000); //Wait a couple of seconds before stopping active scheduler
	CActiveScheduler::Stop();
	}

/*
-------------------------------------------------------------------------------

	CTestSmsSendActive

	This class makes send SMS requests to the SMS Stack.

-------------------------------------------------------------------------------
*/

CTestSmsSendActive* CTestSmsSendActive::NewL(RSocketServ& aSocketServer,CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CSmsStackTestUtils* aSmsStackUtils, CTestStep* aTestStep)
/**
 *  2 Phase constructor. Open the socket that is used for sending messages.
 *  Create also instance of CSmsMessage class that will be used while running send tests.
 *  
 *  @leave Leaves if not enough memory is available.
 *  @leave Leaves if OpenSmsSocketL() or CreateSmsWithStatusReportReqL() leaves.
 *  @param aSocketServer reference to the socket server object
 *  @param aTestSmsActiveStop a reference to the "stopper" object
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	{
	CTestSmsSendActive* sendActive = new (ELeave) CTestSmsSendActive(aTestSmsActiveStop,aHowManyRequests, aTestStep);
	CleanupStack::PushL(sendActive);

	aSmsStackUtils->OpenSmsSocketL(aSocketServer,sendActive->iSocket,ESmsAddrSendOnly);

	//Create test message
	_LIT(KTestMsg1,"3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
3 PDU test SMS message. \
The End.");
	//Set destination and SC numbers
	_LIT(KVodafoneSC,"+447785016005");
	_LIT(KRegTestNumber,"+447747065548");
	sendActive->iTelephoneNumber=KRegTestNumber;
	sendActive->iServiceCenterNumber=KVodafoneSC;
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	sendActive->CreateSmsWithStatusReportReqL(KTestMsg1,alphabet, aFs);

	CleanupStack::Pop(sendActive);
	return sendActive;
	}

void CTestSmsSendActive::CreateSmsWithStatusReportReqL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RFs& aFs)
	{
	/**
	 *  Create a uninitialised SMS message with Status Report request
	 *  @param aDes contains text that will be inserted to the pdu
	 *  @param aAlphabet describes the alphabet of the pdu
	 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
	 */
	CSmsMessage* smsMessage=CreateSmsMessageL(aDes, aAlphabet, aFs);

	CleanupStack::PushL(smsMessage);

	//Set Status report request
	CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
	submitPdu.SetStatusReportRequest(ETrue);

	CleanupStack::Pop(smsMessage);
	iSmsMessage = smsMessage;
	}


CSmsMessage* CTestSmsSendActive::CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RFs& aFs, CSmsPDU::TSmsPDUType aType)
/**
 *  Create a uninitialised SMS message
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(aFs, aType, buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(aAlphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);

	smsMessage->SetToFromAddressL(iTelephoneNumber);
	smsMessage->SmsPDU().SetServiceCenterAddressL(iServiceCenterNumber);
	buffer->InsertL(0,aDes);
	CleanupStack::Pop(smsMessage);
	return smsMessage;
	}


CTestSmsSendActive::CTestSmsSendActive (CTestSmsActiveStop& aTestSmsActiveStop, TInt aHowManyRequests, CTestStep* aTestStep):
/**
 *  Constructor
 *  
 *  @param aTestSmsActiveStop a reference to the "stopper" object
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	CTestSmsActiveBase(aHowManyRequests),
	iTestSmsActiveStop(aTestSmsActiveStop),
	iTestStep(aTestStep)
	{
	}

CTestSmsSendActive::~CTestSmsSendActive()
/**
 *  Destructor.
 *  
 *  Cancels sending process and deletes CSmsMessage object.
 */
	{
	Cancel();
	delete iSmsMessage;
	}

void CTestSmsSendActive::DoCancel()
/**
 *  Cancels send SMS process.
 */
	{
	iSocket.CancelIoctl();
	}

void CTestSmsSendActive::StartL()
/**
 *  Start making send requests to the SMS Stack.
 *  
 *  @leave Leaves if SendMessageL() leaves.
 */
	{
	SendMessageL();
	}

void CTestSmsSendActive::RunL()
/**
 *  Make next send request to the SMS Stack. Stop tests if there was an error or
 *  if iHowManyRequests limit was achieved.
 *  
 *  @leave Leaves if SendMessageL() leaves.
 */
	{
	iTestStep->INFO_PRINTF2(_L("CTestSmsSendActive::RunL iStatus = %d"), iStatus.Int());
	iCounter++;
	if(iCounter<iHowManyRequests && iStatus.Int()==KErrNone)
		{
		SendMessageL();
		}
	else
		iTestSmsActiveStop.StopTests(iStatus.Int());
	}

TInt CTestSmsSendActive::RunError(TInt aError)
/**
 *  Handle possible error situation by stopping all tests.
 */
	{
	iTestStep->INFO_PRINTF2(_L("CTestSmsSendActive::RunError aError = %d"),aError);
	iTestSmsActiveStop.StopTests(aError);
	return KErrNone;
	}

void CTestSmsSendActive::SendMessageL()
/**
 *  Write message to the stream and make send request to the SMS Stack.
 *  
 *  @leave Leaves if CommitL() leaves or if not enough memory available.
 */
	{
	//INFO_PRINTF1(_L("Send message"));
	RSmsSocketWriteStream writestream(iSocket);
	writestream << *iSmsMessage;
	writestream.CommitL();

	iSocket.Ioctl(KIoctlSendSmsMessage,iStatus,&iSbuf,KSolSmsProv);
	SetActive();
	}

/*
-------------------------------------------------------------------------------

	CTestSmsParametersActive

	This class makes retrieve and store parameters requests to the
	SMS Stack. It also get retrieved parameter list from stream and
	compare it to the stored list.

-------------------------------------------------------------------------------
*/
CTestSmsParametersActive* CTestSmsParametersActive::NewL(RSocketServ& aSocketServer,CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, CSmsStackTestUtils* aSmsStackTestUtils, CTestStep* aTestStep)
/**
 *  2 Phase constructor. Open the socket that is used for storing and retrieving parameters.
 *  Create instance of CMobilePhoneSmspList class that will be used while running parameter tests.
 *  
 *  @leave Leaves if not enough memory is available.
 *  @leave Leaves if CMobilePhoneSmspList::NewL() or AddEntryL() leaves.
 *  @param aSocketServer reference to the socket server object
 *  @param aTestSmsActiveStop a reference to the "stopper" object
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	{
	CTestSmsParametersActive* parametersActive = new (ELeave) CTestSmsParametersActive(aTestSmsActiveStop,aHowManyRequests, aSmsStackTestUtils, aTestStep);
	CleanupStack::PushL(parametersActive);

	aSmsStackTestUtils->OpenSmsSocketL(aSocketServer,parametersActive->iSocket,ESmsAddrLocalOperation);

	parametersActive->iStoreSmspList=CMobilePhoneSmspList::NewL();

	RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy;
	_LIT(KSmspSetName,"SMSP_SET_1");
	entryToTsy.iText = KSmspSetName;
	entryToTsy.iValidParams = RMobileSmsMessaging::KDcsIncluded;
	entryToTsy.iDcs = 0;
	parametersActive->iStoreSmspList->AddEntryL(entryToTsy);

	CleanupStack::Pop(parametersActive);
	return parametersActive;
	}

CTestSmsParametersActive::CTestSmsParametersActive(CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, CSmsStackTestUtils* aSmsStackTestUtils, CTestStep* aTestStep):
/**
 *  Constructor
 *  
 *  @param aTestSmsActiveStop a reference to the "stopper" object
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	CTestSmsActiveBase(aHowManyRequests),
	iTestSmsActiveStop(aTestSmsActiveStop),
	iSmsStackTestUtils(aSmsStackTestUtils),
	iTestStep(aTestStep)
	{
	}

CTestSmsParametersActive::~CTestSmsParametersActive()
/**
 *  Destructor.
 *  
 *  Cancels parameter store handling process and deletes CMobilePhoneSmspList object.
 */
	{
	Cancel();
	delete iStoreSmspList;
	}

void CTestSmsParametersActive::DoCancel()
/**
 *  Cancels parameter store handling process.
 */
	{
	iSocket.CancelIoctl();
	}

void CTestSmsParametersActive::StartL()
/**
 *  Make store parameters request to the SMS Stack.
 *  
 *  @leave Leaves if StoreParametersL() leaves.
 */
	{
	StoreParametersL();
	}

void CTestSmsParametersActive::RunL()
/**
 *  Make store/retrieve parameters request to the SMS Stack or get retrieved parameters from stream.
 *  Stop tests if there was an error or if iHowManyRequests limit was achieved.
 *  
 *  @leave Leaves if StoreParametersL() or GetParametersL() leaves.
 */
	{
	iTestStep->INFO_PRINTF2(_L("CTestSmsParametersActive::RunL iStatus = %d"), iStatus.Int());
	iCounter++;
	if(iCounter<iHowManyRequests && iStatus.Int()==KErrNone)
		{
		switch(iState)
			{
			case ESmsNextStoreParameters:
				{
				StoreParametersL();
				break;
				}
			case ESmsNextRetrieveParameters:
				{
				RetrieveParameters();
				break;
				}
			case ESmsNextGetParameters:
				{
				GetParametersL();
				break;
				}
			default:
				iTestSmsActiveStop.StopTests(KErrGeneral);
			}
		}
	else
		iTestSmsActiveStop.StopTests(iStatus.Int());
	}

TInt CTestSmsParametersActive::RunError(TInt aError)
/**
 *  Handle possible error situation by stopping all tests.
 */
	{
	iTestStep->INFO_PRINTF2(_L("CTestSmsParametersActive::RunError aError = %d"),aError);
	iTestSmsActiveStop.StopTests(aError);
	return KErrNone;
	}

void CTestSmsParametersActive::StoreParametersL()
/**
 *  Write list to the stream and make write parameters request to the SMS Stack.
 *  
 *  @leave Leaves if CommitL() or if not enough memory available.
 */
	{
	iTestStep->INFO_PRINTF1(_L("Store parameters"));
	iState = ESmsNextRetrieveParameters;
	RSmsSocketWriteStream writestream(iSocket);
	writestream << *iStoreSmspList;
	writestream.CommitL();

	iSocket.Ioctl(KIoctlWriteSmsParams,iStatus,NULL,KSolSmsProv);
	SetActive();
	}

void CTestSmsParametersActive::RetrieveParameters()
/**
 *  Make retrieve parameters request to the SMS Stack.
 */
	{
	iTestStep->INFO_PRINTF1(_L("Retrieve parameters"));
	iState = ESmsNextGetParameters;
    iSocket.Ioctl(KIoctlReadSmsParams,iStatus,NULL,KSolSmsProv);
	SetActive();
	}

void CTestSmsParametersActive::GetParametersL()
/**
 *  Get retrieved parameter list from stream and complete request to the SMS Stack.
 *  
 *  @leave Leaves if CMobilePhoneSmspList::NewL() or PushL() leaves.
 *  @leave Leaves if comparing of parameter lists fails.
 */
	{
	iTestStep->INFO_PRINTF1(_L("Get parameters"));
	iState = ESmsNextStoreParameters;
	CMobilePhoneSmspList* retrieveSmspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(retrieveSmspList);

	RSmsSocketReadStream readstream(iSocket);
    readstream >> *retrieveSmspList;

	User::LeaveIfError(iSmsStackTestUtils->TestParameters(*iStoreSmspList,*retrieveSmspList));
	CleanupStack::PopAndDestroy(retrieveSmspList);

    iSocket.Ioctl(KIoctlCompleteReadSmsParams,iStatus,NULL, KSolSmsProv);
	SetActive();
	}

/*
-------------------------------------------------------------------------------

	CTestSmsStorageActive

	This class makes enumerate, write and delete messages requests to the
	SMS Stack. It also get enumerated messages from stream and
	compare them to the stored messages.

-------------------------------------------------------------------------------
*/

CSmsMessage* CTestSmsStorageActive::CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RFs& aFs, CSmsPDU::TSmsPDUType aType)
/**
 *  Create a uninitialised SMS message
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(aFs, aType, buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(aAlphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);

	smsMessage->SetToFromAddressL(iTelephoneNumber);
	smsMessage->SmsPDU().SetServiceCenterAddressL(iServiceCenterNumber);
	buffer->InsertL(0,aDes);
	CleanupStack::Pop(smsMessage);
	return smsMessage;
	}




CTestSmsStorageActive* CTestSmsStorageActive::NewL(RSocketServ& aSocketServer,CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CSmsStackTestUtils* aSmsStackTestUtils, CTestStep* aTestStep)
/**
 *  2 Phase constructor. Open the socket that is used for writing, deleting and enumerating of messages to/from
 *  phone side SMS storage. Create also instance of CSmsMessage class that will be used while running storage tests.
 *  
 *  @leave Leaves if not enough memory is available.
 *  @leave Leaves if CreateSmsMessageL() leaves.
 *  @param aSocketServer reference to the socket server object
 *  @param aTestSmsActiveStop a reference to the "stopper" object
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	{
	CTestSmsStorageActive* storageActive = new (ELeave) CTestSmsStorageActive(aTestSmsActiveStop,aHowManyRequests, aFs, aTestStep);
	CleanupStack::PushL(storageActive);

	aSmsStackTestUtils->OpenSmsSocketL(aSocketServer,storageActive->iSocket,ESmsAddrLocalOperation);

	_LIT(KTestMsg1,"Stored SMS Message.");
	_LIT(KPekka,"+358408415528");
	_LIT(KRadiolinjaSC,"+358508771010");
	storageActive->iTelephoneNumber=KPekka;
	storageActive->iServiceCenterNumber=KRadiolinjaSC;
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsmessage=storageActive->CreateSmsMessageL(KTestMsg1,alphabet, aFs);
	smsmessage->SetStatus(NMobileSmsStore::EStoredMessageUnsent);
	CleanupStack::PushL(smsmessage);
	User::LeaveIfError(storageActive->iStoredMessages.Append(smsmessage));
	CleanupStack::Pop(smsmessage);
	storageActive->iStoredMessages[0]->SetStorage(CSmsMessage::ESmsSIMStorage);

	CleanupStack::Pop(storageActive);
	return storageActive;
	}

CTestSmsStorageActive::CTestSmsStorageActive(CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CTestStep* aTestStep):
/**
 *  Constructor
 *  
 *  @param aTestSmsActiveStop a reference to the "stopper" object
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	CTestSmsActiveBase(aHowManyRequests),
	iTestSmsActiveStop(aTestSmsActiveStop),
	iFs(aFs),
	iTestStep(aTestStep)
	{
	}

CTestSmsStorageActive::~CTestSmsStorageActive()
/**
 *  Destructor.
 *  
 *  Cancels SMS store handling process and deletes iEnumeratedMessages and iStoredMessages arrays.
 */
	{
	Cancel();
	iEnumeratedMessages.ResetAndDestroy();
	iStoredMessages.ResetAndDestroy();
	}

void CTestSmsStorageActive::DoCancel()
/**
 *  Cancels SMS store handling process.
 */
	{
	iSocket.CancelIoctl();
	}

void CTestSmsStorageActive::StartL()
/**
 *  Make write message request to the SMS Stack and set state to ESmsNextWriteMessage.
 *  
 *  @leave Leaves if WriteMessageL() leaves.
 */
	{
	//Enumerate messages from store before deleting
	iState = ESmsNextWriteMessage;
	WriteMessageL();
	}

void CTestSmsStorageActive::RunL()
/**
 *  Make write/enumerate/delete messages request to the SMS Stack or get enumerated messages from stream.
 *  Stop tests if there was an error or if iHowManyRequests limit was achieved.
 *  
 *  @leave Leaves if WriteMessageL() or GetMessagesL() or DeleteMessageL() leaves.
 */
	{
	iTestStep->INFO_PRINTF2(_L("CTestSmsStorageActive::RunL iStatus = %d"), iStatus.Int());
	iCounter++;
	if(iCounter<iHowManyRequests && iStatus.Int()==KErrNone)
		{
		switch (iState)
			{
			case ESmsNextWriteMessage:
				{
				WriteMessageL();
				break;
				}
			case ESmsNextEnumerateMessages:
				{
				EnumerateMessages();
				break;
				}
			case ESmsNextGetMessages:
				{
				if(iSbuf()>0)
					GetMessagesL();
				else
					//Something went wrong. There should be messages
					iTestSmsActiveStop.StopTests(KErrNotFound);
				break;
				}
			case ESmsNextDeleteMessage:
				{
				DeleteMessageL();
				break;
				}

			default:
				iTestSmsActiveStop.StopTests(KErrGeneral);
			}
		}
	else
		iTestSmsActiveStop.StopTests(iStatus.Int());
	}

TInt CTestSmsStorageActive::RunError(TInt aError)
/**
 *  Handle possible error situation by stopping all tests.
 */
	{
	iTestStep->INFO_PRINTF2(_L("CTestSmsStorageActive::RunError aError = %d"),aError);
	iTestSmsActiveStop.StopTests(aError);
	return KErrNone;
	}

void CTestSmsStorageActive::WriteMessageL()
/**
 *  Write message to the stream and make write SMS request to the SMS Stack.
 *  
 *  @leave Leaves if CommitL leaves or if not enough memory available.
 */
	{
	iState = ESmsNextEnumerateMessages;
	iTestStep->INFO_PRINTF1(_L("Write message"));
	RSmsSocketWriteStream writestream(iSocket);
	writestream << *iStoredMessages[0];
	writestream.CommitL();

	iSocket.Ioctl(KIoctlWriteSmsMessage,iStatus,NULL,KSolSmsProv);
	SetActive();
	}

void CTestSmsStorageActive::EnumerateMessages()
/**
 *  Write message to the stream and make write SMS request to the SMS Stack.
 */
	{
	iTestStep->INFO_PRINTF1(_L("Enumerate messages"));
	iState=ESmsNextGetMessages;
	iEnumeratedMessages.ResetAndDestroy();
	iReadMessagesCount=0;
	iSbuf()=0;
	iSocket.Ioctl(KIoctlEnumerateSmsMessages,iStatus,&iSbuf,KSolSmsProv);
	SetActive();
	}

void CTestSmsStorageActive::TestSmsListsL(RPointerArray<CSmsMessage>& aLeft,RPointerArray<CSmsMessage>& aRight)
/**
 *  Compare sms lists
 *  @param aLeft an Array containing CSmsMessage objects
 *  @param aRight an Array containing CSmsMessage objects
 *  @return TInt : The error code
 */
	{
    if(aLeft.Count() == aRight.Count())
        {
        for(TInt leftIndex=0; leftIndex<aLeft.Count(); leftIndex++)
            {
			TBool found=EFalse;
            CSmsMessage* leftEntry=aLeft[leftIndex];

			CSmsBufferBase& leftBuffer=leftEntry->Buffer();
			TInt bufLen=leftBuffer.Length();
			HBufC* leftTextBuf=HBufC::NewL(bufLen);
			CleanupStack::PushL(leftTextBuf);
			TPtr leftTextPtr(leftTextBuf->Des());
			leftBuffer.Extract(leftTextPtr,0,bufLen);

            for(TInt rightIndex=0; rightIndex<aRight.Count(); rightIndex++)
                {
                CSmsMessage* rigthEntry=aRight[rightIndex];

				CSmsBufferBase& rightBuffer=rigthEntry->Buffer();
				bufLen=rightBuffer.Length();
				HBufC* rigthTextBuf=HBufC::NewL(bufLen);
				CleanupStack::PushL(rigthTextBuf);
				TPtr rightTextPtr(rigthTextBuf->Des());
				rightBuffer.Extract(rightTextPtr,0,bufLen);

                if((leftTextPtr == rightTextPtr) &&
					/* 9210 does not report SCA (leftEntry->ServiceCenterAddress() == rigthEntry->ServiceCenterAddress()) &&*/
					(leftEntry->ToFromAddress() == rigthEntry->ToFromAddress()) &&
					(leftEntry->Storage() == rigthEntry->Storage()) &&
					(leftEntry->Status() == rigthEntry->Status()))
					{
					found=ETrue;
                    }
				CleanupStack::PopAndDestroy(rigthTextBuf);
				if(found)
					break;
                }
			CleanupStack::PopAndDestroy(leftTextBuf);

            if(!found)
                {
		        iTestStep->INFO_PRINTF1(_L("ERROR: SMS lists doesn't match: KErrNotFound"));
                User::Leave(KErrNotFound);
                }
            }
        }
    else
    	{
        iTestStep->INFO_PRINTF1(_L("ERROR: SMS lists doesn't match: KErrCorrupt"));
        User::Leave(KErrCorrupt);
        }

   	iTestStep->INFO_PRINTF1(_L("SMS list comparing Ok!") );
	}


void CTestSmsStorageActive::GetMessagesL()
/**
 *  Get enumerated message from stream.
 *  Set state to ESmsNextDeleteMessage if got all messages, else set state to ESmsNextGetMessages.
 *  Compete get message process to the SMS Stack.
 *  
 *  @leave Leaves if CSmsBuffer::NewL() or CSmsMessage::NewL() or PushL() leaves.
 *  @leave Leaves comparing of SMS lists fails.
 */
	{
	iTestStep->INFO_PRINTF1(_L("Get read message from stream"));
	RSmsSocketReadStream readstream(iSocket);
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
	CleanupStack::PushL(smsMessage);

	readstream >> *smsMessage;
	User::LeaveIfError(iEnumeratedMessages.Append(smsMessage));
	CleanupStack::Pop(smsMessage);

	iReadMessagesCount++;

	TInt count = iSbuf();
	if(iReadMessagesCount < count)
		//Read next message from stream
		iState = ESmsNextGetMessages;
	else
		{
		//Got all messages. Compare enumerated and stored messages
		TestSmsListsL(iEnumeratedMessages,iStoredMessages);

		//Delete message
		iState = ESmsNextDeleteMessage;
		}
	iSocket.Ioctl(KIoctlReadMessageSucceeded,iStatus,NULL,KSolSmsProv);
	SetActive();
	}

void CTestSmsStorageActive::DeleteMessageL()
/**
 *  Write message to the stream and make delete request to the SMS Stack.
 *  
 *  @leave Leaves if CommitL() leaves or if not enough memory available.
 */
	{
	iTestStep->INFO_PRINTF1(_L("Delete message"));
	iState=ESmsNextWriteMessage;
	RSmsSocketWriteStream writestream(iSocket);
	writestream << *iEnumeratedMessages[0];
	writestream.CommitL();

	iSocket.Ioctl(KIoctlDeleteSmsMessage,iStatus,NULL,KSolSmsProv);
	SetActive();
	}

/*-------------------------------------------------------------------------------

	CTestSmsReceiveActive

	This class makes receive message requests to the
	SMS Stack.

-------------------------------------------------------------------------------
*/

CTestSmsReceiveActive* CTestSmsReceiveActive::NewL(RSocketServ& aSocketServer,CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CSmsStackTestUtils* aSmsStackTestUtils, CTestStep* aTestStep)
/**
 *  2 Phase constructor. Open the socket that is used for receiving messages.
 *  
 *  @leave Leaves if not enough memory is available.
 *  @leave Leaves if OpenSmsSocketL() leaves.
 *  @param aSocketServer reference to the socket server object
 *  @param aTestSmsActiveStop a reference to the "stopper" object
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	{
	CTestSmsReceiveActive* receiveActive = new (ELeave) CTestSmsReceiveActive(aTestSmsActiveStop,aHowManyRequests, aFs, aTestStep);
	CleanupStack::PushL(receiveActive);

	aSmsStackTestUtils->OpenSmsSocketL(aSocketServer,receiveActive->iSocket,ESmsAddrRecvAny);

	CleanupStack::Pop(receiveActive);
	return receiveActive;
	}

CTestSmsReceiveActive::CTestSmsReceiveActive(CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CTestStep* aTestStep):
/**
 *  Constructor
 *  
 *  @param aTestSmsActiveStop a reference to the "stopper" object
 *  @param aHowManyRequests a variable that defines how many requests active object makes
 */
	CTestSmsActiveBase(aHowManyRequests),
	iTestSmsActiveStop(aTestSmsActiveStop),
	iFs(aFs),
	iTestStep(aTestStep)
	{
	}

CTestSmsReceiveActive::~CTestSmsReceiveActive()
/**
 *  Destructor.
 *  
 *  Cancels receiving process.
 */
	{
	Cancel();
	}

void CTestSmsReceiveActive::DoCancel()
/**
 *  Cancels receiving process.
 */
	{
	iSocket.CancelIoctl();
	}

void CTestSmsReceiveActive::StartL()
/**
 *  Make receive message request to the SMS Stack
 */
	{
	ReceiveMessage();
	}

void CTestSmsReceiveActive::RunL()
/**
 *  Make receive message request to the SMS Stack or get incoming message from stream.
 *  Stop tests if there was an error or if iHowManyRequests limit was achieved.
 *  
 *  @leave Leaves if GetMessagesL() leaves.
 */
	{
	iTestStep->INFO_PRINTF2(_L("CTestSmsReceiveActive::RunL iStatus = %d"), iStatus.Int());
	//iCounter++;
	if(iCounter<iHowManyRequests && iStatus.Int()==KErrNone)
		{
		switch(iState)
			{
			case ESmsNextReceiveMessage:
				{
				ReceiveMessage();
				break;
				}
			case ESmsNextGetMessage:
				{
				GetMessagesL();
				break;
				}

			default:
				iTestSmsActiveStop.StopTests(KErrGeneral);
			}
		}
	else
		iTestSmsActiveStop.StopTests(iStatus.Int());
	}

TInt CTestSmsReceiveActive::RunError(TInt aError)
/**
 *  Handle possible error situation by stopping all tests.
 */
	{
	iTestStep->INFO_PRINTF2(_L("CTestSmsReceiveActive::RunError aError = %d"),aError);
	iTestSmsActiveStop.StopTests(aError);
	return KErrNone;
	}

void CTestSmsReceiveActive::ReceiveMessage()
/**
 *  Make receive message request to the SMS Stack.
 */
	{
	iTestStep->INFO_PRINTF1(_L("Receive message"));
	iState = ESmsNextGetMessage;
	iSbuf()=KSockSelectRead;
	iSocket.Ioctl(KIOctlSelect,iStatus,&iSbuf,KSOLSocket);
	SetActive();
	}

TText8 CTestSmsReceiveActive::IsCharDisplayable( const TText8 aChar )
	{
	if(( aChar >= 0x20) && (aChar <= 0x80))
		return aChar;
	else
		return '.';
	}

void CTestSmsReceiveActive::PrintMessageL(const CSmsMessage* aSms)
/**
 *  Print the content of SMS to the console
 */
	{
	CSmsBuffer& smsbuffer = (CSmsBuffer&)aSms->Buffer();
	const TInt len = smsbuffer.Length();
	HBufC* hbuf = HBufC::NewL(len);
	TPtr ptr = hbuf->Des();
	smsbuffer.Extract(ptr,0,len);
	iTestStep->INFO_PRINTF1(_L("SMS contains..."));

	for (TInt j = 0; j < len; j++)
        ptr[j]=IsCharDisplayable((TText8)ptr[j]);

   	iTestStep->INFO_PRINTF1(ptr);
	iTestStep->INFO_PRINTF1(_L(""));

	delete hbuf;
	}

void CTestSmsReceiveActive::GetMessagesL()
/**
 *  Get received message from stream and print it to the screen.
 *  
 *  @leave Leaves if not enough memory is available.
 *  @leave Leaves if CSmsBuffer::NewL() or PrintMessageL() or CSmsMessage::NewL() leaves.
 */
	{
	iTestStep->INFO_PRINTF1(_L("Get received message from stream"));
	iState = ESmsNextReceiveMessage;
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs,CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	RSmsSocketReadStream readstream(iSocket);
	readstream >> *smsMessage;

	// Make sure that SMS Stack doesn't try to send us enumerated messages
	if(smsMessage->Type() != CSmsPDU::ESmsDeliver && smsMessage->Type() != CSmsPDU::ESmsStatusReport)
		iTestSmsActiveStop.StopTests(KErrArgument);

	PrintMessageL(smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	iCounter++;
	iSbuf()=0;
	iSocket.Ioctl(KIoctlReadMessageSucceeded,iStatus,&iSbuf,KSolSmsProv);
	SetActive();
	}
