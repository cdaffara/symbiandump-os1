// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Public API for GT0137 Integration Test Framework
// 
//

#ifndef __TESTFRAMEWORK_H__
#define __TESTFRAMEWORK_H__

#include <e32base.h>
#include <f32file.h>
#include <ecom/ecom.h>

#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <testframeworkipc.h>

/**
 *
 * Server name
 *
 *
 *
 */
_LIT(KTestFrameworkServerName, "TestFrameworkServer");

/**
 *
 * Server .EXE name
 *
 *
 *
 */

// Multimedia Heap Macros
#define __MM_HEAP_MARK __UHEAP_MARK;
#define __MM_HEAP_MARKEND REComSession::FinalClose(); __UHEAP_MARKEND;


_LIT(KTestFrameworkServerImg, "z:\\system\\libs\\TestFrameworkServer.exe"); //<DLL/EXE name

/**
 *
 * Server UID - MUST match the UID in TestFrameworkServer.mmp
 *
 *
 *
 */
const TUid KTestFrameworkServerUid3={0x101f45ef};

/**
 *
 * Server stack/heap sizes.
 * Only needed for WINS
 *
 *
 *
 */
// #if defined (__WINS__)

/**
 *
 * To specify a version when creating a session with the server
 *
 *
 *
 */
const TUint KTestFrameworkServerMajorVersionNumber = 1;
const TUint KTestFrameworkServerMinorVersionNumber = 1;
const TUint KTestFrameworkServerBuildVersionNumber = 1;


/**
 *
 * Opcodes used in message passing between client and server
 *
 *
 *
 */
enum TTestFrameworkRequest
    {
    ECreateInputWindow,
    ENotifyIfWindowChange,
    ECancelNotifyIfWindowChange,
    EOpenLog,
    EWriteLog,
    ECloseLog,
    ELogStatus,
    };

/**
 *
 * Bitmask flags for logging mode.
 *
 *
 *
 */
enum TTestFrameworkLogMode
    {
    ELogToConsole = 0x0001,
    ELogToFile = 0x0002,
    ELogToPort = 0x0004,
    ELogConsoleFull = 0x0008
    };

/**
 *
 * For passing window dimensions to console windows.
 *
 *
 *
 */
typedef TPckgBuf<TRect> TRectBuf;

// Logging includes

/** 
 *
 * Max length of log file line and source filename.
 *
 *
 *
 */
const TInt KMaxLogLineLength =  256;
const TInt KMaxLogFilenameLength = 200;

/**
 *
 * Bitmask flags for logging severity levels.
 *
 *
 *
 */
enum TSeverity
    {
    ESevrErr  = 0x0001,
    ESevrWarn = 0x0002,
    ESevrInfo = 0x0004,
    ESevrVer  = 0x0007,     // always log verdicts
    ESevrAll  = 0x0007
    };

/**
 *
 * Static helper class for TSeverity.
 *
 *
 *
 */
class LogSeverity
    {
public:
    IMPORT_C static TBool IsValid(TInt aSev);
    IMPORT_C static TBool IsActive(TInt aThisSev, TInt aGlobalSev);
    };

/**
 *
 * Verdict enumerators for test results.
 *
 *
 *
 */
enum TVerdict
    {
    EPass = 0,
    EFail,
    EInconclusive,
    ETestSuiteError,
    EAbort,

    //A new TVerdict for a known failed test which cannot be fixed in the near 
    //future, and is not allowed to be removed from the suite.
    EKnownFailure 
    };

/**
 *
 * Logging client session class.
 *
 *
 *
 *
 */
class RTestFrameworkClientSession : public RMmfSessionBaseX
    {
public:
    RTestFrameworkClientSession();
    TInt Connect();
    void CreateInputWindow(TRectBuf& aAllocatedWindow, TRequestStatus& aReqStat);
    void NotifyIfWindowChange(TRectBuf& aNewWindow, TRequestStatus& aReqStat);
    TInt CancelNotifyIfWindowChange();
    void OpenLog(const TDesC& aLogName, TInt aLogMode);
    void OpenLog();
    void WriteLog(const TDesC& aMsg, TInt aLogMode);
    void CloseLog();
    TInt LogStatus();
    };


