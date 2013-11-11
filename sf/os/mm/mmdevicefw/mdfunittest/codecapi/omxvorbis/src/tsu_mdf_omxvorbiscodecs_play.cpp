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

#include "tsu_mdf_omxvorbiscodecs_play.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>

#include <e32debug.h>

// for ogg_packet
#include "ogg.h"
	
//
// RTestStepOmxVorbisCodecPlay
// play a Vorbis file

/**
 * Constructor
 */
RTestStepOmxVorbisCodecPlay::RTestStepOmxVorbisCodecPlay()
 	{
 	iTestStepName = _L("MM-MDF-OMXVORBISCODECS-U-0001-HP");
	iHeapSize = KTestHeapSize;
 	}

/**
 * So that CPlayAudioFile can set the test verdict and output messages
 */
void RTestStepOmxVorbisCodecPlay::SetVerdict(TPtrC16 aText, TVerdict aVerdict)
	{
	iVerdict = aVerdict;
	INFO_PRINTF2(_L("---> %s"), aText.Ptr());
	}

/**
 * Do the test step
 */
TVerdict RTestStepOmxVorbisCodecPlay::DoTestStepL()
	{
	iPlayAudioFile = CPlayAudioFile::NewL(this);
	iPlayAudioFile->StartL();
	delete iPlayAudioFile;
	return iVerdict;
	}	

//
// CPlayAudioFile
// Active player for RTestStepOmxVorbisCodecPlay and derived test classes.

/**
 * Constructor
 */
CPlayAudioFile::CPlayAudioFile(RTestStepOmxVorbisCodecs* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceCreateAndInit),
	iSourceFile(NULL),
	iSourceFilePos(0),
	iSourceFileReadPos(0),
	iFinished(EFalse),
	iParent(aParent)
	{
	}

/**
 * ConstructL
 */
void CPlayAudioFile::ConstructL()
	{
	// create an Ogg pager
    iPager = COggPager::NewL();
    iStream = COggStream::NewL();

	CActiveScheduler::Add(this);
	}

/**
 * NewL
 */
CPlayAudioFile* CPlayAudioFile::NewL(RTestStepOmxVorbisCodecs* aParent)
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
	// delete Ogg pager
	delete iStream;
    delete iPager;
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
	fs.Connect();
	
	RFile file;
	TInt err = file.Open(fs, KTestDecoderPlayFile, EFileRead);
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
	// RDebug::Print(_L("CPlayAudioFile::FillThisHwBuffer"));

	ReadNextBuffer(aHwDataBuffer);
	return KErrNone;
	}

/**
 * @see MMMFHwDeviceObserver
 */
TInt CPlayAudioFile::EmptyThisHwBuffer(CMMFBuffer& aEmptyBufferPtr)
	{
	// RDebug::Print(_L("CPlayAudioFile::EmptyThisHwBuffer"));

	// write to file
	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(&aEmptyBufferPtr);
	TInt err = KErrNone;
	err = WriteDataToFile(dataBuffer->Data());
	if(err != KErrNone)
		{
		return err;
		}
	
	err = iHwDevice->ThisHwBufferEmptied(aEmptyBufferPtr);
	if(err != KErrNone)
		{
		return err;
		}
	
	return err;	
	}

/**
 * writes decoded data to file
 */
TInt CPlayAudioFile::WriteDataToFile(TDes8& aData)
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
	
/**
 * opens record file
 */
TInt CPlayAudioFile::SetUpRecordFile()
	{
	iFileServerSession.Connect();
	TInt err = iRecordFile.Replace(iFileServerSession, KTestDecoderRecordedFile, EFileWrite|EFileShareExclusive);	
	return err;
	}

/**
 * closes record file
 */
void CPlayAudioFile::CloseRecordFile()
	{
	iRecordFile.Close();
	iFileServerSession.Close();
	}
	
/**
 * adds a WAV header to recorded data
 */
