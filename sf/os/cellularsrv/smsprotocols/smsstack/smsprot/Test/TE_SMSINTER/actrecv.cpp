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

#include <gsmubuf.h>
#include <gsmuset.h>
#include <smsustrm.h>

#include "TE_smsinterbase.h"

const TInt KActiveReceiverSocketGranularity = 8;
const TInt KActiveSocketPriority = CActive::EPriorityIdle;

class CStopIfKeyPressed;

class MStopTest
	{
public:
	virtual void StopTests() = 0;
	};

//
//
// CActiveSocket
//
//

class CActiveSocket : public CActive
	{
public:
	enum { EPriority = KActiveSocketPriority };
public:
	static CActiveSocket* NewL(const TTestCase& aTest, RSocketServ& aSocketServer, RFs& aFs);
	~CActiveSocket();
	void ReceiveMessage(TRequestStatus* aStatus);
	void SetIndex(TInt aIndex);
	void TestResultL();
	TBool TestComplete();
	TPtrC Message();
private:
	virtual void DoCancel();
	virtual void RunL();
	CActiveSocket(RSocketServ& aSocketServer);
private:
	void InitRSocketL();
private:
	TRequestStatus* iActiveReceiverStatus;
	RSocket iSocket;
	TTestCase iTestCase;
	CSmsMessage* iSmsMessage;
	TInt iActiveReceiverIndex;
	TPckgBuf<TUint> iSelectBuf;
	RSocketServ& iSocketServer;
	};

//
//
// CActiveSenderImp
//
//

class CActiveSenderImp : public CActiveSender, MStopTest
	{
public:
	static CActiveSenderImp* NewL(RSocketServ& aSocketServer,
			                      RFs& aFs,
			                      const TDesC& aTelephoneNumber,
                                  const TDesC& aServiceCenterNumber);
	virtual void StartSendingL();
	virtual void StopIfKeyPressedL();
	~CActiveSenderImp();
public:
	void StopTests();
private:
	void InitRSocketL();
	void SendSmsMessageL();
private:
	virtual void DoCancel();
	virtual void RunL();
private:
	CActiveSenderImp(RSocketServ& aSocketServer, RFs& aFs);
	CStopIfKeyPressed* iStopTest;
	RSocket iSocket;
	CSmsMessage* iSmsMessage;
	TPckgBuf<TUint> iIntBuf;
	TInt iCount;
	};

//
//
// CActiveReceiverImp
//
//

class CActiveReceiverImp : public CActiveReceiver, MStopTest
	{
public:
	static CActiveReceiverImp* NewL(RSocketServ& aSocketServer, RFs& aFs);
	virtual void AddTestL(const TTestCase& aTest);
	virtual void StartTestsL();
	virtual void StopIfKeyPressedL();
	~CActiveReceiverImp();
public:
	void StopTests();
private:
	virtual void DoCancel();
	virtual void RunL();
private:
	CActiveReceiverImp(RSocketServ& aSocketServer, RFs& aFs);
	CArrayPtr<CActiveSocket>* iSockArray;
	TInt iResultCount;
	CStopIfKeyPressed* iStopTest;
	};

//
//
// CStopIfKeyPressed
//
//

class CStopIfKeyPressed : public CActive
	{
public:
	enum { EPriority = KStopIfKeyPressedPriority };
public:
	static CStopIfKeyPressed* NewL(MStopTest* aReceiver);
	virtual void RunL();
	virtual void DoCancel();
public:
	void StopIfKeyPressed();
private:
	CStopIfKeyPressed();
	MStopTest* iReceiver;
	};

CStopIfKeyPressed* CStopIfKeyPressed::NewL(MStopTest* aReceiver)
//
// Factory
//
	{

	CStopIfKeyPressed* stopTest = new(ELeave) CStopIfKeyPressed;
	stopTest->iReceiver = aReceiver;
	CActiveScheduler::Add(stopTest);
	return stopTest;
	}

void CStopIfKeyPressed::StopIfKeyPressed()
//
// Activate
//
	{

	if (IsActive())
		return;
//	CConsoleBase* console = gTest.Console();
//	console->Read(iStatus);
//	SetActive();
	}

