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
// TS_SMSCAPSSTEP.CPP
// 
//

#include <smsuaddr.h>
#include <gsmubuf.h>
#include <gsmuset.h>
#include <smsustrm.h>
#include <mmlist.h>
#include <simtsy.h>
#include <es_wsms.h>
#include <wap_sock.h>
#include "sacls.h"
#include "te_smscapsstep.h"


const TUint KSocketMessageSlots = 16;	// Override the default value of 8

// SMS PARAMETERS STORAGE
#define SMSCAPS_STORE_SCA								_L("02075474324")
#define SMSCAPS_STORE_EVENT							    RMobilePhoneStore::EStoreFull
#define SMSCAPS_STORE_INDEX1							(1)
#define SMSCAPS_STORE_TEXT1								_L("Katie's SMS address")
#define SMSCAPS_STORE_VALID_PARAMS1						(TUint8)(0x1F)
#define SMSCAPS_STORE_PID1								(TUint8)(0x34)
#define SMSCAPS_STORE_DCS1								(TUint8)(0x8A)
#define SMSCAPS_STORE_VALIDITY1							(TUint8)(0x91)

#define SMSCAPS_STORE_INDEX2							(2)
#define SMSCAPS_STORE_TEXT2								_L("Ian's SMS address")
#define SMSCAPS_STORE_VALID_PARAMS2						(TUint8)(0x1F)
#define SMSCAPS_STORE_PID2								(TUint8)(0x73)
#define SMSCAPS_STORE_DCS2								(TUint8)(0xF2)
#define SMSCAPS_STORE_VALIDITY2							(TUint8)(0x04)

#define SMSCAPS_STORE_STATUS							(TBool)(EFalse)
#define SMSCAPS_STORE_COUNT								(2)

#define SMSCAPS_MESSAGE_DESTINATION_TON_0				(RMobilePhone::EInternationalNumber)
#define SMSCAPS_MESSAGE_DESTINATION_NP_0					(RMobilePhone::EIsdnNumberPlan)
#define SMSCAPS_MESSAGE_DESTINATION_TEL_NUMBER_0			_L("358405202000")

#define SMSCAPS_MESSAGE_GSMSERVICECENTRE_TON_1			(RMobilePhone::EUnknownNumber)
#define SMSCAPS_MESSAGE_GSMSERVICECENTRE_NP_1			(RMobilePhone::EUnknownNumberingPlan)
#define SMSCAPS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_1	_L("0405202000")


TVerdict CSmsCapsTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	
	TInt ret=iSocketServer.Connect(KSocketMessageSlots);
    if(ret !=KErrNone)
    	return EFail;
    return TestStepResult();
	}

TVerdict CSmsCapsTestStep::doTestStepPostambleL()
	{
	TRequestStatus status;
	iSocketServer.StopProtocol(KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol,status);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	iSocketServer.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

CSmsCapsTestStep::CSmsCapsTestStep(RSocketServ& aSockServer,RFs& aFs)
  : iSocketServer(aSockServer),
    iFs(aFs)
	{
	// NOP
	}


CSmsCapsBase::CSmsCapsBase(RSocketServ& aSockServer, RFs& aFs)
  : CSmsCapsTestStep(aSockServer,aFs)
	{
	// NOP
	}


TVerdict CSmsCapsBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	//
	// turn phone on
	//
    User::LeaveIfError(RProperty::Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
    User::After(1000000);

	return CSmsCapsTestStep::doTestStepPreambleL();
	}

TVerdict CSmsCapsBase::doTestStepPostambleL()
	{

	//
	// turn phone off
	//
    User::LeaveIfError(RProperty::Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
    User::After(1000000);

	__UHEAP_MARKEND;

	return CSmsCapsTestStep::doTestStepPostambleL();
	}


CSmsMessage* CSmsCapsTestStep::CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType)
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

	TSmsServiceCenterAddress telephoneNumber=_L("+358503878682"); //Test SIM
	TSmsServiceCenterAddress serviceCenterNumber=_L("+358508771010"); //Radiolinja

	smsMessage->SetToFromAddressL(telephoneNumber);
	smsMessage->SmsPDU().SetServiceCenterAddressL(serviceCenterNumber);
	buffer->InsertL(0,aDes);
	CleanupStack::Pop(smsMessage);
	return smsMessage;
	}

