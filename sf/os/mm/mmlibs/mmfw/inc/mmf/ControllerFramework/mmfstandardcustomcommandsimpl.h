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

#ifndef __MMFSTANDARDCUSTOMCOMMANDS_IMPL_H
#define __MMFSTANDARDCUSTOMCOMMANDS_IMPL_H

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfvideo.h>
#include <mmf/common/mmcaf.h>

class CMMFVideoFrameMessage;
class CFbsBitmap;
class CDesC8Array;


/**
@publishedPartner
@released

Client class to allow setting the initial screen for video display.

*/
class RMMFVideoSetInitScreenCustomCommands : public RMMFCustomCommandsBase
	{
public:

	/**
	Constructor.

	@param  aController
	        The client side controller object to be used by this custom command interface.

	*/
	IMPORT_C RMMFVideoSetInitScreenCustomCommands(RMMFController& aController);

	/**
	Sets the initial screen number for the video display on the controller.

	@param  aScreenNumber
	        The screen number
	
	*/
	IMPORT_C TInt SetInitScreenNumber(TInt aScreenNumber);
	};



/**
@publishedPartner
@released

Mixin class to be derived from controller plugins that could support setting the initial screen for displaying video.
*/
class MMMFVideoSetInitScreenCustomCommandImplementor
	{
public:

	/**
	Sets the initial screen number.

	@param  aScreenNumber
	        The screen number
	*/
	virtual void MvsdSetInitScreenNumber(TInt aScreenNumber)=0;
	};


/**
@publishedPartner
@released

Custom command parser class to be used by controller plugins wishing to support setting the initial screen
number for the video display.

The controller plugin should create an object of this type and add it to the list of custom command parsers 
in the controller framework. The controller plugin must be derived from MMMFVideoSetInitScreenCustomCommandImplementor 
to use this class. 
*/
class CMMFVideoSetInitScreenCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser that supports setting screen device for video dusplay.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.

	@return A pointer to the object created.

	*/
	IMPORT_C static CMMFVideoSetInitScreenCustomCommandParser* NewL(MMMFVideoSetInitScreenCustomCommandImplementor& aImplementor);

	/**
	Destructor.

	*/
	IMPORT_C ~CMMFVideoSetInitScreenCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	*/
	void HandleRequest(TMMFMessage& aMessage);

private:
	/**
	Constructor.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.
	*/
	CMMFVideoSetInitScreenCustomCommandParser(MMMFVideoSetInitScreenCustomCommandImplementor& aImplementor);
	void DoHandleRequestL(TMMFMessage& aMessage);
	TBool DoSetInitScreenNumberL(TMMFMessage& aMessage);

private:
	MMMFVideoSetInitScreenCustomCommandImplementor& iImplementor;
	};


/**
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
@internalComponent
*/
enum TMMFVideoPixelAspectRatioMessages
	{
	EMMFVideoGetPixelAspectRatio,
	EMMFVideoSetPixelAspectRatio,
	EMMFVideoGetSupportedPixelAspectRatios,
	EMMFVideoPixelAspectRatioCopyArrayData
	};

/**
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
@internalComponent
*/
enum TMMFVideoAudioSamplingRateAndChannelConfigMessages
	{
	EMMFVideoGetAudioChannels,
	EMMFVideoSetAudioChannels,
	EMMFVideoGetSupportedAudioChannels,
	EMMFVideoGetAudioSampleRate,
	EMMFVideoSetAudioSampleRate,
	EMMFVideoGetSupportedAudioSampleRates,
	EMMFVideoAudioSamplingRateAndChannelConfigCopyArrayData
	};


