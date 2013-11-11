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

#include <ecom/ecom.h>
#include <mmf/plugin/mmfformatimplementationuids.hrh>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfaudioinput.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>

#include "mmfwavformat.h"

#include "formatUtils.h"
#include "MMFFormatBaseClasses.h"

const TInt	KGsmWavEncodedFrameSize = 65;
const TInt	KGsmWavEncodedFrameDuration = 40000; //40ms

//const TInt	KPcmWavEncodedFrameSize = 160*2;						
//const TInt	KPcmWavInputFrameSize   = KPcmWavEncodedFrameSize*2;

const TInt32	KMaxSampleRateSupported = 96000; //96000 Hz	

/*
 TMmfWavFormatPanics is an enumeration with the following entries:
 EPreconditionViolation indicates a precondition violation
 EPostConditionViolation indicates a post condition violation
*/
enum TMmfWavFormatPanics
	{
	EPostConditionViolation,
	EPreconditionViolation
	};

/**
* Panic
* This method generates a panic
* @internalTechnology
* @param "TInt"
*/
void Panic(TInt aPanicCode)
	{
	_LIT(KMMFWavFormatPanicCategory, "MMFWavFormat");
	User::Panic(KMMFWavFormatPanicCategory, aPanicCode);
	}


/**
*
* Read16
*  fast read for non-aligned 16-bit data
*/
LOCAL_C TUint16 Read16(const TUint8* aPtr)

    {
	TUint16 ret = *(REINTERPRET_CAST(const TUint16*, aPtr));
	return ret;
    }

/**
* Read32
* @internalTechnology
* @param aPtr
* fast read for non-aligned little-endian 32-bit data
*/
TUint32 Read32(const TUint8* aPtr)
    {
    TUint32 x = *aPtr++;
    x |= *aPtr++ << 8;
    x |= *aPtr++ << 16;
    x |= *aPtr++ << 24;
    return x;
    }

/**
*
* Write16
* @param aPtr
* @param aValue
* fast read for non-aligned 16-bit data
*
*/
LOCAL_C void Write16(TUint8*& aPtr, TInt16 aValue)
    {
    *aPtr++ = (TUint8)aValue;
	*aPtr++ = (TUint8)(aValue >> 8);
    }

/**
*
* Write32
* @internalTechnology
* @param aPtr
* @param aValue
*
*/
LOCAL_C void Write32(TUint8*& aPtr,TInt aValue)
    {
	*aPtr++ = (TUint8)aValue;
	*aPtr++ = (TUint8)(aValue >> 8);
	*aPtr++ = (TUint8)(aValue >> 16);
	*aPtr++ = (TUint8)(aValue >> 24);
    }



