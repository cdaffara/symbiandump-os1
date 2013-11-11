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

/**
 @file
 @internalComponent
*/

#include "tsu_mdf_omxvorbiscodecs_record.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/mdfpuconfig.h>

// for the bitrate custom interface
#include <mmf/server/devsoundstandardcustominterfaces.h>

//
// RTestStepOmxVorbisCodecRecord
// record a Vorbis file

/**
 * Constructor
 */
RTestStepOmxVorbisCodecRecord::RTestStepOmxVorbisCodecRecord()
 	{
 	iTestStepName = _L("MM-MDF-OMXVORBISCODECS-U-0002-HP");
	iHeapSize = KTestHeapSize;
 	}

/**
 * So that CRecordAudioFile can set the test verdict and output messages
 */
void RTestStepOmxVorbisCodecRecord::SetVerdict(TPtrC16 aText, TVerdict aVerdict)
	{
	iVerdict = aVerdict;
	INFO_PRINTF2(_L("---> %s"), aText.Ptr());
	}

/**
 * Do the test step
 */
TVerdict RTestStepOmxVorbisCodecRecord::DoTestStepL()
	{
	iRecordAudioFile = CRecordAudioFile::NewL(this);
	iRecordAudioFile->StartL();
	delete iRecordAudioFile;
	return iVerdict;
	}

// instructs the Hw Device Adapter to record a file	
CRecordAudioFile::CRecordAudioFile(RTestStepOmxVorbisCodecs* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceCreateAndInit),
	iParent(aParent)
	{
	}

//
// CRecordAudioFile
// Active recorder for RTestStepOmxVorbisCodecRecord and derived test classes.

/**
 * ConstructL
 */
void CRecordAudioFile::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

/**
 * NewL
 */
