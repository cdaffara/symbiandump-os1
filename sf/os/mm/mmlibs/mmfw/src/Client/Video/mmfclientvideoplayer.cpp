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


#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/common/mmfpaniccodes.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <videoplayer.h>
#include "mmfvideocallback.h"
#include "VideoPlayerBody.h"
#include "mmfvideopaniccodes.h"
#include "mmfclientvideocommon.h"


void Panic(TInt aPanicCode)
	{
	_LIT(KMMFMediaClientVideoPanicCategory, "MMFVideoClient");
	User::Panic(KMMFMediaClientVideoPanicCategory, aPanicCode);
	}

/**
Constructs and initialises a new instance of the video player utility.

This function leaves if the video player utility object cannot be created.

@param  aObserver
        A client class to receive notifications from the video player.
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.
@param  aWs
        The window server session id.
@param  aScreenDevice
        The software device screen.
@param  aWindow
        The display window.
@param  aScreenRect
        The dimensions of the display window.
@param  aClipRect
        The area of the video clip to display in the window.

@return A pointer to the new video player utility object.

@since 7.0s

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CVideoPlayerUtility* CVideoPlayerUtility::NewL(MVideoPlayerUtilityObserver& aObserver,
														TInt aPriority,
														TInt aPref,
														RWsSession& aWs,
														CWsScreenDevice& aScreenDevice,
														RWindowBase& aWindow,
														const TRect& aScreenRect,
														const TRect& aClipRect)
	{
	CVideoPlayerUtility* s = new(ELeave) CVideoPlayerUtility();
	CleanupStack::PushL(s);
	s->iBody = CBody::NewL(s, aObserver, aPriority, aPref, aWs, aScreenDevice,
												aWindow, aScreenRect, aClipRect);
	CleanupStack::Pop();
	return s;
	}

/**
Destructor. Closes any open video clips and frees any resources held by the Video Player.

@since  7.0s
*/
CVideoPlayerUtility::~CVideoPlayerUtility()
	{
	delete iBody;
	}

/**
Opens a video clip from a file.

This function opens a video clip from a file using an optionally specified plugin. If no controller
plugin is specified, this function searches through a list of all available plugins and attempts to 
use each one until successful or the end of the list is reached.

Once the opening of the video clip is complete, successfully or otherwise, the callback function 
MVideoPlayerUtilityObserver::MvpuoOpenComplete() is called.

This function leaves if errors are encountered opening the specified video clip file, or in initialising a 
specified/unspecified controller plugin.

This function can leave with one of the specified error codes. Most open failures
will however be reported through the MVideoPlayerUtilityObserver interface.

From Symbian release v9.5 onwards, this function will enable graphics surface use if the controller supports it. 
If this function is called through an instance of the derived class CVideoPlayerUtility2 and the controller doesn't support graphics surfaces, 
it fails with KErrNotSupported.

@param  aFileName
        The full path name of the file containing the video data.
@param  aControllerUid
        If specified, it will force the video player to use the
		controller with the given UID.

@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::OpenFileL(const TDesC& aFileName, TUid aControllerUid)
	{
	iBody->OpenFileL(aFileName, aControllerUid);
	}

/**
Opens a video clip from a file.

This function opens a video clip from a file using an optionally specified plugin. If no controller
plugin is specified, this function searches through a list of all available plugins and attempts to 
use each one until successful or the end of the list is reached.

Once the opening of the video clip is complete, successfully or otherwise, the callback function 
MVideoPlayerUtilityObserver::MvpuoOpenComplete() is called.

This function leaves if errors are encountered opening the specified video clip file, or in initialising a 
specified/unspecified controller plugin.

This function can leave with one of the specified error codes. Most open failures
will however be reported through the MVideoPlayerUtilityObserver interface.

From Symbian release v9.5 onwards, this function will enable graphics surface use if the controller supports it. 
If this function is called through an instance of the derived class CVideoPlayerUtility2 and the controller doesn't support graphics surfaces, 
it fails with KErrNotSupported.

@param  aFile
        An open handle to a file containing the video clip
@param  aControllerUid
        If specified, it will force the video player to use the
		controller with the given UID.

@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::OpenFileL(const RFile& aFile, TUid aControllerUid)
	{
	iBody->OpenFileL(aFile, aControllerUid);
	}


/**
Opens a video clip from a file.

This function opens a video clip from a file using an optionally specified plugin. If no controller
plugin is specified, this function searches through a list of all available plugins and attempts to 
use each one until successful or the end of the list is reached.

Once the opening of the video clip is complete, successfully or otherwise, the callback function 
MVideoPlayerUtilityObserver::MvpuoOpenComplete() is called.

This function leaves if errors are encountered opening the specified video clip file, or in initialising a 
specified/unspecified controller plugin.

This function can leave with one of the specified error codes. Most open failures
will however be reported through the MVideoPlayerUtilityObserver interface.

From Symbian release v9.5 onwards, this function will enable graphics surface use if the controller supports it. 
If this function is called through an instance of the derived class CVideoPlayerUtility2 and the controller doesn't support graphics surfaces, 
it fails with KErrNotSupported.

@param  aSource
        A filename or an open handle to a file containing the video clip
@param  aControllerUid
        If specified, it will force the video player to use the
		controller with the given UID.
*/
EXPORT_C void CVideoPlayerUtility::OpenFileL(const TMMSource& aSource, TUid aControllerUid)
	{
	iBody->OpenFileL(aSource, aControllerUid);
	}

