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

#include "mmfSBCCodecImplementationUIDs.hrh" // KMmfUidSBCConfigure

#include "BtSBCEncoder.h"
#include "BtSBCConst.h"
#include "BtSBCFrameParameters.h"
#include "../../MmfBtFileDependencyUtil.h"

/**
The sbc configuration UID, used to identify configuration type, 
have to use this to configure any sbc codec.
*/
const TUid   KSBCConfigTypeUid = { KMmfUidSBCConfigure };

/**
SBC CRC shift register initial value for SBC CRC calculation
*/
const TUint8 KSbcCRCShiftRegisterInit 	 = 0x0f;
/**
SBC CRC XOR mask, derived from polynomial G(X) = X^8 + X^4 + X^3 + X^2 + 1
*/
const TUint8 KSbcCRCShiftRegisterXorMask = 0x1d;

#ifdef _DEBUG

enum 
	{
	ESbcBitstreamPosErr,
	ESbcSampleOverflow
	};
	
_LIT(KSBCEncoderPanicCategory, "CSBCEncoder");

static inline void Panic(TInt aError)
	{
	User::Panic(KSBCEncoderPanicCategory, aError);
	}
	
#endif
	
/* ========================= class CBitStreamParser ========================= */

/**
class CBitStreamParser constructor
@internalComponent
@param aBitStream
The bit stream buffer to be parsed
*/
CBitStreamParser::CBitStreamParser(TDes8& aBitStream) : iBitStream(aBitStream),	
	iByteOffset(0), iBitOffset(0)
	{
	iPtr = const_cast<TUint8*>(aBitStream.Ptr() );
	}
	
/**
class CBitStreamParser destructor
@internalComponent
*/
CBitStreamParser::~CBitStreamParser()
	{
	}
	
/**
class CBitStreamParser second phase constructor
@internalComponent
*/
void CBitStreamParser::ConstructL()
	{
	}

