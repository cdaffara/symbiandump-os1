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
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TestIniData.h"
#include "asyncscheduler.h"

// do not export if Unit Testing
#if defined (__TSU_TESTFRAMEWORK__)
#undef EXPORT_C
#define EXPORT_C
#endif

static const TUint16*const KVerdictString[] = // must match up with TVerdict
	{
	_S("EPass"),
	_S("EFail"),
	_S("EInconclusive"),
	_S("ETestSuiteError"),
	_S("EAbort"),
	_S("EKnownFailure")
	};

/**
 *
 * Test step constructor.
 *
 * @xxxx
 *
 */
EXPORT_C RTestStep::RTestStep()
	{
	iTestStepResult = EPass;
	iTestStepName.Zero();
	iSuite = NULL;
	iConfigData = NULL;
	iConfigDataAvailable = EFalse;
	iStackSize = KTestStackSize;
	iHeapSize = KMaxTestThreadHeapSize;

	// NB : RTestStep has no destructor
	}

/**
 *
 * CBase compatibility functionality.
 *
 * @xxxx
 *
 */


EXPORT_C TAny* RTestStep::operator new(TUint aSize, TAny* aBase) __NO_THROW
	{
	Mem::FillZ(aBase,aSize);
	return(aBase);
	}


EXPORT_C TAny* RTestStep::operator new(TUint aSize, TLeave)
	{
	return newL(aSize);		// will leave on alloc failure
	}

EXPORT_C TAny* RTestStep::operator new(TUint aSize) __NO_THROW
	{
	TAny* pM=User::Alloc(aSize);
	if (pM)
		Mem::FillZ(pM,aSize);
	return(pM);
	}

EXPORT_C TAny* RTestStep::newL(TUint aSize)
	{
	TAny* pM=User::AllocL(aSize);
	Mem::FillZ(pM,aSize);
	return pM;
	}

EXPORT_C TAny* RTestStep::operator new(TUint aSize,TUint anExtraSize) __NO_THROW
	{
	aSize+=anExtraSize;
	TAny *pM=User::Alloc(aSize);
	if (pM)
		Mem::FillZ(pM,aSize);
	return(pM);
	}

/**
 *
 * Pre-preamble for all test steps. This grows the cleanup stack to
 * allow for allocation errors.
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::PreOpenL()
	{
	TAny* dummyPtr = NULL;
	for(TInt i = 0 ; i < 20 ; i++)
		CleanupStack::PushL(dummyPtr); // Grow the cleanup stack.
	CleanupStack::PopAndDestroy(20);
	}

/**
 *
 * Default empty implementation of OpenL (preamble).
 * Test steps can override this to implement required code.
 *
 * @return	"TVerdict"
 *			Result of preamble (should only be EPass or EInconclusive)
 *
 * @xxxx
 *
 */
EXPORT_C TVerdict RTestStep::OpenL()
	{
	// for backward compatibility with CTestStep
	return DoTestStepPreambleL();
	}

/**
 *
 * Default empty implementation of CleanupAfterOpenFail (preamble cleanup).
 * Test steps can override this to implement required code.
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::CleanupAfterOpenFail()
	{
	// default empty implementation
	// a step should implement its own method if required
	}

/**
 *
 * Default empty implementation of Close (postamble)
 * Test steps can override this to implement required code.
 *
 * NB this does NOT leave - any leaves should be trapped and panicked.
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::Close()
	{
	// for backward compatibility with CTestStep
	_LIT(KPanicStr, "RTestStep::Close");
	TVerdict ver = EPass;
	TRAPD(err, ver = DoTestStepPostambleL());
	if(err != KErrNone)
		User::Panic(KPanicStr, 0);
	// any DoTestStepPostambleL() which returns EInconclusive should be panicked
 	if(ver != EPass)
		User::Panic(KPanicStr, 1);

	}

/**
 *
 * Set the test suite
 *
 * @param	"CTestSuite*"
 *			The test suite
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::SetSuite(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	}

/**
 *
 * Set the test result
 *
 * @param	"TVerdict"
 *			The test result
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::SetResult(TVerdict aResult)
	{
	iTestStepResult = aResult;
	}

/**
 *
 * Get the step name
 *
 * @return	"TPtrC"
 *			The step name
 *
 * @xxxx
 *
 */
EXPORT_C TPtrC RTestStep::StepName() const
	{
	return iTestStepName;
	}