/**
 *
 * Logging helper class, used to panic if format text overflows 
 * the internal buffer.
 *
 *
 *
 *
 */
class TIntegrationTestLog16Overflow :public TDes16Overflow
    {
public:
    // TDes16Overflow pure virtual
    virtual void Overflow(TDes16& aDes);
    };

/**
 *
 * Flags for logging mode.
 *
 *
 *
 */
enum TLogFlags
    {
    ELogPutSrcInfo = 0x01,
    ELogHtmlMode = 0x02
    };

 /**
 *
 * Logging class for all console/file/port output.
 *
 *
 *
 *
 *
 */
class CLog  : public CBase
    {
public:
    IMPORT_C static CLog* NewL();
    IMPORT_C static CLog* NewLC();
    IMPORT_C void Construct();
    IMPORT_C ~CLog();

    // open / close log file
    IMPORT_C void OpenLogFileL(const TDesC& aLogName, TInt aLogMode);
    IMPORT_C void OpenLogFileL();
    IMPORT_C void CloseLogFile();

    // write to log output in Printf() style
    IMPORT_C void Log(TRefByValue<const TDesC16> aFmt, ...);
    IMPORT_C void Log(TInt aSeverity, TRefByValue<const TDesC16> aFmt, ...);
    IMPORT_C void Log(TRefByValue<const TDesC16> aFmt, VA_LIST aList);

    IMPORT_C void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
        TRefByValue<const TDesC16> aFmt, VA_LIST aList);
    IMPORT_C void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
        TRefByValue<const TDesC16> aFmt, ...);
    IMPORT_C void LogResult(TVerdict aVerdict, TRefByValue<const TDesC16> aFmt, ...);

    // add some blank lines to log output
    IMPORT_C void LogBlankLine(TInt aNumber = 1);
    
    // convert an EPOC error to text
    IMPORT_C static TPtrC EpocErrorToText(TInt aError);

    // convert a test verdict to text 
    IMPORT_C static TPtrC TestResultText(TVerdict aTestVerdict);

    // write formatted text to file - moved from CFileLogger
    IMPORT_C void WriteFormat(TRefByValue<const TDesC16> aFmt, ...);

    // write text to console with date / time stamp
    IMPORT_C void WriteLogConsole(const TDesC& aBuf);

    // get logging status from session
    IMPORT_C TInt LogStatus();

    // accessors
    IMPORT_C void SetSeverity(TInt aSeverity);
    IMPORT_C TInt Severity() const; 
    IMPORT_C void SetPutSrcInfo(TBool aPutSrcInfo);
    IMPORT_C void SetHtmlLogMode(TBool aArg);
    IMPORT_C TBool HtmlLogMode() const;
    
private:
    // the client session
    RTestFrameworkClientSession iClientSession;

protected:
    // Current server logging status
    TInt iLogStatus;
    // Severity level
    TInt iSeverity;
    // Do we need to put information about source file & #line? Default is yes.
    // Do we need to produce HTML log file? Default is yes.
    TUint iLogFlags;

    // data buffer for logging - avoids panics in alloc testing
    TBuf16<KMaxLogLineLength> iDataBuf;
    };


/**
 *
 * Print formatting macros.
 * Any class which implements LogExtra() can use these.
 *
 *
 *
 */
#define __FILE8__                                   REINTERPRET_CAST(const TText8*, __FILE__)

#define INFO_PRINTF1(p1)                            LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1)) 
#define INFO_PRINTF2(p1, p2)                        LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1), (p2)) 
#define INFO_PRINTF3(p1, p2, p3)                    LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1), (p2), (p3)) 
#define INFO_PRINTF4(p1, p2, p3, p4)                LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1), (p2), (p3), (p4)) 
#define INFO_PRINTF5(p1, p2, p3, p4, p5)            LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5)) 
#define INFO_PRINTF6(p1, p2, p3, p4, p5, p6)        LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define INFO_PRINTF7(p1, p2, p3, p4, p5, p6, p7)    LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7)) 
#define INFO_PRINTF8(p1, p2, p3, p4, p5, p6, p7, p8)        LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8)) 
#define INFO_PRINTF9(p1, p2, p3, p4, p5, p6, p7, p8, p9)    LogExtra(__FILE8__, __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9)) 

