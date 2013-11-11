// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/
#include "TE_smsprt.h"

#include <sacls.h>
#include <exterror.h>
#include <emsformatie.h>
#include <logwraplimits.h>
#include <smspver.h>

#include "smsulog.h"

using namespace CommsDat;

TVerdict CTestSimpleTxAndRx::doTestStepL()
/**
 *  Test a simple Transmit and Receive of a TPDU
 */
	{
	INFO_PRINTF1(_L("Test Simple Tx and Rx SMS"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);
	
	ChangeReceiveModeL(RMobileSmsMessaging::EReceiveUnstoredClientAck);
	
	_LIT(KTestMsg,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg, alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
    CleanupStack::PushL(smsMessage);
    
	TestSmsContentsL(smsMessage, KTestMsg);

	//Save the received message to the SMS storage
	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	WriteSmsToSimL(*smsMessage, socket);
	CleanupStack::PopAndDestroy(smsMessage);

	// Enumerate messages from Store
	RPointerArray<CSmsMessage> messages;
	ReadSmsStoreL(socket, messages);
	messages.ResetAndDestroy();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}
	
TVerdict CTestBinaryTxAndRx::doTestStepL()
/**
 *  Test a binary Transmit and Receive of a TPDU
 */
	{
	INFO_PRINTF1(_L("Test Binary Tx and Rx SMS"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer,socket,ESmsAddrRecvAny);
	CleanupClosePushL(socket);
	
	ChangeReceiveModeL(RMobileSmsMessaging::EReceiveUnstoredClientAck);

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;
		
	const TInt KTestPDUSize = 128;
	TBuf<KTestPDUSize> arrBuf;
	arrBuf.SetLength(KTestPDUSize);
	for(TInt i = 0; i < KTestPDUSize; ++i)
		{			
		arrBuf[i] = i;
		}
		
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(arrBuf, alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);
	
	//Send first SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);
		
	TBuf<KTestPDUSize> arrBuf2;
	arrBuf2.SetLength(KTestPDUSize);
	for(TInt i = 0; i < KTestPDUSize; ++i)
		{		
		arrBuf2[i] = 128 + i;
		}
		
	//Send second SMS
    smsMessage=CreateSmsMessageLC(arrBuf2, alphabet);
    SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);		
				
	//Receive first SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage, arrBuf, ETrue);

	//Save the received message to the SMS storage
	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	WriteSmsToSimL(*smsMessage, socket);
	CleanupStack::PopAndDestroy(smsMessage);
			
	//Receive second SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage, arrBuf2, ETrue);

	//Save the received message to the SMS storage
	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	WriteSmsToSimL(*smsMessage, socket);
	CleanupStack::PopAndDestroy(smsMessage);				
	
	// Enumerate messages from Store
	RPointerArray<CSmsMessage> messages;
	ReadSmsStoreL(socket, messages);
	messages.ResetAndDestroy();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;	 	
	}	
	
TVerdict CTestStatusReport::doTestStepL()
/**
 *  Test a simple Transmit and Receive with status reporting
 */
	{
	INFO_PRINTF1(_L("Test Tx an SMS and then receive a status report"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsWithStatusReportReqL(KTestMsg1,alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Create and send the second SMS
	_LIT(KTest7bitMsg,"test message, length 23"); //7 bits test message, length 23 characters

	//Set destination number
	iTelephoneNumber=KOther;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	smsMessage=CreateSmsWithStatusReportReqL(KTest7bitMsg, alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Create and send the third SMS

	//Set destination number
	iTelephoneNumber=KOther;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	smsMessage=CreateSmsWithStatusReportReqL(KTestMsg1,alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Create and send the fourth SMS

	//Set destination and SC numbers
	iTelephoneNumber=KLocalNumber;
	iServiceCenterNumber=KRadiolinjaSC;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	smsMessage=CreateSmsWithStatusReportReqL(KTestMsg1,alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	// Create and send the 5th SMS. Here the submit PDU contains an international number
	// and the Status Report that will be received contains the local version of that number

	_LIT(KTestInternationalSMSWithLocalSRMsg, "Test international number");

	//Set destination and SC numbers
	iTelephoneNumber = KInternationalTestNumber;

	alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
	smsMessage = CreateSmsWithStatusReportReqL(KTestInternationalSMSWithLocalSRMsg, alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"), &iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"), &iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage, socket);
	CleanupStack::PopAndDestroy(smsMessage);

	////////////
	// PDEF137451
	//Create and send the sixth SMS
	//
	
	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	smsMessage=CreateSmsWithStatusReportReqL(KTestMsg1,alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//
	//Create and send the seventh SMS (7-bit)
	//
	
	//Set destination number
	iTelephoneNumber=KOther;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	smsMessage=CreateSmsWithStatusReportReqL(KTest7bitMsg, alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);
	
	///////////////
	
	//Receive status report
	TSmsServiceCenterAddress telephoneNumber;
	telephoneNumber.Copy( KOther );
	RecvStatusReportL(telephoneNumber, socket);
	
	telephoneNumber = KLocalNumber;
	RecvStatusReportL(telephoneNumber, socket);
	
	telephoneNumber = KOther;
	RecvStatusReportL(telephoneNumber, socket);
	
	telephoneNumber.Copy( KPekka );
	RecvStatusReportL(telephoneNumber, socket);
	
	// CSmsPDUProcessor::DecodeAndProcessPDUL sets the address of the SR
	// to the same as that of the original submit. So we expect the
	// received SR to have an international number, despite the SR PDU
	// having a local number.
	telephoneNumber.Copy(KInternationalTestNumber);
	RecvStatusReportL(telephoneNumber, socket);

	// truncated user data, 8-bit:
	telephoneNumber.Copy( KPekka );
	RecvStatusReportL(telephoneNumber, socket);
	
	// truncated user data, 7-bit:
	telephoneNumber.Copy( KOther );			
	RecvStatusReportL(telephoneNumber, socket);

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestTxWithError::doTestStepL()
/**
 *  Test a simple Transmit and Receive. This test case tests different errors from TSY.
 */
	{
	INFO_PRINTF1(_L("Test Tx an SMS, completed with error"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	RSocket socket2;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket2,ESmsAddrSendOnly);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KSoneraSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	//Set Status report request
	CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
	submitPdu.SetStatusReportRequest(ETrue);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	INFO_PRINTF1(_L("waiting 2 secs for smsprot to load"));
	User::After(2000000);

	//Try sending, Tsy returns error
	SendSmsCancelL(smsMessage,socket, socket2);

	INFO_PRINTF1(_L("Try again sending the SMS"));
	//Now sending succeeds KErrNone
	SendSmsL(smsMessage,socket);

	//set reject duplicates to false
	submitPdu.SetRejectDuplicates(EFalse);
	INFO_PRINTF1(_L("Try sending duplicate SMS when Reject Duplicate is False"));
	SendSmsL(smsMessage,socket);
	TEST(submitPdu.RejectDuplicates() == EFalse);
    		
	//set reject duplicates to true
	submitPdu.SetRejectDuplicates(ETrue);
	INFO_PRINTF1(_L("Try sending duplicate SMS when Reject Duplicate is True"));
	SendSmsL(smsMessage,socket);
	TEST(submitPdu.RejectDuplicates());
	TEST(ETrue);

	// Set reply path to False
    submitPdu.SetReplyPath(EFalse);
	INFO_PRINTF1(_L("Try sending SMS when Reply Path is False"));    
	SendSmsL(smsMessage,socket);
	TEST(submitPdu.ReplyPath() == EFalse);

	// Set reply path to True
    submitPdu.SetReplyPath(ETrue);
	INFO_PRINTF1(_L("Try sending SMS when Reply Path is True"));    
	SendSmsL(smsMessage,socket);
	TEST(submitPdu.ReplyPath());
	TEST(ETrue);

	CleanupStack::PopAndDestroy(smsMessage);
	
	//Receive SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KTestMsg1);

	CleanupStack::PopAndDestroy(3, &socket); // socket, socket2, smsMessage
	return TestStepResult() ;
	}

TVerdict CTestTxFailed::doTestStepL()
/**
 *  Test sending an SMS, transmit will be failed with different error codes
 */
	{
	INFO_PRINTF1(_L("Test Tx an SMS, failed with different error codes"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KSoneraSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Try sending, Dummy Tsy returns RP-error
	TInt i;
	TInt ret;

	for (i=0; i < 25; ++i)
		{
		ret = SendSmsErrorL(smsMessage,socket);
		}

	//Try sending, Dummy Tsy returns RP-error with corrupted or wrong submit report PDU
	for (i=0; i < 2; ++i)
		{
		ret = SendSmsErrorL(smsMessage,socket);
		}

	//Try sending, Dummy Tsy returns general error
	for (i=0; i < 3; ++i)
		{
		ret = SendSmsErrorL(smsMessage,socket);
		}

	//Try sending, check expected error code
	ret = SendSmsErrorL(smsMessage,socket);
	TEST(ret== KErrGsmSMSMemCapacityExceeded);

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
	return TestStepResult() ;
	}

// TODO: test does not seem to be used in any scripts that are run as part of tests
// only present in TE_smsprtRegressionClass0StoreEnabled.script, check if OK to remove
TVerdict CTestMatchingToObserver::doTestStepL()
/**
 *  Test a simple Receive of a single TPDU containing a simple text
 */
	{
	INFO_PRINTF1(_L("Test Messages matching to correct observer"));
	RSocket socket1;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket1,ESmsAddrRecvAny);

	RSocket socket2;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket2,ESmsAddrSendOnly);

	_LIT(KTestMsg1,"test message, 8bits, length 30"); //8 bits test message, length 30 chars
	_LIT(KTestMsg2,"test message, length 23"); //7 bits test message, length 23 characters

	TBuf8<128> match;
	match.Copy(KTestMsg2);

	RSocket socket3;
	TInt ret=socket3.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TEST(ret == KErrNone);
	CleanupClosePushL(socket3);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrMatchText);
	smsaddr.SetTextMatch(_L8("test message, len"));
	ret=socket3.Bind(smsaddr);
	TEST(ret == KErrNone);

	smsaddr.SetTextMatch(_L8("test message, 8bi"));
	ret=socket1.Bind(smsaddr);
	TEST(ret == KErrNone);

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsWithStatusReportReqL(KTestMsg1,alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"), &iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"), &iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket2);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("waiting for incoming SMS...") );

	//Set receiving status report to socket2
	TSmsAddr smsaddr2;

	//ESmsAddrStatusReport
	smsaddr2.SetSmsAddrFamily(ESmsAddrStatusReport);
	ret=socket2.Bind(smsaddr2);
	TEST(ret == KErrNone);

	INFO_PRINTF1(_L("waiting for incoming SMS...") );
	smsMessage = RecvSmsL(socket3);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KTestMsg2);

	CleanupStack::PopAndDestroy(smsMessage);

	//Receive a message to socket1
	smsMessage = RecvSmsL(socket1);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KTestMsg1);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive a status report message to socket2
	smsMessage = RecvSmsL(socket2);
    CleanupStack::PushL(smsMessage);
    
	//Check the status report
	if (smsMessage->Type()==CSmsPDU::ESmsStatusReport)
		{
		INFO_PRINTF1(_L("Received status report"));
		TSmsServiceCenterAddress telephoneNumber=smsMessage->ToFromAddress();
		TEST(telephoneNumber==KPekka);
		INFO_PRINTF2(_L("Message delivered to %S"),&telephoneNumber);
		}

	CleanupStack::PopAndDestroy(4, &socket1); // socket1, socket2, socket3, smsMessage
	return TestStepResult() ;
	}

TVerdict CTestCommandMessages::doTestStepL()
/**
 *  Test first transmit of a message, then send Command message to enable status reporting to previously sent
 *  message and then receive a status report when previously sent message is delivered to recipient.
 */
	{
	INFO_PRINTF1(_L("Test Command messages"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTest7bitMsg,"test message, length 23"); //7 bits test message, length 23 characters

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KSoneraSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTest7bitMsg,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send the message
	INFO_PRINTF1(_L("Sending a message..."));
	SendSmsL(smsMessage,socket);

	//Send the command message
	//Enable status report request
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsCommand=CSmsMessage::NewL(iFs, CSmsPDU::ESmsCommand, buffer);
	CleanupStack::PushL(smsCommand);
	smsCommand->SetToFromAddressL(smsMessage->ToFromAddress());
	smsCommand->SetServiceCenterAddressL(smsMessage->ServiceCenterAddress());
	smsCommand->SetTime(smsMessage->Time());
	CSmsCommand& command=(CSmsCommand&) smsCommand->SmsPDU();
	command.SetCommandType(TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest);

	//Set status report request for Command message
	CSmsSubmit& submitPdu=(CSmsSubmit&)smsCommand->SmsPDU();
	submitPdu.SetStatusReportRequest(EFalse);

	INFO_PRINTF1(_L("Sending command message..."));
	SendCommandSmsL(smsCommand, socket);
	CleanupStack::PopAndDestroy(smsCommand);

	//Receive status report
	TSmsServiceCenterAddress telephoneNumber;
	telephoneNumber.Copy( KPekka );
	RecvStatusReportL(telephoneNumber, socket);

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
	return TestStepResult() ;
	}

TVerdict CTestSimpleRx::doTestStepL()
/**
 *  Test a simple Receive of a single TPDU containing a simple text
 */
	{
	INFO_PRINTF1(_L("Test Simple Rx SMS with Client ACK"));
	RSocket socket;
	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrMatchText);
	smsaddr.SetTextMatch(_L8("test message, 8b"));
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, smsaddr);

	RSocket socketRecvAny;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socketRecvAny, ESmsAddrRecvAny);

	INFO_PRINTF1(_L("waiting for incoming SMS No. 1...") );
	WaitForRecvL(socket);
	CSmsMessage* smsMessage = RecvSmsFailedL(socket);
	CleanupStack::PushL(smsMessage);

	_LIT(KTestMsg1,"Test message, 8bits, length 30");
	TestSmsContentsL(smsMessage,KTestMsg1);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("waiting for incoming SMS No. 2...") );
	WaitForRecvL(socketRecvAny);
	smsMessage = RecvSmsFailedL(socketRecvAny);
	CleanupStack::PushL(smsMessage);

	_LIT(KTestMsg2,"Sest message, 8bits, length 30");
	TestSmsContentsL(smsMessage,KTestMsg2);

	CleanupStack::PopAndDestroy(3, &socket); // socket, socketRecvAny, smsMessage
	return TestStepResult() ;
	}

TVerdict CTest7bitMessTest::doTestStepL()
/**
 *  Test a simple Transmit and Receive of a single TPDU containing a single character
 */
	{
	INFO_PRINTF1(_L("Test Tx and Rx 7 bit SMS with Client ACK"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTest7bitMsg,"test message, length 23"); //7 bits test message, length 23 characters
	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KSoneraSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTest7bitMsg,alphabet);

	//Set status report request
	CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
	submitPdu.SetStatusReportRequest(ETrue);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send the message
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KTest7bitMsg);

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
 	return TestStepResult() ;
	}

TVerdict CTestClassMessages::doTestStepL()
/**
 *  Test Send and receive messages of different classes
 *  TODO - why do I have to send a class 2 sms before the class 1 and 3 sms
 */
    {
    INFO_PRINTF1(_L("Send and receive messages with different classes"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KSoneraSC;

	INFO_PRINTF1(_L("Sending message..."));
	TTestCase Class0msg(_L("Class 0 message"));
    Class0msg.SetSmsClass(TSmsDataCodingScheme::ESmsClass0);
    SendAndRecvTestMessageL(Class0msg,socket);

    TTestCase Class2msg(_L("Class 2 message"));
    Class2msg.SetSmsClass(TSmsDataCodingScheme::ESmsClass2);
    SendAndRecvTestMessageL(Class2msg,socket);

	INFO_PRINTF1(_L("Sending message..."));
    TTestCase Class1msg(_L("Class 1 message"));
    Class1msg.SetSmsClass(TSmsDataCodingScheme::ESmsClass1);
    SendAndRecvTestMessageL(Class1msg,socket);

	INFO_PRINTF1(_L("Sending message..."));
    TTestCase Class3msg(_L("Class 3 message"));
    Class3msg.SetSmsClass(TSmsDataCodingScheme::ESmsClass3);
    SendAndRecvTestMessageL(Class3msg,socket);

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestRxConcatenated::doTestStepL()
/**
 *  Test reception of a concatenated SMS message spanning 3 TPDUs
 *  with the TSY requiring the SMS Stack to ACK each TPDU
 */
	{
	INFO_PRINTF1(_L("Test Rx Three Part Concatenated Message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	WaitForRecvL(socket);
	CSmsMessage* smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	_LIT(KLongText,"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"The End.");
	
	TestSmsContentsL(smsMessage,KLongText);

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
	return TestStepResult() ;
	}

TVerdict CTestRxDuplicate::doTestStepL()
/**
 *  Test reception of a concatenated SMS message spanning 3 TPDUs
 *  with the TSY requiring the SMS Stack to ACK each TPDU, where the second
 *  PDU is repeated even though correctly ACKed by the SMS Stack.
 */
	{
	INFO_PRINTF1(_L("Test Rx Three Part Concatenated Message with 2nd TPDU Duplicated"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	WaitForRecvL(socket);
	CSmsMessage* smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	_LIT(KLongText,"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"The End.");
	
	TestSmsContentsL(smsMessage,KLongText);

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
	return TestStepResult() ;
	}

TVerdict CTestRxRingToneA::doTestStepL()
/**
 *  Test reception of a concatenated SMS Ringing Tone message spanning 3 TPDUs
 *  with the TSY requiring the SMS Stack to ACK each TPDU.
 */
	{
	INFO_PRINTF1(_L("Test Rx Three Part Concatenated SMS Ringing Tone Message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	WaitForRecvL(socket);
	CSmsMessage* smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	const TUint16 KRingingTonePDUA[]= {
	0x02, 0x4A, 0x3A, 0x75, 0x09, 0x85, 0x91, 0xA5, 0xB9, 0x95,
	0xC9, 0xA5, 0x95, 0x0D, 0xBD, 0xC1, 0xE4, 0x04, 0x01, 0xD5,
	0x28, 0x61, 0x0A, 0x32, 0xC2, 0xCC, 0x22, 0x82, 0xCC, 0x22,
	0xC4, 0x99, 0x42, 0x88, 0xB1, 0x26, 0x58, 0x58, 0x84, 0x58,
	0x59, 0x86, 0x58, 0x59, 0x86, 0x18, 0x59, 0x86, 0x18, 0x59,
	0x85, 0x58, 0x61, 0x86, 0xD8, 0x61, 0x86, 0x50, 0x59, 0x0A,
	0x2C, 0x83, 0x2C, 0x2C, 0xC2, 0x28, 0x2C, 0xC2, 0x2C, 0x49,
	0x94, 0x28, 0x8B, 0x12, 0x65, 0x85, 0x88, 0x65, 0x86, 0x18,
	0x65, 0x06, 0x58, 0x61, 0x86, 0x50, 0xA2, 0xCD, 0x49, 0x96,
	0x9B, 0x89, 0x94, 0x18, 0x42, 0x88, 0xB1, 0x27, 0x18, 0xA2,
	0x28, 0x22, 0xC4, 0x9C, 0x62, 0x88, 0xB4, 0xCA, 0x08, 0xA0,
	0x9C, 0x08, 0xA1, 0x27, 0x10, 0x61, 0x8A, 0x22, 0xC2, 0x8C,
	0x22, 0xC2, 0x6C, 0x22, 0xC2, 0x6C, 0x22, 0xC4, 0x9C, 0x62,
	0x89, 0xB0, 0xB3, 0x09, 0xB0, 0xA3, 0x09, 0xB0, 0x8B, 0x12,
	0x71, 0x8A, 0x22, 0xC2, 0x8C, 0x22, 0xC4, 0x9C, 0x62, 0x88,
	0xB0, 0xB3, 0x08, 0xB1, 0x27, 0x18, 0xA2, 0x2C, 0x30, 0xC2,
	0x2C, 0x49, 0xC6, 0x28, 0x8B, 0x0C, 0xB0, 0x8B, 0x12, 0x71,
	0x8A, 0x22, 0xC3, 0x2C, 0x30, 0xC2, 0xCC, 0x30, 0xC2, 0x8C,
	0x26, 0xC2, 0x2C, 0x28, 0xC2, 0x70, 0x26, 0xC2, 0x2C, 0x22,
	0x84, 0x10, 0x42, 0x88, 0xA0, 0xA3, 0x08, 0xB1, 0x26, 0x10,
	0xA2, 0x2C, 0x49, 0x86, 0x14, 0x41, 0x86, 0x14, 0x61, 0x12,
	0x17, 0x41, 0x64, 0x1B, 0x41, 0xA6, 0x28, 0x8B, 0x0A, 0x20,
	0x93, 0x08, 0xB0, 0x92, 0x12, 0x6D, 0x0A, 0x24, 0x82, 0xCC,
	0x24, 0xC4, 0x9B, 0x42, 0x89, 0x21, 0x26, 0x10, 0x6D, 0x86,
	0x18, 0x50, 0x86, 0x58, 0xA2, 0x2C, 0x49, 0x96, 0x1B, 0x61,
	0x96, 0x1B, 0x61, 0x96, 0x18, 0x61,	0xB6, 0x28, 0x93, 0x12,
	0x6D, 0x8A, 0x22, 0xC4, 0x9B, 0x61, 0x96, 0x18, 0x61, 0x96,
	0x28, 0x8B, 0x12, 0x65, 0x86, 0x18, 0x65, 0x8A, 0x24, 0xC4,
	0x99, 0x61, 0x86, 0x19, 0x62, 0x8A, 0x31, 0x26, 0x58, 0x61,
	0x86, 0x58, 0xA2, 0xCC, 0x49, 0x96, 0x18, 0x61, 0x96, 0x28,
	0xB3, 0x0A, 0x30, 0x93, 0x0A, 0x30, 0x8B, 0x12, 0x6D, 0x86,
	0x18, 0xA2, 0x2D, 0x49, 0xB8, 0x1B, 0x61, 0x96, 0x19, 0x28 };

	TPtrC ringingTonePDUA(KRingingTonePDUA,sizeof(KRingingTonePDUA)/sizeof(TUint16));
	TestSmsContentsL(smsMessage,ringingTonePDUA);

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
  	return TestStepResult() ;
	}

TVerdict CTestTxRxConcatenated::doTestStepL()
/**
 *  Test transmition and reception of a concatenated SMS message spanning 3 TPDUs
 */
	{
	INFO_PRINTF1(_L("Test Tx and Rx SMS with 3 PDU message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KLongText,"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"The End.");

	//Set destination and SC numbers
	iTelephoneNumber=KRegTestNumber;
	iServiceCenterNumber=KVodafoneSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsMessage=CreateSmsWithStatusReportReqL(KLongText,alphabet);

	CleanupStack::PushL(smsMessage);
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KLongText);
	CleanupStack::PopAndDestroy(smsMessage);

	TSmsServiceCenterAddress telephoneNumber;
	telephoneNumber.Copy( KRegTestNumber );
	RecvStatusReportL(telephoneNumber, socket);

	// TX & RX 8 bit conc msg
	alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	smsMessage=CreateSmsMessageLC(KLongText,alphabet);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KLongText);
	CleanupStack::PopAndDestroy(smsMessage);

	// TX & RX 16 bit conc msg
	_LIT(KLongText2,"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"The End.");

	alphabet=TSmsDataCodingScheme::ESmsAlphabetUCS2;
	smsMessage=CreateSmsMessageLC(KLongText2,alphabet);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KLongText2);

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
 	return TestStepResult();
	}

TVerdict CTestParamStorage::doTestStepL()
/**
 *  Test retrieving and storing the sms parameters
 */
	{
	INFO_PRINTF1(_L("Test the parameter storage"));
    // Open the socket for SIM operations
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrLocalOperation);

    //
    // Retrieve SMS parameters
    //

	//Create the smspList
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	INFO_PRINTF1(_L("waiting 2 secs for smsprot to load"));
	User::After(2000000);

    // Make read SMS params request to the SMS Stack.
    TRequestStatus status;

    socket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
    INFO_PRINTF1(_L("waiting for SMS parameters..."));
    User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

    // Read list from stream and make acknowledgement to the SMS Stack
    RSmsSocketReadStream readstream(socket);

    readstream >> *smspList;
    socket.Ioctl(KIoctlCompleteReadSmsParams, status, NULL,KSolSmsProv);
    User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	CleanupStack::PopAndDestroy(smspList);

	//
	// Store SMS parameters
	//
	smspList=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy;

	entryToTsy.iText = DMMTSY_SMSP_STORE_TEXT1;
	entryToTsy.iValidParams = DMMTSY_SMSP_STORE_VALID_PARAMS1;
    entryToTsy.iDestination.iTypeOfNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TON_0;
    entryToTsy.iDestination.iNumberPlan = DMMTSY_SMS_MESSAGE_DESTINATION_NP_0;
	entryToTsy.iDestination.iTelNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_0;
	entryToTsy.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY1;
    entryToTsy.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_1;
    entryToTsy.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_1;
	entryToTsy.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_1;
	entryToTsy.iProtocolId = DMMTSY_SMSP_STORE_PID1;
	entryToTsy.iDcs = DMMTSY_SMSP_STORE_DCS1;

    // Add 6 entries to the list
	TInt i = 0;

    for(i=0; i<6; ++i)
		{
        entryToTsy.iIndex = i;
	    smspList->AddEntryL(entryToTsy);
		}

    INFO_PRINTF1(_L("storing the SMS parameters..."));

    // Write parameter list to the stream and make write SMS parameters request to the SMS Stack.
    TInt ret = iSmsStackTestUtils->StoreParamsL(*smspList,socket,EFalse);
    TEST_CHECKL(ret,KErrArgument,_L("Status values doesn't match"));

    // Writing failed because SIM tsy's SMS parameter store can contain only 5 SMSP sets.

    // Delete one entry from SMSP list and try to write the list again.
    smspList->DeleteEntryL(5);

    ret = iSmsStackTestUtils->StoreParamsL(*smspList,socket,EFalse);
	TEST(ret == KErrNone);

	//
	// Retrieve SMS parameters again
	//
	CMobilePhoneSmspList* smspList2=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList2);

    // Test first canceling of Read SMS Parameters request
    socket.Ioctl(KIoctlReadSmsParams,status,NULL,KSolSmsProv);
    socket.CancelIoctl();
    User::WaitForRequest(status);

    // originl code was
	// GLOBAL_CHECKPOINT_CODE(status.Int());
	// thus always assumes that the cancelling will be missed,
	// but with the change due defect (DEF40029) to smsppara.cpp,
	// see CSmsReadParams::Start this however is not true.
	TEST_CHECKL(status.Int(),KErrCancel,_L("Status values doesn't match"));


	// Now read SMS parameters
	socket.Ioctl(KIoctlReadSmsParams,status,NULL,KSolSmsProv);
	INFO_PRINTF1(_L("waiting for SMS parameters...") );
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	// Read list from stream and make acknowledgement to the SMS Stack
	readstream >> *smspList2;
	socket.Ioctl(KIoctlCompleteReadSmsParams,status,NULL, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	ret = iSmsStackTestUtils->TestParameters(*smspList,*smspList2);
	TEST(ret == KErrNone);

	CleanupStack::PopAndDestroy(2);	//smspList,smspList2

	//
	// Store SMS parameters again
	//
	smspList=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy2;

	entryToTsy2.iText = DMMTSY_SMSP_STORE_TEXT2;
	entryToTsy2.iValidParams = DMMTSY_SMSP_STORE_VALID_PARAMS2;
	entryToTsy2.iDestination.iTypeOfNumber = DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_2;
	entryToTsy2.iDestination.iNumberPlan = DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_2;
	entryToTsy2.iDestination.iTelNumber = DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_2;
	entryToTsy2.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY2;
	entryToTsy2.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_2;
	entryToTsy2.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_2;
	entryToTsy2.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_2;
	entryToTsy2.iProtocolId = DMMTSY_SMSP_STORE_PID2;
	entryToTsy2.iDcs = DMMTSY_SMSP_STORE_DCS2;

	// Add 5 entries to the list
	for(i=0; i<5; i++)
		{
		entryToTsy2.iIndex = i;
		smspList->AddEntryL(entryToTsy2);
		}

	INFO_PRINTF1(_L("storing the SMS parameters...") );

	// Test canceling of Write SMS Parameters request
	ret = iSmsStackTestUtils->StoreParamsL(*smspList,socket,ETrue);
	TEST_CHECKL(ret,KErrCancel,_L("Status values doesn't match"));

	// Make Write SMS Parameters request
	ret = iSmsStackTestUtils->StoreParamsL(*smspList,socket,EFalse);
	TEST(ret == KErrNone);

	CleanupStack::PopAndDestroy(2, &socket); // socket,smspList
   	return TestStepResult();
	}

TVerdict CTestSmsStore::doTestStepL()
/**
 *  Test Sms message storage
 */
	{
	INFO_PRINTF1(_L("Test the SMS storage"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	// Enumerate messages
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);

	const CSmsMessage* message = messages[0];

	TInt ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	//Try to delete the same message again!
	ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	//Delete concatenated msg that has one pdu missing
	message = messages[2];
	ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	messages.ResetAndDestroy();

	// Create and store the message
	_LIT(KStoreMsg1,"SIM TEST MESSAGE 1");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsmessage=CreateSmsMessageLC(KStoreMsg1,alphabet);

	smsmessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	WriteSmsToSimL(*smsmessage, socket);
	CleanupStack::PopAndDestroy(smsmessage);

	// Create and store another message
	_LIT(KStoreMsg2,"ME TEST MESSAGE 2");
	alphabet=TSmsDataCodingScheme::ESmsAlphabetUCS2;
	smsmessage=CreateSmsMessageLC(KStoreMsg2,alphabet);

	smsmessage->SetStorage(CSmsMessage::ESmsPhoneStorage);
	WriteSmsToSimL(*smsmessage, socket);
	CleanupStack::PopAndDestroy(smsmessage);

	// Enumerate messages
	ReadSmsStoreL(socket, messages);

	//Check that count of messages matches to supposed count
	TEST(messages.Count()==4);

	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&socket);
   	return TestStepResult() ;
	}

TVerdict CTestSmsStoreList::doTestStepL()
/**
 *  Test Sms message storage
 */
	{
	INFO_PRINTF1(_L("Test the SMS storage - read SmsList"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	// Enumerate messages
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);

	const CSmsMessage* message = messages[1];

	TInt ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	//Try to delete the same message again!
	ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	//Delete concatenated msg that has one pdu duplicated
	message = messages[4];
	ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	messages.ResetAndDestroy();

	// Create and store the message
	_LIT(KStoreMsg1,"SIM TEST MESSAGE 1");
	iTelephoneNumber=KOther;
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsmessage=CreateSmsMessageLC(KStoreMsg1,alphabet);
	smsmessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	smsmessage->SetStatus(NMobileSmsStore::EStoredMessageUnsent);

	WriteSmsToSimL(*smsmessage, socket);
	CleanupStack::PopAndDestroy(smsmessage);

	// Create and store another message
	_LIT(KStoreMsg2,"COMB TEST MESSAGE 2");
	alphabet=TSmsDataCodingScheme::ESmsAlphabetUCS2;
	smsmessage=CreateSmsMessageLC(KStoreMsg2,alphabet);
	smsmessage->SetStorage(CSmsMessage::ESmsCombinedStorage);
	smsmessage->SetStatus(NMobileSmsStore::EStoredMessageUnsent);

	WriteSmsToSimL(*smsmessage, socket);
	CleanupStack::PopAndDestroy(smsmessage);

	// Create and store third message
	_LIT(KStoreMsg3,"ME TEST MESSAGE 3");
	alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	smsmessage=CreateSmsMessageLC(KStoreMsg3,alphabet);
	smsmessage->SetStorage(CSmsMessage::ESmsPhoneStorage);
	smsmessage->SetStatus(NMobileSmsStore::EStoredMessageUnsent);

	WriteSmsToSimL(*smsmessage, socket);
	CleanupStack::PopAndDestroy(smsmessage);

	// Enumerate messages
	TEST(messages.Count() == 0);
	ReadSmsStoreL(socket, messages);

	//Check that count of messages matches to supposed count
	TEST(messages.Count()==6);

	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestDeleteSms::doTestStepL()
/**
 *  Try to delete message without enumerating the store before it.
 */
	{
	INFO_PRINTF1(_L("Test deleting message from SMS storage"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KStoreMsg1,"HELLO CHRIS");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsmessage=CreateSmsMessageLC(KStoreMsg1,alphabet);

	//Try to delete the message without enumerating first!
	TRequestStatus status;

	RSmsSocketWriteStream writestream(socket);
	writestream << *smsmessage;
	writestream.CommitL();

    TBool tryAgain = ETrue;
    TInt attempts (0);
    TInt maxRetries = 3;
	
    // Delete messages from store. If it fails with KErrNotReady wait for 3 seconds to 
    // allow sms protocol to fully load and repeat
    while( tryAgain && attempts++ < maxRetries )
        {
        socket.Ioctl(KIoctlDeleteSmsMessage, status, NULL, KSolSmsProv);
        User::WaitForRequest(status);
        INFO_PRINTF2(_L("Delete messages from store returned [status=%d]"), status.Int());
        
        if ( status.Int() == KErrNotReady )
            {
            INFO_PRINTF1(_L("Trying to delete again... "));
            User::After(3000000);
            }
        else
            {
            tryAgain = EFalse;
            }
        }

	TEST(status.Int() == KErrArgument);

	CleanupStack::PopAndDestroy(2, &socket); //socket, smsmessage
 	return TestStepResult() ;
	}

/**
 *  Tests the TSmsAddr class
 * 
 *  @return A TVerdict test result.
 */
TVerdict CTestSmsAddr::doTestStepL()
    {
    TSmsAddr addr1;

    INFO_PRINTF1(_L("Test default values ..."));

    TEST(addr1.SmsAddrFamily() == ESmsAddrUnbound);
    
    INFO_PRINTF1(_L("Test address family set/get and dual-SIM aware flag ..."));
    
    addr1.SetSmsAddrFamily(ESmsAddrLocalOperation);
    TEST(addr1.SmsAddrFamily() == ESmsAddrLocalOperation);
 
    
    addr1.SetSmsAddrFamily(ESmsAddrEmail);
    TEST(addr1.SmsAddrFamily() == ESmsAddrEmail);
    
    INFO_PRINTF1(_L("Test identifier match set/get ..."));
    
    addr1.SetSmsAddrFamily(ESmsAddrMatchIEI);
    addr1.SetIdentifierMatch(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit);
    TEST(addr1.IdentifierMatch() == CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit);
    
    INFO_PRINTF1(_L("Test text match set/get ..."));
    
    addr1.SetSmsAddrFamily(ESmsAddrMatchText);
    addr1.SetTextMatch(_L8("test message, len"));
    TEST(addr1.TextMatch().CompareF(_L8("test message, len")) == 0);
    
    addr1.SetTextMatch(_L8("test message this is max:extra"));
    TEST(addr1.TextMatch().CompareF(_L8("test message this is max")) == 0);
    
    INFO_PRINTF1(_L("Test compare ..."));
    
    TSmsAddr addr2;
    addr2.SetSmsAddrFamily(ESmsAddrMatchText);
    addr2.SetTextMatch(_L8("test message, len"));

    TSmsAddr addr3;
    addr3.SetSmsAddrFamily(ESmsAddrMatchText);
    addr3.SetTextMatch(_L8("test message, len"));

    TEST((addr2 == addr3));
    
    addr3.SetTextMatch(_L8("diff test message"));
    TEST(!(addr2 == addr3));
  
    addr2.SetSmsAddrFamily(ESmsAddrMatchIEI);
    addr2.SetIdentifierMatch(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit);
    
    TEST(!(addr2 == addr3));

    addr3.SetSmsAddrFamily(ESmsAddrMatchIEI);
    addr3.SetIdentifierMatch(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit);

    TEST((addr2 == addr3));
    
    addr2.SetIdentifierMatch(CSmsInformationElement::ESmsIEIApplicationPortAddressing16Bit);
    TEST(!(addr2 == addr3));
    
    return TestStepResult();    
    }

TVerdict CTestSocketBinding::doTestStepL()
/**
 *  Test opening and binding the socket to different SMS Address types
 */
	{
	INFO_PRINTF1(_L("Test Open & Bind the Socket"));
	RSocket socket1;
	RSocket socket2;

	TInt ret1=socket1.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TEST(ret1 == KErrNone);
	CleanupClosePushL(socket1);
	TSmsAddr smsaddr1;

	TInt ret2=socket2.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TEST(ret2 == KErrNone);
	CleanupClosePushL(socket2);
	TSmsAddr smsaddr2;

	//ESmsAddrUnbound
	smsaddr1.SetSmsAddrFamily(ESmsAddrUnbound);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrUnbound);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	//ESmsAddrSendOnly
	smsaddr1.SetSmsAddrFamily(ESmsAddrSendOnly);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrSendOnly);
	ret2=socket1.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	//ESmsAddrMessageIndication
	smsaddr1.SetSmsAddrFamily(ESmsAddrMessageIndication);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	//  client can  re-bind
	smsaddr2.SetSmsAddrFamily(ESmsAddrMessageIndication);
	ret2=socket1.Bind(smsaddr2);
	TEST(ret2==KErrNone);

	//ESmsAddrMatchIEI
	smsaddr1.SetSmsAddrFamily(ESmsAddrMatchIEI);
	smsaddr1.SetIdentifierMatch(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrMatchIEI);
	smsaddr2.SetIdentifierMatch(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication);
	ret2=socket1.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	//ESmsAddrMatchText
	smsaddr1.SetSmsAddrFamily(ESmsAddrMatchText);
	smsaddr1.SetTextMatch(_L8("test message, len"));
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrMatchText);
	smsaddr2.SetTextMatch(_L8("test message,"));
	ret2=socket1.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	//ESmsAddrRecvAny
	smsaddr1.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

//  Only one client could bind to ESmsAddrRecvAny at a time
//  This address type is returned with KErrAlreadyExists
	smsaddr2.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2==KErrAlreadyExists);

	//ESmsAddrStatusReport
	smsaddr1.SetSmsAddrFamily(ESmsAddrStatusReport);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

//  Only one client could bind to ESmsAddrStatusReport at a time
//  This address type is returned with KErrAlreadyExists
	smsaddr2.SetSmsAddrFamily(ESmsAddrStatusReport);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2 == KErrAlreadyExists);

	//ESmsAddrLocalOperation
	smsaddr1.SetSmsAddrFamily(ESmsAddrLocalOperation);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrLocalOperation);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	CleanupStack::Pop(&socket2);
	CleanupStack::Pop(&socket1);
 	INFO_PRINTF1(_L("All bindings ok!"));
	return TestStepResult() ;
	}

TVerdict CTestSmsEventLogger::doTestStepL()
/**
 *  Test SMS event logger
 */
	{
	INFO_PRINTF1(_L("Test SMS event logger"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	CLogEvent* logEvent=CLogEvent::NewL();
	CleanupStack::PushL(logEvent);

	//
	// Test logging of sending and receiving events
	//
	
	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	_LIT(KTest7bitMsg,"test message, length 23"); //7 bits test message, length 23 characters

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendSmsMessage=CreateSmsMessageLC(KTest7bitMsg,alphabet);

	//Send SMS-SUBMIT
	SendSmsL(sendSmsMessage,socket);

	//Receive SMS-DELIVER
	WaitForRecvL(socket);
	CSmsMessage* receiveSmsMessage = RecvSmsL(socket);
	CleanupStack::PushL(receiveSmsMessage);

	//Get sent message's log event
	iSmsStackTestUtils->GetLogEventL(*logEvent,receiveSmsMessage->LogServerId()-1);

	//Check contents of log event
	INFO_PRINTF1(_L("Checking sent messages log event.") );
	if((logEvent->Description() != _L("Short message")) ||
	   (logEvent->Direction() != _L("Outgoing")) ||
	   (logEvent->Status() != _L("Sent")) ||
	   (logEvent->Subject() != _L("test message, length 23")) ||
	   (logEvent->Number() != sendSmsMessage->ToFromAddress()))
		TEST(KErrArgument);

	//Get received message's log event
	iSmsStackTestUtils->GetLogEventL(*logEvent,receiveSmsMessage->LogServerId());

	//Check contents of log event
	INFO_PRINTF1(_L("Checking received messages log event.") );
	if((logEvent->Description() != _L("Short message")) ||
	   (logEvent->Direction() != _L("Incoming")) ||
	   (logEvent->Status() != _L("Delivered")) ||
	   (logEvent->Subject() != _L("test message, length 23")) ||
	   (logEvent->Number() != receiveSmsMessage->ToFromAddress()))
		TEST(KErrArgument);

    CleanupStack::PopAndDestroy(2); //sendSmsMessage, receiveSmsMessage

//
// Test event logging when sending SMS with SMS-STATUS-REPORT request
//
	_LIT(KTestMsg1,"test message, 8bits, length 30");

	alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	sendSmsMessage=CreateSmsWithStatusReportReqL(KTestMsg1,alphabet);
	CleanupStack::PushL(sendSmsMessage);

	//Send SMS-SUBMIT with SMS-STATUS-REPORT request
	SendSmsL(sendSmsMessage,socket);

	//Receive SMS-DELIVER
	WaitForRecvL(socket);
	receiveSmsMessage = RecvSmsL(socket);
	CleanupStack::PushL(receiveSmsMessage);

	//Get sent message's log event
	iSmsStackTestUtils->GetLogEventL(*logEvent,receiveSmsMessage->LogServerId()-1);

	//Check contents of log event
	INFO_PRINTF1(_L("Checking sent messages log event before status report received.") );
	if((logEvent->Description() != _L("Short message")) ||
	   (logEvent->Direction() != _L("Outgoing")) ||
	   (logEvent->Status() != _L("Pending")) ||
	   (logEvent->Subject() != _L("Data Message")) ||
	   (logEvent->Number() != sendSmsMessage->ToFromAddress()))
		TEST(KErrArgument);

	//Get received message's log event
	TInt logServerId = receiveSmsMessage->LogServerId();
	iSmsStackTestUtils->GetLogEventL(*logEvent,receiveSmsMessage->LogServerId());

	//Check contents of log event
	INFO_PRINTF1(_L("Checking received messages log event.") );
	if((logEvent->Description() != _L("Short message")) ||
	   (logEvent->Direction() != _L("Incoming")) ||
	   (logEvent->Status() != _L("Delivered")) ||
	   (logEvent->Subject() != _L("Data Message")) ||
	   (logEvent->Number() != receiveSmsMessage->ToFromAddress()))
		TEST(KErrArgument);

	//Receive SMS-STATUS-REPORT
	CleanupStack::PopAndDestroy(receiveSmsMessage);
	WaitForRecvL(socket);
	receiveSmsMessage = RecvSmsL(socket);
	CleanupStack::PushL(receiveSmsMessage);

	//Status report received. Get sent message's log event again.
	//Status report's LogServerId should be same as submit's LogServerId
	iSmsStackTestUtils->GetLogEventL(*logEvent,receiveSmsMessage->LogServerId());

	//Check contents of log event
	INFO_PRINTF1(_L("Checking sent messages log event after status report received.") );
	if((logEvent->Description() != _L("Short message")) ||
	   (logEvent->Direction() != _L("Outgoing")) ||
	   (logEvent->Status() != _L("Delivered")) ||
	   (logEvent->Subject() != _L("Data Message")) ||
	   (logEvent->Number() != sendSmsMessage->ToFromAddress()))
		TEST(KErrArgument);

    // validate that log entry  has been updated with the time the
    // SMS was actually received by the destination mobile, ie the
    // Status Report's discharge time. This has been set to be in 
    // 2020 to ensure that the SMS does not have a time stamp
    // which is earlier than the time it was sent  by the originator- this would
    // result in the event log entry being purged from the logging system.
    TTime time = logEvent->Time();
    TDateTime dateTime = time.DateTime();
    
    if((dateTime.Year()   != 2020) ||
       (dateTime.Month()  != 10) ||
       (dateTime.Day()    != 1)  ||
       (dateTime.Hour()   != 13) ||
       (dateTime.Minute() != 38) ||
       (dateTime.Second() != 18) )     
        TEST(KErrArgument); 
	
    //
    // Test logging when senging fails.
    //

	//Send SMS-SUBMIT
	SendSmsErrorL(sendSmsMessage,socket);

	//TSY returned KErrGsmSMSNetworkFailure error to the send request.
    //Get and check event information.
	iSmsStackTestUtils->GetLogEventL(*logEvent,logServerId+1);

	//Check contents of log event
	INFO_PRINTF1(_L("Checking log event of unsuccessful send event.") );
	if((logEvent->Description() != _L("Short message")) ||
	   (logEvent->Direction() != _L("Outgoing")) ||
	   (logEvent->Status() != _L("Not sent")) ||
	   (logEvent->Subject() != _L("Data Message")) ||
	   (logEvent->Number() != sendSmsMessage->ToFromAddress()))
		TEST(KErrArgument);

    CleanupStack::PopAndDestroy(2); //sendSmsMessage, receiveSmsMessage

    //
    // Test event logging when an intermittent concatenated message status report received.
    //
	_LIT(KLongText,"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"The End.");

	//Set destination and SC numbers
	iTelephoneNumber=KRegTestNumber;
	iServiceCenterNumber=KVodafoneSC;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	sendSmsMessage=CreateSmsWithStatusReportReqL(KLongText,alphabet);
	CleanupStack::PushL(sendSmsMessage);

	//Send concatenated message that contains 3 PDU's.
	SendSmsL(sendSmsMessage,socket);

    //Get and check event information.
	iSmsStackTestUtils->GetLogEventL(*logEvent,logServerId+2);

	//Check contents of log event
	INFO_PRINTF1(_L("Checking concatenated sent messages log event before status report received.") );
	if((logEvent->Description() != _L("Short message")) ||
	   (logEvent->Direction() != _L("Outgoing")) ||
	   (logEvent->Status() != _L("Pending")) ||
	   (logEvent->Subject() != _L("3 PDU test SMS message. 3 PDU te")) ||
	   (logEvent->Number() != sendSmsMessage->ToFromAddress()))
		TEST(KErrArgument);

	//Receive SMS-STATUS-REPORT
	WaitForRecvL(socket);
	receiveSmsMessage = RecvSmsL(socket);
	CleanupStack::PushL(receiveSmsMessage);

    //Get and check event information.
	iSmsStackTestUtils->GetLogEventL(*logEvent,receiveSmsMessage->LogServerId());

	//Check contents of log event
	INFO_PRINTF1(_L("Status report has TP-ST value 'Connection rejected by SME'.") );
	INFO_PRINTF1(_L("Checking concatenated sent messages log event again."));
	if((logEvent->Description() != _L("Short message")) ||
	   (logEvent->Direction() != _L("Outgoing")) ||
	   (logEvent->Status() != _L("Failed")) ||
	   (logEvent->Subject() != _L("3 PDU test SMS message. 3 PDU te")) ||
	   (logEvent->Number() != sendSmsMessage->ToFromAddress()))
		TEST(KErrArgument);

    time = logEvent->Time();
    dateTime = time.DateTime();
    
    if((dateTime.Year()   != 2020) ||
       (dateTime.Month()  != 10) ||
       (dateTime.Day()    != 1)  ||
       (dateTime.Hour()   != 13) ||
       (dateTime.Minute() != 38) ||
       (dateTime.Second() != 18) )     
        TEST(KErrArgument); 
	
	CleanupStack::PopAndDestroy(4, &socket); // sendSmsMessage, receiveSmsMessage, logEvent, socket
 	return TestStepResult() ;
	}


TVerdict CTestBearerChange::doTestStepL()
/**
 *  @test Test changing the GPRS bearer while sending and receiving concatenated messages
 *  The idea is to change bearer midway through sending the PDUs making up the complete
 *  message.  Buffer size of 500 => 4 PDU message
 */
    {
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);
    
    INFO_PRINTF1(_L("Testing tx & rx while changing the SMS Bearer"));
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC; //maybe not needed...

	// Create message
    const TInt KTestMessageBufferSize=500;
    HBufC* messageBuffer=HBufC::New(KTestMessageBufferSize);
    CleanupStack::PushL(messageBuffer);
    TPtr bufferPtr=messageBuffer->Des();
    FillDes(bufferPtr,KTestMessageBufferSize);
    CSmsMessage* smsMessage=CreateSmsMessageLC(bufferPtr, TSmsDataCodingScheme::ESmsAlphabet7Bit);

    INFO_PRINTF1(_L("Testing bearer change to GPRS only setting"));

    // Send message & change bearer
    RMobileSmsMessaging::TMobileSmsBearer smsBearer = RMobileSmsMessaging::ESmsBearerPacketOnly;
	iSmsStackTestUtils->SendSmsAndChangeBearerL(smsMessage, socket, smsBearer);

	// Receive message
	CSmsMessage* rxSmsMessage = RecvSmsL(socket);
	CleanupStack::PushL(rxSmsMessage);
	TestSmsContentsL(rxSmsMessage,bufferPtr);
	CleanupStack::PopAndDestroy(rxSmsMessage);  // rxSmsMessage

	INFO_PRINTF1(_L("Testing bearer change to CSD only setting"));

	// Send message & change bearer
	smsBearer = RMobileSmsMessaging::ESmsBearerCircuitOnly;
	iSmsStackTestUtils->SendSmsAndChangeBearerL(smsMessage, socket, smsBearer);

	// Receive message
	rxSmsMessage=RecvSmsL(socket);
	CleanupStack::PushL(rxSmsMessage);
	TestSmsContentsL(rxSmsMessage,bufferPtr);
	CleanupStack::PopAndDestroy(rxSmsMessage);  // rxSmsMessage

	INFO_PRINTF1(_L("Testing bearer change to GPRS preferred setting"));

	// Send message & change bearer
	smsBearer=RMobileSmsMessaging::ESmsBearerPacketPreferred;
	iSmsStackTestUtils->SendSmsAndChangeBearerL(smsMessage, socket, smsBearer);

	// Receive message
	rxSmsMessage=RecvSmsL(socket);
	CleanupStack::PushL(rxSmsMessage);
	TestSmsContentsL(rxSmsMessage,bufferPtr);
	CleanupStack::PopAndDestroy(rxSmsMessage);  // rxSmsMessage

	INFO_PRINTF1(_L("Testing bearer change to CSD preferred setting"));

	// Send message & change bearer
	smsBearer=RMobileSmsMessaging::ESmsBearerCircuitPreferred;
	iSmsStackTestUtils->SendSmsAndChangeBearerL(smsMessage,socket, smsBearer);

	// Receive message
	rxSmsMessage=RecvSmsL(socket);
	CleanupStack::PushL(rxSmsMessage);
	TestSmsContentsL(rxSmsMessage,bufferPtr);
	CleanupStack::PopAndDestroy(rxSmsMessage);  // rxSmsMessage

	CleanupStack::PopAndDestroy(3, &socket); // messageBuffer, smsMessage, CleanupCloseItem
  	return TestStepResult() ;
	}

	
TVerdict CTestRestoreBearer::doTestStepL()
/**
@test Test attempt to set the bearer which returns an error reverts
      the bearer back to the previous setting.
*/
	{
    // Get the initial bearer value from CommDB
    RMobileSmsMessaging::TMobileSmsBearer retrievedBearer;
    RMobileSmsMessaging::TMobileSmsBearer bearer;
    iSmsStackTestUtils->GetBearerL(retrievedBearer);

    RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);
    
    // Attempt to change the bearer
	if (retrievedBearer == RMobileSmsMessaging::ESmsBearerPacketPreferred )
	    {
        bearer = RMobileSmsMessaging::ESmsBearerCircuitPreferred;
 	    }
	else
        {
        bearer = RMobileSmsMessaging::ESmsBearerPacketPreferred;
        }
    INFO_PRINTF2(_L("Changing bearer in CommDB global settings to %d"), bearer);
    iSmsStackTestUtils->ChangeBearerL(bearer);
    
    // Wait 5 seconds for CSmspSetBearer to complete and revert the bearer
    const TInt delay = 5;
    User::After(delay*1000000);
    
    // With simtsy, setting the bearer to any value is not
    // supported, therefore the smsstack should revert 
    // back to the previous bearer setting.
    INFO_PRINTF2(_L("Bearer should be reverted back to previous supported setting after wait for %d seconds"), delay);
    INFO_PRINTF1(_L("The initial supported setting is obtained from CommDB."));
    iSmsStackTestUtils->GetBearerL(bearer);
    
    TESTCHECK(retrievedBearer, bearer, "Checking if the Bearer was reverted back to previous supported setting");
    
	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult();	
	}

TVerdict CTestRecvModeChange::doTestStepL()
/**
 *  @test Test changing the recv mode while sending and receiving concatenated messages
 *  
 */
    {
    INFO_PRINTF1(_L("Testing tx & rx while changing the SMS Receive mode"));
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);
    
	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC; //maybe not needed...

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageL(KTestMsg1,alphabet);

    CleanupStack::PushL(smsMessage);
    CSmsMessage* rxSmsMessage=NULL;

    // Create comms database object
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(db);
    INFO_PRINTF1(_L("Testing recvMode change to EReceiveModeUnspecified"));

    // Send message & wait a sec(!)
    SendSmsL(smsMessage,socket);

    // EReceiveModeUnspecified
    CMDBField<TUint32>* smsReceiveModeField = new(ELeave) CMDBField<TUint32>(KCDTIdSMSReceiveMode);
    CleanupStack::PushL(smsReceiveModeField);
    smsReceiveModeField->SetRecordId(1); //it's GlobalSettingsRecord
    *smsReceiveModeField = RMobileSmsMessaging::EReceiveModeUnspecified;
    smsReceiveModeField->ModifyL(*db);

    // Receive message
    rxSmsMessage=RecvSmsL(socket);
    CleanupStack::PushL(rxSmsMessage);
    TestSmsContentsL(rxSmsMessage,KTestMsg1);
    CleanupStack::PopAndDestroy();  // rxSmsMessage

    INFO_PRINTF1(_L("Testing recvMode change to EReceiveUnstoredPhoneAck"));

    // Send message & wait a sec(!)
    SendSmsL(smsMessage,socket);

    // UnstoredPhoneAck
    *smsReceiveModeField = RMobileSmsMessaging::EReceiveUnstoredPhoneAck;
    smsReceiveModeField->ModifyL(*db);

    // Receive message
    rxSmsMessage=RecvSmsL(socket);
    CleanupStack::PushL(rxSmsMessage);
    TestSmsContentsL(rxSmsMessage,KTestMsg1);
    CleanupStack::PopAndDestroy();  // rxSmsMessage

    INFO_PRINTF1(_L("Testing recv mode change to EReceiveUnstoredClientAck"));

    // Send message & wait a sec(!)
    SendSmsL(smsMessage,socket);

    // UnstoredClientAck
    *smsReceiveModeField = RMobileSmsMessaging::EReceiveUnstoredClientAck;
    smsReceiveModeField->ModifyL(*db);

    // Receive message
    rxSmsMessage=RecvSmsL(socket);
    CleanupStack::PushL(rxSmsMessage);
    TestSmsContentsL(rxSmsMessage,KTestMsg1);
    CleanupStack::PopAndDestroy();  // rxSmsMessage

    INFO_PRINTF1(_L("Testing recv mode change to EReceiveStored"));

    // Send message & wait a sec(!)
    SendSmsL(smsMessage,socket);

    // Stored
    *smsReceiveModeField = RMobileSmsMessaging::EReceiveStored;
    smsReceiveModeField->ModifyL(*db);

    // Receive message
    rxSmsMessage=RecvSmsL(socket);
    CleanupStack::PushL(rxSmsMessage);
    TestSmsContentsL(rxSmsMessage,KTestMsg1);
    CleanupStack::PopAndDestroy();  // rxSmsMessage

    INFO_PRINTF1(_L("Testing recv mode change to EReceiveEither"));

    // Send message & wait a sec(!)
    SendSmsL(smsMessage,socket);

    // Either
    *smsReceiveModeField = RMobileSmsMessaging::EReceiveEither;
    smsReceiveModeField->ModifyL(*db);
    CleanupStack::PopAndDestroy(smsReceiveModeField);

    // Receive message
    rxSmsMessage=RecvSmsL(socket);
    CleanupStack::PushL(rxSmsMessage);
    TestSmsContentsL(rxSmsMessage,KTestMsg1);
    CleanupStack::PopAndDestroy();  // rxSmsMessage

    // Cleanup
    CleanupStack::PopAndDestroy(3); // smsMessage, socket, db

    return TestStepResult() ;
    }

TVerdict CTestTsyCaps::doTestStepL()
/**
 *  Test sms stack when tsy doesn't support anything
 */
	{
	INFO_PRINTF1(_L("Test Sms stack when Tsy supports nothing"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsErrorL(smsMessage,socket);

	//Try to store the message to the SMS store, tsy doesn't support this
	User::After(1000000);

	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);

	INFO_PRINTF1(_L("Write message"));

	TRequestStatus status;

	RSmsSocketWriteStream writestream(socket);
	writestream << *smsMessage;
	writestream.CommitL();

	socket.Ioctl(KIoctlWriteSmsMessage,status,NULL, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Write Sms - returned %d"), status.Int());
	TEST(status.Int() == KErrNotSupported);

	CleanupStack::PopAndDestroy(smsMessage);

	// Try to enumerate messages from Store, tsy doesn't support reading
	INFO_PRINTF1(_L("Enumerating messages"));

	TPckgBuf<TUint> sbuf;
	sbuf()=0;

	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Enumerate store - returned %d"), status.Int());
	TEST(status.Int() == KErrNotSupported);

	// Try to read sms parameters, tsy doesn't support this

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrLocalOperation);
	TInt ret=socket.Bind(smsaddr);
	TEST(ret == KErrNone);

    socket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
    INFO_PRINTF1(_L("waiting for SMS parameters...") );
    User::WaitForRequest(status);
	INFO_PRINTF2(_L("Read Sms parameters - returned %d"), status.Int());
	TEST(status.Int() == KErrNotSupported);

	// Try to store SMS parameters, tsy doesn't support this
	CMobilePhoneSmspList* smspList=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy;

	entryToTsy.iText = DMMTSY_SMSP_STORE_TEXT1;
	entryToTsy.iValidParams = DMMTSY_SMSP_STORE_VALID_PARAMS1;
    entryToTsy.iDestination.iTypeOfNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TON_0;
    entryToTsy.iDestination.iNumberPlan = DMMTSY_SMS_MESSAGE_DESTINATION_NP_0;
	entryToTsy.iDestination.iTelNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_0;
	entryToTsy.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY1;
    entryToTsy.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_1;
    entryToTsy.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_1;
	entryToTsy.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_1;
	entryToTsy.iProtocolId = DMMTSY_SMSP_STORE_PID1;
	entryToTsy.iDcs = DMMTSY_SMSP_STORE_DCS1;

	entryToTsy.iIndex = 0;
	smspList->AddEntryL(entryToTsy);

    INFO_PRINTF1(_L("storing the SMS parameters...") );

	TRAP(ret,writestream << *smspList);
	TEST(ret == KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret == KErrNone);

	socket.Ioctl(KIoctlWriteSmsParams,status,NULL, KSolSmsProv);
 	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Write Sms parameters - returned %d"), status.Int());
	TEST(status.Int() == KErrNotSupported);

	CleanupStack::PopAndDestroy(2, &socket); //socket, smspList
 	return TestStepResult() ;
	}

TVerdict CTestOOMSendSms::doTestStepL()
/**
 *  Test out of memory handling, Send Sms
 */
	{
	INFO_PRINTF1(_L("Test out of memory handling"));
#ifdef _DEBUG

	INFO_PRINTF1(_L("OOM test: Send Sms"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	TInt count=0;
	TInt ret=KErrNoMemory;
	TInt successfullSends=0;  //maximum expected number of succeeded sends is 2 in TSY config file
	while ((ret==KErrNoMemory || ret==KErrEof || count < 18) && (successfullSends<2))
		{
		iSocketServer.__DbgFailNext(count);
		iSocketServer.__DbgMarkHeap();
		TRAP(ret,SendSmsDontCheckReturnValueL(smsMessage,socket));
		iSocketServer.__DbgMarkEnd(0);
		if(ret==KErrNone)
			successfullSends++;
		count++;
		}
	
	TEST(ret == KErrNone);

	iSocketServer.__DbgFailNext(-1); // Reset heap

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage

#endif
	return TestStepResult() ;
	}

TVerdict CTestOOMWriteSms::doTestStepL()
/**
 *  Test out of memory handling, Write Sms to store
 */
	{
	INFO_PRINTF1(_L("OOM test: Write Sms to Store"));
#ifdef _DEBUG
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	// Create and store the message
	_LIT(KStoreMsg1,"HELLO CHRIS");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KStoreMsg1,alphabet);

	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);

	TInt count=0;
	TInt ret=KErrNoMemory;
	while (ret==KErrNoMemory || ret==KErrEof || count < 8)
		{
		iSocketServer.__DbgFailNext(count);
		iSocketServer.__DbgMarkHeap();
		TRAP(ret,WriteSmsLeaveIfErrorL(*smsMessage,socket););
		iSocketServer.__DbgMarkEnd(0);
		count++;
		}

	iSocketServer.__DbgFailNext(-1); // Reset heap
	TEST(ret == KErrNone);
	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
#endif
	return TestStepResult() ;
	}

TVerdict CTestOOMReadSms::doTestStepL()
/**
 *  Test out of memory handling, Read SMS messages
 */
	{
	INFO_PRINTF1(_L("OOM test: Read, SMS store ReadAll not supported."));
#ifdef _DEBUG
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	RSmsSocketReadStream readstream(socket);
	TRequestStatus status;

	CSmsMessage* smsMessage = NULL;

	TInt numberOfMessages(0);
	smsMessage = CreateSmsMessageLC(_L(""),TSmsDataCodingScheme::ESmsAlphabet7Bit);

	TInt count=0;
	TInt ret=KErrNoMemory;
	while (ret==KErrNoMemory || ret==KErrEof || count < 10)
		{
		iSocketServer.__DbgFailNext(count);
		iSocketServer.__DbgMarkHeap();
		TRAP(ret,numberOfMessages=MakeReadSmsStoreRequestL(socket););
		iSocketServer.__DbgMarkEnd(0);
		
		if (ret==KErrNone)
			{
			for (TInt i=0; i<numberOfMessages; ++i)
				{
				TRAPD(ret,readstream >> *smsMessage);
				TEST(ret == KErrNone);
				socket.Ioctl(KIoctlReadMessageSucceeded,status,NULL, KSolSmsProv);
				User::WaitForRequest(status);
				}
			}
		++count;
		}

	CleanupStack::PopAndDestroy(smsMessage);
	TEST(ret == KErrNone);
	iSocketServer.__DbgFailNext(-1); // Reset heap

	INFO_PRINTF2(_L("%d enumerated messages"), numberOfMessages);

	//Check that test client can read all messages from the stream
	for(TInt i=0; i< numberOfMessages; ++i)
		{
		CSmsBuffer* buffer=CSmsBuffer::NewL();
		smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
		CleanupStack::PushL(smsMessage);

		readstream >> *smsMessage;
		socket.Ioctl(KIoctlReadMessageSucceeded, status, NULL, KSolSmsProv);
		User::WaitForRequest(status);
		TEST(status.Int() == KErrNone);

		CleanupStack::PopAndDestroy(smsMessage);
		}

	CleanupStack::PopAndDestroy(&socket);
#endif
	return TestStepResult() ;
	}

TVerdict CTestOOMReadSmsList::doTestStepL()
/**
 *  Test out of memory handling, Read SMS messages from store
 */
	{
	INFO_PRINTF1(_L("OOM test: Read, SMS store ReadAll not supported."));
#ifdef _DEBUG
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	RSmsSocketReadStream readstream(socket);
	TRequestStatus status;

	CSmsMessage* smsMessage = NULL;

	TInt numberOfMessages(0);
	smsMessage = CreateSmsMessageLC(_L(""),TSmsDataCodingScheme::ESmsAlphabet7Bit);

	TInt count=0;
	TInt ret=KErrNoMemory;
	while (ret==KErrNoMemory || ret==KErrEof || count < 10)
		{
		iSocketServer.__DbgFailNext(count);
		iSocketServer.__DbgMarkHeap();
		TRAP(ret,numberOfMessages=MakeReadSmsStoreRequestL(socket););
		iSocketServer.__DbgMarkEnd(0);
		
		if (ret==KErrNone)
			{
			//Check that test client can read all messages from the stream
			for (TInt i=0; i<numberOfMessages; ++i)
				{
				TRAPD(ret,readstream >> *smsMessage);
				TEST(ret == KErrNone);
				socket.Ioctl(KIoctlReadMessageSucceeded,status,NULL, KSolSmsProv);
				User::WaitForRequest(status);
				TEST(status.Int() == KErrNone);
				}
			}
		++count;
		}

	CleanupStack::PopAndDestroy(smsMessage);

	TEST(ret == KErrNone);

	iSocketServer.__DbgFailNext(-1); // Reset heap

	INFO_PRINTF2(_L("%d enumerated messages"), numberOfMessages);

	CleanupStack::PopAndDestroy(&socket);
#endif
	return TestStepResult() ;
	}

TVerdict CTestOOMDeleteSms::doTestStepL()
/**
 *  Test out of memory handling, Delete SMS message
 */
	{
	INFO_PRINTF1(_L("OOM test: Delete SMS message."));
#ifdef _DEBUG
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrLocalOperation);

	// First get one message from store.
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);

	TInt count=0;
	TInt i (0);
	TInt ret=0;
	while (ret==KErrNoMemory || ret==KErrEof || i<messages.Count())
		{
		iSocketServer.__DbgFailNext(count);
		iSocketServer.__DbgMarkHeap();
		TRAP(ret,DeleteSmsLeaveIfErrorL(*messages[i],socket));
		iSocketServer.__DbgMarkEnd(0);
		
		if(ret == KErrNone)
		    {
			//Deleting was successfull. Delete next message.
			++i;
		    }
		else if(ret == KErrNotFound)
		    {
			//SMS Prot has deleted message, but returned KErrNoMemory.
			//When test harness tried to delete same message again, SMS stack
			//returned KErrNotFound.
			TEST(ret == KErrNone);
		    }
		++count;
		}

	iSocketServer.__DbgFailNext(-1); // Reset heap
	messages.ResetAndDestroy();
	ReadSmsStoreL(socket, messages);

	TEST(messages.Count()== 0);
	CleanupStack::PopAndDestroy(2, &socket); // socket, messages
#endif
	return TestStepResult() ;
	}

TVerdict CTestOOMSmsParams::doTestStepL()
/**
 *  Test out of memory handling, Retrieve and store SMS parameters
 */
	{
#ifdef _DEBUG
    INFO_PRINTF1(_L("OOM test: Read and Store Sms params."));
    RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrLocalOperation);

	RSmsSocketReadStream readstream(socket);
	TRequestStatus status;

	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	INFO_PRINTF1(_L("Retrieve SMS parameters."));

	TInt count = 0;
	TInt ret=KErrNoMemory;
	while (ret==KErrNoMemory || ret==KErrEof || count < 8)
		{
		iSocketServer.__DbgFailNext(count);
		iSocketServer.__DbgMarkHeap();		
		TRAP(ret,iSmsStackTestUtils->MakeParametersReadRequestL(socket););
		iSocketServer.__DbgMarkEnd(0);
		count++;
		}
	iSocketServer.__DbgFailNext(-1);	// suppress any lurking heap failure

	TEST(ret == KErrNone);

	// Read list from stream and make acknowledgement to the SMS Stack
	readstream >> *smspList;
	socket.Ioctl(KIoctlCompleteReadSmsParams,status,NULL,KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	CleanupStack::PopAndDestroy(smspList);

	//
	//Store parameters
	//
	smspList=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy;
	entryToTsy.iText = DMMTSY_SMSP_STORE_TEXT1;
	entryToTsy.iValidParams = DMMTSY_SMSP_STORE_VALID_PARAMS1;
	entryToTsy.iDestination.iTypeOfNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TON_0;
	entryToTsy.iDestination.iNumberPlan = DMMTSY_SMS_MESSAGE_DESTINATION_NP_0;
	entryToTsy.iDestination.iTelNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_0;
	entryToTsy.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY1;
	entryToTsy.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_1;
	entryToTsy.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_1;
	entryToTsy.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_1;
	entryToTsy.iProtocolId = DMMTSY_SMSP_STORE_PID1;
	entryToTsy.iDcs = DMMTSY_SMSP_STORE_DCS1;

	// Add entry to the list
	entryToTsy.iIndex = 0;
	smspList->AddEntryL(entryToTsy);

	INFO_PRINTF1(_L("Store SMS parameters."));

	count=0;
	ret=KErrNoMemory;
	while (ret==KErrNoMemory || count < 10)
		{
		iSocketServer.__DbgFailNext(count);
		iSocketServer.__DbgMarkHeap();
		TRAP(ret,iSmsStackTestUtils->StoreParamsLeaveIfErrorL(*smspList,socket););
		iSocketServer.__DbgMarkEnd(0);
		count++;
		}

	TEST(ret == KErrNone);

	iSocketServer.__DbgFailNext(-1); // Reset heap
	//
	// Retrieve SMS parameters again and test the content of params
	//
	//Wait a second, OOM macro is still closing!!
	INFO_PRINTF1(_L("Retrieve parameters again..."));

	//Create the smspList
	CMobilePhoneSmspList* smspList2 = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList2);

	// Make read SMS params request to the SMS Stack.
	socket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
	INFO_PRINTF1(_L("waiting for SMS parameters...") );
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	// Read list from stream and make acknowledgement to the SMS Stack
	readstream >> *smspList2;
	socket.Ioctl(KIoctlCompleteReadSmsParams, status, NULL,KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	ret = iSmsStackTestUtils->TestParameters(*smspList,*smspList2);
	TEST(ret == KErrNone);

	CleanupStack::PopAndDestroy(3, &socket);  //smspList2, smspList, socket
#endif
	return TestStepResult();
	}

//
// Integration test harnesses against SIM tsy
//
TVerdict CTestMeStoreDupAndMiss::doTestStepL()
/**
 *  Test SIM store containing concatenated messages when
 *  some pdu is missing and some pdu is duplicated.
 */
	{
	INFO_PRINTF1(_L("Test the ME store containing duplicated and missing pdus"));
	// Open the socket for SIM operations
	RSocket enumSocket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, enumSocket, ESmsAddrLocalOperation);

	// Enumerate messages
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(enumSocket, messages);

	TEST(messages.Count() == 0);
	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&enumSocket);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	INFO_PRINTF1(_L("waiting for 1st incoming SMS...") );
	WaitForRecvL(socket);
	CSmsMessage* smsMessage = RecvSmsL(socket);
	delete smsMessage;

	INFO_PRINTF1(_L("waiting for 2nd incoming SMS...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
    delete smsMessage;

	INFO_PRINTF1(_L("waiting for 3rd incoming SMS...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
    delete smsMessage;

	INFO_PRINTF1(_L("waiting for 4th incoming SMS...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
    delete smsMessage;

	CleanupStack::PopAndDestroy(&socket);
 	INFO_PRINTF1(_L("Additional waiting SMS protocol to get unloaded"));
	User::After(20000000); // TODO: reviewers - is this needed here?
	return TestStepResult() ;
	}

TVerdict CTestSimStoreDupAndMiss::doTestStepL()
/**
 *  Test ME store containing concatenated messages when
 *  some pdu is missing and some pdu is duplicated.
 */
	{
	INFO_PRINTF1(_L("Test the ME store containing duplicated and missing pdus"));
	// Open the socket for SIM operations
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrLocalOperation);

	// Enumerate messages
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);

	const CSmsMessage* message = messages[1];

	TInt ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	//Try to delete the same message again!
	ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	//Delete concatenated msg that has one pdu duplicated
	message = messages[2];
	ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	//Delete concatenated msg that has one pdu duplicated and pdus are in different order
	message = messages[3];
	ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);

	//Delete concatenated msg that has one pdu missing and pdus are in different order
	message = messages[0];
	ret = DeleteSmsL(*message, socket);
	TEST(ret == KErrNone);
	messages.ResetAndDestroy();

	ReadSmsStoreL(socket, messages);
	TEST(messages.Count() == 4);

	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult() ;
	}

TVerdict CTestRxCphs::doTestStepL()
/**
 *  Test the reception of a CPHS Message Waiting Indication
 */
	{
	INFO_PRINTF1(_L("Test Rx Message Waiting Cphs"));
	// Open the socket for Address Message Indications
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrMessageIndication);

	INFO_PRINTF1(_L("...waiting for incoming Message Waiting Indicator Cphs...") );
	WaitForRecvL(socket);
	CSmsMessage* smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	INFO_PRINTF1(_L("incoming Message Waiting Indicator Cphs") );

	//Ideally we want to test the ton, npi, length and the content of the address byte 3 & 4
	//However, as far as I can see you can't get the npi from the address. (==0?)
	//and we need an example of PDU to get the right address.
	TGsmSmsTelNumber address;
	smsMessage->ParsedToFromAddress(address);

	TEST(( address.iTypeOfAddress.TON() == EGsmSmsTONAlphaNumeric));
	TEST(( address.iTelNumber.Length()==4));
	TEST(((address.iTelNumber[2] & 0x7E) == 0x10));
	TEST(((address.iTelNumber[3] & 0x7E) == 0x00));

	//test for the user data: space and then free-format text
	_LIT(KTestSingleSpace," ");
	TestSmsContentsL(smsMessage,KTestSingleSpace);
	INFO_PRINTF1(_L("Successful Match of Message Waiting Indicator Cphs") );

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
 	return TestStepResult() ;
	}

TVerdict CTestInvalidPDUs::doTestStepL()
/**
 *  Test receiving invalid pdus.
 *  See tsmsprt_config.txt for definition of pdus.
 */
	{
	INFO_PRINTF1(_L("Test receiving invalid pdus"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);
	_LIT(KTestMsg1,"test message, 8bits, length 30");
	_LIT(KTestMsg2,"Voicemail waiting");

	CSmsMessage* smsMessage = NULL;
	TInt i(1);
	//Receive SMSes
	while (i<17)//16 of the PDUs in tsmsprt_config.txt test31
		{       //should be decoded successfully into CSmsMessages
		INFO_PRINTF2(_L("waiting for incoming SMS No. %d..."), i);
		WaitForRecvL(socket);
		smsMessage = RecvSmsL(socket);
		CleanupStack::PushL(smsMessage);

		if (i!=10 && i!= 16)
			PrintMessageL(smsMessage);

		if (i!=4 &&  i!= 10 && i!=12 && i<14)
			TestSmsContentsL(smsMessage,KTestMsg1);
		if (i==12)
			TestSmsContentsL(smsMessage, KTestMsg2);
		CleanupStack::PopAndDestroy(smsMessage);
		++i;
		}

	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult() ;
	}

TVerdict CTestStress::doTestStepL()
/**
 *  Test Transmit and Receive large number of different messages
 */
	{
	INFO_PRINTF1(_L("Test Tx and Rx large number of messages"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	INFO_PRINTF1(_L("Send large number (20) of messages"));

	TInt i(0);
	while (i<20)
		{
		SendSmsL(smsMessage,socket);
		++i;
		}

	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("Receive large number (20) of messages"));
	while (i!=0)
		{
		WaitForRecvL(socket);
		smsMessage = RecvSmsL(socket);
		CleanupStack::PushL(smsMessage);

		TestSmsContentsL(smsMessage,KTestMsg1);
		CleanupStack::PopAndDestroy(smsMessage);
		--i;
		}

	_LIT(KTest7bitMsg,"test message, length 23"); //7 bits test message, length 23 characters

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KSoneraSC;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	smsMessage=CreateSmsMessageLC(KTest7bitMsg,alphabet);

	//Set status report request
	CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
	submitPdu.SetStatusReportRequest(ETrue);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	INFO_PRINTF1(_L("Send large number (40) of messages"));
	while (i<40)
		{
		SendSmsL(smsMessage,socket);
		++i;
		}
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive status report
	TSmsServiceCenterAddress telephoneNumber(KPekka);

	INFO_PRINTF1(_L("Receive large number (40) of messages"));
	while (i!=0)
		{
		WaitForRecvL(socket);
		smsMessage = RecvSmsL(socket);
		CleanupStack::PushL(smsMessage);
		
		TestSmsContentsL(smsMessage,KTest7bitMsg);
		CleanupStack::PopAndDestroy(smsMessage);
		--i;
		//Receive status report
		RecvStatusReportL(telephoneNumber, socket);
		}

	//Tx and rx concatenated message & status report
	_LIT(KLongText,"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"The End.");

	//Set destination and SC numbers
	iTelephoneNumber=KRegTestNumber;
	iServiceCenterNumber=KVodafoneSC;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;//changed to 7 bits
	smsMessage=CreateSmsWithStatusReportReqL(KLongText,alphabet);

	CleanupStack::PushL(smsMessage);
	INFO_PRINTF1(_L("Send concatenated message"));
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("Recv concatenated message and status report"));
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	TestSmsContentsL(smsMessage,KLongText);
	CleanupStack::PopAndDestroy(smsMessage);

	telephoneNumber.Copy( KRegTestNumber );
	RecvStatusReportL(telephoneNumber, socket);

	//Tx and rx longer concatenated message & status report
	_LIT(KLongText2,"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"7 PDU test SMS message. "
	        L"The End.");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KVodafoneSC;

	alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	smsMessage=CreateSmsWithStatusReportReqL(KLongText2,alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF1(_L("Send concatenated message"));
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("Recv concatenated message and status report"));
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	TestSmsContentsL(smsMessage,KLongText2);

	telephoneNumber.Copy( KPekka );
	RecvStatusReportL(telephoneNumber, socket);

    //
    // Validate that log entry has been updated with the time the last 
    // SMS PDU was actually received by the destination mobile, ie the discharge time contained in the last
    // Status Report which is received. The discharge time for the last PDU has been set to be in 
    // 2021 - the discharge time of the previous PDUs has been set to 2020. Choosing dates which
    // are well in the future ensures that the SMS Status Reports do not have time stamps
    // which are earlier than the time that the corresponding SMS Submit PDUs were sent by
    // the originator- this would result in the event log entry being purged from the logging system.
    // 
    
    CLogEvent* logEvent=CLogEvent::NewL();
    CleanupStack::PushL(logEvent);
        
    //Status report received. Get sent message's log event again.
    //Status report's LogServerId should be same as submit's LogServerId
    iSmsStackTestUtils->GetLogEventL(*logEvent,smsMessage->LogServerId()-1);

    //Check contents of log event
    INFO_PRINTF1(_L("Checking sent messages log event after status report received.") );
    if((logEvent->Description() != _L("Short message")) ||
       (logEvent->Direction() != _L("Outgoing")) ||
       (logEvent->Status() != _L("Delivered")) ||
       (logEvent->Subject() != _L("Data Message")) )
        TEST(KErrArgument);

    TTime time = logEvent->Time();
    TDateTime dateTime = time.DateTime();
    
    if ((dateTime.Year() != 2021 ) ||
        (dateTime.Month() != 11)   ||
        (dateTime.Day() != 1)      ||
        (dateTime.Hour() != 13)    ||
        (dateTime.Minute() != 38   ||
        (dateTime.Second() != 18)))
        TEST(KErrArgument);         
    
    CleanupStack::PopAndDestroy(logEvent);
    CleanupStack::PopAndDestroy(smsMessage);        
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestCombinedStore::doTestStepL()
/**
 *  Test combined store.
 */
	{
	INFO_PRINTF1(_L("Test Combined SMS Store"));
	// Open the socket for SIM operations
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrLocalOperation);

	// Enumerate messages. Concatenated message with constituent PDUs is stored to the SIM, Phone store and
	// Combined store. Also one normal single part message is stored to Combined store.
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	TEST(messages.Count()==3);

	CSmsMessage::TMobileSmsStorage storage = messages[0]->Storage();
	TEST(storage==CSmsMessage::ESmsSIMStorage);
	storage = messages[1]->Storage();
	TEST(storage==CSmsMessage::ESmsCombinedStorage);

	// Delete messages
	TInt ret = DeleteSmsL(*messages[0], socket);
	TEST(ret == KErrNone);
	ret = DeleteSmsL(*messages[2], socket);
	TEST(ret == KErrNone);

	ret = DeleteSmsL(*messages[1], socket);
	TEST(ret == KErrNone);
	messages.ResetAndDestroy();

	// Read messages and check that message store is empty
	ReadSmsStoreL(socket, messages);
	TEST(messages.Count()==0);
	messages.ResetAndDestroy();

	// Create and write message to the Combined store
	_LIT(KStoreMsg1,"Store MSG to combined store");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsmessage1=CreateSmsMessageLC(KStoreMsg1,alphabet);
	smsmessage1->SetStorage(CSmsMessage::ESmsCombinedStorage);
	WriteSmsToSimL(*smsmessage1, socket);

	// Create and write concatenated message to the Combined store
	_LIT(KStoreMsg2,"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"3 PDU test SMS message. "
	        L"The End.");
	CSmsMessage* smsmessage2=CreateSmsMessageLC(KStoreMsg2,alphabet);
	smsmessage2->SetStorage(CSmsMessage::ESmsCombinedStorage);
	WriteSmsToSimL(*smsmessage2, socket);

	// Read all message and check store info
	ReadSmsStoreL(socket, messages);
	TEST(messages.Count()==2);

	storage = messages[0]->Storage();
	TEST(storage==CSmsMessage::ESmsCombinedStorage);
	storage = messages[1]->Storage();
	TEST(storage==CSmsMessage::ESmsCombinedStorage);

	// Delete both messages
	ret = DeleteSmsL(*messages[0], socket);
	TEST(ret == KErrNone);
	ret = DeleteSmsL(*messages[1], socket);
	TEST(ret == KErrNone);
	messages.ResetAndDestroy();

	// Read messages and check that message store is empty
	ReadSmsStoreL(socket, messages);
	TEST(messages.Count()==0);

	CleanupStack::PopAndDestroy(4, &socket);	//socket, messages, smsmessage1, smsmessage2
    return TestStepResult() ;
	}

TVerdict CTestParamsInter::doTestStepL()
/**
 *  Test retrieving and storing the sms parameters.
 *  Data tests: GT83-T_SmsInter-015 ... GT83-T_SmsInter-022
 */
	{
	INFO_PRINTF1(_L("Parameter storage data testing"));
	TInt ret(KErrNone);
    TRequestStatus status;

    //
    // Retrieve SMS parameters
    //
	// Open the socket for SIM operations
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrLocalOperation);

    RSmsSocketReadStream readstream(socket);

	//Create the smspList
	CMobilePhoneSmspList* smspListOriginal = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspListOriginal);

	INFO_PRINTF1(_L("waiting 2 secs for smsprot to load"));
	User::After(2000000);

    // Make read SMS params request to the SMS Stack.
    socket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
    INFO_PRINTF1(_L("waiting for SMS parameters...") );
    User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

    // Read list from stream and make acknowledgement to the SMS Stack
	// Store parameters to return them to SIM at the end of the test
    readstream >> *smspListOriginal;
    socket.Ioctl(KIoctlCompleteReadSmsParams, status, NULL,KSolSmsProv);
    User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	//
	// Store SMS parameters
	//
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy;

	entryToTsy.iIndex = 0;
	entryToTsy.iText = _L("Test SMS address 1");
	entryToTsy.iValidParams = RMobileSmsMessaging::KSCAIncluded;
    entryToTsy.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_1;
    entryToTsy.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_1;
	entryToTsy.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_1;

    smspList->AddEntryL(entryToTsy);

	entryToTsy.iIndex = 1;
	entryToTsy.iText = _L("Test SMS address 2");
	entryToTsy.iValidParams = RMobileSmsMessaging::KSCAIncluded | RMobileSmsMessaging::KDestinationIncluded;
    entryToTsy.iDestination.iTypeOfNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TON_0;
    entryToTsy.iDestination.iNumberPlan = DMMTSY_SMS_MESSAGE_DESTINATION_NP_0;
	entryToTsy.iDestination.iTelNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_0;
    entryToTsy.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_2;
    entryToTsy.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_2;
	entryToTsy.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_2;

    smspList->AddEntryL(entryToTsy);

	entryToTsy.iIndex = 2;
	entryToTsy.iText = _L("Test SMS address 3");
	entryToTsy.iValidParams = RMobileSmsMessaging::KSCAIncluded | RMobileSmsMessaging::KDestinationIncluded | RMobileSmsMessaging::KValidityPeriodIncluded;
    entryToTsy.iDestination.iTypeOfNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TON_1;
    entryToTsy.iDestination.iNumberPlan = DMMTSY_SMS_MESSAGE_DESTINATION_NP_1;
	entryToTsy.iDestination.iTelNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_1;
	entryToTsy.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY1;
    entryToTsy.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_3;
    entryToTsy.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_3;
	entryToTsy.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_3;

    smspList->AddEntryL(entryToTsy);

	entryToTsy.iIndex = 3;
	entryToTsy.iText = _L("Test SMS address 4");
	entryToTsy.iValidParams = RMobileSmsMessaging::KSCAIncluded | RMobileSmsMessaging::KDestinationIncluded | RMobileSmsMessaging::KValidityPeriodIncluded | RMobileSmsMessaging::KProtocolIdIncluded;
    entryToTsy.iDestination.iTypeOfNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TON_2;
    entryToTsy.iDestination.iNumberPlan = DMMTSY_SMS_MESSAGE_DESTINATION_NP_2;
	entryToTsy.iDestination.iTelNumber = DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_2;
	entryToTsy.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY1;
    entryToTsy.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_4;
    entryToTsy.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_4;
	entryToTsy.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_4;
	entryToTsy.iProtocolId = DMMTSY_SMSP_STORE_PID1;
	entryToTsy.iDcs = DMMTSY_SMSP_STORE_DCS1;

    smspList->AddEntryL(entryToTsy);

	entryToTsy.iIndex = 4;
	entryToTsy.iText = _L("Test SMS address 5");
	entryToTsy.iValidParams = RMobileSmsMessaging::KSCAIncluded | RMobileSmsMessaging::KValidityPeriodIncluded | RMobileSmsMessaging::KProtocolIdIncluded | RMobileSmsMessaging::KDcsIncluded;
	entryToTsy.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY1;
    entryToTsy.iServiceCentre.iTypeOfNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_5;
    entryToTsy.iServiceCentre.iNumberPlan = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_5;
	entryToTsy.iServiceCentre.iTelNumber = DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_5;
	entryToTsy.iProtocolId = DMMTSY_SMSP_STORE_PID1;
	entryToTsy.iDcs = DMMTSY_SMSP_STORE_DCS1;

    smspList->AddEntryL(entryToTsy);

    INFO_PRINTF1(_L("storing the SMS parameters...") );

    // Write parameter list to the stream and make write SMS parameters request to the SMS Stack.
    ret = iSmsStackTestUtils->StoreParamsL(*smspList,socket,EFalse);
	TEST(ret == KErrNone);

	//
	// Retrieve SMS parameters again
	//
	CMobilePhoneSmspList* smspList2=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList2);

    // Now read SMS parameters
    socket.Ioctl(KIoctlReadSmsParams,status,NULL,KSolSmsProv);
    INFO_PRINTF1(_L("waiting for SMS parameters...") );
    User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

    // Read list from stream and make acknowledgement to the SMS Stack
    readstream >> *smspList2;
    socket.Ioctl(KIoctlCompleteReadSmsParams,status,NULL, KSolSmsProv);
    User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	ret = iSmsStackTestUtils->TestParameters(*smspList,*smspList2);
	TEST(ret == KErrNone);

	CleanupStack::PopAndDestroy(2);	//smspList,smspList2

	//
	// Store original SMS parameters to the SIM
	//

    INFO_PRINTF1(_L("storing the original SMS parameters...") );

    // Make Write SMS Parameters request
    ret = iSmsStackTestUtils->StoreParamsL(*smspListOriginal,socket,EFalse);
	TEST(ret == KErrNone);

	CleanupStack::PopAndDestroy(2, &socket); // socket,smspListOriginal
 	return TestStepResult() ;
	}

TVerdict CTestTxRxAlphanumeric::doTestStepL()
/**
 *  Test a Tx And Rx of messages with alphanumeric dest/orig addresses
 */
	{
	INFO_PRINTF1(_L("Test Tx And Rx SMSes with Alphanumeric originator address"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* txSms=NULL;
	CSmsMessage* rxSms=NULL;
	TGsmSmsTelNumber gsmSmsTelNumber;
	TInt i(0);

	//Set destination and SC numbers
	RArray<TSmsServiceCenterAddress> destinationNumbers;
	CleanupClosePushL(destinationNumbers);

	destinationNumbers.Append(_L("TooLongAddress")); //not delivered
	destinationNumbers.Append(_L("SONERA"));
	destinationNumbers.Append(_L("S\xD6NER\xC4"));
	destinationNumbers.Append(_L("\xD1\xC5\xE6\xA7\xA3\xA5\xE9\xE7\xDF\xDC\x40"));
	destinationNumbers.Append(_L("\x40\x20\xE0\xC9\xF1\xD8\xF8\xBF\xA4"));
	destinationNumbers.Append(_L("@")); //=0x00
	destinationNumbers.Append(_L(" ")); //=0x20
	destinationNumbers.Append(_L(""));

	while (i<destinationNumbers.Count())
		{
		iTelephoneNumber=destinationNumbers[i];

		txSms=CreateSmsMessageLC(KTestMsg1,alphabet);

		gsmSmsTelNumber.iTelNumber.Copy(iTelephoneNumber);

		// Type of number is EGsmSmsTONAlphaNumeric
		gsmSmsTelNumber.iTypeOfAddress.SetTON(EGsmSmsTONAlphaNumeric);
		txSms->SetParsedToFromAddressL(gsmSmsTelNumber);
		INFO_PRINTF1(_L("Send SMS message. Type of number is EGsmSmsTONAlphaNumeric") );
		INFO_PRINTF2(_L("Destination number:...[%S]"),&iTelephoneNumber);

		//Send SMS
		if(i!=0)
		    {
            SendSmsL(txSms,socket);
		    }
		else
		    {
			SendSmsErrorL(txSms,socket);
		    }
		
		if(i!=0)
			{
			INFO_PRINTF2(_L("waiting for incoming SMS No. %d..."), i);
			WaitForRecvL(socket);
			rxSms = RecvSmsL(socket);
			CleanupStack::PushL(rxSms);

			TestSmsContentsL(rxSms,KTestMsg1);

			INFO_PRINTF1(_L("Originator address:...["));
			Logger().Write(rxSms->ToFromAddress());
			INFO_PRINTF1(_L("]"));

			TEST(rxSms->ToFromAddress() == txSms->ToFromAddress());
			INFO_PRINTF1(_L("Orig address matches to destination address!!"));
			INFO_PRINTF1(_L("ServiceCenter number:...["));
			Logger().Write(rxSms->ServiceCenterAddress());
			INFO_PRINTF1(_L("]"));

			CleanupStack::PopAndDestroy(rxSms);
			}
		CleanupStack::PopAndDestroy(txSms);
		++i;
		}

	CleanupStack::PopAndDestroy(2, &socket); //socket, destinationNumbers
 	return TestStepResult() ;
	}

TVerdict CTestRxSuspend::doTestStepL()
/**
 *  Test suspend receiving messages
 */
	{
	const TInt testNumber = 36;
	const TInt smsRxCount = KDefaultMaxmumNumberOfCompleteMessagesInReassemblyStore + 1; // Number of messages that will cause a suspend

	//Add smsRxCount SmsRx tags to c:\config.txt, taking KTSmsPrtConfigFileName as input
	CTestConfig* configFile = CTestConfig::NewLC(iFs, KGmsSmsConfigFileDir, KTSmsPrtConfigFileName);

	TInt i(0);
	for (i = 0; i < smsRxCount; ++i)
		{
		_LIT(KTestRxSuspendMessage, "TestRxSuspendMessage %d");
		TBuf<32> buf;
		buf.AppendFormat(KTestRxSuspendMessage, i);
		CSmsMessage* message = iSmsStackTestUtils->CreateSmsMessageLC(CSmsPDU::ESmsDeliver, iTelephoneNumber, buf);

		iSmsStackTestUtils->AddSmsRxL(*configFile, testNumber, *message, iServiceCenterNumber);

		CleanupStack::PopAndDestroy(message);
		}

	//Write the configFile to c:\config.txt
	configFile->WriteFileL(KSimTsyConfigFileName);

	CTestConfigSection& section = iSmsStackTestUtils->GetSectionL(*configFile, testNumber);
	_LIT8(KTestRxSuspendSmsRxPeriod, "SmsRxPeriod");
	const TInt rxPeriod = section.ItemValue(KTestRxSuspendSmsRxPeriod, 5) + 1;

	CleanupStack::PopAndDestroy(configFile);

	// Set the test number. SIMTSY will re-configure itself
	SetSimTSYTestNumberL(testNumber);
	
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	INFO_PRINTF1(_L("Test suspend receiving messages"));

	CSmsMessage* smsMessage;
	for (i = 0; i < smsRxCount; ++i)
		{
		INFO_PRINTF2(_L("%d Waiting for incoming SMS..."), i);
		WaitForRecvL(socket);
		smsMessage = RecvSmsL(socket, KIoctlReadMessageFailed);
		CleanupStack::PushL(smsMessage);

		PrintMessageL(smsMessage);
		CleanupStack::PopAndDestroy(smsMessage);

		INFO_PRINTF3(_L("%d Waiting %d secs for next message"), i, rxPeriod);
		User::After(rxPeriod * 1000000);
		}

	for (i = 0; i < smsRxCount; ++i)
		{
		INFO_PRINTF2(_L("%d Waiting for incoming SMS..."), i);
		WaitForRecvL(socket);
		smsMessage = RecvSmsL(socket, KIoctlReadMessageSucceeded);
		CleanupStack::PushL(smsMessage);

		PrintMessageL(smsMessage);
        CleanupStack::PopAndDestroy(smsMessage);
		}

	INFO_PRINTF2(_L("Waiting %d secs to see if other (unexpected) message received"), rxPeriod * 3);
	const TBool otherReceived = TimedWaitForRecvL(socket, rxPeriod * 3 * 1000000);

	if (otherReceived)
		{
		INFO_PRINTF1(_L("Unexpected message received:"));
		smsMessage = RecvSmsL(socket, KIoctlReadMessageSucceeded);
		CleanupStack::PushL(smsMessage);
		PrintMessageL(smsMessage);
        CleanupStack::PopAndDestroy(smsMessage);
		}

	TEST(!otherReceived);

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestEnumerateCorrupted::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	// Enumerate messages
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);

	TInt numberOfMess=messages.Count();
	TEST(numberOfMess==2);//the 3rd message is corrupted and therefore ignored

	messages.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&socket);
  	return TestStepResult() ;
	}

TVerdict CTestCancelling::doTestStepL()
	{
	/*This test harness has been created to reproduce  HOE-573FF6 that has been closed
	This test shows that even if the cancelling operation return KErrCancel, the request is
	still processed
	note: the message count will keep increasing as the socket is not emptied
	*/
	INFO_PRINTF1(_L("Test Cancelling requests"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	RSocket socket2;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket2,ESmsAddrLocalOperation);
	RSmsSocketWriteStream writestream(socket);

	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);

	_LIT(KStoreMsg1,"SIM TEST MESSAGE 1");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* smsmessage=CreateSmsMessageLC(KStoreMsg1,alphabet);
	smsmessage->SetStorage(CSmsMessage::ESmsSIMStorage);

	TPckgBuf<TUint> sbuf;
	TRequestStatus status;

	INFO_PRINTF1(_L("testing cancel send message"));

	writestream << *smsmessage;
	writestream.CommitL();

	socket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
	INFO_PRINTF1(_L("Cancel send request"));
	socket.CancelIoctl();
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrCancel, "Cancel send request");
	//need to check here if the message has actually been sent

	INFO_PRINTF1(_L("testing cancel Delete message"));

	ReadSmsStoreL(socket, messages);
	const CSmsMessage* message = messages[0];
	INFO_PRINTF1(_L("Delete message"));

	writestream << *message;
	writestream.CommitL();

	socket.Ioctl(KIoctlDeleteSmsMessage,status,NULL,KSolSmsProv);
	INFO_PRINTF1(_L("Cancel"));
	User::After(300000);
	socket.CancelIoctl();
	User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrCancel, "Cancel delete request");

	INFO_PRINTF1(_L("testing cancel write message"));

	writestream << *smsmessage;
	writestream.CommitL();
	TInt ret;
	ret = CancelWriteSmsToSimL(*smsmessage, socket, 100000);
	TEST(ret==KErrCancel);

	CleanupStack::PopAndDestroy(smsmessage);
	messages.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&socket2);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestSendAndRecvMsgsWithDifferentTON::doTestStepL()
/**
 *  @test Send and receive messages with different type of number
 */
	{
    // TODO: replace the following two lines with 
    // in-script commands(copy file, ParseSettingsFromFileL is called from preample) 
	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigExtraFileName);
	ParseSettingsFromFileL();
	
	TBool found( ETrue );
	TInt count( 0 );
	
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendAndRecvMsgsWithDifferentTON, count++ );
		}
	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigFileName);
	return TestStepResult() ;
	}