/**
 *
 * General logging function for test steps.
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style aFmt.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::Log(TRefByValue<const TDesC16> aFmt, ...)
	{
    
	VA_LIST aList;
	VA_START(aList, aFmt);

	TIntegrationTestLog16Overflow iOverflow16;

	// decode formatted data for display on console
	TBuf<KMaxLogLineLength> lineBuf;
	lineBuf.AppendFormatList(aFmt, aList, &iOverflow16);

	// send the data to the log system via the suite
	iSuite->Log(_L("%S"),&lineBuf);

	VA_END(aList); 

	}

/**
 *
 * General logging function for test steps, with severity.
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style aFmt.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::Log( TInt aSeverity, TRefByValue<const TDesC16> aFmt, ... )
{
	VA_LIST aList;
	VA_START(aList, aFmt);

	TIntegrationTestLog16Overflow iOverflow16;

	// decode formatted data for display on console
	TBuf<KMaxLogLineLength> lineBuf;
	lineBuf.AppendFormatList(aFmt, aList, &iOverflow16);

	// send the data to the log system via the suite
	if(LogSeverity::IsActive(aSeverity, iSuite->Severity()))
		iSuite->Log(aSeverity, _L("%S"),&lineBuf);

	VA_END(aList); 
}

/**
 *
 * Traceable logging function for test steps.
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);

	TIntegrationTestLog16Overflow iOverflow16;

	// decode formatted data for display on console
	TBuf<KMaxLogLineLength> lineBuf;
	lineBuf.AppendFormatList(aFmt, aList, &iOverflow16);

	// send the data to the log system via the suite
	if(LogSeverity::IsActive(aSeverity, iSuite->Severity()))
		iSuite->LogExtra(aFile, aLine, aSeverity, lineBuf);

	VA_END(aList); 
	}
	
/**
Set default paramSet
Test steps can use this when looking up configs, to provide a level of script control
*/
void RTestStep::SetDefaultParamSet(const TDesC& aParamSet)
	{
	iDefaultParamSet.Set(aParamSet);
	}

/**
 *
 * Load a configuration file.
 * If successful, data member iConfigDataAvailable is set.
 *
 * @param	"TPtrC aConfig"
 *			The configuration file name.
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::LoadConfig(const TDesC& aConfig)
	{

	// if a config file supplied then use
	if (aConfig.Length() != 0)
		{

		// get the full pathname default drive name and extension
		_LIT(KRelated,"C:\\config.ini"); 
		TParse configFileName;
		TInt returnCode = configFileName.Set(aConfig, &KRelated, NULL);

		if (returnCode != KErrNone)
			{
			// error opening FileManager
			ERR_PRINTF2(_L("Error opening config file %S"), &(configFileName.FullName())); 
			}

		// create and load the CTestIniData object
		TRAPD(r, iConfigData = CTestIniData::NewL(configFileName.FullName()));
		
		// check if loaded ok
		if (r == KErrNone)
			{
			// loaded ok
			iConfigDataAvailable = ETrue;
			}
		else
			{
			// failed to load
			iConfigDataAvailable = EFalse;
			iConfigData = NULL;

			// report error 
			TPtrC errortxt = CLog::EpocErrorToText(r);
			ERR_PRINTF2(_L("Failed to load config data file - error %S"), &errortxt);
			}
		}
	}

/**
 *
 * Unload any existing configuration data.
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::UnloadConfig()
	{
	iConfigDataAvailable = EFalse;

	// clean up Config data object
	delete iConfigData;
	iConfigData = NULL;

	}

/**
 *
 * Check the config file for a boolean value
 *
 * @param	"const TDesC &aSectName"
 *			Section name to check in
 *
 * @param	"const TDesC &aKeyName"
 *			Key name to check for
 *
 * @param	"Bool &aResult"
 *			TBool returned from config file
 *
 * @return	"TBool"
 *			Result (ETrue if found)
 *
 * @xxxx
 *
 */
EXPORT_C TBool RTestStep::GetBoolFromConfig(const TDesC &aSectName, const TDesC &aKeyName, TBool &aResult)
	{
	// check file available
	if (!iConfigDataAvailable)
		{
		ERR_PRINTF1(_L("No config file available"));
		return EFalse;
		}

	TBool ret = EFalse;
	TPtrC result;

	// get the value 
	ret = iConfigData->FindVar(aSectName, aKeyName, result);

	// if failed to decode display error
	if (!ret) 
		{
		// display error message
		ERR_PRINTF3(_L("Failed to read section:%S key:%S "),
				&aSectName, &aKeyName );

		// return fail
		return EFalse;
		}

	// set result as a TBool
	if (result.FindF(_L("true")) == KErrNotFound)
		aResult = EFalse;
	else
		aResult = ETrue;
	// return success
	return ETrue;
	}