/**
@publishedPartner
@released

Client class to allow setting and getting the pixel aspect ratio for video recording.

*/
class RMMFVideoPixelAspectRatioCustomCommands : public RMMFCustomCommandsBase
	{
public:

	/**
	Constructor.

	@param  aController
	        The client side controller object to be used by this custom command interface.

	*/
	IMPORT_C RMMFVideoPixelAspectRatioCustomCommands(RMMFController& aController);

	/**
	Sets the pixel aspect ratio for the video recording on the controller.

	@param  aAspectRatio
	        The new pixel aspect ratio. The default pixel aspect ratio is 1:1.
	
	@return One of the system-wide error codes.
	
	*/
	IMPORT_C TInt SetPixelAspectRatio(const TVideoAspectRatio& aAspectRatio);
	
	/**
	Gets the current pixel aspect ratio for the video recording on the controller.

	@param  aAspectRatio
	        The current video pixel aspect ratio.

	@return One of the system-wide error codes.	
	*/
	IMPORT_C TInt GetPixelAspectRatio(TVideoAspectRatio& aAspectRatio) const;
	
	/**
	Gets the list of supported pixel aspect ratios for the video recording on the controller.

	@param  aAspectRatios
	        The supported pixel aspect ratios.
	
	@leave	This method may leave with one of the system-wide error codes.
	*/
	IMPORT_C void GetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios) const;

private:
	void DoGetVideoPixelAspectRatioArrayL(RArray<TVideoAspectRatio>& aArray, TMMFVideoPixelAspectRatioMessages aIpc) const;
	};


/**
@publishedPartner
@released

Mixin class to be derived from controller plug-ins that could support setting and getting the pixel aspect ratio for video recording. 
*/
class MMMFVideoPixelAspectRatioCustomCommandImplementor
	{
public:

	/**
	Sets the pixel aspect ratio.

	@param  aAspectRatio
	        The pixel aspect ratio to use.
	*/
	virtual void MvparSetPixelAspectRatioL(const TVideoAspectRatio& aAspectRatio) = 0;

	/**
	Gets the current pixel aspect ratio.

	@param  aAspectRatio
	        The current pixel aspect ratio.
	*/
	virtual void MvparGetPixelAspectRatioL(TVideoAspectRatio& aAspectRatio) = 0;
	
	/**
	Gets the list of supported pixel aspect ratios.

	@param  aAspectRatios
	        The list of supported pixel aspect ratios.
	*/
	virtual void MvparGetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios) = 0;
	};


/**
@publishedPartner
@released

Custom command parser class to be used by controller plug-ins wishing to support setting and getting the pixel aspect 
ratio for the video recording.

The controller plug-in should create an object of this type and add it to the list of custom command parsers 
in the controller framework. The controller plug-in must be derived from MMMFVideoPixelAspectRatioCustomCommandImplementor 
to use this class. 
*/
class CMMFVideoPixelAspectRatioCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser that supports setting and getting pixel aspect ratio for video recording.

	@param  aImplementor
	        A reference to the controller plug-in that owns this new object.

	@return A pointer to the object created.

	*/
	IMPORT_C static CMMFVideoPixelAspectRatioCustomCommandParser* NewL(MMMFVideoPixelAspectRatioCustomCommandImplementor& aImplementor);

	/**
	Destructor.

	*/
	IMPORT_C ~CMMFVideoPixelAspectRatioCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	*/
	void HandleRequest(TMMFMessage& aMessage);

private:
	CMMFVideoPixelAspectRatioCustomCommandParser(MMMFVideoPixelAspectRatioCustomCommandImplementor& aImplementor);
	void DoHandleRequestL(TMMFMessage& aMessage);
	TBool DoSetPixelAspectRatioL(TMMFMessage& aMessage);
	TBool DoGetPixelAspectRatioL(TMMFMessage& aMessage);
	TBool DoGetSupportedPixelAspectRatiosL(TMMFMessage& aMessage);
	TBool DoCopyArrayDataL(TMMFMessage& aMessage);
	void DoCreateBufFromVideoAspectRatioArrayL(RArray<TVideoAspectRatio>& aArray);

private:
	MMMFVideoPixelAspectRatioCustomCommandImplementor& iImplementor;
	
	CBufFlat* iDataCopyBuffer;
	};