#define WARN_PRINTF1(p1)                            LogExtra(__FILE8__, __LINE__, ESevrWarn, (p1)) 
#define WARN_PRINTF2(p1, p2)                        LogExtra(__FILE8__, __LINE__, ESevrWarn, (p1), (p2)) 
#define WARN_PRINTF3(p1, p2, p3)                    LogExtra(__FILE8__, __LINE__, ESevrWarn, (p1), (p2), (p3)) 
#define WARN_PRINTF4(p1, p2, p3, p4)                LogExtra(__FILE8__, __LINE__, ESevrWarn, (p1), (p2), (p3), (p4)) 
#define WARN_PRINTF5(p1, p2, p3, p4, p5)            LogExtra(__FILE8__, __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5)) 
#define WARN_PRINTF6(p1, p2, p3, p4, p5, p6)        LogExtra(__FILE8__, __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define WARN_PRINTF7(p1, p2, p3, p4, p5, p6, p7)    LogExtra(__FILE8__, __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6), (p7)) 

#define ERR_PRINTF1(p1)                             LogExtra(__FILE8__, __LINE__, ESevrErr, (p1)) 
#define ERR_PRINTF2(p1, p2)                         LogExtra(__FILE8__, __LINE__, ESevrErr, (p1), (p2)) 
#define ERR_PRINTF3(p1, p2, p3)                     LogExtra(__FILE8__, __LINE__, ESevrErr, (p1), (p2), (p3)) ;
#define ERR_PRINTF4(p1, p2, p3, p4)                 LogExtra(__FILE8__, __LINE__, ESevrErr, (p1), (p2), (p3), (p4)) 
#define ERR_PRINTF5(p1, p2, p3, p4, p5)             LogExtra(__FILE8__, __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5)) 
#define ERR_PRINTF6(p1, p2, p3, p4, p5, p6)         LogExtra(__FILE8__, __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define ERR_PRINTF7(p1, p2, p3, p4, p5, p6, p7)     LogExtra(__FILE8__, __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6), (p7)) 

#define TEST_START(p1)                              LogExtra(__FILE8__, __LINE__, ESevrInfo, (_L("======Start test %S")), (p1)) 
#define TEST_NEXT(p1)                               LogExtra(__FILE8__, __LINE__, ESevrInfo, _L("Next test %S"), (p1)) 
#define TEST_END()                                  LogExtra(__FILE8__, __LINE__, ESevrInfo, _L("======End test =====") ) 

// Test Suite includes

/**
 *
 * Max length of test suite name.
 *
 *
 *
 */
const TInt KMaxLenTestSuiteName = 55;

class RTestStep;

/**
 *
 * Enumerator to indicate what stage a test has reached.
 *
 *
 *
 */
enum TTestStepStatus
    {
    EStepStatusNone = -1,
    EStepStatusStart = 0,
    EStepStatusPreamble,
    EStepStatusTest,
    EStepStatusFinished
    };

/**
 *
 * Base test suite class. All test suite classes
 * derive from this.
 *
 *
 *
 *
 */