/**
This function creates a new CBitStreamParser object and push it into CleanupStack.
@internalComponent
@param aBitStream
The bit stream buffer to be parsed
@return pointer to the new CBitStreamParser object
@leave if out of memory
*/
CBitStreamParser* CBitStreamParser::NewLC(TDes8& aBitStream)
	{
	CBitStreamParser* self = new(ELeave) CBitStreamParser(aBitStream);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/**
This function reset the internal bit position of CBitStreamParser
@internalComponent
*/
void CBitStreamParser::Reset()
	{
	iPtr = const_cast<TUint8*>(iBitStream.Ptr() );
	iByteOffset = 0;
	iBitOffset = 0;
	}
	
/**
This function reads a number of bits from bit stream buffer at current bit position
and change the bit position to then end of the last bit read.
@internalComponent
@param aBitsToRead
Number of bits to read, at most 8 bits
@return the bits value in byte
@panic if bit position is outside of the stream buffer
*/
TUint8 CBitStreamParser::ReadBits(TInt aBitsToRead)
	{
	TUint8 result = 0;
	if (aBitsToRead >= 8 - iBitOffset)
		{
		__ASSERT_DEBUG(iByteOffset < static_cast<TUint>(iBitStream.MaxLength() ), Panic(ESbcBitstreamPosErr) );
		// extra code to handle exception for URel version
		if (iByteOffset >= static_cast<TUint>(iBitStream.MaxLength() ) )
			{
			return 0;
			}
			
		aBitsToRead -= (8 - iBitOffset);
		result = static_cast<TUint8>( (*iPtr & (0xff >> iBitOffset) ) << aBitsToRead);
		
		iPtr++;
		iByteOffset++;
		iBitOffset = 0;
		}
	if (aBitsToRead > 0)
		{
		__ASSERT_DEBUG(iByteOffset < static_cast<TUint>(iBitStream.MaxLength() ), Panic(ESbcBitstreamPosErr) );
		// extra code to handle exception for URel version
		if (iByteOffset >= static_cast<TUint>(iBitStream.MaxLength() ) )
			{
			return 0;
			}
			
		result |= static_cast<TUint8>( (*iPtr & (0xff >> iBitOffset) ) >> (8 - iBitOffset - aBitsToRead) );
		iBitOffset = static_cast<TUint8>(iBitOffset + aBitsToRead);
		}
	return result;
	}
	
/**
This function writes a number of bits to the bit stream buffer at current bit position
and change the bit position to then end of the last bit written.
@internalComponent
@param aBitsToWrite
Number of bits to write, at most 8 bits
@param aBitsValue
The bits value to write in byte
@panic if bit position is outside of the stream buffer
*/
void CBitStreamParser::WriteBits(TInt aBitsToWrite, TUint8 aBitsValue)
	{
	if (aBitsToWrite >= 8 - iBitOffset)
		{
		__ASSERT_DEBUG(iByteOffset < static_cast<TUint>(iBitStream.MaxLength() ), Panic(ESbcBitstreamPosErr) );
		// extra code to handle exception for URel version
		if (iByteOffset >= static_cast<TUint>(iBitStream.MaxLength() ) )
			{
			return;
			}
			
		aBitsToWrite -= (8 - iBitOffset);
		*iPtr &= ~(0xff >> iBitOffset); // clear bits
		*iPtr |= (aBitsValue >> aBitsToWrite) & (0xff >> iBitOffset); // set bits
		
		iPtr++;
		iByteOffset++;
		iBitOffset = 0;
		}
	if (aBitsToWrite > 0)
		{
		__ASSERT_DEBUG(iByteOffset < static_cast<TUint>(iBitStream.MaxLength() ), Panic(ESbcBitstreamPosErr) );
		// extra code to handle exception for URel version
		if (iByteOffset >= static_cast<TUint>(iBitStream.MaxLength() ) )
			{
			return;
			}
			
		*iPtr &= (0xff << (8 - iBitOffset) ) | (0xff >> (iBitOffset + aBitsToWrite) ); // clear bits
		*iPtr |= (aBitsValue << (8 - iBitOffset - aBitsToWrite) ) & (0xff >> iBitOffset); // set bits
		iBitOffset = static_cast<TUint8>(iBitOffset + aBitsToWrite);
		}
	}
	
/**
This function reads 8 bits from bit stream buffer at current bit position
and change the bit position to then end of the last bit read.
@internalComponent
@return the bits value in byte
*/
TUint8 CBitStreamParser::ReadByte()
	{
	return ReadBits(8);
	}
	
/**
This function writes 8 bits to the bit stream buffer at current bit position
and change the bit position to then end of the last bit written.
@internalComponent
@param aByteValue
The byte value to write
*/
void CBitStreamParser::WriteByte(TUint8 aByteValue)
	{
	WriteBits(8, aByteValue);
	}
	
/**
This function sets the bit position to a specific position.
@internalComponent
@param aByteOffset
New byte position to set
@param aBitOffset
New bit position to set
@panic if bit position is outside of the stream buffer
*/
void CBitStreamParser::SetPosition(TUint aByteOffset, TUint8 aBitOffset)
	{
	while (aBitOffset >= 8)
		{
		aBitOffset -= 8;
		iByteOffset++;
		}
	
	__ASSERT_DEBUG(iByteOffset < static_cast<TUint>(iBitStream.MaxLength() ), Panic(ESbcBitstreamPosErr) );
	// extra code to handle exception for URel version
	if (iByteOffset >= static_cast<TUint>(iBitStream.MaxLength() ) )
		{
		aBitOffset = 0;
		iByteOffset = iBitStream.MaxLength();
		}
		
	iPtr = const_cast<TUint8*>(iBitStream.Ptr() ) + aByteOffset;
	iByteOffset = aByteOffset;
	iBitOffset = aBitOffset;
	}
	
/**
This function gets the bit position.
@internalComponent
@param aByteOffset
Read byte position
@param aBitOffset
Read bit position
*/
void CBitStreamParser::Position(TUint& aByteOffset, TUint8& aBitOffset) const
	{
	aByteOffset = iByteOffset;
	aBitOffset = iBitOffset;
	}
	
/* ========================= class CSbcCRCCalculator ========================= */

/**
Constructor
@internalComponent
*/
CSbcCRCCalculator::CSbcCRCCalculator() : iShiftRegister(KSbcCRCShiftRegisterInit)
	{
	}
	
/**
This function resets the shift register value to intial SBC CRC value
@internalComponent
*/
void CSbcCRCCalculator::Reset()
	{
	iShiftRegister = KSbcCRCShiftRegisterInit;
	}
	
/**
This function inputs one bit into the shift register
@internalComponent
@param aBit
The lowest bit contains the bit to input. 
*/
void CSbcCRCCalculator::InputBit(TUint8 aBit)
	{
	TUint8 inputBit = static_cast<TUint8>( (iShiftRegister >> 7) ^ (aBit & 0x1) );
	iShiftRegister <<= 1;
	
	if (inputBit)
		{
		iShiftRegister ^= KSbcCRCShiftRegisterXorMask;
		}
	}
	
/**
This function inputs a number of bits into the shift register
@internalComponent
@param aBits
The number of bits to input, at most 8 bits.
@param aValue
The input bits value. 
*/
void CSbcCRCCalculator::InputBits(TUint8 aBits, TUint8 aValue)
	{
	for (TInt bit = aBits - 1; bit >= 0; bit--)
		{
		InputBit(static_cast<TUint8>(aValue >> bit) );
		}
	}
	
/**
This function inputs 8 bits into the shift register.
@internalComponent
@param aValue
The input byte value. 
*/
void CSbcCRCCalculator::InputByte(TUint8 aByte)
	{
	InputBit(static_cast<TUint8>(aByte >> 7) );
	InputBit(static_cast<TUint8>(aByte >> 6) );
	InputBit(static_cast<TUint8>(aByte >> 5) );
	InputBit(static_cast<TUint8>(aByte >> 4) );
	InputBit(static_cast<TUint8>(aByte >> 3) );
	InputBit(static_cast<TUint8>(aByte >> 2) );
	InputBit(static_cast<TUint8>(aByte >> 1) );
	InputBit(aByte);
	}
	
/**
This function gets the shift register value.
@internalComponent
@return the shift register value. 
*/
TUint8 CSbcCRCCalculator::ShiftRegister()
	{
	return iShiftRegister;
	}

/* ============================ class CSBCEncoder ============================ */

/**
Constructor.
@internalComponent
*/
CSBCEncoder::CSBCEncoder()
	{
	}
	
/**
Destructor.
@internalComponent
*/
CSBCEncoder::~CSBCEncoder()
	{
	delete iSbcFrameEncoder;
	delete iPcmSampleCach; // this is used to cache any remaining samples less than on frame
	}
	
/**
Second phase constructor.
@internalComponent
@param aInitParams
Initial parameters for creating this object, not in use for the moment
@leave never
*/
void CSBCEncoder::ConstructL(TAny* /*aInitParams*/)
	{
	}

/**
This function resets any existing audio samples from the cach.
@internalComponent
@leave never
*/
void CSBCEncoder::ResetL()
	{
	if (iPcmSampleCach)
		{
		iPcmSampleCach->Des().Zero();
		}
	}

/**
This function creates a new CSBCEncoder object.
@internalComponent
@param aInitParams
Initial parameters for creating this object, not in use for the moment
@leave if out of memory
*/
CMMFCodec* CSBCEncoder::NewL(TAny* aInitParams)
	{
	CSBCEncoder* self = new(ELeave) CSBCEncoder();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop();
	return self;
	}
	
/**
This function is used for configuring the CSBCEncoder object. Should be called before encode
@internalComponent
@param aConfigType
Configuration UID, has to be set to KSBCConfigTypeUid
@param aConfigData
A package buffer which contains all the settings
@leave KErrNotSupported if configuration UID does't match or parameters setting is invalid.
*/
void CSBCEncoder::ConfigureL(TUid aConfigType, const TDesC8& aConfigData)
	{
	if (aConfigType != KSBCConfigTypeUid)
		{
		User::Leave(KErrNotSupported);
		}
		
	const TSBCFrameParameters& param = 
		static_cast<const TPckgBuf<TSBCFrameParameters>&>(aConfigData)();
	
	if (param.Validate() != 0)
		{
		User::Leave(KErrArgument);
		}
	
	iParameters = param;
	
	if (iPcmSampleCach)
		{
		delete iPcmSampleCach;
		iPcmSampleCach = NULL;
		}

	if (iSbcFrameEncoder)
		{
		// must call this whenever ConfigureL() is called to make sure CSBCFrameEncoder
		// uses the new configuration, as we can change the configuration without creating 
		// a new CSBCFrameEncoder object
		iSbcFrameEncoder->Configure(iParameters); 
		}

	iSbcFrameLength = iParameters.CalcFrameLength();
	iPcmFrameSize = sizeof(TInt16) * param.BlockLength() * param.Subbands() * param.Channels();
	}

/**
This function encodes sbc audio stream with PCM16 audio source samples, and write processed 
result to destination buffer. It also caches any less than one frame remaining audio samples.
@internalComponent
@param aSrc
Source buffer contains the PCM16 audio source samples
@param aDst
Destination buffer to contain the encoded sbc audio stream
@return processed result
@leave 
	KErrAbort if configuration is invalid, 
	KErrArgument if destination buffer size is smaller than one frame length,
	KErrCorrupt if output bytes is not the same as frame length or 
				if we still have enough src and dst but the process stoped, 
	Or other errors e.g. out of memory error.
*/
TCodecProcessResult CSBCEncoder::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	// check if ConfigureL gets called already
	if (iParameters.Validate() != 0)
		{
		User::Leave(KErrAbort);
		}
	
	// check if dst big enough to hold at least one frame
	CMMFDataBuffer* dst = static_cast<CMMFDataBuffer*>(&aDst);
	const TUint dstMaxLen = dst->Data().MaxLength();
	
	if (dstMaxLen < iSbcFrameLength)
		{
		User::Leave(KErrArgument);
		}

	// process data
	const CMMFDataBuffer* src = static_cast<const CMMFDataBuffer*>(&aSrc);
	const TUint srcLen = src->Data().Length();
	TUint srcPos = src->Position();
	TUint dstPos = dst->Position();
	
	const TUint8* srcPtr = src->Data().Ptr();
	TUint8* dstPtr = const_cast<TUint8*>(dst->Data().Ptr() );
	TUint cachedSize = CachedSampleSize();
	
	while (cachedSize + srcLen - srcPos >= iPcmFrameSize && dstMaxLen - dstPos >= iSbcFrameLength)
		{
		TPtrC8 srcDes(srcPtr + srcPos, iPcmFrameSize);
		TPtr8  dstDes(dstPtr + dstPos, iSbcFrameLength);
		
		srcPos += EncodeFrameL(srcDes, dstDes);
		dstPos += iSbcFrameLength;
		cachedSize = 0;
		}
	
	// check result
	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessComplete;
	
	if (dstMaxLen - dstPos >= iSbcFrameLength) // still enough dst buffer
		{
		result.iStatus = TCodecProcessResult::EDstNotFilled;
		}
	
	// cach remaining src
	if (CachedSampleSize() + srcLen - srcPos >= iPcmFrameSize) // still enough src
		{
		if (result.iStatus == TCodecProcessResult::EDstNotFilled)
			{
			User::Leave(KErrCorrupt);
			}
		else
			{
			result.iStatus = TCodecProcessResult::EProcessIncomplete;
			}
		}
	else if (srcLen - srcPos > 1) // remaining src less than one frame, cach it
		{
		srcPos += CachePcmSamplesL(*src, srcPos);
		}
	
	// set new position for dst
	dst->Data().SetLength(dstPos);
	
	// return result
	result.iSrcBytesProcessed = srcPos - src->Position();
	result.iDstBytesAdded = dstPos - dst->Position();
	return result;
	}