template<class T>
void TMMFWavConsolidationMethods<T>::ProcessFormatHeaderL()
	{
	iThat.DoReadL(0);

	FindRiffChunksL();
	
	iLastReadPosition = iThat.iFormatChunk.iPosition; // Should be beginning of fmt block
	iThat.DoReadL(iLastReadPosition);
	
	// Set the real format
	TUint8* rawform = &(iThat.iBuffer->Data()[0]);	//skip _fmt & length
	iThat.iCodecId = Read16(rawform); rawform+=2;
	iThat.iChannels = Read16(rawform); rawform+=2;
	
	iThat.iSampleRate = Read32(rawform); rawform+=4; // Skip bytes per second estimate
	if (!iThat.iSampleRate) 	User::Leave(KErrCorrupt);
	
	if (iThat.iSampleRate > KMaxSampleRateSupported) 	
		{
		User::Leave(KErrNotSupported);
		}

	iThat.iAverageBytesPerSecond = Read32(rawform); rawform+=4;
	iThat.iBlockAlign = Read16(rawform); rawform+=2;
	
	iThat.iBitsPerSample = Read16(rawform);
	rawform+=2;
	
	switch (iThat.iCodecId)
		{
		case KMMFWavFormatTypePcm:
			{
			if (iThat.iBitsPerSample == 8)		//8 bit PCM
				{ 
				iThat.iFourCC.Set(KMMFFourCCCodePCMU8);
				}	
			else if (iThat.iBitsPerSample == 16) //16 bit PCM
				{
				iThat.iFourCC.Set(KMMFFourCCCodePCM16);
				}
			else User::Leave(KErrNotSupported);
			}
			break;
		case KMMFWavFormatTypeImaAdpcm:
			{
			if ((iThat.iBitsPerSample == 4) && (iThat.iChannels == 1))		//mono IMA
				{ 
				iThat.iFourCC.Set(KMMFFourCCCodeIMAD);
				} 
			else User::Leave(KErrNotSupported);
			}
			break;
		case KMMFWavFormatTypeAlaw:
			{
			if (iThat.iBitsPerSample == 8)
				{ 
				iThat.iFourCC.Set(KMMFFourCCCodeALAW);
				}
			else User::Leave(KErrNotSupported);
			}
			break;
		case KMMFWavFormatTypeMulaw:
			{
			if (iThat.iBitsPerSample == 8)
				{ 
				iThat.iFourCC.Set(KMMFFourCCCodeMuLAW);
				}
			else User::Leave(KErrNotSupported);
			}
			break;
		case KMMFWavFormatTypeGSM610:
			{
			iThat.iFourCC.Set(KMMFFourCCCodeGSM610);
			}
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	
	if (iThat.iCodecId == KMMFWavFormatTypeImaAdpcm)
		{
		TUint16 extraData = Read16(rawform);
		if (extraData == 2)
			{
			rawform+=2;
			iThat.iSamplesPerBlock = Read16(rawform);
			rawform+=2;
			}
			
		if (iThat.iFactChunk.iFound)
			{
			// read number of audio samples from "fact" chunk
			iThat.DoReadL(iThat.iFactChunk.iPosition);
			TUint8* data = &(iThat.iBuffer->Data()[0]);
			iThat.iSamples = Read32(data);
			// it seems the iDataLength is always longer than the audio data length,
			// guess might be some more chunks in the end of the file. 
			// this following line fix one playing problem, as we only stop playing 
			// until eigher EOF of end of data.
			iThat.iDataLength = (iThat.iSamples >> 1);
			}
		}
	
	iThat.iHeaderLength =  iThat.iDataChunk.iPosition;

	// Find the data block
	iThat.iDataLength = iThat.iDataChunk.iLength;
	iThat.iStartPosition = iThat.iDataChunk.iPosition;

	//[ added to fix inconsistency, --oo ]
	//must account for possible trailing fact chunk
	if((iThat.iFactChunk.iFound) && (iThat.iFactChunk.iPosition > iThat.iDataChunk.iPosition))
		iThat.iClipLength = iThat.iFactChunk.iPosition + iThat.iFactChunk.iLength + (iThat.iFactChunk.iLength & 1);

	else
		iThat.iClipLength = iThat.iStartPosition + iThat.iDataLength + (iThat.iDataLength & 1);

	iThat.CalculateFrameSize();
	}


/**
*
* FindRiffChunks
* @param aClip
*
*/

template<class T>
void TMMFWavConsolidationMethods<T>::FindRiffChunksL()
	{
	iStartPtr = iThat.iBuffer->Data().Ptr();

	iThat.iFilePosition=0;
	iLastReadPosition=0;
	TBool foundChunks = EFalse;

	while (!foundChunks)
		{
		TInt advance=0;
		
		ReadChunk(&iCurrent);
		
		if (iCurrent.iName == KRiffChunkNameRiff)//we need to look INSIDE the RIFF chunk
			{
			if(iThat.iBuffer->Data().Length() < static_cast<TInt>(KRiffContainerChunkHeaderLength))
				User::Leave(KErrCorrupt);
			iThat.iRiffChunkLength = iCurrent.iLength + KRiffChunkHeaderLength;
			advance=KRiffContainerChunkHeaderLength;
			}
		else
			{
			if (iCurrent.iName == KRiffChunkNameFmt_)
				AssignChunkTo(&iThat.iFormatChunk);
			else if (iCurrent.iName == KRiffChunkNameFact)
				AssignChunkTo(&iThat.iFactChunk);		
			else if (iCurrent.iName == KRiffChunkNameData)
				AssignChunkTo(&iThat.iDataChunk);

			advance = iCurrent.iLength + KRiffChunkHeaderLength;		//... and skip all others
			}
		
		
		if (iThat.iDataChunk.iFound && iThat.iFormatChunk.iFound && iThat.iFactChunk.iFound)
			{
			foundChunks=ETrue;
			}
		else
			{//still have chunks to look for
			iThat.iFilePosition+=advance;
			if (iThat.iFilePosition & 1)
				iThat.iFilePosition++;
			
			if ((TUint)iThat.iFilePosition>=(TUint)iThat.iRiffChunkLength)
				{
				foundChunks=ETrue;//end of file
				}
			else
				{//make sure we have at least a chunk's worth left in the buffer
				if ((TUint)(iThat.iFilePosition - iLastReadPosition) > 
					(TUint)(iThat.iBuffer->Data().Length() - KRiffChunkHeaderLength))
					{
					iLastReadPosition = iThat.iFilePosition;
					iThat.DoReadL(iLastReadPosition);

					//We've done another read. If there's < chunk in the buffer then something's wrong
					if (iThat.iBuffer->Data().Length() < static_cast<TInt>(KRiffChunkHeaderLength))
						{
						if ((iThat.iFormatChunk.iFound) && (iThat.iDataChunk.iFound)) 
							{
							foundChunks = ETrue; //it should be ok to exit loop
							return;
							}
						else
							{
							User::Leave(KErrCorrupt);
							}
						}
					}	
				}
			}
		}
	
	iThat.iClipLength = iThat.iRiffChunkLength;
	if (iThat.iClipLength == 0) 
		User::Leave(KErrNotFound);
	else if (!(iThat.iDataChunk.iFound && iThat.iFormatChunk.iFound))
		User::Leave(KErrCorrupt);
	}


/**
*
* AssignChunkTo
* @param aAssignedChunk
*
*/
template<class T>
void TMMFWavConsolidationMethods<T>::AssignChunkTo(TMdaRiffChunk* aAssignedChunk)
	{
	Mem::Copy(REINTERPRET_CAST(TUint8*, aAssignedChunk), REINTERPRET_CAST(TUint8*, &iCurrent),sizeof(TMdaRiffChunk));
	aAssignedChunk->iFound=ETrue;
	}

/**
*
* ReadChunk
* @param aChunk
*
*/
template<class T>
void TMMFWavConsolidationMethods<T>::ReadChunk(TMdaRiffChunk* aChunk)
	{
	Mem::FillZ(REINTERPRET_CAST(TUint8*, aChunk),sizeof(TMdaRiffChunk)); // Zero data
	aChunk->iPosition=iThat.iFilePosition + KRiffChunkHeaderLength;
	aChunk->iName = Read32(iStartPtr + iThat.iFilePosition - iLastReadPosition);
	aChunk->iLength = Read32(iStartPtr + iThat.iFilePosition - iLastReadPosition + KRiffChunkDataLength);
	}





/**
*
* Duration
* @param aMediaId
* @returns TTimeIntervalMicroSeconds
*
*/
template<class T>
TTimeIntervalMicroSeconds TMMFWavConsolidationMethods<T>::Duration(TMediaId aMediaId) const
	{
	TTimeIntervalMicroSeconds duration(0); //default
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		{
		if (iThat.iFourCC == KMMFFourCCCodeGSM610) 
			{
			TUint actualBytesPerSecond = (KGSMAvgBytesPerSecond * iThat.iSampleRate) / KGSMAvgBytesPerSecondSampleRate;
			duration = DurationCalculation(iThat.iDataLength, actualBytesPerSecond);
			}
		else if (iThat.iFourCC == KMMFFourCCCodeIMAD && iThat.iFactChunk.iFound)
			{
			// IMAD, duration = samples / sample_rate, number of samples is stored in iFactChunk.iType
			duration = TTimeIntervalMicroSeconds(TInt64(iThat.iSamples) * TInt64(KOneSecondInMicroSeconds) / TInt64(iThat.iSampleRate) );
			}
		else 
			{
			duration = DurationCalculation(iThat.iDataLength, iThat.iSampleRate, iThat.iBitsPerSample, iThat.iChannels);
			}
		}
	return duration;
	}


/**
*
* CalculateFrameTimeInterval
* 
*/
template<class T>
void TMMFWavConsolidationMethods<T>::CalculateFrameTimeInterval()
	{
	if (iThat.iFourCC == KMMFFourCCCodeGSM610)
		iThat.iFrameTimeInterval = FrameTimeIntervalCalculation(iThat.iFrameSize, KGSMAvgBytesPerSecond);
	else
		iThat.iFrameTimeInterval = FrameTimeIntervalCalculation(iThat.iFrameSize, iThat.iSampleRate, iThat.iBitsPerSample, iThat.iChannels);
	}






template<class T>
TTimeIntervalMicroSeconds TMMFWavConsolidationMethods<T>::PositionL()
	{
	TTimeIntervalMicroSeconds position(0);
	if(iThat.iFourCC == KMMFFourCCCodeGSM610)
		{
		TTimeIntervalMicroSeconds GSMFrameDuration(KGsmWavEncodedFrameDuration);
		position =  DurationCalculation(iThat.iFilePosition - iThat.iStartPosition, GSMFrameDuration, KGsmWavEncodedFrameSize);
		}
	else
		{
		position =  DurationCalculation(iThat.iFilePosition - iThat.iStartPosition, iThat.iSampleRate, iThat.iBitsPerSample, iThat.iChannels);
		}

	return position;
	}


template<class T>
void TMMFWavConsolidationMethods<T>::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
	//if position is beyond the end of the file, set to end of file.
	if(aPosition >= Duration(KUidMediaTypeAudio))
		{
		iThat.iFilePosition =  iThat.iStartPosition + iThat.iDataLength;
		return;
		}


	TUint dataPosn;
	TUint sampleSize;
	if(iThat.iFourCC == KMMFFourCCCodeGSM610)
		{
		TTimeIntervalMicroSeconds GSMFrameDuration(KGsmWavEncodedFrameDuration);
		dataPosn = DataPosition(aPosition, GSMFrameDuration, KGsmWavEncodedFrameSize);

		sampleSize = KGsmWavEncodedFrameSize;
		}
	else if (iThat.iFourCC == KMMFFourCCCodeIMAD)
		{
		dataPosn =  DataPosition(aPosition, iThat.iSampleRate, iThat.iBitsPerSample, iThat.iChannels);

		// samples need to aligned to an ADPCM block boundary
		sampleSize = iThat.iBlockAlign;
		}
	else
		{
		dataPosn =  DataPosition(aPosition, iThat.iSampleRate, iThat.iBitsPerSample, iThat.iChannels);

		sampleSize = iThat.iBitsPerSample * iThat.iChannels / 8;
		
		//if iBitsPerSample < 8
		if(sampleSize <1)
			sampleSize=1;
		}

	//Must align the data read to the start of a sample
	if(sampleSize != 1)
		dataPosn = AlignToSample(dataPosn, sampleSize, 0, iThat.iDataLength);

	iThat.iFilePosition =  iThat.iStartPosition + dataPosn;
	}


