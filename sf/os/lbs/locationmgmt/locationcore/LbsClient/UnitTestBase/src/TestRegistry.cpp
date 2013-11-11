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

#include "unittest.h"
#include "TestRegistry.h"
#include "Test.h"

/**
 * The overloaded Convert functions are used by the
 * RTestRegistry::TestFailed function to create diagnostic
 * output.
 */
void Convert(TDes& aDes, TInt8 aValue)
	{
	aDes.AppendNum(aValue);
	}

void Convert(TDes& aDes, TInt16 aValue)
	{
	aDes.AppendNum(aValue);
	}

void Convert(TDes& aDes, TInt32 aValue)
	{
	aDes.AppendNum(static_cast<TInt>(aValue));
	}

void Convert(TDes& aDes, TInt64 aValue)
	{
	aDes.AppendNum(aValue);
	}

void Convert(TDes& aDes, TInt aValue)
	{
	aDes.AppendNum(aValue);
	}

void Convert(TDes& aDes, TUint8 aValue)
	{
	aDes.AppendNum(static_cast<TUint>(aValue));
	}

void Convert(TDes& aDes, TUint16 aValue)
	{
	aDes.AppendNum(static_cast<TUint>(aValue));
	}

void Convert(TDes& aDes, TUint32 aValue)
	{
	aDes.AppendNum(static_cast<TUint>(aValue));
	}

void Convert(TDes& aDes, TUint aValue)
	{
	aDes.AppendNum(aValue);
	}

void Convert(TDes& aDes, TReal32 aValue)
	{
	aDes.AppendNum(aValue, TRealFormat());
	}

void Convert(TDes& aDes, TReal64 aValue)
	{
	aDes.AppendNum(aValue, TRealFormat());
	}

void Convert(TDes& aDes, const TDesC8& aValue)
	{
	aDes.Copy(aValue);
	}

void Convert(TDes& aDes, const TDesC16& aValue)
	{
	aDes = aValue;
	}

void Convert(TDes& aDes, TTime aValue)
	{
	_LIT(KLbsFormat, "[0x%08X, 0x%08X]");	
	aDes.AppendFormat(KLbsFormat,
		I64HIGH(aValue.Int64()),
		I64LOW(aValue.Int64()));
	}

void Convert(TDes& aDes, TTimeIntervalMicroSeconds aValue)
	{
	_LIT(KLbsFormat, "[0x%08X, 0x%08X]");	
	aDes.AppendFormat(KLbsFormat,
		I64HIGH(aValue.Int64()),
		I64LOW(aValue.Int64()));
	}

void Convert(TDes& aDes, TPositionQuality::TCostIndicator aValue)
	{
	aDes.AppendNum(static_cast<TUint>(aValue));
	}

void Convert(TDes& aDes, TPositionQuality::TPowerConsumption aValue)
	{
	aDes.AppendNum(static_cast<TUint>(aValue));
	}

void Convert(TDes& aDes, TPositionSelectionOrder::TField aValue)
	{
	aDes.AppendNum(static_cast<TUint>(aValue));
	}

void Convert(TDes& aDes, TPositionSelectionOrder::TOrder aValue)
	{
	aDes.AppendNum(static_cast<TUint>(aValue));
	}



/**
 * Note: Console output is not available when the constructor is
 *       called.  No leaving functions can be called...
 */
RTestRegistry::RTestRegistry()
	{
	}

void RTestRegistry::ConstructL()
	{

	_LIT(KLogFileHeader, "#------------------------Test Run Start-------------------------\r\n");

	if (KErrNone == iFs.Connect())
		{
		iFs.MkDirAll(KLogPath);

		TBuf<80> logFileName(KLogPath);
		logFileName += KLogFileName;
		TInt err = iLogFile.Open(iFs, logFileName, EFileWrite);
		if (err == KErrNotFound) // file does not exist - create it
			{
			err = iLogFile.Create(iFs, logFileName, EFileWrite);
			}
		if (err == KErrNone)
			{
			TInt pos = 0;
			iLogFile.Seek(ESeekEnd, pos);

			err = iLogFile.Write(KLogFileHeader);
			iLogFile.Flush();
			}
		}
	}

RTestRegistry::~RTestRegistry()
	{
	iLogFile.Close();
	iFs.Close();
	}


RTestRegistry& RTestRegistry::Instance()
	{
	static RTestRegistry t;
	return t;
	}

void RTestRegistry::CreateL()
	{
	Instance().ConstructL();
	}

void RTestRegistry::AddTest(const TTest& aTest)
	{
	Instance().DoAddTest(aTest);
	}

void RTestRegistry::DoAddTest(const TTest& aTest)
	{
	TInt err = iTests.Append(&aTest);
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
	}

