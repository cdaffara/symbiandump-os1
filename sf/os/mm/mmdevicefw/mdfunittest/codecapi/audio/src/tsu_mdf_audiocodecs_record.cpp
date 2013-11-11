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

#include "tsu_mdf_audiocodecs_record.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>


RTestStepAudioCodecRecord::RTestStepAudioCodecRecord()
 	{
 	iTestStepName = _L("MM-MDF-AUDIOCODECS-0003");
 	}


void RTestStepAudioCodecRecord::SetVerdict(TPtrC16 aText, TVerdict aVerdict)
	{
	iVerdict = aVerdict;
	INFO_PRINTF2(_L("---> %s"), aText.Ptr());
	}

TVerdict RTestStepAudioCodecRecord::DoTestStepL()
	{
	iRecordAudioFile = CRecordAudioFile::NewL(this);
	iRecordAudioFile->StartL();
	delete iRecordAudioFile;
	return iVerdict;
	}

// **************************************************
// instructs the Hw Device Adapter to record a file	
// **************************************************
CRecordAudioFile::CRecordAudioFile(RTestStepAudioCodecs* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceCreateAndInit),
	iParent(aParent)
	{
	}

void  CRecordAudioFile::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CRecordAudioFile* CRecordAudioFile::NewL(RTestStepAudioCodecs* aParent)
	{
	CRecordAudioFile* self = new (ELeave) CRecordAudioFile(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CRecordAudioFile::StartL()	
	{
	SetState(EHwDeviceCreateAndInit);
	CActiveScheduler::Start();
	}
	
void CRecordAudioFile::DoCancel()
	{
	}

CRecordAudioFile::~CRecordAudioFile()
	{
	}	

void CRecordAudioFile::SetState(TRecordAudioFileState aState)
	{
	iState = aState;
	SetActive();	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);	
	}


TInt CRecordAudioFile::FillThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/)
	{	
	return KErrNotSupported;	
	}


TInt CRecordAudioFile::EmptyThisHwBuffer(CMMFBuffer& aEmptyBufferPtr)
	{
	iParent->SetVerdict(_L("ThisHwBufferEmptied"));	
	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(&aEmptyBufferPtr);
	if(iWrittenDataTotal == 0)
		{
		iBufferSize = dataBuffer->Data().Size();
		}
	TInt err = KErrNone;
	err = WriteDataToFile(dataBuffer->Data());
	if(err != KErrNone)
		{
		return err;
		}
		
	if(dataBuffer->LastBuffer())
		{
		SetState(EHwDeviceEncodeCheckData);
		iLastBufferSize = dataBuffer->Data().Size();		
		iHwDevice->Stop();
		}
	else
		{
		err = iHwDevice->ThisHwBufferEmptied(aEmptyBufferPtr);	
		}
		
	return err;
	}

TInt CRecordAudioFile::WriteDataToFile(TDes8& aData)
	{
	TInt err = KErrNone;
	err = iRecordFile.Write(aData);
	if(err != KErrNone)
		{
		return err;
		}
	// keep record of amount of data and the number of buffers written out
	iWrittenDataTotal += aData.Size();	
	iBuffersWrittenCount++;
	return err;
	}
	
TInt CRecordAudioFile::SetUpRecordFile()
	{
	iFileServerSession.Connect();
	TInt err = iRecordFile.Replace(iFileServerSession, KTestRecordedFile, EFileWrite|EFileShareExclusive);	
	return err;
	}

void CRecordAudioFile::CloseRecordFile()
	{
	iRecordFile.Close();
	iFileServerSession.Close();	
	}

void CRecordAudioFile::RunL()
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
				//Record is pcm16 -> pcmU8	
				TRAPD(err, setup->SetDataTypesL(KMMFFourCCCodePCM16, KMMFFourCCCodeTestPCMU8));	
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
			SetState(EHwDeviceStartEncode);			
			break;
			}
		case EHwDeviceStartEncode:
			{			
			User::LeaveIfError(SetUpRecordFile());			
			// tell the HwDeviceAdapter to record the file
			err = iHwDevice->Start(EDevEncode, EDevOutFlow);
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot start"));
				break;
				}
			iParent->SetVerdict(_L("The Hw Device Adapter has started successfully"));
			iRecordTimer = CMdfTimerRecordStop::NewL(this, iHwDevice, 3000000);
			iRecordTimer->Start();
			SetState(EHwDeviceAllowToComplete);			
			break;
			}
		case EHwDeviceEncodeCheckData:			
			{
			iParent->SetVerdict(_L("The Hw Device Adapter has stopped successfully"));
			TInt fileSize = 0;
			err = iRecordFile.Size(fileSize);
			if(err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("Problem with accessing the file after writing to it"));
				break;
				}
			TInt dataWritten = ((iBuffersWrittenCount - 1) * iBufferSize) + iLastBufferSize;	
			if(fileSize != dataWritten)			
				{
				CleanupAndSetDeviceError(_L("Unexpected amount of data written to file"));
				break;
				}
			//iParent->SetVerdict(_L("Encode was a success"));
			SetState(EHwDeviceDone);
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
			CloseRecordFile();
			break;
			}
		case EHwDeviceError:
			{
			CActiveScheduler::Stop();
			CloseRecordFile();
			break;
			}
		default:
			{
			CleanupAndSetDeviceError(_L("Unknown CRecordAudioFile iState"));
			break;
			}
		}
	}
	
void CRecordAudioFile::CleanupAndSetDeviceError(TPtrC16 aText)
	{
	iParent->SetVerdict(aText, EFail);
	delete iHwDevice;
	OMX_Deinit();	
	SetState(EHwDeviceError);
	}

void CRecordAudioFile::Error(TInt aError)
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
	
void CRecordAudioFile::Stopped()
	{
	iStoppedReceived = ETrue;
	}

/*
/
/
/ End of Record Implementation
/
/
*/
CMdfTimerRecordStop* CMdfTimerRecordStop::NewL(CRecordAudioFile* aParent, 
	CMMFHwDevice* aHwDevice, TTimeIntervalMicroSeconds32 aWaitTime)
	{
	CMdfTimerRecordStop* self = new (ELeave) CMdfTimerRecordStop(aParent, aHwDevice, aWaitTime);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
CMdfTimerRecordStop::CMdfTimerRecordStop(CRecordAudioFile* aParent,
		CMMFHwDevice* aHwDevice, TTimeIntervalMicroSeconds32 aWaitTime)
	: CTimer(EPriorityNormal), iParent(aParent), iHwDevice(aHwDevice), iWaitTime(aWaitTime)
	{	
	CActiveScheduler::Add(this);
	}	
	
void CMdfTimerRecordStop::Start()
	{
	After(iWaitTime);
	}

void CMdfTimerRecordStop::RunL()
	{		
	User::LeaveIfError(iHwDevice->Pause());	 
	// Need to wait for LastBuffer to be processed by the AudioDevice then call Stop()		 
	}

TInt CMdfTimerRecordStop::RunError(TInt aError)
	{
	iParent->CleanupAndSetDeviceError(_L("CMdfTimerRecordStop::RunL() left due to problem stopping HwDevice"));
	return aError;
	}