void CPlayAudioFile::AddWavHeader()
	{
	// add a WAV header to the decoded file, so we can play it
	
	HBufC8* rawDecFile = NULL;
	RFs fs;
	fs.Connect();
	
	RFile file;
	TInt err = file.Open(fs, KTestDecoderRecordedFile, EFileRead);
	if (err != KErrNone)
		{
		iParent->SetVerdict(_L("Couldn't open recorded file for reading"), EFail);
		return;
		}
		
	TInt size; 
	err = file.Size(size);
	if (err != KErrNone)
		{
		iParent->SetVerdict(_L("Recorded file was empty"), EFail);
		return;
		}

	rawDecFile = HBufC8::NewMax(size);
	if(!rawDecFile)
		{
		iParent->SetVerdict(_L("Couldn't allocate memory to read back recorded file"), EFail);
		return;
		}

	TPtr8 ptr = rawDecFile->Des();
	file.Read(ptr,size);
	file.Close();
			
	// add headers
	err = file.Replace(fs, KTestDecoderRecordedFile, EFileWrite);
	if (err != KErrNone)
		{
		iParent->SetVerdict(_L("Couldn't open recorded file for writing wave headers"), EFail);
		return;
		}

	// this is canonical WAV file format header
	TInt32 chunkSize = size + KTestWavFormatPCMChunkHeaderSize;
	TInt32 subchunk1size = KTestWavFormatPCMSubchunk1Size;
	TInt16 audioFormat = KTestAudioFormatPCM;
	TInt16 numChannels = KTestNumChannels;
	TInt32 sampleRate = KTestSampleRate;
	TInt16 bitsPerSample = KTestBitsPerSample;
	TInt32 byteRate = sampleRate * numChannels * (bitsPerSample / 8);
	TInt16 blockAlign = numChannels * (bitsPerSample / 8);
	TInt32 subchunk2size = size;
	
	file.Write(_L8("RIFF"));
	{ TPtrC8 buf((TText8*)&chunkSize,sizeof(TInt32)); file.Write(buf); }
	file.Write(_L8("WAVEfmt "));
	{ TPtrC8 buf((TText8*)&subchunk1size,sizeof(TInt32)); file.Write(buf); }
	{ TPtrC8 buf((TText8*)&audioFormat,sizeof(TInt16)); file.Write(buf); }
	{ TPtrC8 buf((TText8*)&numChannels,sizeof(TInt16)); file.Write(buf); }
	{ TPtrC8 buf((TText8*)&sampleRate,sizeof(TInt32)); file.Write(buf); }
	{ TPtrC8 buf((TText8*)&byteRate,sizeof(TInt32)); file.Write(buf); }
	{ TPtrC8 buf((TText8*)&blockAlign,sizeof(TInt16)); file.Write(buf); }
	{ TPtrC8 buf((TText8*)&bitsPerSample,sizeof(TInt16)); file.Write(buf); }
	file.Write(_L8("data"));
	{ TPtrC8 buf((TText8*)&subchunk2size,sizeof(TInt32)); file.Write(buf); }
	file.Write(ptr,size);

	// store file size
	file.Size(iWrittenDataTotal);
	
	file.Close();
	fs.Close();
	}

/**
 * send chunks of data into the encoder.
 */
