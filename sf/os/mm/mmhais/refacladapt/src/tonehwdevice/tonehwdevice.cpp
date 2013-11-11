// tonehwdevice.cpp
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



// INCLUDE FILES
#include <mmf/server/mmfhwdevice.h>
#include "tonehwdevice.hrh" //for KUidToneHwDevice
#include <ecom/implementationproxy.h> // For making it ECom plugin
#include "tonehwdevice.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES   

// CONSTANTS
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidToneHwDevice,  CToneHwDevice::NewL),
	};
//current supported sample rate
const TInt KSupportedSampleRate = 8000;

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CToneHwDevice::CToneHwDevice()
	{
	DP_CONTEXT(CToneHwDevice::CToneHwDevice *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CToneHwDevice::ConstructL()
	{
	DP_CONTEXT(CToneHwDevice::ConstructL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();

	iHwDataBufferFill = CMMFDataBuffer::NewL(sizeof(TToneData));
	iHwDataBufferFill->SetLastBuffer(EFalse);
	iHwDataBufferFill->Data().SetLength(sizeof(TToneData));
	iHwDataBufferFill->SetRequestSizeL(sizeof(TToneData));

	iCodec = new(ELeave)CToneCodec();

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian constructor
// -----------------------------------------------------------------------------
//
CToneHwDevice* CToneHwDevice::NewL()
	{
	DP_STATIC_CONTEXT(CToneHwDevice::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CToneHwDevice* self = new (ELeave) CToneHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CToneHwDevice::~CToneHwDevice()
	{
	DP_CONTEXT(CToneHwDevice::~CToneHwDevice *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	delete iHwDataBufferFill;
	delete iCodec;
	delete iPlayCustomInterface;

	if(iDataPath)
		{
		delete iDataPath;
		}

	if(iToneBuffer1)
		{
		delete iToneBuffer1;
		}
	if(iToneBuffer2)
		{
		delete iToneBuffer2;
		}
	DP_OUT();
	}

/**
*
* Codec
*
*/
CToneCodec& CToneHwDevice::Codec()
	{
	return *iCodec;
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::Init
// ---------------------------------------------------------------------------
//
TInt CToneHwDevice::Init(THwDeviceInitParams& aDevInfo)
	{
	DP_CONTEXT(CToneHwDevice::Init *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if (!iToneInitialized) // Check if tones is not initialized yet.
		{
		iToneInitialized = ETrue;
		}

	// [ precondition that aDevInfo has a valid observer ]
	if (!aDevInfo.iHwDeviceObserver) 
		{
		DP0_RET(KErrArgument, "%d");
		}

	iHwDeviceObserver = aDevInfo.iHwDeviceObserver;

	//[ assert the post condition ]
	if (!iCodec)
		{
		DP0_RET(KErrNotSupported, "%d");
		}

	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::Start
// ---------------------------------------------------------------------------
//
TInt CToneHwDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/)
	{
	DP_CONTEXT(CToneHwDevice::Start *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt error = KErrNone;
		
	// Start for first time OR resuming
	if (!iTonePlaying || iDataPath->State() == CToneDataPath::EPaused)
		{
		iLastBuffer = EFalse;
		//[ assert precondition that play custom interface is present]
		//if there is no tone play custom interface then the user of the CToneHwDevice
		//cannot have set any of the custom settings such as sample rate.
		if (!iPlayCustomInterface)
			{
			DP0_RET(KErrNotReady, "%d");
			}
	
		//play
		if (!iDataPath)
			{
			//create a datapath
			TRAP(error,iDataPath = CToneDataPath::NewL());
			if ((iDataPath)&&(error == KErrNone))
				{
				ASSERT(iHwDeviceObserver);
				iDataPath->SetObserver(*this);
				error = iDataPath->AddCodec(*iCodec);
				if (error == KErrNone)
					{
					iDeviceBufferSize = (iCodec->SinkBufferSize());
					}
				}
			}
		if ((error == KErrNone) && (iDataPath->State() != CToneDataPath::EPlaying))
			{
			//datapath was created ok and we are not playing
			if (iDataPath->State() == CToneDataPath::EStopped)
				{
				// starting from 'fresh so set sound device settings
				if (!iDataPath->Device().Handle())
					{
					//if Device() is called then we need a valid sound device handle
					error = iDataPath->Device().Open();
					if (error != KErrNone)
						{
						DP0_RET(error, "%d");
						}
						
					}
				static_cast<TToneCustomInterface*>(iPlayCustomInterface)->SetDevice(&(iDataPath->Device()));
				
				TUint iVol = iPlayCustomInterface->Volume();
				iDataPath->Device().SetPlayVolume(iVol);
							
				soundDeviceSettings().iRate = iSampleRate;
			
				//this would normally be pcm16
				soundDeviceSettings().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
			
				//1 = mono 2 = stereo
				soundDeviceSettings().iChannels = iChannels;
			
				//tell sound driver what buffer size to expect
				//it is up the the implementor to make use the device can support
				//the required buffer size
				soundDeviceSettings().iBufferSize = iDeviceBufferSize;
				error = iDataPath->Device().SetPlayFormat(soundDeviceSettings);
			
				} // End of iDataPath->State() == CToneDataPath::EStopped
			
			//else resuming from pause	
			if ((error == KErrNone)||(error == KErrInUse))
				{
				// Hw device hasn't played anything yet so don't change
				// active buffer. This is checked in FillThisHwBuffer.
				if(iDataPath->State() != CToneDataPath::EPaused)
					{
					iFirstCallFromHwDevice = ETrue;		
					}
				iTonePlaying = ETrue;
				error = iDataPath->Start();
				}
			}//status == KErrNone       
		}
	else // if tone playback is already ongoing do nothing
		{
		DP0(DLINFO,"Previous tone call is not completed yet");
		}
	DP0_RET(error, "%d");
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::Stop
// ---------------------------------------------------------------------------
TInt CToneHwDevice::Stop()
	{
	DP_CONTEXT(CToneHwDevice::Stop *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iTonePlaying) 
		{
		iTonePlaying = EFalse;
		}
	
	if (!iDataPath)
		{
		DP0_RET(KErrNotReady, "%d");
		}
		
	delete iDataPath;
	iDataPath = NULL;
	
	//Setting device to NULL since after stop it doesn't exists any more
	if(iPlayCustomInterface)
		{
		static_cast<TToneCustomInterface*>(iPlayCustomInterface)->SetDevice(NULL);
		}
	
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::Pause
// ---------------------------------------------------------------------------
//
TInt CToneHwDevice::Pause()
	{
	DP_CONTEXT(CToneHwDevice::Pause *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if (!iDataPath) 
		{
		DP0_RET(KErrNotReady, "%d");
		}
	iDataPath->Pause();
	
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::StopAndDeleteCodec
// ---------------------------------------------------------------------------
//
TInt CToneHwDevice::StopAndDeleteCodec()
	{
	DP_CONTEXT(CToneHwDevice::StopAndDeleteCodec *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNotSupported, "%d");
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::DeleteCodec
// ---------------------------------------------------------------------------
//
TInt CToneHwDevice::DeleteCodec()
	{
	DP_CONTEXT(CToneHwDevice::DeleteCodec *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNotSupported, "%d");
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::SetConfig
// ---------------------------------------------------------------------------
//
TInt CToneHwDevice::SetConfig(TTaskConfig& aConfig)
	{
	DP_CONTEXT(CToneHwDevice::SetConfig *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if (aConfig.iUid != KUidRefDevSoundTaskConfig)
		{
		DP0_RET(KErrArgument, "%d");
		}

	if (aConfig.iRate != KSupportedSampleRate )
		{
		DP0_RET(KErrNotSupported, "%d");
		}

	iSampleRate = aConfig.iRate;

	if (aConfig.iStereoMode == ETaskMono)
		{
		iChannels = 1;
		}
	else if (aConfig.iStereoMode == ETaskInterleaved || aConfig.iStereoMode == ETaskNonInterleaved)
		{
		iChannels = 2;
		}
	else
		{
		DP0_RET(KErrArgument, "%d");
		}
		
	DP0_RET(KErrNone, "%d");
	}

// CToneHwDevice::FillThisHwBuffer
// ---------------------------------------------------------------------------
TInt CToneHwDevice::FillThisHwBuffer(CMMFBuffer& aHwBuffer)
	{
	DP_CONTEXT(CToneHwDevice::FillThisHwBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err(KErrNone);
	
	if(iFirstCallFromHwDevice)
		{
		err = iHwDeviceObserver->FillThisHwBuffer(aHwBuffer);
		}
	else
		{
		err = ThisHwBufferFilled(aHwBuffer);
		}
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::ThisHwBufferFilled
// ---------------------------------------------------------------------------
//
TInt CToneHwDevice::ThisHwBufferFilled(CMMFBuffer& aMmfBuffer)
	{
	DP_CONTEXT(CToneHwDevice::ThisHwBufferFilled *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = KErrNone;
	CMMFDataBuffer* myBuffer = static_cast<CMMFDataBuffer*> (&aMmfBuffer);
	// Set the request length, From HwDevice this comes with buffer
	// length.
	TInt len = myBuffer->Data().MaxLength();
	// Ignore error. since buffer size = Buffer Length 
	TRAP(err, myBuffer->SetRequestSizeL(len));

	if(iFirstCallFromHwDevice)
		{
		myBuffer->SetLastBuffer(EFalse);
		
		Mem::Copy((TAny*)(&myToneData), (TAny*)(myBuffer->Data().Ptr()), sizeof(TToneData));
		
		err = ReadToneData();
		if(err==KErrNone)
			{
			err= GenerateBufferData();
			}
		}
	else
		{
		// Hw device will call this method right after its Start was called.
		// When it calls this for the first time it hasn't played one single
		// buffer yet so check that.
		// In this case there's no need to set the active buffer as it's already
		// waiting to be played.
		SetActiveToneBuffer();
		}

	if (err == KErrNone)
		{
		// If there is no data in the active buffer, tone play is finished.
		// DevSound just have to wait for completion event from audio device.
		if (iActiveToneBuffer->Data().Length() == 0)
			{
			iActiveToneBuffer->SetLastBuffer(ETrue);
			myBuffer->SetLastBuffer(ETrue);
			iLastBuffer=ETrue;
			}

		TInt tonelen = iActiveToneBuffer->Data().Length();

		// don't enter more data than can be handled by the receiving buffer
		if (len >= tonelen)
			{
			len = tonelen;
			}

		// Copy data from tone buffer to hw device buffer
		Mem::Copy((TAny*)(myBuffer->Data().Ptr()), (TAny*)(iActiveToneBuffer->Data().Ptr()), len);
			
		myBuffer->Data().SetLength(len);

		//Play data and try to generate next data block
		TRAP(err,iDataPath->BufferFilledL(static_cast<CMMFDataBuffer&> (*myBuffer)));
		if(err == KErrNone)
			{
			if(iLastBuffer)
				{
				// coverity[check_after_deref]
				if(myBuffer)
					{
					myBuffer = NULL;
					}
				FreeBuffers();
				iFirstCallFromHwDevice = EFalse;
				}
			else
				{
				// Check again whether this is the first call from Hw device.
				// FillFreeToneBuffer assumes the iActiveToneBuffer has already
				// been played.
				if (!iFirstCallFromHwDevice)
					{
					err = FillFreeToneBuffer();
					}
				else
					{
					iFirstCallFromHwDevice = EFalse;  // Reset flag 
					}
				}
			}
		}
	if ( err != KErrNone )
		{
		myBuffer->SetLastBuffer(ETrue);
		myBuffer->Data().SetLength(0);
		//Use error additional variable for sending last buffer so can still send Error(err)
		TRAPD(datapathErr, iDataPath->BufferFilledL(static_cast<CMMFDataBuffer&> (*myBuffer)));
		// coverity[check_after_deref]
		if(myBuffer)
			{
			myBuffer = NULL;
			}
		FreeBuffers();
		iFirstCallFromHwDevice = EFalse;
		if ( datapathErr != KErrNone )
			{
			iHwDeviceObserver->Error(datapathErr);
			DP0_RET(datapathErr, "%d");
			}
		iHwDeviceObserver->Error(err);
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CToneHwDevice::ThisHwBufferEmptied
// ---------------------------------------------------------------------------
TInt CToneHwDevice::ThisHwBufferEmptied(CMMFBuffer& /*aMmfBuffer*/)
	{
	DP_CONTEXT(CToneHwDevice::ThisHwBufferEmptied *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNotSupported, "%d");
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CToneHwDevice::EmptyThisHwBuffer
// ---------------------------------------------------------------------------
TInt CToneHwDevice::EmptyThisHwBuffer(CMMFBuffer& /*aMmfBuffer*/)
	{
	DP_CONTEXT(CToneHwDevice::EmptyThisHwBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNotSupported, "%d");
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CToneHwDevice::MsgFromHwDevice
// ---------------------------------------------------------------------------
TInt CToneHwDevice::MsgFromHwDevice(TUid aMessageType, const TDesC8& aMsg)
	{
	DP_CONTEXT(CToneHwDevice::MsgFromHwDevice *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	err = iHwDeviceObserver->MsgFromHwDevice(aMessageType, aMsg);
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CToneHwDevice::Stopped
// ---------------------------------------------------------------------------
void CToneHwDevice::Stopped()
	{
	DP_CONTEXT(CToneHwDevice::Stopped *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iHwDeviceObserver->Stopped();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CToneHwDevice::Error
// ---------------------------------------------------------------------------
void CToneHwDevice::Error(TInt aError)
	{
	DP_CONTEXT(CToneHwDevice::Error *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iHwDeviceObserver->Error(aError);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class CMMFHwDevice
// CToneHwDevice::CustomInterface
// ---------------------------------------------------------------------------
//
TAny* CToneHwDevice::CustomInterface(TUid aInterfaceUid)
	{
	DP_CONTEXT(CToneHwDevice::CustomInterface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TAny* ret = NULL;
	TInt err = KErrNone;

	if (aInterfaceUid.iUid == KMmfPlaySettingsCustomInterface)
		{
		if (!iPlayCustomInterface)
			{
			TRAP(err,iPlayCustomInterface = new(ELeave)TToneCustomInterface());
			}
		if (err)
			{
			ret = NULL;
			}
		else
			{
			ret = static_cast<TAny*>(iPlayCustomInterface);
			}
		}
	else if (aInterfaceUid == KIgnoreUnderflowCustomInterfaceTypeUid)
		{
		if (!iDataPath)
			{
			ret = NULL;			
			}
		else
			{
			ret = static_cast<CToneDataPath*>(iDataPath)->CustomInterface(aInterfaceUid);	
			}	
		}

	DP_OUT();
	return ret;
	}

TInt CToneHwDevice::ReadToneData()
	{
	DP_CONTEXT(CToneHwDevice::ReadToneData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TUint vol;
	myToneData.GetType(iToneType);
	TInt64 zeroInt64(0);
	iFrequency1 = myToneData.GetFrequencyOne();
	iFrequency2 = myToneData.GetFrequencyTwo();
	myToneData.GetDuration(iDuration);

	myToneData.GetRepeatTrailingSilence(iRepeatTrailingSilence);
	iRepeatCount = myToneData.GetRepeatCount();
	iRampDuration = iPlayCustomInterface->VolumeRamp();
	vol = iPlayCustomInterface->Volume();
	iDataPath->Device().SetPlayVolume(vol);
	switch (iToneType)
		{
		case TToneData::ESimple:
			DP0(DLINFO, "Playing simple tone");
			iDataPath->Device().GetPlayFormat(soundDeviceSettings);
			if((iFrequency1<0) || (iDuration.Int64() < zeroInt64))
				{
				iHwDeviceObserver->Error(KErrArgument);
				DP0_RET(KErrArgument, "%d");
				}
			iToneGen.SetFrequencyAndDuration(iFrequency1,iDuration);
			// Configure tone generator
			iToneGen.Configure(
				soundDeviceSettings().iRate,
				soundDeviceSettings().iChannels,
				iRepeatCount,
				I64LOW((iRepeatTrailingSilence.Int64()*soundDeviceSettings().iRate)/1000000),
				I64LOW((iRampDuration.Int64()*soundDeviceSettings().iRate)/1000000)
				);
			iCurrentGenerator = &iToneGen;
			break;
		case TToneData::EDual:
			DP0(DLINFO, "Playing dual tone");
			iDataPath->Device().GetPlayFormat(soundDeviceSettings);
			if((iFrequency1<0) || (iFrequency2<0) || (iDuration.Int64() < zeroInt64))
				{
				iHwDeviceObserver->Error(KErrArgument);
				DP0_RET(KErrArgument, "%d");
				}
			iDualToneGen.SetFrequencyAndDuration(iFrequency1, iFrequency2, iDuration);
			// Configure dual tone generator
			iDualToneGen.Configure(
				soundDeviceSettings().iRate,
				soundDeviceSettings().iChannels,
				iRepeatCount,
				I64LOW((iRepeatTrailingSilence.Int64()*soundDeviceSettings().iRate)/KOneMillionMicroSeconds),
				I64LOW((iRampDuration.Int64()*soundDeviceSettings().iRate)/KOneMillionMicroSeconds)
				);
			iCurrentGenerator = &iDualToneGen;
			break;
		case TToneData::EDtmfString:
			DP0(DLINFO, "Playing DTMF string");
			myToneData.GetDtmfLenghts(myToneOnLength, myToneOffLength, myPauseLength);
			iDTMFGen.SetToneDurations(myToneOnLength, myToneOffLength, myPauseLength);
			iDTMFString = myToneData.GetDTMFString();
			if(!ValidDTMFString(const_cast<TDesC&>(*iDTMFString)))
				{
				DP0(DLINFO, "Invalid DTMF String");
				iHwDeviceObserver->Error(KErrCorrupt);
				DP0_RET(KErrCorrupt, "%d");
				}
			iDTMFGen.SetString(const_cast<TDesC&>(*iDTMFString));
			iDataPath->Device().GetPlayFormat(soundDeviceSettings);
			iDTMFGen.Configure(
				soundDeviceSettings().iRate,
				soundDeviceSettings().iChannels,
				iRepeatCount,
				I64LOW((iRepeatTrailingSilence.Int64()*soundDeviceSettings().iRate)/1000000),
				I64LOW((iRampDuration.Int64()*soundDeviceSettings().iRate)/1000000)
				);
			iCurrentGenerator = &iDTMFGen;
			break;
		case TToneData::ESequence:
			DP0(DLINFO, "Playing tone sequence");
			iSequenceData = myToneData.GetSequenceData();
			// Check whether the sequence is signed or not
			if (!RecognizeSequence(*iSequenceData))
				{
				DP0(DLINFO, "Invalid Sequence Sign");
				iHwDeviceObserver->Error(KErrCorrupt);
				DP0_RET(KErrCorrupt, "%d");
				}
			iSequenceGen.SetSequenceData(*iSequenceData);
			iDataPath->Device().GetPlayFormat(soundDeviceSettings);
			iSequenceGen.Configure(
				soundDeviceSettings().iRate,
				soundDeviceSettings().iChannels,
				iRepeatCount,
				I64LOW((iRepeatTrailingSilence.Int64()*soundDeviceSettings().iRate)/1000000),
				I64LOW((iRampDuration.Int64()*soundDeviceSettings().iRate)/1000000)
				);
			iCurrentGenerator = &iSequenceGen;
			break;
		case TToneData::EFixedSequence:
			DP0(DLINFO, "Playing FixedSequnce");
			iHwDeviceObserver->Error(KErrNotSupported);
			DP0_RET(KErrNotSupported, "%d");
		default:
			DP0_RET(KErrNotSupported, "%d");
		}
	DP0_RET(KErrNone, "%d");
	}

/*
 *
 *	Creates buffer and begin playback using the specified tone generator.
 *
 */
TInt CToneHwDevice::GenerateBufferData()
	{
	DP_CONTEXT(CToneHwDevice::GenerateBufferData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err;
	err = KErrNone;
	
	// Delete any buffer from previous call and try to create maximum buffer 
	// size. Double Buffer the Tone data.
	if (iToneBuffer1)
		{
		delete iToneBuffer1;
		iToneBuffer1 = NULL;
		}
	//note the tone buffer needs to be the same as the pcm16->pcm16 'null'
	//hw device plugin
	// Buffer size = (SampleRate * BytesPerSample * Channels) / 4
	TInt useBufferOfSize = ((SamplingFrequency() * 2 * NumberOfChannels())/KDevSoundFramesPerSecond + (KDevSoundDeltaFrameSize-1)) &~ (KDevSoundDeltaFrameSize-1);
	//clamp buffer to desired limits
	if(useBufferOfSize < KDevSoundMinFrameSize) 
		{
		useBufferOfSize = KDevSoundMinFrameSize;
		}
	else if(useBufferOfSize > KDevSoundMaxFrameSize) 
		{
		useBufferOfSize = KDevSoundMaxFrameSize;
		}

	TRAP(err, iToneBuffer1 = CMMFDataBuffer::NewL(useBufferOfSize));
	if ( err != KErrNone )
		{
		DP0_RET(err, "%d");
		}
	
	err = iCurrentGenerator->FillBuffer(iToneBuffer1->Data());
	if(err!=KErrNone)
		{
		DP0_RET(err, "%d");
		}

	if (iToneBuffer2)
		{
		delete iToneBuffer2; 
		iToneBuffer2 = NULL;
		}
		
	TRAP(err, iToneBuffer2 = CMMFDataBuffer::NewL(useBufferOfSize));
	if ( err != KErrNone )
		{
		DP0_RET(err, "%d");
		}
	
	err = iCurrentGenerator->FillBuffer(iToneBuffer2->Data());
	if(err!=KErrNone)
		{
		DP0_RET(err, "%d");
		}

	// Assign active buffer
	iActiveToneBuffer = iToneBuffer1;
	DP0_RET(KErrNone, "%d");
	}

/*
 *
 *	This method assigns the other buffer as active buffer. The tone audio 
 *	generator should fill data in the other buffer by now.
 *
 */
void CToneHwDevice::SetActiveToneBuffer()
	{
	if (iActiveToneBuffer == iToneBuffer1)
		iActiveToneBuffer = iToneBuffer2;
	else if (iActiveToneBuffer == iToneBuffer2)
		iActiveToneBuffer = iToneBuffer1;
	}

	
void CToneHwDevice::FreeBuffers()
	{
	if(iToneBuffer1)
		{
		delete iToneBuffer1;
		iToneBuffer1 = NULL;
		}
	if(iToneBuffer2)
		{
		delete iToneBuffer2;
		iToneBuffer2 = NULL;
		}
	}
	
	/*
 *
 *	Returns an integer representing Sampling Frequency the device is currently
 *	configured to.
 *
 *	@return	"TInt"
 *			Sampling Frequency.
 *
 */
TInt CToneHwDevice::SamplingFrequency()
	{
	return iSampleRate;
	}
	
/*
 *
 *	Returns an integer representing number of channels the device is currently
 *	configured to.
 *
 *	@return	"TInt"
 *			Number of audio channels 1 if mono, 2 if stereo.
 *
 */
TInt CToneHwDevice::NumberOfChannels()
	{
	if(iChannels == EMMFMono)
		return 1;
	else
		return 2;
	}
	
	/*
 *
 *	This method fills data into the free buffer.
 *
 *	@return	"TInt"
 *			Error code. KErrNone if success.
 *
 */
TInt CToneHwDevice::FillFreeToneBuffer()
	{
	TInt err(KErrNone);
	if (iActiveToneBuffer == iToneBuffer1)
		{
		err = iCurrentGenerator->FillBuffer(iToneBuffer2->Data());
		}
	else if (iActiveToneBuffer == iToneBuffer2)
		{
		err = iCurrentGenerator->FillBuffer(iToneBuffer1->Data());
		}
	return err;
	}

TBool CToneHwDevice::RecognizeSequence(const TDesC8& aData)
	{
	// Reference plug-in only supports its own sequence format
	_LIT8(KSequenceSignature,"SQNC");
	if (aData.Length() > 4)
		{
		if (aData.Left(4) == KSequenceSignature)
			{
			return ETrue;
			}
		}
	// Didn't recognise
	return EFalse;
	}

TBool CToneHwDevice::ValidDTMFString(const TDesC& aDTMFString)
	{
	const TDesC* stringDTMF = &aDTMFString;;
	TInt stringPos = 0;
	if (stringPos == stringDTMF->Length())
		{
		return EFalse; // Finished. Nothing to do
		}
	do
		{
		TChar c((*stringDTMF)[stringPos++]);
		if (static_cast<TUint> (c)=='#' || static_cast<TUint> (c)=='*' || static_cast<TUint> (c)==',' || c.IsHexDigit() || c.IsSpace())
			{
			//Do nothing, valid character
			}
		else
			{
			return EFalse;
			}

		}
	while(stringPos < stringDTMF->Length());
	return ETrue;
	}


/************************************************************************
 *				TToneCustomInterface     								*
 ************************************************************************/
/**
 * This method is not be exported as it is only 
 * intended to be called within this DLL.
 * It's purpose is to assign an RMdaDevSound to the play
 * custom interface
 * @internalComponent
 */
void TToneCustomInterface::SetDevice(RMdaDevSound* aDevice)
	{
	iDevice = aDevice;
	}

void TToneCustomInterface::SetVolume(TUint aVolume)
	{
	iVolume = aVolume;
	if (iDevice && iDevice->Handle()!=0)
		{
		iDevice->SetPlayVolume(aVolume);
		}
	}
	
/**
 * Procedure to get the number of bytes played by the device driver
 * If there is no handle available to the device driver then the 
 * procedure returns the last known value
 * @released
 * @return number of bytes played
 */
TUint TToneCustomInterface::BytesPlayed()
	{
	if(iDevice)
		{
		if (iDevice->Handle())
			{
			iBytesPlayed = iDevice->BytesPlayed();
			}
		}
	return iBytesPlayed;
	}


//
//       class CToneCodec            //
//

// ---------------------------------------------------------------------------
// from class CToneCodec
// CToneCodec::CToneCodec
// ---------------------------------------------------------------------------
//
CToneCodec::CToneCodec()
	{
	}
	
// ---------------------------------------------------------------------------
// from class CToneCodec
// CToneCodec::~CToneCodec
// ---------------------------------------------------------------------------
//	
CToneCodec::~CToneCodec()
	{
	}

// ---------------------------------------------------------------------------
// from class CToneCodec
// CToneCodec::ConstructL
// ---------------------------------------------------------------------------
//	
void CToneCodec::ConstructL()
	{
	}
	

// ---------------------------------------------------------------------------
// from class CToneCodec
// CToneCodec::ProcessL
// ---------------------------------------------------------------------------
//
CToneCodec::TCodecProcessResult CToneCodec::ProcessL(const CMMFBuffer& /*aSource*/, CMMFBuffer& /*aDest*/)
	{
	//no processing required for null codec
	User::Leave(KErrNotSupported); 
	//to keep compiler happy
	TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EEndOfData;
	result.iSrcBytesProcessed = 0;
	result.iDstBytesAdded = 0;
	return result;
	}

// ---------------------------------------------------------------------------
// from class CToneCodec
// CToneCodec::SourceBufferSize
// ---------------------------------------------------------------------------
//
TUint CToneCodec::SourceBufferSize()
	{
	return KPCM16ToPCM16BufferSize;
	}

// ---------------------------------------------------------------------------
// from class CToneCodec
// CToneCodec::SinkBufferSize
// ---------------------------------------------------------------------------
//
TUint CToneCodec::SinkBufferSize()
	{
	return KPCM16ToPCM16BufferSize;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================
/**
* ImplementationGroupProxy
* is called to get a pointer to the plugin's implementation table, or table
* of functions used to instantiate the plugin.
* @since
* @param aTableCount returns the number of functions in the table.
* @return retuns a pointer to the table.
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