CStopIfKeyPressed::CStopIfKeyPressed()
//
// Constructor
//
: CActive(EPriority)
	{}

void CStopIfKeyPressed::RunL()
//
// Called if a key has been pressed
//
	{

	iReceiver->StopTests();
	CActiveScheduler::Stop();
	}

void CStopIfKeyPressed::DoCancel()
//
// Cancel has been called
//
	{

//	CConsoleBase* console = gTest.Console();
//	console->ReadCancel();
	}

//
//
// CActiveSocket
//
//

CActiveSocket* CActiveSocket::NewL(const TTestCase& aTest, RSocketServ& aSocketServer, RFs& aFs)
//
// Factory
//
	{

	CActiveSocket* socket = new(ELeave) CActiveSocket(aSocketServer);
	CleanupStack::PushL(socket);
	socket->iTestCase=aTest;
	socket->InitRSocketL();

	CSmsBuffer* buffer=NULL;
	buffer=CSmsBuffer::NewL();
	socket->iSmsMessage=CSmsMessage::NewL(aFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::Pop();

	CActiveScheduler::Add(socket);
	return socket;
	}

CActiveSocket::CActiveSocket(RSocketServ& aSocketServer)
//
// Constructor
//
:	CActive(EPriority),
	iSocketServer(aSocketServer)
	{}

CActiveSocket::~CActiveSocket()
//
// Destructor
//
	{

	delete iSmsMessage;
	iSocket.Close();
	}

void CActiveSocket::TestResultL()
//
// Test the sms message contains the expected data
//
	{

	CSmsBufferBase& smsBuffer=iSmsMessage->Buffer();
	TInt bufLen=smsBuffer.Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	TPtr textPtr(textBuf->Des());
	smsBuffer.Extract(textPtr,0,bufLen);

//	if (iTestCase.iTestForever)
//		gTest.Printf(_L("RECEIVED: Msg %S\n"),&textPtr);
//	else if (iTestCase.iMatchType==ESmsAddrStatusReport)
//		gTest.Printf(_L("SUCCESS: Status report received for %S\n"),&iTestCase.iMsg);
//	else
//		{
//		TBool ret = (textPtr==iTestCase.iMsg);
//		if (ret>0)
//			gTest.Printf(_L("SUCCESS: Msg %S received\n"),&iTestCase.iMsg);
//		else
//			gTest.Printf(_L("SUCCESS: Msg %S not received\n"),&iTestCase.iMsg);
//		}

	delete textBuf;
	}


void CActiveSocket::SetIndex(TInt aIndex)
//
// Set the index ofthe socket within the receiver's iSockArray
//
	{

	iActiveReceiverIndex = aIndex;
	}

TPtrC CActiveSocket::Message()
//
// Return the testcase message
//
	{

	return iTestCase.iMsg;
	}

void CActiveSocket::InitRSocketL()
//
// Initialise the active socket's connection with the socket server
//
	{

	TInt ret=iSocket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	User::LeaveIfError(ret);
	TSmsAddr smsAddr;
	smsAddr.SetSmsAddrFamily(iTestCase.iMatchType);

	switch(iTestCase.iMatchType)
		{
	case ESmsAddrMatchText:
		{
		TBuf8<128> match;
		match.Copy(iTestCase.iMatch);
		smsAddr.SetTextMatch(match);
		break;
		}
	case ESmsAddrMatchIEI:
		smsAddr.SetIdentifierMatch(iTestCase.iIdentifierMatch);
		break;
	case ESmsAddrMessageIndication:
		break;
	default:
		break;
		};

	ret=iSocket.Bind(smsAddr);
	User::LeaveIfError(ret);
	}

void CActiveSocket::ReceiveMessage(TRequestStatus* aStatus)
//
// Receive a message
//
	{

	iActiveReceiverStatus = aStatus;
	iSelectBuf()=KSockSelectRead;
	iSocket.Ioctl(KIOctlSelect,iStatus,&iSelectBuf,KSOLSocket);
	SetActive();
	}

void CActiveSocket::DoCancel()
//
// Cancel the select
//
	{

	iSocket.CancelAll();
	}

void CActiveSocket::RunL()
//
// Got something to read
//
	{

	if (iStatus<0)
		{
		User::RequestComplete(iActiveReceiverStatus,iStatus.Int());
		return;
		}

	RSmsSocketReadStream readstream(iSocket);
	TRAPD(ret,readstream >> *iSmsMessage);
	User::LeaveIfError(ret);

	TPckgBuf<TUint> sbuf;
	TRequestStatus status;
	iSocket.Ioctl(KIoctlReadMessageSucceeded,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);

	User::RequestComplete(iActiveReceiverStatus,iActiveReceiverIndex);
	}

TBool CActiveSocket::TestComplete()
//
// Socket has finished performing all tests
//
	{

	if (iTestCase.iTestForever)
		return EFalse;
	return ETrue;
	}

//
//
// CActiveReceiverImp
//
//

CActiveReceiverImp* CActiveReceiverImp::NewL(RSocketServ& aSocketServer, RFs& aFs)
//
// Factory
//
	{

	CActiveReceiverImp* actRecv = new(ELeave) CActiveReceiverImp(aSocketServer, aFs);
	CleanupStack::PushL(actRecv);
	actRecv->iSockArray=new(ELeave) CArrayPtrSeg<CActiveSocket>(KActiveReceiverSocketGranularity);
	actRecv->iStatus=KRequestPending;
	CleanupStack::Pop();
	CActiveScheduler::Add(actRecv);
	return actRecv;
	}

CActiveReceiverImp::CActiveReceiverImp(RSocketServ& aSocketServer, RFs& aFs)
//
// Constructor
//
:CActiveReceiver(aSocketServer, aFs)
	{}

CActiveReceiverImp::~CActiveReceiverImp()
//
// Destructor
//
	{

	if (iSockArray)
		{
		StopTests();
		iSockArray->ResetAndDestroy();
		}
	delete iStopTest;
	delete iSockArray;
	}

void CActiveReceiverImp::AddTestL(const TTestCase& aTest)
//
// Add a test case to the receiver
//
	{

	CActiveSocket* socket = CActiveSocket::NewL(aTest, iSocketServer, iFs);
	CleanupStack::PushL(socket);
	iSockArray->AppendL(socket);
	socket->SetIndex(iSockArray->Count()-1);
	socket->ReceiveMessage(&iStatus);
	CleanupStack::Pop();

	if (aTest.iMatchType==ESmsAddrStatusReport)
		{
		// Add a test to match the original message as well as the status report
		TTestCase srrIdMatch(aTest.iMsg,aTest.iMsg.Left(TSmsAddr::EMaxTextMatchLength));
		AddTestL(srrIdMatch);
		}
	}

void CActiveReceiverImp::StartTestsL()
//
// Kick off the tests
//
	{

	SetActive();
	CActiveScheduler::Start();
	}

void CActiveReceiverImp::StopTests()
//
// Cancel all active sockets
//
	{

	for (TInt i=0;i<iSockArray->Count();i++)
		(*iSockArray)[i]->Cancel();
	if (iStopTest)
		iStopTest->Cancel();
	Cancel();
	}

void CActiveReceiverImp::StopIfKeyPressedL()
//
// Stop the active scheduler if a key is pressed
//
	{

	if (!iStopTest)
		iStopTest=CStopIfKeyPressed::NewL(this);
	iStopTest->StopIfKeyPressed();
	}

void CActiveReceiverImp::DoCancel()
	{

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrCancel);
	}

