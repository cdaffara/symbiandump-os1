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
//

#include "tsu_mmf_avictrl_teststep.h"
#include "tsu_mmf_avictrl_testsuite.h"

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
const TInt RAsyncTestStepSubtitleBase::KWindowIdValid = 100;
const TInt RAsyncTestStepSubtitleBase::KWindowIdValidAlt = 200;
const TInt RAsyncTestStepSubtitleBase::KWindowIdMismatched = 1;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

RTestStepAviCtrlBase::RTestStepAviCtrlBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iControllerUid = aControllerUid;
	}

/**
DoTestStepPreambleL is the preamble function for all avictrl test steps.It creates an active scheduler and installs it.
It also creates CMMFControllerEventMonitor object and starts it to avail the receiving of events from AviControllers
*/
 TVerdict RTestStepAviCtrlBase::DoTestStepPreambleL(void)
 	{
 	const TInt KDefaultPriorityValue = 11;
 	iScheduler = new(ELeave)CActiveScheduler;
       CleanupStack::PushL(iScheduler );
	CActiveScheduler::Install(iScheduler );
	CleanupStack::Pop();
	iPriority.iPriority = KDefaultPriorityValue;  // set the priority to some default value
	iPriority.iPref = EMdaPriorityPreferenceQuality;
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	return EPass;
 	}

/**
DoTestStepPostambleL is the postamble function for all test steps
It cleans up all the initialization that is done in Preamble
*/
TVerdict RTestStepAviCtrlBase::DoTestStepPostambleL(void)
	{
	iController.Stop();
	iController.Reset();
	iControllerEventMonitor->Cancel();
	delete iControllerEventMonitor;
	iControllerEventMonitor = NULL;
	iController.Close();
	iFs.Close();
	return EPass;
	}
     	       
	
void RTestStepAviCtrlBase::SetSuite( CTestSuiteAviCtrl* aPtr )
	{
	iSuite = aPtr;
	}
	
CTestSuiteAviCtrl* RTestStepAviCtrlBase::GetSuite()
	{
	return iSuite;
	}

RTestStepAviCtrlPlayBase::RTestStepAviCtrlPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepAviCtrlBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}
	

/**
Prepares the TMMFFileConfig from filename to pass it to the AddDataSource
*/
TBool RTestStepAviCtrlPlayBase::SetupReadFileConfigL()
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

TMMFFileConfig& RTestStepAviCtrlPlayBase::ReadFileConfig()
	{
	return iFileConfig;
	}

/**
DoTestStepPreambleL is the preamble function for all play test steps
Loads aviPlayController,adds DataSource and DataSink which is necessary for all the teststeps. 
*/	
 TVerdict RTestStepAviCtrlPlayBase::DoTestStepPreambleL(void)
	{
	RTestStepAviCtrlBase::DoTestStepPreambleL();
	TInt errorCode = iController.Open(KAviPlayControllerUid, iPriority);
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
	errorCode = iController.AddDataSink(KUidMmfAudioOutput,KNullDesC8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSink failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	CActiveScheduler::Start();		
	return EPass;
	}


/**
This is from MMMFControllerEventMonitorObserver. CMMFControllerEventMonitor
calls the method whenever it receives an event from AviPlayController. Basically
checks for the Playcompletion event and passes the error back to the test step.
*/
void RTestStepAviCtrlPlayBase::HandleEvent(const TMMFEvent& aEvent)
	{
	TInt errorCode = 0;
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		if(aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF2(_L("PlayCompletion is passed with  %d "), aEvent.iErrorCode);
			iTestStepResult = EPass;
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PlayCompletion is failed with  %d "), aEvent.iErrorCode);
			}
		CActiveScheduler::Stop();	
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			//add code here to set display window and to update display region.	
			RMMFVideoPlayControllerCustomCommands thePlayCommander(iController);
			errorCode = thePlayCommander.SetDisplayWindow(iScreenRect,iClipRect);
			if(errorCode)
				{
				INFO_PRINTF2(_L("SetDisplayWindow failed with  %d "), errorCode);
				iTestStepResult = EFail;
				}
			errorCode = thePlayCommander.UpdateDisplayRegion(iRegion);
			if(errorCode)
				{
				INFO_PRINTF2(_L("UpdateDisplayRegion failed with  %d "), errorCode);
				iTestStepResult = EFail;
				}
			INFO_PRINTF2(_L("Open completion is passed with  %d "), aEvent.iErrorCode);	
			errorCode = thePlayCommander.Prepare();	
			if(errorCode)
				{
				INFO_PRINTF2(_L("Prepare failed with  %d "), errorCode);
				iTestStepResult = EFail;
				}			
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("Video open complete is failed with  %d "), aEvent.iErrorCode);
			}
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			INFO_PRINTF2(_L("Prepare completion is passed with  %d "), aEvent.iErrorCode);	
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PrepareComplete is failed with  %d "),aEvent.iErrorCode);
			}
		CActiveScheduler::Stop();
		}
	else
		{
		INFO_PRINTF1(_L("Failed to receive completion Events"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}			
	
	}

RTestStepAviCtrlRecordBase::RTestStepAviCtrlRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepAviCtrlBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}
	