/**
@publishedPartner
@released

Client class to allow the audio sampling rate and channels configuration for video recording.

*/
class RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands : public RMMFCustomCommandsBase
	{
public:

	/**
	Constructor.

	@param  aController
	        The client side controller object to be used by this custom command interface.

	*/
	IMPORT_C RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands(RMMFController& aController);

	/**
	Sets the number of audio channels to record (1 for mono, 2 for stereo).

	@param  aChannels
	        The number of audio channels to record.

	@return One of the system-wide error codes.
	
	*/
	IMPORT_C TInt SetAudioChannels(const TUint aChannels);

	/**
	Gets the current number of audio channels to record (1 for mono, 2 for stereo).

	@param  aChannels
	        The current number of audio channels to record.

	@return One of the system-wide error codes.
	
	*/
	IMPORT_C TInt GetAudioChannels(TUint& aChannels) const;

	/**
	Gets the list of supported number of audio channels for video recording.

	@param  aChannels
	        The supported numbers of audio channels.
	
	@leave	This method may leave with one of the system-wide error codes.
	*/
	IMPORT_C void GetSupportedAudioChannelsL(RArray<TUint>& aChannels) const;
	
	/**
	Sets the audio sample rate for video recording.

	@param  aSampleRate
	        The sample rate for audio recording.

	@return One of the system-wide error codes.
	
	*/
	IMPORT_C TInt SetAudioSampleRate(const TUint aSampleRate);

	/**
	Gets the current audio sample rate for recording.

	@param  aSampleRate
	        The current audio sample rate for recording.

	@return One of the system-wide error codes.
	
	*/
	IMPORT_C TInt GetAudioSampleRate(TUint& aSampleRate) const;

	/**
	Gets the list of supported audio sample rates for video recording.

	@param  aSampleRates
	        The supported audio sample rates.
	
	@leave	This method may leave with one of the system-wide error codes.
	*/
	IMPORT_C void GetSupportedAudioSampleRatesL(RArray<TUint>& aSampleRates) const;
	
private:
	void DoGetUintArrayL(RArray<TUint>& aArray, TMMFVideoAudioSamplingRateAndChannelConfigMessages aIpc) const;
	};


/**
@publishedPartner
@released

Mixin class to be derived from controller plug-ins that could support the audio sampling rate and channels configuration. 
*/
class MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor
	{
public:

	/**
	Sets the number of audio channels.

	@param  aChannels
	        The number of audio channels to use.
	*/
	virtual void MvasrccSetAudioChannelsL(const TUint aChannels) = 0;

	/**
	Gets the current number of audio channels.

	@param  aChannels
	        The current number of audio channels.
	*/
	virtual void MvasrccGetAudioChannelsL(TUint& aChannels) = 0;

	/**
	Gets the list of supported number of audio channels.

	@param  aChannels
	        The list of supported number of audio channels.
	*/
	virtual void MvasrccGetSupportedAudioChannelsL(RArray<TUint>& aChannels) = 0;
	
	/**
	Sets the audio sample rate.

	@param  aSampleRate
	        The audio sample rate to use.
	*/
	virtual void MvasrccSetAudioSampleRateL(const TUint aSampleRate) = 0;

	/**
	Gets the current audio sample rate.

	@param  aSampleRate
	        The current audio sample rate.
	*/
	virtual void MvasrccGetAudioSampleRateL(TUint& aSampleRate) = 0;

	/**
	Gets the list of supported audio sample rates.

	@param  aSampleRates
	        The list of supported audio sample rates.
	*/
	virtual void MvasrccGetSupportedAudioSampleRatesL(RArray<TUint>& aSampleRates) = 0;
	};

