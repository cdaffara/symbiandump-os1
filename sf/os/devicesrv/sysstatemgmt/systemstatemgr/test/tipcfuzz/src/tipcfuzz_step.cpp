// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tipcfuzz_step.h"

// Structure to hold server data to Fuzz.
// To add tests for new servers, extend the array referring to the class definition. 
const TIpcFuzzTestParams KTestParameters[] = 
	{
		{ _S("!SysStateMgr"), 7, TVersion(0,1,1),2 },
		{ _S("!SusAdaptionSrv"), 30, TVersion(0,1,1),1},
		{ _S("!SusUtilSrv"), 6, TVersion(0,1,1),1}
	};

// Constants for the data index count
const TInt K8BitDataDescCount = 9;
const TInt K16BitDataDescCount = 9;
const TInt KIntDataCount = 10;

// Destructor
CIpcFuzzTest::~CIpcFuzzTest()
	{
	}

// Constructor
CIpcFuzzTest::CIpcFuzzTest()
	{
	SetTestStepName(KTIpcFuzzStep);
	}
	
// Constructor
RIpcFuzzTest::RIpcFuzzTest()
	{
	}

// Destructor
RIpcFuzzTest::~RIpcFuzzTest() 
	{
	Close();
	}

TVerdict CIpcFuzzTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CIpcFuzzTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CIpcFuzzTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CIpcFuzzTest started..."));

	__UHEAP_MARK;
	doTestIpcFuzzL();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("...CIpcFuzzTest completed!"));
	return TestStepResult();
	}

/** 
 Old Test CaseID 		APPFWK-IPCFUZZ-0001
 New Test CaseID 		DEVSRVS-SSMA-IPCFUZZ-0001
 */
void CIpcFuzzTest::doTestIpcFuzzL()
	{
	INFO_PRINTF1(_L("APPFWK-IPCFUZZ-0001:IpcFuzz Logic started"));
	RIpcFuzzTest rIpcTest;
	CleanupClosePushL(rIpcTest);
	
	CIpcFuzzer* ipcFuzzer = CIpcFuzzer::NewL();
	CleanupStack::PushL(ipcFuzzer);
	
	TInt testIdx = (sizeof(KTestParameters)/sizeof(TIpcFuzzTestParams));
	
	for(TInt i=0; i<testIdx; i++)
		{
		iTestParameters = &(KTestParameters[i]);
		INFO_PRINTF2(_L("IpcFuzz starts on Server:%s"),iTestParameters->iServerName);	
		TRAPD(err,rIpcTest.IpcFuzzLogicL(ipcFuzzer, *iTestParameters));
		TEST(err==KErrNone);
		INFO_PRINTF2(_L("IpcFuzz ends on Server:%s"),iTestParameters->iServerName);
		}

	CleanupStack::PopAndDestroy(ipcFuzzer);
	CleanupStack::PopAndDestroy(&rIpcTest);
	}
	
// Fuzzy Logic method, which prepares the junk data (8 & 16 bit descriptor, integer) to be sent to server.
void RIpcFuzzTest::IpcFuzzLogicL(CIpcFuzzer* aIpcFuzzer, const TIpcFuzzTestParams& aTestParameters)
	{	
	TBuf<15> serverName(aTestParameters.iServerName);
	TVersion serverVersion(aTestParameters.iServerVersion);
	
	RDebug::Print(_L(">>> Server Name:%S"),&serverName);
	//Creates session with the server
	TInt ret = CreateSession(serverName,serverVersion);
	User::LeaveIfError(ret);
	RDebug::Print(_L(">>> Successfully created a session with the Server:%S"),&serverName);
	
	HBufC8* buf8 = NULL;
	HBufC* buf16 = NULL;
	TInt data = 0;
				
	TInt iteration = 1;
	TInt caseIdx = 1;  
				
	while(iteration<4)
		{
		caseIdx = 1;
		switch(iteration)
			{
			case 1:
				{
				while(caseIdx<K8BitDataDescCount)
					{
					// Gets the 8 bit desc data. 
					buf8 = aIpcFuzzer->GetDescData8L(caseIdx);
					if(buf8)
						{
						TPtr8 ptr8 = buf8->Des();
						// Package the arguments to send to server.
						IpcFuzzerL(&ptr8, aTestParameters, serverName);
						delete buf8;
						buf8 = NULL;
						}
					caseIdx++;
					}		
				}
				break;
			case 2:
				{
				while(caseIdx<K16BitDataDescCount)
					{
					// Gets the 16 bit desc data.
					buf16 = aIpcFuzzer->GetDescData16L(caseIdx);
					if(buf16)
						{
						TPtr ptr16 = buf16->Des();
						// Package the arguments to send to server.
						IpcFuzzerL(&ptr16, aTestParameters, serverName);
						delete buf16;
						buf16 = NULL;
						}
					caseIdx++;
					}
				}
				break;
			case 3:
				{
				while(caseIdx<KIntDataCount)
					{
					// Get integer data.
					data = aIpcFuzzer->GetIntData(caseIdx);
					// Package the arguments to send to server.
					IpcFuzzerL(data, aTestParameters, serverName);
					caseIdx++;
					}
				}
				break;
			default:
				break;
			}
		iteration++;	
		}
	}