/**
Prepares the TMMFFileConfig from filename to pass it to the AddDataSource
*/
TBool RTestStepAviCtrlRecordBase::SetupWriteFileConfigL()
	{
	User::LeaveIfError(iFs.Connect());
	TPtrC fileName;
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

TMMFFileConfig& RTestStepAviCtrlRecordBase::WriteFileConfig()
	{
	return iFileConfig;
	}

/**
DoTestStepPreambleL is the preamble function for all record test steps.
 Loads AviRecordController,adds DataSource and DataSink which is necessary for all the teststeps. 
*/	
 TVerdict RTestStepAviCtrlRecordBase::DoTestStepPreambleL(void)
	{
	RTestStepAviCtrlBase::DoTestStepPreambleL();
	TInt errorCode = iController.Open(KAviRecordControllerUid, iPriority);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		iTestStepResult = EFail;
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
		iTestStepResult = EFail;
		}
	errorCode = iController.AddDataSource(KUidMmfAudioInput,KNullDesC8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		iTestStepResult = EFail;
		}
	RMMFVideoRecordControllerCustomCommands theVideoCommander(iController);	
	errorCode = theVideoCommander.SetVideoFormat(KAviRecordFormatUid);	
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVideoFormat failed with  %d "), errorCode);
		iTestStepResult = EFail;
		}
	errorCode = theVideoCommander.SetVideoCodec(KAviVideoCodec);	
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVideoCodec failed with  %d "), errorCode);
		iTestStepResult = EFail;
		}			
	errorCode = theVideoCommander.SetAudioCodec(KMMFFourCCCodePCM8);	
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioCodec failed with  %d "), errorCode);
		iTestStepResult = EFail;
		}
	errorCode = theVideoCommander.SetCameraHandle(0);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetCameraHandle failed with  %d "), errorCode);
		iTestStepResult = EFail;
		}
	CActiveScheduler::Start();					
	return EPass;
	}

/**
This is from MMMFControllerEventMonitorObserver. CMMFControllerEventMonitor
calls the method whenever it receives an event from AviRecordController. Basically
checks for the Playcompletion event and passes the error back to the test step
 */
