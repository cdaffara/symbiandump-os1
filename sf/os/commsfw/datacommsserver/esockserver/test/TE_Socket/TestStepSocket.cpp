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
// Contains definition of CTestStepSocket which is the base class 
// for all the Socket Test Step classes (p.s. very dummy class) 
// 
//

// EPOC includes
#include <e32base.h>
#include <es_sock.h>
#include <test/es_dummy.h>

// Test system includes
#include "TestStepSocket.h"


// static consts
const TUint CTestStepSocket::KNumTestSockets = 10;
const TInt CTestStepSocket::KNumExhaustiveSockets = 100;
const TInt CTestStepSocket::KNumStretchOpens = 16;

_LIT(KProtocolName, "Dummy Protocol 1");          // Name of test protocol to use in these tests
_LIT8(KSendStringTest, "Bad descriptor"); // Sample testdata to send in these tests

const TInt segmentlengthtest = 1;
const TInt txlengthtest = 42; // Length of KSEndStringTest Its useful to be const

// constructor
CTestStepSocket::CTestStepSocket() : iErrText()
	{
	}

// destructor
CTestStepSocket::~CTestStepSocket()
	{
	}


enum TVerdict CTestStepSocket::doTestStepL()
	{
	
	TRAPD(trapRet,
		Logger().ShareAuto();
	enum TVerdict stepRet = InternalDoTestStepL();
	if (stepRet !=EPass)
		{
		SetTestStepResult(stepRet);
		}
		)
		
		if (trapRet != KErrNone)
			{
			SetTestStepResult(EFail);
			return EFail;
			}
		return EPass;
	};