/**
Opens a video clip contained in a descriptor.

This function opens a video clip contained as binary data in a descriptor using an optionally specified 
plugin. If no controller plugin is specified, this function searches through a list of all available 
plugins and attempts to use each one until successful or the end of the list is reached.

Once the opening of the video clip is complete, successfully or otherwise, the callback function 
MVideoPlayerUtilityObserver::MvpuoOpenComplete() is called.

This function can leave with one of the specified error codes. Most open failures will however be
reported through the interface.

From Symbian release v9.5 onwards, this function will enable graphics surface use if the controller supports it. 
If this function is called through an instance of the derived class CVideoPlayerUtility2 and the controller doesn't support graphics surfaces, 
it fails with KErrNotSupported.

@param  aDescriptor
        The descriptor containing the video clip
@param  aControllerUid
        If specified, it will force the video player to use the
        controller with the given UID.

@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::OpenDesL(const TDesC8& aDescriptor, TUid aControllerUid)
	{
	iBody->OpenDesL(aDescriptor, aControllerUid);
	}

/**
Opens a video clip from a URL.

This function opens a video clip stored at a specified URL and identified by a MIME type. In addition
a plugin can be specified if necessary. If no controller plugin is specified, this function searches 
through a list of all available plugins and attempts to use each one until successful or the end 
of the list is reached.

Once the opening of the video clip is complete, successfully or otherwise, the callback function 
MVideoPlayerUtilityObserver::MvpuoOpenComplete() is called.

This function can leave with one of the specified error codes. Most open failures
will however be reported through the MVideoPlayerUtilityObserver interface.

From Symbian release v9.5 onwards, this function will enable graphics surface use if the controller supports it. 
If this function is called through an instance of the derived class CVideoPlayerUtility2 and the controller doesn't support graphics surfaces, 
it fails with KErrNotSupported.

@param  aUrl
        The URL of the video clip to open.
@param  aIapId
        The Id of the internet access point to use
@param  aMimeType
        The MIME type associated with this video clip. This is
        used to determine the format of the video clip.
@param  aControllerUid
        If specified, it will force the video player to use the controller with the given Uid.

@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::OpenUrlL(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType, TUid aControllerUid)
	{
	iBody->OpenUrlL(aUrl, aIapId, aMimeType, aControllerUid);
	}

/**
Closes the video clip.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::Close()
	{
	iBody->Close();
	}

/**
Begins playback of the initialised video sample at the current volume and priority levels.

When playing of the video sample is complete, successfully or otherwise, the callback function
MVideoPlayerUtilityObserver::MvpuoPlayComplete() is called.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::Play()
	{
	iBody->Play();
	}

/**
Begins playback of the initialised video sample at the specified start and end points.

When playing of the video sample is complete, successfully or otherwise, the callback function
MVideoPlayerUtilityObserver::MvpuoPlayComplete() is called.

@param  aStartTime
        The point at which to start playback.
@param  aEndTime
        The point at which to terminate playback.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::Play(const TTimeIntervalMicroSeconds& aStartTime, const TTimeIntervalMicroSeconds& aEndTime)
	{
	iBody->Play(aStartTime, aEndTime);
	}

/**
Stops playback of the video sample as soon as is possible. 

If the video sample is playing, playback is stopped as soon as possible.

If playback is already complete, this function has no effect.

The position is reset to the beginning of the file.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::Stop()
	{
	return iBody->Stop();
	}

/**
Video playback is paused. The current position is maintained and playback can be
resumed by calling Play.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::PauseL()
	{
	iBody->PauseL();
	}

/**
Sets the playback volume for the audio track of the video clip.

The volume can be changed before or during playback and is effective immediately. The volume can
be set to any value between zero (mute) and the maximum permissible volume
(determined using MaxVolume()).

To determine if the current video clip contains an audio track, use AudioEnabledL().


@param  aVolume
        The volume, between 0 and MaxVolume().

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::SetVolumeL(TInt aVolume)
	{
	iBody->SetVolumeL(aVolume);
	}


//
// Sends custom command to the plugin controller.
// Passes handle and size of the drawable window.
//
/**
Sets the display window. This is used to provide the video controller with an area of
the display to render the current video frame.

From Symbian release 9.5 onwards this method is not supported when called on an instance of CVideoPlayerUtility2, and will always 
leave with KErrNotSupported.


@param  aWs
        The window server session ID.
@param  aScreenDevice
        The software device screen.
@param  aWindow
        The window to display.
@param  aScreenRect
        The dimensions of the display window.
@param  aClipRect
        The area of the video clip to display in the window.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::SetDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice,
													 RWindowBase& aWindow, const TRect& aScreenRect,
													 const TRect& aClipRect)
	{
	iBody->SetDisplayWindowL(aWs, aScreenDevice, aWindow, aScreenRect, aClipRect);
	}

/**
Returns the video frame rate in frames/second.

@return The video frame rate (frames/second).

@since  7.0s
*/
EXPORT_C TReal32 CVideoPlayerUtility::VideoFrameRateL() const
	{
	return iBody->VideoFrameRateL();
	}