/**
This function encodes one SBC frame with PCM16 audio source samples, and write processed 
result to destination buffer.
@internalComponent
@param aSrc
Source buffer contains the PCM16 audio source samples
@param aDst
Destination buffer to contain the encoded sbc audio stream
@return the number of bytes the source has been processed
@leave if out of memory.
*/
TUint CSBCEncoder::EncodeFrameL(const TDesC8& aSrc, TDes8& aDst)
	{
	if (!iSbcFrameEncoder)
		{
		iSbcFrameEncoder = CSBCFrameEncoder::NewL();
		iSbcFrameEncoder->Configure(iParameters);
		}
	
	if (CachedSampleSize() > 0)
		{ // encode one frame with cached samples and src
		TUint appendBytes = iPcmFrameSize - CachedSampleSize();
		// append src to cach to make up one frame
		iPcmSampleCach->Des().Append(aSrc.Ptr(), appendBytes);
		// encode cach
		iSbcFrameEncoder->EncodeFrameL(*iPcmSampleCach, aDst);
		// empty cach
		iPcmSampleCach->Des().Zero();
		// return bytes src processed
		return appendBytes;
		}
	else
		{
		// encode one frame with src only
		iSbcFrameEncoder->EncodeFrameL(aSrc, aDst);
		// return bytes src processed
		return iPcmFrameSize;
		}
	}

/**
This function caches any less than one frame remaining audio samples. 
@internalComponent
@param aSrc
Source buffer contains the PCM16 audio source samples.
@param aSrcPos
Position from where the samples are cached.
@return the number of bytes the source has been cached
@leave if out of memory.
*/
TUint CSBCEncoder::CachePcmSamplesL(const CMMFDataBuffer& aSrc, TUint aSrcPos)
	{
	if (!iPcmSampleCach)
		{
		iPcmSampleCach = HBufC8::NewL(iPcmFrameSize);
		}
	
	const TUint8* pSrc = aSrc.Data().Ptr() + aSrcPos;
	const TUint cachSize = (aSrc.Data().Length() - aSrcPos) & 0xfffe; // take even number
	
	iPcmSampleCach->Des().Append(pSrc, cachSize);
	
	return cachSize;
	}

/**
This function gets the size of the cach.
@internalComponent
@return the cached samples size
*/
TUint CSBCEncoder::CachedSampleSize()
	{
	if (!iPcmSampleCach)
		{
		return 0;
		}
	return iPcmSampleCach->Des().Size();
	}
	
/* ========================== class CSBCFrameEncoder ========================== */

/**
Constructor.
@internalComponent
*/
CSBCFrameEncoder::CSBCFrameEncoder()
	{
	}

/**
Destructor.
@internalComponent
*/
CSBCFrameEncoder::~CSBCFrameEncoder()
	{
	}
	
/**
Second phase constructor.
@internalComponent
*/
void CSBCFrameEncoder::ConstructL()
	{
	}
	
/**
This function creates a CSBCFrameEncoder object.
@internalComponent
@return pointer of the CSBCFrameEncoder object.
*/
CSBCFrameEncoder* CSBCFrameEncoder::NewL()
	{
	CSBCFrameEncoder* self = new(ELeave) CSBCFrameEncoder();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
This function resets the analysis filter bank, this should be called everytime 
when encoding a new sbc audio stream.
@internalComponent
*/
void CSBCFrameEncoder::Reset()
	{
	const TUint8 numChannels = iParameters.Channels();
	const TUint8 numSubbands = iParameters.Subbands();
	
	for (TUint8 channel = 0; channel < numChannels; channel++)
		{
		TInt16* analysisSamples = iAnalysisSamples[channel];
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			*analysisSamples++ = 0;
			}
		}
	}
	