/**
@publishedPartner
@released

Custom command parser class to be used by controller plug-ins wishing to support the audio sampling rate and channels
configuration for the video recording.

The controller plug-in should create an object of this type and add it to the list of custom command parsers 
in the controller framework. The controller plug-in must be derived from MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor 
to use this class. 
*/
class CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser that supports the audio sampling rate and channels configuration for video recording.

	@param  aImplementor
	        A reference to the controller plug-in that owns this new object.

	@return A pointer to the object created.

	*/
	IMPORT_C static CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser* NewL(MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor& aImplementor);

	/**
	Destructor.

	*/
	IMPORT_C ~CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	*/
	void HandleRequest(TMMFMessage& aMessage);

private:
	CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser(MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor& aImplementor);
	void DoHandleRequestL(TMMFMessage& aMessage);
	TBool DoSetAudioChannelsL(TMMFMessage& aMessage);
	TBool DoGetAudioChannelsL(TMMFMessage& aMessage);
	TBool DoGetSupportedAudioChannelsL(TMMFMessage& aMessage);
	
	TBool DoSetAudioSampleRateL(TMMFMessage& aMessage);
	TBool DoGetAudioSampleRateL(TMMFMessage& aMessage);
	TBool DoGetSupportedAudioSampleRatesL(TMMFMessage& aMessage);
	
	TBool DoCopyArrayDataL(TMMFMessage& aMessage);
	
	void DoCreateBufFromUintArrayL(RArray<TUint>& aArray);

private:
	MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor& iImplementor;
	
	CBufFlat* iDataCopyBuffer;
	};

/**
@publishedPartner
@released
*/
const TUid KUidInterfaceMMFVideoPlayExt = {0x10283437};	

class TVideoPlayRateCapabilities;

/**
Mixin class to be derived from controller plug-ins that could support
play velocity, enabling/disabling audio/video, and auto scaling.

@publishedPartner
@released
*/	
class MMMFVideoPlayControllerExtCustomCommandImplementor
	{
public:

	/**
	Sets play velocity. 
	Behaviour is same as CVideoPlayerUtility::SetPlayVelocityL
	
	@see CVideoPlayerUtility::SetPlayVelocityL
	*/
	virtual void MvpecSetPlayVelocityL(TInt aVelocity)=0;
	
	/**
	Gets play velocity.
	Behaviour is same as CVideoPlayerUtility::PlayVelocityL	
	
	@see CVideoPlayerUtility::PlayVelocityL
	*/
	virtual TInt MvpecPlayVelocityL()=0;
	
	/**
	Steps through frames in pause mode.
	Behaviour is same as CVideoPlayerUtility::StepFrameL	
		
	@see CVideoPlayerUtility::StepFrameL
	*/
	virtual void MvpecStepFrameL(TInt aStep)=0;
	
	/**
	Gets play rate capabilities.
	Behaviour is same as CVideoPlayerUtility::GetPlayRateCapabilitiesL	
		
	@see CVideoPlayerUtility::GetPlayRateCapabilitiesL
	*/
	virtual void MvpecGetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCapabilities)=0;
	
	/**
	Enables/Disables video playback.
	Behaviour is same as CVideoPlayerUtility::SetVideoEnabledL	
	
	@see CVideoPlayerUtility::SetVideoEnabledL
	*/
	virtual void MvpecSetVideoEnabledL(TBool aVideoEnabled)=0;
	
	/**
    Queries whether video playback is currently enabled or not.
	Behaviour is same as CVideoPlayerUtility::VideoEnabledL	
	
	@see CVideoPlayerUtility::VideoEnabledL
	*/
	virtual TBool MvpecVideoEnabledL()=0;
	
	/**
	Enables or disables audio playback.
	Behaviour is same as CVideoPlayerUtility::SetAudioEnabled	
	
	@see CVideoPlayerUtility::SetAudioEnabled
	*/
	virtual void MvpecSetAudioEnabledL(TBool aAudioEnabled)=0;
	
	/**
	Sets Auto scale mode.
	Behaviour is same as CVideoPlayerUtility::SetAutoScaleL	
	
	@see CVideoPlayerUtility::SetAutoScaleL
	*/
	virtual void MvpecSetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)=0;
	};



