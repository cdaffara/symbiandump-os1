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
 

#ifndef __TIPCFUZZ_STEP_H__
#define __TIPCFUZZ_STEP_H__

#include <test/testexecutestepbase.h>
#include "tipcfuzzer.h"

_LIT(KTIpcFuzzStep,"IpcFuzzTestStep");

class TIpcFuzzTestParams
	{
public:
	const TText* iServerName; // Server name
	const TInt iOpcodeCount; // Maintaining operation count
	const TVersion iServerVersion; //Server version
	const TInt iMsgSlotCount; // Message slot count
	};

class CIpcFuzzTest : public CTestStep
	{
public:
	CIpcFuzzTest(); // Constructor
	~CIpcFuzzTest(); // Destructor

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
private:
	void doTestIpcFuzzL();
	
private:
	const TIpcFuzzTestParams* iTestParameters;
	};

// Test resource class
class RIpcFuzzTest : public RSessionBase
	{
public:
	RIpcFuzzTest(); // Constructor
	~RIpcFuzzTest(); // Destructor
	
	// Fuzzy Logic
	void IpcFuzzLogicL(CIpcFuzzer* aIpcFuzzer, const TIpcFuzzTestParams& aTestParameters);
	void IpcFuzzerL(TPtr8* aPtr, const TIpcFuzzTestParams& aTestParameters, TBuf<15> aServerName);
	void IpcFuzzerL(TPtr* aPtr, const TIpcFuzzTestParams& aTestParameters, TBuf<15> aServerName);
	void IpcFuzzerL(TUint aData, const TIpcFuzzTestParams& aTestParameters, TBuf<15> aServerName);
	void SendMessageToServerL(TInt& aOpcode,const TInt& aOpcodeCnt,TIpcArgs& aIpcArgs, TBuf<15> aServerName);
	};

#endif //__TIPCFUZZ_STEP_H__
