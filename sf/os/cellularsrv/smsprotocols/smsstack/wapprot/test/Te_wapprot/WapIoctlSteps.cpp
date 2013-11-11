// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// implementation of test steps
// 
//

/**
	@file
	@test
*/

#include "WapIoctlSteps.h"

#include <simtsy.h>
#include <smspver.h>
#include <wap_sock.h>

#include "smsstacktestconsts.h"
#include "WapProtSuiteDefs.h"

TVerdict CIoctlStep_1::doTestStepL()
/**
 *  Test step one:
 *  Sends/receives a 7Bit business card using IOCTL
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 1: send/receive 7-Bit business card"));
    // Configure and bind to a socket
	SetupWapSocketL();

	//	Send a business card
	TPtrC testData = GetStringFromConfigL(KTestData1);

	SendWapMessage(testData);

    //  Receive the business card
	ReceiveWapMessage(testData);
	
	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_2::doTestStepL()
/** 
 *	Test step two: 
 *	Sends/receives a 7-Bit calendar entry and VCard using IOCTL(two segments each)
 *	@return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 2: send/receive a VCal and VCard"));
    // Configure and bind to a socket
    SetupWapSocketL();
	
	//	Issue Ioctl for getting the length of the message
	TRequestStatus ioctlStatus;
	TPckgBuf<TInt> length;

	INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the first message (VCal)..."));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    TPtrC testData1 = GetStringFromConfigL(KTestData1);
    TPtrC testData2 = GetStringFromConfigL(KTestData2);
    
    //  Send a VCal entry
    SendWapMessage(testData1);

	//	Send a VCard entry with extended GSM alphabets
    SendWapMessage(testData2);

    //	Get the size of the first incoming message
	User::WaitForRequest(ioctlStatus);
	TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
	TESTCHECK(length(), testData1.Length(), "Check if reported length is correct");
	
    // Receive the VCal message
    ReceiveWapMessageFromSocket(length(), testData1);

    // Receive the VCard message
    ReceiveWapMessage(testData2);
    
	iSocket.Close();
    return TestStepResult();
	}

TVerdict CIoctlStep_3::doTestStepL()
/**
 *  Test step 3:
 *  send/receive one 7-Bit two segments message and one 7-Bit one segment message
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 3: send/receive two 7-Bit messages"));
    // Configure and bind to a socket
    SetupWapSocketL();

	//	Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;
    
    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the first message..."));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    TPtrC testData1 = GetStringFromConfigL(KTestData1);
    TPtrC testData2 = GetStringFromConfigL(KTestData2);
    
    //  Send a business card
    SendWapMessage(testData2);

    //  Send a business card
    SendWapMessage(testData1);

	//	Get the size of the first incoming message
	User::WaitForRequest(ioctlStatus);
	TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
	TESTCHECK(length(), testData2.Length(), "Check if reported length is correct");
	    
	//	Receive the first message
    ReceiveWapMessageFromSocket(length(), testData2);

	//	Receive the second message
    ReceiveWapMessage(testData1);
    
    iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_4::doTestStepL()
/**
 *  Test step 4:
 *  simulating client crash conditions with 7bit message
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 4: client crash condition 7-Bit message "));
    // Configure and bind to a socket
    SetupWapSocketL();

	//	Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;
    
    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the first message..."));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//	Send a business card
    TPtrC testData = GetStringFromConfigL(KTestData1);
    SendWapMessage(testData);

    //  Get the size of the first incoming message
	User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
    TESTCHECK(length(), testData.Length(), "Check if reported length is correct");
	
	//	Receive a message
    TRequestStatus recvStatus;
    TWapAddr recvWapAddr;
    HBufC8* recvBuf = HBufC8::NewLC(length());
    TPtr8 recvPtr = recvBuf->Des(); 
    
	iSocket.RecvFrom(recvPtr, recvWapAddr, 0, recvStatus);
	User::WaitForRequest(recvStatus);
    TESTCHECKL(recvStatus.Int(), KErrNone, "Receiving a message");
	
	// Simulate a crash by closing and reopening the socket
	iSocket.Close();
	INFO_PRINTF1(_L("Socket was closed..."));
	
	OpenSocketL(iSocketServer, iSocket);
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0));
	INFO_PRINTF1(_L("Socket set option for indicating new client"));
	User::LeaveIfError(iSocket.Bind(iWapAddr));
	INFO_PRINTF1(_L("Socket was opened..."));
	
	// Issue another get length of datagram as if not aware of the previous message
	// and retrieve the message from the SAR store
    ReceiveWapMessage(testData);
	
	iSocket.Close();
    CleanupStack::PopAndDestroy(recvBuf);
    return TestStepResult();
	}

TVerdict CIoctlStep_5::doTestStepL()
/**
 *  Test step 5:
 *  simulating client crash conditions with 8bit push message
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 5: client crash condition with 8Bit push message"));
    // Configure and bind to a socket
    SetupWapSocketL();
 
    //  Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;
    
    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the first message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    //  Get the size of the incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");

    //  Receive a message
    TRequestStatus recvStatus;
    TWapAddr recvWapAddr;
    HBufC8* recvBuf = HBufC8::NewLC(length());
    TPtr8 recvPtr = recvBuf->Des(); 
    
    iSocket.RecvFrom(recvPtr, recvWapAddr, 0, recvStatus);
    User::WaitForRequest(recvStatus);
    TESTCHECKL(recvStatus.Int(), KErrNone, "Receiving a message");
	
    // Simulate a crash by closing and reopening the socket
    iSocket.Close();
    INFO_PRINTF1(_L("Socket was closed..."));
    
    OpenSocketL(iSocketServer, iSocket);
    User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0));
    INFO_PRINTF1(_L("Socket set option for indicating new client"));
    User::LeaveIfError(iSocket.Bind(iWapAddr));
    INFO_PRINTF1(_L("Socket was opened..."));
	
	//	Issue another get length of datagram as if not aware of the previous message
	INFO_PRINTF1(_L("Issue of first IOCTL for GetLength"));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
	
	//	waiting for request to be completed
	User::WaitForRequest(ioctlStatus);
	User::LeaveIfError(ioctlStatus.Int());
	INFO_PRINTF1(_L("IOCTL completed..."));
	
	//	New message retrieved from SAR store
    HBufC8* recvBuf2 = HBufC8::NewLC(length());
    TPtr8 recvPtr2 = recvBuf2->Des(); 
	   
	iSocket.RecvFrom(recvPtr2,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	User::LeaveIfError(recvStatus.Int());
	INFO_PRINTF1(_L("Received the message..."));

	//	Confirm the receipt of the message to the client
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));

	//	Compare the received message with the previous one
	//	They should be identical
	TEST(recvPtr2.Compare(recvPtr) == 0);

	iSocket.Close();
    CleanupStack::PopAndDestroy(2, recvBuf); // recvBuf, recvBuf2
	return TestStepResult();
	}

TVerdict CIoctlStep_6::doTestStepL()
/**
 *  Test step 6:
 *  Sends/receives a business card (Old client testing)
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 6: send/receive a VCard (Old client testing)"));
	// Configure and bind to a socket, do not set the "new style client" option 
	SetupWapSocketL(iSocket, iWapAddr, KWapPort, EFalse);

	//	Send a calendar entry
    TPtrC testData = GetStringFromConfigL(KTestData1);
    SendWapMessage(testData);

	//	Receiving a datagram
	TRequestStatus recvStatus;
	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf;
	
    INFO_PRINTF1(_L("Receiving a message from the socket..."));
	iSocket.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
    TESTCHECKL(recvStatus.Int(), KErrNone, "Receiving a message");
	
	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_7::doTestStepL()
/**
 *  Test step 7:
 *  Receives a Push Message (8-Bit)
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 7: Receive push message(8-Bit)"));
    // Configure and bind to a socket
    SetupWapSocketL();

	//
	//	Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;

	INFO_PRINTF1(_L("Issue of first IOCTL for GetLength"));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
	
	//
	//	Get the size of the incomming push message
	//
	User::WaitForRequest(ioctlStatus);
	User::LeaveIfError(ioctlStatus.Int());
	INFO_PRINTF1(_L("IOCTL completed..."));

	//
	//	Receiving push datagram
	//
	TRequestStatus recvStatus;
	TWapAddr recvWapAddr;
    TBuf8<256> recvBuf2(length);
    
    iSocket.RecvFrom(recvBuf2,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	User::LeaveIfError(recvStatus.Int());
	INFO_PRINTF1(_L("Received the push message..."));

	//
	//	Confirm the receipt of the message to the client
	//
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_8::doTestStepL()
/**
 *  Test step 8:
 *  Receives two Push Messages (8-Bit) Tests store of 8-Bit messages
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 8: Receive two push message(8-Bit)"));
    // Configure and bind to a socket
    SetupWapSocketL();

	//
	//	Issue Ioctl for getting the length of the message
	//
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;
    
	INFO_PRINTF1(_L("Issue of first IOCTL for GetLength"));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
	//
	//	Get the size of the incomming push message
	//
	User::WaitForRequest(ioctlStatus);
	User::LeaveIfError(ioctlStatus.Int());
	INFO_PRINTF1(_L("IOCTL completed..."));

	//
	//	Receiving push datagram
	//
    TRequestStatus recvStatus;
	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf1(length);

	iSocket.RecvFrom(recvBuf1,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	User::LeaveIfError(recvStatus.Int());
	INFO_PRINTF1(_L("Received the push message..."));

	//
	//	Confirm the receipt of the message to the client
	//
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));

	//
	//	Issue another Ioctl for getting the length of the message
	//
	INFO_PRINTF1(_L("Issue of first IOCTL for GetLength"));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
	//
	//	Get the size of the second incomming push message
	//
	User::WaitForRequest(ioctlStatus);
	User::LeaveIfError(ioctlStatus.Int());
	INFO_PRINTF1(_L("IOCTL completed..."));
	//
	//	Allocate buffer
	//
	TBuf8<256> recvBuf2(length);

	//
	//	Receiving push datagram
	//
	iSocket.RecvFrom(recvBuf2,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	User::LeaveIfError(recvStatus.Int());
	INFO_PRINTF1(_L("Received the second push message..."));
	//
	//	Compare the received message with the previous one
	//	They should be identical
	//
	TEST(recvBuf2.Compare(recvBuf1) == 0);

	//
	//	Confirm the receipt of the message to the client
	//
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_9::doTestStepL()
/**
 *  Test step 9:
 *  Receive a 8-Bit push message (Old client testing)
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 9: receive a push message (Testing old client)"));
	// Configure and bind to a socket, do not set the "new style client" option 
    SetupWapSocketL(iSocket, iWapAddr, KWapPort, EFalse);

	//
	//	Receiving a datagram
	//
	TRequestStatus recvStatus;
	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf;
	
	iSocket.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	User::LeaveIfError(recvStatus.Int());
	INFO_PRINTF1(_L("Received the message..."));

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_10::doTestStepL()
/**
 *  Test step Ten:
 *  Sends/receives a 7Bit business card using IOCTL
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 10: send/receive 7-Bit business card using IOCTL"));
    // Configure and bind to a socket
    SetupWapSocketL();
  
	//	Send a calendar entry
    TPtrC testData = GetStringFromConfigL(KTestData1);
    SendWapMessage(testData);

	ReceiveWapMessage(testData);

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_11::doTestStepL()
/**
 *  Test step 11:
 *  Test Cancel IOCTL
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 11: Testing CancelIOCTL"));
    // Configure and bind to a socket
    SetupWapSocketL();

	//	Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;

	INFO_PRINTF1(_L("Issue of IOCTL for GetLength"));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//	Issue Cancel Ioctl
	INFO_PRINTF1(_L("Issue of cancel IOCTL for GetLength"));
	iSocket.CancelIoctl();
	TESTCHECK(ioctlStatus.Int(), KErrCancel, "Check if the outstanding IOCTL has been canceled");

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_12::doTestStepL()
/**
 *  Test step 12:
 *  Test issue IOCTL without SetOption NewStyleClient
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 12: Test issue IOCTL without SetOption NewStyleClient"));
    // Configure and bind to a socket, do not set the "new style client" option 
    SetupWapSocketL(iSocket, iWapAddr, KWapPort, EFalse);
    
	//	Issue Ioctl for getting the length of the message (should fail)
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;

    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
    TESTCHECKCONDITION(ioctlStatus.Int()!=KErrNone, "Getting the length of the incoming message");

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_13::doTestStepL()
/**
 *  Test step two:
 *  Sends/receives a 8-Bit calendar entry using IOCTL(two segments)
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 13: send/receive a 8-Bit VCal"));
    // Configure and bind to a socket
    SetupWapSocketL();

    User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS));

	//	Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;
    
    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//	Send a calendar entry
    TPtrC testData = GetStringFromConfigL(KTestData1);
    SendWapMessage(testData);
    
    //  Get the size of the incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the incoming message");
    TESTCHECK(length(), testData.Length(), "Check if reported length is correct");
    
    ReceiveWapMessageFromSocket(length(), testData);
    
	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_14::doTestStepL()
/**
 *  Test step 14:
 *  simulating client crash conditions with 8bit calendar entry
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 14: client crash condition 8-Bit calendar entry "));
    // Configure and bind to a socket
    SetupWapSocketL();
    
    User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS));

	//
	//	Issue Ioctl for getting the length of the message
	//
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;
    
    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//	Send a Calendar entry
    TPtrC testData = GetStringFromConfigL(KTestData1);
    SendWapMessage(testData);

    //  Get the size of the incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the incoming message");
    TESTCHECK(length(), testData.Length(), "Check if reported length is correct");
    
    //  Receive a message
    TRequestStatus recvStatus;
    TWapAddr recvWapAddr;
    HBufC8* recvBuf = HBufC8::NewLC(length());
    TPtr8 recvPtr = recvBuf->Des(); 
    
    iSocket.RecvFrom(recvPtr, recvWapAddr, 0, recvStatus);
    User::WaitForRequest(recvStatus);
    TESTCHECKL(recvStatus.Int(), KErrNone, "Receiving a message");
    
    // Simulate a crash by closing and reopening the socket
    iSocket.Close();
    INFO_PRINTF1(_L("Socket was closed..."));
    
    OpenSocketL(iSocketServer, iSocket);
    User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0));
    INFO_PRINTF1(_L("Socket set option for indicating new client"));
    User::LeaveIfError(iSocket.Bind(iWapAddr));
    INFO_PRINTF1(_L("Socket was opened..."));
    
    // Issue another get length of datagram as if not aware of the previous message
    // and retrieve the message from the SAR store
    ReceiveWapMessage(testData);
    
    iSocket.Close();
    CleanupStack::PopAndDestroy(recvBuf);
    return TestStepResult();
	}

TVerdict CIoctlStep_15::doTestStepL()
/**
 *  Test step fifteen:
 *  Receives a 7Bit v-card using IOCTL with concatenation for new style client
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 15: send/receive 7-Bit v-card"));
    // Configure and bind to a socket
    SetupWapSocketL();
    
    //
	//	Issue Ioctl for getting the length of the message
	//
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;

    INFO_PRINTF1(_L("Issue of IOCTL for GetLength"));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//
	//	Get the size of the first incoming message
	//
	User::WaitForRequest(ioctlStatus);
	User::LeaveIfError(ioctlStatus.Int());
	INFO_PRINTF1(_L("IOCTL completed"));

	//
	//	Receiving a datagram that is 7bit with a concatenated length of > 160 characters
	//
    TRequestStatus recvStatus;
    TWapAddr recvWapAddr;
    TBuf8<360> recvBuf(length);

    iSocket.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	User::LeaveIfError(recvStatus.Int());
	INFO_PRINTF1(_L("Received the message..."));

	//
	//	Confirm the receipt of the message to the client
	//
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));

	iSocket.Close();
	return TestStepResult();
	}
	
TVerdict CIoctlStep_17::doTestStepL()
/**
 *  Test step seventeen:
 *  Sends a 7_Bit business card. Using IOCTL query SMS transport parameters
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 17: send 7-Bit v-card and query SMS transport parameters"));
    // Configure and bind to a socket
    SetupWapSocketL();

	//	Send a business card
    TPtrC testData = GetStringFromConfigL(KTestData1);
    SendWapMessage(testData);

	// Create a descriptor with correct size for obtaining the message parameter
	HBufC8* parameterStore = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr = parameterStore->Des();
	
	// Get the message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr);
	
	// Internalise the message parameter and check
    TPtrC8 scAddr = iWapAddr.WapAddress(); 
        
	InternaliseSmsDataAndCheckL(parameterStorePtr, scAddr);
	
	iSocket.Close();
	CleanupStack::PopAndDestroy(parameterStore);
	return TestStepResult();
	}
	
TVerdict CIoctlStep_18::doTestStepL()
/**
 *  Test step eighteen:
 *  Sends a 7-Bit calendar entry (two segments). Using IOCTL query SMS transport parameters.
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 18: Sends a 7-Bit calendar entry (two segments). Using IOCTL query SMS transport parameters."));
    // Configure and bind to a socket
    SetupWapSocketL();

	//	Send a calendar entry
	TPtrC testData = GetStringFromConfigL(KTestData1);
	SendWapMessage(testData);
	
	// Create a descriptor with correct size for obtaining the message parameter
	HBufC8* parameterStore = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr = parameterStore->Des();
	
	// Get the message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr);
	
	// Internalise the message parameter and check
    TPtrC8 scAddr = iWapAddr.WapAddress(); 
    
	InternaliseSmsDataAndCheckL(parameterStorePtr, scAddr);
	
	iSocket.Close();
	CleanupStack::PopAndDestroy(parameterStore);
	return TestStepResult();
	}
	
TVerdict CIoctlStep_19::doTestStepL()
/**
 *  Test step 19:
 *  send/receive one 7-Bit two segments message and one 7-Bit one segment message
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 19: send/receive two 7-Bit messages"));
    // Configure and bind to a socket
    SetupWapSocketL();
    
	//
	//	Issue Ioctl for getting the length of the message
	//
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;

    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the first message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    TPtrC testData1 = GetStringFromConfigL(KTestData1);
    TPtrC testData2 = GetStringFromConfigL(KTestData2);

    //	Send a calendar entry
	SendWapMessage(testData2);
    
	//	Send a business card
	SendWapMessage(testData1);
	
    //  Get the size of the first incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
	
	// Get the first message parameter value
	HBufC8* parameterStore = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr = parameterStore->Des();
	    
    INFO_PRINTF1(_L("Getting parameter value of first message..."));
    GetMessageParameterL(iSocket, parameterStorePtr);
	
	// Internalise the first message parameter and check
    TPtrC8 scAddr = iWapAddr.WapAddress(); 
    
	InternaliseSmsDataAndCheckL(parameterStorePtr, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore);
	
	//	Receiving a datagram
    TRequestStatus recvStatus;
    TWapAddr recvWapAddr;
    TBuf8<256> recvBuf2(length);
    
    iSocket.RecvFrom(recvBuf2,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Received the first message..."));

	//	Confirm the receipt of the message to the client
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	TESTL(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0)==KErrNone);

	INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the second message..."));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//	Get the size of the SECOND incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
    TESTCHECK(length(), testData1.Length(), "Check if reported length is correct");

	// Create a descriptor with correct size for obtaining the second message parameter
	HBufC8* parameterStore2 = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr2 = parameterStore2->Des();
	
	// Get the second message parameter value
    INFO_PRINTF1(_L("Getting parameter value of second message..."));
	GetMessageParameterL(iSocket, parameterStorePtr2);
	
	// Internalise the second message parameter and check
	InternaliseSmsDataAndCheckL(parameterStorePtr2, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore2);
	
	//	Receiving the second message
	ReceiveWapMessageFromSocket(length(), testData1);
	
	iSocket.Close();
	return TestStepResult();
	}
	
TVerdict CIoctlStep_20::doTestStepL()
/**
 *  Test step 20:
 *  simulating client crash conditions with 7bit message
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 20: client crash condition 7-Bit message "));
    // Configure and bind to a socket
    SetupWapSocketL();

	//	Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;

    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//	Send a business card
    TPtrC testData = GetStringFromConfigL(KTestData1);
    SendWapMessage(testData);
	
    //  Get the size of the incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
    TESTCHECK(length(), testData.Length(), "Check if reported length is correct");
    
	// Create a descriptor with correct size for obtaining the message parameter
	HBufC8* parameterStore = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr = parameterStore->Des();
	
	// Get the message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr);
	
	// Internalise the message parameter and check
	TPtrC8 scAddr = iWapAddr.WapAddress(); 

	InternaliseSmsDataAndCheckL(parameterStorePtr, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore);

    //  Receive a message
    TRequestStatus recvStatus;
    TWapAddr recvWapAddr;
    HBufC8* recvBuf = HBufC8::NewLC(length());
    TPtr8 recvPtr = recvBuf->Des(); 
    
    iSocket.RecvFrom(recvPtr, recvWapAddr, 0, recvStatus);
    User::WaitForRequest(recvStatus);
    TESTCHECKL(recvStatus.Int(), KErrNone, "Receiving a message");
    
    // Simulate a crash by closing and reopening the socket
    iSocket.Close();
    INFO_PRINTF1(_L("Socket was closed..."));
    
    OpenSocketL(iSocketServer, iSocket);
    User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0));
    INFO_PRINTF1(_L("Socket set option for indicating new client"));
    User::LeaveIfError(iSocket.Bind(iWapAddr));
    INFO_PRINTF1(_L("Socket was opened..."));
		
	//	Issue another get length of datagram as if not aware of the previous message
    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
	
    //  Get the size of the incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
    TESTCHECK(length(), testData.Length(), "Check if reported length is correct");
	
	// Recreate a descriptor with correct size for obtaining the message parameter
	HBufC8* parameterStore2 = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr2 = parameterStore2->Des();
	
	// Get the message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr2);
	
	// Internalise the message parameter and check
	InternaliseSmsDataAndCheckL(parameterStorePtr2, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore2);

	//	New message retrieved from SAR store
	ReceiveWapMessageFromSocket(length(), testData);

	iSocket.Close();
    CleanupStack::PopAndDestroy(recvBuf);
	return TestStepResult();
	}
	
TVerdict CIoctlStep_21::doTestStepL()
/**
 *  Test step 21:
 *  simulating client crash conditions with 8bit push message
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 21: client crash condition with 8Bit push message"));
    // Configure and bind to a socket
    SetupWapSocketL();

    //  Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;

    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    //  Get the size of the incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");

	// Create a descriptor with correct size for obtaining the message parameter
	HBufC8* parameterStore = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr = parameterStore->Des();
	
	// Get the message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr);
	
	// Internalise the message parameter and check
    TPtrC8 scAddr = iWapAddr.WapAddress(); 
	
	InternaliseSmsDataAndCheckL(parameterStorePtr, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore);
	
	//	Receiving a datagram
    TRequestStatus recvStatus;
    TWapAddr recvWapAddr;
    TBuf8<256> recvBuf(length);
    
    iSocket.RecvFrom(recvBuf, recvWapAddr, 0, recvStatus);
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Received the message..."));
	
	// Simulate a crash by closing and reopening the socket
	iSocket.Close();
	INFO_PRINTF1(_L("Socket was closed..."));

	OpenSocketL(iSocketServer, iSocket);
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0));
	INFO_PRINTF1(_L("Socket set option for indicating new client"));
	User::LeaveIfError(iSocket.Bind(iWapAddr));
	INFO_PRINTF1(_L("Socket was opened..."));
	
	//	Issue another get length of datagram as if not aware of the previous message
    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    //  Get the size of the incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
	
	// Recreate a descriptor with correct size for obtaining the message parameter
    HBufC8* parameterStore2 = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr2 = parameterStore2->Des();
	
	// Get the message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr2);
	
	// Internalise the message parameter and check
	InternaliseSmsDataAndCheckL(parameterStorePtr2, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore2);
	
	//	New message retrieved from SAR store
	TBuf8<256> newRecvBuf(length);
    iSocket.RecvFrom(newRecvBuf,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Received the message..."));
	
	//	Confirm the receipt of the message to the client
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	TESTL(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0)==KErrNone);
	
	//	Compare the received message with the previous one
	//	They should be identical
	TESTL(newRecvBuf.Compare(recvBuf)==0);

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_22::doTestStepL()
/**
 *  Test step 22:
 *  Push Message (8-Bit). Using IOCTL query SMS transport parameters
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 22: Push Message (8-Bit) and query SMS transport parameters"));
    // Configure and bind to a socket
    SetupWapSocketL();
    
	// Create a descriptor with correct size for obtaining the message parameter
	HBufC8* parameterStore = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr = parameterStore->Des();
	
	// Get the message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr);
	
	// Internalise the message parameter and check
    TPtrC8 scAddr = iWapAddr.WapAddress(); 
    
	InternaliseSmsDataAndCheckL(parameterStorePtr, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore);
	
	iSocket.Close();
	return TestStepResult();
	}
	
TVerdict CIoctlStep_23::doTestStepL()
/**
 *  Test step 23:
 *  Two Push Messages (8-Bit). Using IOCTL query SMS transport parameters
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 23: Push two push message(8-Bit) query transport parameters"));
    // Configure and bind to a socket
    SetupWapSocketL();
    
    //  Issue Ioctl for getting the length of the message
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;

    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    //  Get the size of the incoming message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
    
	INFO_PRINTF1(_L("Getting parameter value of first message..."));
	
	// Create a descriptor with correct size for obtaining the first message parameter
	HBufC8* parameterStore = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr = parameterStore->Des();
	
	// Get the first message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr);
	
	// Internalise the first message parameter and check
	TPtrC8 scAddr = iWapAddr.WapAddress(); 
    
	InternaliseSmsDataAndCheckL(parameterStorePtr, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore);

	//	Receiving push datagram
	TRequestStatus recvStatus;
    TWapAddr recvWapAddr;
    TBuf8<256> recvBuf1(length);
    
    iSocket.RecvFrom(recvBuf1,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Received the push message..."));

	//	Confirm the receipt of the message to the client
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	TESTL(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0)==KErrNone);

    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    //  Get the size of the second incoming push message
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the size of the first incoming message");
	
	INFO_PRINTF1(_L("Getting parameter value of second message..."));

	// Create a descriptor with correct size for obtaining the second message parameter
	HBufC8* parameterStore2 = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr2 = parameterStore2->Des();
	
	// Get the second message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr2);
	
	// Internalise the second message parameter and check
	InternaliseSmsDataAndCheckL(parameterStorePtr2, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore2);
	
	//	Receiving push datagram
	TBuf8<256> recvBuf2(length);

	iSocket.RecvFrom(recvBuf2,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Received the second push message..."));

	//	Compare the received message with the previous one
	//	They should be identical
	TESTL(recvBuf2.Compare(recvBuf1)==0);
		
	//	Confirm the receipt of the message to the client
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	TESTL(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0)==KErrNone);

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_24::doTestStepL()
/**
 *  Test step 24:
 *  Test Cancel IOCTL
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 24: Testing Cancel IOCTL"));
    // Configure and bind to a socket
    SetupWapSocketL();

	TRequestStatus ioctlStatus;
	TBuf8<200> smsdata;
	TPckgBuf<TInt> paramLength;
	
	//	Issue Ioctl for getting the length of the message parameter
	INFO_PRINTF1(_L("Issue of IOCTL for KSOGetMessageParametersLength"));
	iSocket.Ioctl(KSOGetMessageParametersLength, ioctlStatus, &smsdata, KSolWapProv);
	
	//	Issue Cancel Ioctl
	INFO_PRINTF1(_L("Issue of cancel IOCTL for KSOGetMessageParametersLength"));
	iSocket.CancelIoctl();
	TESTCHECKL(ioctlStatus.Int(), KErrCancel, "Checking if the outstanding IOCTL has been cancelled");

	//	Issue Ioctl for getting the message parameter
	INFO_PRINTF1(_L("Issue of IOCTL for KSOGetMessageParameters"));
	iSocket.Ioctl(KSOGetMessageParameters, ioctlStatus, &smsdata, KSolWapProv);

	//	Issue Cancel Ioctl
	INFO_PRINTF1(_L("Issue of cancel IOCTL for KSOGetMessageParameters"));
	iSocket.CancelIoctl();
    TESTCHECKL(ioctlStatus.Int(), KErrCancel, "Checking if the outstanding IOCTL has been cancelled");

	iSocket.Close();
	return TestStepResult();
	}

TVerdict CIoctlStep_25::doTestStepL()
/**
 *  Test step 25:
 *  Test issue IOCTL without SetOption NewStyleClient
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 25: Test issue IOCTL without SetOption NewStyleClient"));
    // Configure and bind to a socket
    SetupWapSocketL(iSocket, iWapAddr, KWapPort, EFalse);

	//	Issue Ioctl for getting the length of the message parameter (should fail)
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> paramLength;

	INFO_PRINTF1(_L("Issue of IOCTL for KSOGetMessageParametersLength"));
	iSocket.Ioctl(KSOGetMessageParametersLength, ioctlStatus, &paramLength, KSolWapProv);
	TESTL(ioctlStatus.Int()!=KErrNone);
				
	//	Issue Ioctl for getting the message parameter (should fail)
	INFO_PRINTF1(_L("Issue of IOCTL for KSOGetMessageParameters"));
	TBuf8<200> smsdata;
	   
	iSocket.Ioctl(KSOGetMessageParameters, ioctlStatus, &smsdata, KSolWapProv);
	User::WaitForRequest(ioctlStatus);
	TESTL(ioctlStatus.Int()!=KErrNone);

	iSocket.Close();
	return TestStepResult();
	}
	
TVerdict CIoctlStep_26::doTestStepL()
/**
 *  Test step 26:
 *  Sends a 8-Bit calendar entry using IOCTL(two segments)
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IOCTL Test step 26: send a 8-Bit VCal (two segments)"));
    // Configure and bind to a socket
    SetupWapSocketL();

	TESTL(iSocket.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS)==KErrNone);

	//	Send a calendar entry
    TPtrC testData = GetStringFromConfigL(KTestData1);
    SendWapMessage(testData);

	// Create a descriptor with correct size for obtaining the message parameter
	HBufC8* parameterStore = HBufC8::NewLC(GetMessageParameterLengthL(iSocket));
	TPtr8 parameterStorePtr = parameterStore->Des();
	
	// Get the message parameter value
	GetMessageParameterL(iSocket, parameterStorePtr);
	
	// Internalise the message parameter and check
    TPtrC8 scAddr = iWapAddr.WapAddress(); 
    
	InternaliseSmsDataAndCheckL(parameterStorePtr, scAddr);
	
	CleanupStack::PopAndDestroy(parameterStore);		
	
	iSocket.Close();
	return TestStepResult();
	}

struct SOneOpTestThreadInfo
	{
	CIoctlStep_27* iTest;
	TPtrC* iTestData1;
	TPtrC16* iTelNumber;
	TInt iPort;
	};

TInt CIoctlStep_27::DoPanicTestL(TPtrC* aTestData1, TPtrC16* aTelNumber, TInt aPort)
	{
    TInt testNumber = 1;
    TInt port = aPort;
    TPtrC TestData1 = *aTestData1;
    TPtrC16 TelNumber = *aTelNumber;

    User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn));
    User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, testNumber));

	//
    //	Setting the port number and service center number of the wap address
	//	The service center number should be the same as the sim phone number used
	//  for test (not required for SIM tsy)
	//
	TWapAddr wapAddr;
	TWapAddr recvWapAddr;
	wapAddr.SetWapPort(TWapPortNumber(port));

	TBuf8<100> scNumber;
	scNumber.Copy(TelNumber);
	TPtrC8 scAddr(scNumber);
	wapAddr.SetWapAddress(scAddr);

	//
	//	Connect to socket server
	//
	RSocketServ socketServer;
    User::LeaveIfError(socketServer.Connect());
	CleanupClosePushL(socketServer);

	//
	//	Define and open the socket
	//
	RSocket sock;
	OpenSocketLC(socketServer, sock);
	//
	//	Indicating to the protocol that it's a new client
	//
    INFO_PRINTF1(_L("Socket set option for indicating new client"));
    TInt ret = sock.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0);
    TESTCHECKL(ret, KErrNone, "Indicating to the protocol that it's a new client")

    ret = sock.Bind(wapAddr);
    TESTCHECKL(ret, KErrNone, "Socket bind");

	//
	//	Send a business card
	//
	TRequestStatus  status;

	TBuf8<200> data;
	data.Copy(TestData1);
	sock.SendTo(data, wapAddr, 0, status);
	User::WaitForRequest(status);
    TESTCHECKL(status.Int(), KErrNone, "Sending a message");

	TRequestStatus ioctlStatus;
	TRequestStatus recvStatus;
	TPckgBuf<TInt> length;
	
	TRequestStatus ioctlmessageStatus;
	TBuf8<200> smsdata;

	//
	//	Issue Ioctl for getting the length of the message
	//
	sock.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//
	//	Issue Ioctl for getting the message parameters (should panic!)
	//
	sock.Ioctl(KSOGetMessageParameters, ioctlmessageStatus, &smsdata, KSolWapProv);
	
	//
	//	Get the size of the first incoming message
	//
	User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the length of the incoming message");

	//
	//	Receiving a datagram
	//
	TBuf8<256> recvBuf(length);
	sock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
	User::WaitForRequest(recvStatus);
    TESTCHECKL(recvStatus.Int(), KErrNone, "Receiving a message");
    TESTCHECK(recvBuf.Compare(data), 0, "Checking the received message matched expected");

	
	//
	//	Confirm the receipt of the message to the client
	//
    INFO_PRINTF1(_L("Setting Socket option for indicating receipt of the message..."));
    ret = iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0);
    TESTCHECKL(ret, KErrNone, "Setting Socket option for indicating receipt of the message");
	
	//
	//	Closing the socket
	//
	CleanupStack::PopAndDestroy(&sock);
    CleanupStack::PopAndDestroy(&socketServer);

	return KErrNone;
	}

TInt testPanicsL(TAny* aPtr)
	{
	SOneOpTestThreadInfo *pI=(SOneOpTestThreadInfo *)aPtr;
	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(err, pI->iTest->DoPanicTestL(pI->iTestData1, pI->iTelNumber, pI->iPort));
	
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}

TVerdict CIoctlStep_27::doTestStepL()
/**
 *  Test step 27:
 *  Send a 7_Bit business card. Issue both a get length and get message parameters to IOCTL  at sametime
 *  @return - TVerdict code
 */
	{
	const TUint KHeapMinSize=0x01000;
	const TUint KHeapMaxSize=0x10000;

	TPtrC TestData1;
	TPtrC16 TelNumber;
	TInt port=226;
	
	if(!GetStringFromConfig(ConfigSection(),KTestData1, TestData1) ||
	   !GetStringFromConfig(ConfigSection(),KSCNumber,TelNumber) ||
	   !GetIntFromConfig(ConfigSection(),KWapPort,port))
		{
		User::Leave(KErrNotFound);
		}
	
	INFO_PRINTF1(_L("Send a 7_Bit business card. Issue both a get length and get message parameters to IOCTL"));

	TBool jitEnabled = User::JustInTime();
	
	User::SetJustInTime(EFalse);
	
	SOneOpTestThreadInfo info;

	info.iTest=this;
	info.iTestData1=&TestData1;
	info.iTelNumber=&TelNumber;
	info.iPort=226;
	
	RThread thread;
	TInt rc = thread.Create(_L("PanicThread"),testPanicsL,KDefaultStackSize,KHeapMinSize,KHeapMaxSize,&info);
	
	TESTCHECKL(rc, KErrNone,"Creating a thread" );
		
	TRequestStatus s;
    thread.Logon(s);
	thread.Resume();
	User::WaitForRequest(s);
	TESTCHECK(thread.ExitType(), EExitPanic, "Checking the correct exit type");
			
	INFO_PRINTF2(_L("Exit type is: %d"), thread.ExitType());
	
	thread.Close();
	
	User::SetJustInTime(jitEnabled);
	
	return TestStepResult();
	}