/**
Gets the video frame size.

@param  aSize
        The video frame size

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::VideoFrameSizeL(TSize& aSize) const
	{
	iBody->VideoFrameSizeL(aSize);
	}

/**
Returns the current playback volume for the audio track of the video clip.

@return A value between 0 (mute) and the maximum volume returned by MaxVolume().

@since  7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::Volume() const
	{
	return iBody->Volume();
	}

/**
Sets the current playback balance for the audio track of the video clip.

The balance can be any value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight,
the default value being KMMFBalanceCenter.

@param  aBalance
        The balance value to set.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::SetBalanceL(TInt aBalance)
	{
	iBody->SetBalanceL(aBalance);
	}

/**
Returns the current balance setting for the audio track of the video clip.

@return A balance value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight.

@since  7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::Balance() const
	{
	return iBody->Balance();
	}

/**
Returns the video format's MIME type.

@return The video clip's MIME type.

@since  7.0s
@see    NumberOfMetaDataEntriesL
*/
EXPORT_C const TDesC8& CVideoPlayerUtility::VideoFormatMimeType() const
	{
	return iBody->VideoFormatMimeType();
	}

/**
Returns the number of meta data entries associated with this clip.

@return The number of meta data entries.

@since 7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::NumberOfMetaDataEntriesL() const
	{
	return iBody->NumberOfMetaDataEntriesL();
	}

/**
Returns a meta data entry from the clip.

@param  aMetaDataIndex
        The index of the meta data entry to retrieve.

@return The metadata entry requested.

@since  7.0s
@see    NumberOfMetaDataEntriesL
*/
EXPORT_C CMMFMetaDataEntry* CVideoPlayerUtility::MetaDataEntryL(TInt aMetaDataIndex) const
	{
	return iBody->MetaDataEntryL(aMetaDataIndex);
	}

/**
Sets the playback priority. 

This is used to arbitrate between multiple objects simultaneously trying to accesses
the sound hardware.

@param  aPriority
        The Priority Value.
@param  aPref
        The Priority Preference.

@see CVideoPlayerUtility::NewL()

@since 7.0s

*/
EXPORT_C void CVideoPlayerUtility::SetPriorityL(TInt aPriority, TInt aPref)
	{
	iBody->SetPriorityL(aPriority, aPref);
	}