/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes 
*
*/
template<class T>
void TMMFWavConsolidationMethods<T>::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	aDataTypes.Reset();
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCMU8));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAD));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeALAW));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeMuLAW));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeGSM610));
	}







/***********************************************************************************************/


/**
*
* NewL
* @param aSource
* @return CMMFFormatDecode* 
* 
*/
CMMFFormatDecode* CMMFWavFormatRead::NewL(MDataSource* aSource)
	{
	if ((aSource->DataSourceType()==KUidMmfDescriptorSource)||
			(aSource->DataSourceType()==KUidMmfFileSource))
		{//currently only files and descriptor MDataSources are supported
		CMMFWavFormatRead* self = new(ELeave)CMMFWavFormatRead;
		CleanupStack::PushL(self);
		self->ConstructL(aSource);
		CleanupStack::Pop();
		return static_cast<CMMFFormatDecode*>(self);
		}
	else 
		{
		User::Leave(KErrNotSupported);
		return NULL;
		}
	}



/**
*
* ConstructL
* @param aSource
* 
*/
void CMMFWavFormatRead::ConstructL(MDataSource* aSource)
	{
	iWavFormatConsolidator = new (ELeave) TMMFWavConsolidationMethods<CMMFWavFormatRead>(*this);

	CMMFFormatRead::ConstructL(aSource);
	}



/**
*
* ~CMMFWavFormatRead
*
*/
CMMFWavFormatRead::~CMMFWavFormatRead()
	{
	delete iWavFormatConsolidator;
	}
	





/**
*
* Duration
* @param aMediaId
* @returns TTimeIntervalMicroSeconds
*
*/
TTimeIntervalMicroSeconds CMMFWavFormatRead::Duration(TMediaId aMediaId) const
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	return iWavFormatConsolidator->Duration(aMediaId);
	}

/**
*
* CalculateFrameTimeInterval
* 
*/
void CMMFWavFormatRead::CalculateFrameTimeInterval()
	{
	__ASSERT_ALWAYS ( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	iWavFormatConsolidator->CalculateFrameTimeInterval();
	}



TTimeIntervalMicroSeconds CMMFWavFormatRead::PositionL()
	{
	__ASSERT_ALWAYS ( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	return iWavFormatConsolidator->PositionL();
	}

void CMMFWavFormatRead::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
	__ASSERT_ALWAYS ( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	iWavFormatConsolidator->SetPositionL(aPosition);
	}





	
/**
*
* ProcessFormatHeaderL
*  Read the format block and find the data chunk
*
*/
void CMMFWavFormatRead::ProcessFormatHeaderL()
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	iDataChunk.iFound = EFalse; // ensure that data chunk is re-read
	iWavFormatConsolidator->ProcessFormatHeaderL();
	}




/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes 
*
*/
void CMMFWavFormatRead::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	iWavFormatConsolidator->GetSupportedDataTypesL(aMediaId, aDataTypes);
	}