TVerdict CTestSendAndReceiveIndicatorMsgs::doTestStepL()
	{
	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigExtraFileName);
	ParseSettingsFromFileL();
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendAndReceiveIndicatorMsgs, count++ );
		}
	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigFileName);
	return TestStepResult() ;
	}

TVerdict CTestSendRecvMaxLengthMsg::doTestStepL()
    {
    INFO_PRINTF1(_L("Sending max length SMS can take a while ... get a cup of coffee or two :-)"));
	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigTxRxMaxMsgFileName);
	ParseSettingsFromFileL();
	
	const TInt testNumber = 42;
	SetSimTSYTestNumberL(testNumber);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	HBufC* textBuf=HBufC::NewL(39015);
	CleanupStack::PushL(textBuf);
	TPtr textPtr=textBuf->Des();
	TInt i;
	for(i=0; i<255; ++i)
		{
		textPtr.Append(KText);
		}

	SendAndRecvSms7BitL(textPtr,socket);
	CleanupStack::PopAndDestroy(2, &socket); // socket, textBuf

	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigFileName);
	return TestStepResult() ;
    }

TVerdict CTestSendAndRecvMsgsWithDifferentPID::doTestStepL()
/**
 *  @test Send and receive messages with different type of protocol identifier
 */
    {
	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigExtraFileName);
	ParseSettingsFromFileL();

    INFO_PRINTF1(_L("Send and receive messages with different type of TP-PID"));
    
    const TInt testNumber = 41;
    SetSimTSYTestNumberL(testNumber);

	RSocket sendSocket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,sendSocket,ESmsAddrSendOnly);

	_LIT(KTestMsg,"Test message.");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage=CreateSmsMessageLC(KTestMsg,alphabet);
    CSmsSubmit& submitPdu=(CSmsSubmit&)sendMessage->SmsPDU();

    // TP-PID is telex (or teletex reduced to telex format)
	submitPdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);
	submitPdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
	submitPdu.SetTelematicDeviceType(TSmsProtocolIdentifier::ESmsTelex);
	INFO_PRINTF1(_L("Send SMS message. TP-PID is telex (or teletex reduced to telex format)") );
	SendSmsL(sendMessage,sendSocket);

	RSocket recvSocket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,recvSocket,ESmsAddrRecvAny);

	WaitForRecvL(recvSocket);
	CSmsMessage* recvMessage = RecvSmsL(recvSocket);
	CleanupStack::PushL(recvMessage);
	CSmsDeliver& deliverPdu1=(CSmsDeliver&)recvMessage->SmsPDU();

	TestSmsContentsL(recvMessage,KTestMsg);
	TEST(deliverPdu1.PIDType() == submitPdu.PIDType());
	TEST(deliverPdu1.TelematicDeviceIndicator() == submitPdu.TelematicDeviceIndicator());
	TEST(deliverPdu1.TelematicDeviceType() == submitPdu.TelematicDeviceType());

	CleanupStack::PopAndDestroy(2, &recvSocket); //recvSocket, recvMessage

	// TP-PID is Replace Short Message Type 1
	submitPdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDShortMessageType);
	submitPdu.SetShortMessageType(TSmsProtocolIdentifier::ESmsReplaceShortMessageType1);
	INFO_PRINTF1(_L("Send SMS message. TP-PID is Replace Short Message Type 1") );
	SendSmsL(sendMessage,sendSocket);

	INFO_PRINTF1(_L("Send another SMS message. TP-PID is Replace Short Message Type 1") );
	SendSmsL(sendMessage,sendSocket);

	INFO_PRINTF1(_L("Open ESmsAddrRecvAny socket and receive messages") );
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,recvSocket,ESmsAddrRecvAny);

	INFO_PRINTF1(_L("Waiting for incoming SMS...") );
	WaitForRecvL(recvSocket);
	recvMessage = RecvSmsL(recvSocket);
	CleanupStack::PushL(recvMessage);
	CSmsDeliver& deliverPdu2=(CSmsDeliver&)recvMessage->SmsPDU();

	TestSmsContentsL(recvMessage,KTestMsg);
	TEST(deliverPdu2.PIDType() == submitPdu.PIDType());
	TEST(deliverPdu2.ShortMessageType() == submitPdu.ShortMessageType());
	CleanupStack::PopAndDestroy(recvMessage);

	/*If Replace Short Message code is present, then the SMS Stack should check the
	originating address and replace any existing stored message having the same
	Protocol Identifier code and originating address with the new short message and
	other parameter values. Anyway this is optional feature in 23040 standard.*/
	/*INFO_PRINTF1(_L("If SMS Stack supports optional Replace Short Message feature (23040/9.2.3.9)") );
	INFO_PRINTF1(_L("it shouldn't deliver same message again to the test harness.") );
	INFO_PRINTF1(_L("Test was successful if test harness stucks here.") );
	INFO_PRINTF1(_L("Waiting for incoming SMS...") );*/

	/*I beleive the above is wrong, the sms stack does not care about the PID and just passes the message
	it is up to the applicatio to do that check; therefore the 2 replace messages are received -Pierre*/

	WaitForRecvL(recvSocket);
	recvMessage = RecvSmsL(recvSocket);
	CleanupStack::PushL(recvMessage);
	CSmsDeliver& deliverPdu3=(CSmsDeliver&)recvMessage->SmsPDU();

	TestSmsContentsL(recvMessage,KTestMsg);
	TEST(deliverPdu3.PIDType() == submitPdu.PIDType());
	TEST(deliverPdu3.ShortMessageType() == submitPdu.ShortMessageType());
	CleanupStack::PopAndDestroy(recvMessage);

	CleanupStack::PopAndDestroy(3, &sendSocket);	// sendSocket,sendMessage,recvSocket
    iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigFileName);
	return TestStepResult() ;
	}