TVerdict CIoctlStep_28::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	
    SetTestNumberFromConfigurationFileL();
	
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	INFO_PRINTF1(_L("Deleting segmentation and reassembly stores..."));

	RFs fileServer;
	User::LeaveIfError(fileServer.Connect());

	// delete segmentation and reassembly store files before the test
	_LIT(KReassemblyStoreName,"C:\\Private\\101F7989\\sms\\smsreast.dat");
	_LIT(KSegmentationStoreName,"C:\\Private\\101F7989\\sms\\smssegst.dat");
	_LIT(KWapReassemblyStoreName,"C:\\Private\\101F7989\\sms\\wapreast.dat");

	fileServer.Delete(KWapReassemblyStoreName);
	fileServer.Delete(KReassemblyStoreName);
	fileServer.Delete(KSegmentationStoreName);

	fileServer.Close();
	
    TInt  ret = iSocketServer.Connect(KSocketMessageSlots);
    if (ret != KErrNone)
        {
        ERR_PRINTF2(_L("Connecting to socket server failed [ret=%d]"), ret);
        }

	return TestStepResult();
	}
	
TVerdict CIoctlStep_28::doTestStepPostambleL()
	{
    iSocketServer.Close();
    
	delete iScheduler;
	iScheduler = NULL;

	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
	
TVerdict CIoctlStep_28::doTestStepL()
/**
 *  Test step seventeen:
 *  Sends a 7_Bit business card. Issue a get message parameters to IOCTL without correct platform security
 *  @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("Sends a 7_Bit business card. Issue a get message parameters to IOCTL without correct platform security"));
    // Configure and bind to a socket
    SetupWapSocketL();

	//
	//	Send a business card
	//
	TRequestStatus status;
    TPtrC testData = GetStringFromConfigL(KTestData1);
    
	TBuf8<200> data;
	data.Copy(testData);
	iSocket.SendTo(data, iWapAddr, 0, status);
	User::WaitForRequest(status);
	TESTL(status.Int()==KErrNone);
	INFO_PRINTF1(_L("Message sent..."));
	
	//
	//	Issue Ioctl for getting the message parameters
	//
	TRequestStatus testStatus;
	TBuf8<200> smsdata;
	
	INFO_PRINTF1(_L("Issue of IOCTL for KSOGetMessageParameters"));
	iSocket.Ioctl(KSOGetMessageParameters, testStatus, &smsdata, KSolWapProv);
	
	User::WaitForRequest(testStatus);
	TESTL(testStatus.Int()==KErrPermissionDenied);
	INFO_PRINTF2(_L("KSOGetMessageParameters on Ioctl failed to complete with: %d"), testStatus.Int());
			
	iSocket.Close();
	return TestStepResult();
	}

CSetDiskMonitorLimits::CSetDiskMonitorLimits()
    {
    iPartOfMultiStepTestCase = ETrue;
    }

TVerdict CSetDiskMonitorLimits::doTestStepL()
/**
	Creates smsu.rsc file which defines the upper and lower limits for the disk space monitor
*/
    {
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else
    TInt err = RProperty::Define(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey, RProperty::EInt);
    if ((err != KErrNone) && (err != KErrAlreadyExists))    
        {
        ERR_PRINTF2(_L("RProperty::Define() failure [err=%d]"), err);
        User::Leave(err);
        }
		
    TInt highDrop = 3;
    TInt lowDrop = 8;
    TInt freeDrop = 0;
  
    GetIntFromConfig(_L("DiskMonitorLimits"), _L("highDrop"), highDrop);
    GetIntFromConfig(_L("DiskMonitorLimits"), _L("lowDrop"), lowDrop);
  
    SetHighLowLimitsAndDiskSpaceLevelL(highDrop, lowDrop, freeDrop); 
#endif    
	return TestStepResult();
    }

CSetDiskSpace::CSetDiskSpace()
    {
    iPartOfMultiStepTestCase = ETrue;
    }

/**
 *  Set free disk space to the required limit
 */
TVerdict CSetDiskSpace::doTestStepL()
    {
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else    
    ReleaseDiskSpaceL();
     
    TInt  freeDrop = 0;
    GetIntFromConfig(ConfigSection(), _L("freeDrop"), freeDrop);         
 
    SetFreeDiskSpaceFromDropLevelL(freeDrop);
#endif        
	return TestStepResult();
    }
    
/**
	Free up disk space by deleting the temp files created
*/
TVerdict CFreeDiskSpace::doTestStepL()
    {
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else   
    ReleaseDiskSpaceL();
      
    TInt err = RProperty::Delete(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey);
    if (err != KErrNone && err != KErrNotFound)
        {
        ERR_PRINTF2(_L("RProperty::Delete() failure [err=%d]"), err);
        }           
#endif
    return TestStepResult();
    }    

CReceiveWapMessage::CReceiveWapMessage()
    {
    iPartOfMultiStepTestCase = ETrue;
    }

TVerdict CReceiveWapMessage::doTestStepL()
    {
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else
    TInt  ret = iSocketServer.Connect(KSocketMessageSlots);
     if (ret != KErrNone)
         {
         ERR_PRINTF2(_L("Connecting to socket server failed [ret=%d]"), ret);
         }
     
    SetupWapSocketL();

	TRequestStatus recvStatus;
	TRequestStatus ioctlStatus;
	TPckgBuf<TInt> length;	  	

	//
	//	Send a business card
	//
	TRequestStatus status;
	TPtrC testData;
	GetStringFromConfig(ConfigSection(),KTestData1, testData);
	TBuf8<200> data;
	data.Copy(testData);
	
	iSocket.SendTo(data, iWapAddr, 0, status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone, "Sending Message...");

	TBool messageIsExpected;
	GetBoolFromConfig(ConfigSection(), _L("messageIsExpected"), messageIsExpected);

	//Setup delay
	TRequestStatus timerStatus;
	RTimer timer;
	//Read from the INI file.  
	TInt timeout = 10000000; // default 10 seconds timeout
	GetIntFromConfig(_L("ReceiveTimeout"), _L("timeout"), timeout);
	
	timer.CreateLocal();
	timer.After(timerStatus, TTimeIntervalMicroSeconds32(timeout));

	//	Get the size of the first incomming message
	INFO_PRINTF1(_L("Issue of the IOCTL for GetLength"));
	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//Check if there is in fact a message
	TBool received=EFalse;
	User::WaitForRequest(timerStatus, ioctlStatus);
	
	if(ioctlStatus == KRequestPending)
		{
		// if timer elapsed but ESock request is still pending
		iSocket.CancelIoctl();
		User::WaitForRequest(ioctlStatus);
		}
	else
		{
		// ESock request is done. Cancel timer
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		// check ESock error status
		if(ioctlStatus.Int() == KErrNone)
			{
            INFO_PRINTF1(_L("IOCTL for GetLength completed successfully"));
			received=ETrue;		     
			}
		}
	timer.Close();

	if(received && messageIsExpected)
		{
        INFO_PRINTF1(_L("Receiving push datagram..."));
		TWapAddr recvWapAddr;			
		TBuf8<256> recvBuf1(length);
		iSocket.RecvFrom(recvBuf1,recvWapAddr,0,recvStatus);
		User::WaitForRequest(recvStatus);
		User::LeaveIfError(recvStatus.Int());
		TEST(recvBuf1.Compare(data) == 0);

		//
		//	Confirm the receipt of the message to the client
		//
		TInt ret = iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0);
		TESTCHECKL(ret, KErrNone, "Setting Socket option for indicating receipt of message" )
		}
	else if (received && !messageIsExpected)
		{
        ERR_PRINTF1(_L("Message is received but not expected"));
		SetTestStepResult(EFail);
		}
	else if (!received && messageIsExpected)
	    {
        ERR_PRINTF1(_L("Message is expected but not received"));
        SetTestStepResult(EFail);
	    }

	iSocket.Close();
	iSocketServer.Close();
#endif	
	return TestStepResult();
   } 