/**
 * Logs unit test results in a standard format to be converted to an HTML
 * report by Billys Perl script.
 *
 * Example log line:
 *     AUTO;07-10-2002;14:00:04;UTS;IUSG;1;FAILED;GprsUnitTest.cpp;IUSG-7-1;2375
 */
void RTestRegistry::LogResult(const TDesC& aName, TInt aTestId, const TDesC& aFileName,
							  TBool aPassed, TInt aErrorCode)
	{

	_LIT(KLogFileFormat, "AUTO;%02d-%02d-%04d;%02d:%02d:%02d;UTS;%S;%d;%s;%S;%S-%d;");

	TTime time;
	TDateTime dateTime;
	time.HomeTime();
	dateTime=time.DateTime();
	TUint year = dateTime.Year();
	TUint day = dateTime.Day() + 1; // Day() & Month() starts from 0
	TUint month = dateTime.Month() + 1;
	TUint hour = dateTime.Hour();
	TUint minute = dateTime.Minute();
	TUint second = dateTime.Second();

	TBuf8<32> name8;
	TBuf8<128> fileName8;
	name8.Copy(aName);
	fileName8.Copy(aFileName.Mid(aFileName.LocateReverse('\\') + 1));
	TBuf8<128> buf;
	_LIT(KPass, "PASSED");
	_LIT(KFail, "FAILED");
	buf.Format(
		KLogFileFormat, day, month, year, hour, minute, second,
		&name8, aTestId, aPassed ? KPass : KFail,
		&fileName8, &name8, aTestId);
	if (aErrorCode)
		buf.AppendNum(aErrorCode);

	iLogFile.Write(buf);
	iLogFile.Write(KNewLine8);
	}

void RTestRegistry::RunTests()
	{
	TInt cnt = iTests.Count();
	__UHEAP_MARK;
	for (TInt i=0; i<cnt; i++)
		{
		RunTest(i);
		}
	__UHEAP_MARKEND;

	iLogFile.Flush();
	_LIT(KFailString, "There were %d failures.\n");
	_LIT(KNoFailString, "There were no test failures.\n");
	if (iNumTestFailures > 0)
		test.Printf(KFailString, iNumTestFailures);
	else
		test.Printf(KNoFailString);
	}

void RTestRegistry::RunAllTests()
	{
	Instance().RunTests();
	}

void RTestRegistry::TestFailed(const TDesC& /*aTestName*/, const char* aCppFile, TInt aErrorCode, const TText* aCondition)
	{
	_LIT(KFailureIndFmt, "Failure: \"%s\" line %d in %S.\n");
	TBuf<64> cppTestFileName;
	TruncateFileName(cppTestFileName, aCppFile);
	test.Printf(KFailureIndFmt,
		aCondition, aErrorCode, &cppTestFileName);
	Instance().iNumTestFailures ++;
	}

void RTestRegistry::TestLeft(const TDesC& aTestName, TInt aErrorCode)
	{
	_LIT(KFailureIndFmt, "Failure: %S Exited abnormally with error code: %d.\n");
	test.Printf(KFailureIndFmt,
		&aTestName, aErrorCode);
	Instance().iNumTestFailures ++;
	}

void RTestRegistry::TruncateFileName(TDes& aTruncated, const char* aFileName)
	{
	aTruncated.Copy(TPtrC8(_S8(aFileName)).Right(aTruncated.MaxLength()));
	aTruncated = aTruncated.Mid(aTruncated.LocateReverse('\\')+1);
	}

//
// Not currently used
//

TInt RTestRegistry::GetNumTests()
	{
	return Instance().iTests.Count();
	}

void RTestRegistry::RunTest(TInt aTestIndex)
	{
	Instance().DoRunTest(aTestIndex);
	}

void RTestRegistry::DoRunTest(TInt aTestIndex)
	{
	TTest& currentTest = Test(aTestIndex);
	TPtrC testName = currentTest.Name();
	TPtrC testSpecId = currentTest.TestSpecId();
	TPtrC testScriptName = currentTest.TestScriptName();
	_LIT(KDoRunTestFormat, "Test #%03d. Running %S.\n");
	test.Printf(KDoRunTestFormat, aTestIndex+1, &testName);

	__UHEAP_MARK;
	TRAPD(err, currentTest.RunTestL())
	__ASSERT_ALWAYS(RThread().RequestCount() == 0, PANIC(EPanicRequestNotWaitedFor));
	__UHEAP_MARKEND;

	if (err)
		TestLeft(currentTest.Name(), err);
	LogResult(
		testSpecId, currentTest.IntTestId(), testScriptName,
		currentTest.Result() == TTest::EPassed && err == KErrNone,
		err ? err : currentTest.LineNumOfTestFailure());
	}

TTest& RTestRegistry::Test(TInt aTestIndex)
	{
	return *(Instance().iTests[aTestIndex]);
	}