void CPlayAudioFile::ReadNextBuffer(CMMFBuffer& aHwDataBuffer)
    {
	// Logic of ReadNextBuffer is similar to logic of COggPlayController's
	// SendPacketToSinkL
	
	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(&aHwDataBuffer);
    while (ETrue)
        {
		// check if finished
		if (iFinished) 
			{
			iHwDevice->ThisHwBufferFilled(aHwDataBuffer);
			SetState(EHwDeviceAllowToComplete);
			return;
			}
				
		TInt srcLength = iSourceFile->Size();
        TInt err = iStream->PacketOut(iPacket);
        if (err == KErrNone)
            {
	        // put the packet into the data buffer.
	        // ensure it's fully copied.
	        ogg_packet pckt;
            pckt.packet     = reinterpret_cast<ogg_reference*>(const_cast<unsigned char*>(iPacket.iData.Ptr()));
            pckt.bytes      = iPacket.iData.Length();
                        
            TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
             
			long begin =pckt.packet->begin; //position in buffer for start of packet
            long length = pckt.packet->length; //length of packet
            
            ogg_buffer *buffer= pckt.packet->buffer; //pointer to buffer
            ogg_reference *next= pckt.packet->next;	//pointer to next ogg reference if needed
            
            //buffer details 
            unsigned char *data = buffer->data;
            //Keep record of how far through data buffer 
            TInt ptr_count=0;
            
            //read data from first buffer 
            for(int i=0;i<length; i++)
            	{
            	data_pointer[i]=data[begin+i];
            	}
            
            ptr_count+=length;
			
			//data is not always in one buffer may be spread over several so repeat while is still next buffer 
			while(next)
				{
				//was next ogg_reference assign this to packet and reset variables
				pckt.packet =next;
				
				//New variable values as assigned to next refernce 
				begin =pckt.packet->begin; //position in buffer for start of packet
           		length = pckt.packet->length; //length of packet
           		
           		buffer= pckt.packet->buffer; //pointer to buffer
            	next= pckt.packet->next;	//pointer to next ogg reference if needed
            
                data = buffer->data;
                
				//read data 
                for(int i=0;i<length; i++)
            		{
            		data_pointer[i+ptr_count]=data[begin+i];
            		}
            		
            	ptr_count+=length;
				
				}
			
			// set buffer size into the CMMFBuffer
			dataBuffer->Data().SetLength(ptr_count);
			
			// set SourceFilePos to this length, so we read from the
			// correct position next time
			iSourceFilePos += ptr_count;
			iSourceFileReadPos = iSourceFilePos;
			
			// callback
			iHwDevice->ThisHwBufferFilled(aHwDataBuffer);
            return;
            }
        else
            {
            err = iPager->NextPage(iPage);
            if (err==KErrNone)
                {
                iStream->PageIn(iPage);
                }
            else
                {
                
                // NB file position (from last read) is what we should be
                // checking, not fileposition decoded
                
				// request for more data
				if (iSourceFileReadPos < srcLength)
					{
					TInt size = srcLength - iSourceFilePos;
	                TPtr8 buf(NULL, 0);
					if (size > dataBuffer->Data().MaxLength())
						{
						size = dataBuffer->Data().MaxLength();			
						}
                	err = iPager->GetBuffer(buf, size);
                	if(!err) 
	                	{
						Mem::Copy((TAny*)buf.Ptr(), (TAny*)iSourceFile->Mid(iSourceFilePos).Ptr(), size);
						buf.SetLength(size);
						err = iPager->DataWritten(buf);
						iSourceFileReadPos += size;	
					
						RDebug::Print(_L("CPlayAudioFile::data read = %d bytes"), iSourceFileReadPos);
	                	}
					}
				else 
					{
					// no more data
					RDebug::Print(_L("CPlayAudioFile::end of data"));
					iFinished = ETrue;
					dataBuffer->SetLastBuffer(ETrue);
					iHwDevice->ThisHwBufferFilled(aHwDataBuffer);
					SetState(EHwDeviceAllowToComplete);
            		return;
					}
                }
            }
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
				//Play is Vorbis -> pcm16
				TRAP(err,setup->SetDataTypesL(KMMFFourCCCodeTestVorbis, KMMFFourCCCodePCM16));
				if(err==KErrNotFound)
					{
					iParent->SetVerdict(_L("Cannot Load pu"),EFail);
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
			CloseRecordFile();
			
			// add wave header
			AddWavHeader();
			
			// check the recorded file against expected file size for the test to pass.
			if(iWrittenDataTotal != KTestDecoderRecordedFileExpectedSize) 
				{
				iParent->SetVerdict(_L("Unexpected amount of data written to file"), EFail);
				}
			
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
	// Completion is signified by :-
	// (a) KErrUnderflow
	// (b) KErrNone, iStoppedReceived and iFinished)
		
	if (aError == KErrUnderflow && iStoppedReceived)
		{
		iParent->SetVerdict(_L("Success (KErrUnderflow)"));
		}
	else if (aError == KErrNone && iStoppedReceived && iFinished)
		{
		iParent->SetVerdict(_L("Success (iFinished)"));
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
	iStoppedReceived = ETrue;
	}

// end