/**
 *
 * Check the config file for a TInt value
 *
 * @param	"const TDesC &aSectName"
 *			Section name to check in
 *
 * @param	"const TDesC &aKeyName"
 *			Key name to check for
 *
 * @param	"TInt &aResult"
 *			TInt returned from config file
 *
 * @return	"TBool"
 *			Result (ETrue if found)
 *
 * @xxxx
 *
 */
EXPORT_C TBool RTestStep::GetIntFromConfig(const TDesC &aSectName, const TDesC &aKeyName, TInt &aResult)
	{
	// check file available
	if ( !iConfigDataAvailable )
		{
		ERR_PRINTF1(_L("No config file available"));
		return EFalse;
		}	

	TBool ret = EFalse;
	TPtrC result;

	// get the value 
	ret = iConfigData->FindVar(aSectName, aKeyName, result);

	// if failed to decode display error
	if (!ret) 
		{
		// display error message
		ERR_PRINTF3(_L("Failed to read section:%S key:%S "),
				&aSectName, &aKeyName );

		// return fail
		return EFalse;
		}

	// use TLex to convert to a TInt
	TLex lex(result);
	if (lex.Val(aResult) == KErrNone)
		return ETrue;
	else
		return EFalse;
}

/**
 *
 * Check the config file for a string TPtr value
 *
 * @param	"const TDesC &aSectName"
 *			Section name to check in
 *
 * @param	"const TDesC &aKeyName"
 *			Key name to check for
 *
 * @param	"TPtrC &aResult"
 *			String returned from config file
 *
 * @return	"TBool"
 *			Result (ETrue if found)
 *
 * @xxxx
 *
 */
EXPORT_C TBool RTestStep::GetStringFromConfig(const TDesC &aSectName, const TDesC &aKeyName, TPtrC &aResult)
	{
	// check file available
	if (!iConfigDataAvailable)
		{
		ERR_PRINTF1(_L("No config file available"));
		return EFalse;
		}	

	// get the value 
	TBool ret = iConfigData->FindVar(aSectName, aKeyName, aResult);

	// if failed to decode display error
	if (ret == EFalse) 
		{
		ERR_PRINTF3(_L("Failed to read section:%S key:%S "),
				&aSectName, &aKeyName );
		}

	return ret;
}

/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TInt reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The integer value of the Hex input
 * @return TBool - ETrue for found, EFalse for not found 
 */	
EXPORT_C TBool RTestStep::GetHexFromConfig(const TDesC &aSectName,const TDesC &aKeyName,TInt &aResult)
	{
	TPtrC result;
	if(!iConfigData)
		return EFalse;
	if(!iConfigData->FindVar(aSectName, aKeyName, result))
		return EFalse;
	TLex lex(result);
	TInt err = lex.Val((TUint &)aResult, EHex);
	if(err)
		return EFalse;
	
	return(ETrue);
	}
	
/**
 *
 * Default empty implementation of DoTestStepPreambleL.
 * Test steps can override this to implement required code.
 *
 * @return	"TVerdict"
 *			Result of preamble (should only be EPass or EInconclusive)
 *
 * @xxxx
 *
 */
// for backward compatibility with CTestStep
EXPORT_C TVerdict RTestStep::DoTestStepPreambleL()
	{
	return EPass;
	}

/**
 *
 * Default empty implementation of DoTestStepPostambleL.
 * Test steps can override this to implement required code.
 *
 * @return	"TVerdict"
 *			Result of postamble (should only be EPass or EInconclusive)
 *
 * @xxxx
 *
 */
// for backward compatibility with CTestStep
EXPORT_C TVerdict RTestStep::DoTestStepPostambleL()
	{
	return EPass;
	}