CSmsCapsSmsIoctlDeleteSms::CSmsCapsSmsIoctlDeleteSms(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsIoctlDeleteSms::doTestStepL()
	{

	RSocket socket;
	TRequestStatus status;
	TInt ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret=socket.Bind(smsaddr);

	if(RProcess().HasCapability(ECapabilityWriteUserData, ECapabilityNetworkServices) && RProcess().HasCapability(ECapabilityReadUserData))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult() ;
		}



	// Create message with SRR
	_LIT(KText,"This is a simple SMS!");
	CSmsMessage* msg=CreateSmsMessageL(KText,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(msg);

	RSmsSocketWriteStream writestream(socket);
	writestream << *msg;
	writestream.CommitL();
	CleanupStack::PopAndDestroy(msg);

	socket.Ioctl(KIoctlDeleteSmsMessage, status, NULL, KSolSmsProv);
	User::WaitForRequest(status);

	if(RProcess().HasCapability(ECapabilityWriteUserData))
		{
		TEST(status.Int() != KErrPermissionDenied);
		}
	else
		{
		TEST(status.Int() == KErrPermissionDenied);
		}

	CleanupStack::PopAndDestroy(&socket);

	return TestStepResult();
	}



CSmsCapsSmsIoctlEnumerateSms::CSmsCapsSmsIoctlEnumerateSms(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsIoctlEnumerateSms::doTestStepL()
	{
	RSocket socket;
	TRequestStatus status;
	TInt ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	CleanupClosePushL(socket);

	if(ret!=KErrNone)
		User::Leave(ret);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret=socket.Bind(smsaddr);

	if(RProcess().HasCapability(ECapabilityWriteUserData, ECapabilityNetworkServices) && RProcess().HasCapability(ECapabilityReadUserData))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult() ;
		}


	TPckgBuf<TUint> sbuf;
	sbuf()=0;

	// test cancel first - due to Lubbock problems canceled out
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);

	User::WaitForRequest(status);

	if(RProcess().HasCapability(ECapabilityReadUserData))
		{
		TEST(status.Int() != KErrPermissionDenied);
		}
	else
		{
		TEST(status.Int() == KErrPermissionDenied);
		}

	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}

//

CSmsCapsSmsIoctlReadSucceededSms::CSmsCapsSmsIoctlReadSucceededSms(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsBase(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsIoctlReadSucceededSms::doTestStepL()
	{
	TInt ret;

	//
	// Set TSY to the test case that has read message
	// #TestSimpleRxL in tsmsprt-config.txt
	//
	TInt testNo=6;

	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);

	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,testNo));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (testNo != testNumberCheck)
		User::Leave(KErrNotFound);

	CleanupStack::PopAndDestroy(&testNumberProperty);

	RSocket socket;
	ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret=socket.Bind(smsaddr);

	if(RProcess().HasCapability(ECapabilityWriteUserData, ECapabilityNetworkServices) && RProcess().HasCapability(ECapabilityReadUserData))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult() ;
		}

	//
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	RSmsSocketReadStream readstream(socket);
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal();
	timer.After(timerStatus, TTimeIntervalMicroSeconds32(20000000));

	TPckgBuf<TUint> sbuf;
	sbuf()=KSockSelectRead;
	socket.Ioctl(KIOctlSelect, status, &sbuf, KSOLSocket);
	User::WaitForRequest(timerStatus, status);
	if(status.Int() != KErrNone)
		{
		socket.CancelIoctl();
    	User::WaitForRequest(status);
        ret = EFalse;
		}
    else
        {
        timer.Cancel();
    	User::WaitForRequest(timerStatus);
        ret = ETrue;
        }
    timer.Close();

	if(ret)
	{
		TRAP(ret,readstream >> *smsMessage);
		User::LeaveIfError(ret);
		socket.Ioctl(KIoctlReadMessageSucceeded, status, NULL, KSolSmsProv);
		User::WaitForRequest(status);

		if(RProcess().HasCapability(ECapabilityReadUserData))
			{
			TEST(status.Int() != KErrPermissionDenied);
			}
		else
			{
			TEST(status.Int() == KErrPermissionDenied);
			}
	}

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}

