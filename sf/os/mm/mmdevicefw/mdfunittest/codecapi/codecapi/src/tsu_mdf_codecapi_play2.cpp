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

#include "tsu_mdf_codecapi_play2.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>

//
// RTestStepCodecApiPlayPartial
// play a file and stop it after N seconds
	
/**
 * Constructor
 */
RTestStepCodecApiPlayPartial::RTestStepCodecApiPlayPartial()
 	{
 	iTestStepName = _L("MM-MDF-CODECAPI-U-0101-HP");
 	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPlayPartial::DoTestStepL()
	{
	iPlayAudioFile = CPlayAudioFilePartial::NewL(this);
	iPlayAudioFile->StartL();
	delete iPlayAudioFile;
	return iVerdict;
	}	

//
// CPlayAudioFilePartial
// Active player for RTestStepCodecApiPlayPartial and derived test classes.

/**
 * Constructor
 */
CPlayAudioFilePartial::CPlayAudioFilePartial(RTestStepCodecApi* aParent) 
  : CPlayAudioFile(aParent)
	{
	}

/**
 * ConstructL
 */
void CPlayAudioFilePartial::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

/**
 * NewL
 */
CPlayAudioFilePartial* CPlayAudioFilePartial::NewL(RTestStepCodecApi* aParent)
	{
	CPlayAudioFilePartial* self = new (ELeave) CPlayAudioFilePartial(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
 * Destructor
 */
CPlayAudioFilePartial::~CPlayAudioFilePartial()
	{
	}	

/**
 * @see CActive
 */
void CPlayAudioFilePartial::RunL()
	{
	THwDeviceInitParams initParams;
	TInt err;	
	switch (iState)
		{
		case EHwDeviceCreateAndInit:
			{
			err = OMX_Init();
			if (err != KErrNone)
				{
				iParent->SetVerdict(_L("The OMX Core cannot be initialised"), EFail);
				SetState(EHwDeviceError);
				}	
			initParams.iHwDeviceObserver = this;
			TRAP(err, iHwDevice = CMMFHwDevice::NewL(TUid::Uid(KUidHwDeviceAudioAdapter)));
			if (err != KErrNone)
				{
				iParent->SetVerdict(_L("The Hw Device Adapter cannot be created"), EFail);
				OMX_Deinit();
				SetState(EHwDeviceError);
				}
			MMdfHwDeviceSetup* setup = (MMdfHwDeviceSetup*)iHwDevice->CustomInterface(KUidHwDeviceSetupInterface);
			if (setup != NULL)
				{
				//Play is pcmU8 -> pcm16
				setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8, KMMFFourCCCodePCM16);
				}				
			err = iHwDevice->Init(initParams);
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot be initialised"));
				break;
				}	
			iParent->SetVerdict(_L("The Hw Device Adapter created & initialised"));	

			// if we are playing a file, first we have to open the file
			err = ReadTestFileInBuffer();
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("Cannot open the file to be played"));
				break;
				}
			iParent->SetVerdict(_L("The file to be played has opened successfully"));
			
			SetState(EHwDeviceStartDecode);
			break;
			}
		case EHwDeviceStartDecode:
			{
			// tell the HwDeviceAdapter to play the file
			err = iHwDevice->Start(EDevDecode, EDevInFlow);
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot start"));
				break;
				}
			iParent->SetVerdict(_L("The Hw Device Adapter has started successfully"));
			
			// set a timer whose RunL() should stop playback after N seconds
			iPlayTimer = CMdfTimerPlayStop::NewL(this, iHwDevice, 1000000);
			iPlayTimer->Start();
			break;
			}
		case EHwDeviceAllowToComplete:
			break;
		case EHwDeviceDone:
			{
			delete iHwDevice;
			OMX_Deinit();	
			Cancel();
			CActiveScheduler::Stop();
			break;
			}				
		case EHwDeviceError:
			{
			CActiveScheduler::Stop();
			break;
			}
		default:
			{
			CleanupAndSetDeviceError(_L("Unknown CPlayAudioFile iState"));
			break;
			}
		}
	}