/**
 *
 * Traceable Boolean condition tester.
 * If the condition is not true, record an error.
 *
 * @param	"TBool aCondition"
 *			Condition to be checked
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::TestBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine)
	{

	// check condition
	if (aCondition)
		return;

	// this is only relevant if the current result is pass
	if (iTestStepResult == EPass)
		iTestStepResult = EFail;

	// convert filename for log
	TBuf<KMaxLogFilenameLength> fileName;
	TPtrC8 fileName8(aFile);
	fileName.Copy(fileName8);  // TText8->TBuf16

	// display a log message
 	ERR_PRINTF3(_L("Test Failed in file:%S line:%d"), &fileName, aLine);

	}

/**
 *
 * Traceable Boolean condition tester.
 * If the condition is not true, record an error and leave.
 *
 * @param	"TBool aCondition"
 *			Condition to be checked
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::TestBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine)
	{

	// check condition
	if (aCondition)
		return;

	// this is only relevant if the current result is pass
	if (iTestStepResult == EPass)
		iTestStepResult = EFail;

	// convert filename for log
	TBuf<KMaxLogFilenameLength> fileName;
	TPtrC8 fileName8(aFile);
	fileName.Copy(fileName8);  // TText8->TBuf16

	// display a log message
 	ERR_PRINTF3(_L("Test Failed in file:%S line:%d"), &fileName, aLine);

	// leave with error code
	User::Leave(KTestErrorCode);

	}

/**
 *
 * Traceable Boolean condition tester.
 * If the condition is not true, record an error with the supplied 
 * error code, and leave.
 *
 * @param	"TBool aCondition"
 *			Condition to be checked
 *
 * @param	"TInt aErrorCode"
 *			Supplied error code
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::TestBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
	{
	// check condition
	if (aCondition)
		return;

	// this is only relevant if the current result is pass
	if (iTestStepResult == EPass)
		iTestStepResult = EFail;

	// convert filename for log
	TBuf<KMaxLogFilenameLength> fileName;
	TPtrC8 fileName8(aFile);
	fileName.Copy(fileName8);  // TText8->TBuf16

	// get the error text
	TPtrC errortxt = CLog::EpocErrorToText(aErrorCode);

	// display a log message
	ERR_PRINTF4(_L("Test Failed with error:%S in file:%S line:%d"),
			&errortxt, &fileName, aLine);

	// leave with error code
	User::Leave(aErrorCode);
	
	}

/**
 *
 * Traceable Boolean condition tester.
 * If the condition is not true, record an error with the supplied 
 * error code.
 *
 * @param	"TBool aCondition"
 *			Condition to be checked
 *
 * @param	"TInt aErrorCode"
 *			Supplied error code
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::TestBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
	{
	// check condition
	if (aCondition)
		return;

	// this is only relevant if the current result is pass
	if (iTestStepResult == EPass)
		iTestStepResult = EFail;

	// convert filename for log
	TBuf<KMaxLogFilenameLength> fileName;
	TPtrC8 fileName8(aFile);
	fileName.Copy(fileName8);  // TText8->TBuf16

	// get the error text
	TPtrC errortxt = CLog::EpocErrorToText(aErrorCode);

	// display a log message
	ERR_PRINTF4(_L("Test Failed with error:%S in file:%S line:%d"),
			&errortxt, &fileName, aLine);
	}

/**
 *
 * Traceable checkpoint tester.
 * If the value does not match expected, record an error with supplied
 * text string, and leave.
 *
 * @param	"TInt aVal"
 *			Value to be checked
 *
 * @param	"TInt aExpectedVal"
 *			Value expected
 *
 * @param	"const TDesC& aText"
 *			Supplied text string
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @xxxx
 *
 */
EXPORT_C void RTestStep::TestCheckPointCompareL(TInt aVal, TInt aExpectedVal, 
												  const TDesC& aText, const TText8* aFile, TInt aLine)
	{
	if(aVal != aExpectedVal)
		{
		// this is only relevant if the current result is pass
		if (iTestStepResult == EPass)
			iTestStepResult = EFail;

		// convert filename for log
		TBuf<KMaxLogFilenameLength> fileName;
		TPtrC8 fileName8(aFile);
		fileName.Copy(fileName8);  // TText8->TBuf16

		ERR_PRINTF6(_L("FAILED test:  Val = %d Exp Val = %d %S in file:%S line:%d"), 
			aVal, aExpectedVal, &aText, &fileName, aLine);

		User::Leave(aVal);
		}
	}

/** 
 *
 * Accessors for stack / heap size
 *
 * NB - These can only be set from within the RTestStep derived constructor itself - there
 * are no setter accessors.
 *
 * @xxxx
 *
 */
EXPORT_C TInt RTestStep::StackSize() const
{
	return iStackSize;
}

EXPORT_C TInt RTestStep::HeapSize() const
{
	return iHeapSize;
}


/** 
 *
 * Virtual destructor for CTestStep
 * Provided for backward compatibility ONLY
 *
 * @xxxx
 *
 */
EXPORT_C CTestStep::~CTestStep()
{
}

//
// RAsyncTestStep
//

