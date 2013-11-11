// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SBCFRAMEPARAMETERS_H__
#define __SBCFRAMEPARAMETERS_H__

/**
This class contains 6 SBC frame parameters: sampling frequency, block length, 
channel mode, allocation mode, subbands and bitpool. It can validate all the 
parameters, calculate frame length and bit rate.
@internalComponent
*/
class TSBCFrameParameters
	{
public:
	/**
	This enum list all the possible sampling frequency settings
	*/
	enum TSamplingFrequency
		{
		/**
		sampling frequency is 16000 Hz
		*/
		E16000Hz = 0,
		/**
		sampling frequency is 32000 Hz
		*/
		E32000Hz,
		/**
		sampling frequency is 44100 Hz
		*/
		E44100Hz,
		/**
		sampling frequency is 48000 Hz
		*/
		E48000Hz
		};

	/**
	This enum list all the possible block length settings
	*/
	enum TBlockLength
		{
		/**
		block length is 4, one frame contains 4 blocks of audio samples
		*/
		E4Blocks = 0,
		/**
		block length is 8, one frame contains 8 blocks of audio samples
		*/
		E8Blocks,
		/**
		block length is 12, one frame contains 12 blocks of audio samples
		*/
		E12Blocks,
		/**
		block length is 16, one frame contains 16 blocks of audio samples
		*/
		E16Blocks
		};

	/**
	This enum list all the possible channel mode settings
	*/
	enum TChannelMode
		{
		/**
		channel mode is Mono, in this mode,
		only one channel contains audio samples.
		*/
		EMono = 0,
		/**
		channel mode is Dual Channel, in this mode,
		it contains two seperate mono audio samples.
		*/
		EDualChannel,
		/**
		channel mode is Stereo, in this mode,
		it contains stereo audio samples.
		*/
		EStereo,
		/**
		channel mode is Joint Stereo, in this mode,
		the left channel stores half of sum of both channels, 
		the right channel stores half of difference of both channels. 
		*/
		EJointStereo
		};

	/**
	This enum list all the possible allocation method settings
	*/
	enum TAllocationMethod
		{
		/**
		allocation method is Loudness, in this mode, 
		the bit allocation calculation uses Table offset4 or offset8 as well as scale factors
		*/
		ELoudness = 0,
		/**
		allocation method is SNR, in this mode,
		bit allocation only uses scale factors
		*/
		ESNR
		};

	/**
	This enum list all the possible subbands settings
	*/
	enum TSubbands
		{
		/**
		subbands is 4, each channel contains 4 subbands in each block, 
		each subband contains one sample
		*/
		E4Subbands = 0,
		/**
		subbands is 8, each channel contains 8 subbands in each block, 
		each subband contains one sample
		*/
		E8Subbands
		};
		
public:
	inline TSBCFrameParameters();
	
	inline void Reset();

	inline TUint8 	SamplingFrequencyEnum() const;
	inline TUint	SamplingFrequencyHz() const;
	inline void		SetSamplingFrequency(TSamplingFrequency aSamplingFrequency);
	
	inline TUint8	BlockLength() const;
	inline void		SetBlockLength(TBlockLength aBlockLength);
	
	inline TUint8 	ChannelMode() const;
	inline TUint8	Channels() const;
	inline void 	SetChannelMode(TChannelMode aChannelMode);
	
	inline TUint8 	AllocationMethod() const;
	inline void 	SetAllocationMethod(TAllocationMethod aAllocationMethod);
	
	inline TUint8	SubbandsEnum() const;
	inline TUint8	Subbands() const;
	inline void 	SetSubbands(TSubbands aSubbands);
	
	inline TUint8	Bitpool() const;
	inline void		SetBitpool(TUint8 aBitpool);
	
	inline TUint8	Parameters() const;
	inline TInt 	Validate() const;

	inline TUint 	CalcFrameLength() const;
	inline TUint 	CalcBitRate(TUint aFrameLength) const;

private:
	TUint8 	iParameters;
	TUint8 	iBitpool;
	};