/**
*
* CustomInterface
* @param aInterfaceId
* @returns TAny*
*
*/
TAny* CMMFWavFormatRead::CustomInterface(TUid aInterfaceId)
	{
	if (aInterfaceId == KUidCustomInterfaceMmfDecodeBlockLength)
		{
		MMMFDecodeCustomInterfaceBlockLength* interface = this;
		return interface; 
		}
	else
		{
		return CMMFFormatRead::CustomInterface(aInterfaceId);
		}
	}
	
/**
*
* FileBlockLength
* @returns TUint
*
*/
TUint CMMFWavFormatRead::FileBlockLength()
	{
	return iBlockAlign;
	}
	

/***********************************************************************************************/

/**
*
* NewL
* @param aSink
* @returns CMMFFormatEncode*
*
*/
CMMFFormatEncode* CMMFWavFormatWrite::NewL(MDataSink* aSink)
	{
	if ((aSink->DataSinkType()==KUidMmfDescriptorSink)||
			(aSink->DataSinkType()==KUidMmfFileSink))
		{//currently only files and descriptor MDataSources are supported
		CMMFWavFormatWrite* self = new(ELeave)CMMFWavFormatWrite;
		CleanupStack::PushL(self);
		self->ConstructL(aSink);
		CleanupStack::Pop();
		return static_cast<CMMFFormatEncode*>(self);
		}
	else 
		{
		User::Leave(KErrNotSupported);
		return NULL;
		}
	}



/**
*
* ConstructL
* @param aSource
* 
*/
void CMMFWavFormatWrite::ConstructL(MDataSink* aSink)
	{
	iWavFormatConsolidator = new (ELeave) TMMFWavConsolidationMethods<CMMFWavFormatWrite>(*this);

	CMMFFormatWrite::ConstructL(aSink);
	}


/**
*
* ~CMMFWavFormatWrite
*
*/
CMMFWavFormatWrite::~CMMFWavFormatWrite()
	{
	delete iWavFormatConsolidator;
	}





/**
*
* NegotiateL
* @param aSource
*
*/
void CMMFWavFormatWrite::NegotiateL(MDataSource& aSource)
	{
	if (aSource.DataSourceType() == KUidMmfAudioInput)
		{
		CMMFDevSound& soundDevice = static_cast<MMMFAudioInput&>(aSource).SoundDevice();
		GetAudioSettingsFromDevice(&soundDevice, iSourceSampleRate, iSourceChannels, iSourceFourCC);
		}
	else if (aSource.DataSourceType() == KUidMmfFormatDecode)
		{//source is a clip so for now set sink settings to match source
		iSourceSampleRate = static_cast<CMMFFormatDecode&>(aSource).SampleRate();
		iSourceChannels = static_cast<CMMFFormatDecode&>(aSource).NumChannels();
		iSourceFourCC.Set(aSource.SourceDataTypeCode(TMediaId(KUidMediaTypeAudio)));
		
		iSourceWillSampleConvert = static_cast<CMMFFormatDecode&>(aSource).SourceSampleConvert();
		//make the start position the end of the clip
		}
	else return;
	
	//
	//[  
	// lets attempt to set the values to the source and check these are ok
	// otherwise set to the defaults ]
	//

	//[ check the sample rate is a valid one
	// and if not scrabble around to get one ]
	if (!IsValidSampleRate( iSampleRate ))
		{
		//[ Is SourceSampleRate Valid samplerate = source sample rate
		//                           samplerate = default sample rate ]
		// [ check source is a valid sample rate for wav formnat ]
		if( IsValidSampleRate(iSourceSampleRate) )
			{
			//[ source sample rate is Kosher and we can accept it]
			iSampleRate = iSourceSampleRate; //might have already been set by custom command
			}
		//[ lets use the first agrreeable rate as the default if all else
		// fails ]
		else iSampleRate = KMMFFormatDefaultSampleRate;
		}

	if (!iChannels)
		{
		//[ IMA ADPCM only support mono ]
		if(iFourCC == KMMFFourCCCodeIMAD)
			iChannels = EMMFMono;
		else
			//[ we will accept the number of source channels as being no problem]
			iChannels = iSourceChannels;
		}

	//[ IMA ADPCM only support mono ]
	if((iFourCC == KMMFFourCCCodeIMAD) && (iChannels != EMMFMono))
		User::Leave(KErrNotSupported);

	if ((!iBitsPerSample)&&(iFourCC != KMMFFourCCCodeGSM610)) //if no bits per sample assume 4CC has not been set except for GSM6
		{
		TInt errorCode = SetSinkDataTypeCode(iSourceFourCC, KUidMediaTypeAudio);
		if( errorCode != KErrNone )
			{
			//[set the four cc to a default value ]
			User::LeaveIfError( SetSinkDataTypeCode( KMMFFourCCCodePCM16, KUidMediaTypeAudio) );
			}

		if (iFourCC == KMMFFourCCCodePCMU8) 
			iBitsPerSample = 8;
		else if (iFourCC == KMMFFourCCCodeIMAD)
			iBitsPerSample = 4;
		else 
			iBitsPerSample = 16; //default to 16
		}
	
	if ((iFourCC == KMMFFourCCCodeIMAD) && (!iClipAlreadyExists))
		iStartPosition = KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize;
	else if ((iFourCC == KMMFFourCCCodeGSM610) && (!iClipAlreadyExists))
		iStartPosition = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
	
	//for now we won't support PCM16->GSM610 unless its 8Khz
	//for the next release (or possibly as a defect fix) we'll use the sample rate convertor to get the PCM to 8Khz
	if ((iFourCC == KMMFFourCCCodeGSM610) && (iSampleRate != 8000))
		User::Leave(KErrNotSupported);

	//now re-evaluate the frame size & interval
	CalculateFrameSize();

	// if we're converting suggest a frame size to the source format
	if (aSource.DataSourceType() == KUidMmfFormatDecode) 
		static_cast<CMMFFormatDecode&>(aSource).SuggestSourceBufferSize(iFrameSize);
	}


