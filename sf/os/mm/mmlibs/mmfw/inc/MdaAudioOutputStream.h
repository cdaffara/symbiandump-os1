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

#ifndef __MDAAUDIOOUTPUTSTREAM_H
#define __MDAAUDIOOUTPUTSTREAM_H


#include <e32std.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mda/common/base.h>      
#include <mda/client/utility.h>
#include <mmf/common/mmfaudio.h>
#include <mmfclntutility.h>


/**
@publishedAll
@released

An interface class that notifies the client of the progress of audio output streaming.

It must be implemented by users of the CMdaAudioOutputStream class.

An object that implements this interface is passed to CMdaAudioOutputStream::NewL().
*/
class MMdaAudioOutputStreamCallback 
	{
public:

	/**
	A callback function that is called when CMdaAudioOutputStream::Open() has completed, indicating that the
	audio output stream is ready for use.

	@param  aError
	        An error value which indicates if open was completed successfully. KErrNone if succeeded.
	*/
	virtual void MaoscOpenComplete(TInt aError) = 0;

	/**
	A callback function that is called when a descriptor has been copied to the lower layers of MMF. 

	It is also called when an error has occurred or when the client has stopped the stream playing before the descriptor 
	has been fully copied (by calling CMdaAudioOutputStream::Stop()).

	This function indicates to the client that it is safe to destroy the buffer passed to CMdaAudioOutputStream::WriteL(). 
	It can also be used to make the next call to WriteL().

	@param  aError
	        KErrNone if the copy succeeded, otherwise one of the system error codes. KErrAbort indicates that 
	        the client has stopped the stream playing before the descriptor has been copied.
	@param  aBuffer
	        A reference to the buffer that has been copied.
	*/
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer) = 0;

	/**
	A callback function that is called when playback terminates as a result of a CMdaAudioOutputStream::Stop().

	If the end of the sound data has been reached, the function returns KErrUnderFlow. If playback terminates for any 
	other reason, the function returns an appropriate error value.

	@param  aError
	        An error value which indicates play success or not. KErrNone if the close succeeded, otherwise one of the
	        system error codes.
	*/
	virtual void MaoscPlayComplete(TInt aError) = 0;
	};


class CMMFMdaAudioOutputStream;