/**
This function sets the configuration for SBC Frame Encoder and resets the analysis filter bank.
@internalComponent
@param aParameters
This contains all the parameters to set
*/
void CSBCFrameEncoder::Configure(const TSBCFrameParameters& aParameters)
	{
	iParameters = aParameters;
	// has to call this whenever the configuration changed, this resets the Analyse Filter Bank
	Reset();
	}
	
/**
This function encodes one SBC frame with PCM16 source samples and output it to destination buffer.
@internalComponent
@param aSrc
Source buffer contains the source samples
@param aFrame
Destination buffer to contain the processed sbc audio stream
@leave if out of memory
*/
void CSBCFrameEncoder::EncodeFrameL(const TDesC8& aSrc, TDes8& aFrame)
	{
	// encode frame
	Analyse(aSrc);
	CalcScaleFactors();
	JoinSubbands();
	CalcBitAllocation();
	Quantize();
	
	// output frame
	WriteFrameL(aFrame);
	}

/**
This function does the analysis filtering, the analysed samples are used to encode sbc subbands.
@internalComponent
@param aSrc
Source buffer contains the source samples
*/
void CSBCFrameEncoder::Analyse(const TDesC8& aSrc)
	{
	const TUint8 channelMode = iParameters.ChannelMode();
	const TInt16* inputSamples = reinterpret_cast<const TInt16*>(aSrc.Ptr() );
	
	if (channelMode == TSBCFrameParameters::EMono)
		{
		AnalyseMono(inputSamples);
		}
	else // two-channel modes
		{
		// two channel samples are interleavedly stored in the following order
		// one left sample, one right sample, ...
		AnalyseOneChannel(inputSamples, 0);
		AnalyseOneChannel(inputSamples + 1, 1);
		}
	}

/**
This function analyses audio samples for Mono and Dual Channel modes.
@internalComponent
@param aInputSamples
Array of source samples
*/
void CSBCFrameEncoder::AnalyseMono(const TInt16 aInputSamples[])
	{
	const TUint8 numSubbands = iParameters.Subbands();
	const TUint8 numBlocks = iParameters.BlockLength();
	
	for (TUint8 block = 0; block < numBlocks; block++)
		{
		if (numSubbands == 4)
			{
			Analyse4Subbands(aInputSamples, block, 0);
			}
		else
			{
			Analyse8Subbands(aInputSamples, block, 0);
			}
		aInputSamples += numSubbands;
		}
	}
	
/**
This function analyses audio samples for Stereo and Joint Stereo modes.
@internalComponent
@param aInputSamples
Array of source samples
@param aChannel
The channel number to be analysed
*/
void CSBCFrameEncoder::AnalyseOneChannel(const TInt16 aInputSamples[], TUint8 aChannel)
	{
	const TUint8 numSubbands = iParameters.Subbands();
	const TUint8 numBlocks = iParameters.BlockLength();
	
	TInt16 inputSamples[KSbcMaxSubbands];
	for (TUint8 block = 0; block < numBlocks; block++)
		{
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			inputSamples[subband] = *aInputSamples;
			aInputSamples += 2; // 1 left sample, 1 right sample, ... 
			}
			
		if (numSubbands == 4)
			{
			Analyse4Subbands(inputSamples, block, aChannel);
			}
		else
			{
			Analyse8Subbands(inputSamples, block, aChannel);
			}
		}
	}
	

/**
This function analyses 4 subbands for sbc frame with 4 subbands.
@internalComponent
@param aInputSamples
Array of source samples
@param aBlock
The block number to be analysed
@param aChannel
The channel number to be analysed
*/
void CSBCFrameEncoder::Analyse4Subbands(const TInt16 aInputSamples[], TUint8 aBlock, TUint8 aChannel)
	{
	// for easier understanding, this code is a copy from the A2DP spec, 
	// all the naming are kept.
	TInt i = 0;
	TInt k = 0;
	TInt16* X = iAnalysisSamples[aChannel]; // 40 analyse samples
	
	for (i = 39; i >= 4; i--)
		{
		X[i] = X[i - 4];
		}
	for (i = 3; i >= 0; i--)
		{
		X[i] = *aInputSamples++;
		}
	
	TInt64 Y[8]; // partial calculation, see Figure 12.5 in A2DP spec for detail
	for (i = 0; i < 8; i++)
		{
		TInt64 sum = 0;
		for (k = 0; k <= 4; k++)
			{
			// for some strange reason, RVCT is not happy about converting 
			// TInt16 to TInt64 in the equation directly.
			const TInt64 sample = X[i + (k << 3)];
			sum += KSBCProto4[i + (k << 3)] * sample;
			}
		Y[i] = sum >> (KSBCProtoBitsShift - 10);
		}

	TInt32* outputSamples = iOutputSamples[aBlock][aChannel];
	for (i = 0; i < 4; i++)
		{
		const TInt32* M = KSBCAnalysisMatrix4[i];
		TInt64 sum = 0;
		for (k = 0; k < 8; k++)
			{
			sum += M[k] * Y[k];
			}
		sum >>= (KSBCAnalysisMatrixBitsShift + 9);
		sum = (sum >> 1) + (sum & 0x1);
		outputSamples[i] = static_cast<TInt32>(sum);
		}
	}

/**
This function analyses 8 subbands for sbc frame with 8 subbands.
@internalComponent
@param aInputSamples
Array of source samples
@param aBlock
The block number to be analysed
@param aChannel
The channel number to be analysed
*/
void CSBCFrameEncoder::Analyse8Subbands(const TInt16 aInputSamples[], TUint8 aBlock, TUint8 aChannel)
	{
	// for easier understanding, this code is a copy from the A2DP spec, 
	// all the naming are kept.
	TInt i = 0;
	TInt k = 0;
	TInt16* X = iAnalysisSamples[aChannel]; // 80 analysis samples
	
	for (i = 79; i >= 8; i--)
		{
		X[i] = X[i - 8];
		}
	for (i = 7; i >= 0; i--)
		{
		X[i] = *aInputSamples++;
		}
	
	TInt64 Y[16]; // partial calculation, see Figure 12.5 in A2DP spec for detail
	for (i = 0; i < 16; i++)
		{
		TInt64 sum = 0;
		for (k = 0; k <= 4; k++)
			{
			// for some strange reason, RVCT is not happy about converting 
			// TInt16 to TInt64 in the equation directly.
			const TInt64 sample = X[i + (k << 4)];
			sum += KSBCProto8[i + (k << 4)] * sample;
			}
		Y[i] = sum >> (KSBCProtoBitsShift - 10);
		}
	
	TInt32* outputSamples = iOutputSamples[aBlock][aChannel];
	for (i = 0; i < 8; i++)
		{
		const TInt32* M = KSBCAnalysisMatrix8[i];
		TInt64 sum = 0;
		for (k = 0; k < 16; k++)
			{
			sum += M[k] * Y[k];
			}
		sum >>= (KSBCAnalysisMatrixBitsShift + 9);
		sum = (sum >> 1) + (sum & 0x1);
		outputSamples[i] = static_cast<TInt32>(sum);
		}
	}
	