CRecordAudioFile* CRecordAudioFile::NewL(RTestStepOmxVorbisCodecs* aParent)
	{
	CRecordAudioFile* self = new (ELeave) CRecordAudioFile(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
 * StartL
 */
void CRecordAudioFile::StartL()	
	{
	SetState(EHwDeviceCreateAndInit);
	CActiveScheduler::Start();
	}
	
/**
 * @see CActive
 */
void CRecordAudioFile::DoCancel()
	{
	}

/**
 * Destructor
 */
CRecordAudioFile::~CRecordAudioFile()
	{
	}	

/**
 * SetState
 */
void CRecordAudioFile::SetState(TRecordAudioFileState aState)
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
TInt CRecordAudioFile::ReadTestFileInBuffer()
	{
	// we don't need to strip off the wav header

	RFs fs;
	fs.Connect();
	
	RFile file;
	TInt err = file.Open(fs, KTestEncoderPlayFile, EFileRead);
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
TInt CRecordAudioFile::FillThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{	
	// RDebug::Print(_L("CRecordAudioFile::FillThisHwBuffer"));

	ReadNextBuffer(aHwDataBuffer);
	return KErrNone;
	}

/**
 * @see MMMFHwDeviceObserver
 */
TInt CRecordAudioFile::EmptyThisHwBuffer(CMMFBuffer& aEmptyBufferPtr)
	{
	// RDebug::Print(_L("CRecordAudioFile::EmptyThisHwBuffer"));

	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(&aEmptyBufferPtr);
	TInt err = KErrNone;
	if(dataBuffer->Data().Size()) 
		{
		RDebug::Print(_L("CRecordAudioFile writing buffer size %d"), dataBuffer->Data().Size());
		err = WriteDataToFile(dataBuffer->Data());
		if(err != KErrNone)
			{
			return err;
			}
		}
	
	err = iHwDevice->ThisHwBufferEmptied(aEmptyBufferPtr);
	if(err != KErrNone)
		{
		return err;
		}

	if(dataBuffer->LastBuffer())
		{
		SetState(EHwDeviceEncodeCheckData);		
		iHwDevice->Stop();
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
	RDebug::Print(_L("CRecordAudioFile:  buffers written %d, total bytes %d"), iBuffersWrittenCount, iWrittenDataTotal);
	return err;
	}
	
/**
 * opens record file
 */
TInt CRecordAudioFile::SetUpRecordFile()
	{
	iFileServerSession.Connect();
	TInt err = iRecordFile.Replace(iFileServerSession, KTestEncoderRecordedFile, EFileWrite|EFileShareExclusive);	
	return err;
	}

/**
 * closes record file
 */
void CRecordAudioFile::CloseRecordFile()
	{
	iRecordFile.Close();
	iFileServerSession.Close();	
	}

/**
 * send chunks of data into the encoder.
 */
void CRecordAudioFile::ReadNextBuffer(CMMFBuffer& aHwDataBuffer)
    {
	// check if finished
	if (iFinished) 
		{
		iHwDevice->ThisHwBufferFilled(aHwDataBuffer);
		SetState(EHwDeviceAllowToComplete);
		return;
		}
		
	// get a chunk of data and send it straight to the encoder
	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(&aHwDataBuffer);
	
	TInt srcLength = iSourceFile->Size();
	if (iSourceFileReadPos < srcLength)
		{
	    TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
			
		TInt size = srcLength - iSourceFileReadPos;
		if (size > dataBuffer->Data().MaxLength())
			{
			size = dataBuffer->Data().MaxLength();			
			}
		Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Mid(iSourceFileReadPos).Ptr(), size);

		// set buffer size into the CMMFBuffer
		dataBuffer->Data().SetLength(size);
		iSourceFileReadPos += size;	
					
		RDebug::Print(_L("CRecordAudioFile::data read = %d bytes"), iSourceFileReadPos);
		
		// callback
		iHwDevice->ThisHwBufferFilled(aHwDataBuffer);
		}
	else 
		{
		// no more data
		RDebug::Print(_L("CRecordAudioFile::end of data"));
		iFinished = ETrue;
		dataBuffer->SetLastBuffer(ETrue);
		iHwDevice->ThisHwBufferFilled(aHwDataBuffer);
		SetState(EHwDeviceAllowToComplete);
		}
    }

/**
 * @see CActive
 */
void CRecordAudioFile::RunL()
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

			// now using the AudioCodecTestAdapter
			TRAP(err, iHwDevice = CMMFHwDevice::NewL(TUid::Uid(KUidHwDeviceAudioCodecTestAdapter)));
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
				setup->SetDataTypesL(KMMFFourCCCodePCM16, KMMFFourCCCodeTestVorbis);	
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
			// if we are playing a file, first we have to open the file
			err = ReadTestFileInBuffer();
			if (err == KErrNotFound)
				{
				CleanupAndSetDeviceError(_L("Cannot open the file to be played"));
				break;
				}
			iParent->SetVerdict(_L("The file to be played has opened successfully"));
			
			err = SetUpRecordFile();			
			if (err)
				{
				CleanupAndSetDeviceError(_L("Cannot open the file to record"));
				break;
				}
			
			// configure the HwDevice
			TTaskConfig config;
			config.iUid = KUidTTaskConfig;
			config.iRate = KTestSampleRate;
			config.iStereoMode = KTestNumChannels;
			err = iHwDevice->SetConfig(config);
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot configure"));
				break;
				}
				
			// set bitrate in the encoder - this requires a custom interface
			MMMFDevSoundCustomInterfaceBitRate* bitrateCI = (MMMFDevSoundCustomInterfaceBitRate*)iHwDevice->CustomInterface(KUidCustomInterfaceDevSoundBitRate);
			if (bitrateCI == NULL)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot find a bitrate custom interface"));
				break;
				}
			else 
				{
				TRAP(err, bitrateCI->SetBitRateL(KTestBitRate));			
				if (err != KErrNone)
					{
					CleanupAndSetDeviceError(_L("The Hw Device Adapter failed to set the bitrate"));
					break;
					}
				}
					
			// tell the HwDeviceAdapter to record the file
			err = iHwDevice->Start(EDevEncode, EDevOutFlow);
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("The Hw Device Adapter cannot start"));
				break;
				}
			iParent->SetVerdict(_L("The Hw Device Adapter has started successfully"));
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
				
			// check the recorded file against expected file size for the test to pass.
			if(fileSize != KTestEncoderRecordedFileExpectedSize) 
				{
				CleanupAndSetDeviceError(_L("Unexpected amount of data written to file"));
				break;
				}
			// encode succeeded
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
	
/**
 * will close down the hardware device on error
 */
void CRecordAudioFile::CleanupAndSetDeviceError(TPtrC16 aText)
	{
	iParent->SetVerdict(aText, EFail);
	delete iHwDevice;
	OMX_Deinit();	
	SetState(EHwDeviceError);
	}

/**
 * @see MMMFHwDeviceObserver
 */
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
	
/**
 * @see MMMFHwDeviceObserver
 */
void CRecordAudioFile::Stopped()
	{
	iStoppedReceived = ETrue;
	}

// end