// extra logging methods
const TPtrC& CTestStepSocket::EpocErrorToText(const TInt aErrCode)
/**
Convert a Error code to text.

  @param aError error code to display as text.
  @return Text describing the error.
  */
	{
	//	TPtr errText;
	switch (aErrCode)
		{
		case KErrNone:
			iErrText.Set(_L("KErrNone"));
			break;
		case KErrNotFound:
			iErrText.Set(_L("KErrNotFound"));
			break;
		case KErrGeneral:
			iErrText.Set(_L("KErrGeneral"));
			break;
		case KErrCancel:
			iErrText.Set(_L("KErrCancel"));
			break;
		case KErrNoMemory:
			iErrText.Set(_L("KErrNoMemory"));
			break;
		case KErrNotSupported:
			iErrText.Set(_L("KErrNotSupported"));
			break;
		case KErrArgument:
			iErrText.Set(_L("KErrArgument"));
			break;
		case KErrTotalLossOfPrecision:
			iErrText.Set(_L("KErrTotalLossOfPrecision"));
			break;
		case KErrBadHandle:
			iErrText.Set(_L("KErrBadHandle"));
			break;
		case KErrOverflow:
			iErrText.Set(_L("KErrOverflow"));
			break;
		case KErrUnderflow:
			iErrText.Set(_L("KErrUnderflow"));
			break;
		case KErrAlreadyExists:
			iErrText.Set(_L("KErrAlreadyExists"));
			break;
		case KErrPathNotFound:
			iErrText.Set(_L("KErrPathNotFound"));
			break;
		case KErrDied:
			iErrText.Set(_L("KErrDied"));
			break;
		case KErrInUse:
			iErrText.Set(_L("KErrInUse"));
			break;
		case KErrServerTerminated:
			iErrText.Set(_L("KErrServerTerminated"));
			break;
		case KErrServerBusy:
			iErrText.Set(_L("KErrServerBusy"));
			break;
		case KErrCompletion:
			iErrText.Set(_L("KErrCompletion"));
			break;
		case KErrNotReady:
			iErrText.Set(_L("KErrNotReady"));
			break;
		case KErrUnknown:
			iErrText.Set(_L("KErrUnknown"));
			break;
		case KErrCorrupt:
			iErrText.Set(_L("KErrCorrupt"));
			break;
		case KErrAccessDenied:
			iErrText.Set(_L("KErrAccessDenied"));
			break;
		case KErrLocked:
			iErrText.Set(_L("KErrLocked"));
			break;
		case KErrWrite:
			iErrText.Set(_L("KErrWrite"));
			break;
		case KErrDisMounted:
			iErrText.Set(_L("KErrDisMounted"));
			break;
		case KErrEof:
			iErrText.Set(_L("KErrEof"));
			break;
		case KErrDiskFull:
			iErrText.Set(_L("KErrDiskFull"));
			break;
		case KErrBadDriver:
			iErrText.Set(_L("KErrBadDriver"));
			break;
		case KErrBadName:
			iErrText.Set(_L("KErrBadName"));
			break;
		case KErrCommsLineFail:
			iErrText.Set(_L("KErrCommsLineFail"));
			break;
		case KErrCommsFrame:
			iErrText.Set(_L("KErrCommsFrame"));
			break;
		case KErrCommsOverrun:
			iErrText.Set(_L("KErrCommsOverrun"));
			break;
		case KErrCommsParity:
			iErrText.Set(_L("KErrCommsParity"));
			break;
		case KErrTimedOut:
			iErrText.Set(_L("KErrTimedOut"));
			break;
		case KErrCouldNotConnect:
			iErrText.Set(_L("KErrCouldNotConnect"));
			break;
		case KErrCouldNotDisconnect:
			iErrText.Set(_L("KErrCouldNotDisconnect"));
			break;
		case KErrDisconnected:
			iErrText.Set(_L("KErrDisconnected"));
			break;
		case KErrBadLibraryEntryPoint:
			iErrText.Set(_L("KErrBadLibraryEntryPoint"));
			break;
		case KErrBadDescriptor:
			iErrText.Set(_L("KErrBadDescriptor"));
			break;
		case KErrAbort:
			iErrText.Set(_L("KErrAbort"));
			break;
		case KErrTooBig:
			iErrText.Set(_L("KErrTooBig"));
			break;
		case KErrCannotFindProtocol:
			iErrText.Set(_L("KErrCannotFindProtocol"));
			break;
		case KErrPermissionDenied:
			iErrText.Set(_L("KErrPermissionDenied"));
			break;	
		default:
			iErrBuf.Format(_L(" %d"),aErrCode);
			iErrText.Set(iErrBuf.Ptr());
			break;
		}
		return iErrText;
	}	
	
// static threads

void CTestStepSocket::DoCreateSubsessionsL(CTestStepSocket *aTestStep, TInt aNumSubsess)
	{
	aTestStep->Logger().WriteFormat(_L("SocketThread : eSock client test sub thread - creating sockets and resolvers"));
	
	aTestStep->Logger().WriteFormat(_L("Attempting to connect to socket server"));
	RSocketServ ss;
	TInt ret = ss.Connect();
	if (KErrNone != ret)
		{
		aTestStep->Logger().WriteFormat(_L("SocketThread : Connect returned %d"), ret);
		User::Leave(ret);
		}
	
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	if (KErrNone != ret)
		{
		aTestStep->Logger().WriteFormat(_L("SocketThread : FindProtocol returned %d"), ret);
		User::Leave(ret);
		}
	
	aTestStep->Logger().WriteFormat(_L("SocketThread : Making %d sockets, host resolvers, service resolvers, net databases"), aNumSubsess);
	
	RSocket* sock = new(ELeave) RSocket[aNumSubsess];
	RHostResolver* hostR = new(ELeave) RHostResolver[aNumSubsess];
	RServiceResolver* servR = new(ELeave) RServiceResolver[aNumSubsess];
	RNetDatabase* netDb = new(ELeave) RNetDatabase[aNumSubsess];
	
	for (TInt i=0;i<aNumSubsess;i++)
		{
		ret = sock[i].Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
		if (KErrNone != ret)
			{
			aTestStep->Logger().WriteFormat(_L("SocketThread : Socket Open returned %d"), ret);
			User::Leave(ret);
			}
		ret = hostR[i].Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
		if (KErrNone != ret)
			{
			aTestStep->Logger().WriteFormat(_L("SocketThread : Host Resolver Open returned %d"), ret);
			User::Leave(ret);
			}
		ret = servR[i].Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
		if (KErrNone != ret)
			{
			aTestStep->Logger().WriteFormat(_L("SocketThread : Service Resolver Open returned %d"), ret);
			User::Leave(ret);
			}
		
		ret = netDb[i].Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
		if (KErrNone != ret)
			{
			aTestStep->Logger().WriteFormat(_L("SocketThread : Net Database Open returned %d"), ret);
			User::Leave(ret);
			}
		}
	}