class CTestSuite : public CBase
    {
public:
    // second phase constructor
    // NB this is public because it is called by the factory function
    IMPORT_C void ConstructL();
    // destructor
    IMPORT_C virtual ~CTestSuite();
    // add a test step to the suite
    IMPORT_C void AddTestStepL(RTestStep* aTestStep);
    // get stack and heap size of a named step
    IMPORT_C void GetHeapAndStackSize(const TDesC& aStep, TInt* aHeapSize, TInt* aStackSize);
    // public interface to run test steps
    IMPORT_C TVerdict DoTestStep(const TDesC& aStep, const TDesC& aConfig, const TDesC& aPAramSet);

/**
 *
 * Test suite second phase initialiser, called from CTestSuite::ConstructL().
 * All test suites must implement this.
 * 
 *
 *
 */
    virtual void InitialiseL() = 0;

    IMPORT_C void Log(TRefByValue<const TDesC16> aFmt, ...);
    IMPORT_C void Log(TInt aSeverity, TRefByValue<const TDesC16> aFmt, ...);
    IMPORT_C void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
        TRefByValue<const TDesC16> aFmt, ...);

    // this should be a pure virtual so every test DLL
    // has to provide a version, but for now defaults to ?.?
    IMPORT_C virtual TPtrC GetVersion() const;

    // accessors
    IMPORT_C void SetSeverity(TInt aSeverity);
    IMPORT_C TInt Severity() const;
    IMPORT_C void SetStepStatus(TTestStepStatus aStatus);
    IMPORT_C TTestStepStatus StepStatus() const;
    IMPORT_C void SetLogSystem(CLog* aLogger);
    IMPORT_C CLog* LogSystem() const;

protected:
    // test functions
    IMPORT_C void TestBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine);

private:
    // severity level
    TInt iSeverity;
    // File logging system
    CLog* iLogger;
    // array of pointers to the test steps in this suite
    CArrayPtr<RTestStep>* iArrayTestSteps;
    // status of current test step
    TTestStepStatus iStepStatus;

protected:
    // the name of this suite
    TBuf<KMaxLenTestSuiteName> iSuiteName;

    };

// Test Step includes

/**
 *
 * Max length of step name
 *
 *
 *
 */
const TInt KMaxLenTestStepName = 55;

class CTestIniData;

/**
 *
 * Default stack and heap sizes for test step
 *
 *
 *
 */
const TInt KTestStackSize = KDefaultStackSize;  // 0x2000 = 8k
const TInt KMaxTestThreadHeapSize = 0x10000;    // = 64k

/**
 *
 * Base test step class. All test step classes
 * derive from this
 *
 *
 *
 *
 */
class RTestStep
    {
public:

    IMPORT_C RTestStep();

    IMPORT_C TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW;
    IMPORT_C TAny* operator new(TUint aSize, TLeave);
    IMPORT_C TAny* operator new(TUint aSize) __NO_THROW;
    IMPORT_C static TAny* newL(TUint aSize);
    IMPORT_C TAny* operator new(TUint aSize,TUint anExtraSize) __NO_THROW;

    IMPORT_C void PreOpenL();
    IMPORT_C virtual TVerdict OpenL();
    IMPORT_C virtual void CleanupAfterOpenFail();
    IMPORT_C virtual void Close();

/**
 *
 * Perform the test step.
 *
 * @return  "TVerdict"
 *          The result of the test step.
 *
 *
 *
 */
    virtual TVerdict DoTestStepL() = 0;
    
    // initialise the config parameter system
    IMPORT_C void LoadConfig(const TDesC& aConfig);
    IMPORT_C void UnloadConfig();

    // accessors for stack / heap size
    IMPORT_C TInt StackSize() const;
    IMPORT_C TInt HeapSize() const;

    IMPORT_C void SetSuite(CTestSuite* aSuite);
    IMPORT_C void SetResult(TVerdict aResult);
    IMPORT_C TPtrC StepName() const;
    
    // param set - not required to be exported. Note assumes actual param is stable
    void SetDefaultParamSet(const TDesC& aParamSet);

protected:

    // tests may optionally implement pre- and post-ambles
    IMPORT_C virtual TVerdict DoTestStepPreambleL();
    IMPORT_C virtual TVerdict DoTestStepPostambleL();

    // read values from the Config file functions
    IMPORT_C TBool GetBoolFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TBool& aResult);
    IMPORT_C TBool GetIntFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TInt& aResult);
    IMPORT_C TBool GetStringFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TPtrC& aResult);
    IMPORT_C TBool GetHexFromConfig(const TDesC &aSectName,const TDesC &aKeyName,TInt &aResult);

    // test functions
    IMPORT_C void TestBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine);
    IMPORT_C void TestBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine);
    IMPORT_C void TestBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine);
    IMPORT_C void TestBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine);
    IMPORT_C void TestCheckPointCompareL(TInt aVal, TInt aExpectedVal, 
                                         const TDesC& aText, const TText8* aFile, TInt aLine);

    // printf format log
    IMPORT_C void Log(TRefByValue<const TDesC16> aFmt, ...);
    IMPORT_C void Log(TInt aSeverity, TRefByValue<const TDesC16> aFmt, ...);
    IMPORT_C void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
        TRefByValue<const TDesC16> aFmt,...);