EXPORT_C RAsyncTestStep::RAsyncTestStep():
	iReason (KErrNone),
	iResult (EPass),
	iStartAO (NULL),
	iActiveSchedulerWait (NULL),
	iStarted (EFalse)
	{
	}

EXPORT_C TVerdict RAsyncTestStep::DoTestStepL()
	{
	// allow recalls to same test step (?), so re-initialise the basic variables. Can't delete
	// as if in that case, old heap would be dead
	iReason = KErrNone;
	iResult = EPass;
	iScheduler = NULL;
	iActiveSchedulerWait = NULL;
	
	iScheduler = new (ELeave) CAsyncTestActiveScheduler(*this);	
	CActiveScheduler::Install(iScheduler);
	iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait();
	
	// arrange for DoCallBack() to be called as the first thing. Use low priority to reduce
	// the overhead of an extra AO on everything else
	TCallBack callback (CallBack, this);
	iStartAO = NULL;
	iStartAO = new (ELeave) CAsyncCallBack (callback, CActive::EPriorityIdle); 
	iStartAO->Call();
	
	iStarted = ETrue; // obviously do this before we start, as can't once it is
	iActiveSchedulerWait->Start();
	return CheckTestResult();
	}

EXPORT_C TVerdict RAsyncTestStep::DoTestStepPostambleL()
	{
	CloseTest();
	delete iStartAO; // no need to Cancel 
	iStartAO = NULL;
	delete iActiveSchedulerWait;
	iActiveSchedulerWait = NULL;
	delete iScheduler; 
	iScheduler = NULL;
	return EPass;
	}
	
EXPORT_C void RAsyncTestStep::StopTest()
	{
	StopTest(KErrNone);
	}
	
EXPORT_C void RAsyncTestStep::StopTest(TInt aReason)
	{
	TVerdict resultToUse = (aReason==KErrNone) ? EPass : EFail;
	StopTest (aReason, resultToUse);
	}

EXPORT_C void RAsyncTestStep::StopTest(TInt aReason, TVerdict aResult)
	{
	// note if stop is called multiple times, we record last
	// non-KErrNone reason and last non-Pass result, but only actually stop once
	if (aReason!=KErrNone)
		{
		iReason = aReason;		
		}
	SetResult(aResult);
	if (iStarted)
		{
		iStarted = EFalse;
		iActiveSchedulerWait->AsyncStop();
		}
	}
	
EXPORT_C TInt RAsyncTestStep::Reason() const
	{
	return iReason;
	}
	
EXPORT_C TVerdict RAsyncTestStep::Result() const
	{
	return iResult;
	}
	
void RAsyncTestStep::SetResult(TVerdict aResult)
	{
	// remember the last non-Pass result
	if (aResult!=EPass)
		{
		iResult = aResult;
		}	
	}
	
EXPORT_C TVerdict RAsyncTestStep::CheckTestResult()
	{
	TVerdict result = Result();
	
	if (result!=EPass) // if the result is a Pass, even if error is too we treat as test pass
		{
		INFO_PRINTF3(_L("Failed test with error %d, result %s"), Reason(), KVerdictString[result]);
		}
	return result;
	}

TInt RAsyncTestStep::CallBack(TAny* aPtr)
	{
	RAsyncTestStep* self = static_cast<RAsyncTestStep*> (aPtr);
	self->DoCallBack(); // if this fails, it will stop itself
	return KErrNone;
	}
	
void RAsyncTestStep::DoCallBack()
	{
	TRAPD(error, KickoffTestL()); 
	if (error!=KErrNone)
		{
		StopTest(error); // if kickoff fails, we stop here
		}	
	}

void RAsyncTestStep::HandleError(TInt aError)
	{
	INFO_PRINTF2(_L("ActiveScheduler::Error(%d)"), aError);
	StopTest(aError);
	}
	
//
// CBusyTestUnit
//

CBusyTestUnit::CBusyTestUnit():
		iPercentBusy(0), iThreadPriority(EPriorityNormal)
	{
	// non exported default constructor to enforce non-derivation
	}

CBusyTestUnit::CBusyTestUnit(TInt aPercentBusy, TThreadPriority aThreadPriority):
		iPercentBusy(aPercentBusy), iThreadPriority(aThreadPriority)
	{
	ASSERT(aPercentBusy>=0 && aPercentBusy<=100); // assume this
	}
	
void CBusyTestUnit::ConstructL()
	{
	}
	