void RTestStepAviCtrlRecordBase::HandleEvent(const TMMFEvent& aEvent)
	{
	TInt errorCode = KErrNone;
	const TSize KDefaultFrameSize(176,144);
	const TReal32 KDefaultFrameRate = 10.00;
	RMMFVideoRecordControllerCustomCommands theVideoCommander(iController);	
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			iTestStepResult = EPass;
			INFO_PRINTF2(_L("RecordCompletion is passed with  %d "), aEvent.iErrorCode);
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("RecordCompletion is failed with  %d "), aEvent.iErrorCode);
			}
		CActiveScheduler::Stop();	
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			errorCode = theVideoCommander.Prepare();	
			if(errorCode)
				{
				INFO_PRINTF2(_L("Prepare failed with  %d "), errorCode);
				iTestStepResult = EFail;
				}				
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("Video open complete is failed with  %d "), aEvent.iErrorCode);
			}
		}
	else if (aEvent.iEventType==KMMFEventCategoryVideoPrepareComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			INFO_PRINTF2(_L("Video Record Prepare complete is called with  %d "), errorCode);
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PrepareComplete is failed with  %d "),aEvent.iErrorCode);
			}
			
		RMMFVideoControllerCustomCommands theCommander(iController);
		TInt errorCode =theCommander.SetFrameRate(KDefaultFrameRate);
		if(errorCode)
			{
			INFO_PRINTF2(_L("SetFrameRate() during Stopped State is failed with  %d "), errorCode);
			iTestStepResult = EFail;
			}
		errorCode =theVideoCommander.SetVideoFrameSize(KDefaultFrameSize);
		if(errorCode)
			{
			INFO_PRINTF2(_L("SetFrameSize() during Stopped State is failed with  %d "), errorCode);
			iTestStepResult = EFail;
			}			
		CActiveScheduler::Stop();	
		}
	 else
		{
		INFO_PRINTF1(_L("Failed to receive completion Events"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}					
	}



//
// RAsyncTestStepAviCtrlBase
//

RAsyncTestStepAviCtrlBase::RAsyncTestStepAviCtrlBase(const TDesC& aTestName,
													 const TDesC& aSectName,
													 const TDesC& aKeyName,
													 TUid aControllerUid)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iControllerUid = aControllerUid;
	iKickoffAOp = NULL;
	}

/**
   This method is used at the beginning of the test, and initialises the
   asynchronous calls that will be activated once the call returns. The
   ActiveScheduler is active at this point.  If this test leaves, then
   StopTest() will be called with the leave value, so implicitly the test
   stops.

   KickoffTestL is the preamble function for all avictrl test steps. It creates
   CMMFControllerEventMonitor object and starts it to avail the receiving of
   events from AviControllers. Note that CMMFControllerEventMonitor is an
   EPriorityStandard AO.
*/
void RAsyncTestStepAviCtrlBase::KickoffTestL()
 	{
	// Set up an asynchronous call
	TCallBack callback (RAsyncTestStepAviCtrlBaseCallBack, this);
	delete iKickoffAOp;
	iKickoffAOp = NULL;
	iKickoffAOp =
		new (ELeave) CAsyncCallBack (callback, CActive::EPriorityLow);
	// Queues this active object to be run once.
	iKickoffAOp->Call();
 	}

TInt RAsyncTestStepAviCtrlBase::RAsyncTestStepAviCtrlBaseCallBack(TAny* aPtr)
	{
	RAsyncTestStepAviCtrlBase* self = static_cast<RAsyncTestStepAviCtrlBase*> (aPtr);
	self->DoRAsyncTestStepAviCtrlBaseCallBack();
	return KErrNone;
	}

void RAsyncTestStepAviCtrlBase::DoRAsyncTestStepAviCtrlBaseCallBack()
	{
 	const TInt KDefaultPriorityValue = 11;
	iPriority.iPriority = KDefaultPriorityValue;  // set the priority to some default value
	iPriority.iPref = EMdaPriorityPreferenceQuality;
	TRAPD(errorCode,
		  iControllerEventMonitor =
		  CMMFControllerEventMonitor::NewL(*this, iController));
	if (KErrNone != errorCode)
		{
		StopTest(errorCode, EInconclusive);
		}
	}

void RAsyncTestStepAviCtrlBase::CloseTest()
 	{
	iController.Stop();
	iController.Reset();
	iControllerEventMonitor->Cancel();
	delete iControllerEventMonitor;
	iControllerEventMonitor = NULL;
	iController.Close();
	iFs.Close();

	delete iKickoffAOp; // no need to Cancel
	iKickoffAOp = NULL;
 	}


