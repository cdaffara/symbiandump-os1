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




#ifndef CDEVRECORDCONTROL_H
#define CDEVRECORDCONTROL_H

#include <e32base.h>

#include "cdevcommoncontrol.h"

#include <a3f/maudioprocessingunitobserver.h>
#include <a3f/maudiodataconsumer.h>
#include <a3f/a3f_trace_utils.h>


class MMMFBufferSink;

/**
 * CDevRecordControl.
 *
 * ?description
 *
 * @lib mmfdevsoundadaptation.lib
 * @since
 */
NONSHARABLE_CLASS(CDevRecordControl) :	public CDevCommonControl,
										public MMMFAudioDataConsumer
	{
public:

	/**
	* Destructor.
	* Releases all resources and deletes all objects owned by this instance.
	*/
	~CDevRecordControl();

	static CDevRecordControl* NewL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver);

	void ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver);

	// from base class CDevAudioControl

	/**
	* Initialize recording state control components
	* @since
	* @return error code
	*/
	virtual TInt Initialize(TUid aFormat);

	/**
	* Initializes the audio device and starts the record process. This
	* function queries and acquires the audio policy before initializing
	* audio device. If there was an error during policy initialization,
	* RecordError() function will be called on the observer with error code
	* KErrAccessDenied, otherwise BufferToBeEmptied() function will be called
	* with a buffer reference. This buffer contains recorded or encoded
	* data. After processing data in the buffer reference passed, the client
	* should call RecordData() to continue recording process.
	* The amount of data that is available is specified in
	* CMMFBuffer::RequestSize().
	*
	* @since
	* @return error code
	*/
	virtual TInt ProcessInit();

	/**
	* Contine the process of recording.
	* Once the buffer is filled with recorded data, the Observer gets a
	* reference to the buffer along with the callback function
	* BufferToBeEmptied(). After processing the buffer (copying over to a
	* different buffer or writing to file) the client should call this
	* function to continue the recording process.
	* @return void
	*/
	virtual void ProcessData();

	/*
	Call-back indicating that is the last buffer has been processed by the sink.
	*/
	virtual void ProcessingFinished (MAudioStream & aStream);
	
	// from base class MAudioDataConsumer
	/**
	* Provide a buffer with audio data to the consumer.
	*
	* The consumer should read the contents of the buffer as quickly as possible and then call
	* MCustomSink::BufferEmptied() passing the used buffer as a parameter.
	*
	* @param aSink a pointer to the sink providing the buffer
	* @param aBuffer a pointer to the buffer to empty
	*/

	virtual void BufferToBeEmptied(MMMFBufferSink* aSink, CMMFBuffer* aBuffer);

	// from base class MAudioDataConsumer
	virtual void DiscardBuffers(MMMFBufferSink* aSource);

    // from base class CDevCommonControl
    void FinishWithError(TInt aError);
	
	/*
	Used to send a stop call when there is a error in the buffer
	*/
	void BufferErrorEvent();
	
	virtual TBool ProcessingError(TBool& aAyncCompletion);

private:
	CDevRecordControl();

private: // data

	/**
	 * Casted convenience pointer to DevAudio::iAudioSink
	 * Not own.
	 */
	MMMFBufferSink *iBufferSink;

	};

#endif // CDEVRECORDCONTROL_H