/**
This function calculates the scale factor for one sample.
@internalComponent
@param aSample
A sample 
@return scale factor of thie sample
*/
static inline TUint8 ScaleFactor(TInt32 aSample)
	{
	if (aSample < 0)
		aSample = -aSample;
	
	// top bit of the sample is sign bit, ignore it
	// start from the second high bit	
	TUint32 mask = 0x40000000; 
	for (TInt8 bit = 30; bit > 0; bit--)
		{
		if (aSample & mask)
			{
			return bit;
			}
		mask >>= 1;
		}
	return 0;
	}
	
/**
This function calculates the scale factors for all samples in one sbc frame.
@internalComponent
*/
void CSBCFrameEncoder::CalcScaleFactors()
	{
	const TUint8 numBlocks = iParameters.BlockLength();
	const TUint8 numChannels = iParameters.Channels();
	const TUint8 numSubbands = iParameters.Subbands();
	
	TInt32 maxSubbandValues[KSbcMaxChannels][KSbcMaxSubbands];
	
	// find all maximum values of each subband
	for (TUint8 block = 0; block < numBlocks; block++)
		{
		for (TUint8 channel = 0; channel < numChannels; channel++)
			{
			const TInt32* samples = iOutputSamples[block][channel];
			TInt32* maxValues = maxSubbandValues[channel];
			
			for (TUint8 subband = 0; subband < numSubbands; subband++)
				{
				if (block == 0 || Abs(*samples) > *maxValues)
					{
					*maxValues = Abs(*samples);
					}
				samples++;
				maxValues++;
				}
			}
		}
	
	// calculate scale factors for all subband
	for (TUint8 channel = 0; channel < numChannels; channel++)
		{
		const TInt32* maxValues = maxSubbandValues[channel];
		TUint8* scale = iScaleFactors[channel];
		
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			*scale++ = ScaleFactor(*maxValues++);
			}
		}
	}

/**
This function joins two subband samples for Joint Stereo mode.
@internalComponent
@param aLeftSample
Left channel subband sample 
@param aRightSample
Right channel subband sample 
*/
static inline void JoinTwoSamples(TInt32& aLeftSample, TInt32& aRightSample)
	{
	aLeftSample = (aLeftSample + aRightSample) >> 1; // L1 = (L0 + R0) / 2
	aRightSample = aLeftSample - aRightSample; // R1 = L1 - R0 = (L0 - R0) / 2
	}

/**
This function sets the join flats for all subbands for one frame, 
and joins those subbands if needed for this frame.
@internalComponent
*/
void CSBCFrameEncoder::JoinSubbands()
	{
	if (iParameters.ChannelMode() != TSBCFrameParameters::EJointStereo)
		{
		return;
		}
	
	const TUint8 numBlocks = iParameters.BlockLength();
	const TUint8 numSubbands = iParameters.Subbands();
	
	TInt32 maxJoinValues[2][KSbcMaxSubbands - 1]; // 2 channels
	
	// find maximum join subband values
	for (TUint8 block = 0; block < numBlocks; block++)
		{
		const TInt32* leftSamples = iOutputSamples[block][0];
		const TInt32* rightSamples = iOutputSamples[block][1];
		
		TInt32* maxLeftJoin = maxJoinValues[0];
		TInt32* maxRightJoin = maxJoinValues[1];
		
		for (TUint8 subband = 0; subband < numSubbands - 1; subband++)
			{
			TInt32 leftJoin = *leftSamples++;
			TInt32 rightJoin = *rightSamples++;
			
			JoinTwoSamples(leftJoin, rightJoin);
			
			if (block == 0 || Abs(leftJoin) > *maxLeftJoin)
				{
				*maxLeftJoin = Abs(leftJoin);
				}
			if (block == 0 || Abs(rightJoin) > *maxRightJoin)
				{
				*maxRightJoin = Abs(rightJoin);
				}
			maxLeftJoin++;
			maxRightJoin++;
			}
		}
	
	// calculate scale factors for all join subbands
	const TInt32* maxLeftJoin = maxJoinValues[0];
	const TInt32* maxRightJoin = maxJoinValues[1];
	
	TUint8* leftScale = iScaleFactors[0];
	TUint8* rightScale = iScaleFactors[1];
	
	for (TUint8 subband = 0; subband < numSubbands - 1; subband++)
		{
		const TUint8 leftJoinScale = ScaleFactor(*maxLeftJoin++);
		const TUint8 rightJoinScale = ScaleFactor(*maxRightJoin++);
		
		iJoin[subband] = 0;
		if (leftJoinScale + rightJoinScale < *leftScale + *rightScale)
			{
			iJoin[subband] = 1;
			*leftScale = leftJoinScale;
			*rightScale = rightJoinScale;
			}
		leftScale++;
		rightScale++;
		}
	iJoin[numSubbands - 1] = 0; // join[subband - 1] is always 0
	
	// now do the joining job
	DoJoinSubbands();
	}
	
/**
This function joins all subbands if needed for this frame.
@internalComponent
*/
void CSBCFrameEncoder::DoJoinSubbands()
	{
	const TUint8 numBlocks = iParameters.BlockLength();
	const TUint8 numSubbands = iParameters.Subbands();
	
	for (TUint8 block = 0; block < numBlocks; block++)
		{
		TInt32* leftSamples = iOutputSamples[block][0];
		TInt32* rightSamples = iOutputSamples[block][1];
		
		for (TUint8 subband = 0; subband < numSubbands - 1; subband++)
			{
			if (iJoin[subband])
				{
				JoinTwoSamples(*leftSamples, *rightSamples);
				}
			leftSamples++;
			rightSamples++;
			}
		}
	}
	