void RAsyncTestStepAviCtrlBase::SetSuite( CTestSuiteAviCtrl* aPtr )
	{
	iSuite = aPtr;
	}

CTestSuiteAviCtrl* RAsyncTestStepAviCtrlBase::GetSuite()
	{
	return iSuite;
	}


//
// RAsyncTestStepAviCtrlPlayBase
//

RAsyncTestStepAviCtrlPlayBase::RAsyncTestStepAviCtrlPlayBase(
	const TDesC& aTestName,
	const TDesC& aSectName,
	const TDesC& aKeyName,
	TUid aControllerUid)
	:
	RAsyncTestStepAviCtrlBase(aTestName, aSectName, aKeyName, aControllerUid),
	iScreenRect(),
	iClipRect(),
	iRegion(),
	iKickoffAOp(NULL)
	{
	}

void RAsyncTestStepAviCtrlPlayBase::CloseTest()
	{
	delete iKickoffAOp; // no need to Cancel
	iKickoffAOp = NULL;
	RAsyncTestStepAviCtrlBase::CloseTest();
	}


/**
   Prepares the TMMFFileConfig from filename to pass it to the AddDataSource
*/
TBool RAsyncTestStepAviCtrlPlayBase::SetupReadFileConfigL()
	{
	User::LeaveIfError(iFs.Connect());
  	TPtrC fileName;
	if(!GetStringFromConfig(iSectName, iKeyName, fileName))
		{
		return EFalse;
		}
 	iFileConfig().iPath = fileName ;
 	
	if (!GetBoolFromConfig(iSectName, _L("gce"), iGceEnabled))
		{
		iGceEnabled = EFalse;
		}
 		
	return ETrue;
	}

TMMFFileConfig& RAsyncTestStepAviCtrlPlayBase::ReadFileConfig()
	{
	return iFileConfig;
	}

/**
   KickoffTestL function for all play test opens the aviplaycontroller and adds
   DataSource and DataSink which are necessary for all the teststeps.
*/
void RAsyncTestStepAviCtrlPlayBase::KickoffTestL()
	{
	RAsyncTestStepAviCtrlBase::KickoffTestL();

	// Set up an asynchronous call
	TCallBack callback (RAsyncTestStepAviCtrlPlayBaseCallBack, this);
	delete iKickoffAOp;
	iKickoffAOp = NULL;
	iKickoffAOp =
		new (ELeave) CAsyncCallBack (callback, CActive::EPriorityLow);
	// Queues this active object to be run once.
	iKickoffAOp->Call();
	}

TInt RAsyncTestStepAviCtrlPlayBase::RAsyncTestStepAviCtrlPlayBaseCallBack(TAny* aPtr)
	{
	RAsyncTestStepAviCtrlPlayBase* self =
		static_cast<RAsyncTestStepAviCtrlPlayBase*> (aPtr);
	self->DoRAsyncTestStepAviCtrlPlayBaseCallBack();
	return KErrNone;
	}

void RAsyncTestStepAviCtrlPlayBase::DoRAsyncTestStepAviCtrlPlayBaseCallBack()
	{
	TInt errorCode = iController.Open(KAviPlayControllerUid, iPriority);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		StopTest(errorCode, EInconclusive);
		return;
		}
	iControllerEventMonitor->Start();

	TBool configFileOk = EFalse;
	TRAPD(setupReadFileConfigErr,
		  configFileOk = SetupReadFileConfigL());

	if((KErrNone != setupReadFileConfigErr) || (configFileOk == EFalse))
		{
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
	errorCode = iController.AddDataSink(KUidMmfAudioOutput,KNullDesC8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSink failed with  %d "), errorCode);
		StopTest(errorCode, EInconclusive);
		return;
		}
	errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		StopTest(errorCode, EInconclusive);
		}
	}


