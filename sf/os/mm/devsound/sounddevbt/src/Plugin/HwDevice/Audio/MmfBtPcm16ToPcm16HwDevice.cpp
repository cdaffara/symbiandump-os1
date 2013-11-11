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

#include "MmfBtPcm16ToPcm16HwDevice.h"
#include "../../MmfBtFileDependencyUtil.h"

/**
 *
 *	Returns the created hw device for passing audio through audio.
 *  for the wins implementation this would always be pcm16 although
 *  this is effectively a null hw device that will pass any datatype through
 *	@return	"CMMFPcm16ToPcm16HwDevice"
 *
 */
CMMFPcm16ToPcm16HwDevice* CMMFPcm16ToPcm16HwDevice::NewL()
	{
    CMMFPcm16ToPcm16HwDevice* self = new (ELeave) CMMFPcm16ToPcm16HwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 *
 *	Second phase constructor.
 *
 */
void CMMFPcm16ToPcm16HwDevice::ConstructL()
	{
    iCodec = new (ELeave) CMMFPcm16ToPcm16Codec();
	static_cast<CMMFPcm16ToPcm16Codec*>(iCodec)->SetHwDevice(this);
	}

/**
*
* ~CMMFPcm16ToPcm16HwDevice
*
**/
CMMFPcm16ToPcm16HwDevice::~CMMFPcm16ToPcm16HwDevice()
	{
	}

/**
*
* Codec
* @return CMMFSwCodec&
**/
CMMFSwCodec& CMMFPcm16ToPcm16HwDevice::Codec()
	{
	return *iCodec;
	}





/**
*
* ProcessL
* @param aSrc Source Buffer
* @param aDest Destintion Buffer
* @return CMMFSwCodec::TCodecProcessResult
*
**/
CMMFSwCodec::TCodecProcessResult CMMFPcm16ToPcm16Codec::ProcessL(const CMMFBuffer& /*aSource*/, CMMFBuffer& /*aDest*/)
	{//no processing required for null codec
	User::Leave(KErrNotSupported); 
	//to keep compiler happy
	TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EEndOfData;
	result.iSrcBytesProcessed = 0;
	result.iDstBytesAdded = 0;
	return result;
	};


TUint CMMFPcm16ToPcm16Codec::SourceBufferSize()
	{
	if (!iBufferSize) 
		iBufferSize = iHwDevice->CalculateBufferSize();
	return iBufferSize;
	}


TUint CMMFPcm16ToPcm16Codec::SinkBufferSize()
	{
	if (!iBufferSize) 
		iBufferSize = iHwDevice->CalculateBufferSize();
	return iBufferSize;
	}

void CMMFPcm16ToPcm16Codec::SetHwDevice(CMMFPcm16ToPcm16HwDevice* aHwDevice)
	{
	iHwDevice = aHwDevice;
	}

TUint CMMFPcm16ToPcm16HwDevice::CalculateBufferSize()
	{
	TUint sampleRate = 0;
	TUint channels = 0;
	TInt useBufferOfSize = 0;
	TInt minBufferSize = 0;
	TInt maxBufferSize = 0;

	if (iPlayCustomInterface)
		{
		sampleRate = iSampleRate;
		channels = iChannels;	
/*		if ((sampleRate) && (channels))
			{
			RMdaDevSound::TSoundFormatsSupportedBuf playFormatsSupported;
			if (iDataPath->Device().Handle())
				{
				iDataPath->Device().PlayFormatsSupported(playFormatsSupported);
				minBufferSize = playFormatsSupported().iMinBufferSize;
				maxBufferSize = playFormatsSupported().iMaxBufferSize;
				}
			else
				{//try to get handle
				TInt err = iDataPath->Device().Open();
				if (err == KErrNone)
					{
					iDataPath->Device().PlayFormatsSupported(playFormatsSupported);
					minBufferSize = playFormatsSupported().iMinBufferSize;
					maxBufferSize = playFormatsSupported().iMaxBufferSize;
					iDataPath->Device().Close();
					}
				}
			}
*/		}
	if ((iRecordCustomInterface) && (!sampleRate) && (!channels))
		{ //must be record
		sampleRate = iSampleRate;
		channels = iChannels;
/*		if ((sampleRate) && (channels))
			{//get max and min supported buffer sizes supported by hw
			RMdaDevSound::TSoundFormatsSupportedBuf recordFormatsSupported;
			if (iDataPath->Device().Handle())
				{
				iDataPath->Device().RecordFormatsSupported(recordFormatsSupported);
				minBufferSize = recordFormatsSupported().iMinBufferSize;
				maxBufferSize = recordFormatsSupported().iMaxBufferSize;
				}
			else
				{//try to get handle
				TInt err = iDataPath->Device().Open();
				if (err == KErrNone)
					{
					iDataPath->Device().RecordFormatsSupported(recordFormatsSupported);
					minBufferSize = recordFormatsSupported().iMinBufferSize;
					maxBufferSize = recordFormatsSupported().iMaxBufferSize;
					iDataPath->Device().Close();
					}
				}
			}
*/		}
//	else convert so not applicable

	if ((sampleRate) && (channels))
		{
		// Buffer size = (SampleRate * BytesPerSample * Channels) / 4
		useBufferOfSize = ((sampleRate * 2 * channels)/KDevSoundFramesPerSecond + (KDevSoundDeltaFrameSize-1)) &~ (KDevSoundDeltaFrameSize-1);
		//clamp buffer to desired limits
		if(useBufferOfSize < KDevSoundMinFrameSize) 
			useBufferOfSize = KDevSoundMinFrameSize;
		else if(useBufferOfSize > KDevSoundMaxFrameSize) 
			useBufferOfSize = KDevSoundMaxFrameSize;

		//clamp buffer to limits of hardware
		if (maxBufferSize)
			{//buffer size limits have been set by sound driver
			 //check we are within the limits
			if(useBufferOfSize < minBufferSize)
				useBufferOfSize = minBufferSize;
			else if(useBufferOfSize > maxBufferSize)
				useBufferOfSize = maxBufferSize;
			}
		}
	else
		{
		useBufferOfSize = KPCM16ToPCM16BufferSize;
		}

	return useBufferOfSize;
	}