//
CSmsCapsSmsIoctlReadFailedSms::CSmsCapsSmsIoctlReadFailedSms(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsBase(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsIoctlReadFailedSms::doTestStepL()
	{

	TInt ret;
	//
	// Set TSY to the test case that has read message
	// #TestSimpleRxL in tsmsprt-config.txt
	//
	TInt testNo=6;

	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);

	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,testNo));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (testNo != testNumberCheck)
		User::Leave(KErrNotFound);

	CleanupStack::PopAndDestroy(&testNumberProperty);
	//

	RSocket socket;
	ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret=socket.Bind(smsaddr);

	if(RProcess().HasCapability(ECapabilityWriteUserData, ECapabilityNetworkServices) && RProcess().HasCapability(ECapabilityReadUserData))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult() ;
		}


	//
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	RSmsSocketReadStream readstream(socket);
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal();
	timer.After(timerStatus, TTimeIntervalMicroSeconds32(20000000));

	TPckgBuf<TUint> sbuf;
	sbuf()=KSockSelectRead;
	socket.Ioctl(KIOctlSelect, status, &sbuf, KSOLSocket);
	User::WaitForRequest(timerStatus, status);
	if(status.Int() != KErrNone)
		{
		socket.CancelIoctl();
    	User::WaitForRequest(status);
        ret = EFalse;
		}
    else
        {
        timer.Cancel();
    	User::WaitForRequest(timerStatus);
        ret = ETrue;
        }
    timer.Close();

	if(ret)
	{
		TRAP(ret,readstream >> *smsMessage);
		User::LeaveIfError(ret);
		socket.Ioctl(KIoctlReadMessageFailed, status, NULL, KSolSmsProv);
		User::WaitForRequest(status);

		if(RProcess().HasCapability(ECapabilityReadUserData))
			{
			TEST(status.Int() != KErrPermissionDenied);
			}
		else
			{
			TEST(status.Int() == KErrPermissionDenied);
			}
	}

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
	return	TestStepResult() ;
	}


//
CSmsCapsSmsIoctlSendSms::CSmsCapsSmsIoctlSendSms(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsIoctlSendSms::doTestStepL()
	{

	RSocket socket;
	TRequestStatus status;
	TInt ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret=socket.Bind(smsaddr);

	if(RProcess().HasCapability(ECapabilityNetworkServices))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult() ;
		}


	// Create message with SRR
	_LIT(KText,"This is a simple SMS!");
	CSmsMessage* msg=CreateSmsMessageL(KText,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(msg);


	// Send message
	RSmsSocketWriteStream writestream(socket);
	TRAP(ret,writestream << *msg);
	User::LeaveIfError(ret);
	TRAP(ret,writestream.CommitL());
	User::LeaveIfError(ret);

	TPckgBuf<TUint> sbuf;
	socket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);

	TEST(ret != KErrPermissionDenied);

	CleanupStack::PopAndDestroy(msg);
	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}


//

CSmsCapsSmsIoctlWriteSms::CSmsCapsSmsIoctlWriteSms(RSocketServ& aSockServ, RFs& aFs)
  :  CSmsCapsBase(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsIoctlWriteSms::doTestStepL()
	{

	TInt ret;
	//
	// Set TSY to the test case
	//
	//
	TInt testNo=13;

	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);

	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,testNo));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (testNo != testNumberCheck)
		User::Leave(KErrNotFound);

	CleanupStack::PopAndDestroy(&testNumberProperty);
	//

	RSocket socket;
	ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret=socket.Bind(smsaddr);

	if(RProcess().HasCapability(ECapabilityWriteUserData, ECapabilityNetworkServices) && RProcess().HasCapability(ECapabilityReadUserData))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult() ;
		}


	// Create message with SRR
	_LIT(KText,"This is a simple SMS!");
	CSmsMessage* msg=CreateSmsMessageL(KText,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(msg);


	// Send message
	RSmsSocketWriteStream writestream(socket);
	TRAP(ret,writestream << *msg);
	User::LeaveIfError(ret);
	TRAP(ret,writestream.CommitL());
	User::LeaveIfError(ret);

	socket.Ioctl(KIoctlWriteSmsMessage,status,NULL, KSolSmsProv);
	User::WaitForRequest(status);

	if(RProcess().HasCapability(ECapabilityWriteUserData))
			{
			TEST(status.Int() != KErrPermissionDenied);
			}
		else
			{
			TEST(status.Int() == KErrPermissionDenied);
			}

	CleanupStack::PopAndDestroy(msg);
	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}