/**
 *  Tests enumerating of a VCard which needs to go via the observer and not returned to the client.
 *
 *  @return A TVerdict code.
 */
TVerdict CTestEnumeratingVCard::doTestStepL()
	{
    // Configure and bind to a socket
    SetupWapSocketL();

	// Create a socket for the enumeration...
	RSocket   enumerateSock;
	TSmsAddr  smsAddr;

	OpenSocketLC(iSocketServer, enumerateSock, KSMSAddrFamily,KSMSDatagramProtocol);

	smsAddr.SetSmsAddrFamily(ESmsAddrLocalOperation);
	User::LeaveIfError(enumerateSock.Bind(smsAddr));

	// Trigger the enumeration...
	TPckgBuf<TUint>  messageCount(0);
	TRequestStatus  enumStatus;
	
	User::After(1000000); // Gives SMS Prot and WAP Prot time to initialise after loading!
	enumerateSock.Ioctl(KIoctlEnumerateSmsMessages, enumStatus, &messageCount, KSolSmsProv);
	User::WaitForRequest(enumStatus);
	TESTL(enumStatus.Int() == KErrNone);
	TEST(messageCount() == 0);

	// Issue an Ioctl for getting the length of the enumerated datagram...
	TRequestStatus  ioctlStatus;
	TPckgBuf<TInt>  length;

	iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
	User::WaitForRequest(ioctlStatus);
	TESTL(ioctlStatus.Int() == KErrNone);
	TEST(length() == 118);

	// Receive the VCard...
	TRequestStatus  recvStatus;
	TBuf8<256>  recvBuf;
	TBuf<256>  vcardBuf16;
	TWapAddr  recvWapAddr;

	iSocket.RecvFrom(recvBuf, recvWapAddr, 0, recvStatus);
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone);
	
	vcardBuf16.Copy(recvBuf);
	INFO_PRINTF2(_L("Received the VCard: \"%S\""), &vcardBuf16);

	// Confirm the receipt of the message...
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));

	iSocket.Close();
    CleanupStack::PopAndDestroy(&enumerateSock);
	return TestStepResult();
	} 