/**
Get the current playback priority. This is used to arbitrate between simultaneous accesses of
the sound hardware.

@param  aPriority
        On return, contains the priority level, EMdaPriorityMin client can be interrupted by any
        other client, EMdaPriorityNormal client can only be interrupted by a client with a higher 
        priority or EMdaPriorityMax client cannot be interrupted by other clients.
@param  aPref
        On return, contains the time and quality preferences, enumerated in TMdaPriorityPreference.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::PriorityL(TInt& aPriority, TMdaPriorityPreference& aPref) const 
	{
	iBody->PriorityL(aPriority, aPref);
	}

/**
Sets the position within the video clip from where to start playback.

@param  aPosition
        Position from start of clip in microseconds.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
	iBody->SetPositionL(aPosition);
	}

/**
Returns the duration of video clip in mircoseconds.

@return The duration of clip in microseconds.

@since  7.0s
*/
EXPORT_C TTimeIntervalMicroSeconds CVideoPlayerUtility::DurationL() const
	{
	return iBody->DurationL();
	}

/**
Returns the current playback position.

@return The current position from the start of the clip in microseconds.

@since  7.0s
*/
EXPORT_C TTimeIntervalMicroSeconds CVideoPlayerUtility::PositionL() const
	{
	return iBody->PositionL();
	}

/**
Returns an integer representing the maximum volume that the audio track can support.

This is the maximum value that can be passed to SetVolumeL(). This value is platform 
independent, but is always greater than or equal to one.

@return The naximum playback volume.

@since  7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::MaxVolume() const
	{
	return iBody->MaxVolume();
	}

/**
Gets the current frame. The current frame is requested, and will be sent to
the client asynchrynously via the MvpuoFrameReady callback.

@param  aDisplayMode
        The display mode for the retrieved frame.

@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::GetFrameL(TDisplayMode aDisplayMode)
	{
	iBody->GetFrameL(aDisplayMode);
	}

/**
Gets the current frame. The current frame is requested, and will be sent to
the client asynchrynously via the MvpuoFrameReady callback.

@param  aDisplayMode
        The display mode for the retrieved frame.
@param  aIntent
        The DRM Intent to pass to the controller.
*/
EXPORT_C void CVideoPlayerUtility::GetFrameL(TDisplayMode aDisplayMode, ContentAccess::TIntent aIntent)
	{
	iBody->GetFrameL(aDisplayMode, ETrue, aIntent);
	}

/**
Returns the video bit rate.

@return The video bit rate in bits/second.

@since  7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::VideoBitRateL() const
	{
	return iBody->VideoBitRateL();
	}

/**
Returns the audio bit rate in bits/second.

@return The audio bit rate (bits/second).

@since  7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::AudioBitRateL() const
	{
	return iBody->AudioBitRateL();
	}

/**
Returns whether the current clip has an audio stream and audio playback
is enabled. This method will return EFalse if the clip does no have an
audio track or if audio playback has been disabled with
SetAudioEnabledL().

@return ETrue if an audio track is present and audio playback is
        enabled, otherwise EFalse.

@since  7.0s
*/
EXPORT_C TBool CVideoPlayerUtility::AudioEnabledL() const
	{
	return iBody->AudioEnabledL();
	}

EXPORT_C void Reserved2()
	{
	// reserved function replacing removed VideoTypeL() method
	// this function should never be called hence generate a
	// panic with code ENoVideoTypeL
	Panic( ENoVideoTypeL );
	}

/**
Returns the codec used for the audio component of the video clip.

@return The four character code representing the audio codec.

@since  7.0s
*/
EXPORT_C TFourCC CVideoPlayerUtility::AudioTypeL() const
	{
	return iBody->AudioTypeL();
	}

