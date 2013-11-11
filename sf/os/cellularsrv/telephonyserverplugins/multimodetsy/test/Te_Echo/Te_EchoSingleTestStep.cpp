// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed the test the data functionality of the Basic GsmTsy.
// The files te_data and te_echo are designed as a Data and Echo server test and should
// be run simultaneously on two EPOC machines. 
// The test scenario is outlined below:
// Machine A (te_data.exe)					Machine B (te_echo.exe)
// 1.		Dial
// 2.												Answer
// 3.		Transmit local tel. number
// 4.												Rx and log remote modem's tel. number
// 5.		Transmit Data
// 6.												Receive and then echo data
// 7.		Receive and Compare Data
// 8.		Hang up									Hang up
// 9.		Pause									Pause
// 10.											Dial remote modem's tel. number
// 11.	Answer
// 13.											Transmit Data
// 14.	Echo received data
// 15.											Receive and Compare Data
// 16.	Hang up									Hang up
// 
//

/**
 @file
*/

#include "Te_EchoSingleTestStep.h"

_LIT(KMmtsyName,"MM");
_LIT(KPhoneName,"GsmPhone1");
_LIT(KCsyName, "ECUART.CSY");
_LIT(KDataLineName,"Data");
_LIT8(KWriteTestData,"GSM TSY Data and Echo Server Test is still in progress...");

const TInt KTenSeconds=10000000;
const TInt KTimeToWaitForRemoteEnd=2*KTenSeconds; // Twenty Seconds
const TInt KTimeToWaitForRead=6*KTenSeconds; // Sixty Seconds

CEchoTestStep::CEchoTestStep()
	{
	SetTestStepName(KEchoTestStepName);
	}

TVerdict CEchoTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	SetTestStepResult(EPass);

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KMmtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}
	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KMmtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
//
//	This test requires a mobile phone to be connected to the serial port
//      specified in the .cfg file for the initialise below to work
//
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KMmtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
	ret = iPhone.GetCaps(iPhoneCaps) ;
	if ((KErrNone != ret) || !(iPhoneCaps.iFlags & RPhone::KCapsData))
		{
		INFO_PRINTF2(_L("This phone does not support data calls (caps=0x%x)"),iPhoneCaps.iFlags);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KMmtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CEchoTestStep::doTestStepPostambleL()
	{
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KMmtsyName);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}

TVerdict CEchoTestStep::doTestStepL()
	{
	AnswerIncomingCallTestL();
	User::After(KTimeToWaitForRemoteEnd);
	DialRemoteModemTestL();

	return TestStepResult();
	}