/**
Custom command parser class to be used by controller plug-ins wishing to support play velocity, 
enabling/disabling audio/video, and auto scaling.

The controller plug-in should create an object of this type and add it to the list of custom command parsers 
in the controller framework. The controller plug-in must be derived from MMMFVideoPlayControllerExtCustomCommandImplementor 
to use this class. 

@publishedPartner
@released
*/
class CMMFVideoPlayControllerExtCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser that supports the video rate control and auto scaling.

	@param  aImplementor
	        A reference to the controller plug-in that owns this new object.

	@return A pointer to the object created.

	*/
	IMPORT_C static CMMFVideoPlayControllerExtCustomCommandParser* NewL(MMMFVideoPlayControllerExtCustomCommandImplementor& aImplementor);

	/**
	Destructor.

	*/
	IMPORT_C ~CMMFVideoPlayControllerExtCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	*/
	void HandleRequest(TMMFMessage& aMessage);

private:
	CMMFVideoPlayControllerExtCustomCommandParser(MMMFVideoPlayControllerExtCustomCommandImplementor& aImplementor);
	void DoHandleRequestL(TMMFMessage& aMessage);
	TBool DoSetPlayVelocityL(TMMFMessage& aMessage);
	TBool DoPlayVelocityL(TMMFMessage& aMessage);
	TBool DoStepFrameL(TMMFMessage& aMessage);
	TBool DoGetPlayRateCapabilitiesL(TMMFMessage& aMessage);
	TBool DoSetVideoEnabledL(TMMFMessage& aMessage);
	TBool DoVideoEnabledL(TMMFMessage& aMessage);
	TBool DoSetAudioEnabledL(TMMFMessage& aMessage);
	TBool DoSetAutoScaleL(TMMFMessage& aMessage);
private:
	MMMFVideoPlayControllerExtCustomCommandImplementor& iImplementor;
	};

/**
Resource class to be used by player utility for sending custom commands.

@publishedPartner
@released
*/	
class RMMFVideoPlayControllerExtCustomCommands: public RMMFCustomCommandsBase
	{
public:
	/**
	Constructor.

	@param  aController
	        The client side controller object to be used by this custom command interface.
	*/
	IMPORT_C RMMFVideoPlayControllerExtCustomCommands(RMMFController& aController);
	
	/**
	Sets play velocity.
	Behaviour is same as CVideoPlayerUtility::SetPlayVelocityL
		
	@param  aVelocity 
			Velocity of the playback.

	@return One of the system-wide error codes.	

	@see CVideoPlayerUtility::SetPlayVelocityL	
	*/
	IMPORT_C TInt SetPlayVelocity(TInt aVelocity);

	/**
	Gets play velocity.
	Behaviour is same as CVideoPlayerUtility::PlayVelocityL	

	@param  aVelocity 
			Velocity of the playback.

	@return One of the system-wide error codes.	
		
	@see CVideoPlayerUtility::PlayVelocityL
	*/
	IMPORT_C TInt PlayVelocity(TInt &aVelocity)const;

	/**
	Steps through frames in pause mode.
	Behaviour is same as CVideoPlayerUtility::StepFrameL	

	@param  aStep 
			Count indicating number of frames to step playback position.

	@return One of the system-wide error codes.

	@see CVideoPlayerUtility::StepFrameL
	*/
	IMPORT_C TInt StepFrame(TInt aStep);
	
	/**
	Gets play rate capabilities.
	Behaviour is same as CVideoPlayerUtility::GetPlayRateCapabilitiesL	

    @param aCapabilities Playback rate capabilities

	@return One of the system-wide error codes.
		
	@see CVideoPlayerUtility::GetPlayRateCapabilitiesL
	*/
	IMPORT_C TInt GetPlayRateCapabilities(TVideoPlayRateCapabilities& aCapabilities)const;

	/**
	Enables/Disables video playback.
	Behaviour is same as CVideoPlayerUtility::SetVideoEnabledL	
    
    @param aVideoEnabled ETrue to enable video playback, EFalse to disable

	@return One of the system-wide error codes.    	

	@see CVideoPlayerUtility::SetVideoEnabledL
	*/
	IMPORT_C TInt SetVideoEnabled(TBool aVideoEnabled);
	
	/**
    Queries whether video playback is currently enabled or not.
	Behaviour is same as CVideoPlayerUtility::VideoEnabledL

    @param aVideoEnabled 
    	   Out parameter returns ETrue if video playback is enabled, EFalse if not.

	@return One of the system-wide error codes.
	
	@see CVideoPlayerUtility::VideoEnabledL
	*/
	IMPORT_C TInt VideoEnabled(TBool &aVideoEnabled)const;
	
	/**
	Enables or disables audio playback.
	Behaviour is same as CVideoPlayerUtility::SetAudioEnabledL

    @param aVideoEnabled ETrue to enable audio playback, EFalse to disable		

	@return One of the system-wide error codes.

	@see CVideoPlayerUtility::SetAudioEnabledL
	*/
	IMPORT_C TInt SetAudioEnabled(TBool aAudioEnabled);

	/**
	Sets Auto scale mode.
 	Behaviour is same as CVideoPlayerUtility::SetAutoScaleL
	
    @param aScaleType Automatic scaling type
    @param aHorizPos Video picture horizontal position, relative to the
                     video window.
    @param aVertPos Video picture vertical position, relative to the
                     video window.
	@return One of the system-wide error codes.
		
	@see CVideoPlayerUtility::SetAutoScaleL
	*/
	IMPORT_C TInt SetAutoScale(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos);
	};

