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




#ifndef CDEVPLAYCONTROL_H
#define CDEVPLAYCONTROL_H

#include <e32base.h>

#include "cdevcommoncontrol.h"

#include <a3f/maudiodatasupplier.h>
#include <a3f/maudioprocessingunitobserver.h>
#include <a3f/a3f_trace_utils.h>


class MMMFBufferSource;

/**
 * CDevPlayControl.
 *
 * ?description
 *
 * @lib mmfdevsoundadaptation.lib
 * @since
 */

NONSHARABLE_CLASS(CDevPlayControl) :public CDevCommonControl,
									public MMMFAudioDataSupplier
    {
public:

	/**
	* Destructor.
	* Releases all resources and deletes all objects owned by this instance.
	*/
	~CDevPlayControl();

	static CDevPlayControl* NewL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver);

	void ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aAdaptationObserver);

	// from base class CDevAudioControl

	/**
	* Initialize playing state control components
	* @since
	* @return error code
	*/
	virtual TInt Initialize(TUid aFormat);

	/**
	* Initializes the audio device and starts the play process. This
	* function queries and acquires the audio policy before initializing
	* audio device. If there was an error during policy initialization,
	* PlayError() function will be called on the observer with error code
	* KErrAccessDenied, otherwise BufferToBeFilled() function will be called
	* with a buffer reference. After reading data into the buffer reference
	* passed, the client should call PlayData() to play data.
	* The amount of data that can be played is specified in
	* CMMFBuffer::RequestSize(). Any data that is read into buffer beyond
	* this size will be ignored.
	* Leaves on failure.
	* @since
	* @return void
	*/
	virtual TInt ProcessInit();

	/**
	* Plays data in the buffer at the current volume.
	* The client should fill the buffer with audio data before calling this
	* function. The observer gets a reference to the buffer along with the
	* callback function BufferToBeFilled(). When playing of the audio sample
	* is complete, successfully or otherwise, the function PlayError() on
	* the observer is called.
	* The last buffer of the audio stream being played should have the last
	* buffer flag set using CMMFBuffer::SetLastBuffer(TBool). If a
	* subsequent attempt to play the clip is made, this flag will need
	* resetting by the client.
	* @return void
	*/
	virtual void ProcessData();

	/*
	Call-back indicating that is the last buffer has been processed
	*/
	virtual void ProcessingFinished (MAudioStream& aStream);

	// from base class MAudioDataSupplier

	/**
	* Request a buffer to be filled with audio data by the data supplier.
	*
	* The supplier should fill the buffer as quickly as possible and then call
	* MBufferSource::BufferFilled() passing the filled buffer as a parameter.
	*
	* @param aSource a pointer to the source requesting the buffer
	* @param aBuffer a pointer to the buffer to fill
	*/
	virtual void BufferToBeFilled(MMMFBufferSource* aSource, CMMFBuffer* aBuffer);

	/**
	* Indicates that all buffers previously requested by this source have become
	* invalid and must not be used.
	*
	* @param aSource the source whose previous buffer requests should be ignored.
	*/
	virtual void DiscardBuffers(MMMFBufferSource* aSource);

	// from base class CDevCommonControl
	void FinishWithError(TInt aError);

	/**
	* Called when a ProcessingFinished callback is received
	*
	* @since
	* @param TBool& aAyncCompletion
	* @return an error code KErrNone if successful
	*/
	virtual TInt ProcessingFinishedReceived(TBool& aAyncCompletion);
	
	/*
	Used to send a stop call when there is a error in the buffer
	*/
	void BufferErrorEvent();
	
	virtual TInt ProcessingError(TBool& aAyncCompletion);
	
private:
	CDevPlayControl();

private: // data

	/**
	* Casted convenience pointer to DevAudio::iAudioSource
	* Not own.
	*/
	MMMFBufferSource *iBufferSource;

	};

#endif // CDEVPLAYCONTROL_H