void CEchoTestStep::AnswerIncomingCallTestL()
/**
 * Answers an incoming data call.
 * This method opens a data line and call, and retrieves the status of the call.
 * It then answers an incoming call from a remote modem (initiated by t_data.exe), 
 * loans the Comm port, connects to the Comms Server, opens a serial port and proceeds
 * to read data transmitted by the remote modem. It then echoes the received data
 * closes the serial port, its connection to the Comms Server, the data port, call and 
 * line.
 * Leaves if test fails
 */
	{
	const TInt KExpectedBytes=51;		// Number of bytes expected from remote end

	RMobileLine line;
	INFO_PRINTF1(_L("Answer incomming call"));
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_FOR_ERROR_L(line.Open(iPhone,KDataLineName));
    CleanupClosePushL(line);

	INFO_PRINTF1(_L("Opening New Data Call"));
	RMobileCall dataCall;
	TEST_FOR_ERROR_L(dataCall.OpenNewCall(line));
    CleanupClosePushL(dataCall);

	INFO_PRINTF1(_L("Get call status"));
	RCall::TStatus callStatus;
	TEST_FOR_ERROR_L(dataCall.GetStatus(callStatus));
	INFO_PRINTF2(_L("Call Status = %d"),callStatus);

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	INFO_PRINTF1(_L("Answer incomming call"));
	dataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);		
	User::WaitForRequest(iStatus);
	TEST_FOR_ERROR_L(iStatus.Int());

	INFO_PRINTF1(_L("Get call status"));
	TEST_FOR_ERROR_L(dataCall.GetStatus(callStatus));
	INFO_PRINTF2(_L("Call Status = %d"),callStatus);
	
	INFO_PRINTF1(_L("Loan comm port"));
	RCall::TCommPort commPort;
	dataCall.LoanDataPort(iStatus,commPort);
	User::WaitForRequest(iStatus);
	if (iStatus==KErrCommsLineFail)
		{
		INFO_PRINTF1(_L("This error is currently known to only happen with the Motorola"));
		INFO_PRINTF1(_L("Time port. Please see the PhoneInfo.cpp file for more information."));
		}
	TEST_FOR_ERROR_L(iStatus.Int());

	INFO_PRINTF1(_L("Connect to the Comms Server and load the CSY"));
	RCommServ cs;
	TEST_FOR_ERROR_L(cs.Connect());
    CleanupClosePushL(cs);

	TInt ret=cs.LoadCommModule(KCsyName);
	if (!(ret==KErrNone || ret==KErrAlreadyExists))
		{
		FAIL_WITH_ERROR_L(ret);
		}

	INFO_PRINTF1(_L("Open a serial port"));
	RComm port;
	TEST_FOR_ERROR_L(port.Open(cs,commPort.iPort,ECommShared));
    CleanupClosePushL(port);

	INFO_PRINTF1(_L("Read 1st stream of data from the serial port"));

	port.Read(iStatus,KTimeToWaitForRead,iRemoteNumber);	
	User::WaitForRequest(iStatus);
	if (iStatus!=KErrNone)
		{
		if (iStatus==KErrTimedOut)
			{
			INFO_PRINTF1(_L("A Time out error occured during the read"));
			}
		FAIL_WITH_ERROR_L(iStatus.Int());
		}
	TBuf<KRemoteNum> convertTel;	// Convert the 8-bit data read from serial port to 
	convertTel.Copy(iRemoteNumber);	// 16-bit so that it can be displayed to the user
	INFO_PRINTF2(_L("Data stream 1 = %S"), &convertTel);

	INFO_PRINTF1(_L("Read 2nd stream of data from the serial port"));
	TBuf8<KExpectedBytes> readData;
	port.Read(iStatus,KTimeToWaitForRead,readData);
	User::WaitForRequest(iStatus);

	if (iStatus!=KErrNone)
		{
		if (iStatus==KErrTimedOut)
			{
			INFO_PRINTF1(_L("A Time out error occured during the read"));
			}
		FAIL_WITH_ERROR_L(iStatus.Int());
		}
	TBuf<KExpectedBytes> convertData;	// Convert the 8-bit data read from serial port to 
	convertData.Copy(readData);			// 16-bit so that it can be displayed to the screen
	INFO_PRINTF2(_L("Data stream 2 = %S"),&convertData);

	INFO_PRINTF1(_L("Echo the received data"));
	port.Write(iStatus,readData);
	User::WaitForRequest(iStatus);
	TEST_FOR_ERROR_L(iStatus.Int());

	CleanupStack::PopAndDestroy(); // closes cs
	CleanupStack::PopAndDestroy(); // closes port

	INFO_PRINTF1(_L("Recover data port"));
	TEST_FOR_ERROR_L(dataCall.RecoverDataPort());

	INFO_PRINTF1(_L("Hanging up the Data call"));
	TEST_FOR_ERROR_L(dataCall.HangUp()); 

	CleanupStack::PopAndDestroy(); // closes dataCall
	CleanupStack::PopAndDestroy(); // closes line

	return;
	}