/**
*
* DetermineIfValidClip
* @param aClip
*
*/
TInt CMMFWavFormatWrite::DetermineIfValidClip()
	{
	TRAPD(err,DoReadL(0)); //read from the beginning of the clip
	if (err) 
		return err;

	// It is possible at this point iClipLength has had padding byte
	// add if chunk data length is odd, so do the same to the filesize
	// before comparing the two.
	// 
	TInt fileSize = static_cast<CMMFClip*>(iClip)->Size();
	
	fileSize += (fileSize & 1);
	
	// To avoid behavior break for now we do this only for Gsm610
	// If the RIFF chunk data size is greater than the file size and
	// it is not a new file, then the file must be corrupted or not a complete file.
	//
	if ( (iFourCC == KMMFFourCCCodeGSM610) && (iRiffChunkLength > fileSize) && (iBuffer->Data().Size() != 0) )
		{
		err = KErrCorrupt;
		}
	else if (iBuffer->Data().Size() >= static_cast<TInt>(iHeaderLength))
		{
		__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
		TRAP(err, iWavFormatConsolidator->FindRiffChunksL());
		}
	else if (iBuffer->Data().Size() ==0)
		err = KErrNotFound; //no file present
	else
		err = KErrCorrupt;

	return err;
	}


/**
*
* SetDefaultHeaderParametersL
*
*/
void CMMFWavFormatWrite::SetDefaultHeaderParametersL()
	{
	//If this is a new file, set initial data parameters, else retain the existing settings.
	//This situation can occur if the file has been deleted 
	if(!iClipAlreadyExists)
		{
		//Need an initial frame size, so assume certain settings
		iChannels      = KMMFFormatDefaultChannels;		       // can't have stereo
		iSampleRate    = KMMFFormatDefaultSampleRate;  // assume 8KHz for now
		iBitsPerSample = KMMFFormatDefaultBitsPerSample; // default bits per sample
		CalculateFrameSize();

		//these values must be set via Negotiation or explicitly set via a custom command
		iChannels      = 0;
	    iSampleRate    = 0;
		iBitsPerSample  =0;	
		iBlockAlign = KImaAdpcmBlockAlign;


		iFourCC.Set(KMMFFourCCCodePCM16); //16 bit PCM
		iHeaderLength = KWavFileUncompressedDataHeaderSize; //default for PCM.
		}

		iDataLength =0;
		iClipLength = iHeaderLength;
		iStartPosition = iClipLength;
	}



/**
*
* Duration
* @param aMediaId
* returns TTimeIntervalMicroSeconds
*
*/
TTimeIntervalMicroSeconds CMMFWavFormatWrite::Duration(TMediaId aMediaId) const
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	return iWavFormatConsolidator->Duration(aMediaId);
	}




TTimeIntervalMicroSeconds CMMFWavFormatWrite::PositionL()
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	return iWavFormatConsolidator->PositionL();
	}

void CMMFWavFormatWrite::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	iWavFormatConsolidator->SetPositionL(aPosition);
	}





/**
*
* EmptyBufferL
* @param aBuffer
* @param aSupplier
* @param aMediaId
* @preconditions
* aBuffer is not NULL
* aSupplier is not NULL
* max clip size has not been exceeded
* media is audio
*
*/
void CMMFWavFormatWrite::EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId)
	{
	//since wav always contains linear audio the sink buffer size can set the source buffer size
	//there needs to be a separate buffer negotiating phase but for now put it here
	//the code assumes the sink buffer size is constant
	//the sink buffer size will be used to determine the frame time.
	//check media id &pass onto clip

	//[ precondition the buffer is not null ]
	__ASSERT_DEBUG( aBuffer, Panic( EPreconditionViolation ) );
	
	if ((aBuffer != NULL) && (!CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type())))
		User::Leave(KErrNotSupported);
	
	//[ precondition it must be audio ]
	if(aMediaId.iMediaType!=KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	
	//[ precondition we have not exceeded the clip length]
	if( IsMaxClipLengthExceeded( *aBuffer ) )
		User::Leave(KErrEof );
	
	//[ precondition aSupplier is not NULL]
	__ASSERT_DEBUG( aSupplier, Panic( EPreconditionViolation ) );
	
	//[ assign the supplier to the data path for later asynch callback]
	iDataPath = aSupplier;
	TUint currentClipLength = static_cast<CMMFClip*>(iClip)->Size();
	
	if (iState == EInitialised)
		{	
		if (iFilePosition < iStartPosition) 
			iFilePosition = iStartPosition; //can't write before start of header		
		else 
			{
			if (iFilePosition > currentClipLength)
				iFilePosition = currentClipLength; //can't have position > clip length of the clip
			}


		// Check the buffer size and calculate the Frame Time Interval
		TUint bufferFrameSize = static_cast<CMMFDataBuffer*>(aBuffer)->Data().MaxLength();//PAC don't really know frame size till here
		CalculateFrameSize(bufferFrameSize);

		if ((iChannelAndSampleRateConverter) && (!iSourceWillSampleConvert))
			{//need to perform channel & sample rate conversion before writing to clip

			//need to create an intermediate buffer in which to place the converted data
			if(!iConvertBuffer || iConvertBufferSize < iChannelAndSampleRateConverter->MaxConvertBufferSize(bufferFrameSize))
				{
				if (iConvertBuffer)
					{
					delete iConvertBuffer;
					iConvertBuffer = NULL;
					}
				iConvertBufferSize = iChannelAndSampleRateConverter->MaxConvertBufferSize(bufferFrameSize);
				iConvertBuffer = CreateSinkBufferOfSizeL(iConvertBufferSize);
				}

			TInt frmSize = iChannelAndSampleRateConverter->Convert(*static_cast<CMMFDataBuffer*>(aBuffer),*iConvertBuffer);
			if (!iConverterFrameSizeSet  || !iFrameSize ) //[ modification made to fix effect of side effect calls ]
				{//need to set new framesize - note that the frmSize can vary by 2 bytes therfore 
				//to avoid bad filepositions just use the same frame size throughout
				iFrameSize = frmSize;
				iConverterFrameSizeSet = ETrue;
				}
			iBufferToEmpty = aBuffer; //save this so it can be returned to datapath
			
			STATIC_CAST( CMMFClip*, iClip )->WriteBufferL( iConvertBuffer, iFilePosition, this ) ;
			}
		else
			{//no need to convert the data
			iBufferToEmpty = aBuffer; //save this so it can be returned to datapath

						
			//this is an "emergency" quick fix to get GSM 610 working - AD
			//except it doesn't - PAC
			if ((iFourCC==KMMFFourCCCodeGSM610) && (!iGSM610FrameSizeSet))
				{//if
				//need to calculate the frame size for GSM610 which may be a multiple of actual GSM frames
				//ie will be 65 (KGsmWavEncodedFrameSize) bytes or a multiple of 65 bytes 
				TUint bufferLength =  aBuffer->BufferSize();
				if (bufferLength > iHeaderLength) //ensure buf has actual data
					{
					//check buffer length contains an integer of GSM610 frames
					//if buffer does not contain an integer of GSM610 frames then cannot use as frame size
					if (!(bufferLength % KGsmWavEncodedFrameSize))
						{
						iFrameSize = bufferLength; //Note that this assumes that the code source buffer
						//also has a constant size - this is I believe a reasonable assumption
						iGSM610FrameSizeSet = ETrue;
						}
					else iFrameSize = 0; //can't write frame if it does not contain complete GSM610 frames
					}
				}
			
			STATIC_CAST( CMMFClip*, iClip )->WriteBufferL( aBuffer, iFilePosition, this ) ;
			}
		}
	else	
		User::Leave(KErrNotReady);	
	}