/**
Gets the progress of video clip loading/rebuffering.

@param aPercentageProgress
       The percentage of loading/rebuffering complete. The parameter
       is zero if loading has not begun and 100 if loading has already completed.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::GetVideoLoadingProgressL(TInt& aPercentageProgress)
	{
	iBody->GetVideoLoadingProgressL(aPercentageProgress);
	}

/**
Sends a synchronous custom command to the controller.

@param  aDestination
        The destination of the message, consisting of the UID of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client.  Can't be KNullDesC8.

@return The result of the request. The exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	return iBody->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}

/**
Sends a synchronous custom command to the controller.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.

@return The result of the request. The exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CVideoPlayerUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	return iBody->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}

/**
Sends an asynchronous custom command to the controller.

Note: 
This method will return immediately.  The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the
controller framework.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client.  Can't be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object.  This will contain the
        result of the request on completion.  The exact range of
        result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iBody->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}

/**
Send a asynchronous custom command to the controller.

Note: 
This method will return immediately.  The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the
controller framework.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object.  This will contain the
        result of the request on completion.  The exact range of
        result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iBody->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}
	
/**
Register to receive notifications of video clip loading/rebuffering.

When a video clip is being streamed in, it may require rebuffering from time to time. This function 
"switches on" callbacks that report the status of the rebuffering.

The two callbacks that can be sent are MVideoLoadingObserver::MvloLoadingStarted() and 
MVideoLoadingObserver::MvloLoadingComplete().

@param  aCallback
        The object to receive video loading notifications.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::RegisterForVideoLoadingNotification(MVideoLoadingObserver& aCallback)
	{
	iBody->RegisterForVideoLoadingNotification(aCallback);
	}

/**
Sets the video frame rate.

@param  aFramesPerSecond
        The number of frames per second to request.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::SetVideoFrameRateL(TReal32 aFramesPerSecond)
	{
	iBody->SetVideoFrameRateL(aFramesPerSecond);
	}

/**
Requests the video controller to redraw the current frame.

@since 7.0s
*/
EXPORT_C void CVideoPlayerUtility::RefreshFrameL()
	{
	iBody->RefreshFrameL();
	}

/**
Rotates the video image on the screen.

@param  aRotation
        The desired rotation to apply in 90 degree increments.

@see    RotationL
@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::SetRotationL(TVideoRotation aRotation)
	{
	iBody->SetRotationL(aRotation);
	}

/**
Query the rotation that is currently applied to the video image.

@return The applied rotation

@since 7.0s
@see   SetRotationL
*/
EXPORT_C TVideoRotation CVideoPlayerUtility::RotationL() const
	{
	return iBody->RotationL();
	}

/**
Scales the video image to a specified percentage of its original size.

@param  aWidthPercentage
        The percentage (100 = original size) to be used to scale the width of the video image
@param  aHeightPercentage
        The percentage (100 = original size) to be used to scale the height of the video image. 
        If this is not equal to aWidthPercentage then the image may be distorted.
@param  aAntiAliasFiltering
        A boolean specifying if anti-aliasing should be used. True if anti-aliasing filtering 
        should be used.  If the plugin does not	support this kind of processing, 
        this value will be ignored.

@since  7.0s
@see    GetScaleFactorL
*/
EXPORT_C void CVideoPlayerUtility::SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering)
	{
	iBody->SetScaleFactorL(aWidthPercentage, aHeightPercentage, aAntiAliasFiltering);
	}

/**
Gets the scale factor currently applied to the video image.

@param  aWidthPercentage
        On function return, contains the current scaling percentage applied to the width of the
        video image (100 = original size).
@param  aHeightPercentage
        On function return, contains the current scaling percentage applied to the height of the
        of the video image (100 = original size).
@param  aAntiAliasFiltering
        The boolean specifying if anit-aliasing is being used.

@since  7.0s
@see    SetScaleFactorL
*/
EXPORT_C void CVideoPlayerUtility::GetScaleFactorL(TReal32& aWidthPercentage, TReal32& aHeightPercentage, TBool& aAntiAliasFiltering) const
	{
	iBody->GetScaleFactorL(aWidthPercentage, aHeightPercentage, aAntiAliasFiltering);
	}

/**
Selects a region of the video image to be displayed.

@param  aCropRegion
        The dimensions of the crop region, relative to the video image.

@since 7.0s
@see   GetCropRegionL
*/
EXPORT_C void CVideoPlayerUtility::SetCropRegionL(const TRect& aCropRegion)
	{
	iBody->SetCropRegionL(aCropRegion);
	}

/**
Gets the crop region currently applied to the image.

@param  aCropRegion
        The dimensions of the crop region, relative to the video image. If no
        crop region has been applied, the full dimensions of the video image will
        be returned.

@since 7.0s
@see   SetCropRegionL
*/
EXPORT_C void CVideoPlayerUtility::GetCropRegionL(TRect& aCropRegion) const
	{
	iBody->GetCropRegionL(aCropRegion);
	}