/**
@publishedAll
@released

The interface to an audio stream player passing raw audio data from specified buffers to the audio hardware.

This class enables MMF clients to:

Stream raw audio data to the audio hardware from specified buffers

Specify the priority of the audio stream in relation to other clients that may request to use the same audio hardware

Set the sample rate and the number of channels after successfully opening the stream. It is not possible to change these 
values once  streaming has started.

Change the volume and balance before or while the stream is open for writing. Volume and balance settings take effect immediately.

The API supports callbacks from the server to notify the client:

MaoscOpenComplete() will be called when the audio streaming object is open and ready to stream data back to the
audio hardware as a result of a previous call to Open().

MaoscBufferCopied() will be called each time audio data has been successfully copied to the lower layers of the
MMF as a result of a previous WriteL().

MaoscPlayComplete() will be called when the audio data stream has been closed as a result of a previous Stop().
*/
class CMdaAudioOutputStream : public CBase,
							  public MMMFClientUtility
	{
public:

	/**
	Allocates and constructs an audio stream player object.

	@param  aCallBack
	        A callback to notify the client when the sound device is open and ready to receive data, when 
	        each descriptor has been copied and when the stream is closed. The caller must create a 
	        callback class which implements this interface.
	@param  aServer
	        A pointer to a CMdaServer. CMdaServer is deprecated and as such this parameter is only provided 
	        for backward compatibility.

	@return A pointer to new audio stream player.
	*/
	IMPORT_C static CMdaAudioOutputStream* NewL(MMdaAudioOutputStreamCallback& aCallBack,
												CMdaServer* aServer = NULL);

	/**
	Constructs and initialises a new instance of an audio streaming object.

	The function leaves if the audio streaming object cannot be created.

	@param  aCallBack
	        A callback to notify the client when the sound device is open and ready to receive data, when each
	        descriptor has been copied and when the stream is closed. The caller must create a callback class
	        which implements this interface.
    @param  aPriority
            The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
            EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
    @param  aPref
            The Priority Preference - an additional audio policy parameter. The suggested default is 
            EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
            values may be supported by given phones and/or platforms, but should not be depended upon by 
            portable code.

	@return A pointer to CMdaAudioOutputStream.

    Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
    several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
    the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
    Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
    vary between different phones. Portable applications are advised not to assume any specific behaviour. 
	*/
	IMPORT_C static CMdaAudioOutputStream* NewL(MMdaAudioOutputStreamCallback& aCallBack,
												TInt aPriority,
												TInt aPref = EMdaPriorityPreferenceTimeAndQuality);


	/**
    Destructor.

	Frees all resources owned by the object prior to its destruction.
	*/
	~CMdaAudioOutputStream();

	/**
	Sets the sample rate and number of audio channels.

	@param  aSampleRate
	        The new sample rate. For possible values, see the TAudioCaps enum in class TMdaAudioDataSettings.
	@param  aChannels
	        The new number of channels. For possible values, see the TAudioCaps enum in class TMdaAudioDataSettings.
	
	*/
	virtual void SetAudioPropertiesL(TInt aSampleRate, TInt aChannels);

	/**
	Opens an output audio stream package.

	The MMdaAudioOutputStreamCallback::MaoscOpenComplete() callback function is called when the stream has been 
	opened and is ready to receive audio data. If the open was unsuccessful, this is indicated by the aError 
	parameter of the callback.


	@param  aSettings
	        A pointer to a TMdaPackage object.
	
	*/
	virtual void Open(TMdaPackage* aSettings);

	/**
	Returns the maximum volume level.

	@return The maximum volume level supported by the sound device, as an integer.
	*/
	virtual TInt MaxVolume();

	/**
	Returns the current volume.

	@return The current volume as an integer.
	*/
	virtual TInt Volume();

	/**
	Sets the audio volume.

	Set the volume to zero for "sound off" or any other value between 1 and MaxVolume().

	@param  aNewVolume
	        A specified audio volume.

	*/
	virtual void SetVolume(const TInt aNewVolume);

	/**
	Sets the audio priority values.

	This function cannot be used while the stream object is open. It is intended for use before an Open() 
	is issued, or between a previous Stop() and a new Open().

	@param  aPriority
	        The Priority Value.
	@param  aPref
	        The Priority Preference.

    @see CMdaAudioOutputStream::NewL()
	*/
	virtual void SetPriority(TInt aPriority, TInt aPref);
	
	/**
	Writes (plays) streaming raw audio data.

	This function is asynchronous. When aData has been received, the client is notified by a call to 
	MMdaAudioOutputStreamCallback::MaoscBufferCopied(). The client can call WriteL() again before this notification 
	takes place because the buffers are maintained in a client-side queue until they have been sent. An active object 
	performs the notification, and copies the next item in the queue to the server.
	MMdaAudioOutputStreamCallback::MaoscPlayComplete() is called when all descriptors have been sent.

	@param  aData
	        A reference to the stream data.

	*/
	virtual void WriteL(const TDesC8& aData);

	/**
	Stops writing data to a stream.

	*/
	virtual void Stop();

	/**
	Pause data rendering by audio hardware.
	@return  An error code indicating if the operation was successful. KErrNone on success, 
		KErrNotReady if not streaming
		KErrNotSupported if trying to pause when resume is not supported by DevSound
	*/

	IMPORT_C TInt Pause();


	/**
	Resume data rendering by audio hardware.
	@return  An error code indicating if the operation was successful. KErrNone on success, 
		KErrNotReady if not paused. 
		KErrNotSupported if trying to resume when resume is not supported by DevSound
	*/
	IMPORT_C TInt Resume();


	/**
	Returns the current position within the data stream.

	@return The current position within the stream in microseconds.
	*/
	virtual const TTimeIntervalMicroSeconds& Position();

	/**
	Sets the audio balance.

	@param  aBalance
	        A specified balance volume. Default is KMMFBalanceCenter.

	*/
	IMPORT_C void SetBalanceL(TInt aBalance = KMMFBalanceCenter);
	
	/**
	Returns the current balance as an integer.

	@return The current balance value as integer.
	*/
	IMPORT_C TInt GetBalanceL() const;

	/**
	Returns the current number of bytes rendered by audio hardware.

	@return The current current number of bytes rendered by audio hardware as an integer.
	*/
	IMPORT_C TInt GetBytes();

	/**
    Sets the data type.  If the data type is not explicitly set it will assumed to be pcm16.
    If it is set then the hardware must support the data type being set otherwise the 
    function leaves with KErrNotSupported.
    
    @param	aAudioType The fourCC code used to specify the data type of the streamed audio

	@leave KErrNotSupported
	       Leaves with this for any unsuported data type.
	*/
    IMPORT_C void SetDataTypeL(TFourCC aAudioType);

	/**
	Returns the current data type.

	@return The ID of the data type.
	*/
	IMPORT_C TFourCC DataType() const;
	
	/**
	Registers the Event for Notification when resource is avaliable.
	@param	aCallback
	      	The audio outputstream observer interface..
	@param 	aNotificationEventUid
	 	The Event for which the client is registered.
	@param 	aNotificationRegistrationData
		Notification registration specific data.
	@return An error code indicating if the registration was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
	*/
	IMPORT_C TInt RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData = KNullDesC8);
	
	/**
	Cancels the registered notification event.
	@param  aNotificationEventUid
		The Event to notify the client.
		
	@return An error code indicating if the registration was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
	*/
	IMPORT_C TInt CancelRegisterAudioResourceNotification(TUid aNotificationEventId);
    
    	/**
	Waits for the client to resume the play even after the default timer expires.
	@return An error code indicating if the registration was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
	*/
	IMPORT_C TInt WillResumePlay();
	
	/**
	Retrieves a custom interface to the underlying device.

	@param  aInterfaceId
	        The interface UID, defined with the custom interface.
	
	@return A pointer to the interface implementation, or NULL if the device does not
	        implement the interface requested. The return value must be cast to the
	        correct type by the user.
	*/
	IMPORT_C TAny* CustomInterface(TUid aInterfaceId);
	
	/**
	When this method is called, AudioOutputStream goes into a different mode wherein it does not call 
	MaoscPlayComplete() with KErrUnderflow when all the supplied data has been played. Instead client 
	should signal the end of the play by calling RequestStop() or Stop() on AudioOutputStream.
	If the client calls RequestStop(), AudioOutputStream waits until all the queued data has been played out
	and then calls MaoscPlayComplete(). This behaviour is different from that of Stop(), which stops the play
	immediately by discarding all the queued data.
	
	Client should call this method on CMdaAudioOutputStream just after its construction and its 
	effect remains through out its lifetime.
	
	Note: This feature is supported only on a DevSound which ignores the underflow errors in the middle of
	the play i.e which returns ETrue from QueryIgnoresUnderflow().
	
	@return KErrNone on success, 
			KErrNotSupported if the underlying DevSound does not ignore the underflow errors in the middle of the play
				
	@see CMdaAudioOutputStream::RequestStop()
	@see CMMFDevSound::QueryIgnoresUnderflow()
	*/
	IMPORT_C TInt KeepOpenAtEnd();
	
	/**
	This method signals the end of play when the AudioOutputStream is in KeepOpenAtEnd mode i.e when client
	makes KeepOpenAtEnd call on it. When RequestStop is called, AudioOutputStream completes playing all
	the data that is supplied to it and calls MaoscPlayComplete() with KErrUnderflow.
	
	Note: Before calling this method, client must have already called KeepOpenAtEnd() successfully, Otherwise, this
		  method returns KErrNotSupported. 
		  It is recommended to use KeepOpenAtEnd and RequestStop calls to get a predictable behaviour during stopping.
	
	@return KErrNone on success
			KErrNotSupported when this method is called without calling KeepOpenAtEnd
			KErrNotReady when this method is called before completing previous RequestStop or AudioOutputStream
			is already in the stopped state
	
	@see CMdaAudioOutputStream::KeepOpenAtEnd()
	*/
	IMPORT_C TInt RequestStop();

private:
	CMdaAudioOutputStream();
private:
	/**
	This member is internal and not intended for use.
	*/
	CMMFMdaAudioOutputStream* iProperties;
	};

#endif
