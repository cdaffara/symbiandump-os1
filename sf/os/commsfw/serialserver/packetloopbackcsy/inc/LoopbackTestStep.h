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
// Declarations for loopback tests
// 
//

/**
 @file
*/

#ifndef __LOOPBACK_TEST_STEP_H__
#define __LOOPBACK_TEST_STEP_H__

#include <c32comm.h>

#include <test/testexecutestepbase.h>

class CLoopbackTestStepBase : public CTestStep
	{
public:
	CLoopbackTestStepBase(TInt aTestNumber);
	virtual ~CLoopbackTestStepBase();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	static void SetTestNumber(TInt aTestNumber);
	static void SetFlowControl(TUint aUnit, TBool aFlowControlOn);
	static void SetReadResult(TUint aUnit, TInt aReadResult);
	static void SetWriteResult(TUint aUnit, TInt aWriteResult);
	
	void TestErrorCodeL(TInt aErrCode, const TDesC& aMessage);
	void TestErrorCodeL(TInt aErrCode, TInt aExpectedErrCode, const TDesC& aMessage);
	void TestBooleanTrueL(TInt aBool, const TDesC& aMessage);
	void TestErrorCodeAndDescriptorL(TInt aErrCode, const TDesC8& aDesC, const TDesC8& aExpectedDesC, const TDesC& aMessage);
	void TestErrorCodeAndDescriptorL(TInt aErrCode, TInt aExpectedErrCode, const TDesC8& aDesC, const TDesC8& aExpectedDesC, const TDesC& aMessage);
	
	/** The test number for this test */
	TInt iTestNumber;
protected:
	/** The comm server for this test step */
	RCommServ iCommServer;
	RComm iCommPort1, iCommPort2, iCommPort3, iCommPort4;
	};
	
class CLoopbackTestStep1 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep1();
	~CLoopbackTestStep1();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep2 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep2();
	~CLoopbackTestStep2();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep3 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep3();
	~CLoopbackTestStep3();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep4 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep4();
	~CLoopbackTestStep4();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep5 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep5();
	~CLoopbackTestStep5();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep6 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep6();
	~CLoopbackTestStep6();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep7 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep7();
	~CLoopbackTestStep7();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep8 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep8();
	~CLoopbackTestStep8();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep9 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep9();
	~CLoopbackTestStep9();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep10 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep10();
	~CLoopbackTestStep10();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep11 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep11();
	~CLoopbackTestStep11();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep12 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep12();
	~CLoopbackTestStep12();
	virtual TVerdict doTestStepL();
private:
	};

class CLoopbackTestStep13 : public CLoopbackTestStepBase
	{
public:
	CLoopbackTestStep13();
	~CLoopbackTestStep13();
	virtual TVerdict doTestStepL();
private:
	};

_LIT(KLoopbackTestStep1,"LoopbackTestStep1");
_LIT(KLoopbackTestStep2,"LoopbackTestStep2");
_LIT(KLoopbackTestStep3,"LoopbackTestStep3");
_LIT(KLoopbackTestStep4,"LoopbackTestStep4");
_LIT(KLoopbackTestStep5,"LoopbackTestStep5");
_LIT(KLoopbackTestStep6,"LoopbackTestStep6");
_LIT(KLoopbackTestStep7,"LoopbackTestStep7");
_LIT(KLoopbackTestStep8,"LoopbackTestStep8");
_LIT(KLoopbackTestStep9,"LoopbackTestStep9");
_LIT(KLoopbackTestStep10,"LoopbackTestStep10");
_LIT(KLoopbackTestStep11,"LoopbackTestStep11");
_LIT(KLoopbackTestStep12,"LoopbackTestStep12");
_LIT(KLoopbackTestStep13,"LoopbackTestStep13");

#endif