void CActiveReceiverImp::RunL()
//
// An active socket has received a message
//
	{

	TInt index = iStatus.Int();
	CActiveSocket* socket = (*iSockArray)[index];
	socket->TestResultL();

	if (socket->TestComplete())
		iResultCount++;
	else
		socket->ReceiveMessage(&iStatus);

	if (iResultCount == iSockArray->Count())
		{
		StopTests();
		CActiveScheduler::Stop();
		return;
		}

	SetActive();
	iStatus=KRequestPending;
	}

//
//
// TTestCase
//
//

TTestCase::TTestCase()
//
// Constructor
//
	{

	CommonInit();
	}

TTestCase::TTestCase(const TDesC& aMsg, const TDesC& aMatch)
//
// Constructor
//
: iMatchType(ESmsAddrMatchText), iMsg(aMsg), iMatch(aMatch)
	{

	CommonInit();
	}

void TTestCase::CommonInit()
//
// Reset member variables
//
	{

	iTestSmsClass = EFalse;
	iTestValidityPeriod = EFalse;
	iTestForever=EFalse;
	iTestIndicators=EFalse;
	}

TTestCase::TTestCase(const TDesC& aMsg, CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier)
//
// Constructor
//
: iMatchType(ESmsAddrMatchIEI), iMsg(aMsg), iIdentifierMatch(aIdentifier)
	{

	CommonInit();
	}