/**
*
* SetSinkDataTypeCode
* @param aSinkFourCC
* @param aMediaId
* @returns TInt
*
*/
TInt CMMFWavFormatWrite::SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId)
	{
	TInt err = KErrNone;
	if (aMediaId.iMediaType != KUidMediaTypeAudio) 
		err = KErrNotSupported;
	else if ((iClipAlreadyExists) && (iFourCC != aSinkFourCC)) 
		err = KErrAlreadyExists; //can't set fourCC if clip exists - it already has a code
		
	if (!err)
		{		
		if (aSinkFourCC==KMMFFourCCCodeIMAD)
			{
			iBitsPerSample = 4;
			iFourCC = aSinkFourCC;
			}
		else if(aSinkFourCC == KMMFFourCCCodePCM16) 
			{
			iBitsPerSample = 16;
			iFourCC = aSinkFourCC;
			}
		else if(aSinkFourCC == KMMFFourCCCodeGSM610) //special case for GSM610
			{
			iBitsPerSample = 0;
			iSampleRate = 8000;
			iChannels = 1;
			iFourCC = aSinkFourCC;
			}
		else if(aSinkFourCC == KMMFFourCCCodeMuLAW ||
			aSinkFourCC == KMMFFourCCCodeALAW ||
			aSinkFourCC == KMMFFourCCCodePCMU8 ) 
			{
			iBitsPerSample = 8;
			iFourCC = aSinkFourCC;
			}
		else
			{
			err = KErrNotSupported;
			}
		}

	if (iFourCC==KMMFFourCCCodeIMAD)
		{
		iHeaderLength = KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize;
		iStartPosition = iHeaderLength;
		iFilePosition = iStartPosition;
		}
	else if (iFourCC==KMMFFourCCCodeGSM610)
		{
		iHeaderLength = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
		iStartPosition = iHeaderLength;
		iFilePosition = iStartPosition;
		}
	else
		iHeaderLength = KWavFileUncompressedDataHeaderSize; //default for PCM.


		
	return err;
	}


/**
*
*  ProcessFormatHeaderL
*  Read the format block and find the data chunk
*
*/
void CMMFWavFormatWrite::ProcessFormatHeaderL()
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	iWavFormatConsolidator->ProcessFormatHeaderL();
	}



/**
*
* OKToRecordL
* 
* @return TInt system wide error codes
*
*/
TInt CMMFWavFormatWrite::OKToRecord()
	{
	return KErrNone;
	}



