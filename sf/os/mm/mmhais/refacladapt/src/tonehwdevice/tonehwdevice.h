/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef C_TONEHWDEVICE_H
#define C_TONEHWDEVICE_H

//  INCLUDES
#include <mmf/server/mmfhwdevice.h>
#include <mmf/server/sounddevice.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>
#include <a3f/a3f_trace_utils.h>
#include "mdasoundadapter.h"
#include "ToneGenerator.h"
#include "tonedatapath.h"
#include <a3f/tonedata.h>


//note we need to keep this buffer at 8K as the tone utility expects 8K
const TInt KPCM16ToPCM16BufferSize = 0x2000;

//controlls buffer sizes
const TInt KDevSoundDefaultFrameSize = 0x1000;
const TInt KDevSoundMinFrameSize = 0x800; //2K
const TInt KDevSoundMaxFrameSize = 0x4000;  //16K
const TInt KDevSoundDeltaFrameSize = 0x800; //2K
const TInt KDevSoundFramesPerSecond = 4;

// FORWARD DECLARATIONS
class CToneDataPath;

// CLASS DECLARATION

/**
 * Implementation of custom interface class for tone play functionality created by the
 * CToneCodec::CustomInterface() method.  It provides
 * access to miscellaneous functionality such as volume settings
 */
class TToneCustomInterface : public MPlayCustomInterface
	{
public:
	TToneCustomInterface() : iVolume(0),iBytesPlayed(0),iDevice(NULL),iRampDuration(0) {}
	void SetVolume(TUint aVolume);
	TUint Volume();
	TUint BytesPlayed();
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	TTimeIntervalMicroSeconds& VolumeRamp();
	void SetDevice(RMdaDevSound* iDevice);
private:
	TUint iVolume;
	TUint iBytesPlayed;
	RMdaDevSound* iDevice;
	TTimeIntervalMicroSeconds iRampDuration;
	};



/*
* Codec Implementation
*/

class CToneCodec : public CBase
	{
public:
	/**
	Indicates the result of processing data from the source buffer to a destination buffer
	and provides functions to compare the result code.
	The CToneCodec buffer sizes should be set to return EProcessComplete
	The other return codes are to keep the ProcessL method compatible with
	the 7.0s CMMFCodec API.
	*/
	class TCodecProcessResult
		{
	public:
		/**
		Flag to track the codec's processing status.
		*/
		enum TCodecProcessResultStatus
			{
			/** The codec has successfully completed its processing. */
			EProcessComplete,
			/** Could not empty the source buffer because the destination buffer became full. */
			EProcessIncomplete,
			/** Codec came across an end of data. */
			EEndOfData,
			/** Could not fill the destination buffer because the source buffer has been emptied. */
			EDstNotFilled,
			/** An error occured. */
			EProcessError
			};

		/** Overloaded operator to test equality. */
		TBool operator==(const TCodecProcessResultStatus aStatus) const {return (iCodecProcessStatus == aStatus);}
		/** Overloaded operator to test inequality. */
		TBool operator!=(const TCodecProcessResultStatus aStatus) const {return (iCodecProcessStatus != aStatus);}

		/**
		Default constructor.
		*/
		TCodecProcessResult()
			:iCodecProcessStatus(EProcessError), iSrcBytesProcessed(0), iDstBytesAdded(0) {};

		public:
		/**
		The codec's processing status

		@see enum TCodecProcessResultStatus
		*/
		TCodecProcessResultStatus iCodecProcessStatus;

		/** The number of source bytes processed */
		TUint iSrcBytesProcessed;

		/** The number of bytes added to the destination buffer */
		TUint iDstBytesAdded;
		};
public:

	CToneCodec();
	~CToneCodec();

	void ConstructL();


	/**
	Processes the data in the specified source buffer and writes the processed data to
	the specified destination buffer.

	This function is synchronous, when the function returns the data has been processed.

	@param	aSource
			The source buffer containing data to encode or decode.
	@param	aDest
	 		The destination buffer to hold the data after encoding or decoding.

	@return	The result of the processing.

	@see    TCodecProcessResult
	*/
	TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);

	/**
	Gets the max size of the source buffer passed into the
	CToneCodec::ProcessL function.

	Note that this means that this is the Max size of each buffer passed to the codec.  The actual
	size of the data could be less than the max size.

	@return The max size of the source buffer in bytes.
	*/
	TUint SourceBufferSize();

	/**
	Gets the max size of the sink (destination) buffer passed into the
	CToneCodec::ProcessL method.

	Note that this means that this is the Max size of each buffer passed to the codec.  The actual
	size of the data written to this buffer could be less than the max size.

	@return The max size of the sink buffer in bytes.
	*/
	TUint SinkBufferSize();

	TBool IsNullCodec() {return ETrue;};


	private:

	TUint iBufferSize;

	};


