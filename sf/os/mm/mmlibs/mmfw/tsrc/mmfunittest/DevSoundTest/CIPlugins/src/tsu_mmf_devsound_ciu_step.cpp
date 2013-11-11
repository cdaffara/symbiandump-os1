// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Custom interface definitions
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>

// Test system includes
#include "tsu_mmf_devsound_ciu_step.h"
#include "tsu_mmf_devsound_ciu_suite.h"


/**
CTestStepCustominterfaceUnificationUnit constructor
*/
CTestStepCustominterfaceUnificationUnit::CTestStepCustominterfaceUnificationUnit() : 
iScheduler(NULL)
	{
	iRes = &iSourceFileName;
	}

CTestStepCustominterfaceUnificationUnit::CTestStepCustominterfaceUnificationUnit(const TDesC& aTestName) : 
iScheduler(NULL)
	{
	iRes = &iSourceFileName;
	iTestStepName = aTestName;
	}

/**
CTestStepCustominterfaceUnificationUnit destructor
*/
CTestStepCustominterfaceUnificationUnit::~CTestStepCustominterfaceUnificationUnit()
	{
	if (iMMFDevSound)
		{
		delete iMMFDevSound;
		}
	if (iActiveListener)
		{
		delete iActiveListener;
		}
	if (iScheduler)
		{
		delete iScheduler;
		}
	}

/**
CTestStepCustominterfaceUnificationUnit accessor : set suite

 @param CTestSuiteCustominterfaceUnificationUnit*
		The test suite.
 */
void CTestStepCustominterfaceUnificationUnit::SetSuite(CTestSuiteCustomInterfaceUnificationUnit* aSuite)
	{ 
	iTestSuite = aSuite;
	}

/**
CTestStepCustominterfaceUnificationUnit accessor : get suite
 *
 * @return "CTestSuiteCustominterfaceUnificationUnit*"
 *		    The test suite.
 */
CTestSuiteCustomInterfaceUnificationUnit* CTestStepCustominterfaceUnificationUnit::TestSuite() const
	{
	return iTestSuite;
	}

/**
DoTestStepPreambleL is the preamble function for test steps
It creates an active scheduler and installs it. This is necessary
since we are multithreading and each test step runs in a separate thread.
@return TVerdict which is either EPass or Efail
 
The function can leave if there is insufficient memeory
*/
enum TVerdict CTestStepCustominterfaceUnificationUnit::DoTestStepPreambleL(void)
	{
	iScheduler = new(ELeave)CActiveScheduler;
	CleanupStack::PushL( iScheduler );

	CActiveScheduler::Install( iScheduler );

	CleanupStack::Pop();

	iActiveListener= new(ELeave)CActiveListener;

	INFO_PRINTF1(_L("Creating DevSound Object"));
	TRAPD(err, iMMFDevSound = CMMFDevSound::NewL());
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		return EPass;
		}
	}

/**
The post amble step cleans up the allocated memory in the preamble
@return TVerdict EPass or EFail
@leave can leave due to insufficient memory
*/
enum TVerdict CTestStepCustominterfaceUnificationUnit::DoTestStepPostambleL(void)
	{
	INFO_PRINTF1(_L("Deleting DevSound Object"));

	if (iMMFDevSound)
		{
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	if (iActiveListener)
		{
		delete iActiveListener;
		iActiveListener = NULL;
		}
	if (iScheduler)
		{
		delete iScheduler;
		iScheduler = NULL;
		}

	return EPass;
	}


//
//		start Callback methods from MDevSoundObserver mixin
//

/**
InitializeComplete
@param aError
*/
void CTestStepCustominterfaceUnificationUnit::InitializeComplete (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called InitializeComplete with aError = %d"), aError);
	TRequestStatus* stat = &(iActiveListener->iStatus);
	User::RequestComplete(stat, aError);
	iCallbackArray[EInitComplete] ++;
	iCallbackError = aError;
	}

/**
ToneFinished
@param aError
*/
void CTestStepCustominterfaceUnificationUnit::ToneFinished (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called ToneFinished with aError = %d"), aError);
	TRequestStatus* stat = &(iActiveListener->iStatus);
	User::RequestComplete(stat, aError);
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	}

/**
BufferToBeFilled
@param aBuffer
*/
void CTestStepCustominterfaceUnificationUnit::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	TRequestStatus* stat = &(iActiveListener->iStatus);
	if (aBuffer != NULL) 
		{
		User::RequestComplete(stat, KErrNone);
		iCallbackError = KErrNone;
		}
	else 
		{
		User::RequestComplete(stat, KErrNotFound);
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToFill] ++;
	}

/**
PlayError
@param aError
*/
void CTestStepCustominterfaceUnificationUnit::PlayError (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called PlayError with aError = %d"), aError);
	TRequestStatus* stat = &(iActiveListener->iStatus);
	User::RequestComplete (stat, aError);
	iCallbackError = aError;
	iCallbackArray[EPlayError] ++;
	}

