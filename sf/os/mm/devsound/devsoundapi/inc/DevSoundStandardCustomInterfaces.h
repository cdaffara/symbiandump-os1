// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
 @file
*/

#ifndef __DEVSOUNDSTANDARDCUSTOMINTERFACES_H__
#define __DEVSOUNDSTANDARDCUSTOMINTERFACES_H__

#include <e32std.h>

/**
UID associated with the custom interface MMMFDevSoundCustomInterfaceBitRate.
*/
const TUid KUidCustomInterfaceDevSoundBitRate = {0x101F7DD5};



/**
This class provides an interface to set and retrive the DevSound bit rate.
*/
class MMMFDevSoundCustomInterfaceBitRate
	{	
public:
	
	/**
	Gets the bit rates that are supported by DevSound in its current configuration.

	@param  aSupportedBitRates
	The supported bit rates, in bits per second, shall be appended to this array. Note that 
	the array shall be reset by this method.
	*/
	virtual void GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates) = 0;

	/**
	Returns	the current bit rate.

	@return	The current bit rate, in bits per second.
	*/
	virtual TInt BitRateL() = 0;

	/**
	Sets the bit rate to a new value.

	@param  aBitRate
	The new bit rate, in bits per second.
	*/
	virtual void SetBitRateL(TInt aBitRate) = 0;
	};


/*****************************************************************************/
/**
UID associated with the Custom interface MMMFDevSoundCustomInterfaceFileBlockLength.
*/
const TUid KUidCustomInterfaceDevSoundFileBlockLength = {0x10273806};