/**
   This is from MMMFControllerEventMonitorObserver. CMMFControllerEventMonitor
   calls the method whenever it receives an event
   fromAviPlayController. Basically checks for the Playcompletion event and
   passes the error back to the test step. This method may be reimplemented by
   subclasses in case its behaviour needs to be customized.
*/
void RAsyncTestStepAviCtrlPlayBase::HandleEvent(const TMMFEvent& aEvent)
	{
	TInt errorCode = 0;
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		if(aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF2(_L("PlayCompletion is passed with  %d "),
						 aEvent.iErrorCode);
			iTestStepResult = EPass;
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PlayCompletion is failed with  %d "),
						 aEvent.iErrorCode);
			}
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			//add code here to set display window and to update display region.
			RMMFVideoPlayControllerCustomCommands thePlayCommander(iController);
			errorCode = thePlayCommander.SetDisplayWindow(iScreenRect,iClipRect);
			if(errorCode)
				{
				INFO_PRINTF2(_L("SetDisplayWindow failed with  %d "), errorCode);
				iTestStepResult = EFail;
				}
			errorCode = thePlayCommander.UpdateDisplayRegion(iRegion);
			if(errorCode)
				{
				INFO_PRINTF2(_L("UpdateDisplayRegion failed with  %d "), errorCode);
				iTestStepResult = EFail;
				}
			INFO_PRINTF2(_L("Open completion is passed with  %d "), aEvent.iErrorCode);
			errorCode = thePlayCommander.Prepare();
			if(errorCode)
				{
				INFO_PRINTF2(_L("Prepare failed with  %d "), errorCode);
				iTestStepResult = EFail;
				}
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("Video open complete is failed with  %d "), aEvent.iErrorCode);
			}
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			INFO_PRINTF2(_L("Prepare completion is passed with  %d "), aEvent.iErrorCode);
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PrepareComplete is failed with  %d "),aEvent.iErrorCode);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed to receive completion Events"));
		iTestStepResult = EFail;
		}

	// Stop the test here for convenience in case of failure. Subclasses classes
	// may need to change this behaviour by overwritting this method.
	if (iTestStepResult == EFail)
		{
		StopTest(KErrGeneral, EFail);
		return;
		}
	}

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
RAsyncTestStepSubtitleBase::RAsyncTestStepSubtitleBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
: RAsyncTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid), 
	iSurfaceCommands(iController), 
	iSubtitleCommands(iController)
	{
	}

void RAsyncTestStepSubtitleBase::KickoffTestL()
	{
	// Reset the CRP ready event counter and kick off the test.
	iCrpReadyEventCount = 0;
	RAsyncTestStepAviCtrlPlayBase::KickoffTestL();
	}

