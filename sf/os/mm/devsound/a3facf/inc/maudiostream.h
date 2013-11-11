/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @publishedPartner
 @released
*/

#ifndef MAUDIOSTREAM_H
#define MAUDIOSTREAM_H

#include <e32base.h>
#include <f32file.h>
#include <mmf/common/mmfcontroller.h>
#include <a3f/a3fbase.h>

class MAudioStreamObserver;
class MAudioCodecObserver;
class MAudioCodec;
class MAudioProcessingUnit;

/**
 * this is the main control interface for the state of a processing unit chain.
 *
 * An AudioStream object binds together audio components processing the same data.
 * An AudioStrem executes the audio processing states as requested by the client. The processing units
 * attached to a stream will follow the state of the stream.
 *
 * The possible states of an AudioStream object are:
 * EUninitialized: The state which the chain is in initially. 
 *		  The logical chain can be constructed.  
 *		  The settings in the logical chain cannot be related to the adaptation because no adaptation has been selected yet. 
 *
 * EInitialized: this state is set after a successful initialization request.
 * EIdle: All the chain resources are allocated, however, no processing time 
 *		(other than expended to put the chain into EIdle state) is consumed in this phase.
 * EPrimed: As per EIdle, but the stream can consume processing time, by filling its buffers.  
 * EActive: The chain has all the resources as per EIdle and EPrimed but also has started to 
 *	   process the actions requested by the client.  
 * EDead:   The stream can no longer function due to fatal error.
 *	   The logical chain still exists, but any physical resources should be reclaimed by the adaptation. 
 *
 * The MAudioStreamObserver includes completion callbacks for many of the methods of MAudioStream.
 * The protocol is such that when an asynchronous method is called and KErrNone is returned,
 * then the call will be followed by a corresponding callback. if the returned error code from
 * the asynchronous function is other than KErrNone, then a callback will not be made as
 * a result of that method call. The callbacks will always occur out-of-context.
 * Note that many of the functions will also require a context commit for the actual processing to take place. 
 */