void CEchoTestStep::DialRemoteModemTestL()
/**
 * Dials a data call.
 * This method opens a data line and call, and retrieves the status of the call.
 * It then dials a remote modem (initiated by t_data.exe), loans the Comm port, 
 * connects to the Comms Server, opens a serial port and proceeds to transmit data to 
 * the remote modem. It subsequently reads the data echoed back by the remote modem and 
 * compares the transmitted and received data. The serial port, the connection to the 
 * Comms Server, the data port, call and line are then closed.
 * Leaves if test fails
 */
	{
	const TInt KNumOfBytes=57; // Number of bytes to send to the remote end

	RMobileLine line;
	INFO_PRINTF1(_L("Dial remote modem"));
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_FOR_ERROR_L(line.Open(iPhone,KDataLineName));
    CleanupClosePushL(line);

	INFO_PRINTF1(_L("Opening New Data Call"));
	RMobileCall dataCall;
	TEST_FOR_ERROR_L(dataCall.OpenNewCall(line));
    CleanupClosePushL(dataCall);

	INFO_PRINTF1(_L("Get call status"));
	RCall::TStatus callStatus;
	TEST_FOR_ERROR_L(dataCall.GetStatus(callStatus));
	INFO_PRINTF2(_L("Call Status = %d"),callStatus);

	TBuf16<KRemoteNum> phoneNum; // Convert remote tel. no. to 16-bit data
	phoneNum.Copy(iRemoteNumber);
	INFO_PRINTF2(_L("Dialling %S...."), &phoneNum);

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	dataCall.Dial(iStatus, callDataParamsPckg, phoneNum);
	User::WaitForRequest(iStatus);
	TEST_FOR_ERROR_L(iStatus.Int());

	INFO_PRINTF1(_L("Get call status"));
	TEST_FOR_ERROR_L(dataCall.GetStatus(callStatus));
	INFO_PRINTF2(_L("Call Status = %d"),callStatus);

	INFO_PRINTF1(_L("Loan the Comm port"));
	RCall::TCommPort commPort;
	dataCall.LoanDataPort(iStatus,commPort);
	User::WaitForRequest(iStatus);
	
	if (iStatus==KErrCommsLineFail)
		{
		INFO_PRINTF1(_L("This error is currently known to only happen with the Motorola"));
		INFO_PRINTF1(_L("Time port. Please see the PhoneInfo.cpp file for more information."));
		}
	TEST_FOR_ERROR_L(iStatus.Int());

	INFO_PRINTF1(_L("Connect to the Comms Server and load the CSY"));
	RCommServ cs;
	TEST_FOR_ERROR_L(cs.Connect());
    CleanupClosePushL(cs);

	TInt ret=cs.LoadCommModule(KCsyName);
	if (!(ret==KErrNone || ret==KErrAlreadyExists))
		{
		FAIL_WITH_ERROR_L(ret);
		}

	INFO_PRINTF1(_L("Open a serial port"));
	RComm port;
	TEST_FOR_ERROR_L(port.Open(cs,commPort.iPort,ECommShared));
    CleanupClosePushL(port);

	User::After(KTimeToWaitForRemoteEnd);
	
	INFO_PRINTF1(_L("Write data stream"));	
	port.Write(iStatus,KWriteTestData);
	User::WaitForRequest(iStatus);
	TEST_FOR_ERROR_L(iStatus.Int());


	INFO_PRINTF1(_L("Read Echoed data"));	
	TBuf8<KNumOfBytes> echoedData;
	port.Read(iStatus,KTimeToWaitForRead,echoedData);
	User::WaitForRequest(iStatus);
	if (iStatus!=KErrNone)
		{
		if (iStatus==KErrTimedOut)
			{
			INFO_PRINTF1(_L("A Time out error occured during the read"));
			}
		FAIL_WITH_ERROR_L(iStatus.Int());
		}

	if (echoedData!=KWriteTestData)
		{
		INFO_PRINTF1(_L("Data does not match error"));
		FAIL_WITH_ERROR_L(KErrCorrupt);
		}

	TBuf<KNumOfBytes> convertData; // Convert the 8-bit data read from serial port to 16-bit 
	convertData.Copy(echoedData); // so that it can be displayed to the user.
	INFO_PRINTF2(_L("Echoed data stream = %S\n"),&convertData);

	// Close the serial port, connection to Comms Server and recover the data port
	CleanupStack::PopAndDestroy(); // closes cs
	CleanupStack::PopAndDestroy(); // closes port

	INFO_PRINTF1(_L("Recover data port"));
	TEST_FOR_ERROR_L(dataCall.RecoverDataPort());

	INFO_PRINTF1(_L("Hanging up the Data call"));
	TEST_FOR_ERROR_L(dataCall.HangUp()); 

	CleanupStack::PopAndDestroy(); // closes dataCall
	CleanupStack::PopAndDestroy(); // closes line
	}
