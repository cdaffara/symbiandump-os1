
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



#ifndef __MDAAUDIOINPUTSTREAM_H
#define __MDAAUDIOINPUTSTREAM_H


#include <e32std.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mda/common/base.h>
#include <mmfclntutility.h>


/**
@publishedAll
@released

An interface class that notifies the client of the progress of the audio input streaming. 

It must be implemented by users of the CMdaAudioInputStream class. 

An object that implements this interface is passed to CMdaAudioInputStream::NewL().
*/
class MMdaAudioInputStreamCallback 
	{
public:

	/**
	A callback function that is called when CMdaAudioInputStream::Open() has 
	completed, indicating that the audio input stream is ready for use.

	@param  aError
	        An error value which indicates if open was successful or not. KErrNone if the open succeeded,
	        otherwise one of the system error codes.

	*/
	virtual void MaiscOpenComplete(TInt aError) = 0;

	/**
	A callback function that is called when a chunk of audio data has been copied to the descriptor specified 
	in a CMdaAudioInputStream::ReadL().

	It is also called as part of a 'cleanup' operation when CMdaAudioInputStream::Stop() is used. In such circumstances, 
	the remaining audio data in the FIFO since the last ReadL() and the Stop() command is returned. In addition aError 
	is set to KErrAbort.

	Use this callback to retrieve the pointers to your recorded data and to issue subsequent ReadL() calls, and be prepared 
	to shut down you recording functions if KErrAbort (input stream closed) is returned in aError.

	@param aError
	       An error value indicating if the copy was successful. KErrNone if the copy succeeded, KErrAbort if the input stream 
	       was closed for some reason, otherwise one of the system error codes.
	@param aBuffer
	       A reference to the buffer that has been copied.
	*/
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer) = 0;

	/**
	A callback function that is called when the input stream is closed using CMdaAudioInputStream::Stop(). 

	This callback is usually 'paired' with a MaiscBufferCopied() that returns the last remaining audio data
	in the FIFO between the last CMdaAudioInputStream::ReadL() and the Stop() command.

	@param aError
	       An error value indicating if the record was successful. KErrNone if the stop succeeded, otherwise one
	       of the system error codes.
	*/
	virtual void MaiscRecordComplete(TInt aError) = 0;
	};


class CMMFMdaAudioInputStream;