TVerdict CTestSmsParamsErrorCases::doTestStepL()
 /**
  *  Test SMS parameters while error returned from TSY.
  *  Manual function moved from t_sms_inter_auto test suite.
  */
	{
	INFO_PRINTF1(_L("Test SMS prameters while error returned from TSY"));
	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigExtraFileName);
	
	const TInt testNumber = 43;         // or any number it reaches
	CTestConfig* configFile = CTestConfig::NewLC(iFs, KGmsSmsConfigFileDir, KTSmsPrtConfigExtraFileName);
	
	CTestConfigSection& section = iSmsStackTestUtils->GetSectionL(*configFile, testNumber);
	TInt param = section.ItemValue(KSmspEntryValidParams, 2);
	
	RMobileSmsMessaging::TMobileSmspStoreValidParams smspEntryValidParams = ( RMobileSmsMessaging::TMobileSmspStoreValidParams )param;
	param = section.ItemValue(KSmspEntryScTypeOfNumber, 1);
	
	RMobilePhone::TMobileTON smspEntryScTypeOfNumber = ( RMobilePhone::TMobileTON )param;
	param = section.ItemValue(KSmspEntryScNumberPlan, 1);
	
	RMobilePhone::TMobileNPI smspEntryScNumberPlan = ( RMobilePhone::TMobileNPI )param;
	const TDesC8& smspEntryScTelNumber = section.ItemValue(KServiceCentreAddress, (_L8("+358508771010")));

	TestSmsParamsErrorCasesL(smspEntryValidParams, smspEntryScTypeOfNumber, smspEntryScNumberPlan, smspEntryScTelNumber);
	
	CleanupStack::PopAndDestroy(configFile);
	iSmsStackTestUtils->CopyConfigFileL(KTSmsPrtConfigFileName);
	return TestStepResult() ;
	}