//

CSmsCapsSmsIoctlReadParamsSms::CSmsCapsSmsIoctlReadParamsSms(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsBase(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsIoctlReadParamsSms::doTestStepL()
	{
	TInt testNo=13;

	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);

	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,testNo));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (testNo != testNumberCheck)
		User::Leave(KErrNotFound);

	CleanupStack::PopAndDestroy(&testNumberProperty);

	RSocket socket;
	TInt ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrLocalOperation);

	ret=socket.Bind(smsaddr);

	if(RProcess().HasCapability(ECapabilityNetworkServices))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult() ;
		}


	socket.Ioctl(KIoctlReadSmsParams,status,NULL,KSolSmsProv);
	User::WaitForRequest(status);

	TEST(status.Int() != KErrPermissionDenied);

	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}


	//

CSmsCapsSmsIoctlCompleteReadParamsSms::CSmsCapsSmsIoctlCompleteReadParamsSms(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsBase(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsIoctlCompleteReadParamsSms::doTestStepL()
	{

	TInt ret;
	//
	// Set TSY to the test case that has read message
	// #TestSimpleRxL in tsmsprt-config.txt
	//
	TInt testNo=13;

	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);

	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,testNo));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (testNo != testNumberCheck)
		User::Leave(KErrNotFound);

	CleanupStack::PopAndDestroy(&testNumberProperty);

	RSocket socket;
	ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	User::LeaveIfError(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrLocalOperation);

	ret=socket.Bind(smsaddr);
	if(RProcess().HasCapability(ECapabilityNetworkServices))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult();
		}

	// Small delay to allow the SMS Stack PRT to initialise...
	User::After(1*1000000);

	RSmsSocketReadStream readstream(socket);

	//Create the smspList
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

    // Make read SMS params request to the SMS Stack.
    socket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
    User::WaitForRequest(status);

    // Read list from stream and make acknowledgement to the SMS Stack
    readstream >> *smspList;
    socket.Ioctl(KIoctlCompleteReadSmsParams, status, NULL,KSolSmsProv);
    User::WaitForRequest(status);

	TEST(status.Int() != KErrPermissionDenied);

	CleanupStack::PopAndDestroy(smspList);
	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}

//
CSmsCapsSmsIoctlWriteParamsSms::CSmsCapsSmsIoctlWriteParamsSms(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}


TVerdict CSmsCapsSmsIoctlWriteParamsSms::doTestStepL()
	{

	RSocket socket;
	TRequestStatus status;
	TInt ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrLocalOperation);
	ret=socket.Bind(smsaddr);
	if(RProcess().HasCapability(ECapabilityWriteUserData, ECapabilityNetworkServices) && RProcess().HasCapability(ECapabilityReadUserData))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult();
		}



	CMobilePhoneSmspList* smspList=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy;

	entryToTsy.iText = SMSCAPS_STORE_TEXT1;
	entryToTsy.iValidParams = SMSCAPS_STORE_VALID_PARAMS1;
    entryToTsy.iDestination.iTypeOfNumber = SMSCAPS_MESSAGE_DESTINATION_TON_0;
    entryToTsy.iDestination.iNumberPlan = SMSCAPS_MESSAGE_DESTINATION_NP_0;
	entryToTsy.iDestination.iTelNumber = SMSCAPS_MESSAGE_DESTINATION_TEL_NUMBER_0;
	entryToTsy.iValidityPeriod = SMSCAPS_STORE_VALIDITY1;
    entryToTsy.iServiceCentre.iTypeOfNumber = SMSCAPS_MESSAGE_GSMSERVICECENTRE_TON_1;
    entryToTsy.iServiceCentre.iNumberPlan = SMSCAPS_MESSAGE_GSMSERVICECENTRE_NP_1;
	entryToTsy.iServiceCentre.iTelNumber = SMSCAPS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_1;
	entryToTsy.iProtocolId = SMSCAPS_STORE_PID1;
	entryToTsy.iDcs = SMSCAPS_STORE_DCS1;

	entryToTsy.iIndex = 0;
	smspList->AddEntryL(entryToTsy);

	RSmsSocketWriteStream writestream(socket);
	TRAP(ret,writestream << *smspList);
	User::LeaveIfError(ret);
	TRAP(ret,writestream.CommitL());
	User::LeaveIfError(ret);

	socket.Ioctl(KIoctlWriteSmsParams,status,NULL, KSolSmsProv);
 	User::WaitForRequest(status);

	if(RProcess().HasCapability(ECapabilityWriteDeviceData))
			{
			TEST(status.Int() != KErrPermissionDenied);
			}
		else
			{
			TEST(status.Int() == KErrPermissionDenied);
			}

	CleanupStack::PopAndDestroy(smspList);
	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}