// The overloaded method which takes 8 bit pointer descriptor along with the test parameters
// data. Package the arguments to send to server.
void RIpcFuzzTest::IpcFuzzerL(TPtr8* aPtr, const TIpcFuzzTestParams& aTestParameters, TBuf<15> aServerName)
	{
	TIpcArgs args;
	TInt msgSlotCnt = aTestParameters.iMsgSlotCount;
	TInt opcode=0;
		
	for (TInt msgSlot=0;msgSlot<msgSlotCnt;msgSlot++)
		{
		// loop executes for number of message slots
	 	 opcode=0;
	 	 // Sets the argument value
	 	 args.Set(msgSlot, aPtr);
		 SendMessageToServerL(opcode, aTestParameters.iOpcodeCount, args, aServerName);
	    }
	}
	
// The overloaded method which takes 16 bit pointer descriptor along with the test parameters
// data. Package the arguments to send to server.
void RIpcFuzzTest::IpcFuzzerL(TPtr* aPtr, const TIpcFuzzTestParams& aTestParameters, TBuf<15> aServerName)
	{
	TIpcArgs args;
	TInt msgSlotCnt = aTestParameters.iMsgSlotCount;
	TInt opcode=0;
		
	for (TInt msgSlot=0;msgSlot<msgSlotCnt;msgSlot++)
		{
		 // loop executes for number of message slots
	 	 opcode=0;
	 	 // Sets the argument value
	 	 args.Set(msgSlot, aPtr);
		 SendMessageToServerL(opcode, aTestParameters.iOpcodeCount, args, aServerName);
	    }
	}

// The overloaded method which takes unsigned integer value along with the test parameters
// data. Package the arguments to send to server.
void RIpcFuzzTest::IpcFuzzerL(TUint aData, const TIpcFuzzTestParams& aTestParameters, TBuf<15> aServerName)
	{
	TIpcArgs args;
	TInt msgSlotCnt = aTestParameters.iMsgSlotCount;
	TInt opcode=0;
		
	for (TInt msgSlot=0;msgSlot<msgSlotCnt;msgSlot++)
		{
		 // loop executes for number of message slots
	 	 opcode=0;
	 	 // Sets the argument value
	 	 args.Set(msgSlot, aData);
		 SendMessageToServerL(opcode, aTestParameters.iOpcodeCount, args, aServerName);
	    }
	}

// Sends a message to the server and waits for the reply. An opcode specifies the service
// required. A set of message arguments is passed that can be used to specify client addresses,
// which the server can use to read from and write to client address space.
void RIpcFuzzTest::SendMessageToServerL(TInt& aOpcode,const TInt& aOpcodeCnt,TIpcArgs& aIpcArgs, TBuf<15> aServerName)
	{
	while(aOpcode<aOpcodeCnt)
		{
		aOpcode++;
		//sends a packaged data to server to execute particular function at the given Opcode.
		TInt ret = SendReceive(aOpcode, aIpcArgs);
		
		// We are checking for just BAD error codes that should not be returned by the server
		// For e.g. KErrServerTerminated should never be returned, so we are failing the test here.
		// Other normal error codes such as KErrArgument, KErrNotSupported, KErrOverflow, 
		// KErrBadDescriptor..etc are normal in the course of validation and so the test does not
		// check that.
		// Use the epocwind.out file to see the error returned from server for each fuzzy test call.
		if(ret==KErrServerTerminated)
			{
			RDebug::Print(_L("-- Server Response code===>%d on Opcode:%d"),ret,aOpcode);
			User::Leave(KErrServerTerminated);
			}
		//SusAdaptionSrv panics when we try to load the same library twice, hence we are cleaning up after every call to SusAdaptionSrv
		if (aServerName.Compare(_L("!SusAdaptionSrv")) == 0)
			{
			TInt opcode = 30;
			TInt ret2 = SendReceive(opcode, aIpcArgs);
			RDebug::Print(_L("-- Cleaning up: ret %d"), ret2);
			}
		RDebug::Print(_L("-- Server Response code===>%d on Opcode:%d"),ret,aOpcode);
		}
	}
	
