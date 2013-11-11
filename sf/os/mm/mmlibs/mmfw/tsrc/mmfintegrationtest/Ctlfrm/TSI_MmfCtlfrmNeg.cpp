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
// TSIMmfCtlfrmNeg.cpp
// Negative integration testing for MMF CTLFRM
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TSI_MmfCtlfrmNeg.h"
#include "TSI_MmfCtlfrmStep.h"
#include "TSI_MmfCtlfrmSuite.h"
#include "TSI_MmfCodes.h"
#include "TSI_MmfEventIds.h"
#include "ActrlTestUids.h"

#include <mmf/common/mmfcontroller.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>

//const TUid KTestControllerUid = {KTSIMmfControllerUid};	// EABI warning removal
//const TUid KTestController2Uid = {KTSIMmfController2Uid};	// EABI warning removal
const TUid KTestDataSourceUid = {KTSIMmfDataSourceUid};
//const TUid KTestDataSinkUid   = {KTSIMmfDataSinkUid};		// EABI warning removal

// audio controller, from mmfControllerImplementationUIDs.hrh
//const TUid KTestAudioControllerUid = {KMmfUidControllerAudio};	// EABI warning removal
// ---------------------------
// RTestMmfCtlfrmI0502
//
// Load controller plugin for unsupported file format
//
// REQ172.5.3

RTestMmfCtlfrmI0502* RTestMmfCtlfrmI0502::NewL()
	{
	RTestMmfCtlfrmI0502* self = new(ELeave) RTestMmfCtlfrmI0502;
	return self;
	}

RTestMmfCtlfrmI0502::RTestMmfCtlfrmI0502()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0502");
	}


TVerdict RTestMmfCtlfrmI0502::DoTestStepL()
	{
	INFO_PRINTF1(_L("open controller by unsupported extension"));

//  TO DO 
//  This test needs to be looked at to clarify if we need TMMFPrioritySettings 
//
//	TMMFPrioritySettings settings;					

//	settings.iPriority = ETSIMmfPriorityLow;
//	settings.iPref = EMdaPriorityPreferenceTime;
//	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.xls"));
	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	TInt numControllers = controllers.Count();
	if(numControllers)
		{
		ERR_PRINTF1(_L("Error : found a suitable controller"));
		CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("Success : found no suitable controllers"));
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0504
//
// Load a controller by preferred supplier where only compatible controller
// available is from a rival
//
// REQ172.5.5.1

RTestMmfCtlfrmI0504* RTestMmfCtlfrmI0504::NewL()
	{
	RTestMmfCtlfrmI0504* self = new(ELeave) RTestMmfCtlfrmI0504;
	return self;
	}

RTestMmfCtlfrmI0504::RTestMmfCtlfrmI0504()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0504");
	}