void RAsyncTestStepSubtitleBase::HandleEvent(const TMMFEvent& aEvent)
	{
	if (KMMFEventCategoryVideoOpenComplete == aEvent.iEventType)
		{
		if (aEvent.iErrorCode != KErrNone)
			{
			ERR_PRINTF2(_L("Video open complete has failed with %d "), aEvent.iErrorCode);
			StopTest(aEvent.iErrorCode, EFail);
			}
		else
			{
			TInt err = iSurfaceCommands.UseSurfaces();
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("UseSurfaces() failed with %d."), err);
				StopTest(err, EInconclusive);
				return;
				}
		
			TRAP(err, OnOpenCompleteL());
		
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Test failed with error %d in open complete event handler"), err);
				StopTest(err, EFail);
				}
			}
		}
	else if (KMMFEventCategoryPlaybackComplete == aEvent.iEventType)
		{
		if (aEvent.iErrorCode != KErrNone)
			{
			ERR_PRINTF2(_L("Playback completed with error &d."), aEvent.iErrorCode);
			StopTest(aEvent.iErrorCode, EFail);
			}
		else
			{
			TRAPD(err, OnPlayCompleteL());
			
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Test failed with error %d in play complete event handler"), err);
				StopTest(err, EFail);
				}
			
			if (iExpectedCrpReadyCount != iCrpReadyEventCount)
				{
				ERR_PRINTF3(_L("Received %d CRP ready event notifications, but expecting %d."), iCrpReadyEventCount, iExpectedCrpReadyCount);
				StopTest(KErrGeneral, EFail);
				}
			
			StopTest();
			}
		}
	else if (KMMFEventCategoryVideoSubtitleCrpReady == aEvent.iEventType)
		{
		iCrpReadyEventCount++;
		
		TWsGraphicId id(TWsGraphicId::EUninitialized);
		TRect rect;
		TInt err = iSubtitleCommands.GetCrpParameters(aEvent.iErrorCode, id, rect);
		
		if (err != KErrNone)
			{
			ERR_PRINTF3(_L("GetCrpParameters returned %d in CRP ready event handler; expected %d"), err, KErrNone);
			StopTest(err, EFail);
			}
		
		if (rect.IsEmpty())
			{
			ERR_PRINTF1(_L("GetCrpParameters returned an empty rectangle"));
			StopTest(KErrGeneral, EFail);
			}
		
		if (!id.IsId())
			{
			ERR_PRINTF1(_L("GetCrpParameters returned an invalid id; IsId() is EFalse"));
			StopTest(KErrGeneral, EFail);
			}
		
		if (id.IsUid())
			{
			ERR_PRINTF1(_L("GetCrpParameters returned an invalid id; IsUid() is ETrue"));
			StopTest(KErrGeneral, EFail);
			}
		
		if (id.Id() == 0)
			{
			ERR_PRINTF1(_L("GetCrpParameters returned an invalid id; Id() is 0"));
			StopTest(KErrGeneral, EFail);
			}
		}
	else if (KMMFEventCategoryVideoSurfaceCreated == aEvent.iEventType)
		{
		// Ignore these events
		}
	else
		{
		ERR_PRINTF3(_L("Unexpected Event from the Controller: type %d error: %d"), aEvent.iEventType, aEvent.iErrorCode);
		StopTest(KErrGeneral, EFail);
		}
	}

void RAsyncTestStepSubtitleBase::SetExpectedCrpReadyCount(TInt aCount)
	{
	iExpectedCrpReadyCount = aCount;
	}

void RAsyncTestStepSubtitleBase::CheckAndEnableSubtitlesL()
	{
	TBool available = EFalse;
	
	TInt err = iSubtitleCommands.GetSubtitlesAvailable(available);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GetSubtitlesAvailable failed with %d"), err);
		User::Leave(err);
		}
	
	if (!available)
		{
		ERR_PRINTF1(_L("Subtitles unavailable"));
		User::Leave(KErrGeneral);
		}
	
	err = iSubtitleCommands.EnableSubtitles();
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitles failed with %d"), err);
		User::Leave(err);
		}
	}

void RAsyncTestStepSubtitleBase::AddDefaultSubtitleConfL()
	{
	TMMFSubtitleWindowConfig config;
	config.iWindowId = KWindowIdValid;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = KWindowClipRectFull;
	TInt err = iSubtitleCommands.AddSubtitleConfig(config);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddSubtitleConfig failed with %d"), err);
		User::Leave(err);
		}
	}

void RAsyncTestStepSubtitleBase::StartPlaybackL()
	{
	TInt err = iController.Prime();
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Prepare failed with %d."), err);
		User::Leave(err);
		}
	
	err = iController.Play();
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not start playback.  Play() failed with %d."), err);
		User::Leave(err);
		}
	}

void RAsyncTestStepSubtitleBase::RemoveConfigDisableL()
	{
	TInt err = iSubtitleCommands.RemoveSubtitleConfig(KWindowIdValid);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("RemoveSubtitleConfig failed with %d"), err);
		User::Leave(err);
		}
	
	err = iSubtitleCommands.DisableSubtitles();
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DisableSubtitles failed with %d"), err);
		User::Leave(err);
		}
	}

void RAsyncTestStepSubtitleBase::OnOpenCompleteL()
	{
	}

void RAsyncTestStepSubtitleBase::OnPlayCompleteL()
	{
	}

#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