private:
    // true if ConfigData has been loaded
    TBool   iConfigDataAvailable;

protected:
    // the name of the test step
    TBuf<KMaxLenTestStepName> iTestStepName;
    // the parameter set to use - supplied to the run_test statement
    TPtrC iDefaultParamSet;
    // pointer to suite which owns this test 
    CTestSuite* iSuite;
    // the current test step verdict
    TVerdict iTestStepResult;
    // config file data
    CTestIniData *iConfigData;
    // stack and heap size
    TInt iStackSize;
    TInt iHeapSize;
    };
    
class CAsyncTestActiveScheduler; 
    
/**
Sub-class of RTestStep that encapsulates CActiveScheduler and works asynchonously
*/

class RAsyncTestStep : public RTestStep
    {
    friend class CAsyncTestActiveScheduler;
protected:
    IMPORT_C RAsyncTestStep();
    
    // from RTestStep - these should not be replaced by RAsyncTestStep children
    IMPORT_C TVerdict DoTestStepL();
    IMPORT_C TVerdict DoTestStepPostambleL();
    
    // new methods
    /**
    Start the test
    This method is used at the beginning of the test, and should initialise whatever
    is required - typically setting up asynchronous calls that will activate once
    the call returns. The method is abstract, and must be supplied by any derived
    class.
    The ActiveScheduler is active (this call is made from a RunL() instance).
    If this test leaves, then StopTest() will be called with the leave value, so
    implicitly the test stops.
    */
    virtual void KickoffTestL() = 0;
    /**
    Called after the test has stopped to workout the result.
    This can be replaced by derived classes, if they have specific requirements, but
    the default should be suitable for most uses. The default returns the result
    set by StopTest() - if this is not pass, this result will also be written to the log.
    */
    IMPORT_C virtual TVerdict CheckTestResult();
    /**
    Close test.
    This call requests the test step to delete any remaining objects left when running
    a test. It is always called following the test being stopped.
    */
    virtual void CloseTest() = 0;
    
    // new commands
    /**
    Stop the test.
    This is called by the client to stop a test - calling CActiveScheduler::Stop() on the
    embedded active scheduler object. aReason is a standard error code. It defaults to
    KErrNone. aResult is the verdict to use as the result of the test - its default depends
    on aReason, EPass if the reason is KErrNone and EFail otherwise. 
    Note that StopTest() can be called multiple times - in which case only the first non-KErrNone
    reason and the first non-EPass result are used. */
    IMPORT_C void StopTest();
    IMPORT_C void StopTest(TInt aReason);
    IMPORT_C void StopTest(TInt aReason, TVerdict aResult);
    
    // these two are intended for overrides of CheckTestResult()
    /**
    The aReason value set by StopTest()
    */
    IMPORT_C TInt Reason() const;
    /**
    The aResult value set by StopTest()
    */
    IMPORT_C TVerdict Result() const;
    
private:
    CActiveScheduler* iScheduler;
    
    void SetResult(TVerdict aResult);
    
    static TInt CallBack (TAny* aPtr);
    void DoCallBack();
    
    void HandleError(TInt aReason);
    
private:
// have copies of these here, as they are commonly required
    TInt iReason;
    TVerdict iResult;
    TBool iResultSet;

    CAsyncCallBack* iStartAO;
    CActiveSchedulerWait* iActiveSchedulerWait;
    TBool iStarted;
    };