/**
Custom interface class for setting the file's block length on the hwdevice. 

Note also that this interface is just a simple interface to 
set file's block length. No checking is perfomed on the values sent (hence the
Set methods do not return an error code).
*/
class MMMFDevSoundCustomInterfaceFileBlockLength
	{
public:
	/**
	Sets the file's block length on the hwdevice
	@param aBlockAlign
	       The file's block length 
	*/
	virtual void SetFileBlockLength(TUint aBlockAlign)=0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFGsmConfig
*/
const TUid KUidGsmConfig = {0x102825FC};

/**
This class provides an interface to set and retrive the GSM conversion format.
*/
class MMMFGsmConfig
	{
public: 
	/** The GSM conversion format */
	enum TMMFGsmConversionFormat
		{
		/** no conversion specified */
		ENone, 
		/** for use in Full rate (06.10) and half rate (GSM 06.20 Half Rate (HR)) Vocoder */
		EALaw8bit, 
		/** for use in Full rate (06.10) and half rate (GSM 06.20 Half Rate (HR)) Vocoder */
		EULaw8bit,
		/** for use in GSM 06.60 Enhanced Full Rate (EFR) Vocoder.*/
		EAlawPCM
		};
		
	/**
	Set the conversion format using the TMMFGsmConversionFormat enum.
	This conversion must be set before the coder/decoder is started.

	@param aConvFormat - format desired.  For encoding behaviour, format will 
	be the input format and for decoding behaviour format will be the output 
	format.

	@return KErrNone - Format change supported, KErrNotSupported - Format 
	change not supported, KErrInUse - Format change attempted whilst decoding.
	*/	
	virtual TInt SetConversionFormat(TMMFGsmConversionFormat aConvFormat) = 0;

	/** 
	Provides the current conversion format.  Can be called at any time.

	@param aConvFormat - after the call contains the current format.
	For encoding behaviour, format will be the input format and for decoding
	behaviour format will be the output format.

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using
			SetConversionFormat() at least once for the current instance of the
			interface and there is no default value.
			KErrGeneral for all other error scenario.
	*/
	virtual TInt ConversionFormat(TMMFGsmConversionFormat& aConvFormat) const = 0;
	};


/*****************************************************************************/
/**
UID associated with the Custom interface MSpeechEncoderConfig
*/
const TUid KUidSpeechEncoderConfig = {0x102825FB};

/**
This class provides an interface to those CMMFHwDevices providing speech encoding. 
This interface is used for querying and configuring the operational modes of speech 
encoders as supported by those specific encoders. 
If a speech encoder does not support a mode provided by the interface, it will return KErrNotSupported.
*/
class MSpeechEncoderConfig 
	{
public:
	/**
	Controls voice activity detection (VAD) mode.
	This method can be called at all times - while actively encoding or not.
	It should be noted that a VAD mode change could also instigate a change in the 
	mode of encoding (fixed rate encoding versus variable rate) e.g. in cases where 
	VAD requires variable rate encoding.
	The effects of activating VAD mode behaviour should be documented in the 
	CMMFHwDevice specification implementing it.

	@param aVadModeOn ETrue=On, EFalse=Off
	@return KErrNone if successful. KErrNotSupported if this method is not implemented 
	or not supported by the encoder. 
	*/
	virtual TInt SetVadMode(TBool aVadModeOn) = 0;

	/**
	Gets the current state of the voice activity detection (VAD) mode.
	This method can be called at all times - while actively encoding or not.

	@param aVadModeOn On output ETrue = On, EFalse = Off
	@return KErrNone if successful, KErrNotSupported if this method is not implemented 
	or not supported by the encoder. 
	*/
	virtual TInt GetVadMode(TBool& aVadModeOn) = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MAacDecoderConfig
*/
const TUid KUidAacDecoderConfig = {0x102825FD};

/**
This class provides an interface to the AAC decoder CMMFHwDevice in order 
to provide additional configuration information for decoding raw AAC data blocks. 

The sample rate can be configured via the CMMFHwDevice::SetConfig() method. 
An extension mechanism will be provided for further functionality as required.
The CMMFHwDevice does not contain Output channel configuration at time of writing. 
The HWA decoders to date have been hardcoded for 2ch (left / right) output. 
Additional configuration information may be provided by using extension mechanisms. 
*/
class MAacDecoderConfig 
	{
public: 

	/**
	This is a class containing an enumerated type that defines the audio object types 
	required for configuring the AAC decoder for decoding raw AACDefined within TAudioConfig.
	*/
	class TAudioConfig
		{
	public:
		/**  The audio object type */
	    enum TAudioObjectType
	    	{
			/** Null */
	       	ENull    = 0,
			/** AacMain */
	       	EAacMain = 1,
			/** AacLc */
	       	EAacLc   = 2,
			/** AacSsr */
	       	EAacSsr  = 3,
			/** AacLtp */
	       	EAacLtp  = 4,
			/** Sbr */
	       	ESbr     = 5
	       	};
		TAudioObjectType iAudioObjectType;
		};	

public:
	/**
	Sets additional configuration parameters required for decoding raw AAC. 

	This method can be called when encoding is not active - anytime before the CMMFHwDevice is started. 
	
	For consistent configuration, this method should be called sometime before each 
	start is called on the CMMFHwDevice when decoding raw AAC. That is to say the 
	CMMFHwDevice is not required to retain prior configuration information, detect raw 
	AAC sent to it, and reconfigure the decoder with this saved information after stop 
	is called on the CMMFHwDevice. However, calling pause on the CMMFHwDevice implies 
	that the next start call will be resuming same content. For this condition, this API 
	is not required when restarting the CMMFHwDevice.
	
	@param aAudioConfig The structure containing the additional information required for decoding the raw AAC.

	@return KErrNone if successful or KErrInUse if this method is used when decoding is active. 
	*/
	virtual TInt SetAudioConfig(TAudioConfig& aAudioConfig) = 0;

	/**
	Gets additional configuration parameters supported for decoding raw AAC. 
	
	This method is allowable when encoding is not active - anytime before the CMMFHwDevice is started. 
		
	@param aSupportedAudioConfigs An array of structures containing the additional supported configurations.

	@return KErrNone if successful. 
			KErrInUse if this method is used when decoding is active. 
	*/
	virtual TInt GetSupportedAudioConfigs(RArray<TAudioConfig>& aSupportedAudioConfigs) = 0;
	};
	
/*****************************************************************************/
/**
UID associated with the Custom interface MEAacPlusDecoderIntfc
*/
const TUid KUidEAacPlusDecoderIntfc = {0x102825FF};

/**
This class provides an interface to the eAAC+ decoder hwdevice in order to provide configuration information.

The Get.. methods can be used to retrieve the last successfully applied configuration parameters.  
A message is not sent to adaptation or the decoder. Instead, a locally saved set of the 
parameters will be updated upon successful use of ApplyConfig(). It is these saved values that 
the proxy maintains that will be returned to the user. For instance, if two interfaces are used 
and one interface is used to modify the settings, the other interface's Get… methods would not 
return those settings.  The Get.. methods will return an error if a successful use of ApplyConfig() 
has not been performed.

The Set.. methods are used to update a configuration structure within the implementation of the 
interface. When the interface is instantiated, the state of SBR and Downsampled modes will be 
disabled. Therefore it is not necessary to configure these parameters before using 
ApplyConfig() the first time. However, once modified using this instance of the interface, they 
remain in that state until Set.. again.  The values of these items are updated in an internal 
structure when each Set.. is called. 

The ApplyConfig() method is used to configure the encoder with the parameters specified by 
the Set.. methods. ApplyConfig will send back an error (KErrUnknown) if not all items are 
Set at least once. This check and the parameter values returned will be implemented in the interface proxy 
and will not query the actual decoder. The exception to this is for configuration parameters with defined 
default values (see next paragraph). 

Calling ApplyConfig() will send these values using a single message to the message handler.
ApplyConfig() should fail if it is used during decoding with a KErrInUse (or, as previously stated, 
KErrUnknown if a value without a default is not set). Since the interface proxy is not aware of decoding state, 
this check would have to be done in the adaptation implementation for this interface.
*/
class MEAacPlusDecoderIntfc 
	{
public:
	/**
	Defines additional configuration elements required to decode eAAC+.
	*/
	enum TAudioObjectType
        {
        /** Null */
        ENull    = 0,
        /** AacLc */
        EAacLc   = 2,
        /** AacLtp */
        EAacLtp  = 4
        };

	/**
	Sets the value of the sampling frequency of the decoder to be configured by the ApplyConfig() 
	method. This is the sample rate of the core decoder, not the output sampling frequency of the decoder.
	This method can be called at all times but if ApplyConfig() is subsequently called 
	after changing this value whilst decoding, a KErrInUse error will result.

	@param aInputSamplingFrequency  value of the sampling frequency. 
	*/
	virtual void SetInputSamplingFrequency(TUint aInputSamplingFrequency) = 0;

	/**
	Sets the object type of the core decoder to be configured by the ApplyConfig() method.

	This method can be called at all times but if ApplyConfig() is subsequently called after 
	changing this value whilst decoding, a KErrInUse error will result.

	@param  aAudioObjectType TAudioObjectType for core decoder as supported by the decoder.
	*/
	virtual void SetAudioObjectType(TAudioObjectType aAudioObjectType) = 0;
	
	/**
	Sets the number of channels for the output of the decoder to be configured by the ApplyConfig() 
	method. The use of this method is to ensure coordination of output channels rendered by the decoder 
	and channel configuration of the audio system. This method does not imply that stereo to mono downmix 
	is supported by the decoder. The main purpose is for the case considering the presence of PS. A user 
	may not know if the decoder output will be mono or stereo based on header information alone, but may 
	configure the decoder for two channel output to match the audio system configuration. If the user were 
	to set the output to 1 channel and stereo data is present, the decoder may choose only one channel to 
	output or to downmix the stereo to mono. Regarding this interface, it would be more desirable to configure 
	the decoder when it is known to be mono source data and rely on downmix from the audio system instead of 
	from the decoder. The decoder, however, would provide for duplicating mono data into 2 channel data when 
	configured for 2 channels and the source data contains only 1 channel.
	This method is allowable at all times - while actively encoding or not, but using ApplyConfig() 
	if changing this value while encoding will return an error.
		 
	@param aNumOfChannels 1 - output one channel audio  = 0; 2 - output two channel audio.
	 */
	virtual void SetNumOfChannels(TUint aNumOfChannels) = 0;
	
	/**
	Controls the SBR mode of the decoder to be configured by the ApplyConfig() method. 

	If SBR is enabled and no SBR data is present, the audio data will be upsampled, 
	unless Down Sampled mode is enabled.

	This method can be called at all times but if ApplyConfig() is subsequently called 
	after changing this value whilst decoding, a KErrInUse error will result.

	@param aSbrEnabled ETrue - SBR mode enabled, EFalse - SBR mode disabled 
	*/
	virtual void SetSbr(TBool aSbrEnabled) = 0;

	/**
	Controls the downsampled mode of the decoder to be configured by the ApplyConfig() method. This setting is only meaningful if SBR is enabled.

	This method can be called at all times but if ApplyConfig() is subsequently called after 
	changing this value whilst decoding, a KErrInUse error will result.
		 
	@param aDsmEnabled ETrue - downsampled mode enabled, EFalse - downsampled mode disabled.
	*/
	virtual void SetDownSampledMode(TBool aDsmEnabled) = 0;

	/**
	Used to configure the decoder with parameters passed in the Set methods.

	This method can be called all times but will return an KErrInUse if the configuration 
	parameters would change while decoding (as none of the set parameters can be changed 
	during decoding). This checking operational state of the decoder would be done by the 
	adaptation implementation of this interface since the interface proxy is not aware of 
	the decoder state. The adaptation implementation will return an error if incorrect values 
	are used to configure the decoder.

	@return KErrNone if successful, KErrNotSupported if this method is not implemented, 
			KErrInUse if encoding in active and values are changed that are not allowed to be 
			changed during encoding, KErrArgument if one of the configuration items is not appropriate or not set. 
	*/
	virtual TInt ApplyConfig() = 0;

	/**
	Returns the value of the sampling frequency of the decoder that was last successfully 
	configured by the ApplyConfig() method.
	
	This method can be called at all times but must be called after a value has been set via 
	the ApplyConfig

	@param aInputSamplingFrequency last setting successfully applied using ApplyConfig().

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() at 
			least once for the current instance of the interface or 
			KErrInUse if the encoder is active.
	*/
	virtual TInt GetInputSamplingFrequency(TUint& aInputSamplingFrequency) = 0;
	
	/**
	Returns the object type of the core decoder that was last successfully configured by the 
	ApplyConfig() method.
	
	This method can be called at all times. 
		 
	@param aAudioObjectType last setting successfully applied using ApplyConfig().

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface.
	*/
	virtual TInt GetAudioObjectType(TAudioObjectType& aAudioObjectType) = 0;

	/**
	Returns the number of channels that was last successfully configured by the 
	ApplyConfig() method.

	This method can be called at all times. It will return an error if used before 
	ApplyConfig() has been used successfully.

	@param aNumOfChannels last setting successfully applied using ApplyConfig().	 

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface.
	*/
	virtual TInt GetNumOfChannels(TUint& aNumOfChannels) = 0;

	/**
	Returns the SBR mode that was last successfully configured by the ApplyConfig() method.

	This method can be called at all times.  It will return an error if used before 
	ApplyConfig() has been used successfully.

	@param aSbrEnabled last setting successfully applied using ApplyConfig().

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface.	 
	*/
	virtual TInt GetSbr(TBool& aSbrEnabled) = 0;
	
	/**
	Returns the downsampled mode that was last successfully configured by the ApplyConfig() method.

	This method can be called at all times.  It will return an error if used before 
	ApplyConfig() has been used successfully.

	@param aDsmEnabled last setting successfully applied using ApplyConfig().	 

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() at least 
			once for the current instance of the interface.
	*/
	virtual TInt GetDownSampledMode(TBool& aDsmEnabled) = 0;
	};
	
/*****************************************************************************/
/**
UID associated with the Custom interface MSbcEncoderIntfc
*/
const TUid KUidSbcEncoderIntfc = {0x10282600};

/**
This class provides an interface to the SBC encoder hwdevice in order to provide configuration information.

The Get... supported methods can be used to check capabilities of the encoder by retrieving 
the last successfully applied configuration parameters. The purpose of the Get 
functions is to return the last successfully applied configuration of decoder by this 
interface. A message is not sent to adaptation or the decoder. Instead, a locally saved set 
of the parameters will be updated upon successful use of ApplyConfig(). The Get.. methods 
return an error (KErrUnknown ) if a successful use of ApplyConfig() has not been performed 
apart from the following methods, which return the range of properties supported by the 
implementation ( GetSupportedSamplingFrequencies, GetSupportedChannelModes, 
GetSupportedNumOfSubbands, GetSupportedAllocationMethods, GetSupportedNumOfBlocks, 
GetSupportedBitpoolRange).  These methods do not have setter functions as they are not 
configurable and hence will return valid data at all times.

ApplyConfig() will send these values using one message with these values in the internally 
defined structure. The message handler should define this structure in the SBC messages 
header file.   ApplyConfig() should fail (KErrInUse) if it is used during encoding and any 
values are changed other than bitpool value. Since the interface proxy is not aware of 
encoding state, this check would have to be done in the adaptation implementation for 
this interface.  The Bitpool can be updated during encoding to change the bitrate. This would 
require the use of the SetBitpoolSize() and ApplyConfig() methods.  Any other parameters can 
be changed but will result in a KErrInUse when ApplyConfig() is called.

The Set... methods are used update a client-side maintained configuration structure within the 
implementation of the interface.  When the structure is completed, ApplyConfig() should be called 
to send these client side settings to the actual hwdevice implementation.  There are no default 
values for the configuration parameters (hence all values must be set before ApplyConfig() is 
called or an error (KErrArgument) will result).

@note  This constraint is only true for configurations 
where this is supported (e.g. If GetSupportedChannelModes returns KErrNotSupported then SetChannelMode 
does not need to be set before ApplyConfig() is called). This check and the parameter values 
returned will be implemented in the interface proxy and will not query the actual encoder. If 
multiple instances of the interface are created, each would have this requirement.  
*/
class MSbcEncoderIntfc 
	{
public:
	/**
	Defines the channel modes for the SBC encoder
	*/
	enum TSbcChannelMode
        {
        /** SbcChannelMono */
    	ESbcChannelMono,
        /** SbcChannelDual */
		ESbcChannelDual,
        /** SbcChannelStereo */
   	 	ESbcChannelStereo,
        /** SbcChannelJointStereo */
    	ESbcChannelJointStereo
        };

	/**	
	This type defines the allocation methods for the SBC encoder.
	*/
	enum TSbcAllocationMethod
       	{
        /** SbcAllocationSNR */
    	ESbcAllocationSNR,
        /** SbcAllocationLoudness */
    	ESbcAllocationLoudness
        };

	/**
	Retrieves the sampling frequencies supported by the encoder. See the class comments for details.
	This method can be called at all times.
	
	@param aSamplingFrequencies Reference to the location to store the array of supported 
	sampling frequencies.
	 
	@return KErrNone if successful, KErrNotSupported if this method is not supported.
	*/
	virtual TInt GetSupportedSamplingFrequencies(RArray<TUint>& aSamplingFrequencies) = 0;
	
	/**
	Retrieves the channel encoding supported by the encoder. See the class comments for details.
	
	This method can be called at all times.

	@param aChannelModes Reference to the location to store the array of supported channel encoding modes.

	@return KErrNone if successful, KErrNotSupported if this method is not supported.
	*/
	virtual TInt GetSupportedChannelModes(RArray<TSbcChannelMode>& aChannelModes) = 0;
	
	/**
	Retrieves the number of subbands supported by the encoder. See the class comments for details.
	
	This method can be called at all times.

	@param aNumOfSubbands Reference to the location to store the array of supported number of subbands.

	@return KErrNone if successful, KErrNotSupported if this method is not supported.
	*/
	virtual TInt GetSupportedNumOfSubbands(RArray<TUint>& aNumOfSubbands) = 0; 

	/**
	Retrieves the allocation methods supported by the encoder. See the class comments for details.
	
	This method can be called at all times.

	@param aAllocationMethods Reference to the location to store the array of supported allocation methods.

	@return KErrNone if successful, KErrNotSupported if this method is not supported.
	*/
	virtual TInt GetSupportedAllocationMethods(RArray<TSbcAllocationMethod>& aAllocationMethods) = 0;
	
	/**
	Retrieves the number of blocks supported by the encoder. See the class comments for details.
	
	This method can be called at all times but if ApplyConfig() is subsequently called 
	after changing this value whilst encoding, a KErrInUse error will result.

	@param aNumOfBlocks Reference to the location to store the array of supported number of blocks.

	@return KErrNone if successful, KErrNotSupported if this method is not supported.
	*/
	virtual TInt GetSupportedNumOfBlocks(RArray<TUint>& aNumOfBlocks) = 0;
	
	/**
	Retrieves the min and max bitpool values supported by the encoder. See the class comments for details.
	
	This method can be called at all times but and ApplyConfig() can be called to change the value whilst encoding.

	@param aMinSupportedBitpoolSize minimum bitpool value allowed by the encoder.
	@param aMaxSupportedBitpoolSize maximum bitpool value allowed by the encoder.

	@return KErrNone if successful, KErrNotSupported if this method is not supported.
	*/
	virtual TInt GetSupportedBitpoolRange(TUint& aMinSupportedBitpoolSize, TUint& aMaxSupportedBitpoolSize) = 0;

	/**
	Sets the value of the sampling frequency of the encoder to be configured by the ApplyConfig() method.

	This method can be called at all times but if ApplyConfig() is subsequently called 
	after changing this value whilst encoding, a KErrInUse error will result.

	@param aSamplingFrequency values according to standard and supported by encoder.
 	*/
	virtual void SetSamplingFrequency(TUint aSamplingFrequency) = 0;

	/**
	Sets the channel mode of the encoder to be configured by the ApplyConfig() method. 
	See the class comments for details.
	
	This method can be called at all times but if ApplyConfig() is subsequently called after 
	changing this value whilst encoding, a KErrInUse error will result.

	@param aChannelMode (ESbcChannelMono, ESbcChannelDual, ESbcChannelStereo, 
			ESbcChannelJointStereo) when supported by encoder.	 

	*/
	virtual void SetChannelMode(TSbcChannelMode aChannelMode) = 0;

	/**
	Sets the channel mode of the encoder to be configured by the ApplyConfig() method.  

	This method can be called at all times but if ApplyConfig() is subsequently called after 
	changing this value whilst encoding, a KErrInUse error will result.

	@param aNumOfSubbands values according to standard and supported by encoder.	

	*/
	virtual void SetNumOfSubbands(TUint aNumOfSubbands) = 0;

	/**
	Sets the allocation method of the encoder to be configured by the ApplyConfig() method.

	This method can be called at all times but if ApplyConfig() is subsequently called after 
	changing this value whilst encoding, a KErrInUse error will result.
		 
	@param aAllocationMethod ESbcAllocationSNR, ESbcAllocationLoudness when supported by encoder

	*/
	virtual void SetAllocationMethod(TSbcAllocationMethod aAllocationMethod) = 0;

	/**
	Sets the number of blocks of the encoder to be configured by the ApplyConfig() method.

	This method can be called at all times but if ApplyConfig() is subsequently called after 
	changing this value whilst encoding, a KErrInUse error will result.
		
	@param aNumOfBlocks values according to standard and supported by encoder.	  	 
	*/
	virtual void SetNumOfBlocks(TUint aNumOfBlocks) = 0;
	
	/**
	Sets the size of the bitpool of the encoder to be configured by the ApplyConfig() method.  

	This method is allowable at all times - while actively encoding or not.

	@param aBitpoolSize values according to standard and supported by encoder.

	*/
	virtual void SetBitpoolSize(TUint aBitpoolSize) = 0;

	/**
	Used to configure the encoder with parameters passed in the Set… methods.
	This method can be called at all times - while actively encoding or not. 
	An error (KErrInUse) will be returned if any configuration parameter, other 
	than bitpool size, is changed while encoding. This checking operational 
	state of the encoder would be done by the adaptation implementation of this 
	interface since the interface proxy is not aware of the encoder state. Also, 
	the adaptation implementation will return an error if incorrect values are 
	used to configure the encoder.
	
	@note  Even though some preliminary checking is done in the set methods (to see 
	if it is valid to set the values at all) it is expected that 

	@return KErrNone if successful, 
			KErrNotSupported if this method is not implemented, 
			KErrInUse if encoding in active and values are changed that are not 
			allowed to be changed during encoding, 
			KErrArgument if one of the configuration items is not appropriate or 
			if all the values are not set (unless some of the values are not 
			supported, in which case these values should be ignored).	 
	*/
	virtual TInt ApplyConfig() = 0;

	/**
	Gets the value of the sampling frequency of the encoder that was last successfully 
	configured by the ApplyConfig() method.

	This method can be called at all times - while actively encoding or not, but will 
	return an error if used before ApplyConfig() has been used successfully.

	@param aSamplingFrequency last setting successfully applied using ApplyConfig().

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface, 
			KErrNotSupported if this method is not implemented (to match its set and 
			getsupported methods).
	*/
	virtual TInt GetSamplingFrequency(TUint& aSamplingFrequency) = 0;

	/**
	Gets the channel mode of the encoder that was last successfully configured by the 
	ApplyConfig() method.

	This method can be called at all times - while actively encoding or not, but will 
	return an error if used before ApplyConfig() has been used successfully.

	@param aChannelMode last setting successfully applied using ApplyConfig(). 

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface.
	*/
	virtual TInt GetChannelMode(TSbcChannelMode& aChannelMode) = 0;

	/**
	Gets the channel mode of the encoder that was last successfully configured by the 
	ApplyConfig() method.

	This method can be called at all times - while actively encoding or not, 
	but will return an error if used before ApplyConfig() has been used successfully.
		 
	@param aNumOfSubbands last setting successfully applied using ApplyConfig().
	
	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface, 
			KErrNotSupported if this method is not implemented (to match its set and 
			getsupported methods)
	*/
	virtual TInt GetNumOfSubbands(TUint& aNumOfSubbands) = 0;
	
	/**
	Gets the number of blocks of the encoder that was last successfully configured by 
	the ApplyConfig() method.

	This method can be called at all times - while actively encoding or not, but will 
	return an error if used before ApplyConfig() has been used successfully.

	@param aNumOfBlocks last setting successfully applied using ApplyConfig().

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface, 
			KErrNotSupported if this method is not implemented (to match its set and 
			getsupported methods)
	*/
	virtual TInt GetNumOfBlocks(TUint& aNumOfBlocks) = 0;

	/**
	Gets the allocation method of the encoder that was last successfully configured 
	by the ApplyConfig() method.

	This method can be called at all times - while actively encoding or not, but will 
	return an error if used before ApplyConfig() has been used successfully.
	@param aAllocationMethod last setting successfully applied using ApplyConfig().

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface, 
			KErrNotSupported if this method is not implemented (to match its set and 
			getsupported methods).
	*/
	virtual TInt GetAllocationMethod(TSbcAllocationMethod& aAllocationMethod) = 0;

	/**
	Gets the size of the bitpool of the encoder that was last successfully 
	configured by the ApplyConfig() method.

	This method can be called at all times - while actively encoding or not, but 
	will return an error if used before ApplyConfig() has been used successfully.

	@param aBitpoolSize last setting successfully applied using ApplyConfig().

	@return KErrNone if value returned successfully, 
			KErrUnknown if value has not been successfully configured using ApplyConfig() 
			at least once for the current instance of the interface, 
			KErrNotSupported if this method is not implemented (to match its set and 
			getsupported methods).
	*/
	virtual TInt GetBitpoolSize(TUint& aBitpoolSize) = 0;
	}; 

/*****************************************************************************/
/**
UID associated with the Custom interface MG711DecoderIntfc
*/
const TUid KUidG711DecoderIntfc = {0x10282601};

/**
This class provides an interface to the G711 decoder CMMFHwDevice in order to provide 
additional configuration information.
*/
class MG711DecoderIntfc 
	{
public:
	/** The Decoder mode */
	enum TDecodeMode
        {
        /** DecALaw */
        EDecALaw,
        /** DecULaw */
        EDecULaw
        };

	/**
	Informs the decoder of the encoded data format that will be sent to it.

	This method can be called when decoding is not active - anytime before started 
	or after stopped. 
	@param 	aDecodeMode the decode mode.

	@return KErrNone if successful. 
			KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt SetDecoderMode(TDecodeMode aDecodeMode) = 0;
	
	/**
	Queries the decoder of the encoded data format that has been active.

	This method can be called at all times.
	@param 	aDecodeMode the decode mode.

	@return KErrNone if successful. 
			KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt GetDecoderMode(TDecodeMode& aDecodeMode) = 0;

	/** 	
	Used to enable and disable the comfort noise generation capability.

	This method can be called when decoding is not active - anytime before started 
	or after stopped. 
	
	@param 	aCng ETrue = Comfort Noise Generation enabled; EFalse = Comfort Noise 
	Generation disabled
	
	@return	KErrNone if successful. 
			KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt SetComfortNoiseGeneration(TBool aCng) = 0;

	/** 	
	Used to find out if comfort noise generation is enabled or not.

	This method can be called when decoding is not active - anytime before started 
	or after stopped. 

	@param 	aCng ETrue = Comfort Noise Generation enabled; EFalse = Comfort Noise 
	Generation disabled

	@return	KErrNone if successful. 
			KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt GetComfortNoiseGeneration(TBool& aCng) = 0;

	/**
	Used to enable and disable the packet loss concealment capability.

	This can be called when decoding is not active - anytime before started or after 
	stopped. 

	@param	aPlc ETrue = plc enabled; EFalse = plc disabled

	@return KErrNone if successful. KErrInUse if this method is used when encoding is 
	active. KErrNotSupported if this method is not implemented.
	*/
	virtual TInt SetPacketLossConcealment(TBool aPlc) = 0;

	/** 	
	Used to find out if packet loss concealment capability is enabled or not.

	This method can be called when decoding is not active - anytime before started or 
	after stopped. 

	@param 	aPlc ETrue = packet loss concealment enabled; EFalse = packet loss 
	concealment disabled

	@return	KErrNone if successful. KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt GetPacketLossConcealment(TBool& aPlc) = 0;

	};
	
/*****************************************************************************/
/**
UID associated with the Custom interface MG711EncoderIntfc
*/
const TUid KUidG711EncoderIntfc = {0x10282602};

/**
This class provides an interface to the G711 encoder CMMFHwDevice in order to provide 
additional configuration information.
*/
class MG711EncoderIntfc 
	{
public:
	/** The encoder mode */
	enum TEncodeMode
        {
        /** EncALaw */
        EEncALaw,
        /** EncULaw */
        EEncULaw
        };

	/** 	
	Configures the encoder's encoding format.

	This method can be called when decoding is not active - anytime before started or 
	after stopped. 

	@param 	aEncodeMode the encode mode.

	@return	KErrNone if successful. KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt SetEncoderMode(TEncodeMode aEncodeMode) = 0;

	/** 	
	Controls voice activity detection (VAD) mode.

	This method can be called at all times.

	@param 	aVadModeOn ETrue=On, EFalse=Off

	@return	KErrNone if successful, 
			KErrNotSupported if this method is not implemented or not supported by the encoder
	*/
	virtual TInt SetVadMode(TBool aVadModeOn) = 0;

	/** 
	Gets the current state of the voice activity detection (VAD) mode.

	This method can be called at all times - while actively encoding or not.

	@param 	aVadModeOn On output ETrue=On, EFalse=Off

	@return	KErrNone if successful. 
			KErrNotSupported if this method is not implemented or VAD is not supported by the encoder. 
	*/
	virtual TInt GetVadMode(TBool& aVadModeOn) = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MG729DecoderIntfc
*/
const TUid KUidG729DecoderIntfc = {0x10282603};

/**
This class provides an interface to the G729 decoder CMMFHwDevice in order to provide additional 
configuration information.
*/
class MG729DecoderIntfc
	{
public:
	/**
	Informs the decoder that the next buffer sent to it will contain bad LSF data.

	This method can be called when decoding is active.
 
	This method will be called after a buffer has been received from the CMMFHwDevice, 
	and before it is returned to the CMMFHwDevice.

	@return	KErrNone if successful. 
			KErrInUse if this method is used out of sequence. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual	TInt BadLsfNextBuffer() = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MG729EncoderIntfc
*/
const TUid KUidG729EncoderIntfc = {0x10282604};

/**
This class provides an interface to the G729 encoder CMMFHwDevice in order to provide 
additional configuration information.
*/
class MG729EncoderIntfc
	{
public:
	/** 
	Controls voice activity detection (VAD) mode.

	This method is allowable at all times - while actively encoding or not.
	@param 	aVadModeOn ETrue=On, EFalse=Off

	@return	KErrNone if successful. KErrNotSupported if this method is not implemented 
	or not supported by the encoder. 
	*/
	virtual TInt SetVadMode(TBool aVadModeOn) = 0;

	/**
	Gets the current state of the voice activity detection (VAD) mode.

	This method can be called at all times - while actively encoding or not.

	@param 	aVadModeOn On output ETrue=On, EFalse=Off
	@return	KErrNone if successful. KErrNotSupported if this method is not implemented 
	or VAD is not supported by the encoder. 
	*/
	virtual TInt GetVadMode(TBool& aVadModeOn) = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MIlbcDecoderIntfc
*/
const TUid KUidIlbcDecoderIntfc = {0x10282605};

/**
This class provides an interface to the Ilbc decoder CMMFHwDevice in order to provide 
additional configuration information.
*/
class MIlbcDecoderIntfc
	{
public:
	/** The Ibc decoder mode */
	enum TDecodeMode
        {
        /** 20msFrame */
        E20msFrame,
        /** 30msFrame */
        E30msFrame
        };

	/**
	Informs the decoder of the encoded data format that will be sent to it.

	This method can be called when decoding is not active - anytime before started or 
	after stopped. 

	@param 	aDecodeMode the decode mode.

	@return	KErrNone if successful. 
			KErrInUse if this method is used when decoding is active. 
			KErrNotSupported if this method is not implemented.
	*/
	virtual TInt SetDecoderMode(TDecodeMode aDecodeMode) = 0;

	/**
	Used to enable and disable the comfort noise generation capability.

	This method is allowable when decoding is not active - anytime before started or 
	after stopped. 

	@param 	aCng ETrue = comfort noise generation enabled; EFalse = comfort noise 
	generation disabled

	@return	KErrNone if successful. 
			KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt SetComfortNoiseGeneration(TBool aCng) = 0;

	/**
	Used to find out if comfort noise generation is enabled or not.

	This method is allowable when decoding is not active - anytime before started or 
	after stopped. 

	@param 	aCng On output ETrue = comfort noise generation enabled; EFalse = comfort 
	noise generation disabled

	@return	KErrNone if successful. 
			KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt GetComfortNoiseGeneration(TBool& aCng) = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MIlbcEncoderIntfc
*/
const TUid KUidIlbcEncoderIntfc = {0x10282606};

/**
This class provides an interface to the Ilbc encoder CMMFHwDevice in order to provide 
additional configuration information.
*/
class MIlbcEncoderIntfc
	{
public:
	/** The Ibc encoder mode*/
	enum TEncodeMode
        {
        /** 20msFrame */
        E20msFrame,
        /** 30msFrame */
        E30msFrame
        };

	/**
	Configures the encoder's encoding format.

	This method is allowable when encoding is not active - anytime before started or 
	after stopped. 

	@param 	aEncodeMode the encode mode.

	@return	KErrNone if successful. 
			KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt SetEncoderMode(TEncodeMode aEncodeMode) = 0;

	/**
	Controls voice activity detection (VAD) mode.

	This method is allowable at all times - while actively encoding or not.

	@param 	aVadModeOn ETrue=On, EFalse=Off

	@return	KErrNone if successful. 
			KErrNotSupported if this method is not implemented or not supported by the encoder. 
	*/
	virtual TInt SetVadMode(TBool aVadModeOn) = 0;

	/**
	Gets the encoder's current encoding format.

	This method is allowable when encoding is not active - anytime before started or 
	after stopped. 

	@param 	&aEncodeMode the encode mode.

	@return	KErrNone if successful. 
			KErrInUse if this method is used when encoding is active. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt GetEncoderMode(TEncodeMode &aEncodeMode) = 0;

	/**
	Gets the current state of the voice activity detection (VAD) mode.

	This method is allowable at all times - while actively encoding or not.

	@param 	aVadModeOn On output ETrue=On, EFalse=Off

	@return	KErrNone if successful. 
			KErrNotSupported if this method is not implemented or VAD is not supported 
			by the encoder. 
	*/
	virtual TInt GetVadMode(TBool& aVadModeOn) = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFErrorConcealmentIntfc
*/
const TUid KUidErrorConcealmentIntfc = {0x10282607};

/**
This custom interface can be used to indicate that data has been lost. Typical usage 
would be during streaming. Lost packets of audio data would not get played, but the 
video data may still be available for playback. If video playback is synchronized to 
audio then this API can be used to indicate that audio data is lost so that video 
playback can continue. It could also be used for an audio-only source to maintain 
synchronization in time if data is lost.
*/
class MMMFErrorConcealmentIntfc
	{
public:
	/**
	Indicates that next buffer sent to CMMFHwDevice will not contain any valid data, 
	and that a single frame of data has been lost, and that error concealment should 
	be provided for that lost frame. This method will be called after a buffer has been 
	received from the Hardware Device, and before it is returned to the Hardware Device.

	@return	KErrNone if successful. 
			KErrInUse if this method is used out of sequence. 
			KErrNotSupported if this method is not implemented.
	*/
	virtual TInt ConcealErrorForNextBuffer() = 0;
	/**
	Puts the CMMFHwDevice into frame-based operation. This may be required in cases where
	 the decoder only supports error concealment in a frame-based mode of operation. 
	 Operating in frame-based mode means that the decoder requires that each buffer it 
	 receives contains only complete data frames. 

	Frame mode is limited to a single frame per buffer. The alternative to frame-based mode 
	is buffer-based mode, the default interface for CMMFHwDevices and decoders. 

	In buffer-based mode, complete data frames are not required to be contained within a 
	single buffer. A data frame may span consecutive data buffers in buffer-based mode. 

	As it is less efficient than buffer-based mode, Frame-based mode should be used as a 
	less-preferred option where errors are present and the CMMFHwDevice implementation would 
	not be tolerant to buffer-based mode error.  This condition would be indicated by 
	FrameModeRqrdForEC resulting in its parameter being set to ETrue.
  
	This method can be called when decoding is not active - anytime before the CMMFHwDevice 
	is started, or after it is stopped.

	@param	aFrameModeOn ETrue = frame mode on; EFalse = frame mode off.
	
	@return KErrNone if successful. 
			KErrInUse if this method is used out of sequence. 
			KErrNotSupported if this method is not implemented.	
	*/
	virtual TInt SetFrameMode(TBool aFrameModeOn) = 0;

	/**
	Queries the CMMFHwDevice to find out if frame mode is required by the implementing 
	CMMFHwDevice in order for it to support error concealment.
	
	This method is allowable when decoding is not active - anytime before the CMMFHwDevice 
	is started, or after it is stopped.
	@param	aFrameModeRqrd ETrue = frame mode required; EFalse = frame mode not required
	@return	KErrNone if successful. 
			KErrInUse if this method is used during decoding. 
			KErrNotSupported if this method is not implemented. 
	*/
	virtual TInt FrameModeRqrdForEC(TBool& aFrameModeRqrd) = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MAudioVibraControl
*/
const TUid KUidAudioVibraControl = {0x10282608};

/**
This class provides an interface for controlling vibra. DevSound should interpret the 
instantiation of this interface as a vibra control request by the user. The default vibra 
behavior implemented by the DevSound for the associated stream will not be performed while 
the user has control, In other words whilst this interface is instantiated. DevSound default vibra behavior 
for the associated stream will return once this interface is deleted. Other instances of 
DevSound may be instantiated for other audio streams at the same time. When multiple instances 
of DevSound are instantiated, priority and preference values are used to determine which audio 
streams are rendered. Prioritization of vibra control is determined by adaptation. Due to 
resource conflicts, and prioritization by adaptation, it may be possible that an audio stream 
is played, but not the vibra that accompanies that audio. For instance, if audio is mixed and 
two audio streams are rendered, vibra playback might not be mixed. Vibra playback may be 
prioritized by adaptation to a single controller. The behavior in these cases would be based 
on adaptation implementation. The user is not notified if vibra is not played.
*/
class MAudioVibraControl 
	{
public:
	/** 
	Turns vibra on.

	@return	KErrNone if successful KErrNotSupported if this method is not implemented.	
	*/
	virtual TInt StartVibra() = 0;
	
	/** 
	Turns vibra off.
	
	@return	KErrNone if successful. KErrNotSupported if this method is not implemented.	
	*/
	virtual TInt StopVibra() = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFSampleBuffering
*/
const TUid KUidSampleBuffering = {0x10282609};

/**
Custom interface class to allow DevSound to support buffering of samples before playback begins. 
This allows playback to be more robust to underflow errors when playback begins at the expense 
of higher latency before playback starts.
*/
class MMMFSampleBuffering
	{
public:
	/**
	Enables buffering up of samples before playback begins. This function is only effective before 
	CMMFDevSound::PlayInit() is called - calling the function after this will have no effect.
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmsbEnableSampleBufferingBeforePlayback() = 0;
	
	/**
	Disables buffering up of samples before playback begins. This function is only effective before 
	CMMFDevSound::PlayInit() is called - calling the function after this will have no effect.
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmsbDisableSampleBufferingBeforePlayback() = 0;

	/**
	Asynchronous request that completes when Playback actually starts.
	@param	"aStatus" "TRequestStatus that will be completed when play actually starts"
	*/
	virtual void MmsbNotifyPlayStarted(TRequestStatus& aStatus) = 0;

	/**
	Cancel any outstanding asynchronous NotifyPlayStarted requests.
	*/
	virtual void MmsbCancelNotifyPlayStarted() = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFPlaybackStatus.
*/
const TUid KUidPlaybackStatus = {0x10273811};

/**
Custom interface class to enable access to the DevSound implementation to report the number of 
consumed input bytes, number of successfully decoded samples, samples played, system time and
total samples played.
*/	
class MMMFPlaybackStatus
	{
public:
	/**  Play back status information */
	struct TMMFPlaybackStatus
		{
		/** Number of consumed input bytes */
		TInt64	aInputBytes;	
		/** Number of successfully decoded samples */
		TInt64	aDecodedSamples;		 
		/** Number of decoded samples played */
		TInt64	aDecodedSamplesPlayed;	
		/** Current system time */
		TInt64	aSystemTime;
		/**  Total samples played */
		TInt64 aTotalSamplesPlayed;
		};
	/**
	A method to query for detailed status information from a playing DevSound.
	including timestamp information from the DSP.
	@param  aStatus the current playback status.
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmpsGetPlaybackStatusInformation(TMMFPlaybackStatus& aStatus)= 0;

	/**
	Request notification of loss of sync during decode - the notification will be received 
	in the DevSound client callback SendEventToClient with the	event type set to KMMFEventLossOfDecodeSync. 
	
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmpsRequestLossOfSyncNotification() = 0;
	
	/**
	Cancel the request for notification of loss of sync.
	
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmpsCancelLossOfSyncNotification() = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFDSPControl
*/
const TUid KUidDSPControl = {0x1028260A};

/**
Custom interface class to allow control of DSP behaviour.
*/		
class MMMFDSPControl
	{
public:
	
	/**
	Get the timestamp information from the DSP
	
	@param aSamplesPlayed - number of audio samples played so far.
	@param aSystemTime - current system time 
	@param aB - bytes consumed by the decoder
	@param aT - bytes successfully decoded by the decoder.

	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmdspcGetAudioPlaybackInfo(TInt64& aSamplesPlayed, TInt64& aSystemTime, TUint& aB, TUint& aT) = 0;

	/**
	Place DevSound in a mode where overflow/underflow errors on recording are ignored.
	@return "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmdspcAcceptRecordBuffersInvalidFollowingStop() = 0;

	/**
	Place DevSound in a mode where overflow/underflow errors on playing are ignored.
	@return "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmdspcAcceptPlaybackBuffersInvalidFollowingStop() = 0;
	};
	
/*****************************************************************************/
/**
UID associated with the Custom interface MMMFUnderflowAutoStopControl
*/
const TUid KUidUnderflowAutoStopControl = {0x1028260B};

/**
Custom interface class to control underflow behaviour.
*/	
class MMMFUnderflowAutoStopControl
	{	
public:
	/**
	Turn off the default DevSound behaviour of automatically Stopping when an underflow 
	occurs. The underflow will be reported but play can continue when the client sends 
	more data - it is up to the client to call stop even in the case of end of file.
	
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmuascTurnOffUnderflowAutoStop() = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFSetDRMProtected
*/
const TUid KUidSetDRMProtected = {0x1028260C};

/**
This class provides an interface to mark data being played as DRM protected.
*/	
class MMMFSetDRMProtected
	{
public:
	/**
	Indicate whether the data being played is DRM protected or not
	 
	@param  aDRMProtected the required DRM state.
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmsdpMarkDataAsDRMProtected(TBool aDRMProtected) = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFBufferFramesConfig.
*/
const TUid KUidBufferFramesConfig = {0x1028260D};

/**
This class provides an interface to configure and retrive the buffer frames.
*/		
class MMMFBufferFramesConfig
	{
public:
	/**
	For use when encoding to set the number of encoded frames contained in each buffer 
	sent back to the client. The method should be called after CMMFDevSound::InitilaizeL 
	but before RecordInitL.

	@param aFrameCount - Number of frames per buffer 
	@param aSamplesPerFrame - numer of samples per frame
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmbfcSetNumberOfFramesPerInputBuffer(TInt aFrameCount, TInt aSamplesPerFrame)=0;
	
	/**
	For use when decoding to set the number of encoded frames contained in each buffer sent 
	down to DevsSund for playback. The method should be called after CMMFDevSound::InitilaizeL 
	but before PlayInitL.

	@param aFrameCount - Number of frames per buffer 
	@param aSamplesPerFrame - numer of samples per frame
	
	@return  "This method may return one of the system-wide error codes."
	*/
	virtual TInt MmbfcSetNumberOfFramesPerOutputBuffer(TInt aFrameCount, TInt aSamplesPerFrame)=0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFGetTimestamps
*/
const TUid KUidGetTimestamps = {0x1028260E};

/**
This class provides an interface to configure and retrive time stamps.
*/	
class MMMFGetTimestamps
	{
public:
	/**
	Enable or disable support for system timestamps during recording, used for Audio / Video sync. 
	This method can only be called after recording has been initialised, but before recording 
	has started. Once recording is underway, the setting cannot be changed.

	@param aEnable ETrue to enable recording timestamps, EFalse to disable it.

	@return KErrNotReady if DevSound has not yet been initialised, KErrNotSupported if 
	DevSound is not in recording mode or KErrInUse if recording is already underway.
	*/
	virtual TInt MmgtSetRecordSystemTimestampsEnabled(TBool aEnable) = 0;

	/**
	This method returns the system timestamp corresponding to the CMMFDataBuffer with the 
	given position within the recording stream. This timestamp is the system time at which 
	the buffers timestamp, as returned by CMMFBuffer::TimeToPlay, was valid.
	This method can only be used if the recording timestamps were enabled before recording 
	started using the SetRecordSystemTimestampsEnabled method.

	@param aBufferPosition The timestamp from the buffer, as returned by CMMFBuffer::TimeToPlay.
	@param aTimestamp Returns the corresponding system time, in universal time.

	@return A System error code. 
			KErrNotSupported if recording is not ongoing; 
			KErrNotReady if DevSound is not initialized, or TimeStamps have not been enabled; 
			KErrNotFound if the timestamp given does not correspond to a recent record buffer.
	*/
	virtual TInt MmgtGetSystemTimestampForBuffer(const TTimeIntervalMicroSeconds& aBufferPosition, 
	                                                   TTime& aTimestamp) const = 0;
	};

/*****************************************************************************/
/**
UID associated with the Custom interface MMMFAdvancedAACEncodeSettings
*/
const TUid KUidAdvancedAACEncoderSettings = {0x102825FE};

/**
Advanced AAC Encode Bit-rate Configuration
*/
class MMMFAdvancedAACEncodeSettings
	{
public:
	/** AAC BItrate mode */
	enum TAACBitrateMode
		{
		/** AACBitrateModeNone */
		EAACBitrateModeNone,
		/** AACBitrateModeAbsolute */
		EAACBitrateModeAbsolute,
		/** AACBitrateModeVariable */
		EAACBitrateModeVariable,
		/** AACBitrateModeFixed */
		EAACBitrateModeFixed
		};
	/**
	SetAACEncodeBitrateMode

	Set the bitrate mode for AAC encoding. This must be called before the encoding starts.

	@param aBitrateMode the structure containing the additional information required for decoding the raw AAC.
	@return KErrNone if successful. KErrInUse if this method is used when decoding is active. 
	*/
	virtual TInt SetAACEncodeBitrateMode(TAACBitrateMode aBitrateMode)=0;

	/**
	This interface returns the current AACEncoding bitrate mode.

	@param  aBitrateMode the structure containing the additional information.
	@return  KErrNone if successful. KErrNotSupported if this method is not implemented.
	*/
	virtual TInt GetAACEncodeBitrateMode(TAACBitrateMode& aBitrateMode) = 0;
	};
	
/*****************************************************************************/

	
#endif 
	
