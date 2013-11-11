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
//

#include<c32root.h>
#include <e32base.h>
#include <es_sock.h>
#include <ss_std.h>
#include <e32std.h>
#include <test/es_dummy.h>

#include "TE_TestStepSocketServer.h"
#include "ES_DUMMY.H"

//_LIT(KProtocolName, "Dummy Protocol 1");          // Name of test protocol to use in these tests
//_LIT8(KSendStringTest, "Bad descriptor"); // Sample testdata to send in these tests
_LIT(KOptimalDealerName, "!SocketServer-1*"); //The name of the optimal dealer

//A class needed to get to the protected method CreateSession;
class RIllegalConnect : public RSessionBase
	{
	public:
		TInt Do(const RSocketServ& aSS); //Does the illegal Connect
	};

TInt RIllegalConnect::Do(const RSocketServ& aSS)
	{
	TFindServer fs(KOptimalDealerName);
	TFullName fn;
	TFullName newName;
	
	while (fs.Next(newName) == KErrNone)
		{
		fn = newName;
		}
	
	return CreateSession(fn, aSS.Version(), KESockDefaultMessageSlots);
	}

// constructor
CTestStepSocketServer::CTestStepSocketServer() : iErrText()
	{
	}

// destructor
CTestStepSocketServer::~CTestStepSocketServer()
	{
	}

enum TVerdict CTestStepSocketServer::doTestStepPreambleL( void )
	{
/*
	TInt ret;
	ret = RProperty::Define(KDummyUid, KDummyTerminationProperty,0);
	if (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Error creating property"));
		return EFail;
		}
	
	ret = iHangingModuleFlag.Attach(KDummyUid, KDummyTerminationProperty);
	if (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Error attaching to property"));
		return EFail;
		}
	ret = iHangingModuleFlag.Set(0);
	if (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Error setting property to 0"));
		return EFail;
		}
*/
	return EPass;
	}

enum TVerdict CTestStepSocketServer::doTestStepL()
	{
	
	TRAPD(trapRet,enum TVerdict stepRet = InternalDoTestStepL();\
		if (stepRet !=EPass)\
			{\
			SetTestStepResult(stepRet);\
			}\
			)
			
			if (trapRet != KErrNone)
				{
				SetTestStepResult(EFail);
				return EFail;
				}
			return EPass;
	};

//This method ensures that any
enum TVerdict CTestStepSocketServer::doTestStepPostambleL( void )
	{
/*
	TInt ret;
	TInt hangingModules;
	iHangingModuleFlag.Get(hangingModules);
	if (hangingModules > 0)
		{
		//The code here is specifically designed to work on the function "void CDummyProvd::HangModule(void)"
		//It wakes up any modules that have been hung during the course of testing and works in the following
		//fashion:
		//Each module when hung gets the value of the hanging modules property 
		//The module increments the property by 1
		//The module waits until the module retunrs to the previous value
		//The module then decrements the decrements the hanging module count
		//CDummyProvd::HangModule(void) returns
		//So to start the shutdown process, decrement the hanging modules property by one and 
		//wait for the property to reach -1
		RProperty deathCountProp;
		TInt requiredDeathCount;
		TInt deathCount = 0;
		TRequestStatus deathCountStat;
		TRequestStatus hangingModulesStat;
#ifdef SYMBIAN_NETWORKING_PLATSEC
		ret = deathCountProp.Attach(KUidSystemCategory, KUidC32RootModuleDeathKey.iUid);
#else
		ret = deathCountProp.Attach(KUidCommsProcess, KC32RootModuleDeathKey);
#endif
		if (ret != KErrNone)
			{
			Logger().WriteFormat(_L("Attach returned with %d"),ret);
			}
		ret =deathCountProp.Get(requiredDeathCount);
		if (ret != KErrNone)
			{
			Logger().WriteFormat(_L("Get returned with %d"),ret);
			}
		requiredDeathCount += hangingModules;
		
		iHangingModuleFlag.Subscribe(hangingModulesStat);
		
		--hangingModules;
		Logger().WriteFormat(_L("Starting to unwind the hung modules"));
		iHangingModuleFlag.Set(hangingModules); //Do decrement to begin process
		iHangingModuleFlag.Get(hangingModules);
		
		Logger().WriteFormat(_L("Waiting for all the modules to be unwound"));
		while(hangingModules > -1)
			{
			User::WaitForRequest(hangingModulesStat);
			iHangingModuleFlag.Subscribe(hangingModulesStat);
			iHangingModuleFlag.Get(hangingModules);
			}
		iHangingModuleFlag.Cancel();
		Logger().WriteFormat(_L("All modulles now unwound - Waiting for thread to terminate"));
		
		//Now just make sure all the "stuck" prts have exited
		
		deathCountProp.Subscribe(deathCountStat);	
		deathCountProp.Get(deathCount);
		while (deathCount < requiredDeathCount)
			{
			User::WaitForRequest(deathCountStat);
			deathCountProp.Subscribe(deathCountStat);
			deathCountProp.Get(deathCount);
			}
		
		deathCountProp.Cancel();
		deathCountProp.Close();
		Logger().WriteFormat(_L("Thread termination completed"));
		}
	iHangingModuleFlag.Close();	
*/
	return EPass;

	}