TInt CTestStepSocket::SocketThread(TAny * anArg)
	{
	
	TSocketThreadArg* tArg = static_cast<TSocketThreadArg*>(anArg); 
	CTestStepSocket *h = tArg->iHandle;
	
	TInt trapRet;
	CTrapCleanup *cleanup = CTrapCleanup::New(); //Creates and then installs the cleanup stack
	if (cleanup)
		{
		TRAP(trapRet, DoCreateSubsessionsL(h, tArg->iNumSockets));
		}
	else
		{
		trapRet = KErrNoMemory;
		}

	delete cleanup;
	RSemaphore *s = tArg->iSem;
	s->Signal();
	User::WaitForAnyRequest();

	return trapRet;
	}

	
	
TInt CTestStepSocket::ConnectThread(TAny * anArg)
	{
	TSocketThreadArg* tArg = static_cast<TSocketThreadArg*>(anArg); 
	CTestStepSocket *h = tArg->iHandle;
	
	CTrapCleanup *cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		tArg->iSem->Signal();
		return KErrNoMemory;
		}
	
	TInt trapRet;
	TRAP(trapRet,
		h->Logger().WriteFormat(_L("ConnectThread : eSock client test sub thread - connect and disconnect"));
	
	h->Logger().WriteFormat(_L("Attempting to connect to socket server"));
	RSocketServ ss;
	TInt ret = ss.Connect();
	if (KErrNone != ret)
		{
		h->Logger().WriteFormat(_L("ConnectThread : Connect returned %d"), ret);
		delete cleanup;
		tArg->iSem->Signal();
		return ret;
		}
	ss.Close();
	
	)
		
		delete cleanup;
	
	tArg->iSem->Signal();
	
	return trapRet;
	}


TInt CTestStepSocket::BadHandleThread(TAny* anArg)
	{
	TSocketThreadArg* tArg = static_cast<TSocketThreadArg*>(anArg); 
	CTestStepSocket *h = tArg->iHandle;
	
	CTrapCleanup *cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
	
	TInt trapRet;
	TRAP(trapRet,
		h->Logger().WriteFormat(_L("BadHandleThread : eSock client test sub thread - use bad handle"));
	RSocketServ ss;
	TInt ret = ss.Connect();
	if (KErrNone != ret)
		{
		h->Logger().WriteFormat(_L("SocketThread : Connect returned %d"), ret);
		delete cleanup;
		return ret;
		}
	
	// Give socket a bad handle
	RSocket socket;
	*(RSessionBase*)(&socket)=ss;
	*(((TInt*)&socket)+(sizeof(RSessionBase)/sizeof(TInt))) = 4;
	
	// attempt to use it - should panic
	TSockAddr addr;
	socket.LocalName(addr);
	
	)
		// Shouldn't get there (panic before ;-)
		delete cleanup; //really a waste of time because will not get here. Just here for neatness
	
	return trapRet;
	}

