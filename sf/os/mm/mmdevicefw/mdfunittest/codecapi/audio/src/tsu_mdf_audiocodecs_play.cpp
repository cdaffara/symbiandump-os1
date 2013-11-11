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

#include "tsu_mdf_audiocodecs_play.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>

	
RTestStepAudioCodecPlay::RTestStepAudioCodecPlay()
 	{
 	iTestStepName = _L("MM-MDF-AUDIOCODECS-0002");
 	}


void RTestStepAudioCodecPlay::SetVerdict(TPtrC16 aText, TVerdict aVerdict)
	{
	iVerdict = aVerdict;
	INFO_PRINTF2(_L("---> %s"), aText.Ptr());
	}

TVerdict RTestStepAudioCodecPlay::DoTestStepL()
	{
	iPlayAudioFile = CPlayAudioFile::NewL(this);
	iPlayAudioFile->StartL();
	delete iPlayAudioFile;
	return iVerdict;
	}	

/*
/
/
/	Play implementation
/
/
*/	

// **************************************************
// instructs the Hw Device Adapter to play a file	
// **************************************************
CPlayAudioFile::CPlayAudioFile(RTestStepAudioCodecs* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceCreateAndInit),	
	iParent(aParent)
	{
	}

void  CPlayAudioFile::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CPlayAudioFile* CPlayAudioFile::NewL(RTestStepAudioCodecs* aParent)
	{
	CPlayAudioFile* self = new (ELeave) CPlayAudioFile(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CPlayAudioFile::StartL()	
	{
	SetState(EHwDeviceCreateAndInit);
	CActiveScheduler::Start();
	}
	
void CPlayAudioFile::DoCancel()
	{
	}

CPlayAudioFile::~CPlayAudioFile()
	{
	}	

void CPlayAudioFile::SetState(TPlayAudioFileState aState)
	{
	iState = aState;
	SetActive();	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);	
	}

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


TInt CPlayAudioFile::FillThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{
	iParent->SetVerdict(_L("FillThisHwBuffer"));
	ReadNextBuffer(aHwDataBuffer);
	return KErrNone;
	}


TInt CPlayAudioFile::EmptyThisHwBuffer(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return KErrNotSupported;
	}
	
void CPlayAudioFile::ReadNextBuffer(CMMFBuffer& aHwDataBuffer)
	{
	TBool finished = EFalse;
	TInt length = iSourceFile->Size();
	if (iSourceFilePos < length)
		{
		TInt size = length - iSourceFilePos;
		CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(&aHwDataBuffer);
		if (size > dataBuffer->Data().MaxLength())
			{
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
	iHwDevice->ThisHwBufferFilled(aHwDataBuffer);
	if (finished)
		{
		SetState(EHwDeviceAllowToComplete);
		}
	}

void CPlayAudioFile::RunL()
	{
	THwDeviceInitParams initParams;
	TInt err = KErrNone;	
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
				
				TRAPD(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8, KMMFFourCCCodePCM16));
				if (err == KErrNotFound)
					{
					iParent->SetVerdict(_L("Cannot load the PU"), EFail);
					delete iHwDevice;
					OMX_Deinit();
					SetState(EHwDeviceError);
					break;
					}
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
			// if we are playing a file, first we have to open the file
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
	
void CPlayAudioFile::CleanupAndSetDeviceError(TPtrC16 aText)
	{
	iParent->SetVerdict(aText, EFail);
	delete iHwDevice;
	OMX_Deinit();	
	SetState(EHwDeviceError);
	}

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
	
void CPlayAudioFile::Stopped()
	{
	iStoppedReceived = ETrue;
	}

/*
/
/
/ End of Play Implementation
/
/
*/	