// extra logging methods
const TPtrC& CTestStepSocketServer::EpocErrorToText(const TInt aErrCode)
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
		case KRequestPending:
			iErrText.Set(_L("KRequestPending"));
			break;
		default:
			iErrBuff.Format(_L(" %d"),aErrCode);
			iErrText.Set(iErrBuff.Ptr());
			break;
		}
		return iErrText;
	}	

TInt CTestStepSocketServer::OptimalConnect(RSocketServ &aSrv)
	{
    TSessionPref pref;
    pref.iAddrFamily = KDummyAddrFamily;
    pref.iProtocol = KDummyThree;
	return aSrv.Connect(pref);
	}

TInt CTestStepSocketServer::IllegalOptimalConnect(RSocketServ &aSrv)
	{
	RIllegalConnect ill;
	return ill.Do(aSrv); 
	}


TInt CTestStepSocketServer::PanicProtocolModule(RSocket &aSock)
	{
	TRequestStatus st;
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse); //Don't stop for panic
	aSock.Ioctl(KDummyIoctlPanicProtocolModule,st);
	User::WaitForRequest(st);
	User::SetJustInTime(jit);
	return st.Int();
	}


void CTestStepSocketServer::NonCompletingIoclt(RSocket &aSock,TRequestStatus &st)
	{
	aSock.Ioctl(KDummyIocltNonCompleting,st);
	}


TInt CTestStepSocketServer::PanicDealer(RSocketServ &aSockServ)
	{
	/*This is a really crude way to cause the dealer to panic.
	It works by marking the heap. Deliberately adding stuff to the heap
	And then checking to see that nothing was allocated*/
	TInt ret = aSockServ.__DbgMarkHeap();
	if (ret != KErrNone)
		{
		return ret;
		}
	RSocket sock;
	sock.Open(aSockServ);
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse); //Don't stop for panic
	ret = aSockServ.__DbgCheckHeap(0); //Heap should now be dead
	User::SetJustInTime(jit);
	return ret;
	
	}

TInt CTestStepSocketServer::BlockHardOnClose(RSocket &aSock)
	{
	TRequestStatus st;
	aSock.Ioctl(KDummyIoctBlockHardOnClose,st);
	User::WaitForRequest(st);
	return st.Int();
	}

void CTestStepSocketServer::SlowIoctlReturn(RSocket &aSock, TRequestStatus &aStat,
											TBool aBlockPermanently)
	{
	aBlockPermanently ? aSock.Ioctl(KDummyIoctlHangIoctlReturn,aStat) : 
aSock.Ioctl(KDummyIoctlSlowIoctlReturn,aStat);
	}

TInt CTestStepSocketServer::CheckForModuleL(const TDesC8 &aModuleName)
	{
	RRootServ rsrv;
	TRSIter it;
	TCFModuleName name;
	int ret;
	
	ret = rsrv.Connect();
	if (ret != KErrNone)
		{
		return ret;
		}
	
	CleanupClosePushL(rsrv);
	
	TBool tst = false;
	while (ret == KErrNone && !tst)
		{
		ret = rsrv.EnumerateModules(it,name);
		if (ret != KErrNone && ret != KErrEof)
			{
			CleanupStack::PopAndDestroy();
			return ret;
			}
		tst = (name == aModuleName);
		}
		
	CleanupStack::PopAndDestroy(); //rsrv
	
	if (tst)
		{
		Logger().WriteFormat(_L8("Module was found in module list"));
		}
	else
		{
		Logger().WriteFormat(_L8("Module does not exist in module list"));
		}	
	TESTL(!tst);
		
	
	return ret;
	}

void CTestStepSocketServer::OpenMultipleSocketsL(RSocketServ &aSS,RSocket *aSockets,TInt aNumSockets)
	{
	int ret;
	int i;
	for (i = 0; i < aNumSockets; i++)
		{
		Logger().WriteFormat(_L("Attempting to open socket %d"),i);
		ret = aSockets[i].Open(aSS,_L("Dummy Protocol 1"));
		Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
		TESTL(ret == KErrNone);
		}
	}

TInt CTestStepSocketServer::OpenSocketProtocolStarted()
	{
	TRequestStatus protStatus;
	TRequestStatus exclStatus;
	TInt ret;
	RSocketServ ss;
    
	TInt err = ss.Connect();
	if(err != KErrNone)
		{
		return err;
		}			         
	ss.StartProtocol( KAfInet,KSockDatagram,KProtocolInetUdp, protStatus );
	User::WaitForRequest(protStatus);
	if(protStatus.Int() != KErrNone)
		{
		return protStatus.Int();
		}
	ss.SetExclusiveMode(exclStatus);
	User::WaitForRequest(exclStatus);
	if(exclStatus.Int() != KErrNone)
		{
		return exclStatus.Int();
		}
	RSocket socketArray[2];
	ret = socketArray[0].Open(ss);
	if(ret != KErrNone)
		{
		return ret;
		}
	ret = socketArray[1].Open(ss);
	if(ret != KErrNone)
		{
		return ret;
		}
	ss.ClearExclusiveMode();
	ss.StopProtocol( KAfInet,KProtocolInetTcp, KSockStream, protStatus );
	return ret;
	}