/**
This function quantizes one sample according to:
	sample = (sample / scale + 1.0) * level / 2.0
	scale = 2 ^ (scale_factor + 1)
	level = (2 ^ bits) - 1
@internalComponent
@param aSample
A sample to be quantized.
@param aScaleFactor
The scale factor value.
@param aBits
The number of bits for this sample
@panic if quantized sample overflow
*/
static void QuantizeOneSample(TInt32& aSample, TUint8 aScaleFactor, TUint8 aBits)
	{
	// output = sample + scale
	TInt64 temp = (TInt)aSample + (0x1 << (aScaleFactor + 1) ); 
	// output = (sample + scale) * level / scale
	temp = ( (temp << aBits) - temp) >> (aScaleFactor + 2);

	aSample = static_cast<TInt32>(temp);

	// check bounce
	__ASSERT_DEBUG(aSample >= 0 && aSample <= (TInt32)0xffff, Panic(ESbcSampleOverflow) );
	// extra code to handle exception for URel version
	if (aSample < 0)
		{
		aSample = 0;
		}
	if (aSample > (TInt32)0xffff)
		{
		aSample = (TInt32)0xffff;
		}
	}
	
/**
This function quantizes all samples in one sbc frame.
@internalComponent
*/
void CSBCFrameEncoder::Quantize()
	{
	const TUint8 numBlocks = iParameters.BlockLength();
	const TUint8 numChannels = iParameters.Channels();
	const TUint8 numSubbands = iParameters.Subbands();
	
	for (TUint8 block = 0; block < numBlocks; block++)
		{
		for (TUint8 channel = 0; channel < numChannels; channel++)
			{
			const TUint8* bits = iBits[channel];
			const TUint8* scale = iScaleFactors[channel];
			TInt32* samples = iOutputSamples[block][channel];

			for (TUint8 subband = 0; subband < numSubbands; subband++)
				{
				QuantizeOneSample(*samples++, *scale++, *bits++);
				}
			}
		}
	}
	
/**
This function calculates bit allocation for all samples in one sbc frame using scale factors.
@internalComponent
*/
void CSBCFrameEncoder::CalcBitAllocation()
	{
	switch (iParameters.ChannelMode())
		{
		case TSBCFrameParameters::EMono:
		case TSBCFrameParameters::EDualChannel:
			CalcBitAllocIndependent();
			break;
		
		case TSBCFrameParameters::EStereo:
		case TSBCFrameParameters::EJointStereo:
			CalcBitAllocCombined();
			break;
		}
	}

/**
This function calculates bit allocation for one channel for Mono and Dual Channel.
@internalComponent
*/
void CSBCFrameEncoder::CalcBitAllocIndependent()
	{
	const TUint8 numChannels = iParameters.Channels();
	for (TUint8 channel = 0; channel < numChannels; channel++)
		{
		TInt8 bitneed[KSbcMaxSubbands];
		CalcBitneedIndependent(bitneed, iScaleFactors[channel]);
		DistributeBitsIndependent(bitneed, iBits[channel]);
		}
	}

/**
This function calculates bitneed for one channel for Mono and Dual Channel.
@internalComponent
@param aBitneed
Array of bitneed to hold the result
@param aScaleFactors
The scale factors used for this calculation
*/
void CSBCFrameEncoder::CalcBitneedIndependent(TInt8 aBitneed[], const TUint8 aScaleFactors[])
	{
	// see A2DP spec for reference
	const TUint8 numSubbands = iParameters.Subbands();
	
	if (iParameters.AllocationMethod() == TSBCFrameParameters::ESNR)
		{
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			*aBitneed++ = *aScaleFactors++;
			}
		}
	else // Loudness
		{
		const TInt8* offset = NULL;
		if (numSubbands == 4)
			{
			offset = KSBCOffset4[iParameters.SamplingFrequencyEnum()];
			}
		else
			{
			offset = KSBCOffset8[iParameters.SamplingFrequencyEnum()];
			}
			
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			if (*aScaleFactors == 0)
				{
				*aBitneed = -5;
				}
			else if ( (*aBitneed = static_cast<TUint8>(*aScaleFactors - *offset) ) > 0)
				{
				(*aBitneed) >>= 1;
				}
			aScaleFactors++;
			aBitneed++;
			offset++;
			}
		}
	}
	
/**
This function gets the maximum bitneed in one channel for Mono and Dual Channel.
@internalComponent
@param aBitneed
Array of bitneed.
*/
TInt8 CSBCFrameEncoder::MaxBitneedIndependent(const TInt8 aBitneed[])
	{
	// see A2DP spec for reference
	TInt8 maxBitneed = 0;
	const TUint8 numSubbands = iParameters.Subbands();
	
	for (TUint8 subband = 0; subband < numSubbands; subband++)
		{
		if (*aBitneed > maxBitneed)
			{
			maxBitneed = *aBitneed;
			}
		aBitneed++;
		}
		
	return maxBitneed;
	}
	
/**
This function calculates how many bitslices fit into the bitpool for one channel 
for Mono and Dual Channel.
@internalComponent
@param aBitneed
Array of bitneed.
@param aBitCount
The bit count, counts how many bits used
@return the number of bitslices
*/
TInt8 CSBCFrameEncoder::CalcBitSlicesIndependent(const TInt8 aBitneed[], TInt& aBitCount)
	{
	// see A2DP spec for reference
	aBitCount = 0;
	TInt8 sliceCount = 0;
	TInt8 bitSlices = static_cast<TInt8>(MaxBitneedIndependent(aBitneed) + 1);
	
	const TUint8 numSubbands = iParameters.Subbands();
	const TUint8 bitpool = iParameters.Bitpool();
	
	do {
		bitSlices--;
		aBitCount += sliceCount;
		sliceCount = 0;
		
		const TInt8* bitneed = aBitneed;
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			if (*bitneed > bitSlices + 1 && *bitneed < bitSlices + 16)
				{
				sliceCount++;
				}
			else if (*bitneed == bitSlices + 1)
				{
				sliceCount += 2;
				}
			bitneed++;
			}
		} while (aBitCount + sliceCount < bitpool);
	
	if (aBitCount + sliceCount == bitpool)
		{
		aBitCount += sliceCount;
		bitSlices--;
		}
		
	return bitSlices;
	}

