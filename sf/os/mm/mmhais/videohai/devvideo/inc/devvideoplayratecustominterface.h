// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DEVVIDEOPLAYRATECUSTOMINTERFACE_H
#define DEVVIDEOPLAYRATECUSTOMINTERFACE_H

#include <mmf/common/mmfvideo.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfvideoenums.h>
#endif

/**
MMmfVideoPlayRateControl interface UID.

@publishedPartner
@released
*/
const TUid KUidMmfVideoPlayRateControl = { 0x10285D3C };

/**
Video play rate observer that is used with the MMmfVideoPlayRateControl 
custom interface. Controllers or other DevVideo clients using that custom
interface must implement this observer interface.

@publishedPartner
@released
*/
class MMmfVideoPlayRateObserver
	{
public:
	/**
	Indicates that a step frame request has been completed. This callback is a 
	response to an MmvprcStepFrameL() call.

	@param aTimestamp The timestamp for the frame that was actually rendered as
			a result of the frame step operation
	*/
	virtual void MmvproStepFrameComplete(TTimeIntervalMicroSeconds aTimestamp) = 0;

	/**
	Requests that the controller should only pass key frames to DevVideo.
	The key frame mode will be in operation until normal playback is resumed.
	*/
	virtual void MmvproKeyFrameModeRequest() = 0;
};

/**
Video play rate control custom interface. Controllers or other DevVideo clients
can use this interface for trick play control.

Note that both decoders and post-processors can implement this interface. Clients 
must query both the decoder and the post-processor (if in use) for the custom 
interface, and call methods on both if they implement the interface. At least 
the rendering media device (typically the post-processor) must implement this 
interface for playback rate control to be available.

Additionally, the timestamp returned from the clock source must reflect the 
playback speed. For example, for 200% fast forward, the timestamps returned must 
increase at two times the normal speed.

@publishedPartner
@released
*/
class MMmfVideoPlayRateControl
	{
public:
	/**
	Queries the current video playback rate capabilities. The capabilities
	describe whether fast forward, reverse playback, slow motion, or
	step backward or forward is possible. The capabilities may depend on
	the video decoder / post processor implementation and the video stream. 

	If an error occurs, this function leaves with any of the system wide error codes.

	@param aCapabilities Playback rate capabilities
	*/
	virtual void MmvprcGetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCapabilities) = 0;

	/**
	Set video playback rate relative to the normal video stream speed.
	This method can be used for fast forward, rewind, and slow-motion
	playback, depending on the capabilities of the underlying video 
	decoder / post processor implementation and the characteristics of 
	the video stream. Rate set in this call will take effect immediately. 
	Use	MmvprcGetPlayRateCapabilitiesL() to determine what playback modes 
	are	available.

	Note that due to performance reasons, it may not be possible to perform
	fast forward or rewind at the requested speed. If that happens, the
	video decoder / post processor will use the nearest available rate. This is not
	considered an error and the method will not leave. DevVideo clients can query
	rate in effect by calling MmvprcPlayRateL().

	The default playback rate is 100.
	Play rate is persistent across stop start calls on DevVideo. 

	If an error occurs, this function leaves with any of the system wide error codes.
	Common error codes are listed below.

	@param aRate Playback rate as a percentage relative to the
                 normal video stream speed. Use 100 for normal-speed
                 forward playback and negative value for reverse. Values above
                 100 and below 0 correspond to fast forward and
                 rewind respectively, while values 1 to 100 represent
                 slow-motioned playback.

	@pre Playback has been started, but may be paused.

	@see MMmfVideoPlayRateControl::MmvprcGetPlayRateCapabilitiesL()

	@leave KErrNotSupported The requested playback rate is not supported.
	@leave KErrNotReady Playback has not yet been started
	*/
	virtual void MmvprcSetPlayRateL(const TInt aRate) = 0;

	/**
	Returns the current playback rate. If setting play rate is
	not supported or rate is not set this will return default play rate
	100 corresponding to normal playback.

	If implementation is not supporting the rate set with MmvprcSetPlayRateL
	it will default to the nearest rate. In this case this API will return
	the value it has defaulted to.

	If an error occurs, this function leaves with any of the system
	wide error codes. Common error codes are listed below.

	@return The current playback rate as a percentage relative to the
        	normal video stream speed.

	@pre Playback has been started, but may be paused.

	@see MMmfVideoPlayRateControl::MmvprcGetPlayRateCapabilitiesL()
	@see MMmfVideoPlayRateControl::MmvprcSetPlayRateL()

	@leave KErrNotReady Playback has not yet been started
	*/
	virtual TInt MmvprcPlayRateL() = 0;

	/**
	Steps the current video playback position forward or backward by a
	number of frames. Frame step is only available when playback is paused.

	Support for frame stepping may depend on the underlying video decoder / 
	post processor implementation and the video stream open. Use 
	MmvprcGetPlayRateCapabilitiesL() to query if frame step is currently possible. 

	Implementations may not be able to step exactly the number of frames
	requested, especially when stepping backwards. If this happens, the
	video decoder / post processor will step to a frame close to the one requested. 
	This is	not considered an error.

	If an error occurs, this function leaves with any of the system wide error codes.
	Common error codes are listed below.

	@param aStep The number of frames to step. Use positive values for
             stepping forward and negative values for stepping back.

	@pre Playback has been started and is currently paused. That means this API 
			can be called only in paused state.

	@see MMmfVideoPlayRateControl::MmvprcGetPlayRateCapabilitiesL()

	@leave KErrNotSupported Frame step is not supported. Note that some
         	implementations may support step forward but not step backward.
	@leave KErrNotReady Playback has not yet been started or is not in paused state.
	*/
	virtual void MmvprcStepFrameL(const TInt aStep) = 0;

	/**
	Sets a play rate observer

	@param aObserver Play rate observer
	*/
	virtual void MmvprcSetObserver(MMmfVideoPlayRateObserver& aObserver) = 0;
	};

#endif // DEVVIDEOPLAYRATECUSTOMINTERFACE_H
