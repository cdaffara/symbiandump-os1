//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This file contains the defintion for the base class from 
// *               which all unit test classes are derived.
//

// UnitTestBase.h

/** @file UnitTestBase.h
 *
 */

#ifndef UNITTESTBASE_H__
#define UNITTESTBASE_H__

#include <e32base.h>
#include <e32test.h>
#include <c32comm.h>
#include "f32file.h"
//#include "inifile.h"

#include <etel.h>
#include <etelmm.h>

//#include "Constants.h"

//comment out this line if both passed & failed tests
//are to be logged in the test log file, reftsyTest.txt
//#define LOG_FAILED_TESTS_ONLY

#define TEST(AAA) {TInt XXX = (AAA); if (XXX) return (__LINE__);}

//_LIT(KCsyName, "ECUART.CSY");
_LIT(KCsyName, "GSM0710.CSY");
//_LIT(KTsyName,    "phonetsy");
_LIT(KTsyName,    "GSM0707");

_LIT(KTestSessionPath, "c:\\Logs\\Tests\\");
_LIT(KUnitTestSpec, "UTS;");
_LIT(KAccTestSpec, "ATS;");
_LIT(KAutoTestMarker, "AUTO;");
_LIT(KPassed, "PASSED;");
_LIT(KFailed, "FAILED;");
_LIT(KZeroPadding, "0");
_LIT(KHyphen,"-");
_LIT(KSemiColon,";");
_LIT(KColon, ":");
_LIT(KNewLine, "\r\n");

const TUint KTsyTesterSetupTime = 3000000;
const TUint KTsyTestCompleteTime = 3000000;
const TUint KBufSize1 = 1;
const TUint KBufSize4 = 4;
const TUint KBufSize25 = 25;
const TUint KBufSize10 = 10;
const TUint KBufSize100 = 100;
const TUint KZeroPadValue = 10;
const TUint KMaxTestTelNumberSize = 20;

// BttTest.Ini constants
_LIT(KIniFile, "CommPTest.INI");
_LIT(KIniSection, "PhoneNumberList");
_LIT(KIniSection2, "Sms");
_LIT(KIniSection3, "GeneralSettings");
_LIT(KIniTelNumConnect1, "TelNumConnect1");
_LIT(KIniTelNumConnect2, "TelNumConnect2");
_LIT(KIniTelNumConnect3, "TelNumConnect3");
_LIT(KIniTelNumConnect4, "TelNumConnect4");
_LIT(KIniTelNumConnect5, "TelNumConnect5");
_LIT(KIniTelNumConnect6, "TelNumConnect6");
_LIT(KIniTelNumConnect7, "TelNumConnect7");
_LIT(KIniTelNumDataConnect, "TelNumDataConnect");
_LIT(KIniTelNumNeverConnect, "TelNumNeverConnect");
_LIT(KIniTelNumRejectCall, "TelNumRejectCall");
_LIT(KIniTelNumRemoteEndHangUp, "TelNumRemoteEndHangUp");
_LIT(KIniTelNumIncorrect1, "TelNumIncorrect1");
_LIT(KIniTelNumIncorrect2, "TelNumIncorrect2");
_LIT(KIniTelNumMobile1, "TelNumMobile1");
_LIT(KIniTelNumDTMFTest, "TelNumDTMFTest");
_LIT(KIniTelNumCallback, "TelNumCallbackService");
_LIT(KIniTelNumDataServer1, "TelNumDataServer1");
_LIT(KIniTelNumDataServer2, "TelNumDataServer2");
_LIT(KIniTelNumDataServer3, "TelNumDataServer3");
_LIT(KIniTelNumDataServer4, "TelNumDataServer4");
_LIT(KIniSmsDestNumber, "SmsDestNumber");
_LIT(KIniTelNumberInMadCow, "SimTelNumberInMadCow");
_LIT(KIniSmsNetworkName, "SmsNetworkName");
_LIT(KIniTypeOfTest, "TypeOfTest");
_LIT(KIniKeepPhoneOpen, "KeepPhoneOpen");
_LIT(KIniAutoRepeatFailedTests, "AutoRepeatFailedTests");
_LIT(KIniTypeOfNumber1, "DestNumberIs44Format");
_LIT(KIniTypeOfNumber2, "MadCowNumberIs44Format");

// Moved from phone
const TUint KDelayLong   = 10000000;
const TUint KDelayMedium = 1000000;
const TUint KDelayShort  = 100000;

//const TUint KOneSecond = 1000000;
const TUint KTwoSeconds = 2000000;
const TUint KFiveSeconds = 5000000;
const TUint KTenSeconds = 10000000;
const TUint KTwentySeconds = 20000000;

#define KEightSeconds TTimeIntervalMicroSeconds32(8000000)

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_SIR_NAME _L("ESIR")
#define PDD_UART_NAME _L("ECUART4")
#define LDD_NAME _L("ECOMM")
#endif

const TInt KErrWrongValue=KErrGeneral;