/**
This function distributes number of bits to each subband for all samples
for Mono and Dual Channel.
@internalComponent
@param aBitneed
Array of bitneed.
@param aBits
Bits allocated for each subbands
*/
void CSBCFrameEncoder::DistributeBitsIndependent(const TInt8 aBitneed[], TUint8 aBits[])
	{
	// see A2DP spec for reference
	TInt bitCount = 0;
	TInt8 bitSlices = CalcBitSlicesIndependent(aBitneed, bitCount);
	
	const TUint8 numSubbands = iParameters.Subbands();
	
	// distribute bits until the last bitslice is reached
	TUint8 subband = 0;
	for (; subband < numSubbands; subband++)
		{
		if (aBitneed[subband] < bitSlices + 2)
			{
			aBits[subband] = 0;
			}
		else
			{
			aBits[subband] = static_cast<TUint8>(Min(aBitneed[subband] - bitSlices, 16) ); 
			}
		}
	
	// distribute the remaining bits
	const TUint8 bitpool = iParameters.Bitpool();
	
	subband = 0;
	while (bitCount < bitpool && subband < numSubbands)
		{
		if (aBits[subband] >= 2 && aBits[subband] < 16)
			{
			aBits[subband]++;
			bitCount++;
			}
		else if (aBitneed[subband] == bitSlices + 1 && bitpool > bitCount + 1)
			{
			aBits[subband] += 2; // ? bits[ch][sb] = 2 in A2DP spec, a bug in the spec?
			bitCount += 2;
			}
		subband++;
		}
	
	subband = 0;
	while (bitCount < bitpool && subband < numSubbands)
		{
		if (aBits[subband] < 16)
			{
			aBits[subband]++;
			bitCount++;
			}
		subband++;
		}
	}

/**
This function calculates bit allocation for both channels for Stereo and Joint Stereo.
@internalComponent
*/
void CSBCFrameEncoder::CalcBitAllocCombined()
	{
	TInt8 bitneed[2][KSbcMaxSubbands];
	
	CalcBitneedCombined(bitneed);
	DistributeBitsCombined(bitneed);
	}

/**
This function calculates bitneed for both channels for Stereo and Joint Stereo.
@internalComponent
@param aBitneed
Array of bitneed to hold the result
*/
void CSBCFrameEncoder::CalcBitneedCombined(TInt8 aBitneed[][KSbcMaxSubbands])
	{
	// see A2DP spec for reference
	const TUint8 numSubbands = iParameters.Subbands();
	
	if (iParameters.AllocationMethod() == TSBCFrameParameters::ESNR)
		{
		for (TInt8 channel = 0; channel < 2; channel++)
			{
			const TUint8* scaleFactor = iScaleFactors[channel];
			TInt8* bitneed = aBitneed[channel];
			for (TInt8 subband = 0; subband < numSubbands; subband++)
				{
				*bitneed++ = *scaleFactor++;
				}
			}
		}
	else // Loudness
		{
		const TInt8* offset = NULL;
		if (numSubbands == 4)
			{
			offset = KSBCOffset4[iParameters.SamplingFrequencyEnum()];
			}
		else
			{
			offset = KSBCOffset8[iParameters.SamplingFrequencyEnum()];
			}
			
		for (TInt8 channel = 0; channel < 2; channel++)
			{
			const TUint8* scaleFactor = iScaleFactors[channel];
			TInt8* bitneed = aBitneed[channel];
			for (TUint8 subband = 0; subband < numSubbands; subband++)
				{
				if (*scaleFactor == 0)
					{
					*bitneed = -5;
					}
				else if ( (*bitneed = static_cast<TUint8>(*scaleFactor - offset[subband]) ) > 0)
					{
					(*bitneed) >>= 1;
					}
				scaleFactor++;
				bitneed++;
				} // for subband
			} // for channel
		}
	}

/**
This function gets the maximum bitneed of both channels subbands for Stereo and Joint Stereo.
@internalComponent
@param aBitneed
Array of bitneed.
*/
TInt8 CSBCFrameEncoder::MaxBitneedCombined(const TInt8 aBitneed[][KSbcMaxSubbands])
	{
	// see A2DP spec for reference
	TInt8 maxBitneed = 0;
	const TUint8 numSubbands = iParameters.Subbands();
	
	for (TInt8 channel = 0; channel < 2; channel++)
		{
		const TInt8* bitneed = aBitneed[channel];
		for (TInt8 subband = 0; subband < numSubbands; subband++)
			{
			if (*bitneed > maxBitneed)
				{
				maxBitneed = *bitneed;
				}
			bitneed++;
			}
		}
	return maxBitneed;
	}
	
/**
This function calculates how many bitslices fit into the bitpool for both channels 
for Stereo and Joint Stereo.
@internalComponent
@param aBitneed
Array of bitneed.
@param aBitCount
The bit count, counts how many bits used
@return the number of bitslices
*/
TInt8 CSBCFrameEncoder::CalcBitSlicesCombined(const TInt8 aBitneed[][KSbcMaxSubbands], TInt& aBitCount)
	{
	// see A2DP spec for reference
	aBitCount = 0;
	TInt8 sliceCount = 0;
	TInt8 bitSlices = static_cast<TUint8>(MaxBitneedCombined(aBitneed) + 1);
	
	const TUint8 numSubbands = iParameters.Subbands();
	const TUint8 bitpool = iParameters.Bitpool();
	
	do {
		bitSlices--;
		aBitCount += sliceCount;
		sliceCount = 0;
		
		for (TInt8 channel = 0; channel < 2; channel++)
			{
			const TInt8* bitneed = aBitneed[channel];
			for (TInt8 subband = 0; subband < numSubbands; subband++)
				{
				if (*bitneed > bitSlices + 1 && *bitneed < bitSlices + 16) 
					{
					sliceCount++;
					}
				else if (*bitneed == bitSlices + 1)
					{
					sliceCount += 2;
					}
				bitneed++;
				}
			}
		} while (aBitCount + sliceCount < bitpool);
	
	if (aBitCount + sliceCount == bitpool)
		{
		aBitCount += sliceCount;
		bitSlices--;
		}
		
	return bitSlices;
	}
	
