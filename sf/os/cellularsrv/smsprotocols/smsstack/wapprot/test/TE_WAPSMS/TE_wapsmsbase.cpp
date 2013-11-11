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
// @file
// This test program tests WAPPROT.PRT and SMSPROT.PRT
// WARNING:
// When using \the modified 7110 only sending will work.
// Receiving SMSs will not work because the 7110 snaps them up as smart
// messages. It will probably work with an old non-intelligent GSM phone.
// Nokia CardPhone 2.1 does not work
// possible sources for bugs:
// * you must make sure that the MessageCenterNumber in CommDB is set
// to a propriate SC number (e.g. +447785016005 for Vodafone)
// if this is not set then it will not work.
// you can edit this field using the T_EDITDB program
// * if you can't receive anything, try switching the 7110 off and then on
// 
//

#include <e32property.h>
#include <sacls.h>
#include "TE_wapsmsbase.h"

// implementation of helper functions

TInt CWapSmsTestStep::CommInit()
{
	iTelephoneNumber=_L("+358503878682");
	iServiceCenterNumber=_L("+358508771010");

    
    INFO_PRINTF1(_L("CommInit: Starting C32 ") );
    TInt ret = StartC32();
    return (ret == KErrNone || ret == KErrAlreadyExists) ? KErrNone : ret;
}

void CWapSmsTestStep::InitGlobalsL()
/**
 *  Initialise global variables.
 */
{
	INFO_PRINTF1(_L("Connecting to file server ") );
	TInt ret = iFs.Connect();
    TESTL(ret == KErrNone);

    User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn ));
    User::After(5000000); //wait 5s here for the completion of the previous
                         // asynchronous request

    TRAPD( ret1, ParseSettingsFromFileL());
	TESTL(ret1 == KErrNone);

	INFO_PRINTF2(_L("TelNo: [%S]"), &iTelephoneNumber);
    INFO_PRINTF2(_L("ScNo:  [%S]"), &iServiceCenterNumber);
}

void CWapSmsTestStep::DestroyGlobals()
/**
 *  Destroy global variables
 */
{
	iFs.Close();
}

void CWapSmsTestStep::Print8BitDes(const TDesC8& aDes8)
/**
 *  Print an 8 bit descriptor to the console
 */
{
	TBuf<128> buf;
	TInt len=aDes8.Length();
	const TText8* textPtr=aDes8.Ptr();
	while(len)
	{
		TInt copyLen = Min(len,buf.MaxLength());
		TPtrC8 viewOverDes8(textPtr,copyLen);
		len -= copyLen;
		textPtr+=copyLen;
		buf.Copy(viewOverDes8);
		INFO_PRINTF1(buf);
	}
}

void CWapSmsTestStep::WaitForInitializeL()
/**
 *  Initialize the phone for the tsy. This will prevent message sends from
 *  completing with KErrNotReady
 */
{
	CCommsDatabase* commdb = CCommsDatabase::NewL(EDatabaseTypeUnspecified);
	CleanupStack::PushL(commdb);

	TName tsy;

	TUint32 modemId = 0;
	commdb->GetGlobalSettingL(TPtrC(MODEM_PHONE_SERVICES_SMS), modemId);

	// Now read the TSY name from the modem table record
	CCommsDbTableView* tableView = commdb->OpenViewMatchingUintLC(TPtrC(MODEM),TPtrC(COMMDB_ID),modemId);
	User::LeaveIfError(tableView->GotoFirstRecord());
	tableView->ReadTextL(TPtrC(MODEM_TSY_NAME), tsy);
	CleanupStack::PopAndDestroy(tableView);
	CleanupStack::PopAndDestroy(commdb);

	INFO_PRINTF2(_L("Using TSY \"%S\"\nLoading RTelServer..."), &tsy);

	RTelServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);
	User::LeaveIfError(server.LoadPhoneModule(tsy));

	// Find the phone corresponding to this TSY and open a number of handles on it
	TInt numPhones;
	User::LeaveIfError(server.EnumeratePhones(numPhones));
	RPhone phone;
	TBool found=EFalse;

	while (numPhones--)
	{
		TName phoneTsy;
		User::LeaveIfError(server.GetTsyName(numPhones,phoneTsy));
		if (phoneTsy.CompareF(tsy)==KErrNone)
		{
			INFO_PRINTF1(_L("Found RPhone..."));
			found = ETrue;
			RTelServer::TPhoneInfo info;
			User::LeaveIfError(server.GetPhoneInfo(numPhones,info));
			User::LeaveIfError(phone.Open(server,info.iName));
			CleanupClosePushL(phone);
			INFO_PRINTF1(_L("Initalizing..."));
			const TInt err = phone.Initialise();
			INFO_PRINTF2(_L("Completed Intialize [err=%d]"), err);
			User::LeaveIfError(err);
			CleanupStack::PopAndDestroy(&phone);
			break;
		}
	}

	TESTL(found);
	CleanupStack::PopAndDestroy(&server);
}

void CWapSmsTestStep::ParseSettingsFromFileL()
{
	CTestConfig* configFile = CTestConfig::NewLC(iFs,KConfigFileDir,KGmsSmsConfigFileName);
	const CTestConfigSection* cfgFile = configFile->Section(KSetupTelNumbers);
	if (cfgFile == NULL)
	{
		User::Leave(KErrNotFound);
	}
	const CTestConfigItem* item = cfgFile->Item(KServiceCenter,0);
	if (item == NULL)
	{
		User::Leave(KErrNotFound);
	}
	iServiceCenterNumber.Copy(item->Value());

	item = cfgFile->Item(KTelefoneNumber,0);
	if (item == NULL)
	{
		User::Leave(KErrNotFound);
	}
	iTelephoneNumber.Copy(item->Value());
	CleanupStack::PopAndDestroy(configFile);
}