/**
@publishedPartner
@released
*/
const TUid KUidInterfaceMMFVideoRecorderExt = {0x10283438};

/**
Mixin class to be derived from controller plug-ins that could support quality and enabling video. 

@publishedPartner
@released
*/	
class MMMFVideoRecordControllerExtCustomCommandImplementor
	{
public:

	/**
	Enabled or disables video in recorder.
	Behaviour is same as CVideoRecorderUtility::SetVideoEnabledL	

	@see CVideoRecorderUtility::SetVideoEnabledL
	*/
    virtual void MvrecSetVideoEnabledL(TBool aEnabled)=0;
	/**
	Gets the flag that indicates video enabled or not.
	Behaviour is same as CVideoRecorderUtility::VideoEnabledL	

	@see CVideoRecorderUtility::VideoEnabledL
	*/
    virtual TBool MvrecVideoEnabledL() const=0;
	/**
	Sets the quality of video being recorded.
	Behaviour is same as CVideoRecorderUtility::SetVideoQualityL	

	@see CVideoRecorderUtility::SetVideoQualityL
	*/
    virtual void MvrecSetVideoQualityL(TInt aQuality)=0;
	/**
	Gets video quality level set using SetVideoQualityL.
	Behaviour is same as CVideoRecorderUtility::VideoQualityL	

	@see CVideoRecorderUtility::VideoQualityL
	*/
    virtual TInt MvrecVideoQualityL() const=0;
	/**
	Sets the video framerate to fixed.
	Behaviour is same as CVideoRecorderUtility::SetVideoFrameRateFixedL	

	@see CVideoRecorderUtility::SetVideoFrameRateFixedL
	*/
    virtual void MvrecSetVideoFrameRateFixedL(TBool aFixedFrameRate)=0;
	/**
	Gets FrameRateFixed flag set using SetVideoFrameRateFixedL.
	Behaviour is same as CVideoRecorderUtility::VideoFrameRateFixedL	

	@see CVideoRecorderUtility::VideoFrameRateFixedL
	*/
    virtual TBool MvrecVideoFrameRateFixedL() const=0;
	};