void CTestSmsParamsErrorCases::TestSmsParamsErrorCasesL(
		RMobileSmsMessaging::TMobileSmspStoreValidParams aSmspEntryValidParams,
		RMobilePhone::TMobileTON aSmspEntryScTypeOfNumber,
		RMobilePhone::TMobileNPI aSmspEntryScNumberPlan,
		const TDesC8& aSmspEntryScTelNumber)
/**
 *  TSY returns error to retrieve and store requests
 */
    {
    INFO_PRINTF1(_L("TSY returns error to retrieve and store SMSP list requests"));
    
    const TInt testNumber = 43;
    SetSimTSYTestNumberL(testNumber);
    
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrLocalOperation);

	RSmsSocketReadStream readstream(socket);
	TRequestStatus status;

    TBool tryAgain = ETrue;
    TInt attempts (0);
    TInt maxRetries = 3;
	
    //
	//Retrieve SMSP list
	//
	
    // Make read SMS params request to the SMS Stack. If it fails with KErrNotReady wait for 3 seconds to 
    // allow sms protocol to fully load and repeat
    INFO_PRINTF1(_L("Waiting for SMS parameters...") );
    while( tryAgain && attempts++ < maxRetries )
        {
        socket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
        User::WaitForRequest(status);
        INFO_PRINTF2(_L("Read parameters returned %d"), status.Int());
        
        if ( status.Int() == KErrNotReady )
            {
            INFO_PRINTF1(_L("Trying again... "));
            User::After(3000000);
            }
        else
            {
            tryAgain = EFalse;
            }
        }
    
	if(status.Int() == KErrNone)
		{
		CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
		CleanupStack::PushL(smspList);
		// Read list from stream and make acknowledgement to the SMS Stack
		readstream >> *smspList;
		socket.Ioctl(KIoctlCompleteReadSmsParams, status, NULL,KSolSmsProv);
		User::WaitForRequest(status);
		CleanupStack::PopAndDestroy(smspList);
		}
	TEST(status.Int() == KErrNone);

	//
	//Store SMSP list
	//
	// Create the smspList
    CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspList);

	// Because of AT-command limitations we can store only SC address to the phone.
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
	smspEntry.iIndex=0;
    smspEntry.iValidParams = aSmspEntryValidParams;
    smspEntry.iServiceCentre.iTypeOfNumber = aSmspEntryScTypeOfNumber;
    smspEntry.iServiceCentre.iNumberPlan = aSmspEntryScNumberPlan;
	smspEntry.iServiceCentre.iTelNumber.Copy(aSmspEntryScTelNumber);
	smspList->AddEntryL(smspEntry);

	INFO_PRINTF1(_L("writing SMS parameters..."));
	TInt ret = iSmsStackTestUtils->StoreParamsL(*smspList,socket,EFalse);
	TEST(ret == KErrNone);

	CleanupStack::PopAndDestroy(smspList);
    CleanupStack::PopAndDestroy(&socket);
	}