/**
@publishedAll
@released

The interface to an audio stream player passing raw audio data from the audio hardware to specified buffers.

This class enables MMF clients to:

Stream raw audio data from the audio hardware to specified buffers.

Specify the priority of the audio stream in relation to other clients that may request to use the same audio hardware.

Set the sample rate and  the number of channels after successfully opening the stream. It is not possible to
change these values once streaming has started.

Change the gain and balance before or while streaming is in progress. Gain and balance settings take effect immediately.


The API supports callbacks from the server to notify the client:

MaiscOpenComplete() will be called when the audio streaming object is open and ready to stream data back to the
user as a result of a previous call to Open().

MaiscBufferCopied() will be called multiple times while the FIFO (queued via ReadL()) is emptied.
Note: Each buffer will be flagged with KErrAbort if received after Stop() on is called. MaiscRecordComplete is called
once all remaining buffers have been read.


MaiscRecordComplete() is called after Stop() has been called. aError - KErrAbort if MaiscRecordComplete called
as a result of a call to Stop().

Users of object CMdaAudioInputStream should ensure that implementation of the callback methods MaiscOpenComplete,MaiscBufferCopied 
and MaiscRecordComplete in class MMdaAudioInputStreamCallback do not delete the object,otherwise a Kern-Exec 3 would occur during callback.
*/
class CMdaAudioInputStream : public CBase,
							 public MMMFClientUtility	
{
public:

	/**
	Instantiates a new audio input stream using default priority preferences.

	@param  aCallBack
	        A callback to notify the client when the input stream has been initialised and is ready 
	        for use, when data has been copied to a specified descriptor and when input streaming is 
	        stopped. The caller must create a callback class that implements this interface.

	@return A pointer to the audio input stream object.
	
	@capability	UserEnvironment
			For recording - the requesting client process must have a 
			UserEnvironment capability.
	*/
	IMPORT_C static CMdaAudioInputStream* NewL(MMdaAudioInputStreamCallback& aCallBack);

	/**
	Instantiates a new audio input stream.

	@param  aCallBack
	        A callback to notify the client when the input stream has been initialised and is ready for 
	        use. The caller must create a callback class which implements this interface.
	@param  aPriority
	        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
	        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
    @param  aPref
            The Priority Preference - an additional audio policy parameter. The suggested default is 
            EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
            values may be supported by given phones and/or platforms, but should not be depended upon by 
            portable code.

	@return A pointer to the audio input stream object.

	@capability	UserEnvironment
			For recording - the requesting client process must have a 
			UserEnvironment capability.
			
	Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
	several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
	the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
	Whatever, the decision 	as to what to do in such situations is up to the audio adaptation, and may
	vary between different phones. Portable applications are advised not to assume any specific behaviour. 
	*/
	IMPORT_C static CMdaAudioInputStream* NewL(MMdaAudioInputStreamCallback& aCallBack,
		TInt aPriority,	TInt aPref);

	/**
    Destructor.

	Frees all resources owned by the object prior to its destruction.
	*/
	~CMdaAudioInputStream();

	/**
	Sets the sample rate and number of audio channels.

	@param  aSampleRate
	        The new sample rate. For possible values, see the TAudioCaps enum in class TMdaAudioDataSettings.
	@param  aChannels
	        The new number of channels. For possible values, see the TAudioCaps enum in class TMdaAudioDataSettings.
	*/
	IMPORT_C void SetAudioPropertiesL(TInt aSampleRate, TInt aChannels);

	/**
	Opens an input audio stream package.

	The MMdaAudioInputStreamCallback::MaisOpenComplete() callback function is called when the stream has been 
	opened and is ready to record data. If the open was unsuccessful, this is indicated by the aError parameter 
	of the callback.

	@param  aSettings
	        A pointer to an TMdaPackage object.
	*/
	IMPORT_C void Open(TMdaPackage* aSettings);

	/**
	Sets the gain for the audio device to a specified value.

	@param  aNewGain
	        The gain setting. This can be any value from zero to the value returned by a call to 
	        MaxGain(). A value which is less than zero is set to zero. A value which is greater 
	        than MaxGain() is set to MaxGain().
	*/
	IMPORT_C void SetGain(TInt aNewGain);

	/**
	Returns the current gain setting.

	@return The current gain setting.
	*/
	IMPORT_C TInt Gain() const;

	/**
	Returns the maximum gain level.

	@return The maximum gain value that is supported by the sound device.
	*/
	IMPORT_C TInt MaxGain() const;

	/**
	Sets the current audio balance.

	The balance can be any value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight, the default value 
	being KMMFBalanceCenter.

	@param  aBalance
	        A specified balance value.
	*/
	IMPORT_C void SetBalanceL(TInt aBalance = KMMFBalanceCenter);

	/**
	Returns the current audio balance.

	@return The current balance value.
	*/
	IMPORT_C TInt GetBalanceL() const;

	/**
	Sets the audio priority values.

	This function cannot be used while the stream object is open. It is intended for use before an Open()
	is issued, or between a previous Stop() and a new Open().

    @param  aPriority
            The Priority Value.
    @param  aPref
            The Priority Preference.

    @see CMdaAudioInputStream::NewL()
	*/
	IMPORT_C void SetPriority(TInt aPriority, TInt aPref);

	/**
	Records streaming raw audio data.

	If this is the first ReadL() after a successful Open() this function starts the audio recording process.

	The MMdaAudioInputStreamCallback::MaisBufferCopied() callback function is called when the buffer has been successfully 
	filled. A pointer to the newly filled buffer is returned in the callback.

	Note: The MMdaAudioInputStreamCallback::MaisBufferCopied() callback function is also called when a Stop() is issued, with a 
	pointer to the remaining recorded audio data and the aError parameter of the callback set to indicate recording has finished.

	@param  aData
	        A reference to a buffer to be written into.
	*/
	IMPORT_C void ReadL(TDes8& aData);

	/**
	Stops the recording process.

	The MMdaAudioInputStreamCallback::MaisRecordComplete() callback function is called when recording has been halted. Just prior 
	to issuing this callback, the FIFO buffers are read and the remaining audio data passed back by a final 
	MMdaAudioInputStreamCallback::MaisBufferCopied() callback with its aError parameter set to KErrAbort.
	
	Any remaining FIFO buffers are also passed back, with their aError parameter set to KErrAbort.
	
	Stop is immediate and best used for premature stops, for example within destructors.
	If it is critical to receive all data buffers upto the point that Recording has Stopped, 
	we recommend using RequestStop instead. Otherwise some trailing buffers may be lost from the recording process.
	
	If a Stop is issued after a RequestStop, Stop will take precedent.

	@see CMdaAudioInputStream::RequestStop()
	*/
	IMPORT_C void Stop();

	/**
	Returns the current position within the data stream in microseconds since the start of streaming.

	@return The current position.
	*/
	IMPORT_C const TTimeIntervalMicroSeconds& Position();

	/**
	Returns the current number of bytes rendered by audio hardware.

	@return The number of bytes.
	*/
	IMPORT_C TInt GetBytes();


	/**
    Sets the data type.  If the data type is not explicitly set it will assumed to be pcm16.
    If it is set then the hardware must support the data type being set otherwise the 
    function leaves with KErrNotSupported.
    
    @param	aAudioType The fourCC code used to request the data type of the recorded data.

	@leave  KErrNotSupported
	        Leaves with this for any unsuported data type.
	*/
	IMPORT_C void SetDataTypeL(TFourCC aAudioType);

	/**
	Returns the current data type.

	@return The ID of the data type.
	*/
	IMPORT_C TFourCC DataType() const;
	
	/**
	Returns the bit rates supported by the sound device
	
	@param  aSupportedBitRates
	@leave KErrNotSupported
		If the sound device does not support setting the bit rate
	The supported bit rates, in bits per second, shall be appended to this array. Note that 
	the array shall be reset by this method.
	*/
	IMPORT_C void GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates);
	/**
	Returns the current bit rate.
	@return The current bit rate, in bits per second.
	@leave KErrNotSupported
		If the sound device does not support returning the current bit rate
	*/
	IMPORT_C TInt BitRateL() const;
	/**
	Sets the bit rate to a new value.
	@param  aBitRate
		The new bit rate, in bits per second.
	@leave KErrNotSupported
		If the sound device does not support returning the current bit rate or the bit rate set is not supported
	*/
	IMPORT_C void SetBitRateL(TInt aBitRate);
	
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
	Indicates that the client wishes to use a single buffer	to collect the data. 
	This buffer is passed via the ReadL api.
	
	@param	aSingleMode
			ETrue to indicate setting of single buffer mode and the use of a single 
			buffer, EFalse to indicate use of multiple buffers.
	*/
	IMPORT_C void SetSingleBufferMode(TBool aSingleMode);

	/**
	Requests a Stop of the recording process.

	The device is requested to Stop and does so after all unprocessed buffers are processed and passed
	back to the input stream. The device is actually Stopped when the final empty buffer indicating 
	buffer process completion is received.

	@see CMdaAudioInputStream::Stop()
	*/
	IMPORT_C void RequestStop();

private:
	CMdaAudioInputStream();
private:
	CMMFMdaAudioInputStream* iProperties;
	};

#endif
