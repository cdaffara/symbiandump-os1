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

#ifndef __TESTREGISTRY_H__
#define __TESTREGISTRY_H__

#include <e32cons.h>
#include <f32file.h>
#include "UnitTest.h"

class TTest;

// Convert appends the second parameter to the first in textual format
// eg. TInt16(-123)  becomes  "-123"
void Convert(TDes& aDes, TInt8 aValue);
void Convert(TDes& aDes, TInt16 aValue);
void Convert(TDes& aDes, TInt32 aValue);
void Convert(TDes& aDes, TInt64 aValue);
void Convert(TDes& aDes, TInt aValue);
void Convert(TDes& aDes, TUint8 aValue);
void Convert(TDes& aDes, TUint16 aValue);
void Convert(TDes& aDes, TUint32 aValue);
void Convert(TDes& aDes, TUint aValue);
void Convert(TDes& aDes, TReal32 aValue);
void Convert(TDes& aDes, TReal64 aValue);
void Convert(TDes& aDes, const TDesC8& aValue);
void Convert(TDes& aDes, const TDesC16& aValue);
void Convert(TDes& aDes, TTime aValue);
void Convert(TDes& aDes, TTimeIntervalMicroSeconds aValue);

#include <LbsCommon.h>

void Convert(TDes& aDes, TPositionQuality::TCostIndicator aValue);
void Convert(TDes& aDes, TPositionQuality::TPowerConsumption aValue);

#include <LbsCriteria.h>

void Convert(TDes& aDes, TPositionSelectionOrder::TField aValue);
void Convert(TDes& aDes, TPositionSelectionOrder::TOrder aValue);

class RTestRegistry
	{
public:
	~RTestRegistry();

	// Static interface functions
	static RTestRegistry& Instance();
	static void CreateL();
	static void AddTest(const TTest& aTest);
	static void RunAllTests();
	static void TruncateFileName(TDes& aTruncated, const char* aFileName);
	static void TestLeft(const TDesC& aTestName, TInt aErrorCode);
	static void TestFailed(const TDesC& /*aTestName*/, const char* aCppFile, TInt aErrorCode, const TText* aCondition);

	template <typename T1, typename T2>
	static void TestFailed(const TDesC& /*aTestName*/, const char* aCppFile, TInt aErrorCode,
			const T1& aExpected, const T2& aActual)
		{
		TBuf<64> cppTestFileName;
		TruncateFileName(cppTestFileName, aCppFile);
		TBuf<40> expected, actual;
		// Convert aExpected and aActual to text. If this gives a compilation error,
		// it means there is no available over load for types T1 and T2: a new overload
		// must be added.
		Convert(expected, aExpected);
		Convert(actual, aActual);
		test.Printf(_L("Failure: expected %S but got %S line %d in %S.\n"),
			&expected, &actual, aErrorCode, &cppTestFileName);
		Instance().iNumTestFailures ++;
		}

	// Functions for possible use in acceptance testing
	static TInt GetNumTests();
	static void RunTest(TInt aTestIndex);
	static TTest& Test(TInt aTestIndex);

private:
	RTestRegistry();
	void ConstructL();
	RTestRegistry(const RTestRegistry&); // not defined / disabled
	void operator=(const RTestRegistry&); // not defined / disabled
private:
	// Actual methods of RTestRegistry
	void DoAddTest(const TTest& aTest);
	void LogResult(const TDesC& aName, TInt aTestId, const TDesC& aFileName, TBool aPassed, TInt aErrorCode = 0);
	void RunTests();
	void DoRunTest(TInt aTestIndex);
private:
	// Owned
	RFs iFs;
	RFile iLogFile;
	TInt iNumTestFailures;
private:
	// Un-owned
	RPointerArray<TTest> iTests;
	};

#endif // __TESTREGISTRY_H__