TTestCase::TTestCase(const TDesC& aMsg, TSmsFirstOctet::TSmsStatusReportRequest /*aSRR*/)
//
// Match on status report - ignore actual value of aSRR
//
: iMatchType(ESmsAddrStatusReport), iMsg(aMsg)
	{

	CommonInit();
	}

TTestCase::TTestCase(const TDesC& aMsg, TSmsDataCodingScheme::TSmsIndicationType aIndicationType,
					 TSmsDataCodingScheme::TSmsDCSBits7To4 aBits7To4,
					 TSmsDataCodingScheme::TSmsIndicationState aIndicationState)
//
// Match on a type of message indication
//
: iMatchType(ESmsAddrMessageIndication), iMsg(aMsg), iIndicationType(aIndicationType),
	iBits7To4(aBits7To4), iIndicationState(aIndicationState)
	{

	__ASSERT_ALWAYS(iBits7To4==TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationDiscardMessage
		|| iBits7To4==TSmsDataCodingScheme::ESmsDCSMessageWaitingIndication7Bit
		|| iBits7To4==TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationUCS2,
		User::Panic(_L("TestCase"), KErrNotSupported));

	CommonInit();
	iTestIndicators=ETrue;

	}

TTestCase::TTestCase(const TDesC& aMsg)
//
// Constructor
//
: iMatchType(ESmsAddrRecvAny), iMsg(aMsg)
	{

	CommonInit();
	}

const TTestCase& TTestCase::operator=(const TTestCase& aTest)
//
// Assignment operator
//
	{

	iMatchType = aTest.iMatchType;
	iMsg.Set(aTest.iMsg);
	iMatch.Set(aTest.iMatch);
	iIdentifierMatch = aTest.iIdentifierMatch;
	iTestForever = aTest.iTestForever;
	return *this;
	}

void TTestCase::SetSmsClass(TSmsDataCodingScheme::TSmsClass aClass)
//
// Set Sms Class
//
	{

	iTestSmsClass = ETrue;
	iSmsClass = aClass;
	}

void TTestCase::SetValidityPeriod(const TTimeIntervalMinutes& aTimeInterval)
//
// Set the validity period
//
	{

	iTestValidityPeriod = ETrue;
	iValidityPeriod = aTimeInterval;
	}

void TTestCase::SetRecvForever()
//
// Recv all messages forever
//
	{

	iTestForever = ETrue;
	iMatchType = ESmsAddrRecvAny;
	}


//
//
// CActiveReceiver
//
//

CActiveReceiver* CActiveReceiver::NewL(RSocketServ& aSocketServer, RFs& aFs)
	{ return CActiveReceiverImp::NewL(aSocketServer, aFs); }
CActiveReceiver::CActiveReceiver(RSocketServ& aSocketServer, RFs& aFs)
:	CActive(EPriority),
	iSocketServer(aSocketServer),
	iFs(aFs)
	{}
void CActiveReceiver::AddTestL(const TTestCase& /*aTest*/)
	{}
void CActiveReceiver::StartTestsL()
	{}
void CActiveReceiver::StopIfKeyPressedL()
	{}

//
//
// CActiveSender
//
//

CActiveSender* CActiveSender::NewL(RSocketServ& aSocketServer,
                                   RFs& aFs,
                                   const TDesC& aTelephoneNumber,
                                   const TDesC& aServiceCenterNumber)
	{ return CActiveSenderImp::NewL(aSocketServer, aFs, aTelephoneNumber, aServiceCenterNumber); }
