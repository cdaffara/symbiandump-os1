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


#include "TestStepCtrlSetRepeatsCustomCommands.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfstandardcustomcommandsenums.h>
#include <mmf/common/mmfstandardcustomcommandsimpl.h>
#endif


CAsyncTestStepCtrlSetRepeatsCustomCommands::CAsyncTestStepCtrlSetRepeatsCustomCommands(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	iTestStepName=aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}


CAsyncTestStepCtrlSetRepeatsCustomCommands * CAsyncTestStepCtrlSetRepeatsCustomCommands::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestStepCtrlSetRepeatsCustomCommands(aTestName, aSectName, aKeyName);
	}

	// from RAsyncTestStep
void CAsyncTestStepCtrlSetRepeatsCustomCommands::KickoffTestL()
	{
	INFO_PRINTF1(_L("This a test for Controller SetRepeat custom command "));
	iPriority.iPriority = KDefaultPriority;  // set the priority to some default value
	iPriority.iPref = EMdaPriorityPreferenceQuality;
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	
	//Prepare the TMMFFileConfig from filename to pass it to the AddDataSource
	TPtrC fileName;
	if(!GetStringFromConfig(iSectName, iKeyName, fileName))
		{
		INFO_PRINTF1(_L("GetStringFromConfig: Failed "));
		StopTest(KErrGeneral, EFail);
		return;
		}
 	iFileConfig().iPath = fileName;

	// Open the controller
	INFO_PRINTF1(_L("Controller: Open"));
	TInt errorCode = iController.Open(KMmfAudioControllerUid, iPriority);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		StopTest(errorCode, EFail);
		User::Leave( errorCode );
		}
	// Start the event monitor
	iControllerEventMonitor->Start();
	
	// Add the DataSource
	INFO_PRINTF1(_L("Controller: AddDataSource "));
	errorCode = iController.AddDataSource(KUidMmfFileSource, iFileConfig);
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		StopTest(errorCode, EFail);
		User::Leave( errorCode );
		}
	// Add the DataSink
	INFO_PRINTF1(_L("Controller: AddDataSink "));
	errorCode = iController.AddDataSink(KUidMmfAudioOutput,KNullDesC8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSink failed with  %d "), errorCode);
		StopTest(errorCode, EFail);
		User::Leave( errorCode );
		}
	INFO_PRINTF1(_L("RMMFAudioPlayControllerSetRepeatsCustomCommands::SetRepeats"));
	RMMFAudioPlayControllerSetRepeatsCustomCommands theCommander(iController);
	errorCode = theCommander.SetRepeats(1, TTimeIntervalMicroSeconds(3000000) );
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	if(errorCode == KErrNotSupported)
		{//controller on old devsound does not support this custom command
		INFO_PRINTF2(_L("SetRepeats is failed with  %d on legacy devsound"), errorCode);
		StopTest();
		return;
		}
	else
		{
		INFO_PRINTF2(_L("SetRepeats is not failed with KErrNotSupported but failed with %d on legacy devsound"), errorCode);
		StopTest(errorCode, EFail);
		User::Leave(errorCode);
		}
	#else
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetRepeats is failed with  %d "), errorCode);
		StopTest(errorCode, EFail);
		User::Leave( errorCode );
		}
	#endif
	// Prime The Controller
	INFO_PRINTF1(_L("Prime"));
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		StopTest(errorCode, EFail);
		User::Leave( errorCode );
		}
	// Start Playing 
	INFO_PRINTF1(_L("Play"));
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		StopTest(errorCode, EFail);
		User::Leave( errorCode );
		}
	}

void CAsyncTestStepCtrlSetRepeatsCustomCommands::CloseTest()
	{
	iController.Stop();
	iController.Reset();
	iControllerEventMonitor->Cancel();
	delete iControllerEventMonitor;
	iController.Close();
	}


/**
 * This is from MMMFControllerEventMonitorObserver. CMMFControllerEventMonitor
 * calls the method whenever it receives an event from PlayController. Basically
 * checks for the Playcompletion event and passes the error back to the test step.
 */
void CAsyncTestStepCtrlSetRepeatsCustomCommands::HandleEvent(const TMMFEvent& aEvent)
	{
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			INFO_PRINTF2(_L("Playcompletion Error %d "), aEvent.iErrorCode);
			StopTest();//Success
			}
		else
			{
			INFO_PRINTF2(_L("Playcompletion is failed with  %d "), aEvent.iErrorCode);
			StopTest(aEvent.iErrorCode, EFail);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed to receive Playcompletion Event "));
		StopTest(KErrGeneral, EFail);
		}
	}