/**
Custom command parser class to be used by controller plug-ins wishing to implement VideoRecorderExt custom commands.

The controller plug-in should create an object of this type and add it to the list of custom command parsers 
in the controller framework. The controller plug-in must be derived from MMMFVideoRecordControllerExtCustomCommandImplementor
to use this class.

@publishedPartner
@released
*/
class CMMFVideoRecordControllerExtCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser that supports the video rate control and auto scaling.

	@param  aImplementor A reference to the controller plug-in that owns this new object.
	@return A pointer to the object created.
	*/
	IMPORT_C static CMMFVideoRecordControllerExtCustomCommandParser* NewL(MMMFVideoRecordControllerExtCustomCommandImplementor& aImplementor);

	/**
	Destructor.
	*/
	IMPORT_C ~CMMFVideoRecordControllerExtCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage The message to be handled.
	*/
	void HandleRequest(TMMFMessage& aMessage);

private:
	CMMFVideoRecordControllerExtCustomCommandParser(MMMFVideoRecordControllerExtCustomCommandImplementor& aImplementor);
	void DoHandleRequestL(TMMFMessage& aMessage);
	TBool DoSetVideoEnabledL(TMMFMessage& aMessage);
	TBool DoVideoEnabledL(TMMFMessage& aMessage);
	TBool DoSetVideoQualityL(TMMFMessage& aMessage);
	TBool DoVideoQualityL(TMMFMessage& aMessage);
	TBool DoSetVideoFrameRateFixedL(TMMFMessage& aMessage);
	TBool DoVideoFrameRateFixedL(TMMFMessage& aMessage);
private:
	MMMFVideoRecordControllerExtCustomCommandImplementor& iImplementor;
	};

/**
Resource class to be used by Recorder utility for sending custom commands.

@publishedPartner
@released
*/	
class RMMFVideoRecordControllerExtCustomCommands: public RMMFCustomCommandsBase
	{
public:
	/**
	Constructor.

	@param  aController
	        The client side controller object to be used by this custom command interface.
	@return One of the system-wide error codes.
	*/
	IMPORT_C RMMFVideoRecordControllerExtCustomCommands(RMMFController& aController);
	
	/**
	Enabled or disables video in recorder.
	Behaviour is same as CVideoRecorderUtility::SetVideoEnabledL	

    @param aEnabled ETrue to enable video recording, EFalse to enable it

	@return One of the system-wide error codes.	
	@see CVideoRecorderUtility::SetVideoEnabledL
	*/
    IMPORT_C TInt SetVideoEnabled(TBool aEnabled);
    
	/**
	Gets the flag that indicates video enabled or not.
	Behaviour is same as CVideoRecorderUtility::VideoEnabledL	

    @param aEnabled 
    	   Out parameter returns ETrue if video recording is enabled , EFalse otherwise.

	@return One of the system-wide error codes.	
	@see CVideoRecorderUtility::VideoEnabledL
	*/
    IMPORT_C TInt VideoEnabled(TBool &aEnabled) const;
    
	/**
	Sets the quality of video being recorded.
	Behaviour is same as CVideoRecorderUtility::SetVideoQualityL	

    @param aQuality Video quality, in the range of 0 to 100, inclusive.

	@return One of the system-wide error codes.	
	@see CVideoRecorderUtility::SetVideoQualityL
	*/
    IMPORT_C TInt SetVideoQuality(TInt aQuality);
    
	/**
	Gets video quality level set using SetVideoQualityL.
	Behaviour is same as CVideoRecorderUtility::VideoQualityL	
    
    @param aQuality Out parameter returning Video quality in the range 0-100.
    
    @return One of the system-wide error codes.	
	@see CVideoRecorderUtility::VideoQualityL
	*/
    IMPORT_C TInt VideoQuality(TInt &aQuality) const;
    
	/**
	Sets the video framerate to fixed.
	Behaviour is same as CVideoRecorderUtility::SetVideoFrameRateFixedL	
	
    @param aFixedFrameRate ETrue to request a fixed framerate, EFalse to
                          request a variable framerate.

	@return One of the system-wide error codes.	
	@see CVideoRecorderUtility::SetVideoFrameRateFixedL
	*/
    IMPORT_C TInt SetVideoFrameRateFixed(TBool aFixedFrameRate);
    
	/**
	Gets FrameRateFixed flag set using SetVideoFrameRateFixedL.
	Behaviour is same as CVideoRecorderUtility::VideoFrameRateFixedL	

    @param aFixedFrameRate Out parameter returns ETrue if fixed framerate
     						is enabled, EFalse otherwise.
    
	@return One of the system-wide error codes.	
	@see CVideoRecorderUtility::VideoFrameRateFixedL
	*/
    IMPORT_C TInt VideoFrameRateFixed(TBool &aFixedFrameRate) const;
    };

