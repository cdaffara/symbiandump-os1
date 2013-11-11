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

#include "tsu_mdf_codecapi_play.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>

//
// RTestStepCodecApiPlay
// play a file from start to finish
	
/**
 * Constructor
 */
RTestStepCodecApiPlay::RTestStepCodecApiPlay()
 	{
 	iTestStepName = _L("MM-MDF-CODECAPI-U-0100-HP");
 	iHeapSize = 0x100000; // 1M
 	}


/**
 * So that CPlayAudioFile can set the test verdict and output messages
 */
void RTestStepCodecApiPlay::SetVerdict(TPtrC16 aText, TVerdict aVerdict)
	{
	iVerdict = aVerdict;
	INFO_PRINTF2(_L("---> %s"), aText.Ptr());
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPlay::DoTestStepL()
	{
	iPlayAudioFile = CPlayAudioFile::NewL(this);
	iPlayAudioFile->StartL();
	delete iPlayAudioFile;
	return iVerdict;
	}	

//
// CPlayAudioFile
// Active player for RTestStepCodecApiPlay and derived test classes.

/**
 * Constructor
 */
CPlayAudioFile::CPlayAudioFile(RTestStepCodecApi* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceCreateAndInit),
	iParent(aParent),
	iFinished(EFalse),
	iSourceFile(NULL),
	iSourceFilePos(0)
	{
	}

/**
 * ConstructL
 */
void CPlayAudioFile::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

/**
 * NewL
 */
CPlayAudioFile* CPlayAudioFile::NewL(RTestStepCodecApi* aParent)
	{
	CPlayAudioFile* self = new (ELeave) CPlayAudioFile(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
 * StartL
 */
void CPlayAudioFile::StartL()	
	{
	SetState(EHwDeviceCreateAndInit);
	CActiveScheduler::Start();
	}
	
/**
 * @see CActive
 */
void CPlayAudioFile::DoCancel()
	{
	}

/**
 * Destructor
 */
CPlayAudioFile::~CPlayAudioFile()
	{
	}	

/**
 * SetState
 */
void CPlayAudioFile::SetState(TPlayAudioFileState aState)
	{
	iState = aState;
	SetActive();	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);	
	}

/**
 * ReadTestFileInBuffer
 * reads file into buffer referenced by iSourceFile
 */
TInt CPlayAudioFile::ReadTestFileInBuffer()
	{
	RFs fs;
	TInt err = fs.Connect();
	if(err != KErrNone)
		{
		return err;
		}
	
	RFile file;
	err = file.Open(fs,_L("c:\\mm\\mmf\\testfiles\\mdf\\depeche1.wav"), EFileRead);
	if (err == KErrNone)
		{
		TInt size; 
		err = file.Size(size);
		if (err == KErrNone)
			{			
	 		iSourceFile = HBufC8::NewMax(size);
	 		if(!iSourceFile)
	 			{
	 			return KErrNoMemory;
	 			}
	 		TPtr8 ptr = iSourceFile->Des();
    		file.Read(ptr,size);
    		file.Close();
			}
		fs.Close();
		}
    return err;
	}

/**
 * @see MMMFHwDeviceObserver
 */
TInt CPlayAudioFile::FillThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{
	iParent->SetVerdict(_L("FillThisHwBuffer"));
	
	ReadNextBuffer(aHwDataBuffer);
	return KErrNone;
	}

/**
 * @see MMMFHwDeviceObserver
 */
TInt CPlayAudioFile::EmptyThisHwBuffer(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return KErrNotSupported;
	}
	
/**
 * fill a data buffer from iSourceFile
 */
void CPlayAudioFile::ReadNextBuffer(CMMFBuffer& aHwDataBuffer)
	{
	TBool finished = EFalse;
	TInt length = iSourceFile->Size();
	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(&aHwDataBuffer);
	if (!iFinished && iSourceFilePos < length)
		{
		TInt size = length - iSourceFilePos;
		if (size > dataBuffer->Data().MaxLength())
			{
			dataBuffer->SetLastBuffer(EFalse); // in case it was previously set
			size = dataBuffer->Data().MaxLength();
			}
		else
			{
			dataBuffer->SetLastBuffer(ETrue);
			finished = ETrue;
			}
		dataBuffer->Data().SetLength(size);
		Mem::Copy((TAny*)dataBuffer->Data().Ptr(), (TAny*)iSourceFile->Mid(iSourceFilePos).Ptr(), size);
		iSourceFilePos += size;
		}
	else 
		{
		// if we've been flagged to stop by a timer, we need to let the HwDevice
		// know that this (empty) buffer is the last one.
		// if we've run out of data, we still have to let the HwDevice know.
		dataBuffer->Data().SetLength(0);
		dataBuffer->SetLastBuffer(ETrue);
		finished = ETrue;
		}
	iHwDevice->ThisHwBufferFilled(aHwDataBuffer);
	if (finished)
		{
		SetState(EHwDeviceAllowToComplete);
		}
	}

/**
 * @see CActive
 */
void CPlayAudioFile::RunL()
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
			SetState(EHwDeviceStartDecode);
			break;
			}
		case EHwDeviceStartDecode:
			{
			// open the file
			err = ReadTestFileInBuffer();
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("Cannot open the file to be played"));
				break;
				}
			iParent->SetVerdict(_L("The files to be played has opened successfully"));
			
			// tell the HwDeviceAdapter to play the file
			err = iHwDevice->Start(EDevDecode, EDevInFlow);
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot start"));
				break;
				}
			iParent->SetVerdict(_L("The Hw Device Adapter has started successfully"));
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

/**
 * set iFinished flag on callback from a timer
 */
void CPlayAudioFile::Finished()
	{
	iParent->SetVerdict(_L("Finished"));
	iFinished = ETrue;
	
	// *** patched this out temporarily.
	// Stop() should send a Stopped() callback
	// else the test will just hang.
	// iHwDevice->Stop();
	}
	
/**
 * will close down the hardware device on error
 */
void CPlayAudioFile::CleanupAndSetDeviceError(TPtrC16 aText)
	{
	iParent->SetVerdict(aText, EFail);
	delete iHwDevice;
	OMX_Deinit();	
	SetState(EHwDeviceError);
	}

/**
 * @see MMMFHwDeviceObserver
 */
void CPlayAudioFile::Error(TInt aError)
	{
	// Completion is signified by KErrUnderflow	
	if (aError == KErrUnderflow && iStoppedReceived)
		{
		iParent->SetVerdict(_L("Success"));
		}
	else
		{
		iParent->SetVerdict(_L("Error from Hw Device"), EFail);
		}
	SetState(EHwDeviceDone);	
	}
	
/**
 * @see MMMFHwDeviceObserver
 */
void CPlayAudioFile::Stopped()
	{
	iParent->SetVerdict(_L("Stopped()"));
	iStoppedReceived = ETrue;
	}

// end