/**
This function distributes number of bits to each subband for all samples 
for Stereo and Joint Stereo.
@internalComponent
@param aBitneed
Array of bitneed.
*/
void CSBCFrameEncoder::DistributeBitsCombined(const TInt8 aBitneed[][KSbcMaxSubbands])
	{
	// see A2DP spec for reference
	TInt bitCount = 0;
	TInt bitSlices = CalcBitSlicesCombined(aBitneed, bitCount);

	const TUint8 numSubbands = iParameters.Subbands();
	const TUint8 bitpool = iParameters.Bitpool();
	
	// distribute bits until the last bitslice is reached
	TInt8 channel = 0;
	TInt8 subband = 0;
	for (; channel < 2; channel++)
		{
		const TInt8* bitneed = aBitneed[channel];
		TUint8* bits = iBits[channel];
		for (subband = 0; subband < numSubbands; subband++)
			{
			if (*bitneed < bitSlices + 2)
				{
				*bits = 0;
				}
			else
				{
				*bits = static_cast<TUint8>(Min(*bitneed - bitSlices, 16) );
				}
			bitneed++;
			bits++;
			}
		}
	
	// distribute the remaining bits
	channel = 0;
	subband = 0;
	while (bitCount < bitpool && subband < numSubbands)
		{
		TUint8& bits = iBits[channel][subband];
		if (bits >= 2 && bits < 16)
			{
			bits++;
			bitCount++;
			}
		else if (aBitneed[channel][subband] == bitSlices + 1 && bitpool > bitCount + 1) 
			{
			bits += 2; // ? bits[ch][sb] = 2 in A2DP spec, a bug in the spec?
			bitCount += 2;
			}
						
		if (channel == 1)
			{
			channel = 0;
			subband++;
			}
		else
			{
			channel = 1;
			}
		}

	channel = 0;	
	subband = 0;
	while (bitCount < bitpool && subband < numSubbands)
		{
		TUint8& bits = iBits[channel][subband];
		if (bits < 16) 
			{
			bits++; 
			bitCount++;
			}
		
		if (channel == 1)
			{
			channel = 0 ;
			subband++;
			}
		else
			{
			channel = 1;
			}
		}
	}

/**
This function calculates the CRC code for sbc frame.
@internalComponent
@return sbc CRC value
*/
TUint8 CSBCFrameEncoder::CalcCRC()
	{
	CSbcCRCCalculator crc;
	
	crc.InputByte(iParameters.Parameters() ); // 5 parameters
	crc.InputByte(iParameters.Bitpool() ); // bitpool

	// join[] & RFA bits
	const TUint8 numSubbands = iParameters.Subbands();
	if (iParameters.ChannelMode() == TSBCFrameParameters::EJointStereo)
		{
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			crc.InputBit(iJoin[subband]);
			}
		}
		
	// scale factors
	const TUint8 numChannels = iParameters.Channels();
	for (TUint8 channel = 0; channel < numChannels; channel++)
		{
		const TUint8* scaleFactors = iScaleFactors[channel];
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			crc.InputBits(4, *scaleFactors++);
			}
		}
		
	return crc.ShiftRegister();
	}
	
/**
This function outputs the encoded sbc frame into the destination buffer.
@internalComponent
@param aFrame
The destination buffer
@leave if out of memory
*/
void CSBCFrameEncoder::WriteFrameL(TDes8& aFrame)
	{
	CBitStreamParser* parser = CBitStreamParser::NewLC(aFrame);
	
	WriteHeader(*parser);
	WriteScaleFactors(*parser);
	WriteData(*parser);
	WritePaddingL(*parser);
	
	CleanupStack::PopAndDestroy(parser);
	}

/**
This function writes the sbc frame header into the destination buffer.
@internalComponent
@param aParser
The bit stream parser which manipulates the destination buffer bit stream
*/
void CSBCFrameEncoder::WriteHeader(CBitStreamParser& aParser)
	{
	// syncword
	aParser.WriteByte(KSBCFrameSyncWord);
	// sampling frequency, blocklength, channel mode, allocatin method, subbands
	aParser.WriteByte(iParameters.Parameters() );
	// bitpool
	aParser.WriteByte(iParameters.Bitpool() );
	// crc check
	aParser.WriteByte(CalcCRC() );

	if (iParameters.ChannelMode() == TSBCFrameParameters::EJointStereo)
		{
		// join[] & RFA
		const TUint8 numSubbands = iParameters.Subbands();
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			aParser.WriteBits(1, iJoin[subband]);
			}
		}
	}
	
/**
This function writes the sbc frame scale factors into the destination buffer.
@internalComponent
@param aParser
The bit stream parser which manipulates the destination buffer bit stream
*/
void CSBCFrameEncoder::WriteScaleFactors(CBitStreamParser& aParser)
	{
	const TUint8 numChannels = iParameters.Channels();
	const TUint8 numSubbands = iParameters.Subbands();
	
	for (TUint8 channel = 0; channel < numChannels; channel++)
		{
		const TUint8* scaleFactors = iScaleFactors[channel];
		for (TUint8 subband = 0; subband < numSubbands; subband++)
			{
			aParser.WriteBits(4, *scaleFactors++);
			}
		}
	}

/**
This function writes one sbc subband sample into the destination buffer.
@internalComponent
@param aParser
The bit stream parser which manipulates the destination buffer bit stream
@param aBits
The number of bits to write
@param aSample
The sample value to write
*/
static void WriteOneSample(CBitStreamParser& aParser, TUint8 aBits, TInt32 aSample)
	{
	if (aBits >= 8)
		{
		aBits -= 8;
		aParser.WriteByte(static_cast<TUint8>( (aSample >> aBits) & 0xff) );
		}
	if (aBits > 0)
		{
		aParser.WriteBits(aBits, static_cast<TUint8>(aSample & 0xff) );
		}
	}
	
/**
This function writes the sbc frame data into the destination buffer.
@internalComponent
@param aParser
The bit stream parser which manipulates the destination buffer bit stream
*/
void CSBCFrameEncoder::WriteData(CBitStreamParser& aParser)
	{
	const TUint8 numBlocks = iParameters.BlockLength();
	const TUint8 numChannels = iParameters.Channels();
	const TUint8 numSubbands = iParameters.Subbands();
	
	for (TUint8 block = 0; block < numBlocks; block++)
		{
		for (TUint8 channel = 0; channel < numChannels; channel++)
			{
			const TUint8* bits = iBits[channel];
			const TInt32* samples = iOutputSamples[block][channel];
			
			for (TUint8 subband = 0; subband < numSubbands; subband++)
				{
				if (*bits > 0)
					{
					WriteOneSample(aParser, *bits, *samples);
					}
				bits++;
				samples++;
				}
			}
		}
	}
	
/**
This function writes the sbc frame padding bits into the destination buffer.
@internalComponent
@param aParser
The bit stream parser which manipulates the destination buffer bit stream
@panic if output frame length is not the same as expected
*/
void CSBCFrameEncoder::WritePaddingL(CBitStreamParser& aParser)
	{
	TUint byteOffset;
	TUint8 bitOffset;
	
	aParser.Position(byteOffset, bitOffset);
	
	if (bitOffset != 0)
		{
		aParser.WriteBits(8 - bitOffset, 0);
		byteOffset++;
		}
		
	if (byteOffset != iParameters.CalcFrameLength() )
		{
		User::Leave(KErrCorrupt);
		}
	}