/**
 *  Sends a 7-bit VCard with an email address in it, to ensure that '@'
 *  characters are sent correctly.
 * 
 *  @return EPass or EFail.
 */
TVerdict CTest7BitBusinessCardWithEmail::doTestStepL()
	{
    // Configure and bind to a socket
    SetupWapSocketL();

	//	Send a business card with the email address.
	TPtrC testData = GetStringFromConfigL(KTestData1);
	SendWapMessage(testData);

	ReceiveWapMessage(testData);
	
	iSocket.Close();
	return TestStepResult();
	}

/**
 *  Attempts to send datagrams bigger than the maximum message and bigger
 *  than the maximum size of an MBuf.
 * 
 *  @return EPass or EFail.
 */
TVerdict CTestOversizedDatagram::doTestStepL()
	{
    // Configure and bind to a socket
    SetupWapSocketL();

	// Send a datagram of the maximum datagram size (this will fail because
	// of the WAP headers)...
	TRequestStatus  status;
	
	HBufC8*  textBuf = HBufC8::NewLC(65536+1);
	TPtr8  textPtr = textBuf->Des();

	textPtr.Fill('A', KWAPSMSMaxDatagramSize);
	iSocket.SendTo(textPtr, iWapAddr, 0, status);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrOverflow);

	// Send a datagram bigger than the maximum datagram...
	textPtr.Fill('B', KWAPSMSMaxDatagramSize+1);
	iSocket.SendTo(textPtr, iWapAddr, 0, status);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrTooBig);

	// Send a datagram bigger than the maximum MBuf...
	textPtr.Fill('C', 65536+1);
	iSocket.SendTo(textPtr, iWapAddr, 0, status);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrTooBig);

	iSocket.Close();
	CleanupStack::PopAndDestroy(textBuf);
	return TestStepResult();
	}

