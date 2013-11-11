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

#ifndef __TBUFFERSECURITY_H__
#define __TBUFFERSECURITY_H__

#include "AUTO.H"
#include "../tlib/testbase.h"
#include <w32std.h>
#include "../SERVER/w32cmd.h"

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
#include "../nonnga/CLIENT/w32comm.h"
#endif

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
#include "../nga/CLIENT/w32comm.h"
#endif

#include <e32property.h>
#include "TGraphicsHarness.h"

//Set this #define to add extra logging to this test case (useful when debugging a test fail)
//#define _TBUFS_LOGGING

//Set this #define to initiate a long running soak test, this should be done periodically
//#define _TBUFS_TEST_SOAK_TEST

#ifdef _TBUFS_LOGGING
#define TBufSStartLogText StartLogText
#define TBufSLogText LogText
#define TBufSLogFormat LogFormat
#else
#define TBufSStartLogText
#define TBufSLogText
#define TBufSLogFormat
#endif

LOCAL_D const TUint KPanicThreadHeapSize=0x4000;

_LIT(KLitKernExec, "KERN-EXEC");
_LIT(KTestName, "DEF086238 Buffer Security Test");
static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
static _LIT_SECURITY_POLICY_C1(KWriteDeviceDataMgmtPolicy,ECapabilityWriteDeviceData);
const TInt KTestDataMax = 256;
const TInt KTestDataMax32 = KTestDataMax/4;

struct TTestThreadData
	{
	TInt iOpCode;
	union
		{
		TUint8 iData[KTestDataMax];
		TUint32 iData32[KTestDataMax32];
		};
	TInt iDataLength;
	};

enum KBufSecTestType
	{
	EBufSecUnInitialised,
	EBufSecRandom,
	EBufSecZero
	};

union TTestDataStore
	{
	TAny *any;
	TWsClCmdSetPointerCursorArea* cursorArea;
	TWsClCmdHeapSetFail *heapSetFail;
	TXYInputType *xyInputType;
	};

class CTBufferSecurity : public CTWsGraphicsBase
	{
public:
	CTBufferSecurity(CTestStep* aStep);
	~CTBufferSecurity();
	void TestWsBufferL(TInt aOpCode, TUint aDataFill, TBool aEightBit=ETrue);
	void TestBadStringAnimDllL();
	void TestBadStringL();
	void TestBadIpcL();
	void ConstructL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestBufferSecurityL();
	};

class RTestIpcSession : public RSessionBase
	{
public:
	inline RTestIpcSession() {};
	TInt Connect();
	TInt SendBadBuffer();
private:
	TInt iWsHandle;
	};

class CTBufferSecurityStep : public CTGraphicsStep
	{
public:
	CTBufferSecurityStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTBufferSecurityStep,"TBufferSecurity");

#endif