class MAudioStream 
	{
public:
	/**
	 * Registers an audio stream observer.
	 *
	 * Note that this function is meant for clients of the Audio Stream. 
	 * The client should unregister using UnregisterAudioStreamObserver() when applicable.
	 *
	 * @param aObserver reference to the observer to register.
	 * @return an error code. KErrNone if successful.
	 *			  			  KErrAlreadyExists if the client is already registered.
	 *						  KErrOutOfMemory in case of memory exhaustion.
	 */
	virtual TInt RegisterAudioStreamObserver(MAudioStreamObserver& aObserver)=0;

	/**
	 * Unregisters an audio stream observer.
	 *
	 * @param aObserver reference to the observer to unregister.
	 * @return an error code.  KErrNone if successful. KErrNotFound if the client has not been registered as observer.
	 */
	virtual void UnregisterAudioStreamObserver(MAudioStreamObserver& aObserver)=0;


	/**
	 * Sets the type of audio being processed in this context.
	 *
	 * The audio type settings are used by the resource control subsystem when it arbitrates between multiple clients 
	 * trying to access the same resource. Additionally it may impact the availability of certain audio features for the client. 
	 * It may also have an effect on other aspects, including, but not limited to routing and automatically applied effects. 
	 * The audio types available for the client may be limited by the platform security capabilities, vendorId etc of the client application.
	 *
	 * @param aAudioTypeSettings defines the settings associated with this stream.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual TInt SetAudioType(const TAudioTypeSettings& aAudioTypeSettings)=0;

	/**
	 * Fetch the type of audio being processed in this stream.
	 *
	 * @param aAudioTypeSettings On return contains the audio type of this stream.
	 */
	virtual void GetAudioType(TAudioTypeSettings& aAudioTypeSettings) const = 0;

	/**
	 * Requests a transition to the EInitialized state. 
	 *
	 * this function is asynchronous, and its successful return merely issues a request which is acted upon on Commit(), 
	 * and while will lead to a StateEvent() callback. this function is only allowed while the stream is in EUninitialized state. 
	 * A call in any other state will cause an error code of KErrNotReady to be returned.
	 *
	 * @return an error code. if KErrNone, the state transition has been requested, 
	 * and there will be a StateEvent() callback following Commit(). 
	 * On error, there is no request made: KErrNotReady states the stream if not in valid current state, 
	 * otherwise one of the system-wide error codes.
	*/
	virtual TInt Initialize()=0;

	/**
	 * Requests a transition to the EIdle state. 
	 *
	 * this function is asynchronous, and its successful return merely issues a request which is acted upon on Commit(), 
	 * and while will lead to a StateEvent() callback. this function is only allowed while the stream is in EInitialized state. 
	 * A call in any other state will cause an error code of KErrNotReady to be returned
	 *
	 * The resources must be released using Unload() when they are no longer needed. 
	 * Note that it is recommended that the resources are not kept reserved for long periods of inactivity, 
	 * since it may prevent other audio streams from becoming active by stealing resources they need to use
	 *
	 * @return An error code. if KErrNone, the state transition has been requested, 
	 * and there will be a StateEvent() callback following Commit(). 
	 * On error, there is no request made: KErrNotReady states the stream if not in valid current state, 
	 * otherwise one of the system-wide error codes.
	 */
	virtual TInt Load()=0;

	/**
	 * Starts the audio processing. this function is only allowed while the stream is in EIdle or EPrimed states. 
	 *
	 * The start-up procedures include requesting permission for audio processing from audio policy. 
	 *
	 * @return an error code. A call in any state different to EIdle or EPrimed will cause an error code of KErrNotReady.
	 * if permission to start is denied by audio policy, a state change back to EPrimed will occur with error code KErrAccessDenied
	*/
	virtual TInt Activate()=0;

	/**
	 * Requests a transition to the Idle state – stopping the ongoing operation. 
	 *
	 * this function is asynchronous, and its successful return merely issues a request which is acted upon on Commit(), 
	 * and while will lead to a StateEvent() callback this function is only allowed while the stream is either Active or Primed. 
	 * A call in any other state will cause an error code of KErrNotReady to be returned. 	
	 * @return An error code. if KErrNone, the state transition has been requested, and there will be a StateEvent() callback 
	 * following Commit(). On error, there is no request made: KErrNotReady states the stream if not in valid current state, 
	 * otherwise one of the system-wide error codes
	*/
	virtual TInt Stop()=0;

	/**
	 * Prepares the audio stream for releasing the resources reserved for it and for the processing units attached to the stream.
	 *
 	 * this function is only allowed while the stream is EIdle state.
	 * A call in any other state will cause an error code of KErrNotReady to be returned.
	 *
	 * During this call the stream will query all attached processing units whether they are ready to unload.
	 * if a processing unit indicates that it is not ready to uninitialize, then an error code other than KErrNone will be returned.
	 * Provided that this call succeeds, the actual unloading process will begins once Commit() is called for the context of this stream.
	 *
	 * @return An error code. KErrNone if successful.
	 *			  KErrNotReady if called while the stream is not in correct state.
	 *			  Other system wide error codes are also possible.
	 */
	virtual TInt Unload()=0;

	/**
	 * Requests a transition to the Unitialized state – returning the stream to its initial state.
	 *
	 * this function is asynchronous, and its successful return merely issues a request which is acted upon on Commit(), 
	 * and while will lead to a StateEvent() callback. this function is only allowed while the stream is in Initialized state. 
	 * A call in any other state will cause an error code of KErrNotReady to be returned.
	 *
	 * @return an error code. if KErrNone, the state transition has been requested, and there will be a StateEvent() callback 
	 * following Commit(). On error, there is no request made: KErrNotReady states the stream if not in valid current state, 
	 * otherwise one of the system-wide error codes.
	 */
	virtual TInt Uninitialize()=0;

	/**
	 * Requests a transition to the EPrime state. 
	 * 
	 * this function is asynchronous, and its successful return merely issues a request which is acted upon on Commit(), 
	 * and while will lead to a StateEvent() callback. this function is only allowed while the stream is in EActive 
	 * (semantics of Prime in EActive– go into a buffering pause) or EIdle state (semantics of Prime in EIdle – start to buffer). 
	 * A call in any other state will cause an error code of KErrNotReady to be returned.  Not all streams support Prime.
	 *
	 * @return an error code. if KErrNone, the state transition has been requested, and there will be a StateEvent() callback 
	 * following Commit(). On error, there is no request made: KErrNotReady states the stream if not in valid current state, 
	 * otherwise one of the system-wide error codes
	 */
	virtual TInt Prime()=0;


	/**
	 * Empty any buffers that have been filled. 
	 *
	 * Reclain any outstanding buffers from the client, if required. 
	 * this function is only allowed while the stream is in Idle state. this is an asynchronous operation. 
	 * if the call returns KErrNone it merely indicates a flush has been requested and there will be a subsequent FlushComplete() callback. 
	 * Note that this does not relate to the Commit() cycle, and is not considered transactional.
	 *
	 * @return a system wide error code. KErrNone if successful.
	 * KErrNotReady if called while the stream is not in correct state. Other system wide error codes are also possible.
	 */
	virtual TInt Flush()=0;

	/**
	 * Adds an audio source to this stream.
	 *
	 * A stream cannot operate before a source has been set.
	 * The source must be removed from the stream before the sink or the stream is destroyed.
	 * The source can only be set while the stream is in uninitialized state.
	 * this is an asynchronous call and the completion will be indicated to the
	 * client via a callback in the stream observer interface.
 	 *
	 * @param aSource the audio source for this stream.
	 * @return an error code. KErrNone if successful, there will be a subsequent callback on MAudioStreamObserver::AddProcessingUnitComplete().
	 * 			KErrInUse a source has already been set fot this stream.
	 *		 	KErrNotReady the stream is not in correct state.
	 */ 
	virtual TInt AddSource(MAudioProcessingUnit* aSource)=0;

	/**
	 * Adds sink for this stream.
	 *
	 * Stream cannot operate before source and sink have been set.	
	 * The sink must be removed from the stream before it is destroyed.
 	 * The sink can only be set while the stream is in uninitialized state.
	 * this is an asynchronous call and the completion will be indicated to the
	 * client via a callback in the stream observer interface.
	 *
	 * @param aSink the sink for this stream
	 * @return an error code. KErrNone if successful, there will be a subsequent callback on MAudioStreamObserver::AddProcessingUnitComplete().
	 * 		          		  KErrInUse a sink has already been set fot this stream.
	 *				  	  KErrNotReady the stream is not in correct state,
	*/
	virtual TInt AddSink(MAudioProcessingUnit* aSink)=0;

	/**
	 * Adds an codec for this stream.
	 *
	 * The codec must be removed from the stream before the codec or the stream
	 * is deleted. The codec can only be set while the stream is in uninitialized state.
	 * this is an asynchronous call and the completion will be indicated to
	 * the client via a callback in the stream observer interface.
	 *
	 * @param aCodec the codec for this stream
	 * @return an error code. KErrNone if successful, there will be a subsequent callback on MAudioStreamObserver::AddProcessingUnitComplete().
	 *		         		  KErrInUse a codec has already been set fot this stream.
	 *               		  KErrNotReady the stream is not in correct state.
	 */
	virtual TInt AddAudioCodec(MAudioProcessingUnit* aCodec)=0;

	/**
	 * Adds an audio processing unit to this stream. 
	 *
	 * The processing unit must be removed from the stream before the processing unit or the stream is removed from the context. 
	 * The supplied component must implement the AudioGainControl feature. 
	 * 
	 * @param aGainControl  the gain control to add to this stream.
	 * @return KErrNone if succesful returned, there will be a subsequent callback on MAudioStreamObserver::AddProcessingUnitComplete().
	 *	       KErrInUse a gain control has already been set fot this stream.
	 *	       KErrNotReady the stream is not in correct state.
	 */
	virtual TInt AddGainControl(MAudioProcessingUnit* aGainControl)=0;

	/**
	 * Removes any audio processing unit from this stream, whether it has been added by AddSource(), AddSink(), AddCodec() or AddGainControl().
	 * this is an asynchronous function, on successful call there will be a subsequent RemoveProcessingUnitComplete() callback.
	 * @param aProcessingUnit  the processing unit to remove.
	 * @return a system wide error code, KErrNone if successful, or the processing unit had not been added to the stream. 
	 * KErrNotReady the decoder is not in correct state. Other system wide error codes are also possible. 
	 */
	virtual TInt RemoveProcessingUnit(MAudioProcessingUnit* aProcessingUnit)=0;

	/**
	 * Resets the duration counter. 
	 *
	 * Used to differentiate between when a stream has been momentarily interrupted and when 
	 * a different conceptual use-case is underway.  Defined for EIdle.
	 */
	virtual TInt ResetStreamTime()=0;

	/**
	 * Returns the duration of audio that has been processed from the hardware device perspective. 
	 * The time is reset when the MAudioStream object transfers from Initialized state to Idle. 
	 * as a result of calling Load(). It can be reset at other times using ResetStreamTime().
	 */
	virtual TInt GetStreamTime(TTimeIntervalMicroSeconds& aStreamTime)=0;

	/**
	 * A mechanism to obtain additional control APIs on the stream.  
	 *
	 * These can be used to extend the basic play/record, pause, stop with future use-cases such as seek play.	
	 *
	 * @param aType Uid that denotes the type of the interface.
	 * @return the pointer to the specified interface, or NULL if it does not exist. Must be cast to correct type by the user..
	 */
	virtual TAny* Interface(TUid aType)=0;

	/**
	 * Returns the component instance id.
	 *
	 * @return component instance id.
	 */
	virtual TAudioStreamId InstanceId() const =0;

	};

#endif // MAUDIOSTREAM_H