//
// RTestStepCodecApiPlayStopDelete
// play a file, stop and delete the codec after N seconds
	
/**
 * Constructor
 */
RTestStepCodecApiPlayStopDelete::RTestStepCodecApiPlayStopDelete()
 	{
 	iTestStepName = _L("MM-MDF-CODECAPI-U-0145-HP");
 	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPlayStopDelete::DoTestStepL()
	{
	iPlayAudioFile = CPlayAudioFileStopDelete::NewL(this);
	iPlayAudioFile->StartL();
	delete iPlayAudioFile;
	return iVerdict;
	}	

//
// CPlayAudioFileStopDelete
// Active player for RTestStepCodecApiPlayStopDelete and derived test classes.

/**
 * Constructor
 */
CPlayAudioFileStopDelete::CPlayAudioFileStopDelete(RTestStepCodecApi* aParent) 
  : CPlayAudioFilePartial(aParent)
	{
	}

/**
 * ConstructL
 */
void CPlayAudioFileStopDelete::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

/**
 * NewL
 */
CPlayAudioFileStopDelete* CPlayAudioFileStopDelete::NewL(RTestStepCodecApi* aParent)
	{
	CPlayAudioFileStopDelete* self = new (ELeave) CPlayAudioFileStopDelete(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
 * Destructor
 */
CPlayAudioFileStopDelete::~CPlayAudioFileStopDelete()
	{
	}	

/**
 * set finished on callback from a timer
 */
void CPlayAudioFileStopDelete::Finished()
	{
	iParent->SetVerdict(_L("Finished - calling StopAndDeleteCodec"));
	
	// NB even if this is stubbed (i.e. audio play continues) the test will pass!
	TInt err = iHwDevice->StopAndDeleteCodec();
	if(err != KErrNone) 
		{
		iParent->SetVerdict(_L("StopAndDeleteCodec returned an error"), EFail);
		}
	SetState(EHwDeviceDone);
	}
	
//
// RTestStepCodecApiPlayStopRestart
// play a file, stop and start playing again after N seconds
	
/**
 * Constructor
 */
RTestStepCodecApiPlayStopRestart::RTestStepCodecApiPlayStopRestart()
 	{
 	iTestStepName = _L("MM-MDF-CODECAPI-U-0125-HP");
 	}


/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPlayStopRestart::DoTestStepL()
	{
	iPlayAudioFile = CPlayAudioFileStopRestart::NewL(this);
	iPlayAudioFile->StartL();
	delete iPlayAudioFile;
	return iVerdict;
	}	

//
// CPlayAudioFileStopRestart
// Active player for RTestStepCodecApiPlayStopRestart and derived test classes.

/**
 * Constructor
 */
CPlayAudioFileStopRestart::CPlayAudioFileStopRestart(RTestStepCodecApi* aParent) 
  : CPlayAudioFile(aParent)
	{
	}

/**
 * ConstructL
 */
void CPlayAudioFileStopRestart::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

/**
 * NewL
 */
CPlayAudioFileStopRestart* CPlayAudioFileStopRestart::NewL(RTestStepCodecApi* aParent)
	{
	CPlayAudioFileStopRestart* self = new (ELeave) CPlayAudioFileStopRestart(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
 * Destructor
 */
CPlayAudioFileStopRestart::~CPlayAudioFileStopRestart()
	{
	}	

/**
 * @see CActive
 */
void CPlayAudioFileStopRestart::RunL()
	{
	THwDeviceInitParams initParams;
	TInt err;	
	switch (iState)
		{
		case EHwDeviceCreateAndInit:
			{
			iRestarted = EFalse;
			err = OMX_Init();
			if (err != KErrNone)
				{
				iParent->SetVerdict(_L("The OMX Core cannot be initialised"), EFail);
				SetState(EHwDeviceError);
				}	
			initParams.iHwDeviceObserver = this;
			TRAP(err, iHwDevice = CMMFHwDevice::NewL(TUid::Uid(KUidHwDeviceAudioAdapter)));
			if (err != KErrNone)
				{
				iParent->SetVerdict(_L("The Hw Device Adapter cannot be created"), EFail);
				OMX_Deinit();
				SetState(EHwDeviceError);
				}
			MMdfHwDeviceSetup* setup = (MMdfHwDeviceSetup*)iHwDevice->CustomInterface(KUidHwDeviceSetupInterface);
			if (setup != NULL)
				{
				//Play is pcmU8 -> pcm16
				setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8, KMMFFourCCCodePCM16);
				}				
			err = iHwDevice->Init(initParams);
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot be initialised"));
				break;
				}	
			iParent->SetVerdict(_L("The Hw Device Adapter created & initialised"));	

			// if we are playing a file, first we have to open the file
			err = ReadTestFileInBuffer();
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("Cannot open the file to be played"));
				break;
				}
			iParent->SetVerdict(_L("The files to be played has opened successfully"));
			
			SetState(EHwDeviceStartDecode);
			break;
			}
		case EHwDeviceStartDecode:
			{
			// tell the HwDeviceAdapter to play the file
			err = iHwDevice->Start(EDevDecode, EDevInFlow);
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot start"));
				break;
				}
			iParent->SetVerdict(_L("The Hw Device Adapter has started successfully"));
			
			// only run the timer at the beginning - after restart, run until end of file
			if(!iRestarted) 
				{
				iPlayTimer = CMdfTimerPlayStop::NewL(this, iHwDevice, 1000000);
				iPlayTimer->Start();
				}
			break;
			}
		case EHwDeviceAllowToComplete:
			break;
		case EHwDeviceDone:
			{
			// restart once, play to the end
			if(!iRestarted) 
				{
				iParent->SetVerdict(_L("Restarting..."));
				iFinished = EFalse;
				iRestarted = ETrue;
				SetState(EHwDeviceStartDecode);
				break;
				}
			delete iHwDevice;
			OMX_Deinit();	
			Cancel();
			CActiveScheduler::Stop();
			break;
			}				
		case EHwDeviceError:
			{
			CActiveScheduler::Stop();
			break;
			}
		default:
			{
			CleanupAndSetDeviceError(_L("Unknown CPlayAudioFile iState"));
			break;
			}
		}
	}