TVerdict CTestResendFailedConcatinatedMessage::doTestStepL()
/**
 *  Test that sent PDUs in failed concatenated message are not re-sent"
 */
	{
	INFO_PRINTF1(_L(" Test that sent PDUs in failed concatenated message are not re-sent"));
	TLogId logid;
	TLogSmsPduData pduData;
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	ChangeReceiveModeL(RMobileSmsMessaging::EReceiveUnstoredClientAck);

	_LIT(KTestMsg1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBCCCCCCC");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS	- first pdu fails
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
	pduData.iType=smsMessage->Type();
	logid=iSmsStackTestUtils->AddLogEventL(*smsMessage,pduData);
	smsMessage->SetLogServerId(logid);
	
	INFO_PRINTF2(_L("Sends message with log id %d"),logid);
	SendSmsErrorL(smsMessage,socket);
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);
	
	//Receive SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg1);
	CleanupStack::PopAndDestroy(smsMessage);

	//Send SMS	- second pdu fails
	smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
	pduData.iType=smsMessage->Type();
	logid=iSmsStackTestUtils->AddLogEventL(*smsMessage,pduData);
	smsMessage->SetLogServerId(logid);
	
	INFO_PRINTF2(_L("Sends message with log id %d"),logid);
	SendSmsErrorL(smsMessage,socket);
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KTestMsg1);
	CleanupStack::PopAndDestroy(smsMessage);

	//Send SMS	- third pdu fails
	smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
	pduData.iType=smsMessage->Type();
	logid=iSmsStackTestUtils->AddLogEventL(*smsMessage,pduData);
	smsMessage->SetLogServerId(logid);
	
	INFO_PRINTF2(_L("Sends message with log id %d"),logid);
	SendSmsErrorL(smsMessage,socket);
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KTestMsg1);
	CleanupStack::PopAndDestroy(smsMessage);

	//Send SMS	- fourth pdu fails
	smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
	pduData.iType=smsMessage->Type();
	logid=iSmsStackTestUtils->AddLogEventL(*smsMessage,pduData);
	smsMessage->SetLogServerId(logid);
	
	INFO_PRINTF2(_L("Sends message with log id %d"),logid);
	SendSmsErrorL(smsMessage,socket);
	smsMessage->SetLogServerId(KLogNullId);
	
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	TestSmsContentsL(smsMessage,KTestMsg1);
	CleanupStack::PopAndDestroy(smsMessage);
	
	CleanupStack::PopAndDestroy(&socket);
    return TestStepResult() ;
	}

// TODO: move to WAP STACK test suite
TVerdict CTestMultipartWapMessage::doTestStepL()
/**
 *  test multi-part WAP message
 */
	{
	INFO_PRINTF1(_L("Test receive multi-part WAP message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	RSocket wapsock;
	TInt ret=wapsock.Open(iSocketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TEST(ret == KErrNone);

	TWapAddr recvWapAddr;
	TBuf8<2048> recvBuf;
	TRequestStatus recvStatus;
	TWapAddr wapAddr;

	wapAddr.SetWapPort(TWapPortNumber(9204));
	ret=wapsock.Bind(wapAddr);
	TEST(ret == KErrNone);

	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Receiving a datagram
	//
	User::WaitForRequest(recvStatus);
	TEST(recvStatus.Int() == KErrNone);

	TInt len=recvBuf.Length();
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK"),len);
	INFO_PRINTF1(_L("WAP message contains:"));

	const TInt KMaxPrintLen = 200;

	for (TInt pos = 0; pos < len; pos += KMaxPrintLen)
		{
		TBuf<KMaxPrintLen> buf;
		buf.Zero();
		const TInt midLen = Min(KMaxPrintLen, len - pos);
		buf.SetLength(midLen);
		for (TInt i=0; i<midLen; ++i)
		    {
			buf[i]=recvBuf[pos+i];
		    }
		INFO_PRINTF2(_L("%S"), &buf);
		}

	wapsock.Close();
    CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestMOSESMessage::doTestStepL()
/**
 *  Test a simple Receive of a single TPDU containing a simple text
 */
	{
	INFO_PRINTF1(_L("Test receive MOSES message"));
	RSocket socketRecvAny;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socketRecvAny, ESmsAddrRecvAny);

	WaitForRecvL(socketRecvAny);
	CSmsMessage* smsMessage = RecvSmsL(socketRecvAny);

	INFO_PRINTF1(_L("incoming SMS received from: ") );
	Logger().Write(smsMessage->ToFromAddress());
	INFO_PRINTF1(_L(""));

	CleanupStack::PushL(smsMessage);
	CleanupStack::PopAndDestroy(2, &socketRecvAny); //  socketRecvAny, smsMessage
 	return TestStepResult() ;
	}

TVerdict CTestClockBack::doTestStepL()
 /**
  *  Test clock back
  */
    {
 	INFO_PRINTF1(_L(" Test that turning back time does not result in 2 receipts of same messages"));
 	RSocket socket;
 	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

 	ChangeReceiveModeL(RMobileSmsMessaging::EReceiveUnstoredClientAck);

 	_LIT(KTestMsg1,"test message, 8bits, length 30");

 	//Set destination and SC numbers
 	iTelephoneNumber=KPekka;
 	iServiceCenterNumber=KRadiolinjaSC;

 	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;

 	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
 	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

 	// turn clock back for two hours (given in seconds)
	User::SetUTCOffset(-2*60*60);

 	//Send SMS
 	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
 	SendSmsL(smsMessage,socket);
 	CleanupStack::PopAndDestroy(smsMessage);

 	//Receive SMS
 	WaitForRecvL(socket);
 	smsMessage = RecvSmsL(socket);
 	CleanupStack::PushL(smsMessage);

 	TestSmsContentsL(smsMessage,KTestMsg1);
 	CleanupStack::PopAndDestroy(smsMessage);

 	INFO_PRINTF2(_L("Waiting %d secs to see if another copy of message received"), 5);
 	const TBool otherReceived = TimedWaitForRecvL(socket, 5 * 1000000);

 	TEST(otherReceived ==EFalse);

 	CleanupStack::PopAndDestroy(&socket);
    return TestStepResult() ;
    }

TVerdict CTestEnumerateNoMessages::doTestStepL()
/**
 *  Test enumerating a message store which has no messages in it.
 *  There are 3 message stores s13 has 0 messages, S14 has 1 message and S15 has 2 messages
 */
    {
	INFO_PRINTF1(_L("Test enumerating a message store which has no messages"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	TRequestStatus status;
	TPckgBuf<TUint> sbuf;
	sbuf()=0;

    TBool tryAgain = ETrue;
    TInt attempts (0);
    TInt maxRetries = 3;

    // Enumerate messages from store. If it fails with KErrNotReady wait for 3 seconds to 
    // allow sms protocol to fully load and repeat
    while( tryAgain && attempts++ < maxRetries )
        {
        socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
        User::WaitForRequest(status);
        INFO_PRINTF2(_L("Enumerate messages from store returned [status=%d]"), status.Int());
        
        if ( status.Int() == KErrNotReady )
            {
            INFO_PRINTF1(_L("Trying to enumerate again... "));
            User::After(3000000);
            }
        else
            {
            tryAgain = EFalse;
            }
        }

    TESTCHECK(status.Int(), KErrNone, "Enumerate messages from store");
    
	//sbuf() includes the count of messages on Store
	TInt count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages"), count);
	TEST(count==3);

	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult() ;
    }

TVerdict CTestIE::doTestStepL()
/**
 *  Test the reception of a CPHS Message Waiting Indication
 */
{
	INFO_PRINTF1(_L("Test IE"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;
	CSmsMessage* smsMessage=NULL;

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	
	CleanupStack::PushL(smsMessage);
	CSmsUserData& recvUserData = smsMessage->SmsPDU().UserData();
	TInt numIE=recvUserData.NumInformationElements();
	
	for(TInt i=0; i<numIE; ++i)
		{
		CSmsInformationElement& ie=recvUserData.InformationElement(i);
		TPtr8 ptr(ie.Data());
		INFO_PRINTF4(_L(" %d. ie  is %d %S"),i ,ie.Identifier(), &ptr);
		}

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult() ;
}

TVerdict CTestEmailIE::doTestStepL()
/**
 *  Test the reception of a CPHS Message Waiting Indication
 */
	{
	INFO_PRINTF1(_L("Test EmailIE"));
	_LIT(KTestMsg1,"MMMMMMMMMM");
	_LIT16(KSmsEmailHeaderShort,"HHHHHH");
	_LIT16(KSmsEmailBodyShort,"BBBBBB");

	_LIT16(KSmsEmailHeaderLong,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	_LIT16(KSmsEmailBodyLong,"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrEmail);

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;
	CSmsMessage* smsMessage=NULL;
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	
	smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
	
	// case 1 short - one pdu
	smsMessage->AddEmailHeaderL(KSmsEmailHeaderShort,KSmsEmailBodyShort);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);
	
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	CSmsUserData& recvUserData = smsMessage->SmsPDU().UserData();
	TInt numIE=recvUserData.NumInformationElements();
	
	for(TInt i=0; i<numIE; ++i)
		{
		CSmsInformationElement& ie=recvUserData.InformationElement(i);
		TPtr8 ptr(ie.Data());
		INFO_PRINTF4(_L(" %d. ie  is %d %S"), i, ie.Identifier(), &ptr);
		}

	TLogId logId(KLogNullId);
	TEST_CHECKL(smsMessage->IsEmailHeader(),ETrue,_L("Checking if message contains E-Mail header"));
	HBufC* bd;
	HBufC* hd;
	if(smsMessage->GetEmailHeaderL(&hd,&bd))
		{
		INFO_PRINTF3(_L(" This is e-mail header:len %d %S"),hd->Length(),hd);
		INFO_PRINTF3(_L(" This is e-mail body: %d %S"),bd->Length(),bd);
		TEST(*hd==KSmsEmailHeaderShort());
		TEST(*bd==KSmsEmailBodyShort());
		logId=smsMessage->LogServerId();
		
		if(logId!=KLogNullId)
			{
			TLogSmsPduData pduData;
			CLogEvent* logEvent=CLogEvent::NewL();
			CleanupStack::PushL(logEvent);
			iSmsStackTestUtils->GetLogEventL(*logEvent,logId);
			TPckg<TLogSmsPduData> packeddata(pduData);
			packeddata.Copy(logEvent->Data());
			TEST(packeddata().iTotal==1);
			TEST(packeddata().iReceived==1);
			CleanupStack::PopAndDestroy(logEvent);
			}
		delete hd;
		delete bd;
		}
	CleanupStack::PopAndDestroy(smsMessage);

	// case 2 - long header
	smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	smsMessage->AddEmailHeaderL(KSmsEmailHeaderLong,KSmsEmailBodyShort);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	CSmsUserData& recvUserData2 = smsMessage->SmsPDU().UserData();
	numIE=recvUserData2.NumInformationElements();
	
	for(TInt i=0; i<numIE; ++i)
		{
		CSmsInformationElement& ie=recvUserData2.InformationElement(i);
		TPtr8 ptr(ie.Data());
		INFO_PRINTF4(_L(" %d. ie  is %d %S"), i, ie.Identifier(), &ptr);
		}

	logId=KLogNullId;
	TEST_CHECKL(smsMessage->IsEmailHeader(),ETrue,_L("Message does noty contain E-Mail header"));
	if(smsMessage->GetEmailHeaderL(&hd,&bd))
	    {
		INFO_PRINTF3(_L(" This is e-mail header:len %d %S"),hd->Length(),hd);
		INFO_PRINTF3(_L(" This is e-mail body: %d %S"),bd->Length(),bd);
		TEST(*hd==KSmsEmailHeaderLong());
		TEST(*bd==KSmsEmailBodyShort());
		logId=smsMessage->LogServerId();
		if(logId!=KLogNullId)
			{
			TLogSmsPduData pduData;
			CLogEvent* logEvent=CLogEvent::NewL();
			CleanupStack::PushL(logEvent);
			iSmsStackTestUtils->GetLogEventL(*logEvent,logId);
			TPckg<TLogSmsPduData> packeddata(pduData);
			packeddata.Copy(logEvent->Data());
			TEST(packeddata().iTotal==2);
			TEST(packeddata().iReceived==2);
			CleanupStack::PopAndDestroy(logEvent);
			}
		delete hd;
		delete bd;
		}
	CleanupStack::PopAndDestroy(smsMessage);


	// case 3 - short header  long body

	smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	smsMessage->AddEmailHeaderL(KSmsEmailHeaderShort,KSmsEmailBodyLong);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("waiting for incoming SMS...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	CSmsUserData& recvUserData3 = smsMessage->SmsPDU().UserData();
	numIE = recvUserData3.NumInformationElements();
	for(TInt i=0; i<numIE; ++i)
		{
		CSmsInformationElement& ie=recvUserData3.InformationElement(i);
		TPtr8 ptr(ie.Data());
		INFO_PRINTF4(_L(" %d. ie  is %d %S"), i, ie.Identifier(), &ptr);
		}

	logId=KLogNullId;
	TEST_CHECKL(smsMessage->IsEmailHeader(),ETrue,_L("Message does noty contain E-Mail header"));
	if(smsMessage->GetEmailHeaderL(&hd,&bd))
		{
		INFO_PRINTF3(_L(" This is e-mail header:len %d %S"),hd->Length(),hd);
		INFO_PRINTF3(_L(" This is e-mail body: %d %S"),bd->Length(),bd);
		TEST(*hd==KSmsEmailHeaderShort());
		TEST(*bd==KSmsEmailBodyLong());
		logId=smsMessage->LogServerId();
		if(logId!=KLogNullId)
			{
			TLogSmsPduData pduData;
			CLogEvent* logEvent=CLogEvent::NewL();
			CleanupStack::PushL(logEvent);
			iSmsStackTestUtils->GetLogEventL(*logEvent,logId);
			TPckg<TLogSmsPduData> packeddata(pduData);
			packeddata.Copy(logEvent->Data());
			TEST(packeddata().iTotal==2);
			TEST(packeddata().iReceived==2);
			CleanupStack::PopAndDestroy(logEvent);
			}
		delete hd;
		delete bd;
		}
	CleanupStack::PopAndDestroy(smsMessage);

	// case 4 - long header  long body
	smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	smsMessage->AddEmailHeaderL(KSmsEmailHeaderLong,KSmsEmailBodyLong);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	CSmsUserData& recvUserData4 = smsMessage->SmsPDU().UserData();
	numIE=recvUserData4.NumInformationElements();
	
    for(TInt i=0; i<numIE; ++i)
		{
		CSmsInformationElement& ie=recvUserData4.InformationElement(i);
		TPtr8 ptr(ie.Data());
		INFO_PRINTF4(_L(" %d. ie  is %d %S"), i, ie.Identifier(), &ptr);
		}
	
	logId=KLogNullId;
	TEST_CHECKL(smsMessage->IsEmailHeader(),ETrue,_L("Message does noty contain E-Mail header"));
	if(smsMessage->GetEmailHeaderL(&hd,&bd))
		{
		INFO_PRINTF3(_L(" This is e-mail header:len %d %S"),hd->Length(),hd);
		INFO_PRINTF3(_L(" This is e-mail body: %d %S"),bd->Length(),bd);
		TEST(*hd==KSmsEmailHeaderLong());
		TEST(*bd==KSmsEmailBodyLong());
		logId=smsMessage->LogServerId();
		if(logId!=KLogNullId)
			{
			TLogSmsPduData pduData;
			CLogEvent* logEvent=CLogEvent::NewL();
			CleanupStack::PushL(logEvent);
			iSmsStackTestUtils->GetLogEventL(*logEvent,logId);
			TPckg<TLogSmsPduData> packeddata(pduData);
			packeddata.Copy(logEvent->Data());
			TEST(packeddata().iTotal==3);
			TEST(packeddata().iReceived==3);
			CleanupStack::PopAndDestroy(logEvent);
			}
		delete hd;
		delete bd;
		}

	CleanupStack::PopAndDestroy(smsMessage);

	// case 5 - long header, long body, unicode
	smsMessage=CreateSmsMessageLC(KTestMsg1,TSmsDataCodingScheme::ESmsAlphabetUCS2);

	smsMessage->AddEmailHeaderL(KSmsEmailHeaderLong,KSmsEmailBodyLong);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	CSmsUserData& recvUserData5 = smsMessage->SmsPDU().UserData();
	numIE=recvUserData5.NumInformationElements();
    for(TInt i=0; i<numIE; ++i)
		{
		CSmsInformationElement& ie=recvUserData5.InformationElement(i);
		TPtr8 ptr(ie.Data());
		INFO_PRINTF4(_L(" %d. ie  is %d %S"), i, ie.Identifier(), &ptr);
		}

	logId=KLogNullId;
	TEST_CHECKL(smsMessage->IsEmailHeader(),ETrue,_L("Message does noty contain E-Mail header"));
	if(smsMessage->GetEmailHeaderL(&hd,&bd))
		{
		INFO_PRINTF3(_L(" This is e-mail header:len %d %S"),hd->Length(),hd);
		INFO_PRINTF3(_L(" This is e-mail body: %d %S"),bd->Length(),bd);
		TEST(*hd==KSmsEmailHeaderLong());
		TEST(*bd==KSmsEmailBodyLong());
		logId=smsMessage->LogServerId();
		if(logId!=KLogNullId)
			{
			TLogSmsPduData pduData;
			CLogEvent* logEvent=CLogEvent::NewL();
			CleanupStack::PushL(logEvent);
			iSmsStackTestUtils->GetLogEventL(*logEvent,logId);
			TPckg<TLogSmsPduData> packeddata(pduData);
			packeddata.Copy(logEvent->Data());
			TEST(packeddata().iTotal==6);
			TEST(packeddata().iReceived==6);
			CleanupStack::PopAndDestroy(logEvent);
			}
		delete hd;
		delete bd;
		}
	CleanupStack::PopAndDestroy(smsMessage);


	// case 6 - long header  long body, 7bit
	smsMessage=CreateSmsMessageLC(KTestMsg1,TSmsDataCodingScheme::ESmsAlphabet7Bit);

	smsMessage->AddEmailHeaderL(KSmsEmailHeaderLong,KSmsEmailBodyLong);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	CSmsUserData& recvUserData6 = smsMessage->SmsPDU().UserData();
	numIE=recvUserData6.NumInformationElements();
	
    for(TInt i=0; i<numIE; ++i)
		{
		CSmsInformationElement& ie=recvUserData6.InformationElement(i);
		TPtr8 ptr(ie.Data());
		INFO_PRINTF4(_L(" %d. ie  is %d %S"),i, ie.Identifier(), &ptr);
		}

	logId=KLogNullId;
	TEST_CHECKL(smsMessage->IsEmailHeader(),ETrue,_L("Message does noty contain E-Mail header"));
	if(smsMessage->GetEmailHeaderL(&hd,&bd))
		{
		INFO_PRINTF3(_L(" This is e-mail header:len %d %S"),hd->Length(),hd);
		INFO_PRINTF3(_L(" This is e-mail body: %d %S"),bd->Length(),bd);
		TEST(*hd==KSmsEmailHeaderLong());
		TEST(*bd==KSmsEmailBodyLong());
		logId=smsMessage->LogServerId();
		if(logId!=KLogNullId)
			{
			TLogSmsPduData pduData;
			CLogEvent* logEvent=CLogEvent::NewL();
			CleanupStack::PushL(logEvent);
			iSmsStackTestUtils->GetLogEventL(*logEvent,logId);
			TPckg<TLogSmsPduData> packeddata(pduData);
			packeddata.Copy(logEvent->Data());
			TEST(packeddata().iTotal==3);
			TEST(packeddata().iReceived==3);
			CleanupStack::PopAndDestroy(logEvent);
			}
		delete hd;
		delete bd;
		}

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestMaxReadTimeForSmsStoreList::doTestStepL()
/**
 *  Test Sms message storage
 */
    {
    INFO_PRINTF1(_L("Enumerating a phone store list that takes 254 seconds"));
    // Open the socket for SIM operations
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrLocalOperation);

    // Enumerate messages
    RPointerArray<CSmsMessage> messages;
    CleanupResetAndDestroyPushL(messages);
    ReadSmsStoreL(socket, messages);

    const TInt count = messages.Count();
    TESTCHECK(count, 2, "Request retrieved 2 messages after 254 seconds");

    CleanupStack::PopAndDestroy(&messages);
    CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
    }

TVerdict CTestSameLogIdMultiplePDU::doTestStepL()
/**
 *  Resend Multi PDU SMS message with same log id"
 */
	{
	// The test is included with a fix for INC039985
	// - Resending SMS message causes CSmsSegmentationStore panic
	//
	// The test is organised as following:
    //
	// For 7 bit alphabet an SMS consisting of a multiple (3) PDU is created,
	// allocated an Log Id number and sent twice, immediately one after the other, to simulate
	// the process of resending of the SMS message.
	// Upon the successful completion of both sending operations, the test should
	// receive exactly the same message back from the SimTsy. The sent and received
	// messages are checked for identity.
	// As indicated  within the initial defect comments  the assertion occurred approx. 50% of the time.
    // To increase the chance of catching the defect
	// the above discussed resending operation step is performed twice.
	//
	// This test is based on test 46.
	// This test step uses section #57 of tsmsprt_config.txt file
    // which provides the encoded versions of the PDUs, the SIMTSY has to receive and send.

	INFO_PRINTF1(_L(" Test that sent SMS consisting of Multiple PDUs with the same logId"));
	TLogId logid;
	TLogSmsPduData pduData;
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	ChangeReceiveModeL(RMobileSmsMessaging::EReceiveUnstoredClientAck);

    // Initialization
    // Message String
	_LIT(KTestMsg1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBCCCCCCC");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	// Select an alphabet
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

    // Twice send an Sms with the same logid - step 1
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
	pduData.iType=smsMessage->Type();

	// Get a log Id
	logid=iSmsStackTestUtils->AddLogEventL(*smsMessage,pduData);
	smsMessage->SetLogServerId(logid);
	INFO_PRINTF2(_L("Sends message with log id %d"),logid);

	SendSmsL(smsMessage,socket);
	// Send SMS again
    SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);

	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg1);
	CleanupStack::PopAndDestroy(smsMessage);

	// Twice send an Sms with the same logid - step 2
	// Create a Message
	smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
	pduData.iType=smsMessage->Type();

	// Get a log Id
	logid=iSmsStackTestUtils->AddLogEventL(*smsMessage,pduData);
	smsMessage->SetLogServerId(logid);
	INFO_PRINTF2(_L("Sends message with log id %d"),logid);

	// Send SMS
	SendSmsL(smsMessage,socket);
	// Send SMS again
    SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);

	//Receive SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg1);

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

TVerdict CTestSameLogIdSinglePDU::doTestStepL()
/**
 *  Resend Single PDU SMS message with same log id"
 */
	{
    // The test is included with a fix for INC039985
	// - Resending SMS message causes CSmsSegmentationStore panic
	//
	// The test is organised as following:
	//
	// For  every supported alphabet (3 at present) an SMS consisting of a a single PDU is created,
	// allocated an Log Id number and sent twice, immediately one after the other, to simulate
	// the process of resending of the SMS message.
	// Upon the successful completion of both sending operations, the test should
	// receive exactly the same message back from the SimTsy. The sent and received
	// messages are checked for identity.
	// As the initial comments,  when the defect was raised were, that the chances
	// of the defect happening more than 50%, to increase the chance of catching the defect
	// the above discussed resending operation step is performed twice.
	//
	// This test is based on test 46.
	// This test step uses section #58 of tsmsprt_config.txt file
    // which provides the encoded versions of the PDUs, the SIMTSY has to receive and send.
    //
	INFO_PRINTF1(_L(" Test that sent SMS consisting of single PDUs with the same logId"));
	TLogId logid;
	TLogSmsPduData pduData;
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);
	
	ChangeReceiveModeL(RMobileSmsMessaging::EReceiveUnstoredClientAck);
	
	_LIT(KTestMsg1,"CCCCCCC");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	CSmsMessage* smsMessage;
	TSmsDataCodingScheme::TSmsAlphabet alphabet;

	// Loop through the individual alphabets, using the defined distance
	// between individual entries. The second loop has been intentionally
	// unwound for clarity
    for ( TInt nStep = TSmsDataCodingScheme::ESmsAlphabet7Bit;
          nStep < TSmsDataCodingScheme::ESmsAlphabetReserved;
          nStep +=(TSmsDataCodingScheme::ESmsAlphabet8Bit - TSmsDataCodingScheme::ESmsAlphabet7Bit) )
        {
        alphabet = (TSmsDataCodingScheme::TSmsAlphabet) nStep;
        INFO_PRINTF2(_L("Alphabet:..... %d"),alphabet);

        // Twice send an Sms with the same logid - step 1
        // Create a Message
        smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
        pduData.iType=smsMessage->Type();

        // Get a log Id
        logid=iSmsStackTestUtils->AddLogEventL(*smsMessage, pduData);
        smsMessage->SetLogServerId(logid);
        INFO_PRINTF2(_L("Sends message with log id %d"),logid);

        // Send SMS
        SendSmsL(smsMessage,socket);
        // Send SMS again
        SendSmsL(smsMessage,socket);

        CleanupStack::PopAndDestroy(smsMessage);

        // Receive SMS
        WaitForRecvL(socket);
        smsMessage = RecvSmsL(socket);

        CleanupStack::PushL(smsMessage);
        TestSmsContentsL(smsMessage,KTestMsg1);
        CleanupStack::PopAndDestroy(smsMessage);

        // Twice send an Sms with the same logid - step 2
        smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
        pduData.iType=smsMessage->Type();

        // Get a log Id
        logid=iSmsStackTestUtils->AddLogEventL(*smsMessage,pduData);
        smsMessage->SetLogServerId(logid);
        INFO_PRINTF2(_L("Sends message with log id %d"),logid);

        // Send SMS
        SendSmsL(smsMessage,socket);
        // Send SMS again
        SendSmsL(smsMessage,socket);

        CleanupStack::PopAndDestroy(smsMessage);

        //Receive SMS
        WaitForRecvL(socket);
        smsMessage = RecvSmsL(socket);

        CleanupStack::PushL(smsMessage);
        TestSmsContentsL(smsMessage,KTestMsg1);
        CleanupStack::PopAndDestroy(smsMessage);
        }

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
}

CTestLog* CTestLog::NewLC(CSmsStackTestUtils& aUtils, RFs& aFs, CSmsBaseTestStep* aTest, TInt aPriority)
	{
	CTestLog* self = new (ELeave) CTestLog(aUtils, aTest, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	return self;
	}

CTestLog::CTestLog(CSmsStackTestUtils& aUtils, CSmsBaseTestStep* aTest, TInt aPriority)
: CActive(aPriority), iUtils(aUtils), iTest(aTest)
	{
	CActiveScheduler::Add(this);
	}

CTestLog::~CTestLog()
	{
	Cancel();
	iSocket.Close();
	delete iLogChecker;
	delete iEventLogger;
    delete iMessage;
	iAddedIds.Close();
	}

void CTestLog::StartOriginalL()
	{
	iAddedIds.Reset();

	iLogChecker->CountOriginalIdsL(iStatus);
	iState = EOriginal;
	SetActive();
	CActiveScheduler::Start();

	//TEST(KErrNone == iStatus.Int());
	iTest->testBooleanTrue(KErrNone == iStatus.Int(),(TText8*)__FILE__, __LINE__);
	}

void CTestLog::StartCompareL(TInt aExpectError)
	{
	iLogChecker->CompareNewIdsL(iAddedIds, iStatus);
	SetActive();
	CActiveScheduler::Start();

	//TEST(aExpectError == iStatus.Int());
	iTest->testBooleanTrue(aExpectError == iStatus.Int(),(TText8*)__FILE__, __LINE__);
	}

_LIT(KLongText,"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"3 PDU test SMS message. "
        L"The End.");

void CTestLog::StartL()
	{

	StartOriginalL();

	iTest->SendSmsL(iMessage,iSocket);

	delete iMessage;
	iMessage = NULL;

	iTest->WaitForRecvL(iSocket);
	iMessage = iTest->RecvSmsL(iSocket);

	iTest->TestSmsContentsL(iMessage, KLongText);

	User::LeaveIfError(iAddedIds.Append(iMessage->LogServerId()));

	StartCompareL(KErrNone);
	delete iMessage;
	iMessage = NULL;
	}

void CTestLog::ConstructL(RFs& aFs)
	{
	iLogChecker = CSmsLogChecker::NewL(aFs, iTest, Priority());
	iEventLogger = CSmsEventLogger::NewL(aFs, Priority());
	
//	iTest->ConnectSocketServerL(iSocketServer);
	iTest->iSmsStackTestUtils->OpenSmsSocketL(iTest->iSocketServer,iSocket,ESmsAddrRecvAny);

	//Set destination and SC numbers
	iTest->iTelephoneNumber=KRegTestNumber;
	iTest->iServiceCenterNumber=KVodafoneSC;

	iMessage=iTest->CreateSmsWithStatusReportReqL(KLongText,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	}

void CTestLog::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EOriginal:
			iMessage->SetLogServerId(KLogNullId);
			break;
		default:
			break;
		}

	if (!IsActive())
		Complete(KErrNone);
	}


TVerdict CTestConcatenatedMessageLogging::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Tx and Rx SMS with 3 PDU message"));

	CTestLog* testLog = CTestLog::NewLC(*iSmsStackTestUtils, iFs,this, CActive::EPriorityStandard);
	testLog->StartL();
	
	CleanupStack::PopAndDestroy(testLog);
	return TestStepResult();
	}


TVerdict CTestEnumerationOfStatusReport::doTestStepL()
	{
	INFO_PRINTF1(_L("Test enumeration of status report"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrLocalOperation);

	CSmsMessage* smsMessage = CreateSmsMessageLC(CSmsPDU::ESmsStatusReport, CSmsBuffer::NewL(), iTelephoneNumber);

	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);

	WriteSmsToSimL(*smsMessage, socket);

 	// Enumerate messages from Store
	TRequestStatus status;
	TPckgBuf<TUint> sbuf;
	sbuf()=0;

	//Now enumerate messages from store
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNone, "Enumerate messages from store");

	//sbuf() includes the count of messages on Store
	TInt count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages"), count);
	TEST(count==1);

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
   	return TestStepResult();
	}

TVerdict CTestWriteStatusReportToSIM::doTestStepL()
	{
	INFO_PRINTF1(_L("Test write of status report to SIM"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrLocalOperation);

	CSmsMessage* smsMessage = CreateSmsMessageLC(CSmsPDU::ESmsStatusReport, CSmsBuffer::NewL(), iTelephoneNumber);

	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);

	WriteSmsToSimL(*smsMessage, socket);

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

TVerdict CTestTxSmsWithDelayedCompletion::doTestStepL()
    {
    /**
     *  INC045765/DEF46216 - Test that sms stack does not time out if the TSY takes 1 minute
     *  59 seconds to complete sending. This test case is cloned from
     *  TestSimpleTxAndRxL(), simply removing the message receiption portion
     *  and adding a delay to the SIM TSY's completion time in tsms_config.txt.
     *  DEF047240 - read sendTryTimeout from ESK file
     */
	INFO_PRINTF1(_L("Test Simple Tx SMS"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	ChangeReceiveModeL(RMobileSmsMessaging::EReceiveUnstoredClientAck);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("SMS message sent successfully") );

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	};

TVerdict CTestSmsStoreReadCancel::doTestStepL()
/**
 *  Test Sms message storage - cancel during read
 */
	{
    return TestStepResult() ;
	}

TVerdict CTestBindWhenPoweredDownWithPublishSubscribe::doTestStepL()
/**
 *  Test bind()ing when powered-down, and subsequent dequeueing of messages upon power-up
 *  using Publish and Subscribe
 */
	{
	INFO_PRINTF1(_L("Test bind() when powered-down with Publish and Subscribe"));

	INFO_PRINTF1(_L("switching phone off") );
	RProperty phonePowerProperty;
	User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
	CleanupClosePushL(phonePowerProperty);

	// Create the socket and open for SIM operations
	RSocket socket;
	INFO_PRINTF1(_L("binding socket") );
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	// knock power off because opening of socket brought it up
	UpdatePhonePowerStatusL(phonePowerProperty, ESAPhoneOff);
	
	// Set the SimTSY config.
	SetSimTSYTestNumberL(29);
	
	// Briefly wait for receipt on it - this shouldn't happen when powered off
	TBool bRcv = TimedWaitForRecvL(socket, 5* 1000000);
	TEST_CHECKL(bRcv, EFalse, _L("Receive should fail with phone off"));

	// Power on & wait for rx again
	UpdatePhonePowerStatusL(phonePowerProperty, ESAPhoneOn);

	// Briefly wait for receipt on it - this should now happen
	bRcv = TimedWaitForRecvL(socket, 5 * 1000000);
	TEST_CHECKL(bRcv, ETrue, _L("Receive should now succeed with phone on again"));
	CSmsMessage* smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);

	TPtrC fromAddr = smsMessage->ToFromAddress();
	INFO_PRINTF2(_L("Received SMS from: %S"), &fromAddr);

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
	CleanupStack::PopAndDestroy(&phonePowerProperty);
 	return TestStepResult() ;
	}

TVerdict CTestObserverNotifiedWhenPoweredDownWithPublishSubscribe::doTestStepL()
/**
 *  Fix Defect 42937
 *  
 *  Test that the SMS Stack notifies the ESmsAddrApplication
 *  observer when the modem is powered down without a panic.
 *  
 *  The test creates a socket for 16 Bit Port 245 and
 *  then shuts down the power, causing the modem to be turned
 *  off. The stack then notifies the observer for port 245,
 *  calling function:
 *  
 *  void CSmsProtocol::NotifyMessageObservers(TInt aStatus)
 *  
 *  The test passes provided this method completes without a
 *  panic.
 *  
 *  Identical operation of test now using publish and subscribe
 */
	{
	INFO_PRINTF1(_L("Test observer when powered-down"));
	// Open the socket and bind it to ESmsAddrApplication16BitPort 245
	RSocket socket;
	TSmsAddr addr16;
	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);
	TInt port(245);
	addr16.SetPort(port);

	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,addr16);

	// Switch phone off, causing the stack to notify the observer
	// corresponding to ESmsAddrApplication16BitPort 245
	INFO_PRINTF1(_L("switching phone off") );

	RProperty phonePowerProperty;
	User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
	CleanupClosePushL(phonePowerProperty);

	// knock power off because opening of socket brought it up
	UpdatePhonePowerStatusL(phonePowerProperty, ESAPhoneOff);

	// bring power back up
	UpdatePhonePowerStatusL(phonePowerProperty, ESAPhoneOn);

	CleanupStack::PopAndDestroy(&phonePowerProperty);
	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult();
	}

TVerdict CTestSmsCNumberChangeWithPublishSubscribe::doTestStepL()
/**
 *  Test SMSC number change
 *  Added to verify fix for defect # INC040029.
 *  Client requires to change the behaviour of the SMS stack from
 *  caching SMS parameters on start up, specifically Service Control
 *  centre number to requesting it each time from the TSY.
 *  This test harness uses the SimTsy. In order to mimic a change
 *  in the Service Centre Number, using the SimTsy, one should initially
 *  power the phone down, switch to a different test number, and power up again.
 *  During the power up stage the SimTsy initialises itself and was deemed as
 *  the only possible way to change the SC number using the SimTsy.
 *  
 *  The test relies on two test sections[#55 and #56] of the tsmsprt_config.txt
 *  file, each of which contains a single Sms parameter entry.The test finally
 *  compares the two parameter entries, which should differ.
 *  
 *  Updated to use publish and subscribe
 *  
 */
    {
    INFO_PRINTF1(_L("Test SMSC Number Change with Publish and Subscribe"));
    // Open the socket for SIM operations
    RSocket socket1;
    iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket1, ESmsAddrLocalOperation);
    RSmsSocketReadStream readstream1(socket1);

    // Create the smspList
    CMobilePhoneSmspList* smspList1 = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspList1);

    // Make read SMS params request to the SMS Stack
    TRequestStatus status;
    TBool tryAgain = ETrue;
    TInt attempts (0);
    TInt maxRetries = 3;

    // Enumerate messages from store. If it fails with KErrNotReady wait for 3 seconds to 
    // allow sms protocol to fully load and repeat
    while( tryAgain && attempts++ < maxRetries )
        {
        socket1.Ioctl(KIoctlReadSmsParams,status,NULL,KSolSmsProv);
        User::WaitForRequest(status);
        INFO_PRINTF2(_L("Reading SMS parameters returned [status=%d]"), status.Int());
        if ( status.Int() == KErrNotReady )
            {
            INFO_PRINTF1(_L("Trying to enumerate again... "));
            User::After(3000000);
            }
        else
            {
            tryAgain = EFalse;
            }
        }

    TESTCHECK(status.Int(), KErrNone, "Reading SMS parameters"); 

    // Read list from stream and make acknowledgement to the SMS Stack
    INFO_PRINTF1(_L("About to write to smspList1..."));
    readstream1 >> *smspList1;
    INFO_PRINTF1(_L("written to smspList1..."));
    socket1.Ioctl(KIoctlCompleteReadSmsParams,status,NULL,KSolSmsProv);
    User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrNone, "Completing Read SMS parameters...");

    iSmsStackTestUtils->PrintSmspList(*smspList1);

    // Switch phone off/on area
    // Init required members
    RProperty phonePowerProperty;
    User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
    CleanupClosePushL(phonePowerProperty);

    // knock power off
    UpdatePhonePowerStatusL(phonePowerProperty, ESAPhoneOff);

    // Move to new test
    RProperty testNumberProperty;
    User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
    CleanupClosePushL(testNumberProperty);

    TInt TestNumber = 56;

    testNumberProperty.Subscribe(status);
    User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,TestNumber));
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNone);
    TInt testNumberCheck;
    User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
    if (TestNumber != testNumberCheck)
        User::Leave(KErrNotFound);

    // Power back on
    UpdatePhonePowerStatusL(phonePowerProperty, ESAPhoneOn);

    CleanupStack::PopAndDestroy(&testNumberProperty);
    CleanupStack::PopAndDestroy(&phonePowerProperty);

    // Open the second socket for SIM operations
    RSocket socket2;
    iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket2,ESmsAddrLocalOperation);

    RSmsSocketReadStream readstream2(socket2);

    // Create the smspList
    CMobilePhoneSmspList* smspList2 = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspList2);

    // Make read SMS params request to the SMS Stack
    socket2.Ioctl(KIoctlReadSmsParams,status,NULL,KSolSmsProv);
    INFO_PRINTF1(_L("waiting for SMS parameters...") );
    User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrNone, "Receiving SMS parameters...");

    // Read list from stream and make acknowledgement to the SMS Stack
    readstream2 >> *smspList2;
    socket2.Ioctl(KIoctlCompleteReadSmsParams,status,NULL,KSolSmsProv);
    User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrNone, "Completing Read SMS parameters...");

    iSmsStackTestUtils->PrintSmspList(*smspList2);

    // Test that the two numbers are different
    RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy1;
    RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy2;
    entryToTsy1 = smspList1->GetEntryL(0);
    entryToTsy2 = smspList2->GetEntryL(0);

    INFO_PRINTF3(_L("Call center Numbers: CS1 <%S> CS2 <%S>"),
                                 &entryToTsy1.iServiceCentre.iTelNumber,
                                 &entryToTsy2.iServiceCentre.iTelNumber);

    // Real test - both parameters have a vaild SC number and they are different - it leaves if error
    TEST((entryToTsy1.iValidParams & RMobileSmsMessaging::KSCAIncluded) &&
         (entryToTsy2.iValidParams & RMobileSmsMessaging::KSCAIncluded) &&
         (entryToTsy1.iServiceCentre.iTelNumber != entryToTsy2.iServiceCentre.iTelNumber));

    CleanupStack::PopAndDestroy(smspList2);
    CleanupStack::PopAndDestroy(&socket2);
    CleanupStack::PopAndDestroy(smspList1);
    CleanupStack::PopAndDestroy(&socket1);
  	return TestStepResult();
    }