/**
The minimum SBC bitpool value is 2 
*/	
const TUint8 KSBCMinBitpoolValue = 2;
/**
The maximum SBC bitpool value is 250 
*/	
const TUint8 KSBCMaxBitpoolValue = 250;

/**
The sampling frequency bits mask is 0b11, 2 bits
*/	
const TUint8 KSBCSampFreqBitsMask  = 0x3;
/**
The bit offset of sampling frequency field in TSBCFrameParameters::iParameters is 6
*/
const TUint8 KSBCSampFreqBitOffset = 6;

/**
The block length bits mask is 0b11, 2 bits
*/	
const TUint8 KSBCBlckLengBitsMask  = 0x3;
/**
The bit offset of block length field in TSBCFrameParameters::iParameters is 4
*/
const TUint8 KSBCBlckLengBitOffset = 4;

/**
The block length bits mask is 0b11, 2 bits
*/	
const TUint8 KSBCChnlModeBitsMask  = 0x3;
/**
The bit offset of block length field in TSBCFrameParameters::iParameters is 2
*/
const TUint8 KSBCChnlModeBitOffset = 2;

/**
The block length bits mask is 0b01, 1 bit
*/	
const TUint8 KSBCAllcMthdBitsMask  = 0x1;
/**
The bit offset of block length field in TSBCFrameParameters::iParameters is 1
*/
const TUint8 KSBCAllcMthdBitOffset = 1;

/**
The block length bits mask is 0b01, 1 bit
*/	
const TUint8 KSBCSubbandsBitsMask  = 0x1;
/**
The bit offset of block length field in TSBCFrameParameters::iParameters is 0
*/
const TUint8 KSBCSubbandsBitOffset = 0;

/**
Constructor
@internalComponent
*/
inline TSBCFrameParameters::TSBCFrameParameters() : iParameters(0), iBitpool(0)
	{
	}
	
/**
This function reset all the parameters
@internalComponent
*/
inline void TSBCFrameParameters::Reset()
	{
	iParameters = 0;
	iBitpool = 0;
	}
	
/**
This function gets the sampling frequency enum value
@internalComponent
@return enum value of sampling frequency
*/
inline TUint8 TSBCFrameParameters::SamplingFrequencyEnum() const
	{
	return static_cast<TUint8>( (iParameters >> KSBCSampFreqBitOffset) & KSBCSampFreqBitsMask);
	}
	
/**
This function gets the sampling frequency value in Hz
@internalComponent
@return samplinng frequency value in Hz
*/
inline TUint TSBCFrameParameters::SamplingFrequencyHz() const
	{
	switch (SamplingFrequencyEnum() )
		{
		case E16000Hz:
			return 16000;
			
		case E32000Hz:
			return 32000;
			
		case E44100Hz:
			return 44100;
			
		case E48000Hz:
			return 48000;
		}
	return 0;
	}

/**
This function sets the sampling frequency value
@internalComponent
@param aSampFreq
New sampling frequency enum value to set
*/
inline void TSBCFrameParameters::SetSamplingFrequency(TSamplingFrequency aSampFreq)
	{
	// clear sampling frequency bits
	iParameters &= ~(KSBCSampFreqBitsMask << KSBCSampFreqBitOffset);
	// set new sampling frequency bits
	iParameters |= ( (aSampFreq & KSBCSampFreqBitsMask) << KSBCSampFreqBitOffset);
	}

/**
This function gets the block length value
@internalComponent
@return number of blocks in one frame
*/
inline TUint8 TSBCFrameParameters::BlockLength() const
	{
	switch ( (iParameters >> KSBCBlckLengBitOffset) & KSBCBlckLengBitsMask)
		{
		case E4Blocks:
			return 4;
			
		case E8Blocks:
			return 8;
			
		case E12Blocks:
			return 12;
			
		case E16Blocks:
			return 16;
		}
	return 0;
	}