CActiveSender::CActiveSender(RSocketServ& aSocketServer, RFs& aFs)
:	CActive(EPriority),
	iSocketServer(aSocketServer),
	iFs(aFs)
	{}
void CActiveSender::StartSendingL()
	{}
void CActiveSender::StopIfKeyPressedL()
	{}


//
//
// CActiveSenderImp
//
//

CActiveSenderImp* CActiveSenderImp::NewL(RSocketServ& aSocketServer,
										 RFs& aFs,
										 const TDesC& aTelephoneNumber,
										 const TDesC& aServiceCenterNumber)
//
// Factory
//
	{

	CActiveSenderImp* actSend = new(ELeave) CActiveSenderImp(aSocketServer, aFs);
	CleanupStack::PushL(actSend);
	actSend->iStatus=KRequestPending;
	actSend->InitRSocketL();

	CSmsBuffer* buffer=NULL;
	buffer=CSmsBuffer::NewL();
	actSend->iSmsMessage=CSmsMessage::NewL(aFs, CSmsPDU::ESmsSubmit,buffer);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
	smsSettings.SetTextCompressed(EFalse);
	actSend->iSmsMessage->SetUserDataSettingsL(smsSettings);

	actSend->iSmsMessage->SetToFromAddressL(aTelephoneNumber);
	actSend->iSmsMessage->SmsPDU().SetServiceCenterAddressL(aServiceCenterNumber);
	buffer->InsertL(0,_L("Test Message"));

	CleanupStack::Pop(); // actSend
	CActiveScheduler::Add(actSend);
	return actSend;
	}

CActiveSenderImp::CActiveSenderImp(RSocketServ& aSocketServer, RFs& aFs)
//
// Constructor
//
:CActiveSender(aSocketServer, aFs)
	{}

CActiveSenderImp::~CActiveSenderImp()
//
// Destructor
//
	{

	Cancel();
	delete iStopTest;
	delete iSmsMessage;
	iSocket.Close();
	}

void CActiveSenderImp::StartSendingL()
//
// Kick off the tests
//
	{

	SendSmsMessageL();
	CActiveScheduler::Start();
	}

void CActiveSenderImp::StopTests()
//
// Prepare for active scheduler stop
//
	{

	Cancel();
	}

void CActiveSenderImp::StopIfKeyPressedL()
//
// Stop the active scheduler if a key is pressed
//
	{

	if (!iStopTest)
		iStopTest=CStopIfKeyPressed::NewL(this);
	iStopTest->StopIfKeyPressed();
	}

void CActiveSenderImp::DoCancel()
//
// Cancel the current send
//
	{

	iSocket.CancelSend();
	}

void CActiveSenderImp::SendSmsMessageL()
//
// Send an SMS message and wait for complete
//
	{

	RSmsSocketWriteStream writestream(iSocket);
    iSmsMessage->SmsPDU().SetClass(ETrue, TSmsDataCodingScheme::ESmsClass2);
	TRAPD(ret,writestream << *iSmsMessage);
	User::LeaveIfError(ret);
	TRAP(ret,writestream.CommitL());
	User::LeaveIfError(ret);

	iSocket.Ioctl(KIoctlSendSmsMessage,iStatus,&iIntBuf, KSolSmsProv);
	SetActive();
	}

void CActiveSenderImp::RunL()
//
// An active socket has received a message
//
	{

	if (iStatus.Int()!=KErrNone)
		{
		//gTest.Printf(_L("Error: Send completed with %d\n"),iStatus.Int());
		iStopTest->DoCancel();
		return;
		}
	iCount++;
	//gTest.Printf(_L("Sent %d messages\n"),iCount);
	SendSmsMessageL();
	}

void CActiveSenderImp::InitRSocketL()
//
// Initialise the sender's socket
//
	{

	TInt ret=iSocket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	User::LeaveIfError(ret);
	TSmsAddr smsAddr;
	smsAddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	ret=iSocket.Bind(smsAddr);
	User::LeaveIfError(ret);
	}