/**
Prepares the video clip to be accessed.

A call to this method tells the loaded controller plugin to finalise its configuration
and to prepare to start reading the video clip.  It is not possible to play the video clip
or query any of its properties (e.g. duration, meta data etc.) until this method has signalled
its completion (successful or otherwise) via the callback MvpuoPrepareComplete.

@since  7.0s
*/
EXPORT_C void CVideoPlayerUtility::Prepare()
	{
	iBody->Prepare();
	}

/**
Returns the controller implementation information associated with the current controller.

@return The controller implementation structure associated with the controller.

@since 7.0s
*/
EXPORT_C const CMMFControllerImplementationInformation& CVideoPlayerUtility::ControllerImplementationInformationL()
	{
	return iBody->ControllerImplementationInformationL();
	}

/**
Gets a controller's DRM custom command implementation.

@return A pointer to a controller's DRM custom command implementation, or NULL
if the controller does not support it.
*/
EXPORT_C MMMFDRMCustomCommand* CVideoPlayerUtility::GetDRMCustomCommand()
	{
	return iBody->GetDRMCustomCommand();
	}


/**
Enables a client application to stop the video player from using direct screen access (DSA).

From Symbian release 9.5 onwards this method is not supported when called on an instance of CVideoPlayerUtility2, and will always 
leave with KErrNotSupported.

This function leaves if errors are encountered when trying to stop DSA.
*/
EXPORT_C void CVideoPlayerUtility::StopDirectScreenAccessL()
	{
	iBody->StopDirectScreenAccessL();
	}

/**
Enables a client application to start the video player using direct screen access (DSA).

From Symbian release 9.5 onwards this method is not supported when called on an instance of CVideoPlayerUtility2, and will always 
leave with KErrNotSupported.

This function leaves if errors are encountered when trying to start DSA.
*/
EXPORT_C void CVideoPlayerUtility::StartDirectScreenAccessL()
	{
	iBody->StartDirectScreenAccessL();
	}

/**
Registers for notification, when audio resource is available.

@param	aCallback
      	Observer interface for audio resource notification event
      	
@param 	aNotificationEventId
 		The Event for which the client registers.
 	
@param 	aNotificationRegistrationData
		Data specific to notification registration.This has been reserved for future use and its value should be NULL.
	
@return An error code indicating if the registration is successful. KErrNone on success, 
	otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CVideoPlayerUtility::RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback, TUid aNotificationEventId, const TDesC8& aNotificationRegistrationData)
	{
	return iBody->RegisterAudioResourceNotification(aCallback, aNotificationEventId, aNotificationRegistrationData);	
	}

/**
Cancels the registration for audio resource notification.

@param  aNotificationEventId
	The Event to be cancellled.
	
@return An error code indicating if the cancellation is successful. KErrNone on success, 
	otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CVideoPlayerUtility::CancelRegisterAudioResourceNotification(TUid aNotificationEventId)
	{
	return iBody->CancelRegisterAudioResourceNotification(aNotificationEventId);
	}

/**
Waits for the client to resume the play even after the default timer expires.
Unless the client completes the play no other client gets notification.

@return An error code indicating if the function call is successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CVideoPlayerUtility::WillResumePlay()
	{
	return iBody->WillResumePlay();	
	}

/**
Sets the initial screen to be used for the video display.

From Symbian release 9.5 onwards this method is not supported when called on an instance of CVideoPlayerUtility2, and will always 
leave with KErrNotSupported.

@param  aScreenNumber
        The screen number of the device.

@return KErrNotSupported if the secondary screen display is not supported in Multimedia Framework or 
from Symbian release 9.5 onwards when called on an instance of CVideoPlayerUtility2. KErrNone on success.
*/
EXPORT_C TInt CVideoPlayerUtility::SetInitScreenNumber(TInt aScreenNumber)
	{
	return iBody->SetInitScreenNumber(aScreenNumber);
	}
	
_LIT(KMMFMediaClientVideoPanicCategory, "MediaClientVideo");
GLDEF_C void Panic(TMmfVideoPanic aError)
	{
	User::Panic(KMMFMediaClientVideoPanicCategory, aError);
	}