/**
This function sets the block length value
@internalComponent
@param aBlockLen
New block length value to set
*/
inline void TSBCFrameParameters::SetBlockLength(TBlockLength aBlockLen)
	{
	// clear block length bits
	iParameters &= ~(KSBCBlckLengBitsMask << KSBCBlckLengBitOffset);
	// set new block length bits
	iParameters |= ( (aBlockLen & KSBCBlckLengBitsMask) << KSBCBlckLengBitOffset);
	}

/**
This function gets the channel mode enum value
@internalComponent
@return channel mode enum value
*/
inline TUint8 TSBCFrameParameters::ChannelMode() const
	{
	return static_cast<TUint8>( (iParameters >> KSBCChnlModeBitOffset) & KSBCChnlModeBitsMask);
	}

/**
This function gets number of channels
@internalComponent
@return number of channels
*/
inline TUint8 TSBCFrameParameters::Channels() const
	{
	switch (ChannelMode() )
		{
		case EMono:
			return 1;
			
		case EDualChannel:
		case EStereo:
		case EJointStereo:
			return 2;
		}
	return 0;
	}
	
/**
This function sets the channel mode enum value
@internalComponent
@param aChnlMode
New channel mode enum value to set
*/
inline void TSBCFrameParameters::SetChannelMode(TChannelMode aChnlMode)
	{
	// clear channel mode bits
	iParameters &= ~(KSBCChnlModeBitsMask << KSBCChnlModeBitOffset);
	// set new channel mode bits
	iParameters |= ( (aChnlMode & KSBCChnlModeBitsMask) << KSBCChnlModeBitOffset);
	}

/**
This function gets the allocation method enum value
@internalComponent
@return allocation method enum value
*/
inline TUint8 TSBCFrameParameters::AllocationMethod() const
	{
	return static_cast<TUint8>( (iParameters >> KSBCAllcMthdBitOffset) & KSBCAllcMthdBitsMask);
	}

/**
This function sets the channel mode enum value
@internalComponent
@param aAllocMethod
New channel mode enum value to set
*/
inline void TSBCFrameParameters::SetAllocationMethod(TAllocationMethod aAllocMethod)
	{
	// clear allocation method bits
	iParameters &= ~(KSBCAllcMthdBitsMask << KSBCAllcMthdBitOffset);
	// set new allocation method bits
	iParameters |= ( (aAllocMethod & KSBCAllcMthdBitsMask) << KSBCAllcMthdBitOffset);
	}		

/**
This function gets the subbands enum value
@internalComponent
@return subbands enum value
*/
inline TUint8 TSBCFrameParameters::SubbandsEnum() const
	{
	return static_cast<TUint8>( (iParameters >> KSBCSubbandsBitOffset) & KSBCSubbandsBitsMask);
	}

/**
This function gets the subbands value
@internalComponent
@return subbands value, i.e 4, 8
*/
inline TUint8 TSBCFrameParameters::Subbands() const
	{
	switch (SubbandsEnum() )
		{
		case E4Subbands:
			return 4;
			
		case E8Subbands:
			return 8;
		}
	return 0;
	}
	
/**
This function sets the subbands enum value
@internalComponent
@param aSubbands
New subbands enum value to set
*/
inline void TSBCFrameParameters::SetSubbands(TSubbands aSubbands)
	{
	// clear subbands bits
	iParameters &= ~(KSBCSubbandsBitsMask << KSBCSubbandsBitOffset);
	// set new subbands bits
	iParameters |= ( (aSubbands & KSBCSubbandsBitsMask) << KSBCSubbandsBitOffset);
	}		

/**
This function gets the bitpool value
@internalComponent
@return bitpool value
*/
inline TUint8 TSBCFrameParameters::Bitpool() const
	{
	return iBitpool;
	}