//
CSmsCapsSmsSetLocalName::CSmsCapsSmsSetLocalName(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsSetLocalName::doTestStepL()
	{

	RSocket socket;
	TInt ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);

	ret=socket.Bind(smsaddr);

	if(RProcess().HasCapability(ECapabilityNetworkServices))
		{
		TEST(ret != KErrPermissionDenied);
		}
	else
		{
		TEST(ret == KErrPermissionDenied);
		}

	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}

//
CSmsCapsSmsWrite::CSmsCapsSmsWrite(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsSmsWrite::doTestStepL()
	{

	RSocket socket;
	TInt ret=socket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	if(ret!=KErrNone)
		User::Leave(ret);
	CleanupClosePushL(socket);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret=socket.Bind(smsaddr);

	// needs WriteUserData, as this is required by protecting policy
	// "smsProviderIoctlWriteSmsParamsPolicy"
	if(RProcess().HasCapability(ECapabilityWriteUserData, ECapabilityNetworkServices))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&socket);
		return TestStepResult();
		}

	// Create message with SRR
	_LIT(KText,"This is a simple SMS!");
	CSmsMessage* msg=CreateSmsMessageL(KText,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(msg);


	// Write message
	RSmsSocketWriteStream writestream(socket);
	CleanupClosePushL(writestream);
	TRAP(ret,writestream << *msg);
	if(ret== KErrPermissionDenied)
		{
		TEST(EFalse);
		}
	else
		{
		TRAP(ret,writestream.CommitL());
		if(ret== KErrPermissionDenied)
			{
			TEST(EFalse);
			}
		else
			{
			TRequestStatus status;
			socket.Ioctl(KIoctlWriteSmsMessage,status,NULL, KSolSmsProv);
 			User::WaitForRequest(status);

			TEST( status.Int() != KErrPermissionDenied);
			}
		}

	CleanupStack::PopAndDestroy(&writestream);
	CleanupStack::PopAndDestroy(msg);
	CleanupStack::PopAndDestroy(&socket);

	return	TestStepResult() ;
	}