class CToneHwDevice : public CMMFHwDevice,
					  public MMMFHwDeviceObserver

	{
	public:  // Constructors and destructor

	static CToneHwDevice* NewL();
	~CToneHwDevice();

	public: // New functions

	public: // Functions from base classes


	TInt Init(THwDeviceInitParams& aDevInfo);
	TInt Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/);
	TInt Stop();

	/* This function is not used in tone playback*/
	TInt Pause();

	TAny* CustomInterface(TUid aInterfaceUid);

	TInt FillThisHwBuffer(CMMFBuffer& aHwBuffer);

	TInt ThisHwBufferFilled(CMMFBuffer& aMmfBuffer);

	/*From MMMFHwDeviceObserver*/
	/* This function is not used in tone playback*/
	TInt ThisHwBufferEmptied(CMMFBuffer& aMmfBuffer);

	/*From MMMFHwDeviceObserver*/
	/* This function is not used in tone playback*/
	TInt EmptyThisHwBuffer(CMMFBuffer& aMmfBuffer);

	/*From MMMFHwDeviceObserver*/
	TInt MsgFromHwDevice(TUid aMessageType, const TDesC8 &aMsg);

	/*From MMMFHwDeviceObserver*/
	void Stopped();

	/*From MMMFHwDeviceObserver*/
	void Error(TInt aError);

	TInt SetConfig(TTaskConfig& aConfig);

	/* This function is not used in tone playback*/
	TInt StopAndDeleteCodec();

	/* This function is not used in tone playback*/
	TInt DeleteCodec();

	CToneCodec& Codec();

	TInt GenerateBufferData();

	void SetActiveToneBuffer();

	TInt SamplingFrequency();

	TInt NumberOfChannels();

	TInt FillFreeToneBuffer();

	TInt ReadToneData();

	void FreeBuffers();

	TBool ValidDTMFString(const TDesC& aDTMFString);

	TBool RecognizeSequence(const TDesC8& aData);

	protected:  // New functions
	protected:  // Functions from base classes

	private:

	CToneHwDevice();
	void ConstructL();

	public:		// Data
	protected:	// Data
	private:	// Data

	/**
	* Pointer to the buffer that was last sent to the observer to be filled.
	* Own pointer.
	*/
	CMMFDataBuffer* iHwDataBufferFill;

	/**
	* Hwdevice observer. Information is send to upper level by using this pointer.
	*/
	MMMFHwDeviceObserver* iHwDeviceObserver;

	/**
	The datapath used to transfer the data
	*/
	CToneDataPath* iDataPath;

	MPlayCustomInterface* iPlayCustomInterface;

	/**
	* Initialize status of the tone
	*/
	TBool iToneInitialized;

	/**
	* Playback status of the tone
	*/
	TBool iTonePlaying;

	/**
	* Pointer to information about hwdevice initializing parameters.
	*/
	//TSizeHwDeviceInitArgs* iSizeHwDeviceInitArgs;

	/**
	* Type of the tone
	*/
	TToneData::TToneType iToneType;

	/**
	* Tone Data
	*/
	TToneData myToneData;

	/**
	* Tone Codec
	*/
	CToneCodec *iCodec;

	/**
	The buffer size of the sound device
	*/
	TUint iDeviceBufferSize;

	/**
	The sample rate of the sound device
	*/
	TInt iSampleRate;
	
	/**
	The number of channels of the sound device
	*/
	TInt iChannels;

	TBool iLastBuffer;

	TTimeIntervalMicroSeconds iRampDuration;

	//WINS Sound Device Structures
	RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;

	// Double buffer tone playing
	CMMFDataBuffer*				iToneBuffer1;
	CMMFDataBuffer*				iToneBuffer2;
	// Reference to current tone buffer playing
	CMMFDataBuffer*				iActiveToneBuffer;

	TBool						iFirstCallFromHwDevice;

	//Tone Stuff:

	MMdaToneSynthesis*			iCurrentGenerator;
	TMdaSimpleToneGenerator		iToneGen;
	TMdaDualToneGenerator		iDualToneGen;
	TMdaDTMFGenerator			iDTMFGen;
	TMdaSequenceGenerator		iSequenceGen; // Not Supported
	TInt						iRepeatCount;
	TInt						iFrequency1;
	TInt						iFrequency2;
	TTimeIntervalMicroSeconds	iRepeatTrailingSilence;
	TTimeIntervalMicroSeconds	iDuration;

	TTimeIntervalMicroSeconds32 myToneOnLength;
	TTimeIntervalMicroSeconds32 myToneOffLength;
	TTimeIntervalMicroSeconds32 myPauseLength;

	TDesC *iDTMFString;

	TDesC8 *iSequenceData;
	};

	#include "tonehwdevice.inl"

#endif

// End of File
