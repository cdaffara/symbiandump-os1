// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DEF075471 buffer security test
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TBUFFERSECURITY.H"
#include "graphics/windowserverconstants.h"

//Set this #define in buffersecurity.h to add extra logging to this test case (useful when debugging a test fail)
//#define _TBUFS_LOGGING

//Set this #define in buffersecurity.h to initiate a long running soak test, this should be done periodically
//#define _TBUFS_TEST_SOAK_TEST



LOCAL_C TInt TestWsThreadFunc(TAny* aPtr)
	{
	TTestThreadData* data = (TTestThreadData*)aPtr;
	RWsSession ws;

	TInt err=ws.Connect();
	if (err!=KErrNone)
		return(err);
	ws.TestWrite(ws.WsHandle(), data->iOpCode, data->iData, data->iDataLength);
	ws.Flush();
	ws.Close();

	return(KErrNone);
	}

LOCAL_C TInt TestAnimDllThreadFunc(TInt aInt, TAny* /*aPtr*/)
	{
	TBool useTestWrite = (TBool)aInt;

	RWsSession ws;
	TInt err=ws.Connect();
	if (err != KErrNone)
		return (err);
	if (useTestWrite)
		{
		TUint32 data[2];
		data[0] = 200;
		data[1] = 4;
		ws.TestWrite(ws.WsHandle(), EWsClOpCreateAnimDll, REINTERPRET_CAST(TUint8*,data), 8);
		ws.Flush();
		}
	else
		{
		RAnimDll animDll(ws);
		const TPtrC	filename(REINTERPRET_CAST(TUint16*,4),200);
		animDll.Load(filename);
		animDll.Close();
		}
	ws.Close();

	return(KErrNone);
	}

TInt RTestIpcSession::Connect()
	{
	TVersion v(KWservMajorVersionNumber,KWservMinorVersionNumber,KWservBuildVersionNumber);
	TInt err = CreateSession(KWSERVServerName,v);
	if (err == KErrNone)
		{
		err=iWsHandle=SendReceive(EWservMessInit,TIpcArgs());
		}
	return err;
	}

TInt RTestIpcSession::SendBadBuffer()
	{
	TIpcArgs ipcArgs;
	TUint32 iData[2];
	iData[0] = 200;
	iData[1] = 4;
	ipcArgs.Set(KBufferMessageSlot,&iData);
	return SendReceive(EWservMessCommandBuffer,ipcArgs);
	}

LOCAL_C TInt TestIpcThreadFunc(TInt /*aInt*/, TAny* /*aPtr*/)
	{
	RTestIpcSession server;

	TInt handle=server.Connect();
	if (handle >= KErrNone)
		server.SendBadBuffer();

	return(KErrNone);
	}