/**
*
* WriteHeaderL
*
*/
void CMMFWavFormatWrite::WriteHeaderL()
	{
	if (iClipAlreadyExists)
		{
		TUint8* bufPtr = &iBuffer->Data()[0]; // Needn't be word aligned
		
		//Fact chunk may come after the data
		//must add pad byte if chunk data length is odd
		if((iFactChunk.iFound) && (iFactChunk.iPosition > iDataChunk.iPosition))
			iClipLength = iFactChunk.iPosition + iFactChunk.iLength + (iFactChunk.iLength & 1); 
		else
			iClipLength = iDataChunk.iPosition + iDataLength + (iDataLength & 1);

		iRiffChunkLength = iClipLength;
		iDataChunk.iLength = iDataLength;

		Write32(bufPtr, iClipLength - KRiffChunkHeaderLength);
		iBuffer->Data().SetLength(KRiffChunkDataLength);
		DoWriteL(4); //total length is always 4 bytes into file
		bufPtr -=4; // reset buffer ptr
		Write32(bufPtr,iDataLength);
		DoWriteL(iDataChunk.iPosition-4);//now write length of data
		
		//This is fix for DEF061537
		if (iFourCC==KMMFFourCCCodeIMAD)
			{
			TUint samplesPerBlock = (((KImaAdpcmBlockAlign-(4*iChannels)) * 8) / (iBitsPerSample*iChannels)) + 1;
			TUint numBlocks = iDataLength / KImaAdpcmBlockAlign;
			TUint dataLengthInSamples = samplesPerBlock * numBlocks;
			iSamples = dataLengthInSamples;	
			}
		}
	else //clip does not already exist  - need to write the entire header
		{
		if (iFourCC==KMMFFourCCCodeIMAD) //IMAD uses different header size....
			iDataLength = iClipLength - (KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize);
		else if (iFourCC==KMMFFourCCCodeGSM610) //GSM uses different header size....
			{
			const TInt KCompDataHeaderPlusFactChunkSize = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
				
			if (iClipLength < KCompDataHeaderPlusFactChunkSize)
				iDataLength = KCompDataHeaderPlusFactChunkSize;
			else
				iDataLength = iClipLength - KCompDataHeaderPlusFactChunkSize;
			}
		else 
			iDataLength = iClipLength - KWavFileUncompressedDataHeaderSize;

		
		TUint sinkFormatType = KMMFWavFormatTypePcm;	//default is PCM, everything else is an exception
		TUint wavHeaderSize = KWavFileUncompressedDataHeaderSize; //default for PCM, GSM610 is different.	
		
		//settings for ALaw RIFF header
		if (iFourCC==KMMFFourCCCodeALAW )
			{
			sinkFormatType = KMMFWavFormatTypeAlaw;
			}
		
		//settings for uLaw RIFF header
		else if (iFourCC==KMMFFourCCCodeMuLAW)
			{
			sinkFormatType = KMMFWavFormatTypeMulaw;
			}
		
		//settings for IMAD RIFF header
		else if (iFourCC==KMMFFourCCCodeIMAD)
			{
			sinkFormatType = KMMFWavFormatTypeImaAdpcm;
			wavHeaderSize = KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize;
			}
		
		//settings for GSM6 RIFF header
		else if (iFourCC==KMMFFourCCCodeGSM610)
			{
			sinkFormatType = KMMFWavFormatTypeGSM610;
			wavHeaderSize = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
			}
		
		iBuffer->Data().SetLength(wavHeaderSize);

		TUint8* bufPtr = &iBuffer->Data()[0]; // Needn't be word aligned
		Write32(bufPtr,KRiffChunkNameRiff);
		
		iClipLength = iDataLength + wavHeaderSize;	
		Write32(bufPtr, iClipLength - KRiffChunkHeaderLength);//+header-dataRIFF head
		Write32(bufPtr,KRiffChunkTypeWave);
		
		//format chunk
		Write32(bufPtr,KRiffChunkNameFmt_);
		
		if (sinkFormatType == KMMFWavFormatTypeImaAdpcm)
			Write32(bufPtr,KImaAdpcmFormatChunkLength);
		else if (sinkFormatType == KMMFWavFormatTypeGSM610)
			Write32(bufPtr,KGSMFormatChunkLength);
		else
			Write32(bufPtr,KDefaultFormatChunkLength);
		
		Write16(bufPtr, static_cast<TUint16>(sinkFormatType)); //encoding type (PCM, aLaw, uLaw, IMA)
		Write16(bufPtr, static_cast<TUint16>(iChannels)); //01 = mono, 02 = stereo
		Write32(bufPtr, iSampleRate);
		
		//average sample rate
		if (sinkFormatType == KMMFWavFormatTypeImaAdpcm)
			Write32(bufPtr, KImaAdpcmAvgBytesPerSecond);
		else if (sinkFormatType == KMMFWavFormatTypeGSM610)
			Write32(bufPtr, KGSMAvgBytesPerSecond);
		else 
			Write32(bufPtr,iSampleRate*((iBitsPerSample*iChannels)/8)); //calculate bytes per second
		
		//block align
		if ((iBitsPerSample == 8) && (iChannels == 2)) 
			Write16(bufPtr,0x02);
		else if ((iBitsPerSample == 16) && (iChannels == 1)) 
			Write16(bufPtr,0x02);
		else if ((iBitsPerSample == 16) && (iChannels == 2)) 
			Write16(bufPtr,0x04);
		else if ((iBitsPerSample == 4) && (sinkFormatType == KMMFWavFormatTypeImaAdpcm)) 
			Write16(bufPtr, static_cast<TUint16>(KImaAdpcmBlockAlign));	//special for IMA
		else if ((iBitsPerSample == 0) && (sinkFormatType == KMMFWavFormatTypeGSM610)) 
			Write16(bufPtr, static_cast<TUint16>(KGSMBlockAlign));  //Bytes Per Sample & Bits per sample
		else 
			Write16(bufPtr,0x01); //default 0x01
		
		Write16(bufPtr, static_cast<TUint16>(iBitsPerSample));
		
		//Extra data for Ima ADPCM
		if (sinkFormatType == KMMFWavFormatTypeImaAdpcm)
			{
			//Extra header data
			Write16(bufPtr, static_cast<TUint16>(KImaAdpcmExtraHeaderDataLength));
			
			//calculate samples per block
			TUint samplesPerBlock = (((KImaAdpcmBlockAlign-(4*iChannels)) * 8) / (iBitsPerSample*iChannels)) + 1;
			Write16(bufPtr, static_cast<TUint16>(samplesPerBlock));
			
			//fact chunk
			Write32(bufPtr, KRiffChunkNameFact);
			Write32(bufPtr, KImaAdpcmFactChunkLength);
			
			TUint numBlocks = iDataLength / KImaAdpcmBlockAlign;
			TUint dataLengthInSamples = samplesPerBlock * numBlocks;
			
			TUint extraBlockData = iDataLength % KImaAdpcmBlockAlign;
			if (extraBlockData)
				dataLengthInSamples += ((extraBlockData-4)*2)+1;
			
			Write32(bufPtr, dataLengthInSamples);
			}
		//Extra data for GSM610
		else if (sinkFormatType == KMMFWavFormatTypeGSM610)
			{
			//Extra header data
			Write16(bufPtr, static_cast<TUint16>(KGSMExtraHeaderDataLength));
			Write16(bufPtr, static_cast<TUint16>(KGSMSamplesPerBlock));
			
			//fact chunk
			Write32(bufPtr, KRiffChunkNameFact);
			Write32(bufPtr, KGSMFactChunkLength);
			Write32(bufPtr, 0x1F40);			//sample rate 8K
			}
		
		//Data chunk
		Write32(bufPtr,KRiffChunkNameData);
		Write32(bufPtr,iDataLength);
		
		DoWriteL(0); //write the header at the beginning of the clip
		}
		
	iFileHasChanged = ETrue;
	iHeaderUpdated = ETrue;
	}

/**
 *  BytesPerSecond.  Calculate and return the number of bytes used for on second of audio.
 *
 *	@return - bytesPerSecond
 */
TInt64 CMMFWavFormatWrite::BytesPerSecond() 
	{
	TInt64 bytesPerSecond;
	if (iFourCC==KMMFFourCCCodeGSM610)
		{
		bytesPerSecond = KGSMAvgBytesPerSecond;
		}
	else
		{	
		//Until these settings have been set by negotiation or by the User, assume default levels
		if (iSampleRate == 0 || iBitsPerSample == 0 || iChannels == 0)
			bytesPerSecond = (KMMFFormatDefaultSampleRate * KMMFFormatDefaultBitsPerSample * KMMFFormatDefaultChannels) / 8;
		else
			bytesPerSecond = (iSampleRate * iBitsPerSample * iChannels) / 8 ;
		}
	return bytesPerSecond ;
	}