TVerdict CTestStatusReportTime::doTestStepL()
/**
 *  Test a simple Transmit and Receive with status reporting, check time stamp
 */
	{
	INFO_PRINTF1(_L("Test Tx an SMS and then receive a status report"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessageSend=CreateSmsWithStatusReportReqL(KTestMsg1,alphabet);
	CleanupStack::PushL(smsMessageSend);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//  Get sent sms message time stamp
	TTime timeSend;
	timeSend = smsMessageSend->Time();

	//Send SMS
	SendSmsL(smsMessageSend,socket);
	CleanupStack::PopAndDestroy(smsMessageSend);

	//Receive status report
	TSmsServiceCenterAddress telephoneNumberSC;
	telephoneNumberSC.Copy( KPekka );

	//Receive SMS
	WaitForRecvL(socket);
	CSmsMessage* smsMessageRecv = RecvSmsL(socket);

	//Check the status report (timestamp should be 10 seconds later)
	CleanupStack::PushL(smsMessageRecv);
	TBool isSR = (smsMessageRecv->Type()==CSmsPDU::ESmsStatusReport);

	if (isSR)
		{
		INFO_PRINTF1(_L("Received status report"));
		TSmsServiceCenterAddress telephoneNumberSR=smsMessageRecv->ToFromAddress();
		TEST(telephoneNumberSR==telephoneNumberSC);
		INFO_PRINTF2(_L("Message delivered to %S"),&telephoneNumberSC);

		//  check time stamp
		TTime timeRecv;
		timeRecv = smsMessageRecv->Time();

		if (timeRecv < timeSend)
			{
			//  fail condition
			ERR_PRINTF1(_L("Timestamp of received status report earlier than sent SMS!"));
			SetTestStepResult(EFail);
			}
		else if (timeRecv == timeSend)
			{
			//  fail condition
			ERR_PRINTF1(_L("Timestamp of received status report same as sent SMS!"));
			SetTestStepResult(EFail);
			}
		else
			{
			//  pass condition
			INFO_PRINTF1(_L("Timestamp of received status report later than sent SMS!"));
			}
		}
	else
	    {
		ERR_PRINTF1(_L("Received SMS is NOT a Status report!"));
		SetTestStepResult(EFail);
	    }

    CleanupStack::PopAndDestroy(2, &socket); // smsMessageRecv, socket
	return TestStepResult() ;
	}

//TODO: move to WAPPROT test suite
TVerdict CTestTx8BitWapWithStatus::doTestStepL()
	//  Fix Defect 42714
	//      This test case verifies that 8 Bit Wap messages can
	//      sent with their Status Report Bit set. Hither to this
	//      functionality was only provided for 7 Bit Wap Messages.
	// 	    It was an apparent oversight that the status report
	//      bit could not be set for a 8 Bit Wap Message.
	//
	//      The test has the following stages:
	//      (1) Create the Server
	//      (2) Create a Socket with which to send 8 Bit Wap Messages
	//      (3) Bind that Socket to a particular port number and address
	//      (4) Send Wap Message
	//      (5) Wait for status report
	//
	//      Note: It is not possible to correlate the outgoing Wap Message to
	//      the inbound status message as the interface does not provide
	//      access to the log id. This will be fixed in defect 42716.
	//
    {
    INFO_PRINTF1(_L("Test sending a 8 Bit Wap Message and receiving a status message"));

    // (1) Create a Socket for sending wap messages
    RSocket socket;
    TInt ret=socket.Open(iSocketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
    INFO_PRINTF2(_L("Socket return code is %d"),ret);
    TESTL(ret == KErrNone);
    CleanupClosePushL(socket);

    TWapSmsDataCodingScheme codingScheme = EWapSms8BitDCS;
    ret = socket.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme);
    TESTL(ret==KErrNone);

    // (2) Create a socket for receiving status reports
    RSocket statusSocket;
    iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,statusSocket,ESmsAddrStatusReport);
    // socket pushed onto stack inside method call.

    // (3) Create a Wap address and bind it to the socket
    TWapAddr wapAddr;
    wapAddr.SetWapPort(TWapPortNumber(226));
    TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
    // iTelephoneNumber is set up to be the value defined
    // in setupgsmsms.txt
    wapAddr.SetWapAddress(buf8);
    ret = socket.Bind(wapAddr);
    TESTL(ret==KErrNone);

    // (4) Send a test message to address specified in setupgsmsms.txt
    // and the port specified above
    TRequestStatus status;
    _LIT8(KWapTestMsg,"BEGIN:VCARD\r\nVERSION:2.1\r\nFN:Jal\r\nN:Jal\r\nORG:PanSoftware\r\nTITLE:Director\r\nLABEL:Islington\r\nEND:VCARD\r\n");
    
    socket.SendTo(KWapTestMsg,wapAddr,0,status);
    User::WaitForRequest(status);
    TESTL(status.Int()==KErrNone);
    INFO_PRINTF2(_L("Sent datagram length: %d on socket OK"),KWapTestMsg().Length());

    // (5) Message has been sent successfully, now check that
    // status report is received for that address.
    WaitForRecvL(statusSocket);
    INFO_PRINTF1(_L("received status report...") );

    User::After(200000);
    CleanupStack::PopAndDestroy(&statusSocket);
    CleanupStack::PopAndDestroy(&socket);
    return TestStepResult();
    }

TVerdict CTestSimStoreCorruptMessage::doTestStepL()
/**
 *  Added to verify fix for DEF039913
 *  This test case is testing how the stack processes PDUs that the TSY declares corrupt
 *  (by returning KErrCorrupt to the client side active object).
 *  
 *  The SIM Store has been configured to contain 1 valid PDU, followed by 2 corrupt PDUs
 *  and then one final valid PDU. The test checks that both, and only, the 2 valid PDUs
 *  are decoded and forwarded to the client.
 */
	{
	INFO_PRINTF1(_L("Test to see whether RMobileSimStore ignores corrupted SMSs"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	// Enumerate messages
	TRequestStatus status;
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);

	ReadSmsStoreL(socket, messages, status);
	TESTCHECK(status.Int(), KErrNone, "Enumerate messages");
	TESTCHECK(messages.Count(), 2, "CHecking message count equals to 2");
		
	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&socket);
  	return TestStepResult() ;
	}
 
 TVerdict CTestCorruptPduWithNegativeResponse::doTestStepL()
 /**
  *  Test for DEF47323
  *  (1) Configure the sms.esk file so that a corrupt pdu will be negatively acknowledged.
  *  (2) Receive a corrupt PDU and negatively acknowledge it.
  *  (3) Receive a second valid PDU, positively acknowledge it and forward it to the client.
  *  
  */
 {
 	INFO_PRINTF1(_L("TestCorruptPduWithNegativeResponse"));
 	RSocket socket;
 	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

 	_LIT(KTestMsg1,"test message, 8bits, length 30");

 	//Set destination and SC numbers
 	iTelephoneNumber=KPekka;
 	iServiceCenterNumber=KRadiolinjaSC;
 	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;

 	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

 	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
 	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

 	//Send SMS
 	SendSmsL(smsMessage,socket);
 	CleanupStack::PopAndDestroy(smsMessage);

 	//Receive SMS
 	WaitForRecvL(socket);
 	smsMessage = RecvSmsL(socket);
 	CleanupStack::PushL(smsMessage);

 	TestSmsContentsL(smsMessage,KTestMsg1);

 	CleanupStack::PopAndDestroy(smsMessage);
 	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult();
 }

TVerdict CTestBootTimer::doTestStepL()
 /**
  *  Test for DEF054033:
  *  Check that the sms stack boot timer value can be configured from the
  *  smswap.sms.esk file.
  *  
  *  Test steps:
  *  (1) Allow the sms stack to boot and for the boot timer value to
  *  be configured from the esk file.
  *  (2) Create a default socket using the address ESmsAddrRecvAny (This is
  *  intended to be used as the default recepient for text messages
  *  but not for wap messages - these are intended to be sent to a wap watcher).
  *  (3) Send a wap message. This should be stored in the reassembly store
  *  in the expectation that wap stack client will shortly bind a
  *  socket. When the wap socket binds, the wap message will be read
  *  from the reassembly store and be sent to the wap socket.
  *  (4) However for the purposes of this test case, the wap socket never binds,
  *  so when the sms stack's boot timer expires after the configured time,
  *  the messages are delivered to the default socket.
  */
	{
    INFO_PRINTF1(_L("Test receive multi-part WAP message"));
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

    TBool msgRcvd = TimedWaitForRecvL(socket, 40000000);

    if (msgRcvd)
        {
        INFO_PRINTF1(_L("WAP message delivered to messaging application"));
        INFO_PRINTF1(_L("on default watcher"));
        }
    else
        {
        INFO_PRINTF1(_L("Boot Time did not timeout - Test Failed"));
        User::Leave(KErrGeneral);
        }

    CleanupStack::PopAndDestroy(&socket);
    return TestStepResult() ;
	}

TVerdict CTestCSmsMessageTimeStamp::doTestStepL()
/**
 *  Creating a CSmsMessage object, checking it's timestamp is set to UTC time
 */
{
	//Get the current UTC offset
	TTime currUTCTime;
	currUTCTime.UniversalTime();
	TTimeIntervalSeconds currUTCOffset=User::UTCOffset();

	_LIT(KTestMsg,"test message, 8bits, length 30");
	TBool result=ETrue;

	//--------------------------------------------------------
	//	Set the universal time
	//	UTC=2005/03/07 13h11min, offset=35seconds
	TInt year=2005;
	TMonth month=EMarch;
	TInt day=6;
	TInt hour=13;
	TInt minute=11;
	TInt seconds=0;
	TInt milliseconds=0;
	TDateTime testTime(year,month,day,hour,minute,seconds,milliseconds);
	TTime utcTime(testTime);
	//Offset
	TInt offsetSeconds=35;
	TTimeIntervalSeconds offset(offsetSeconds);
	//Set UTC and offset
	User::SetUTCTimeAndOffset(utcTime,offset);

	//Create a message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//	Test time stamp, Time()
	//	Time must be = UTC, we allow 1 second for Message to be created.
	TTime smsTimeStamp=smsMessage->Time();
	TInt allowableVariance=1;
	TTimeIntervalSeconds secondVariance(allowableVariance);
	TEST(smsTimeStamp<=utcTime+secondVariance && smsTimeStamp>=utcTime-secondVariance);

	//--------------------------------------------------------
	//SetTime
	//Setup a date time of message
	//Time=2004/12/31 23h45min55sec
	year=2004;
	month=EDecember;
	day=30;
	hour=23;
	minute=45;
	seconds=55;
	milliseconds=0;
	testTime=TDateTime(year,month,day,hour,minute,seconds,milliseconds);
	TTime updatedTime(testTime);
	smsMessage->SetTime(updatedTime);

	//check the time
	TTime messageTime=smsMessage->Time();
	TEST(messageTime==updatedTime);

	//--------------------------------------------------------
	//Check offset, Get
	TTimeIntervalSeconds messageOffset(smsMessage->UTCOffset());
	TEST(messageOffset==offset);

	//--------------------------------------------------------
	//Set offset with message API
	offsetSeconds=60;		//1 minute
	offset=TTimeIntervalSeconds(offsetSeconds);
	TBool ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	//Check the value has changed
	messageOffset=smsMessage->UTCOffset();
	TEST(offset==messageOffset);

	//--------------------------------------------------------
	//Exercise boundaries
	//Value 1 less than maximum
	offsetSeconds=78*15*60+899;		//78 fifteen minute blocks in seconds, plus 899 sec
	offset=TTimeIntervalSeconds(offsetSeconds);
	ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	messageOffset=smsMessage->UTCOffset();
	TEST(offset==messageOffset);

	//Value on than maximum
	offsetSeconds=79*15*60;		//79 fifteen minute blocks in seconds
	offset=TTimeIntervalSeconds(offsetSeconds);
	ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	messageOffset=smsMessage->UTCOffset();
	TEST(offset==messageOffset);

	//Value 1 more than maximum
	offsetSeconds=79*15*60+1;		//78 fifteen minute blocks in seconds, plus 1 sec
	TTimeIntervalSeconds outOfBoundsOffset(offsetSeconds);
	ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	TEST(outOfBoundsOffset!=messageOffset);
	TEST(offset==messageOffset);

	//Value -1
	offsetSeconds=-1;		//-1
	offset=TTimeIntervalSeconds(offsetSeconds);
	ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	messageOffset=smsMessage->UTCOffset();
	TEST(offset==messageOffset);

	//Negative boundaries
	offsetSeconds=79*15*60*-1;		//79 fifteen minute blocks in seconds, negative
	offset=TTimeIntervalSeconds(offsetSeconds);
	ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	messageOffset=smsMessage->UTCOffset();
	TEST(offset==messageOffset);

	//Value 1 less than negative maximum
	offsetSeconds=(79*15*60*-1)-1;		//79 fifteen minute blocks in seconds negative, minus 1 sec
	outOfBoundsOffset=TTimeIntervalSeconds(offsetSeconds);
	ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	TEST(outOfBoundsOffset!=messageOffset);
	TEST(offset==messageOffset);

	//Check clearing bits
	offsetSeconds=0xFFFF;		//0 1111 1111 1111 1111
	offset=TTimeIntervalSeconds(offsetSeconds);
	ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	messageOffset=smsMessage->UTCOffset();
	TEST(offset==messageOffset);

	//Clear the bits
	offsetSeconds=0x10000;		//1 0000 0000 0000 0000
	offset=TTimeIntervalSeconds(offsetSeconds);
	ret=smsMessage->SetUTCOffset(offset);
	TEST(ret==result);
	messageOffset=smsMessage->UTCOffset();
	TEST(offset==messageOffset);

	CleanupStack::PopAndDestroy(smsMessage);
	User::SetUTCTimeAndOffset(currUTCTime,currUTCOffset);
	return TestStepResult();
}

TVerdict CTestCSmsMessageWithDeliverPDU::doTestStepL()
{
/**
 *  Creating a CSmsMessage object when receiving a message with a deliver PDU
 */
	const TInt KMaxAllowanceSeconds=4;
	const TInt KMaxRxPeriod=60;
	TTimeIntervalSeconds allowableDelay(KMaxAllowanceSeconds);
	TTimeIntervalSeconds rxPeriod(KMaxRxPeriod);

	INFO_PRINTF1(_L("Test Simple Tx and Rx SMS"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//Create the message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Wait for first SMS
	WaitForRecvL(socket);

	//Before we create the message received, we get the UTC time and the offset.
	TTime utcTime;
	utcTime.UniversalTime();
	TTimeIntervalSeconds offset=User::UTCOffset();

	//Receive first message
	//Messages are received at 1 minute intervals
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	INFO_PRINTF1(_L("incoming first SMS") );

	//Checks make on message
	//iTime is within 2 seconds of UTC
	TTime messageTime=smsMessage->Time();
	TEST(messageTime>=utcTime-allowableDelay&&messageTime<=utcTime+allowableDelay);
	TTimeIntervalSeconds messageOffset=smsMessage->UTCOffset();
	TEST(messageOffset==offset);

	//Receive second message
	//This message will be received after we have set UTC
	//UTC=1999/09/16 22hr35min45sec, offset=55sec
	TInt year=1999;
	TMonth month=ESeptember;
	TInt day=15; //16th
	TInt hour=22;
	TInt minute=35;
	TInt second=45;
	TInt milliseconds=0;
	TDateTime specificDateTime(year,month,day,hour,minute,second,milliseconds);
	TTime specificUTCTime(specificDateTime);
	TInt specificOffsetValue=55;
	TTimeIntervalSeconds specificOffset(specificOffsetValue);
	User::SetUTCTimeAndOffset(specificUTCTime,specificOffset);

	CleanupStack::PopAndDestroy(smsMessage);

	//	Wait for second message to arrive.  Must be round 60 seconds
	WaitForRecvL(socket);

	//Receive second message
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	INFO_PRINTF1(_L("incoming second SMS") );

	//Checks make on message
	messageTime=smsMessage->Time();
	TEST(messageTime>=specificUTCTime+rxPeriod-allowableDelay&&messageTime<=specificUTCTime+rxPeriod+allowableDelay);
	messageOffset=smsMessage->UTCOffset();
	TEST(messageOffset==specificOffset);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive three PDU's at 1 minute intervals.
	//We set UTC prior to receiving anything.
	//PDU will contain time center stamp with different offsets.
	//The test checks that the time stamp of the PDU inside the message contains the
	//time stamp of the first PDU received.

	User::SetUTCOffset(TTimeIntervalSeconds(0));
	WaitForRecvL(socket);
	//Receive third message
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Check the pdu time stamp offset.
	//Expected UTC = 2001/03/06 10hr19min35sec, offset = 0
	year=2001;
	month=EMarch;
	day=5; 			//6th
	hour=10;
	minute=19;
	second=35;
	milliseconds=0;

	TDateTime expectedServiceCenterTimeStamp(year,month,day,hour,minute,second,milliseconds);
	TTime expectedTime(expectedServiceCenterTimeStamp);
	TTime timeStamp;
	TInt timeStampOffset=0;
	CSmsDeliver& pdu=static_cast<CSmsDeliver&>(smsMessage->SmsPDU());
	pdu.ServiceCenterTimeStamp(timeStamp,timeStampOffset);
	TEST(timeStampOffset==0);
	TEST(expectedTime==timeStamp);

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
   	return TestStepResult();
}


TVerdict CTestCSmsMessageWithStatusReportPDU::doTestStepL()
/**
 *  Test the timestamp in a statusReport PDU
 */
	{
	INFO_PRINTF1(_L("Test Tx an SMS and then receive a status report"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsWithStatusReportReqL(KTestMsg1,alphabet);
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	TSmsServiceCenterAddress telephoneNumber;
	telephoneNumber.Copy( KPekka );

	// Receive status report
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	//Get the status report
	CleanupStack::PushL(smsMessage);
	TEST(smsMessage->Type()==CSmsPDU::ESmsStatusReport);
	TSmsServiceCenterAddress messageTelephoneNumber=smsMessage->ToFromAddress();
	TEST(telephoneNumber==messageTelephoneNumber);

	//Check PDU
	TTime timeStamp;
	TInt timeStampOffset;
	CSmsStatusReport& pdu=static_cast<CSmsStatusReport&>(smsMessage->SmsPDU());
	//Expected service center time stamp
	TInt year=2004;
	TMonth month=EJune;
	TInt day=14; 	//15th
	TInt hour=4;
	TInt minute=0;
	TInt second=12;
	TInt milliseconds=0;
	TTimeIntervalSeconds offset(-5);

	TDateTime expectedServiceCenterTimeStamp(year,month,day,hour,minute,second,milliseconds);
	TTime expectedTime(expectedServiceCenterTimeStamp);
	pdu.ServiceCenterTimeStamp(timeStamp,timeStampOffset);
	TEST(timeStampOffset==offset.Int());
	TEST(expectedTime==timeStamp);
	//TDateTime actualDateTime=timeStamp.DateTime();

	//and discharge time
	year=2004;
	month=EAugust;
	day=25; 	//26th
	hour=01;
	minute=13;
	second=3;
	milliseconds=0;
	offset=TTimeIntervalSeconds(-9);
	expectedServiceCenterTimeStamp=TDateTime(year,month,day,hour,minute,second,milliseconds);
	expectedTime=TTime(expectedServiceCenterTimeStamp);

	pdu.DischargeTime(timeStamp,timeStampOffset);
	TEST(timeStampOffset==offset.Int());
	TEST(expectedTime==timeStamp);
	//actualDateTime=timeStamp.DateTime();

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult() ;
	}

TVerdict CTestCSmsMessageWithSubmitPDU::doTestStepL()
/**
 *  Test creating a sms message with a submit pdu.  PDU timestamp should reflect
 *  UTC value, offset and validity period.
 */
	{
	INFO_PRINTF1(_L("Test Tx an SMS and let SIMTSY validate the validityPeriod in the submit pdu"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//--------------------------------------------------------
	//	Set the universal time
	//Setup a date time
	TInt year=2009;
	TMonth month=EMay;
	TInt day=25;
	TInt hour=5;
	TInt minute=40;
	TInt seconds=15;
	TInt milliseconds=0;
	TDateTime testTime(year,month,day,hour,minute,seconds,milliseconds);
	TTime utcTime(testTime);
		
	//Offset
	TInt offsetSeconds=55+(60*20)+(60*60*4);			//55 seconds, 20 min, 4 hours
	TTimeIntervalSeconds offset(offsetSeconds);
	//Set UTC and offset
	User::SetUTCTimeAndOffset(utcTime,offset);		
	//--------------------------------------------------------
	
	//Create a message
	_LIT(KTestMsg,"test message, 8bits, length 30");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//	Test validity period of submit pdu
	TEST(smsMessage->Type()==CSmsPDU::ESmsSubmit);

	//Get submit pdu
	TTimeIntervalMinutes expectedPeriod(24*60);		//default set to 1 day
	CSmsSubmit& pdu=static_cast<CSmsSubmit&>(smsMessage->SmsPDU());
	TTimeIntervalMinutes validityPeriod=pdu.ValidityPeriod();
	TEST(validityPeriod==expectedPeriod);
	TTimeIntervalMinutes newValidityPeriod(24*60*4);
	pdu.SetValidityPeriod(newValidityPeriod);
	//TSmsFirstOctet validityPeriodFormat=TSmsFirstOctet::ESmsVPFSemiOctet;

	pdu.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFSemiOctet);
	
	//Send the message
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestHandlePID40h::doTestStepL()
/**
 *  Test for DEF055800
 *  Check that PDUs with PID = 0x40 are acknowledged but not forwarded to
 *  the inbox.
 */
 	{
    INFO_PRINTF1(_L("Check that PDUs with PID = 0x40 are acknowledged but not forwarded to the inbox."));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);

	//Receive 2 PDUs
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KTestMsg1);

	//Save the received message to the SMS storage
	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	WriteSmsToSimL(*smsMessage, socket);
	CleanupStack::PopAndDestroy(smsMessage);

	// Enumerate messages from Store
	RPointerArray<CSmsMessage> messages;
	ReadSmsStoreL(socket, messages);
	// The standard PDU should be in the SIM store, along with two other messages
	// The PDU with PID 0x40 should be NOT be present
	TESTCHECK(messages.Count(), 3, "Expecting 3 messages in SIM Store" );
	messages.ResetAndDestroy();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestDiskSpaceMidRangeClass0Class2::doTestStepPreambleL()
    {
    CSmsBaseTestStep::doTestStepPreambleL();

#ifdef _DEBUG    
    TInt err = RProperty::Define(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey, RProperty::EInt);
    if ((err != KErrNone) && (err != KErrAlreadyExists))    
        {
        ERR_PRINTF2(_L("RProperty::Define() failure [err=%d]"), err);
        User::Leave(err);
        }    
#endif
    
    return TestStepResult();
    }

/**
 *  Receive class0 and class2 messages when DiskSpace in mid range. Only the class 0
 *  message will be received initially, but the class 2 will be delivered when the
 *  free disk space returns.
 *
 *  @note This test can only be run when the SMS Stack is in debug mode.
 *
 */
TVerdict CTestDiskSpaceMidRangeClass0Class2::doTestStepL()
	{
#ifndef _DEBUG
	INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else
	//
	// Disk space should be available at the start...
	//
	TInt  diskSpaceStatus(ESmsDiskSpaceUnknown);
	TInt  ret(KErrUnknown);
	
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceAvailable);

	//
	// Setup the .RSC file and free disk space to be in the mid-range...
	//
	TUint highDrop = 4;
	TUint lowDrop = 10;
	TUint freeDrop = 7;
	
	SetHighLowLimitsAndDiskSpaceLevelL(highDrop, lowDrop, freeDrop);

	//
	// Load the SMS Stack...
	//
	RSocket  socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrRecvAny);
	
	//
	// Attempt to receive an SMS - Class 0 message should arrive...
	//
	INFO_PRINTF1(_L("Receiving SMS message - Class 0 message should arrive..."));
	
    CSmsMessage* message1 = CreateSMSL();
    CleanupStack::PushL(message1);
    SendSmsL(message1, socket);
    CleanupStack::PopAndDestroy(message1);    

	TBool  messageReceived;
	messageReceived = TimedWaitForRecvL(socket, 10*1000000);
	if( messageReceived )
		{
		CSmsMessage*  sms = RecvSmsL(socket);

		INFO_PRINTF1(_L("Message received!"));

		TSmsDataCodingScheme::TSmsClass  msgClass;

		if (!sms->SmsPDU().DataCodingSchemePresent()  ||
			!sms->SmsPDU().Class(msgClass)  ||
			msgClass != TSmsDataCodingScheme::ESmsClass0)
			{
			ERR_PRINTF1(_L("Message was not Class 0!"));
			SetTestStepResult(EFail);
			}

		delete sms;
		}
	else
		{
		ERR_PRINTF1(_L("Message not received!"));
		SetTestStepResult(EFail);
		}

	// 1 - rx now triggered from tx; more deterministic
	// 2 - if class 0 received, disk space available.
	// 
	// But I agree with comment below - this seems like a defect in disk monitor
	// as it depends on order of received messages and not actuall OOD situation!
	// Disk monitor needs to be updated to give diskfull status when below high
	// but above low.
	// 
	// Old comment below;
	// Disk space should not be available, even though the Class 0 came through okay.
	// Potentially I think this maybe a defect in the Disk Space Monitor class.
	//
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceAvailable);

	//
	// Attempt to receive an SMS - Class 2 message should not arrive...
	//
	INFO_PRINTF1(_L("Receiving SMS message - Class 2 message should not arrive..."));
	
    CSmsMessage* message2 = CreateSMSL();
    CleanupStack::PushL(message2);
    SendSmsL(message2, socket);
    CleanupStack::PopAndDestroy(message2);    

    messageReceived = TimedWaitForRecvL(socket, 10*1000000);
	if (messageReceived)
		{
		CSmsMessage*  sms = RecvSmsL(socket);

		ERR_PRINTF1(_L("Message received!"));
		SetTestStepResult(EFail);

		delete sms;
		}
	else
		{
		INFO_PRINTF1(_L("Message not received!"));
		}

	//
	// Disk space should not be available now...
	//
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceFull);

	//
	// Raise the disk space to above the high limit...
	//
	INFO_PRINTF1(_L("Raise the disk space to above the high limit"));
	
    freeDrop = 2;
    ReleaseDiskSpaceL();
    SetFreeDiskSpaceFromDropLevelL(freeDrop);	

	//
	// Attempt to receive an SMS - Class 2 message should arrive...
	//
	INFO_PRINTF1(_L("Receiving SMS message - Class 2 message should arrive..."));
	
    CSmsMessage* message3 = CreateSMSL();
    CleanupStack::PushL(message3);
    SendSmsL(message3, socket);
    CleanupStack::PopAndDestroy(message3);    

    messageReceived = TimedWaitForRecvL(socket, 10*1000000);
	if (messageReceived)
		{
		CSmsMessage*  sms = RecvSmsL(socket);

		INFO_PRINTF1(_L("Message received!"));

		TSmsDataCodingScheme::TSmsClass  msgClass;

		if (!sms->SmsPDU().DataCodingSchemePresent()  ||
			!sms->SmsPDU().Class(msgClass)  ||
			msgClass != TSmsDataCodingScheme::ESmsClass2)
			{
			ERR_PRINTF1(_L("Message was not Class 2!"));
			SetTestStepResult(EFail);
			}

		delete sms;
		}
	else
		{
		ERR_PRINTF1(_L("Message not received!"));
		SetTestStepResult(EFail);
		}

	//
	// Disk space should be available now...
	//
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceAvailable);

	//
	// Close the SMS Stack...
	//
	ReleaseDiskSpaceL();
	CleanupStack::PopAndDestroy(&socket);
#endif

	return TestStepResult() ;
	}


/**
 *  Post-amble funtion to ensure the environment is reverted to normal.
 */
TVerdict CTestDiskSpaceMidRangeClass0Class2::doTestStepPostambleL()
    {
#ifdef _DEBUG   
    TInt err = RProperty::Delete(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey);
    if (err != KErrNone && err != KErrNotFound)
        {
        ERR_PRINTF2(_L("RProperty::Delete() failure [err=%d]"), err);
        }   
#endif
    
    //
    // Restore the environment to normal and then call the previous post-amble
    // function.
    //
    RemoveLowHighLimitsFromSmsuRscL();
    
    CSmsBaseTestStep::doTestStepPostambleL();

    return TestStepResult() ;
    }

TVerdict CTestDiskSpaceLowRangeClass0Class2::doTestStepPreambleL()
    {
    CSmsBaseTestStep::doTestStepPreambleL();

#ifdef _DEBUG    
    TInt err = RProperty::Define(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey, RProperty::EInt);
    if ((err != KErrNone) && (err != KErrAlreadyExists))    
        {
        ERR_PRINTF2(_L("RProperty::Define() failure [err=%d]"), err);
        User::Leave(err);
        }    
#endif
    
    return TestStepResult();
    }

/**
 *  Set the disk space to below the low limit, attempt to receive class 0 and
 *  class 2, raise disk space and receive class 0 and class 2.  Class 0 and
 *  class 2 messages rejected when disk space below the low limit, received
 *  when disk space increases.
 *
 *  @note This test can only be run when the SMS Stack is in debug mode.
 *
 */
TVerdict CTestDiskSpaceLowRangeClass0Class2::doTestStepL()
	{
#ifndef _DEBUG
	INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else
	//
	// Disk space should be available at the start...
	//
	TInt  diskSpaceStatus(ESmsDiskSpaceUnknown);
	TInt  ret(KErrUnknown);
	
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceAvailable);

	//
	// Setup the .RSC file and free disk space to be in the low-range...
	//
    TUint highDrop = 4;
    TUint lowDrop = 10;
    TUint freeDrop = 11;
    
    SetHighLowLimitsAndDiskSpaceLevelL(highDrop, lowDrop, freeDrop);
	
	RSocket  socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrRecvAny);
	
	//
	// Attempt to receive an SMS - Class 0 message should not arrive...
	//
	INFO_PRINTF1(_L("Receiving SMS message - Class 0 message should not arrive..."));
	TBool  messageReceived;

	messageReceived = TimedWaitForRecvL(socket, 10*1000000);
	if (messageReceived)
		{
		CSmsMessage*  sms = RecvSmsL(socket);

		ERR_PRINTF1(_L("Message received!"));
		SetTestStepResult(EFail);
		delete sms;
		}
	else
		{
		INFO_PRINTF1(_L("Message not received!"));
		}

	//
	// Disk space should not be available...
	//
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceFull);

	//
	// Attempt to receive an SMS - Class 2 message should not arrive...
	//
	INFO_PRINTF1(_L("Receiving SMS message - Class 2 message should not arrive..."));

	messageReceived = TimedWaitForRecvL(socket, 10*1000000);
	if (messageReceived)
		{
		CSmsMessage*  sms = RecvSmsL(socket);

		ERR_PRINTF1(_L("Message received!"));
		SetTestStepResult(EFail);
		delete sms;
		}
	else
		{
		INFO_PRINTF1(_L("Message not received!"));
		}

	//
	// Disk space should still not be available...
	//
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceFull);

	//
	// Raise the disk space to above the high limit...
	//
    freeDrop = 2;
    ReleaseDiskSpaceL();
    SetFreeDiskSpaceFromDropLevelL(freeDrop);
	
	//
	// Attempt to receive an SMS - Class 0 message should arrive...
	//
	INFO_PRINTF1(_L("Receiving SMS message - Class 0 message should arrive..."));

	messageReceived = TimedWaitForRecvL(socket, 10*1000000);
	if (messageReceived)
		{
		CSmsMessage*  sms = RecvSmsL(socket);

		INFO_PRINTF1(_L("Message received!"));

		TSmsDataCodingScheme::TSmsClass  msgClass;

		if (!sms->SmsPDU().DataCodingSchemePresent()  ||
			!sms->SmsPDU().Class(msgClass)  ||
			msgClass != TSmsDataCodingScheme::ESmsClass0)
			{
			ERR_PRINTF1(_L("Message was not Class 0!"));
			SetTestStepResult(EFail);
			}

		delete sms;
		}
	else
		{
		ERR_PRINTF1(_L("Message not received!"));
		SetTestStepResult(EFail);
		}

	//
	// Disk space should be available now...
	//
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceAvailable);

	//
	// Attempt to receive an SMS - Class 2 message should arrive...
	//
	INFO_PRINTF1(_L("Receiving SMS message - Class 2 message should arrive..."));

	messageReceived = TimedWaitForRecvL(socket, 10*1000000);
	if (messageReceived)
		{
		CSmsMessage*  sms = RecvSmsL(socket);

		INFO_PRINTF1(_L("Message received!"));

		TSmsDataCodingScheme::TSmsClass  msgClass;

		if (!sms->SmsPDU().DataCodingSchemePresent()  ||
			!sms->SmsPDU().Class(msgClass)  ||
			msgClass != TSmsDataCodingScheme::ESmsClass2)
			{
			ERR_PRINTF1(_L("Message was not Class 2!"));
			SetTestStepResult(EFail);
			}

		delete sms;
		}
	else
		{
		ERR_PRINTF1(_L("Message not received!"));
		SetTestStepResult(EFail);
		}

	//
	// Disk space should still be available now...
	//
	ret = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
						 diskSpaceStatus);
	TEST(ret == KErrNone);
	TEST(diskSpaceStatus == ESmsDiskSpaceAvailable);

	//
	// Close the SMS Stack...
	//
	ReleaseDiskSpaceL();
	CleanupStack::PopAndDestroy(&socket);
    
