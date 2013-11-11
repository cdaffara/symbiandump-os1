// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsu_mmf_oggctrl_teststep.h"

const TInt	KSizeBuf = 256;
const TInt KDefaultPriority = 11;
/**
 * Constructor for RTestStepOggCtrlBase
 */
RTestStepOggCtrlBase::RTestStepOggCtrlBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iControllerUid = aControllerUid;
	}
	
/**
 * DoTestStepPreambleL is the preamble function for all oggctrl test steps
 * It creates an active scheduler and installs it. 
 * It also creates CMMFControllerEventMonitor object and starts it 
 * to avail the receiving of events from OggControllers
 */
TVerdict RTestStepOggCtrlBase::DoTestStepPreambleL(void)
	{
	iScheduler = new(ELeave)CActiveScheduler;
    CleanupStack::PushL(iScheduler );
	CActiveScheduler::Install(iScheduler );
	CleanupStack::Pop();
	iPriority.iPriority = KDefaultPriority;  // set the priority to some default value
	iPriority.iPref = EMdaPriorityPreferenceQuality;
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	return EPass;
	}
	
/**
 * DoTestStepPostambleL is the postamble function for all test steps
 * It cleans up all the initialization that is done in Preamble
 */
TVerdict RTestStepOggCtrlBase::DoTestStepPostambleL(void)
	{
	iController.Stop();
	iController.Reset();
	iControllerEventMonitor->Cancel();
	delete iControllerEventMonitor;
	iController.Close();
	iFs.Close();
	return EPass;
	}

void RTestStepOggCtrlBase::SetSuite( CTestSuiteOggCtrl* aPtr )
	{
	iSuite = aPtr;
	}

CTestSuiteOggCtrl* RTestStepOggCtrlBase::GetSuite()
	{
	return iSuite;
	}
/**
 * Compares Actual time with the expected time allowing aDeviation 
 */
TBool RTestStepOggCtrlBase::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// save unnessary conditions
	if(aActual == aExpected)
		{
		return ETrue;
		}
	// Prevent unsigned wrapping errors 
	TUint difference;
	if(aActual > aExpected)
		{
		difference = aActual - aExpected;	
		}
	else
		{
		difference = aExpected - aActual;	
		}
	// compare
	if(difference <= aDeviation)
		{
		return ETrue;	
		}
	return EFalse;
	}
/**
 * Creates a file and copies the data in the source file to the destination file. 
 */
TInt RTestStepOggCtrlBase::CopyFile(TPtrC aFileNameSrc, TPtrC aFileNameDst)
	{
	TInt theErr = KErrNone;
	RFs		fs;
	fs.Connect();
	RFile	theFileNew;
	RFile	theFile;

	fs.MkDirAll(aFileNameDst);
	theFile.Open(fs, aFileNameSrc, EFileShareReadersOnly);
	TInt	theSize;
	theFile.Size(theSize);
	
	TBuf8<KSizeBuf>	theBuf;
	TInt	theStep = KSizeBuf;
	if(theStep > theSize)
		{
		theStep = theSize;
		}
	TInt theRes = theFileNew.Create(fs, aFileNameDst, EFileShareAny);
	if(theRes == KErrAlreadyExists)
		{
		theRes = theFileNew.Replace(fs, aFileNameDst, EFileShareAny);	
		}
	if(theRes != KErrNone)
		{
		return theRes;	
		}
	TInt	theCurLength = 0;
	while(theCurLength < theSize)
		{
		theFile.Read(theBuf, theStep);
		theFileNew.Write(theBuf, theStep);
		theCurLength += theStep;
		if(theCurLength + theStep > theSize )
			{
			theStep = theSize - theCurLength;
			}
		}
	theFileNew.Close();
	theFile.Close();
	fs.Close();
	return theErr;
	}
/**
 * Constructor for RTestStepOggCtrlPlayBase
 */