//Network Name
//Size of text string must be less than
//12 for the  Cellnet and Vodafone test SIM
//10 for the  Orange test SIM
//8  for the  One-2-One  test SIM
//for the SMSP list storage test to pass.
const TUint KMaxSimNameLen = 12;

class CUnitTestBase : public CBase
	{
public:
	enum TTestMode
		{
		EBatch,
		EInteractive
		};

	enum TTestPhoneNumbers
		{
		ETelNumConnect1,
		ETelNumConnect2,
		ETelNumConnect3,
		ETelNumConnect4,
		ETelNumConnect5,
		ETelNumConnect6,
		ETelNumConnect7,
		ETelNumDataConnect,
		ETelNumNeverConnect,
		ETelNumRejectCall,
		ETelNumIncorrect1,
		ETelNumIncorrect2,
		ETelNumDTMFTest,
		ETelNumCallbackService,
		ETelNumDataServer1,
		ETelNumDataServer2,
		ETelNumDataServer3,
		ETelNumDataServer4,
		ESmsDestNumber,
		ETelNumberInMadCow,
		ESmsNetworkName,
		ETypeOfTest
		};

//	virtual static void CreateTestL()=0;//not allowed in C++!!
	~CUnitTestBase();

	inline RTest Test() const;

	//These functions are called within CreateTestL() which is a
	//static member function of CUnitTestBase derived classes.
	//Every derived class must implement CreateTestL() as it
	//first calls NewL() to instantiate the derived class before
	//calling SetUpTest() and RunUnitTestsL().
	//CreateTestL() is called in E32Main() which should be in the file
    //containing the CUnitTestBase derived class implementation
	//Please look at the SMS test code for an example on how to
	//code up E32main() and CreateTestL()
	virtual TInt SetUpTest();    //load drivers & start C32
	void RunUnitTestsL();

	//Helper Functions
	TInt CompareTestResult(TInt aExpectedResult,TRequestStatus aStatus);

	//For running more than one at once
	void ResizeConsoleL(const TDesC& aName, TInt aXOrg, TInt aYOrg, TInt aXSize, TInt aYSize);

	//For test result logging
	//Setters to be called by derived classes within their specific tests
	inline void SetTsIdentifier(const TName& aTsIdentifier);
	inline void SetMethodName(const TName& aMethodName);
	inline void SetFunIndex(TUint aFunIndex);
	void SetFunIndex2(TUint aFunIndex1,TUint aFunIndex2);
	void SetFunIndex3(TUint aFunIndex1,TUint aFunIndex2,
		              TUint aFunIndex3);
	void SetFunIndex4(TUint aFunIndex1,TUint aFunIndex2,
		              TUint aFunIndex3,TUint aFunIndex4);

	// These three made public for the acceptance tests
	TInt  LoadComms();
	TInt  UnloadComms();
	TBool DoSingleTestL(TInt aMethodNum, TInt aTestNum, TTestMode aTestMode);

protected:
	CUnitTestBase(const TDesC& aUnitTestName,const TDesC& aLogFileName,
				  const TDesC& aTestSpecName,const TDesC& aTestScriptName);
	void ConstructL();

	TInt GetRequestOrTimeOut(TRequestStatus& aRequestStatus,
		TTimeIntervalMicroSeconds32 aTimeOut);

	//Derived classes must implement this function
	//which is just simply a switch statement to
	//run their specific tests according to API and test numbers
	virtual void RunTestsL(TBool aEndInd,TTestMode aTestMode,
						   TInt& aErrCode, TInt& aLeaveCode,
		                   TInt aApiNumber,TInt aTestNumber)=0;
	//this function is to be called within RunTestsL() in the default case
	//of the switch statement
	void ActionNoMatch(TBool aEndInd,TTestMode aTestMode,
					   TInt aApiNumber,TInt aTestNumber);

	// overload this function if you want to show what test is what number
	virtual void PrintTestSpecIdentifiers();
	// overload this function to show detail of the tests
	virtual void PrintTestDetails(TInt aTstSpecNumber);

	// Should the base class automatically connect to the Etel server. Overload if not
	virtual TBool HasTestRangesDefined();

	void RequestIncomingCallL(RMobileCall& aCall);
	TBool DoTestsL();

protected:
	virtual TInt PostTestDelayTime();     ///< configurable delay time after each test
	inline void LeaveIfPhoneAlreadyOpenL();

private:
	void GetCompilationDate(TInt& aDay, TInt& aMonth, TInt& aYear);
	void EnsureTargetDeviceClockIsSet();  ///< prompt user for date if Assabet clock not set
	void ReadStr(TDes& aDes);

	TBool DoAutoMethodsL();
	TBool DoAutoMethodTestsL(TInt aMethodNum);
	TBool DoTestsInSpecifiedRangeL(TInt aStartMethodNum,
									TInt aStartTestNum,
									TInt aEndMethodNum,
									TInt aEndTestNum);

	//DoTestsL() calls SelectTestL(), RunTestsL() and LogTestResult()

	TBool SelectTestL(TInt& aApiNum,TInt& aTestNum,
						TInt& aEndApiNum, TInt& aEndTestNum,
						TTestMode& aTestMode, TBool& aExit);

	//These functions are called within SelectTestL()
	TInt  UserChoice(TChar aNumberKey);
	TBool UserSelectionL(TChar aMainNumber,
						TInt aNumMethods,
						TBool aRangeSpecified,
						TInt& aApiNum, TInt& aTestNum,
						TTestMode& aTestMode);




	//process command line params
	TBool CommandLineL(TInt& aApiNum, TInt& aTestNum,
						TInt& aEndApiNum, TInt& aEndTestNum);

	void LogTestResult(TInt aErrCode, TInt aLeaveCode, TBool aEndInd,
		               TInt aApiNumber,TInt aTestNumber,TTestMode aTestMode);

	TInt WriteLog(TBool aIsPassed,TUint aFunIndex,TUint aApiNum,
		          TUint aTstNum, TInt aErrorCode);
	void FormDateTimeBit(TDes8& aDateTimeField,TUint aDateTimeBit,
		                 TBool aIsDateSepa,TBool aAddSepa);

private:
	RCommServ   iCommServer;

	TBuf<KMaxTestTelNumberSize> iTelNumConnect1;
	TBuf<KMaxTestTelNumberSize> iTelNumConnect2;
	TBuf<KMaxTestTelNumberSize> iTelNumConnect3;
	TBuf<KMaxTestTelNumberSize> iTelNumConnect4;
	TBuf<KMaxTestTelNumberSize> iTelNumConnect5;
	TBuf<KMaxTestTelNumberSize> iTelNumConnect6;
	TBuf<KMaxTestTelNumberSize> iTelNumConnect7;
	TBuf<KMaxTestTelNumberSize> iTelNumMobile1;
	TBuf<KMaxTestTelNumberSize> iTelNumDataConnect;
	TBuf<KMaxTestTelNumberSize> iTelNumNeverConnect;
	TBuf<KMaxTestTelNumberSize> iTelNumRejectCall;
	TBuf<KMaxTestTelNumberSize> iTelNumRemoteEndHangUp;
	TBuf<KMaxTestTelNumberSize> iTelNumIncorrect1;
	TBuf<KMaxTestTelNumberSize> iTelNumIncorrect2;
	TBuf<KMaxTestTelNumberSize> iTelNumDTMFTest;
	TBuf<KMaxTestTelNumberSize> iTelNumCallback;
	TBuf<KMaxTestTelNumberSize> iTelNumDataServer1;
	TBuf<KMaxTestTelNumberSize> iTelNumDataServer2;
	TBuf<KMaxTestTelNumberSize> iTelNumDataServer3;
	TBuf<KMaxTestTelNumberSize> iTelNumDataServer4;
	TBuf<KMaxTestTelNumberSize> iSmsDestNumber;
	TBuf<KMaxTestTelNumberSize> iTelNumberInMadCow;
	TBuf<KMaxSimNameLen>		iSmsNetworkName;
	TInt	 iTypeOfTest;

	///< Determines whether to repeat the tests that failed at the end of a full automatic run
	TBool    iRepeatFailedTests;
	///< Do we want to keep the phone open between tests?
	TBool    iKeepPhoneOpen;

	TBool iExitAfterTestsComplete;

private:
	// Another session with Etel, to be kept permanently open. Helps to test
	// bad states building up, speeds the tests up too...

	struct TTestFailure
		{
		TTestFailure(TInt aMethodNum,TInt aTestIndex) : iMethodNum(aMethodNum), iTestIndex(aTestIndex) {}
		TInt iMethodNum;
		TInt iTestIndex;
		};
	typedef RArray<TTestFailure> RFailedTestsArray;
	RFailedTestsArray iFailedTests;

protected:
	RFs iFs;
	RFile iLogFile;
	RTest		iTest;
	const TName iUnitTestName;
	const TName iLogFileName;
	const TName iTestScriptName;
	const TName iTestSpecName;
	TName iTsIdentifier;
	TName iMethodName;
	RArray<TUint> iFunIndices;
	};

//inline functions
RTest CUnitTestBase::Test() const
	{
	return iTest;
	}

void CUnitTestBase::SetTsIdentifier(const TName& aTsIdentifier)
	{
	iTsIdentifier=aTsIdentifier;
	}

void CUnitTestBase::SetMethodName(const TName& aMethodName)
	{
	iMethodName=aMethodName;
	}

void CUnitTestBase::SetFunIndex(TUint aFunIndex)
	{
	iFunIndices.Insert(aFunIndex,0);
	}

void CUnitTestBase::LeaveIfPhoneAlreadyOpenL()
	{
	if (iKeepPhoneOpen)
		User::Leave(KErrAbort);
	}

#endif//END UNITTESTBASE_H__