/**
 *
 * CTestStep, thin subclass of RTestStep - provided for
 * backward compatibility
 *
 *
 *
 *
 */
class CTestStep : public RTestStep
    {
public :
    // CTestStep destructor - provided for backward compatibility ONLY
    IMPORT_C virtual ~CTestStep();
    };

/**
 *
 * Autotest macros mapped to RTestStep functions
 *
 *
 *
 *
 */
// check a boolean is true
#define TESTL(a) TestBooleanTrueL((a), __FILE8__, __LINE__) 
#define TEST(a)  TestBooleanTrue((a), __FILE8__, __LINE__) 

// check a boolean is true if not return error code b
#define TESTE(a, b) TestBooleanTrueWithErrorCode((a), (b), __FILE8__, __LINE__) 
#define TESTEL(a, b) TestBooleanTrueWithErrorCodeL((a), (b), __FILE8__, __LINE__)  
#define TEST_CHECKL(p1, p2, p3) TestCheckPointCompareL((p1), (p2), (p3), __FILE8__, __LINE__)   
// leave error code
const TInt KTestErrorCode = 84; 

/**
 *
 * Test utilities class
 *
 *
 *
 *
 */
class CTestUtils : public CBase
    {
public:

    IMPORT_C  static CTestUtils* NewL(CLog* aLogSystem);
    IMPORT_C  void RunUtils(const TDesC& aText);

private:
    void Construct(CLog* aLogSystem);
    void RunUtilsL(const TDesC& aText);

    void MakeDirL (const TDesC& aDirname);
    void CopyFileL (const TDesC& aOld,const TDesC& aNew); 
    void CopyAndInvertFileL (const TDesC& aOld,const TDesC& aNew);
    void DeleteFileL (const TDesC& aFile); 
    void MakeReadWriteL(const TDesC& aFile);

    // printf format log
    void Log(TRefByValue<const TDesC16> aFmt, ...);
    void Log(TInt aSeverity, TRefByValue<const TDesC16> aFmt, ...);
    void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
        TRefByValue<const TDesC16> aFmt,...);

private:

    // called by CopyAndInvertFileL func
    void DoCopyAndInvertL (const TDesC& aOld,const TDesC& aNew);
    
    // pointer to Logging object which handles File and Console logging
    CLog* iLogSystem;
    // local file server
    RFs iFs;
    };
    
/*
Busy unit - once started, uses a log of CPU time.
*/

NONSHARABLE_CLASS(CBusyTestUnit) : public CBase
    {
    CBusyTestUnit();
    CBusyTestUnit(TInt aPercentBusy, TThreadPriority aThreadPriority);
    void ConstructL();
    
public:
    ~CBusyTestUnit();
    
    IMPORT_C static CBusyTestUnit* NewL(TInt aPercentBusy, TThreadPriority aThreadPriority);
    IMPORT_C static CBusyTestUnit* NewLC(TInt aPercentBusy, TThreadPriority aThreadPriority);
    
    IMPORT_C TInt Start(); // run until Stop()
    IMPORT_C TInt Start(TTimeIntervalMicroSeconds aRunFor); 
            // run til Stop or aRunFor. aRunFor=0 means run until Stop()
    IMPORT_C TInt Start(TTimeIntervalMicroSeconds32 aDelayFor, TTimeIntervalMicroSeconds aRunFor); 
            // after aDelayFor, run til Stop or aRunFor
    
    IMPORT_C void Stop();
    
    
private:
    TInt RunThread();
    static TInt StartThread(TAny* aPtr);
    void ThreadFunction();
    
    static TInt StaticTimerCallback(TAny* aPtr);
    TInt TimerCallback();
    
private:
    const TInt iPercentBusy;
    const TThreadPriority iThreadPriority;
    TTimeIntervalMicroSeconds iRunFor;
    CPeriodic* iTimer;
    volatile TInt iBusyVariable;
    RThread iChildThread;
    };


#endif // __TESTFRAMEWORK_H__