RTestStepOggCtrlPlayBase::RTestStepOggCtrlPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}
/**
 * Prepares the TMMFFileConfig from filename to pass it to the AddDataSource
 */
TBool RTestStepOggCtrlPlayBase::SetupReadFileConfigL()
	{
	User::LeaveIfError(iFs.Connect());
 	TPtrC fileName;
	if(!GetStringFromConfig(iSectName, iKeyName, fileName))
		{
		return EFalse;
		}
 	iFileConfig().iPath = fileName ;
	return ETrue;
	}

TMMFFileConfig& RTestStepOggCtrlPlayBase::ReadFileConfig()
	{
	return iFileConfig;
	}

/**
 * DoTestStepPreambleL is the preamble function for all play test steps
 * Loads OggPlayController,adds DataSource and DataSink which is necessary for all the teststeps. 
 */	
TVerdict RTestStepOggCtrlPlayBase::DoTestStepPreambleL(void)
	{
	RTestStepOggCtrlBase::DoTestStepPreambleL();
	TInt errorCode = iController.Open(iControllerUid, iPriority);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	iControllerEventMonitor->Start();
	if(!SetupReadFileConfigL())
		{
		return EInconclusive;
		}
	errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	errorCode = iController.AddDataSink(KUidMmfAudioOutput,KNullDesC8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSink failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	return EPass;
	}


/**
 * This is from MMMFControllerEventMonitorObserver. CMMFControllerEventMonitor
 * calls the method whenever it receives an event from OggPlayController. Basically
 * checks for the Playcompletion event and passes the error back to the test step.
 */
void RTestStepOggCtrlPlayBase::HandleEvent(const TMMFEvent& aEvent)
	{
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		if(aEvent.iErrorCode == KErrNone)
			{
			iTestStepResult = EPass;
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PlayCompletion is failed with  %d "), aEvent.iErrorCode);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed to receive PlayCompletion Event "));
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();
	}

/**
 * Constructor for RTestStepOggCtrlRecordBase
 */
RTestStepOggCtrlRecordBase::RTestStepOggCtrlRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}
	
/**
 * Prepares the TMMFFileConfig from filename to pass it to the AddDataSource
 */
TBool RTestStepOggCtrlRecordBase::SetupWriteFileConfigL()
	{
	User::LeaveIfError(iFs.Connect());
	TPtrC	fileName;
	if(!GetStringFromConfig(iSectName, iKeyName, fileName))
		{
		return EFalse;
		}
	TInt ret = iFs.Delete(fileName);
	if (ret != KErrNone && ret != KErrNotFound)
		{
		User::Leave(ret);	
		}
	iFileConfig().iPath = fileName ;
	return ETrue;
	}

TMMFFileConfig& RTestStepOggCtrlRecordBase::WriteFileConfig()
	{
	return iFileConfig;
	}

/**
 * DoTestStepPreambleL is the preamble function for all record test steps
 * Loads OggRecordController,adds DataSource and DataSink which is necessary for all the teststeps. 
 */	
TVerdict RTestStepOggCtrlRecordBase::DoTestStepPreambleL(void)
	{
	RTestStepOggCtrlBase::DoTestStepPreambleL();
	TInt errorCode = iController.Open(iControllerUid, iPriority);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	iControllerEventMonitor->Start();
	if(!SetupWriteFileConfigL())
		{
		return EInconclusive;
		}
	errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSink failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	errorCode = iController.AddDataSource(KUidMmfAudioInput,KNullDesC8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	return EPass;
	}

/**
 * This is from MMMFControllerEventMonitorObserver. CMMFControllerEventMonitor
 * calls the method whenever it receives an event from OggRecordController. Basically
 * checks for the Playcompletion event and passes the error back to the test step.
 */
void RTestStepOggCtrlRecordBase::HandleEvent(const TMMFEvent& aEvent)
	{
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			iTestStepResult = EPass;
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("RecordCompletion is failed with  %d "), aEvent.iErrorCode);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed to receive RecordCompletion Event "));
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();
	}