/**
Set video playback velocity relative to the normal video clip speed.
This method can be used for fast forward, rewind, and slow-motion
playback, depending on the capabilities of the underlying play-controller
implementation and the characteristics of the video clip. Velocity set
in this call will take effect immediately. Additionally,
playback velocity cannot be changed for streamed video. Use
GetPlayRateCapabilitiesL() to determine what playback modes are
available.

Note that due to performance reasons, it may not be possible to perform
fast forward or rewind at the requested speed. If that happens, the
play-controller will use the nearest available velocity. This is not
considered an error and the method will not leave. User can query
velocity in effect by calling PlayVelocityL().

The default playback velocity is 100.
Play velocity is persistent across stop play calls on utility. 
That is SetPlayVelocityL(),call StopL(),then PlayL() will play with 
the velocity set in most recent call. On opening different
file the velocity will default to 100.

This function quits or exits leaving any of the system wide error codes.
Common error codes are listed below.

@param aVelocity Playback velocity as a percentage relative to the
                 normal video clip speed. Use 100 for normal-speed
                 forward playback and negative value for reverse. Values above
                 100 and below 0 correspond to fast forward and
                 rewind respectively, while values 1 to 100 represent
                 slow-motioned playback.

@pre Playback has been started, but may be paused.

@see CVideoPlayerUtility::GetPlayRateCapabilitiesL()

@leave KErrNotSupported The requested playback velocity is not
         supported.
@leave KErrNotReady Playback has not yet been started

@publishedPartner
*/
EXPORT_C void CVideoPlayerUtility::SetPlayVelocityL(TInt aVelocity)
	{
	iBody->SetPlayVelocityL(aVelocity);
	}

/**
Returns the current playback velocity. If setting play velocity is
not supported or velocity is not set this will return default play velocity
100 corresponding to normal playback.

If implementation is not supporting the velocity set with SetPlayVelocityL
it will default to the nearest velocity. In this case this API will return
the value it has defaulted to.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@return The current playback velocity as a percentage relative to the
        normal video clip speed.

@pre Playback has been started, but may be paused.

@see CVideoPlayerUtility::SetPlayVelocityL()

@leave KErrNotReady Playback has not yet been started

@publishedPartner
*/
EXPORT_C TInt CVideoPlayerUtility::PlayVelocityL() const
	{
	return iBody->PlayVelocityL();
	}

/**
Steps the current video playback position forward or backward by a
number of frames. Frame step is only available when playback is paused.

Support for frame stepping may depend on the underlying play-controller
implementation and the video clip open. Additionally, frame step is
typically not available in streamed playback. Use GetPlayRateCapabilitiesL()
to query if frame step is currently possible. Application may check the
value of the current positon after stepping through several frames by calling
PositionL() API.

Implementations may not be able to step exactly the number of frames
requested, especially when stepping backwards. If this happens, the
play-controller will step to a frame close to the one requested. This is
not considered an error.

This function quits or exits leaving any of the system wide error codes.
Common error codes are listed below.

@param aStep The number of frames to step. Use positive values for
             stepping forward and negative values for stepping
             back.

@pre Playback has been started and is currently paused. 
	 That means this API can be called only in paused state.

@see CVideoPlayerUtility::GetPlayRateCapabilitiesL()

@leave KErrNotSupported Frame step is not supported. Note that some
         implementations may support step forward but not step backward.
@leave KErrNotReady Playback has not yet been started or is not in paused state.

@publishedPartner
*/
EXPORT_C void CVideoPlayerUtility::StepFrameL(TInt aStep)
	{
	iBody->StepFrameL(aStep);
	}

/**
Queries the current playback rate capabilities. The capabilities
describe whether fast forward, reverse playback, slow motion, or
step backward or forward is possible. The capabilities may depend on
the controller implementation and the video clip. Additionally,
only normal-speed forward playback is typically possible in streamed playback.

This function quits or exits leaving any of the system wide error codes.
Common error codes are listed below.

@param aCapabilities Playback rate capabilities

@pre The video clip has been opened

@leave KErrNotReady The video clip has not yet been opened

@publishedPartner
*/
EXPORT_C void CVideoPlayerUtility::GetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCapabilities)const
	{
	iBody->GetPlayRateCapabilitiesL(aCapabilities);
	}

/**
Enables or disables video playback. This function is used to disable
video playback in a video clip to play only the audio track. By default
video playback is always enabled if the clip has a video track.

This method can be called after the video clip has been opened, but
only before calling Prepare().

This function quits or exits leaving any of the system wide error codes.
Common error codes are listed below.

@param aVideoEnabled ETrue to enable video playback, EFalse to disable

@pre The video clip has been opened by the client and Prepare() has not been called.

@leave KErrNotSupported This method is not supported -- video playback
         is always enabled. 
@leave KErrNotReady The clip has not been opened or the client has
         already called Prepare().

@publishedPartner
*/
EXPORT_C void CVideoPlayerUtility::SetVideoEnabledL(TBool aVideoEnabled)
	{
	iBody->SetVideoEnabledL(aVideoEnabled);
	}