/**
@publishedPartner
@released

Interface UID and messages for the Audio Play Controller Set Repeats API.
*/
const TUid KUidInterfaceMMFAudioPlaySetRepeatsController = {0x102834D5};


/**
@publishedPartner
@released

Client class to access loop play functionality of audio play controller.

The class uses the custom command function of the controller plugin, and removes the necessity
for the client to formulate the custom commands.
*/
class RMMFAudioPlayControllerSetRepeatsCustomCommands : public RMMFCustomCommandsBase
	{
public:
	
	/**
	Constructor.

	@param  aController
	        The client side controller object to be used by this custom command interface.
	*/
	IMPORT_C RMMFAudioPlayControllerSetRepeatsCustomCommands(RMMFController& aController);

	/**
	Sets the number of times the audio sample to be repeated and the trailing silence period
	
	@param  aRepeatNumberOfTimes
			The number of times the audio sample needs to be repeated
	
	@param  aTrailingSilence
	        The time interval of the trailing silence in microseconds.
	        
	@return KErrNotSupported if the controller plugin does not support the loop play functionality.
	*/
	IMPORT_C TInt SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	};

/**
@publishedPartner
@released

Mixin class to be derived from by controller plugins that could support the set repeats custom command.
*/
class MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor
	{
public:

	/**
	Sets the number of times the audio sample is to be repeated during the playback operation.	
	A period of silence can follow each playing of the sample. The audio sample can be repeated indefinitely.
	
	@param	aRepeatNumberOfTimes
    		The number of times the audio sample, together with the trailing silence, is to be repeated. 
    		If this is set to KMdaRepeatForever, then the audio sample, together with the trailing silence, 
    		is repeated indefinitely or until Stop() is called. 
    		If this is set to zero, then the audio sample is not repeated.
	
	@param  aTrailingSilence
         	The time interval of the trailing silence in microseconds.
	
	@return	KErrNone if the controller plugin supports the loop play functionality
			KErrNotSupported if the controller plugin does not support the loop play functionality
			KErrNotReady if the controller plugin is not yet added the audio sink
	*/		
	virtual TInt MapcSetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence) = 0;		
	};

/**
@publishedPartner
@released

Custom command parser class to be used by controller plugins wishing to support set repeats functionality

The controller plugin should create an object of this type and add it to the list of custom command parsers 
in the controller framework. The controller plugin must be derived from 
MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor to use this class. 
*/

class CMMFAudioPlayControllerSetRepeatsCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser capable of handling set repeats controller commands.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.

	@return A pointer to the object created.

	*/
	IMPORT_C static CMMFAudioPlayControllerSetRepeatsCustomCommandParser* NewL(MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor& aImplementor);

	/**
	Destructor.

	*/
	IMPORT_C ~CMMFAudioPlayControllerSetRepeatsCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	*/
	void HandleRequest(TMMFMessage& aMessage);
private:

	/**
	Constructor.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.

	*/
	CMMFAudioPlayControllerSetRepeatsCustomCommandParser(MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor& aImplementor);
	// Internal request handling methods.
	void DoHandleRequestL(TMMFMessage& aMessage);
	TBool DoSetRepeatsL(TMMFMessage& aMessage);
private:
	/** 
	The object that implements the set repeats custom command i.e controller plugin
	*/
	MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor& iImplementor;
	};

#endif