CTBufferSecurity::CTBufferSecurity(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTBufferSecurity::~CTBufferSecurity()
	{
	}

void CTBufferSecurity::ConstructL()
	{
	RProperty securityTesting;
	TInt err=securityTesting.Attach(KUidWServSecurityTesting,EWServSecTestBufferSecurity);
	User::LeaveIfError(err);
	err=securityTesting.Define(KUidWServSecurityTesting,EWServSecTestBufferSecurity,RProperty::EInt,KAllowAllPolicy,KWriteDeviceDataMgmtPolicy);
	if (err!=KErrAlreadyExists)
    	User::LeaveIfError(err);
	TInt value = ETrue;
	err = securityTesting.Set(value);
	User::LeaveIfError(err);
	}

void CTBufferSecurity::TestWsBufferL(TInt aOpCode, TUint aDataFill, TBool aEightBit)
	{
	// create a new thread in which to test each of the TWsClientOpCode values
	// so if the thread is panicked the test will not fail.
	RThread testThread;
	TTestThreadData data;
	TRequestStatus status;
	TUint8 storeData[KTestDataMax];
	TTestDataStore store;
	store.any=storeData;

	//collect initial values
	for (TInt count = 0; count < KTestDataMax; count++)
		storeData[count] = 0;
	switch (aOpCode)
		{
		case EWsClOpHeapSetFail:
			store.heapSetFail->type = RAllocator::ENone;
			break;
		case EWsClOpSetPointerCursorArea:
			store.cursorArea->area = TheClient->iWs.PointerCursorArea();
			break;
		case EWsClOpRawEvent:
			//skip switch off (would need a timer to turn it back on again)
			if (aDataFill == TRawEvent::ESwitchOff)
				aDataFill = TRawEvent::ENone;
			break;
#if defined(__WINS__)
		case EWsClOpSimulateXyInput:
			*store.xyInputType = EXYInputMouse;
			break;
#endif
		}

	data.iOpCode = aOpCode;
	if (aEightBit)
		{
		for (TInt count = 0; count < KTestDataMax; count++)
			data.iData[count] = (TUint8)aDataFill;
		}
	else
		{
		for (TInt count = 0; count < KTestDataMax32; count++)
			data.iData32[count] = aDataFill;
		}
	data.iDataLength = KTestDataMax;
	_LIT(KThreadNameFormat,"BufSecTestWsThread-%d-%d");
	HBufC* threadName = HBufC::NewLC(KThreadNameFormat().Size() + 32);
	TPtr threadNamePtr(threadName->Des());
	threadNamePtr.Format(KThreadNameFormat(), aOpCode, aDataFill);
	TInt err = testThread.Create(threadNamePtr, TestWsThreadFunc,KDefaultStackSize,KPanicThreadHeapSize,KPanicThreadHeapSize,(TAny*)&data,EOwnerThread);

	if (err != KErrNone)
		{
		User::After(100000);
		err = testThread.Create(threadNamePtr,TestWsThreadFunc,KDefaultStackSize,KPanicThreadHeapSize,KPanicThreadHeapSize,(TAny*)&data,EOwnerThread);
		}
	CleanupStack::PopAndDestroy(threadName);
	testThread.Logon(status);
	User::SetJustInTime(EFalse);
	testThread.Resume();
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);
#ifdef _TBUFS_LOGGING
	TLogMessageText logMessageText;
	TBufSStartLogText("TestWsBufferL");
	logMessageText.Format(_L(" OpCode(%d), ExitReason: %d"),aOpCode,testThread.ExitReason());
	TBufSLogFormat(logMessageText);
#endif

	testThread.Close();
	//reset some values to sensible ones
	switch (aOpCode)
		{
		case EWsClOpHeapSetFail:
		case EWsClOpSetPointerCursorArea:
#if defined(__WINS__)
		case EWsClOpSimulateXyInput:
#endif
			RThread resetThread;
			TheClient->iWs.TestWrite(TheClient->iWs.WsHandle(), data.iOpCode, storeData, data.iDataLength);
			TheClient->iWs.Flush();
			break;
		}
	}

void CTBufferSecurity::TestBadStringAnimDllL()
	{
	TEST(iTest->TestPanicL(&TestAnimDllThreadFunc,3,EFalse,NULL,KLitKernExec));
	}

void CTBufferSecurity::TestBadStringL()
	{
	TEST(iTest->TestWsPanicL(&TestAnimDllThreadFunc,EWservPanicBufferPtr,ETrue,NULL));
	}

void CTBufferSecurity::TestBadIpcL()
	{
	TEST(iTest->TestWsPanicL(&TestIpcThreadFunc,EWservPanicDescriptor,1));
	}

void CTBufferSecurity::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	TInt ii;
	((CTBufferSecurityStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
		case 1:
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0022"));
			_LIT(KBuffSecTestBadIpcL,"TestBadIpcL");
			iTest->LogSubTest(KBuffSecTestBadIpcL);
			TestBadIpcL();
			break;
		case 2:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0549
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0549"));
			_LIT(KBuffSecTestBadStringLRAnimDll,"TestBadStringL use RAnimDll");
			iTest->LogSubTest(KBuffSecTestBadStringLRAnimDll);
			TestBadStringAnimDllL();
			break;
		case 3:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0550
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0550"));
			_LIT(KBuffSecTestBadStringLTestWrite,"TestBadStringL use TestWrite");
			iTest->LogSubTest(KBuffSecTestBadStringLTestWrite);
			TestBadStringL();
			break;
#ifdef _TBUFS_TEST_SOAK_TEST
		case 4:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0551
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0551"));
			_LIT(KBuffSecTestWsBufferLSoak8bit,"TestWsBufferL Soak 8bit");
			iTest->LogSubTest(KBuffSecTestWsBufferLSoak8bit);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TUint8 soak8=0;//gets set as KMaxTUint8 on first iteration
				do
					{
					--soak8;
					TestWsBufferL(ii,soak8);
					}
				while (soak8>0);
				}
			break;
		case 5:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0552
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0552"));
			_LIT(KBuffSecTestWsBufferLSoak32bit,"TestWsBufferL Soak 32bit");
			iTest->LogSubTest(KBuffSecTestWsBufferLSoak32bit);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TUint32 soak32=0;//gets set as KMaxTUint8 on first iteration
				do
					{
					--soak32;
					TestWsBufferL(ii,soak32,EFalse);
					}
				while (soak32>0);
				}
			break;