#endif

	return TestStepResult() ;
	} // CTestDiskSpaceLowRangeClass0Class2::doTestStepL

/**
 *  Post-amble funtion to ensure the environment is reverted to normal.
 */
TVerdict CTestDiskSpaceLowRangeClass0Class2::doTestStepPostambleL()
	{
#ifdef _DEBUG   
    TInt err = RProperty::Delete(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey);
    if (err != KErrNone && err != KErrNotFound)
        {
        ERR_PRINTF2(_L("RProperty::Delete() failure [err=%d]"), err);
        }       
#endif
	//
	// Restore the environment to normal and then call the previous post-amble
	// function.
	//
	RemoveLowHighLimitsFromSmsuRscL();
	
	CSmsBaseTestStep::doTestStepPostambleL();

	return TestStepResult() ;
	} // CTestDiskSpaceLowRangeClass0Class2::doTestStepPostambleL

/**
 *  Simple test of receiving a few corrupt PDUs and then one good one.
 */
TVerdict CTestCorruptPDUs::doTestStepL()
	{
	RSocket  socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, ESmsAddrRecvAny);
	
	// Attempt to receive the valid SMS...
	WaitForRecvL(socket);
	CSmsMessage*  sms = RecvSmsL(socket);
	delete sms;

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

/**
 *  Tests the reception of a series of messages which cause the assembly to become full,
 *  SMS reception to be suspended and then resumed.
 */
TVerdict CTestCongestedReceive::doTestStepL()
	{
	RSocket socket;
	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer, socket, smsaddr);

	for (TInt  count = 1;  count <= 30; ++count)
		{
		INFO_PRINTF2(_L("Waiting for incoming SMS %d..."), count);

		WaitForRecvL(socket);
		User::After(3*1000000);

		CSmsMessage*  smsMessage = RecvSmsL(socket);
		delete smsMessage;
		}

	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult();
	}