EXPORT_C CBusyTestUnit* CBusyTestUnit::NewLC(TInt aPercentBusy, TThreadPriority aThreadPriority)
	{
	CBusyTestUnit* self = new (ELeave) CBusyTestUnit(aPercentBusy, aThreadPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
EXPORT_C CBusyTestUnit* CBusyTestUnit::NewL(TInt aPercentBusy, TThreadPriority aThreadPriority)
	{
	CBusyTestUnit* self = NewLC(aPercentBusy, aThreadPriority);
	CleanupStack::Pop(self);
	return self;
	}
	
CBusyTestUnit::~CBusyTestUnit()
	{
	Stop();
	}
	
EXPORT_C void CBusyTestUnit::Stop()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		delete iTimer;
		iTimer=NULL;
		}
	if (iChildThread.Handle())
		{
		// child thread created, so kill
		iChildThread.Kill(0);
		iChildThread.Close();
		}
	}
	
EXPORT_C TInt CBusyTestUnit::Start()
	{
	return Start(0);	
	}
	
EXPORT_C TInt CBusyTestUnit::Start(TTimeIntervalMicroSeconds aRunFor)
	{
	return Start(0, aRunFor);
	}
	
EXPORT_C TInt CBusyTestUnit::Start(TTimeIntervalMicroSeconds32 aDelayFor, TTimeIntervalMicroSeconds aRunFor)
	{
	iRunFor = aRunFor;
	if (!aDelayFor.Int())
		{
		// run immediately
		return RunThread();
		}
	else
		{
		iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
		TCallBack callback(StaticTimerCallback, this);
		iTimer->Start(aDelayFor, 0, callback);
		return KErrNone;
		}
	}
	
TInt CBusyTestUnit::StaticTimerCallback(TAny* aPtr)
	{
	CBusyTestUnit* self = static_cast<CBusyTestUnit*>(aPtr);
	return self->TimerCallback();
	}
	
TInt CBusyTestUnit::TimerCallback()
	{
	// called on timer callback, so assume iTimer!
	ASSERT(iTimer);
	// first stop and delete timer - don't need again this run
	iTimer->Cancel();
	// then kick off the thread
	TInt error = RunThread(); 
	// now delete the timer - do now as we've been called back by it!
	delete iTimer;
	iTimer = NULL;
	return error; // any error will stop the test, in theory
	}
	
TInt CBusyTestUnit::RunThread()
	{
	TAny* paramPtr = this;
	TInt error = iChildThread.Create(KNullDesC, StartThread, 
					KDefaultStackSize, NULL, paramPtr, EOwnerThread);
	if (!error)
		{
		iChildThread.SetPriority(iThreadPriority);
		iChildThread.Resume();
		}
	return error;
	}
	
TInt CBusyTestUnit::StartThread(TAny* aPtr)
	{
	CBusyTestUnit* self = static_cast<CBusyTestUnit*>(aPtr);
	if (self)
		{
		self->ThreadFunction();
		}
	return KErrNone;
	}
	
void CBusyTestUnit::ThreadFunction()
	{
	// this runs in a separate thread and tries to use lots of CPU time up to percentage
	// nominally we run busy for loops for iPercentBusy/100ms, and then wait for rest of the
	// 100ms using User::After(). We keep doing this until we reach the target time, if there is
	// one
	
	const TInt KDefaultLoop = 10000;
	TTime timeAtStart; 
	timeAtStart.UniversalTime(); // time of start
	
	const TInt KLoopInterval =100000; // 100ms - loop time
	TTimeIntervalMicroSeconds32 busyInterval(KLoopInterval*iPercentBusy/100); // how much of loop time to be busy
	TTimeIntervalMicroSeconds32 quietInterval(KLoopInterval-busyInterval.Int()); // how much of loop time to be quiet
	
	while (ETrue)
		{
		// the interval, for loops for the busy bit and then a User::After()
		TTime startOfInterval;
		startOfInterval.UniversalTime();
		while (ETrue)
			{
			// the busy bit - iBusyVariable is volatile so should never be optimised out
			for (TInt i=0; i<KDefaultLoop; i++)
				{
				iBusyVariable = i;
				}
			TTime now;
			now.UniversalTime();
			if (startOfInterval + busyInterval < now)
				{
				// we're passed the time
				break;
				}
			}
		User::After(quietInterval);		
			
		if (iRunFor.Int64())
			{
			// check to see if we are passed the interval given at Start()
			TTime now;
			now.UniversalTime();
			if (timeAtStart + iRunFor < now)
				{
				// we're passed the time
				break;
				}
			}
		}
	}