/**
Queries whether video playback is currently enabled or not.
If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@return ETrue if video playback is enabled, EFalse if not.

@publishedPartner
*/
EXPORT_C TBool CVideoPlayerUtility::VideoEnabledL() const
	{
	return iBody->VideoEnabledL();
	}

/**
Enables or disables audio playback. This function is used to disable
audio playback in a video clip to play only the video track. By default
audio playback is always enabled if the clip has an audio track.

This method can be called after the video clip has been opened, but
only before calling Prepare().

This function quits or exits leaving any of the system wide error codes.
Common error codes are listed below.

@param aAudioEnabled ETrue to enable audio playback, EFalse to disable

@pre The video clip has been opened by the client and Prepare() has not been called.

@leave KErrNotSupported This method is not supported -- audio playback
         is always enabled
@leave KErrNotReady The clip has not been opened or the client has
         already called Prepare().

@publishedPartner
*/
EXPORT_C void CVideoPlayerUtility::SetAudioEnabledL(TBool aAudioEnabled)
	{
	iBody->SetAudioEnabledL(aAudioEnabled);
	}

/** 
Set video automatic scaling. When automatic scaling is active, the
video picture is scaled automatically to match the video window,
based on the scaling type. This variant of SetAutoScaleL() will
always center the picture in the window.

Calling SetAutoScaleL() will override any scaling factors set with
SetScaleFactorL(). Calling SetScaleFactorL() will disable automatic
scaling.

Not all video controllers support automatic scaling. 

This function quits or exits leaving any of the system wide error codes.
Common error codes are listed below.

@see TAutoScaleType, THorizontalAlign, TVerticalAlign

@param aScaleType Automatic scaling type

@pre The video clip has been opened by the client

@leave KErrNotSupported Automatic scaling is not supported
*/
EXPORT_C void CVideoPlayerUtility::SetAutoScaleL(TAutoScaleType aScaleType)
	{
	iBody->SetAutoScaleL(aScaleType);
	}

/** 
Set video automatic scaling. When automatic scaling is active, the
video picture is scaled automatically to match the video window,
based on the scaling type, and positioned according to the
parameters.

Calling SetAutoScaleL() will override any scaling factors set with
SetScaleFactorL(). Calling SetScaleFactorL() will disable automatic
scaling.

Not all video controller support automatic scaling. 

This function quits or exits leaving any of the system wide error codes.
Common error codes are listed below.

@see TAutoScaleType, THorizontalAlign, TVerticalAlign

@param aScaleType Automatic scaling type
@param aHorizPos Video picture horizontal position, relative to the
                 video window. The value can be either a pixel offset
                 (positive or negative) from the top left corner of the
                 window to the top left corner of the picture, or an
                 alignment constant from enum THorizontalAlign.
@param aVertPos Video picture vertical position, relative to the
                 video window. The value can be either a pixel offset
                 (positive or negative) from the top left corner of the
                 window to the top left corner of the picture, or an
                 alignment constant from enum TVerticalAlign.

@pre The video clip has been opened by the client.

@leave KErrNotSupported Automatic scaling is not supported
*/
EXPORT_C void CVideoPlayerUtility::SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
	iBody->SetAutoScaleL(aScaleType, aHorizPos, aVertPos );
	}

/**
When enabled sets automatic switching of surface to/from external display when it is connected/disconnected from the device.

Automatic switching is enabled by default, but only if the client thread that created this utility has an Active Scheduler
installed and the device supports external display switching.

To use this function the client thread must have an Active Scheduler installed otherwise it will leave with KErrNotReady.

@param  aControl
        ETrue to enable. EFalse to disable.
@param  aDisplay
        Display id of display to enable external switching for.
@leave  KErrNotSupported Device does not support external displays
@leave  KErrNotReady CActiveScheduler is not installed
*/
EXPORT_C void CVideoPlayerUtility::SetExternalDisplaySwitchingL(TInt aDisplay, TBool aControl)
    {
    iBody->SetExternalDisplaySwitchingL(aDisplay, aControl);
    }
