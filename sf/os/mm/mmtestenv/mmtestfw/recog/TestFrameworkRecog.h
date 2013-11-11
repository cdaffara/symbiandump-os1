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
//

#ifndef __TESTFRAMEWORKRECOG_H__
#define __TESTFRAMEWORKRECOG_H__

#include <apmrec.h>

_LIT(KRecogSemaphoreName, "TFR_SEM");				// Semaphore of the current test thread
_LIT(KRecogParentSemaphoreName, "TFR_PARENT_SEM");	// Semaphore of the main recognizer thread
const TInt KUidTestFrameworkRecognizerValue = 0x101F7C0E;
const TUid KUidTestFrameworkRecognizer = { KUidTestFrameworkRecognizerValue };

class TTestScriptInfo
	{
public:
	TInt iThreadStartupDelay;
	TFileName iScriptPath;
	TBuf<32> iParams;
	};
typedef CArrayFixFlat<TTestScriptInfo> CTestScriptArray;

// CTestFrameworkRecogActive
class CTestFrameworkRecogActive : public CActive
	{
public:
	CTestFrameworkRecogActive(CTestScriptArray* aTestScriptArray);
	~CTestFrameworkRecogActive();
public:
	TInt CreateNextTestThread();
protected:
	static TInt ThreadFunc(TAny* aPtr);
	void DoThreadFuncL();
	static TInt StartTestThreadFn(TAny* aPtr);
	void DoStartTestThreadL();
	// from CActive
protected:
	void RunL();
	void DoCancel();
protected:
	CTestScriptArray* iTestScriptArray;
	TInt iCurrentScript;
	};

// CTestFrameworkRecognizer
class CTestFrameworkRecognizer : public CApaDataRecognizerType
	{
public:
	CTestFrameworkRecognizer();
	~CTestFrameworkRecognizer();
	// New functions
public:
	void DoCreateL();
protected:
	void LoadConfigFileL(const TDesC& aFileName);
	void ProcessLineL(const TDesC8& aLine);
	// from CApaDataRecognizerType
protected:
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt aIndex) const;
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
protected:
	CTestScriptArray* iTestScriptArray;
	CTestFrameworkRecogActive* iTestActive;
	TBool iRunScript;
	};

#endif // __TESTFRAMEWORKRECOG_H__