/**
This function sets the bitpool enum value
@internalComponent
@param aSubbands
New bitpool enum value to set
*/
inline void TSBCFrameParameters::SetBitpool(TUint8 aBitpool)
	{
	iBitpool = aBitpool;
	}

/**
This function gets the 5 parameters (except bitpool) byte value
@internalComponent
@return 5 parameters byte value
*/
inline TUint8 TSBCFrameParameters::Parameters() const
	{
	return iParameters;
	}
	
/**
This function checks the bitpool value according to:
1. bitpool >= 2 and bitpool <= 250
2. bitpool <= 16 * subbands for Mono and Dual Channel,
   bitpool <= 32 * subbands for Stereo and Joint Stereo 
3. results in bit_rate <= 320 kbps for Mono
   results in bit_rate <= 512 kpbs for two-channel modes
@internalComponent
@return -1 if invalid; 0 if valid
*/
inline TInt TSBCFrameParameters::Validate() const
	{
	if (iBitpool < KSBCMinBitpoolValue || iBitpool > KSBCMaxBitpoolValue)
		{
		return -1;
		}
		
	const TUint16 numSubbands = Subbands(); // use 16 bits to avoid overflow
	const TUint8 channelMode = ChannelMode();
	
	if (channelMode == EMono || channelMode == EDualChannel)
		{
		// bitpool <= 16 * subbands, for Mono and Dual_Channel modes
		if (iBitpool > (numSubbands << 4) )
			{
			return -1;
			}
		}
	else
		{
		// bitpool <= 32 * subbands, for Stereo and Joint_Stereo modes
		if (iBitpool > (numSubbands << 5) )
			{
			return -1;
			}
		}
	
	if (channelMode == EMono)
		{
		// bit rate <= 320kbps for Mono mode
		if (CalcBitRate(CalcFrameLength() ) > 320)
			{
			return -1;
			}
		}
	else
		{
		// bit rate <= 512kpbs for two-channels modes
		if (CalcBitRate(CalcFrameLength() ) > 512)
			{
			return -1;
			}
		}
	
	return 0;
	}

/**
This function calculates the frame length value according to:
1. for MONO or DUAL_CHANNEL
   frame_len = 4 + (4 * subbands * channels) / 8 + ceil(blocks * channels * bitpool / 8)
2. for STEREO
   frame_len = 4 + (4 * subbands * channels) / 8 + ceil(blocks * bitpool / 8)
3. for JOINT_STEREO
   frame_len = 4 + (4 * subbands * channels) / 8 + ceil((subbands + blocks * bitpool) / 8)
ceil(), taking the upper integer value
@internalComponent
@return frame length value
*/
inline TUint TSBCFrameParameters::CalcFrameLength() const
	{
	TUint temp = 0;
	switch (ChannelMode() )
		{
		case EMono:
			temp = BlockLength() * Bitpool(); // blocks * bitpool
			break;
			
		case EDualChannel:
			temp = (BlockLength() * Bitpool() ) << 1; // blocks * bitpool * 2
			break;
			
		case EStereo:
			temp = BlockLength() * Bitpool(); // blocks * bitpool
			break;
			
		case EJointStereo:
			temp = Subbands() + BlockLength() * Bitpool(); // subbands + blocks * bitpool
			break;
			
		default:
			User::Panic(_L("Invalid channel mode"), KErrNotSupported);
			break;
		}
		
	TUint frameLen = 4 + ( (Subbands() * Channels() ) >> 1) + (temp >> 3);
	if (temp & 0x7)
		{
		frameLen++;
		}
		
	return frameLen;
	}

/**
This function calculates the bit rate value according to:
	bit_rate = 8 * frame_len * sampling_freq / subbands / blocks
@internalComponent
@return bit rate value in kHz
*/
inline TUint TSBCFrameParameters::CalcBitRate(TUint aFrameLen) const
	{
	return (aFrameLen << 3) * SamplingFrequencyHz() / (Subbands() * BlockLength() * 1000);
	}

#endif // __SBCFRAMEPARAMETERS_H__