//
// CMdfTimerPlayStop
// Timeout timer for those test classes requiring timed stop

/**
 * NewL
 */
CMdfTimerPlayStop* CMdfTimerPlayStop::NewL(CPlayAudioFile* aParent, 
	CMMFHwDevice* aHwDevice, TTimeIntervalMicroSeconds32 aWaitTime)
	{
	CMdfTimerPlayStop* self = new (ELeave) CMdfTimerPlayStop(aParent, aHwDevice, aWaitTime);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
/**
 * Constructor
 */
CMdfTimerPlayStop::CMdfTimerPlayStop(CPlayAudioFile* aParent,
		CMMFHwDevice* aHwDevice, TTimeIntervalMicroSeconds32 aWaitTime)
	: CTimer(EPriorityNormal), iParent(aParent), iHwDevice(aHwDevice), iWaitTime(aWaitTime)
	{	
	CActiveScheduler::Add(this);
	}	
	
/**
 * Start the timer
 */
void CMdfTimerPlayStop::Start()
	{
	After(iWaitTime);
	}

/**
 * @see CActive
 */
void CMdfTimerPlayStop::RunL()
	{		
	// it's not clear how we send the HwDevice a Stop() and get callbacks
	// from it. I think under the current scheme we have to stop sending
	// it data, explicitly let it know that the next buffer is blank
	// and is the last one. I'm not sure this is how it should be.

	// (iParent->Finished() should call Stop() or not, as we decide)	
	iParent->Finished();
	}

/**
 * @see CActive
 */
TInt CMdfTimerPlayStop::RunError(TInt aError)
	{
	iParent->CleanupAndSetDeviceError(_L("CMdfTimerPlayStop::RunL left due to problem stopping HwDevice"));
	return aError;
	}

// end