enum TVerdict CTestEncodingPDUonBoundary::doTestStepL()
/**
 * INC119830
 * * 
 * Boundary Test: force the GSMU to attempt to add a format IE
 * into a full PDU. This should result in the format IE being 
 * encoded into the 2nd PDU.
 */
	{
	INFO_PRINTF1(_L("Test Encoding PDU on Boundary"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);
	
	ChangeReceiveModeL(RMobileSmsMessaging::EReceiveUnstoredClientAck);

	//	Create a 149 character message (from 4 30 character messages + 1 29 character message).
	//  Set the encoding to 7 bit encoding. There should be sufficient space in the PDU to
	//  encode the 1st format IE, but this will leave the PDU full. The 2nd format IE
	//  then needs to be added to the next PDU.
	_LIT(KTestMsg1,"test message, 8bits, length 30test message, 8bits, length 30test message, 8bits, length 30test message, 8bits, length 30test message, 8bits, length 3");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;
	
	TSmsDataCodingScheme::TSmsAlphabet alphabet7Bit=TSmsDataCodingScheme::ESmsAlphabet7Bit;
		
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet7Bit);

	// Create the format object
	CEmsFormatIE* format = CEmsFormatIE::NewL();
	format->SetStartPosition(147);
	format->SetFormatLength(1);
	format->SetAlignment(CEmsFormatIE::ERight);
	CleanupStack::PushL(format);
	smsMessage->AddEMSInformationElementL(*format);
	CleanupStack::PopAndDestroy(format);

	// Create the format object
	CEmsFormatIE* format1 = CEmsFormatIE::NewL();
	format->SetStartPosition(148);
	format->SetFormatLength(1);
	format->SetAlignment(CEmsFormatIE::ERight);
	CleanupStack::PushL(format1);
	smsMessage->AddEMSInformationElementL(*format1);
	CleanupStack::PopAndDestroy(format1);

	TInt num = smsMessage->NumMessagePDUsL();
	TEST(num == 2);
	
	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);

	//Send SMS
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive SMS
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,KTestMsg1);

	num = smsMessage->NumMessagePDUsL();	
	TEST(num == 2);
		
	//Save the received message to the SMS storage	
	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	WriteSmsToSimL(*smsMessage, socket);
	CleanupStack::PopAndDestroy(smsMessage);

	// Enumerate messages from Store
	RPointerArray<CSmsMessage> messages;
	ReadSmsStoreL(socket, messages);
	messages.ResetAndDestroy();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;	
	}

/**
 *  Tests that a PDU can be received if it has an invalid or reserved IE number.
 */
TVerdict CTestReceiveInvalidOrReservedIE::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	// Receive the SMS with the reserved IE value.
	WaitForRecvL(socket);
	CSmsMessage*  smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	_LIT(KTestMsg, "I am in an emergency. Please help me.");
	TestSmsContentsL(smsMessage, KTestMsg);

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

/**
 *  Tests that standard GSM characters can be sent and received using 7bit
 *  without loss of characters.  
 */
TVerdict CTestEncoding7bitStandardGSM::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//
	// Test standard GSM text (default table with no shift characters)...
	//
	iCharSets.Append(KCharacterSetStandard7bitGSM());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("GSM 7bit text"),
		/* Sequences */			3,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingNone,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();

	//
	// Test standard GSM text including default shift characters...
	//
	iCharSets.Append(KCharacterSetStandard7bitGSM());
	iCharSets.Append(KCharacterSetStandard7bitGSMShift());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("GSM 7bit text including default shift characters"),
		/* Sequences */			3,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingNone,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/**
 *  Tests that standard 8bit characters can be sent and received without
 *  loss of characters.
 */
TVerdict CTestEncoding8bitCodePage1252::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//
	// Test standard 8bit text (really 8bit data)...
	//
	iCharSets.Append(KCharacterSetStandard8bitGSM());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("8bit text"),
		/* Sequences */			2,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet8Bit,
		/* Encoding to use */	ESmsEncodingNone,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();
	
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/**
 *  Tests that standard unicode characters can be sent and received without
 *  loss of characters.
 */
TVerdict CTestEncodingUnicode::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//
	// Test unicode text (not really GSM specific)...
	//
	iCharSets.Append(KCharacterSetVariousUCS2());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("various UCS2 characters"),
		/* Sequences */			2,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabetUCS2,
		/* Encoding to use */	ESmsEncodingNone,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/**
 *  Tests that unconvertible GSM characters can be sent using 7bit by being
 *  set to the replacement character.
 */
TVerdict CTestEncoding7bitUnconvertibleGSM::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//
	// Test various characters not supported in the 7bit ASCII text (either
	// default GSM, shifted GSM, downgraded by the internal converter or
	// downgraded by the PREQ2090 converter.
	//
	iCharSets.Append(KCharacterSetUnconvertible7bitGSM());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("unconvertible-GSM 7bit text"),
		/* Sequences */			3,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingNone,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/**
 *  Tests that non-standard GSM characters can be sent and received using 7bit
 *  with downgraded conversions.
 */
TVerdict CTestEncoding7bitNonStandardGSM::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//
	// Test downgrades that were part of the internal converter
	// (e.g. pre-PREQ2090)...
	//
	iCharSets.Append(KCharacterSetDowngrading7bitGSM());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("non-GSM 7bit text (non-PREQ2090)"),
		/* Sequences */			3,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingNone,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();
	
	//
	// Test downgrades that were part of the new PREQ2090 converter...
	//
	iCharSets.Append(KCharacterSetDowngrading7bitGSMExtended());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("non-GSM 7bit text (PREQ2090)"),
		/* Sequences */			3,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingNone,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/**
 *  Tests that Turkish characters can be sent and received using 7bit without
 *  loss of characters.
 */
TVerdict CTestEncoding7bitTurkishNationalLanguage::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//
	// Test Turkish GSM text (no alternative encoding)...
	//
	iCharSets.Append(KCharacterSetDowngradingTurkish7bitGSM());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("Turkish GSM text (no alternative encoding)"),
		/* Sequences */			40,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingNone,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();

	//
	// Test Turkish GSM text (Turkish shift table)...
	//
	iCharSets.Append(KCharacterSetTurkish7bitGSMShift());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("Turkish GSM text (Turkish shift table)"),
		/* Sequences */			25,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingTurkishSingleShift,
		/* Encoding expected */	ESmsEncodingTurkishSingleShift);
	iCharSets.Reset();
	
	//
	// Test Turkish GSM text (Turkish locking table)...
	//
	iCharSets.Append(KCharacterSetTurkish7bitGSMLocking());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("Turkish GSM text (Turkish locking table)"),
		/* Sequences */			25,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingTurkishLockingShift,
		/* Encoding expected */	ESmsEncodingTurkishLockingShift);
	iCharSets.Reset();
	
	//
	// Test Turkish GSM text (Turkish locking and shift table)...
	//
	// In practice the algorithm will pick shift as it is cheaper.
	//
	//iCharSets.Append(KCharacterSetTurkish7bitGSMLocking());
	iCharSets.Append(KCharacterSetTurkish7bitGSMShift());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("Turkish GSM text (Turkish locking and shift table)"),
		/* Sequences */			40,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingTurkishLockingAndSingleShift,
		/* Encoding expected */	ESmsEncodingTurkishSingleShift);
	iCharSets.Reset();

	//
	// Test normal GSM text but with Turkish locking and shift table...
	//
	iCharSets.Append(KCharacterSetStandard7bitGSM());
	iCharSets.Append(KCharacterSetStandard7bitGSMShift());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("normal GSM text but with Turkish locking and shift table"),
		/* Sequences */			3,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingTurkishLockingAndSingleShift,
		/* Encoding expected */	ESmsEncodingNone);
	iCharSets.Reset();
	
	//
	// Test Turkish GSM text with other downgrades (Turkish locking and shift table)...
	//
	iCharSets.Append(KCharacterSetTurkish7bitGSMShift());
	iCharSets.Append(KCharacterSetNonTurkishDowngrading7bitGSMChars());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("Turkish GSM text with other downgrades (Turkish locking and shift table)"),
		/* Sequences */			3,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingTurkishLockingAndSingleShift,
		/* Encoding expected */	ESmsEncodingTurkishSingleShift);
	iCharSets.Reset();

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/**
 *  Generates various messages with some Turkish and unconvertible characters,
 *  sends/receives them and compares the contents.
 */
TVerdict CTestEncoding7bitTurkishAndUnconvertible::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//
	// Test mix of Turkish and unconvertible characters...
	//
	iCharSets.Append(KCharacterSetTurkish7bitGSMShift());
	iCharSets.Append(KCharacterSetNonTurkishDowngrading7bitGSMChars());
	iCharSets.Append(KCharacterSetUnconvertible7bitGSM());
	DoEncodingTestL(
		/* Socket */			socket,
		/* Test */				_L("Turkish 7bit and unicode (to be downgraded)"),
		/* Sequences */			1,
		/* Char sets */			iCharSets,
		/* DCS */				TSmsDataCodingScheme::ESmsAlphabet7Bit,
		/* Encoding to use */	ESmsEncodingTurkishLockingAndSingleShift,
		/* Encoding expected */	ESmsEncodingTurkishSingleShift);
	iCharSets.Reset();
	
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}  // CTestEncoding7bitTurkishAndUnconvertible::doTestStepL


/**
 *  Generates three messages using the three encodings (7bit, 8bit and 16bit).
 *  Each message is three PDUs long and use the same DCS throughout. Then the
 *  SIMTSY will return 3 mixed PDUs (each one starting with a different DCS).
 *  The test ensures they are received correctly.
 */
TVerdict CTestReceivingMessageWithDifferentEncodings::doTestStepL()
	{
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//
	// Generate 3 messages for each encoding type that will require 3 PDUs...
	//
	_LIT(KTestPhrase7bit,  "I like apples! ");
	_LIT(KTestPhrase8bit,  "I like pears! ");
	_LIT(KTestPhrase16bit, "I like bananas! ");
	TInt  pduSize7bit   = (160 - 8);
	TInt  pduSize8bit   = (140 - 6);
	TInt  pduSize16bit  = (70 - 3);
	TInt  repeats7bit   = ((pduSize7bit * 3) / KTestPhrase7bit().Length());
	TInt  repeats8bit   = ((pduSize8bit * 3) / KTestPhrase8bit().Length());
	TInt  repeats16bit  = ((pduSize16bit * 3) / KTestPhrase16bit().Length());
	HBufC*  msg7BitBuf  = HBufC::NewLC(pduSize7bit * 3);
	HBufC*  msg8BitBuf  = HBufC::NewLC(pduSize8bit * 3);
	HBufC*  msg16BitBuf = HBufC::NewLC(pduSize16bit * 3);
	TPtr  msg7BitPtr    = msg7BitBuf->Des();
	TPtr  msg8BitPtr    = msg8BitBuf->Des();
	TPtr  msg16BitPtr   = msg16BitBuf->Des();
	TInt  count;
	
	for (count = 0;  count < repeats7bit;  count++)
		{
		msg7BitPtr.Append(KTestPhrase7bit);
		}
	
	for (count = 0;  count < repeats8bit;  count++)
		{
		msg8BitPtr.Append(KTestPhrase8bit);
		}
	
	for (count = 0;  count < repeats16bit;  count++)
		{
		msg16BitPtr.Append(KTestPhrase16bit);
		}
	
	//
	// Send all three messages and receive them to ensure they are valid...
	//
	CSmsMessage*  smsMessage;
	TInt  pdus;
	
	smsMessage = CreateSmsMessageLC(msg7BitPtr, TSmsDataCodingScheme::ESmsAlphabet7Bit);
	pdus = smsMessage->NumMessagePDUsL();
	TESTCHECK(pdus, 3, "Expected 3 PDUs");
	TRAPD(sendErr, SendSmsL(smsMessage, socket));
	TESTCHECK(sendErr, KErrNone, "Sending SMS");
	CleanupStack::PopAndDestroy(smsMessage);
	
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage, msg7BitPtr);
	CleanupStack::PopAndDestroy(smsMessage);
	
	smsMessage = CreateSmsMessageLC(msg8BitPtr, TSmsDataCodingScheme::ESmsAlphabet8Bit);
	pdus = smsMessage->NumMessagePDUsL();
    TESTCHECK(pdus, 3, "Expected 3 PDUs");
    
	TRAP(sendErr, SendSmsL(smsMessage, socket));
    TESTCHECK(sendErr, KErrNone, "Sending SMS");
	CleanupStack::PopAndDestroy(smsMessage);
	
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage, msg8BitPtr);
	CleanupStack::PopAndDestroy(smsMessage);
	
	smsMessage = CreateSmsMessageLC(msg16BitPtr, TSmsDataCodingScheme::ESmsAlphabetUCS2);
	pdus = smsMessage->NumMessagePDUsL();
    TESTCHECK(pdus, 3, "Expected 3 PDUs");
    
	TRAP(sendErr, SendSmsL(smsMessage, socket));
    TESTCHECK(sendErr, KErrNone, "Sending SMS");
    
	CleanupStack::PopAndDestroy(smsMessage);
	
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage, msg16BitPtr);
	CleanupStack::PopAndDestroy(smsMessage);
	
	//
	// Now receive the multi-DCS messages (that we mixed up by hand in SIMTSY)...
	//
	HBufC*  incomingBuf = HBufC::NewLC(160 * 3);
	TPtr  incomingPtr   = incomingBuf->Des();
	
	INFO_PRINTF1(_L("Waiting for the first mixed up SMS (7bit/8bit/16bit)..."));
	
	incomingPtr.Zero();
	incomingPtr.Append(msg7BitPtr.Mid(0, 153));
	incomingPtr.Append(msg8BitPtr.Mid(134, 134));
	incomingPtr.Append(msg16BitPtr.Mid(134));
	
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage, incomingPtr);
	CleanupStack::PopAndDestroy(smsMessage);
	
	INFO_PRINTF1(_L("Waiting for the second  mixed up SMS (8bit/16bit/7bit)..."));
	
	incomingPtr.Zero();
	incomingPtr.Append(msg8BitPtr.Mid(0, 134));
	incomingPtr.Append(msg16BitPtr.Mid(67, 67));
	incomingPtr.Append(msg7BitPtr.Mid(306));
	
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage, incomingPtr);
	CleanupStack::PopAndDestroy(smsMessage);
	
	INFO_PRINTF1(_L("Waiting for the third  mixed up SMS (16bit/7bit/8bit)..."));
	
	incomingPtr.Zero();
	incomingPtr.Append(msg16BitPtr.Mid(0, 67));
	incomingPtr.Append(msg7BitPtr.Mid(153, 153));
	incomingPtr.Append(msg8BitPtr.Mid(268));
	
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage, incomingPtr);
	CleanupStack::PopAndDestroy(smsMessage);
	
	CleanupStack::PopAndDestroy(incomingBuf);
	CleanupStack::PopAndDestroy(msg16BitBuf);
	CleanupStack::PopAndDestroy(msg8BitBuf);
	CleanupStack::PopAndDestroy(msg7BitBuf);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}  // CTestReceivingMessageWithDifferentEncodings::doTestStepL


/**
 *  Generates a message which is requested but not read before the socket is closed.
 *  On re-request the message should still be available to the client to be read.
 */
TVerdict CTestReceivingMessageAfterSocketClosure::doTestStepL()
	{
	INFO_PRINTF1(_L("Test re-requesting to receive a message after socket closure."));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	// Wait for SMS message receipt.
	WaitForRecvL(socket);
	
	// Close socket before reading the message.
	CleanupStack::PopAndDestroy(&socket);
	
	// Bind the socket again
	INFO_PRINTF1(_L("binding socket again; checking for receipt") );
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	// Briefly wait for receipt on it - this should now happen again.
	TBool bRcv = TimedWaitForRecvL(socket, 5 * 1000000);
	TEST_CHECKL(bRcv, ETrue, _L("Receive after re-bind should succeed."));
	
	// Read the 1st SMS.
	CSmsMessage* smsMessage1 = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage1);

	TPtrC fromAddr1 = smsMessage1->ToFromAddress();
	INFO_PRINTF2(_L("1st received SMS from: %S"), &fromAddr1);
	
	// Now read the second SMS to make sure it is still available
	bRcv = TimedWaitForRecvL(socket, 5 * 1000000);
	TEST_CHECKL(bRcv, ETrue, _L("Receive after reading 1st message should also succeed."));
	
	CSmsMessage* smsMessage2 = RecvSmsL(socket);
    CleanupStack::PushL(smsMessage2);

    TPtrC fromAddr2 = smsMessage2->ToFromAddress();
    INFO_PRINTF2(_L("2st received SMS from: %S"), &fromAddr2);

	CleanupStack::PopAndDestroy(smsMessage2);
	CleanupStack::PopAndDestroy(smsMessage1);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
	}

TVerdict CTestSimpleTxAndRxWithLoggingDisabled::doTestStepL()
/**
 * Test that a simple SMS can be transmitted and received
 *  when the logging subsystem is disabled.
 */
	{
 	INFO_PRINTF1(_L("Test Simple Tx and Rx SMS"));
  	RSocket socket;
 	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer,socket,ESmsAddrRecvAny);
 	CleanupClosePushL(socket);
 
 	iSmsStackTestUtils->DisableLogging();
 	
 	_LIT(KTestMsg1,"test message, 8bits, length 30");
 
 	//Set destination and SC numbers
 	iTelephoneNumber=KPekka;
 	iServiceCenterNumber=KRadiolinjaSC;
 
 	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
 	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);
 
 	INFO_PRINTF2(_L("Destination number:..... %S"),&iTelephoneNumber);
 	INFO_PRINTF2(_L("ServiceCenter number:... %S"),&iServiceCenterNumber);
 
 	//Send SMS
 	SendSmsL(smsMessage,socket);
 	CleanupStack::PopAndDestroy(smsMessage);
 
 	//Receive SMS
 	WaitForRecvL(socket);
 	smsMessage = RecvSmsL(socket);
 	CleanupStack::PushL(smsMessage);

 	TestSmsContentsL(smsMessage,KTestMsg1);
 	CleanupStack::PopAndDestroy(smsMessage);
 
 	iSmsStackTestUtils->EnableLogging();	
 	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult() ;
 	}

/**
 *  Tests that Spanish, Portuguese and Turkish character converters are being used.
 */
TVerdict CTestEncoding7bitNationalLanguages::doTestStepL()
    {
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
    CleanupClosePushL(socket);

    //
    // Test use of Spanish GSM text using shift table...
    // Some of the characters chosen dont appear in Portuguese or Turkish table
    //
    iCharSets.Append(KCharacterSetSpanish7bitGSMShift()); 
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Spanish GSM text (Spanish shift table)"),
        /* Sequences */         1,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingSpanishSingleShift,
        /* Encoding expected */ ESmsEncodingSpanishSingleShift);
    iCharSets.Reset();  
    
    //
    // Test use of Portuguese GSM text using shift table...
    // Some of the characters chosen dont appear in Spanish or Turkish table
    //
    iCharSets.Append(KCharacterSetPortuguese7bitGSMShift()); 
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Portuguese GSM text (Portuguese shift table)"),
        /* Sequences */         1,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingPortugueseSingleShift,
        /* Encoding expected */ ESmsEncodingPortugueseSingleShift);
    iCharSets.Reset();  
    
    //
    // Test use of Portuguese GSM text using locking shift table...
    // Some of the characters chosen dont appear in Spanish or Turkish table
    //
    iCharSets.Append(KCharacterSetPortuguese7bitGSMLocking()); 
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Portuguese GSM text (Portuguese locking shift table)"),
        /* Sequences */         1,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingPortugueseLockingShift,
        /* Encoding expected */ ESmsEncodingPortugueseLockingShift);
    iCharSets.Reset();  
    
    //
    // Test use of Turkish GSM text using shift table...
    // Some of the characters chosen dont appear in Spanish or Portuguese table
    //
    iCharSets.Append(KCharacterSetTurkish7bitGSMShift()); 
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Turkish GSM text (Turkish shift table)"),
        /* Sequences */         1,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingTurkishSingleShift,
        /* Encoding expected */ ESmsEncodingTurkishSingleShift);
    iCharSets.Reset();  
    
    //
    // Test use of Turkish GSM text using locking shift table...
    // Some of the characters chosen dont appear in Spanish or Portuguese table
    //
    iCharSets.Append(KCharacterSetTurkish7bitGSMLocking()); 
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Turkish GSM text (Turkish locking shift table)"),
        /* Sequences */         1,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingTurkishLockingShift,
        /* Encoding expected */ ESmsEncodingTurkishLockingShift);
    iCharSets.Reset();  

    CleanupStack::PopAndDestroy(&socket);
    return TestStepResult();
    }

/**
 *  Tests that Portuguese character converters creates a multi PDU SMS message correctly.
 */
TVerdict CTestEncodingMultiPDUwith7bitNationalLanguages::doTestStepL()
    {
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
    CleanupClosePushL(socket);

    //
    // Test use of Portuguese GSM text using locking shift table...
    // Check that when one byte remains in the last PDU that adding a 2 byte character 
    // creates a new PDU with the 2 byte character and does not split the character between PDUs
    //
    // First add all the character types to ensure that the header in the PDU contains 
    // all the necessary information elements
    iCharSets.Append(KCharacterSetPortuguese7bitGSMExt());
    iCharSets.Append(KCharacterSetPortuguese7bitGSMLocking());   
    // Once sufficient characters are added to create a PDU with one free byte 
    // add a two byte shift character 
    iAdditionalCharSets.Append(KCharacterSetPortuguese7bitGSMExt());
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Portuguese GSM text (Portuguese lock table)"),
        /* characters */        198,
        /* Char sets */         iCharSets,
        /* additional characters */        1,
        /* Char sets */         iAdditionalCharSets,   
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingPortugueseLockingAndSingleShift,
        /* Encoding expected */ ESmsEncodingPortugueseLockingShift);
    iCharSets.Reset(); 
    iAdditionalCharSets.Reset();

    CleanupStack::PopAndDestroy(&socket);
    return TestStepResult();
    }

/**
 * Test Turkish, Portuguese and Spanish Character Downgrading
 */
TVerdict CTestEncodingDowngradedWith7bitNationalLanguages::doTestStepL()
    {
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
    CleanupClosePushL(socket);

    //
    // Test Turkish GSM text (no alternative encoding)...
    //
    iCharSets.Append(KCharacterSetDowngradingTurkish7bitGSM());
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Turkish GSM text (no alternative encoding)"),
        /* Sequences */         40,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingNone,
        /* Encoding expected */ ESmsEncodingNone);
    iCharSets.Reset();
    
    //
    // Test Spanish GSM text (no alternative encoding)...
    //
    iCharSets.Append(KCharacterSetDowngradingSpanish7bitGSM());
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Spanish GSM text (no alternative encoding)"),
        /* Sequences */         25,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingNone,
        /* Encoding expected */ ESmsEncodingNone);
    iCharSets.Reset();
    
    //
    // Test Portuguese GSM text (no alternative encoding)...
    //
    iCharSets.Append(KCharacterSetDowngradingPortuguese7bitGSM());
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Portuguese GSM text (no alternative encoding)"),
        /* Sequences */         20,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingNone,
        /* Encoding expected */ ESmsEncodingNone);
    iCharSets.Reset();

    CleanupStack::PopAndDestroy(&socket);
    return TestStepResult();
    }

/**
 * Test Portuguese GSM text (Portuguese locking and shift table)...
 */
TVerdict CTestOptimumEncodingWithLockingAndShift7bitNationalLanguages::doTestStepL()
    {
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
    CleanupClosePushL(socket);

    // Test to ensure ESmsEncodingPortugueseSingleShift is selected
    // two Portuguese shift (& standard) characters are added for every locking (& shift)
    // character - the result is Portuguese Single Shift
    iCharSets.Append(KCharacterSetPortuguese7bitGSMShiftAndStd1());
    iCharSets.Append(KCharacterSetPortuguese7bitGSMShiftAndStd2());
    // now add the Portuguese Locking Shift characters, note that these characters
    // also occur in the Single Shift table
    iCharSets.Append(KCharacterSetPortuguese7bitGSMShift());
    DoEncodingTestL(
         /* Socket */            socket,
         /* Test */              _L("Portuguese GSM text (Portuguese single shift table)"),
         /* Sequences */         1,
         /* Char sets */         iCharSets,  
         /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
         /* Encoding to use */   ESmsEncodingPortugueseLockingAndSingleShift,
         /* Encoding expected */ ESmsEncodingPortugueseSingleShift);
    iCharSets.Reset(); 
    
    CleanupStack::PopAndDestroy(&socket);
    return TestStepResult();
    }

/**
 * Test normal GSM text but with Turkish locking and shift table...
 */
TVerdict CTestOptimumEncodingWithLockingAndDowngrade7bitNationalLanguages::doTestStepL()
    {
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
    CleanupClosePushL(socket);
    
    // Add characters in the standard table, but request ESmsEncodingTurkishLockingAndSingleShift 
    iCharSets.Append(KCharacterSetStandard7bitGSM());
    iCharSets.Append(KCharacterSetStandard7bitGSMShift());
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("normal GSM text but with Turkish locking and shift table"),
        /* Sequences */         1,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingTurkishLockingAndSingleShift,
        /* Encoding expected */ ESmsEncodingNone);
    iCharSets.Reset();
    
    CleanupStack::PopAndDestroy(&socket);
    return TestStepResult();
    }

/**
 * Test Turkish GSM text with other downgrades (Turkish locking and shift table)...
 */
TVerdict CTestOptimumEncodingWithLockingAndUnconvertible7bitNationalLanguages::doTestStepL()
    {
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, socket, ESmsAddrRecvAny);
    CleanupClosePushL(socket);

    // Add Turkish shift characters
    iCharSets.Append(KCharacterSetTurkish7bitGSMShift());
    // Add Non- Turkish characters that will be downgraded
    iCharSets.Append(KCharacterSetNonTurkishDowngrading7bitGSMChars());
    DoEncodingTestL(
        /* Socket */            socket,
        /* Test */              _L("Turkish GSM text with other downgrades (Turkish locking and shift table)"),
        /* Sequences */         1,
        /* Char sets */         iCharSets,
        /* DCS */               TSmsDataCodingScheme::ESmsAlphabet7Bit,
        /* Encoding to use */   ESmsEncodingTurkishLockingAndSingleShift,
        /* Encoding expected */ ESmsEncodingTurkishSingleShift);
    iCharSets.Reset();

    CleanupStack::PopAndDestroy(&socket);
    return TestStepResult();
    }