TInt CTestStepSocket::ReadTwiceThread(TAny* anArg)
	{
	TSocketThreadArg* tArg = static_cast<TSocketThreadArg*>(anArg); 
	CTestStepSocket *h = tArg->iHandle;
	
	CTrapCleanup *cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
	
	TInt trapRet;
	
	TRAP(trapRet,
		
		h->Logger().WriteFormat(_L("ReadTwiceThread : eSock client test sub thread - read twice"));
	
	h->Logger().WriteFormat(_L("Attempting to connect to socket server"));
	RSocketServ ss;
	TInt ret = ss.Connect();
	if (KErrNone != ret)
		{
		h->Logger().WriteFormat(_L("ReadTwiceThread : Connect returned %d"), ret);
		delete cleanup;
		return ret;
		}
	
	RSocket socket;
	h->Logger().WriteFormat(_L("ReadTwiceThread : Opening socket on dummy protocol 1"));
	ret = socket.Open(ss, _L("Dummy Protocol 1"));
	h->Logger().WriteFormat(_L("ReadTwiceThread : Open returned %d"), ret);
	if (KErrNone != ret)
		{
		delete cleanup;
		return ret;
		}
	
	h->Logger().WriteFormat(_L("ReadTwiceThread : Attempting Reads - should panic"));
	TRequestStatus stat(KRequestPending);
	TBuf8<100> buf;
	socket.Read(buf, stat);
	socket.Read(buf, stat);
	
	//Make sure ESock has time to PANIC us before we exit.
	// We expect ESock will PANIC us immediately upon receiving the
	// IPC message for the second read.
	User::After(5000000); 
	
	)
		// Shouldn't get there (panic before ;-)
		delete cleanup;
	return trapRet;
	}

TInt CTestStepSocket::BadDescriptorThread(TAny* anArg)
	{
	TSocketThreadArg* tArg = static_cast<TSocketThreadArg*>(anArg); 
	CTestStepSocket *h = tArg->iHandle;
	RSocketServ server;
	CTrapCleanup *cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
	
	TInt trapRet;
	TRAP(trapRet,
		
		h->Logger().WriteFormat(_L("Connecting to Socket server"));
	TInt ret = server.Connect();
	
	RSocket socket;
	h->Logger().WriteFormat(_L("Openning socket"));
	ret = socket.Open(server,KProtocolName);
	
	TRequestStatus status;
	TSockAddr addr;
	h->Logger().WriteFormat(_L("Connecting to socket"));
	socket.Connect(addr,status);
	User::WaitForRequest(status);
	
	h->Logger().WriteFormat(_L("Attempt is made to pass a negative descriptor."));
	h->Logger().WriteFormat(_L("Esock panics when a Negative Descriptor is passed"));
	
	TBuf8<txlengthtest> logstr;
	logstr.Copy(KSendStringTest);
	TInt* plength=(TInt*)&logstr;
	*plength=-42;
	
	// Send
	TSockXfrLength length;
	socket.Send(logstr, 0, status, length);
	User::WaitForRequest(status);
	
	// Receive setup
	TSockXfrLength rxlength;
	TBuf8<segmentlengthtest> rxbuf;
	
	//Receive 0 octets
	socket.Recv(rxbuf, 0, status, rxlength);
	User::WaitForRequest(status);
	
	)
		
		delete cleanup;
	
	return trapRet;
	
	}

TInt CTestStepSocket::OptimalConnect(RSocketServ& aSrv)
	{
    TSessionPref pref;
    pref.iAddrFamily = KDummyAddrFamily;
    pref.iProtocol = KDummyThree;
	return aSrv.Connect(pref);
	}

TBool CSocketTestStep_OOMCapable::CheckForHeapFailNext()
	{
	if(iOOMSess.Handle() != KNullHandle)
		{
		// If ESOCK heaps are still in FailNext mode then we try to flag the termination
		// of the loop to TEF by setting the local heap to fail. Because this might get
		// "eaten" inside TEF before it checks we walk a failure point here too
		return iOOMSess.__DbgCheckFailNext();
		}
	return ETrue;
	}

