/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Project specification for WMA 10 Pro decoder 
*              : Custom Interface.
*
*/


#ifndef WMADECODERINTFC_H
#define WMADECODERINTFC_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KUidWmaDecoderIntfc = {0x10207BAD};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;
class CMdaAudioInputStream;
class CMdaAudioOutputStream;

// CLASS DECLARATION

/**
*  Interface for sending configuration info to WMA 10 Pro decoder.
*  This abstract class just provides the static NewL function for the 
*  creation of the proxy. Also, it defines the APIs to be implemented 
*  by the proxy and the real custom interface implementation.
*
*  @lib WmaDecoderIntfc.lib
*  @since S60 3.2
*/
class CWmaDecoderIntfc : public CBase
    {
    public: //data types

    enum TFormat
        {
        EWmaVoice = 10,
        EWmaV1 = 352,
        EWma = 353, 
        EWmaV2 = 353, 
        EWmaPro  = 354,
        EWmaLossless = 355,
        };

    enum TTool
        {
 		EToolOutput32Bit	= 1, //default = disabled
		EDownMixToStereo	= 2, //default = disabled
		ELostDataConcealment= 3  // default = disabled
		};

    public:  // Constructors and destructor

        /**
        * Creates the interface.
        */
		IMPORT_C static CWmaDecoderIntfc* 
		                NewL(CMMFDevSound& aDevSound);

        /**
        * Creates the interface 
        * This function will always leave with KErrNotSuppored
        */
		IMPORT_C static CWmaDecoderIntfc* 
		                NewL(CMdaAudioInputStream& aUtility);

        /**
        * Creates the interface.
        */
		IMPORT_C static CWmaDecoderIntfc* 
		                NewL(CMdaAudioOutputStream& aUtility);

    public: // New functions

        /**
        * Configures the compressed audio format
        * @since S60 3.2
        * @param aFormat - the audio format.
        * @return void
        */
		IMPORT_C virtual void SetFormat(TFormat aFormat) = 0;

        /**
        * Configures the number of bits per sample 
        * @since S60 3.2
        * @param aBitsPerSample - bits per sample.
        * @return void
        */
		IMPORT_C virtual void SetBitsPerSampleIn(TUint aBitsPerSample) = 0;

        /**
        * Configures the number of audio channels  
        * @since S60 3.2
        * @param aNumOfChannels: The number of audio channels.
        * @return void
        */
		IMPORT_C virtual void SetNumChannelsIn (TUint aNumChannelsIn) = 0;

        /**
        * Configures the sampling rate   
        * @since S60 3.2
        * @param aSamplesPerSec: Sample rate value in Hertz.
        * @return void
        */
		IMPORT_C virtual void SetSamplesPerSec (TUint aSamplesPerSec) = 0;

        /**
        * Configures the average number of bytes per second   
        * @since S60 3.2
        * @param aAvgBytesPerSec: Average bytes per second.
        * @return void
        */
		IMPORT_C virtual void SetAvgBytesPerSec (TUint aAvgBytesPerSec) = 0;

        /**
        * Configures the block align size in bytes    
        * @since S60 3.2
        * @param aBlockAlign: Block size in bytes
        * @return void
        */
		IMPORT_C virtual void SetBlockAlign (TUint aBlockAlign) = 0;

        /**
        * Configures information specific to the WMA decoder
        * @since S60 3.2
        * @param aEncodeOptions: The encode options
        * @return void
        */
		IMPORT_C virtual void SetEncodeOptions (TUint aEncodeOptions) = 0;

        /**
        * Configures information specific to the WMA 10 Pro decoder
        * @since S60 3.2
        * @param aEncodeOpts1: The encode options 1
        * @return void
        */
		IMPORT_C virtual void SetEncodeOptions1 (TUint aEncodeOpts1) = 0;

        /**
        * Configures information specific to the WMA 10 Pro decoder
        * @since S60 3.2
        * @param aEncodeOpts2: The encode options 2
        * @return void
        */
		IMPORT_C virtual void SetEncodeOptions2 (TUint aEncodeOpts2) = 0;

        /**
        * Configures the decoder for the channel data arrangement 
        * @since S60 3.2
        * @param aChannelMask: Channel mask
        * @return void
        */
		IMPORT_C virtual void SetChannelMaskIn (TUint aChannelMask) = 0;

        /**
        * Commits configuration settings to the decoder.
        * @since S60 3.2
        * @return TInt - Status
        */
		IMPORT_C virtual TInt ApplyConfig() = 0;

        /**
        * Returns the configuration state of audio format.
        * @since S60 3.2
        * @return aFormat: The audio format
        */
		IMPORT_C virtual TInt GetFormat (TFormat& aFormat) = 0;

        /**
        * Returns the configuration state of number of bits per sample.
        * @since S60 3.2
        * @return aBitsPerSample: Number of bits per sample
        */
		IMPORT_C virtual TInt GetBitsPerSampleIn(TUint& aBitsPerSample) = 0;

        /**
        * Returns the configuration state of the number of channels.
        * @since S60 3.2
        * @return aNumOfChannels: Number of channels
        */
		IMPORT_C virtual TInt GetNumOfChannelsIn (TUint& aNumOfChannels) = 0;

        /**
        * Returns the configuration state of the sample rate.
        * @since S60 3.2
        * @return aSamplesPerSec: Sample rate value in Hertz
        */
		IMPORT_C virtual TInt GetSamplesPerSec (TUint& aSamplesPerSec) = 0;

        /**
        * Returns the configuration state of average bytes per second.
        * @since S60 3.2
        * @return aAvgBytesPerSec: Average bytes per second
        */
		IMPORT_C virtual TInt GetAvgBytesPerSec (TUint& aAvgBytesPerSec) = 0;

        /**
        * Returns the configuration state of block alignment. 
        * @since S60 3.2
        * @return aBlockAlign: Block size in bytes
        */
		IMPORT_C virtual TInt GetBlockAlign (TUint& aBlockAlign) = 0;

        /**
        * Returns the configuration state of the encode options.
        * @since S60 3.2
        * @return aEncodeOpts: The encode options.
        */
		IMPORT_C virtual TInt GetEncodeOptions (TUint& aEncodeOpts) = 0;

        /**
        * Returns the configuration state of the advanced encode options.
        * @since S60 3.2
        * @return aEncodeOpts1: The encode options
        */
		IMPORT_C virtual TInt GetEncodeOptions1 (TUint& aEncodeOpts1) = 0;

        /**
        * Returns the configuration state of the advanced encode options.
        * @since S60 3.2
        * @return aEncodeOpts2: The encode options
        */
		IMPORT_C virtual TInt GetEncodeOptions2 (TUint& aEncodeOpts2) = 0;

        /**
        * Returns the configuration state of the input channel mask.
        * @since S60 3.2
        * @return aChannelMask: Channel mask. 
        */
		IMPORT_C virtual TInt GetChannelMaskIn (TUint& aChannelMask) = 0;

        /**
        * Returns a list of formats supported by the decoder.
        * @since S60 3.2
        * @return aSupportedFormats: An array of the formats, as defined in  TFormat
        */
		IMPORT_C virtual TInt GetSupportedFormats(RArray<TFormat>& aSupportedFormats) = 0;

        /**
        * Returns a list of tools supported by the decoder.
        * @since S60 3.2
        * @return aSupportedTools: An array of the tools supported by the decoder
        */
		IMPORT_C virtual TInt GetSupportedTools(RArray<TTool>& aSupportedTools) = 0;

        /**
        * Returns the maximum number of input channels supported by the decoder.
        * @since S60 3.2
        * @return aSupportedMaxChannelsIn: number of channels. 
        */
		IMPORT_C virtual TInt GetSupportedMaxChannelsIn(TUint& aSupportedMaxChannelsIn) = 0;

        /**
        * Returns the maximum bit rate supported by the decoder.
        * @since S60 3.2
        * @return aSupportedMaxBitrate: maximum bitrate
        */
		IMPORT_C virtual TInt GetSupportedMaxBitrate(TUint& aSupportedMaxBitrate) = 0;

        /**
        * Returns the maximum sample rate supported by the decoder.
        * @since S60 3.2
        * @return aSupportedMaxSampleRate: maximum sample rate
        */
		IMPORT_C virtual TInt GetSupportedMaxSampleRate(TUint& aSupportedMaxSampleRate) = 0;

        /**
        * Returns a list of the tools that are controllable through this interface.
        * @since S60 3.2
        * @return aControllableTools: An array of the tools that are controllable through this interface. 
        */
		IMPORT_C virtual TInt GetControllableTools(RArray<TTool>& aControllableTools) = 0;

        /**
        * Enables the tool specified.
        * @since S60 3.2
        * @return aTool: Specifies the tool to enable.
        */
		IMPORT_C virtual void EnableTool(TTool aTool) = 0;

        /**
        * Disables the tool specified.
        * @since S60 3.2
        * @return aTool: Specifies the tool to disable. 
        */
		IMPORT_C virtual void DisableTool(TTool aTool) = 0;

        /**
        * Returns the state of the specified tool
        * @since S60 3.2
        * @return aTool: Specifies the tool for which the state is requested
        * @       aEnabled : Indicates the state of the tool specified in aTool
        */
		IMPORT_C virtual TInt GetTool(TTool aTool, TBool& aEnabled) = 0;

	};

#endif      // WMADECODERINTFC_H

// End of File