TVerdict CTestWapDatagramSegmentContainingNoData::doTestStepL()
/** 
  *  Test step: Receive 3 Wap Datagrams, each WAP message contains
  *  one or more segments which comprise of a wap header but no data. 
  *  
  *  @return - TVerdict code
  */
    {
    INFO_PRINTF1(_L("CTestWapDatagramSegmentContainingNoData:"));
    // Configure and bind to a socket
    SetupWapSocketL();
    
    TVerdict verdict = EPass;

    // Create an empty string which will encode into a single segment wap message which contains a wap header, but no wap data.
    _LIT8(KLocalTestData1, "");
    TBuf8<300> data1(KLocalTestData1);      
    // Create a VCard which will encode into 2 segment wapdatagram, the 2nd segment contains a wap header, but no wap data.
    // Use a local literal string rather than reading from a text file. This prevents extra unwanted '\' being added to the string, for example \\r\\n
    _LIT8(KLocalTestData2,"BEGIN:VCARD\r\nVERSION:2.1\r\nREV:20090403T094807Z\r\nUID:83702f931a905a6e-00e14456815a8324-33\r\nN:;3SIM;;;\r\nTEL;WORK;CELL:07878931672\r\nEND:VCARD\r");
    TBuf8<300> data2(KLocalTestData2);


    TRequestStatus ioctlStatus;
    TRequestStatus recvStatus;
    TPckgBuf<TInt> length;

    //
    //  Issue Ioctl for getting the length of the message
    //
    INFO_PRINTF1(_L("Issue of the IOCTL for GetLength for first VCal entry"));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);


    TBuf8<300> data;

    //
    //  Send a SMS in order to trigger receiving the SMS messages.
    //  This is to make sure the SMS messages are not received before the 
    //  wap address is bound to the socket.
    // 
    TRequestStatus status;
    
    iSocket.SendTo(data1, iWapAddr, 0, status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    INFO_PRINTF1(_L("First Message (VCal) sent..."));

    //
    //  Get the size of the first incoming message
    //  The text string should be empty.
    //
    TBuf8<300> recvBuf;    
    User::WaitForRequest(ioctlStatus);
    User::LeaveIfError(ioctlStatus.Int());
    INFO_PRINTF1(_L("IOCTL completed"));
    
    //
    //  Check if reported length is correct
    //
    if(length()!=data1.Length())    
        {
        verdict = EFail;
        }
    
    //
    //  Receiving a datagram
    //
    TWapAddr recvWapAddr;
    
    iSocket.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
    User::WaitForRequest(recvStatus);
    User::LeaveIfError(recvStatus.Int());
    INFO_PRINTF1(_L("Received the first message (VCal)..."));

    //  The receive buffer should contain an empty string.
    //  This is as a consequence of the wapprot receiving
    //  a single segment wap message which contains no data.
    if(recvBuf.Compare(data1))
        {
        ERR_PRINTF1(_L("Non empty string returned"));
        verdict = EFail;
        }       

    //
    //  Confirm the receipt of the message to the client
    //
    INFO_PRINTF1(_L("Socket set option for indicating receipt of first message (VCal)"));
    User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));


    //Issue Ioctl for getting the length of the second message  
    INFO_PRINTF1(_L("Issue of the IOCTL for GetLength for second VCard entry"));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

    //Get the size of the second incoming message
    User::WaitForRequest(ioctlStatus);
    User::LeaveIfError(ioctlStatus.Int());
    INFO_PRINTF1(_L("IOCTL completed for second message (VCard)"));

    //Check if reported length is correct
    if(length()!=data2.Length())
        {
        verdict = EFail;
        }

    //Receiving a datagram (VCard)
    iSocket.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
    User::WaitForRequest(recvStatus);
    User::LeaveIfError(recvStatus.Int());
    INFO_PRINTF1(_L("Received the second message (VCard)..."));

    //  Compare the received VCard message with the original one.
    //  They should be identical. This checks that the WAPPROT
    //  can receive a 2 segment wap datagram, with the 2nd segment containing no data.
    //
    if(recvBuf.Compare(data2))
        {
        ERR_PRINTF1(_L("VCard message received does not match with original message."));
        verdict = EFail;
        }

    //Confirm the receipt of the message to the client  
    INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
    User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));
    //Issue Ioctl for getting the length of the third message  

    INFO_PRINTF1(_L("Issue of the IOCTL for GetLength for second VCard entry"));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
     
     //Get the size of the third incoming message
    User::WaitForRequest(ioctlStatus);
    User::LeaveIfError(ioctlStatus.Int());
    INFO_PRINTF1(_L("IOCTL completed for third message (VCard)"));

    //Check if reported length is correct   
    if(length()!=data1.Length())
        {
        verdict = EFail;
        }
     
    //Receiving a datagram (VCard)
    iSocket.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
    User::WaitForRequest(recvStatus);
    User::LeaveIfError(recvStatus.Int());
    INFO_PRINTF1(_L("Received the third message (VCard)..."));

    //  The receive buffer should contain an empty string.
    //  This is as a consequence of the WAPPROT containing 2 concatenated WAP Datagrams
    //  each datagram segement containing no data.
    if(recvBuf.Compare(data1))
        {
        ERR_PRINTF1(_L("VCard message received does not match with original message."));
        verdict = EFail;
        }

    //Confirm the receipt of the message to the client  
    INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
    User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));  

    iSocket.Close();
    SetTestStepResult(verdict);
    return TestStepResult();
    }