// WAP
//
CSmsCapsWapSetLocalName::CSmsCapsWapSetLocalName(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsWapSetLocalName::doTestStepPostambleL()
	{
	TRequestStatus status;
	iSocketServer.StopProtocol(KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol,status);
	User::WaitForRequest(status);
	iSocketServer.Close();

	__UHEAP_MARKEND;

	return TestStepResult() ;
	}


TVerdict CSmsCapsWapSetLocalName::doTestStepL()
	{
	RSocket wapsock;
	TInt ret=wapsock.Open(iSocketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	User::LeaveIfError(ret);
	CleanupClosePushL(wapsock);

	TWapAddr  wapAddr;

	wapAddr.SetWapPort(TWapPortNumber(9204));

	ret=wapsock.Bind(wapAddr);

	if(RProcess().HasCapability(ECapabilityNetworkServices))
		{
		TEST(ret != KErrPermissionDenied);
		}
	else
		{
		TEST(ret == KErrPermissionDenied);
		}

	CleanupStack::PopAndDestroy(&wapsock);

	return	TestStepResult() ;
	}

//
CSmsCapsWapWrite::CSmsCapsWapWrite(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsWapWrite::doTestStepPostambleL()
	{
	TRequestStatus status;
	iSocketServer.StopProtocol(KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol,status);
	User::WaitForRequest(status);
	iSocketServer.Close();
	
	__UHEAP_MARKEND;

	return TestStepResult() ;
	}

TVerdict CSmsCapsWapWrite::doTestStepL()
	{

	RSocket wapsock;
	TRequestStatus status;
	TInt ret=wapsock.Open(iSocketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	User::LeaveIfError(ret);
	CleanupClosePushL(wapsock);

	TWapAddr wapAddr;

	wapAddr.SetWapPort(TWapPortNumber(9204));
	ret=wapsock.Bind(wapAddr);
	if(RProcess().HasCapability(ECapabilityNetworkServices))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&wapsock);
		return TestStepResult() ;
		}


	_LIT(KText,"This is a simple SMS!");
	TBuf8<200> data;
	data.Copy(KText);

	wapsock.SendTo(data, wapAddr, 0, status);
	User::WaitForRequest(status);

	TEST(status.Int() != KErrPermissionDenied);

	CleanupStack::PopAndDestroy(&wapsock);

	return	TestStepResult() ;
	}

//

CSmsCapsWapIoctl::CSmsCapsWapIoctl(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsWapIoctl::doTestStepPostambleL()
	{
	TRequestStatus status;
	iSocketServer.StopProtocol(KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol,status);
	User::WaitForRequest(status);
	iSocketServer.Close();

	__UHEAP_MARKEND;

	return TestStepResult() ;
	}

TVerdict CSmsCapsWapIoctl::doTestStepL()
	{

	RSocket wapsock;
	TRequestStatus status;
	TInt ret=wapsock.Open(iSocketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	User::LeaveIfError(ret);
	CleanupClosePushL(wapsock);

	TWapAddr wapAddr;

	wapAddr.SetWapPort(TWapPortNumber(9204));
	ret=wapsock.Bind(wapAddr);

	if(RProcess().HasCapability(ECapabilityNetworkServices))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&wapsock);
		return TestStepResult() ;
		}

	TPckgBuf<TInt> length;
	//
	//	Issue Ioctl for getting the length of the message (should fail)
	//
	wapsock.Ioctl(KSOGetLength, status, &length, KSolWapProv);
	User::WaitForRequest(status);

	TEST(status.Int() != KErrPermissionDenied);

	CleanupStack::PopAndDestroy(&wapsock);

	return	TestStepResult() ;
	}

//

CSmsCapsWapSetOption::CSmsCapsWapSetOption(RSocketServ& aSockServ, RFs& aFs)
  : CSmsCapsTestStep(aSockServ,aFs)
	{
	// NOP
	}

TVerdict CSmsCapsWapSetOption::doTestStepPostambleL()
	{
	TRequestStatus status;
	iSocketServer.StopProtocol(KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol,status);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	iSocketServer.Close();

	__UHEAP_MARKEND;

	return TestStepResult() ;
	}



TVerdict CSmsCapsWapSetOption::doTestStepL()
	{
	RSocket wapsock;

	TInt ret=wapsock.Open(iSocketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	User::LeaveIfError(ret);
	CleanupClosePushL(wapsock);

	TWapAddr wapAddr;

	wapAddr.SetWapPort(TWapPortNumber(9204));
	ret=wapsock.Bind(wapAddr);
	if(RProcess().HasCapability(ECapabilityNetworkServices))
		{
		TESTL(ret != KErrPermissionDenied);
		}
	else
		{
		// skip this one
		CleanupStack::PopAndDestroy(&wapsock);
		return TestStepResult() ;
		}

	ret = wapsock.SetOpt(KWapSmsOptionNameDCS, KWapSmsOptionLevel, EWapSms8BitDCS);

	TEST(ret != KErrPermissionDenied);

	CleanupStack::PopAndDestroy(&wapsock);


	return	TestStepResult() ;
	}