void CWapSmsTestStep::Connect2EsockLC(RSocketServ& aSocketServer)
/**
 *  seperating the connection to esock due to calling in every test case
 */
{
	// delete segmentation and reassembly store files before the test
	const TUint KSocketMessageSlots = 16;
	iFs.Delete(KReassemblyStoreName);
	iFs.Delete(KSegmentationStoreName);

    INFO_PRINTF1(_L(" ... Connecting to Socket Server ... ") );

    TInt ret=aSocketServer.Connect(KSocketMessageSlots);
    TESTL(ret == KErrNone);

	CleanupClosePushL(aSocketServer);
}

CSmsMessage* CWapSmsTestStep::CreateSmsMessageL(const TDesC& aDes,
						TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
						CSmsPDU::TSmsPDUType aType)
/**
 *  Create a uninitialised SMS message
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, aType, buffer);
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

void CWapSmsTestStep::SendSmsL(const CSmsMessage* aSms, RSocket& aSocket)
/**
 *  Stream aSms out to the socket server
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with error code
 */
{
	TBool tryAgain = ETrue;
	TInt sendTry (0);
	TRequestStatus status = KErrNone;

	while (tryAgain && sendTry < 3)
	{
		RSmsSocketWriteStream writestream(aSocket);
		TRAPD(ret,writestream << *aSms);
		TESTL(ret == KErrNone);
		TRAP(ret,writestream.CommitL());
		TESTL(ret == KErrNone);

		TPckgBuf<TUint> sbuf;
		aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
		User::WaitForRequest(status);
		INFO_PRINTF2(_L("SendSmsL - sendSmsMessage returned %d"),status.Int());
		if (status.Int() )
		{
			tryAgain = ETrue;
			INFO_PRINTF1(_L("Try again... "));
			sendTry++;
		}
		else tryAgain = EFalse;
	}

	TESTL(status.Int() == KErrNone);
}

CSmsMessage* CWapSmsTestStep::RecvSmsL(RSocket& aSocket, TInt aIoctl)
/**
 *  Receive an Sms
 *  @param aSocket is used to stream the sms message from the socket server
 *  @return CSmsMessage* :Sms message from Sms stack
 *  @leave Leaves if streaming the message from the socket server doesn't succeed
 */
{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	RSmsSocketReadStream readstream(aSocket);
	TRAPD(ret,readstream >> *smsMessage);
	TESTL(ret == KErrNone);

	TPckgBuf<TUint> sbuf;
	TRequestStatus status;
	aSocket.Ioctl(aIoctl, status, &sbuf, KSolSmsProv);
	User::WaitForRequest(status);

	CleanupStack::Pop(smsMessage);
	return smsMessage;
}

void CWapSmsTestStep::WaitForRecvL(RSocket& aSocket)
/**
 *  Wait for an Sms to be received
 *  @param aSocket The status is return to this socket
 *  @leave Leaves if receiving is completed with error code
 */
{
	TPckgBuf<TUint> sbuf;
	sbuf()=KSockSelectRead;
	TRequestStatus status;
	aSocket.Ioctl(KIOctlSelect,status,&sbuf,KSOLSocket);
	User::WaitForRequest(status);
	if (status.Int() != KErrNone)
		INFO_PRINTF3(_L("status=%d Line=%d"),status.Int(),__LINE__);
	TESTL(status.Int() == KErrNone);
}

void CWapSmsTestStep::ReadFileL( TDesC8& aFileName, TDes8& aDes )
{
 	RFile file;
    TBuf<255> buf;

    buf.Copy(aFileName);

    User::LeaveIfError(file.Open(iFs, buf, EFileShareAny | EFileRead));
	CleanupClosePushL(file);

	TInt size = 0;
    User::LeaveIfError(file.Size(size));

    User::LeaveIfError(file.Read(0, aDes));

	CleanupStack::PopAndDestroy(&file);
}

void CWapSmsTestStep::DoTestTxRxWapL(TDesC8& aDes, TWapPortNumber aWpn,
											TWapSmsDataCodingScheme aDcs)
/**
 *  @test Send and Receive 8bit business card
 */
{
	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket sock1;
	TInt ret=sock1.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();

	TWapSmsDataCodingScheme codingScheme = aDcs;
	ret = sock1.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme);
	TESTL(ret == KErrNone);

	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf;
	TRequestStatus recvStatus;
	sock1.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	//
	// Sending a datagram
	//
	TWapAddr wapAddr;
	TRequestStatus status;
	wapAddr.SetWapPort(aWpn);
    TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret=sock1.Bind(wapAddr);
	TESTL(ret == KErrNone);

	sock1.SendTo(aDes,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK"),aDes.Length());

	//
	// Receiving a datagram
	//
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone);
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK"),recvBuf.Length());
	//Print8BitDes(recvBuf);
	TESTL(recvBuf == aDes);

   sock1.Close();

   socketServer.StopProtocol(KWAPSMSAddrFamily,KSockDatagram ,KWAPSMSDatagramProtocol,status);
   User::WaitForRequest(status);

   CleanupStack::PopAndDestroy(&socketServer);
}

TVerdict CWapSmsTestStep::doTestStepPreambleL( void )
{
	TInt ret = CommInit();
    TESTL(ret == KErrNone);
	InitGlobalsL();

	return TestStepResult() ;
}

TVerdict CWapSmsTestStep::doTestStepPostambleL( void )
{
	DestroyGlobals();

	return TestStepResult() ;
}