/**
BufferToBeEmptied
@param aBuffer
*/
void CTestStepCustominterfaceUnificationUnit::BufferToBeEmptied (CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeEmptied"));
	iBuffer = aBuffer;
	TRequestStatus* stat = &(iActiveListener->iStatus);
	if (aBuffer != NULL)
		{
		User::RequestComplete(stat, KErrNone);
		iCallbackError = KErrNone;
		}
	else
		{
		User::RequestComplete(stat, KErrNotFound);
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToEmpty] ++;
	}

/**
RecordError
@param aError
*/
void CTestStepCustominterfaceUnificationUnit::RecordError (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called RecordError with aError = %d"), aError);
	TRequestStatus* stat = &(iActiveListener->iStatus);
	User::RequestComplete (stat, aError);
	iCallbackError = aError;
	iCallbackArray[ERecError] ++;
	}

/**
ConvertError
*/
void CTestStepCustominterfaceUnificationUnit::ConvertError (TInt /*aError*/)
	{
	}

/**
DeviceMessage
*/
void CTestStepCustominterfaceUnificationUnit::DeviceMessage(TUid, const TDesC8& /*aMsg*/)
	{
	}


//
//		start Test methods (called by test cases)
//

/**
TestInitialize
@param aMode
@return TVerdict
*/
TVerdict CTestStepCustominterfaceUnificationUnit::TestInitialize(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	INFO_PRINTF1(_L("Initializing DevSound"));

	ResetCallbacks();
	iActiveListener->InitialiseActiveListener();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	}

/**
TestInitialize
@param aHwDeviceUid
@param aMode
@return TVerdict
*/
TVerdict CTestStepCustominterfaceUnificationUnit::TestInitialize(TUid aHwDeviceUid, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	INFO_PRINTF1(_L("Initializing DevSound"));

	ResetCallbacks();
	iActiveListener->InitialiseActiveListener();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aHwDeviceUid, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	}

/**
TestInitialize
@param aDataType
@param aMode
@return TVerdict
*/
TVerdict CTestStepCustominterfaceUnificationUnit::TestInitialize(TFourCC& aDataType, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	INFO_PRINTF1(_L("Initializing DevSound"));

	ResetCallbacks();
	iActiveListener->InitialiseActiveListener();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aDataType, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	}

/**
 * Initialises dev sound for the given HwDevice uid and tries to instantiate the given interface.
 */
TAny* CTestStepCustominterfaceUnificationUnit::InitializeAndInstantiateInterfaceL(const TUid aHwDeviceUid, const TUid aInterfaceUid)
	{
	TAny* interface = NULL;
	
	iTestStepResult = TestInitialize(aHwDeviceUid, EMMFStatePlaying);

	if (iTestStepResult == EPass)
		{
		INFO_PRINTF1(_L("Successfully initialized DevSound"));
		interface = iMMFDevSound->CustomInterface(aInterfaceUid);
		}
	else
		{
		ERR_PRINTF1(_L("Failed to initialize DevSound"));
		}

	return interface;
	}

/**
 * Initialises dev sound for the given FourCC and tries to instantiate the given interface.
 */
TAny* CTestStepCustominterfaceUnificationUnit::InitializeAndInstantiateInterfaceL(TFourCC& aDataType, const TUid aInterfaceUid)
	{
	TAny* interface = NULL;
	
	iTestStepResult = TestInitialize(aDataType, EMMFStatePlaying);

	if (iTestStepResult == EPass)
		{
		INFO_PRINTF1(_L("Successfully initialized DevSound"));
		interface = iMMFDevSound->CustomInterface(aInterfaceUid);
		}
	else
		{
		ERR_PRINTF1(_L("Failed to initialize DevSound"));
		}

	return interface;
	}
	
/**
ReadFileL
@param aName
@param aData
*/
void CTestStepCustominterfaceUnificationUnit::ReadFileL(const TDesC &aName, TInt& aData)
	{
	User::LeaveIfError(iFs.Connect());

	User::LeaveIfError(iFile.Open(iFs, aName, EFileRead));

	TBuf8<4> data;

	User::LeaveIfError(iFile.Read(data));
	iFile.Close();

	iFs.Delete(aName);
	iFs.Close();

	TLex8 lex(data);
	lex.Val(aData);
	}

/**
WriteFileL
@param aName
@param aData
*/
void CTestStepCustominterfaceUnificationUnit::WriteFileL(const TDesC &aName, TInt aData)
	{
	User::LeaveIfError(iFs.Connect());

	iFs.MkDirAll(aName);

	User::LeaveIfError(iFile.Replace(iFs, aName, EFileWrite));

	TBuf8<4> data;
	data.Format(_L8("%d"), aData);

	User::LeaveIfError(iFile.Write(data));
	iFile.Close();
	iFs.Close();
	}

/**
ResetCallbacks
*/
void CTestStepCustominterfaceUnificationUnit::ResetCallbacks()
	{
	//Initialize callback array
	iCallbackArray.Reset();
	}