TVerdict RTestMmfCtlfrmI0504::DoTestStepL()
	{
	// try to open a controller by preferred supplier
	// where only a rival's is available

	INFO_PRINTF1(_L("open controller by preferred supplier"));

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.wav"));
	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Set for only those plugins supplied by "ACME Industries"
	_LIT(KPrefSupplierAcme, "ACME Industries");
	cSelect->SetPreferredSupplierL(KPrefSupplierAcme, CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	TInt numControllers = controllers.Count();
	if(numControllers)
		{
		ERR_PRINTF1(_L("Error : found a suitable controller"));
		CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("Success : found no suitable controllers"));
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0511
//
// Add a data source to a controller which does not support dynamic addition of sources / sinks
//
// REQ172.5.5.1

RTestMmfCtlfrmI0511* RTestMmfCtlfrmI0511::NewL()
	{
	RTestMmfCtlfrmI0511* self = new(ELeave) RTestMmfCtlfrmI0511;
	return self;
	}

RTestMmfCtlfrmI0511::RTestMmfCtlfrmI0511()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0511");
	}


TVerdict RTestMmfCtlfrmI0511::DoTestStepL()
	{
	INFO_PRINTF1(_L("add data source where not supported"));

	// TSI_MmfController2 does not support adding source / sink


	// first : add a data source without a handle
	_LIT8(KInitData,"TEST");

	TInt error = KErrNone;
	error = iController.AddDataSource(KTestDataSourceUid, KInitData);
	if (!error)
		{
		ERR_PRINTF1(_L("Error : AddDataSource succeeded"));
		return iTestStepResult = EFail;
		}
	else if (error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("AddDataSource failed with unexpected error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("AddDataSource failed, error KErrNotSupported"));

	// next : add a data source with a handle
	TMMFMessageDestination* sourceHandlePtr = new (ELeave) TMMFMessageDestination();
	TMMFMessageDestination& sourceHandle = *sourceHandlePtr;
	error = iController.AddDataSource(KTestDataSourceUid, KInitData, sourceHandle);
	if (!error)
		{
		ERR_PRINTF1(_L("Error : AddDataSource (with handle) succeeded"));
		delete sourceHandlePtr;
		return iTestStepResult = EFail;
		}
	else if (error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("AddDataSource (with handle) failed with unexpected error %d"), error);
		delete sourceHandlePtr;
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("AddDataSource (with handle) failed, error KErrNotSupported"));
	delete sourceHandlePtr;
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0512
//
// Add a data source which has already been added to a controller
//
// REQ172.5.5.1

RTestMmfCtlfrmI0512* RTestMmfCtlfrmI0512::NewL()
	{
	RTestMmfCtlfrmI0512* self = new(ELeave) RTestMmfCtlfrmI0512;
	return self;
	}

RTestMmfCtlfrmI0512::RTestMmfCtlfrmI0512()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0512");
	}


TVerdict RTestMmfCtlfrmI0512::DoTestStepL()
	{
	INFO_PRINTF1(_L("add data source where already added"));
	TInt error = KErrNone;

	// this source already exists in the controller
	_LIT(KTestWavFile, "c:\\TsiMmfCtlfrmData\\test.wav");
	iFileConfig().iPath = KTestWavFile;
	error = iController.AddDataSource(KUidMmfFileSource, iFileConfig);
	if (!error)
		{
		ERR_PRINTF1(_L("Error : AddDataSource succeeded"));
		return iTestStepResult = EFail;
		}
	else if (error != KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("AddDataSource failed with unexpected error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("AddDataSource failed, error KErrAlreadyExists"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0513
//
// Remove a source or sink which is currently in use (track playing)
//
// REQ172.5.5.1

RTestMmfCtlfrmI0513* RTestMmfCtlfrmI0513::NewL()
	{
	RTestMmfCtlfrmI0513* self = new(ELeave) RTestMmfCtlfrmI0513;
	return self;
	}

RTestMmfCtlfrmI0513::RTestMmfCtlfrmI0513()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0513");
	}

TVerdict RTestMmfCtlfrmI0513::DoTestStepL()
	{
	//XXX : we may have problems here. the only controller we have is the
	// Audio Controller and this does not yet support removal of sources/sinks,
	// whether it's playing or not...

	INFO_PRINTF1(_L("remove a source / sink when track playing"));

	TInt error = KErrNone;

	// set it playing
 	error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	
	// wait a little
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);

	// try and remove the data source
	TMMFMessageDestination& sourceHandle = *iSourceHandlePtr;
	error = iController.RemoveDataSource(sourceHandle);
	if (error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : RemoveDataSource returned error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("RemoveDataSource returned KErrNotReady"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0514
//
// Remove a source or sink which is not present in controller
//
// REQ172.5.5.2

RTestMmfCtlfrmI0514* RTestMmfCtlfrmI0514::NewL()
	{
	RTestMmfCtlfrmI0514* self = new(ELeave) RTestMmfCtlfrmI0514;
	return self;
	}

RTestMmfCtlfrmI0514::RTestMmfCtlfrmI0514()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0514");
	}


TVerdict RTestMmfCtlfrmI0514::DoTestStepL()
	{
	INFO_PRINTF1(_L("remove a nonexistent source / sink"));

	TInt error = KErrNone;

	// reset will remove the sources/sinks we added in the preamble
	error = iController.Reset();
	if(error)
		{
		ERR_PRINTF2(_L("Error : Reset failed, error %d"), error);	
		return iTestStepResult = EFail;
		}

	// try and remove the data source
	TMMFMessageDestination& sourceHandle = *iSourceHandlePtr;
	error = iController.RemoveDataSource(sourceHandle);
	if (!error)
		{
		ERR_PRINTF1(_L("Error : RemoveDataSource succeeded"));
		return iTestStepResult = EInconclusive;
		}
	else if (error != KErrNotFound)
		{
		ERR_PRINTF2(_L("RemoveDataSource failed with unexpected error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF1(_L("RemoveDataSource failed with error KErrNotFound"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0515
//
// Add a data source by UID, but with source info incompatible with the plugin
//
// REQ172.5.5.2

RTestMmfCtlfrmI0515* RTestMmfCtlfrmI0515::NewL()
	{
	RTestMmfCtlfrmI0515* self = new(ELeave) RTestMmfCtlfrmI0515;
	return self;
	}

RTestMmfCtlfrmI0515::RTestMmfCtlfrmI0515()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0515");
	}

TVerdict RTestMmfCtlfrmI0515::OpenL()
	{
	return iTestStepResult = EPass;
	// so we don't run the parent preamble
	}

void RTestMmfCtlfrmI0515::Close()
	{
	// so we don't run the parent postamble
	}

TVerdict RTestMmfCtlfrmI0515::DoTestStepL()
	{
	// NB : this test is no longer valid - see DEF001550 in TeamTrack.

	// Reject Report :
	// This is 'as designed' behaviour. The CMMFAudioController::AddDataSourceL function traps 
	// the call to CMMFFormatDecode::NewL, which leaves in this instance with error KErrNotSupported.
	// However it completes silently if KErrNotSupported is the error (see the comment on line 182
	// of MmfAudioController.cpp) 

	INFO_PRINTF1(_L("add incompatible data source by UID"));
	INFO_PRINTF1(_L("test no longer valid - see DEF001550 in TeamTrack"));
/*
	TInt error = KErrNone;

	iSettings.iPriority = ETSIMmfPriorityHigh;
	iSettings.iPref = EMdaPriorityPreferenceQuality;
	iSettings.iState = EMMFStateIdle;

	// Open a controller
	error = iController.Open(KTestAudioControllerUid, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// Attempt to add an incompatible source and sink
	_LIT(KTestXlsFile, "c:\\TsiMmfCtlfrmData\\test.xls");
	iFileConfig().iPath = KTestXlsFile;
	error = iController.AddDataSource(KUidMmfFileSource, iFileConfig);
	if (!error)
		{
		ERR_PRINTF1(_L("Error : AddDataSource succeeded"));
		return iTestStepResult = EFail;
		}
	else if (error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("AddDataSource failed with unexpected error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("AddDataSource failed, error KErrNotSupported"));

	error = iController.AddDataSink(KUidMmfFileSource, iFileConfig);
	if (!error)
		{
		ERR_PRINTF1(_L("Error : AddDataSink succeeded"));
		return iTestStepResult = EFail;
		}
	else if (error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("AddDataSink failed with unexpected error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("AddDataSink failed, error KErrNotSupported"));
*/
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0521
//
// Play an unprimed controller
//
// REQ172.5.5.6

RTestMmfCtlfrmI0521* RTestMmfCtlfrmI0521::NewL()
	{
	RTestMmfCtlfrmI0521* self = new(ELeave) RTestMmfCtlfrmI0521;
	return self;
	}

RTestMmfCtlfrmI0521::RTestMmfCtlfrmI0521()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0521");
	}


TVerdict RTestMmfCtlfrmI0521::DoTestStepL()
	{
	INFO_PRINTF1(_L("play an unprimed controller"));
	TInt error = KErrNone;

	// controller is fully setup in the preamble, but not primed
	// try to set it playing
 	error = iController.Play();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : play returned error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Play failed with error KErrNotReady"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0522
//
// Prime a controller which is already primed
//
// REQ172.5.5.6

RTestMmfCtlfrmI0522* RTestMmfCtlfrmI0522::NewL()
	{
	RTestMmfCtlfrmI0522* self = new(ELeave) RTestMmfCtlfrmI0522;
	return self;
	}

RTestMmfCtlfrmI0522::RTestMmfCtlfrmI0522()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0522");
	}


TVerdict RTestMmfCtlfrmI0522::DoTestStepL()
	{
	INFO_PRINTF1(_L("prime an already primed controller"));
	TInt error = KErrNone;

	// controller is fully setup in the preamble, but not primed
	// prime it, then prime it again
 	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Error : first prime returned error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// wait a while, to give it chance to prime
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);

	// prime a second time. this should succeed (KErrNone)
	// (defined in Audio Controller)
 	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Error : second prime returned error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Second prime returned success"));

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0523
//
// Play a controller where there is no defined source
//
// REQ172.5.5.6

RTestMmfCtlfrmI0523* RTestMmfCtlfrmI0523::NewL()
	{
	RTestMmfCtlfrmI0523* self = new(ELeave) RTestMmfCtlfrmI0523;
	return self;
	}

RTestMmfCtlfrmI0523::RTestMmfCtlfrmI0523()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0523");
	}

TVerdict RTestMmfCtlfrmI0523::DoTestStepL()
	{
	INFO_PRINTF1(_L("play a controller with no defined source"));
	TInt error = KErrNone;

	// controller is setup in the preamble, but has no source
	// try to set it playing
 	error = iController.Play();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : play returned error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Play failed with error KErrNotReady"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0524
//
// Play a controller which is already playing
//
// REQ172.5.5.6

RTestMmfCtlfrmI0524* RTestMmfCtlfrmI0524::NewL()
	{
	RTestMmfCtlfrmI0524* self = new(ELeave) RTestMmfCtlfrmI0524;
	return self;
	}

RTestMmfCtlfrmI0524::RTestMmfCtlfrmI0524()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0524");
	}


TVerdict RTestMmfCtlfrmI0524::DoTestStepL()
	{
	INFO_PRINTF1(_L("play a controller which is already playing"));
	TInt error = KErrNone;

	// controller is fully setup in the preamble, and primed
	// set it playing
 	error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Error : play returned error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// wait a while, to give it chance to start playing
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);

	// do it again :- should return KErrNotReady (this is defined in the
	// Audio Controller)
 	error = iController.Play();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : second play returned error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("Second play returned KErrNotReady"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0525
//
// Stop a controller which is not playing
//
// REQ172.5.5.7

RTestMmfCtlfrmI0525* RTestMmfCtlfrmI0525::NewL()
	{
	RTestMmfCtlfrmI0525* self = new(ELeave) RTestMmfCtlfrmI0525;
	return self;
	}

RTestMmfCtlfrmI0525::RTestMmfCtlfrmI0525()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0525");
	}


TVerdict RTestMmfCtlfrmI0525::DoTestStepL()
	{
	INFO_PRINTF1(_L("stop a controller which is not playing"));
	TInt error = KErrNone;

	// controller is fully setup in the preamble, and primed
	// play it, stop, try and stop again
 	error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Error : play returned error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	// wait a while, to give it chance to start playing
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);
	// stop it
	error = iController.Stop();
	if(error)
		{
		ERR_PRINTF2(_L("Error : stop returned error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// wait a while, to give it chance to stop
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);

	// now try and stop it a second time - should return KErrNone
	// (this is defined in Audio Controller)
	error = iController.Stop();
	if(error)
		{
		ERR_PRINTF2(_L("Error : stop returned error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Stop returned KErrNone"));

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0526
//
// Stop a controller where there is no defined source
//
// REQ172.5.5.7

RTestMmfCtlfrmI0526* RTestMmfCtlfrmI0526::NewL()
	{
	RTestMmfCtlfrmI0526* self = new(ELeave) RTestMmfCtlfrmI0526;
	return self;
	}

RTestMmfCtlfrmI0526::RTestMmfCtlfrmI0526()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0526");
	}


TVerdict RTestMmfCtlfrmI0526::DoTestStepL()
	{
	INFO_PRINTF1(_L("stop a controller with no defined source"));
	TInt error = KErrNone;

	// controller is setup in the preamble, but has no source
	// try to stop it
 	error = iController.Stop();
	if(error)
		{
		ERR_PRINTF2(_L("Error : stop returned error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Stop returned KErrNone"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0527
//
// Pause a controller which is stopped
//
// REQ172.5.5.8

RTestMmfCtlfrmI0527* RTestMmfCtlfrmI0527::NewL()
	{
	RTestMmfCtlfrmI0527* self = new(ELeave) RTestMmfCtlfrmI0527;
	return self;
	}

RTestMmfCtlfrmI0527::RTestMmfCtlfrmI0527()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0527");
	}


TVerdict RTestMmfCtlfrmI0527::DoTestStepL()
	{
	INFO_PRINTF1(_L("pause a controller which is stopped"));
	TInt error = KErrNone;

	// controller is fully setup in the preamble, and primed
	// play it, stop, try and pause
 	error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Error : play returned error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	// wait a while, to give it chance to start playing
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);
	// stop it
	error = iController.Stop();
	if(error)
		{
		ERR_PRINTF2(_L("Error : stop returned error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	// wait a while, to give it chance to stop
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);

	// now try and pause. this should return KErrNotReady
	error = iController.Pause();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : pause returned error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Pause returned KErrNotReady"));

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0531
//
// Assign a priority to a controller where it is the only controller loaded
//
// REQ172.5.5.9

RTestMmfCtlfrmI0531* RTestMmfCtlfrmI0531::NewL()
	{
	RTestMmfCtlfrmI0531* self = new(ELeave) RTestMmfCtlfrmI0531;
	return self;
	}

RTestMmfCtlfrmI0531::RTestMmfCtlfrmI0531()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0531");
	}

TVerdict RTestMmfCtlfrmI0531::DoTestStepL()
	{
	INFO_PRINTF1(_L("Assign priority to lone controller"));
	TInt error = KErrNone;

	// this will succeed - priorities are relative, not absolute
	iSettings.iPriority = ETSIMmfPriorityLow;
	iSettings.iPref = EMdaPriorityPreferenceTime;
	iSettings.iState = EMMFStateIdle;

	// NB controller should be Stopped (not Primed) to do this)
	error = iController.SetPrioritySettings(iSettings);
	if(error)
		{
		ERR_PRINTF2(_L("Error : SetPrioritySettings returned %d"), error);	
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("SetPrioritySettings succeeded"));

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0532
//
// Assign identical or conflicting priorities to two controllers
//
// REQ172.5.5.9

RTestMmfCtlfrmI0532* RTestMmfCtlfrmI0532::NewL()
	{
	RTestMmfCtlfrmI0532* self = new(ELeave) RTestMmfCtlfrmI0532;
	return self;
	}

RTestMmfCtlfrmI0532::RTestMmfCtlfrmI0532()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0532");
	}

TVerdict RTestMmfCtlfrmI0532::DoTestStepL()
	{
	INFO_PRINTF1(_L("Assign identical priority to two controllers"));
	TInt error = KErrNone;

	// NB : the preamble for this class sets the controllers with identical settings
	// including priority

	// prime them
	error = iController1.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("controller1 prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	error = iController2.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("controller2 prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// at last we are ready to do the test...
	// play both controllers. give the first time to start before playing the second.
	// we should get an error.
	error = iController1.Play();
	if(error)
		{
		ERR_PRINTF2(_L("iController1 play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	// wait a while, to give it chance to play
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);
	error = iController2.Play();
	if(error)
		{
		ERR_PRINTF2(_L("iController2 play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// search for a KErrInUse event from the MMF controller framework on controller 2
	TMMFEvent inUseEvent( KMMFEventCategoryPlaybackComplete, KErrInUse );
	TVerdict result;
	result = SearchForEvent( inUseEvent, 2 );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("MMF controller framework did not return a KErrInUse event"));
		return iTestStepResult = EFail;
		}
	
	ERR_PRINTF1(_L("MMF controller framework returned a KErrInUse event"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0541
//
// Get position of a source where there is no defined source
//
// REQ172.5.5.10

RTestMmfCtlfrmI0541* RTestMmfCtlfrmI0541::NewL()
	{
	RTestMmfCtlfrmI0541* self = new(ELeave) RTestMmfCtlfrmI0541;
	return self;
	}

RTestMmfCtlfrmI0541::RTestMmfCtlfrmI0541()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0541");
	}

TVerdict RTestMmfCtlfrmI0541::DoTestStepL()
	{
	INFO_PRINTF1(_L("Get position of undefined source"));
	TInt error = KErrNone;

	// controller is setup in the preamble, but has no source
	// try to get a clip position
	TTimeIntervalMicroSeconds clipPos;
	error = iController.GetPosition(clipPos);
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : GetPosition returned error %d"), error);	
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("GetPosition returned KErrNotReady"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0542
//
// Set position of a source to beyond the end of the data
//
// REQ172.5.5.11

RTestMmfCtlfrmI0542* RTestMmfCtlfrmI0542::NewL()
	{
	RTestMmfCtlfrmI0542* self = new(ELeave) RTestMmfCtlfrmI0542;
	return self;
	}

RTestMmfCtlfrmI0542::RTestMmfCtlfrmI0542()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0542");
	}

TVerdict RTestMmfCtlfrmI0542::DoTestStepL()
	{
	// NB this test has changed
	// originally we expected the position to set to the end of the clip.
	// the Audio Controller is now in fact returning KErrArgument
	INFO_PRINTF1(_L("Set position to beyond end of source data"));
	TInt error = KErrNone;
	
	// we know our clip is 5.33 sec :- set to way past the end
	const TTimeIntervalMicroSeconds KPastEndPosition(20000000);	// 20 sec
	const TTimeIntervalMicroSeconds KPlayPosition(5330000);	
	// NB allow for resolution of the controller itself
	const TTimeIntervalMicroSeconds KPlayPositionPlusError(5830000);
	const TTimeIntervalMicroSeconds KPlayPositionMinusError(4830000); // allow +/-0.5 sec
	error = iController.SetPosition(KPastEndPosition);
	if(error != KErrArgument)
		{
		ERR_PRINTF3(_L("SetPosition returned error %d (expected %d)"), error, KErrArgument);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("SetPosition returned KErrArgument"));
	// now set it to the actual end of the clip, and try again
	error = iController.SetPosition(KPlayPosition);
	if(error)
		{
		ERR_PRINTF3(_L("SetPosition returned error %d (expected %d)"), error, KErrNone);
		return iTestStepResult = EFail;
		}

	TTimeIntervalMicroSeconds clipPos;
	error = iController.GetPosition(clipPos);
	if(error)
		{
		ERR_PRINTF2(_L("Error : GetPosition failed, error %d"), error);	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF3(_L("Clip position : %ld Expected : %ld"), I64LOW(clipPos.Int64()), I64LOW(KPlayPosition.Int64()));
	if( (clipPos < (KPlayPositionMinusError)) || (clipPos > (KPlayPositionPlusError)) )
		{
		ERR_PRINTF1(_L("Error : clip position not set correctly"));
		return iTestStepResult = EFail;
		}

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0551
//
// Close a controller when source is still playing
//
// REQ172.5.5.12

RTestMmfCtlfrmI0551* RTestMmfCtlfrmI0551::NewL()
	{
	RTestMmfCtlfrmI0551* self = new(ELeave) RTestMmfCtlfrmI0551;
	return self;
	}

RTestMmfCtlfrmI0551::RTestMmfCtlfrmI0551()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0551");
	}

TVerdict RTestMmfCtlfrmI0551::DoTestStepL()
	{
	INFO_PRINTF1(_L("Close controller while still playing"));
	TInt error = KErrNone;
	
	// controller is fully setup in the preamble, and primed
	// play it, then close
 	error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Error : play returned error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// wait a while, to give it chance to start playing
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), I64LOW(KPlayPosition32.Int()));
	User::After(KPlayPosition32);

	iController.Close();

	// Close() returns void - so instead demonstrate that the controller
	// has been closed - Reset will return KErrNotReady on a closed controller;
	// on an open controller it will succeed in any state.

 	error = iController.Reset();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : reset returned error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("Reset returned KErrNotReady; close succeeded"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0552
//
// Reset a controller when source is still playing
//
// REQ172.5.5.12

RTestMmfCtlfrmI0552* RTestMmfCtlfrmI0552::NewL()
	{
	RTestMmfCtlfrmI0552* self = new(ELeave) RTestMmfCtlfrmI0552;
	return self;
	}

RTestMmfCtlfrmI0552::RTestMmfCtlfrmI0552()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0552");
	}

TVerdict RTestMmfCtlfrmI0552::DoTestStepL()
	{
	INFO_PRINTF1(_L("Reset controller while still playing"));
	TInt error = KErrNone;
	
	// controller is fully setup in the preamble, and primed
	// play it, then reset
 	error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Error : play returned error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// wait a while, to give it chance to start playing
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), I64LOW(KPlayPosition32.Int()));
	User::After(KPlayPosition32);

	error = iController.Reset();
	if(error)
		{
		ERR_PRINTF2(_L("Error : Reset failed, error %d"), error);	
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("Reset succeeded"));
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0553
//
// Close an already closed controller
//
// REQ172.5.5.12

RTestMmfCtlfrmI0553* RTestMmfCtlfrmI0553::NewL()
	{
	RTestMmfCtlfrmI0553* self = new(ELeave) RTestMmfCtlfrmI0553;
	return self;
	}

RTestMmfCtlfrmI0553::RTestMmfCtlfrmI0553()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0553");
	}

TVerdict RTestMmfCtlfrmI0553::DoTestStepL()
	{
	INFO_PRINTF1(_L("Close an already closed controller"));
	TInt error = KErrNone;
	
	// controller is fully setup in the preamble, and primed
	// close it, check that code on reset is KErrNotReady,
	// then close it again

	iController.Close();
 	error = iController.Reset();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : reset returned error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Reset (1) returned KErrNotReady; close succeeded"));

	// if the second close caused problems, we will leave or get an error from Reset
	iController.Close();
 	error = iController.Reset();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : reset returned error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("Reset (2) returned KErrNotReady; close succeeded"));

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0554
//
// Reset an already closed controller
//
// REQ172.5.5.12

RTestMmfCtlfrmI0554* RTestMmfCtlfrmI0554::NewL()
	{
	RTestMmfCtlfrmI0554* self = new(ELeave) RTestMmfCtlfrmI0554;
	return self;
	}

RTestMmfCtlfrmI0554::RTestMmfCtlfrmI0554()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0554");
	}

TVerdict RTestMmfCtlfrmI0554::DoTestStepL()
	{
	INFO_PRINTF1(_L("Reset an already closed controller"));
	TInt error = KErrNone;
	
	// controller is fully setup in the preamble, and primed
	// close it, then attempt a reset

	iController.Close();
	// Close() returns void
 	// wait a while, to give it a chance to complete
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), I64LOW(KPlayPosition32.Int()));
	User::After(KPlayPosition32);

	error = iController.Reset();
	if(error != KErrNotReady)
		{
		// this will fail if the controller hasn't been closed
		ERR_PRINTF2(_L("Error : reset returned error %d"), error);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("Reset returned KErrNotReady"));
	return iTestStepResult = EPass;
	}