/**
 *  CropL. This function shortens the clip from the position specified to the end specified.
 *  It rewrites the header.
 *  
 *
 *  @param aPosition -  Position in middle of clip at which to cut it
 *  @param aToEnd - if ETrue keep the last part, if EFalse keep the first part
 *
 *	@leave - standard error code
 */
void CMMFWavFormatWrite::CropL(TTimeIntervalMicroSeconds aPosition, TBool aToEnd )
	{	
	// Does clip have any size to crop
	if(iDataLength == 0)
		{
		return;
		}
	
	// Is aPosition between the start and the end? Need to be able to crop to zero
	if (( aPosition < TTimeIntervalMicroSeconds(0) ) || ( aPosition > Duration( KUidMediaTypeAudio) ) ) 
		User::Leave( KErrArgument ) ;
	
	// Convert aPosition to cropPosition in bytes
 	TInt64 cropPosition64 = TInt64( ( aPosition.Int64() * iSampleRate * (iBitsPerSample/8) * iChannels ) / KOneSecondInMicroSeconds ) ;
	
	//[ now calculate the position in an integer number of samlpes, to ensure we extract from the start of a sample
	TUint sampleSize = (iBitsPerSample/8) * iChannels;
	TUint cropPosition = AlignToSample(I64INT(cropPosition64), sampleSize, 0, iDataLength, ERoundDown);	

	//lets confirm that we are aligned on a sample boundary
	ASSERT( cropPosition % sampleSize == 0 );

	TInt dataSize = 0;  // This will be the size of the data left after cropping.
	
	if ( !aToEnd )
		{
		// Shift the data physically
		// move the data in blocks
		// Create a CMMFDataBuffer and use CMMFClip to shift the data
		iDataLength = iDataLength - cropPosition; 
		dataSize = iDataLength + (iDataLength & 1); //there may be a pad byte

		//[ assert the data size is >= 0 ]
		ASSERT( dataSize >= 0);
		


		if (( dataSize > 0 ) && (aPosition != TTimeIntervalMicroSeconds(0)))
			{
			TInt bufSize = ( dataSize < KDefineIOBufferSize ? dataSize : KDefineIOBufferSize ) ;
			CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(bufSize) ;
			CleanupStack::PushL( buffer ) ;
			
			TUint rPos = iDataChunk.iPosition + cropPosition; // read position 
			TUint wPos = iDataChunk.iPosition; //write position

			TInt dataToShift = ETrue ;
			while ( dataToShift )
				{
				static_cast<CMMFClip*>(iClip )->ReadBufferL( buffer, rPos ) ;  // synchronous calls
				static_cast<CMMFClip*>(iClip )->WriteBufferL( buffer, wPos );
				rPos += bufSize ;
				wPos += bufSize ;
 
				if ( rPos > iClipLength ) 
					dataToShift = EFalse ;  // past the end:  Done
				}// while dataToShift
			CleanupStack::PopAndDestroy( ) ; // buffer
			}// if data to shift
		}// crop from start
	else // crop to end
		iDataLength = dataSize = cropPosition ;
	

	iDataChunk.iLength = iDataLength; //maintain data length counters
	

	//is there a fact chunk after the data chunk
	if((iFactChunk.iFound) && (iFactChunk.iPosition > iDataChunk.iPosition))
		{
		TUint oldFactPointPosition = iFactChunk.iPosition;
		TInt factChunkSize = KRiffChunkHeaderLength + iFactChunk.iLength + (iFactChunk.iLength & 1);

		iFactChunk.iPosition = iDataChunk.iPosition + iDataLength +(iDataLength & 1)+ KRiffChunkHeaderLength;
		iClipLength = iFactChunk.iPosition + iFactChunk.iLength + (iFactChunk.iLength & 1);

		if(aToEnd) //must copy fact chunk onto end of file
			{
			TInt bufSize = ( factChunkSize < KDefineIOBufferSize ? factChunkSize : KDefineIOBufferSize ) ;

			CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(bufSize);
			CleanupStack::PushL(buffer);
			TUint rPos = oldFactPointPosition -  KRiffChunkHeaderLength; // read position 
			TUint wPos = iFactChunk.iPosition -  KRiffChunkHeaderLength; //write position

			TInt dataToShift = ETrue ;
			while ( dataToShift )
				{
				static_cast<CMMFClip*>(iClip)->ReadBufferL( buffer, rPos ) ;  // synchronous calls
				static_cast<CMMFClip*>(iClip)->WriteBufferL( buffer, wPos );
				rPos += bufSize ;
				wPos += bufSize ;
 
				if ( rPos > iClipLength ) 
					dataToShift = EFalse ;  // past the end:  Done
				}// while dataToShift
			CleanupStack::PopAndDestroy( ) ; // buffer
		
		
			}
		}
	else //data chunk is last chunk
		iClipLength = iDataChunk.iPosition + iDataLength + (iDataLength & 1); 

	WriteHeaderL();

	iStartPosition = iDataChunk.iPosition;

	// Do the physical chop
	User::LeaveIfError( (static_cast<CMMFClip*>(iClip))->SetSize( iClipLength ) );

	iClip->SinkStopL();
	}

/**
*
* CalculateFrameTimeInterval
*
*/
void CMMFWavFormatWrite::CalculateFrameTimeInterval()
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	 iWavFormatConsolidator->CalculateFrameTimeInterval();
	}



/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes
*
*/
void CMMFWavFormatWrite::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	__ASSERT_ALWAYS( iWavFormatConsolidator, Panic( EPreconditionViolation ) );
	iWavFormatConsolidator->GetSupportedDataTypesL(aMediaId, aDataTypes);
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidFormatWAVRead,	CMMFWavFormatRead::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidFormatWAVWrite,	CMMFWavFormatWrite::NewL)
	};

/**
* 
* ImplementationGroupProxy
* @param aTableCount
* @return TImplementationProxy*
*
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}