#else
		case 4:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0553
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0553"));
			_LIT(KBuffSecTestWsBufferL0,"TestWsBufferL 0");
			iTest->LogSubTest(KBuffSecTestWsBufferL0);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,0);
				}
			break;
		case 5:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0554
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0554"));
			_LIT(KBuffSecTestWsBufferL5,"TestWsBufferL 5");
			iTest->LogSubTest(KBuffSecTestWsBufferL5);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,5);
				}
			break;
		case 6:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0555
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0555"));
			_LIT(KBuffSecTestWsBufferL32,"TestWsBufferL 32");
			iTest->LogSubTest(KBuffSecTestWsBufferL32);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,32);
				}
			break;
		case 7:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0556
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0556"));
			_LIT(KBuffSecTestWsBufferL64,"TestWsBufferL 64");
			iTest->LogSubTest(KBuffSecTestWsBufferL64);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,64);
				}
			break;
		case 8:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0557
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0557"));
			_LIT(KBuffSecTestWsBufferL128,"TestWsBufferL 128");
			iTest->LogSubTest(KBuffSecTestWsBufferL128);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,128);
				}
			break;
		case 9:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0558
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0558"));
			_LIT(KBuffSecTestWsBufferL255,"TestWsBufferL 255");
			iTest->LogSubTest(KBuffSecTestWsBufferL255);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,255);
				}
			break;
		case 10:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0559
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0559"));
			_LIT(KBuffSecTestWsBufferL325,"TestWsBufferL 32-5");
			iTest->LogSubTest(KBuffSecTestWsBufferL325);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,5,EFalse);
				}
			break;
		case 11:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0560
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0560"));
			_LIT(KBuffSecTestWsBufferL32128,"TestWsBufferL 32-128");
			iTest->LogSubTest(KBuffSecTestWsBufferL32128);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,128,EFalse);
				}
			break;
		case 12:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0561
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0561"));
			_LIT(KBuffSecTestWsBufferL32512,"TestWsBufferL 32-512");
			iTest->LogSubTest(KBuffSecTestWsBufferL32512);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,512,EFalse);
				}
			break;
		case 13:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0562
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0562"));
			_LIT(KBuffSecTestWsBufferL320x01234567,"TestWsBufferL 32-0x01234567");
			iTest->LogSubTest(KBuffSecTestWsBufferL320x01234567);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,0x01234567,EFalse);
				}
			break;
		case 14:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0563
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0563"));
			_LIT(KBuffSecTestWsBufferL320x89abcdef,"TestWsBufferL 32-0x89abcdef");
			iTest->LogSubTest(KBuffSecTestWsBufferL320x89abcdef);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,0x89abcdef,EFalse);
				}
			break;
		case 15:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0564
*/
			((CTBufferSecurityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0564"));
			_LIT(KBuffSecTestWsBufferL320xCCCCCCCC,"TestWsBufferL 32-0xCCCCCCCC");
			iTest->LogSubTest(KBuffSecTestWsBufferL320xCCCCCCCC);
			for(ii = EWsClOpDisconnect; ii < EWsClOpLastEnumValue; ii++)
				{
				TestWsBufferL(ii,0xCCCCCCCC,EFalse);
				}
			break;
#endif
		default:
			((CTBufferSecurityStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTBufferSecurityStep*)iStep)->CloseTMSGraphicsStep();
			TInt err = RProperty::Set(KUidWServSecurityTesting,EWServSecTestBufferSecurity,EFalse);
			User::LeaveIfError(err);
			TestComplete();
		}
	((CTBufferSecurityStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(BufferSecurity)
