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




#ifndef CDEVTONECONTROL_H
#define CDEVTONECONTROL_H

#include <e32base.h>

#include "cdevcommoncontrol.h"

#include <a3f/maudiodatasupplier.h>
#include <a3f/maudioprocessingunitobserver.h>
#include <a3f/tonedata.h>
#include <a3f/a3f_trace_utils.h>


class MMMFBufferSource;

/**
 * CDevToneControl.
 *
 * ?description
 *
 * @lib mmfdevsoundadaptation.lib
 */

NONSHARABLE_CLASS(CDevToneControl) :public CDevCommonControl,
									public MMMFAudioDataSupplier
	{
public:

	/**
	* Destructor.
	* Releases all resources and deletes all objects owned by this instance.
	*/
	~CDevToneControl();

	static CDevToneControl* NewL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver);

	void ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver);

// from base class CDevAudioControl

	/**
	* Initialize playing state control components
	* @since
	* @return error code
	*/
	virtual TInt Initialize(TUid aFormat);

	/**
	* Initializes and starts the wanted operation (Play, Record, TonePlay).
	* @since
	* @return error code
	*/
	virtual TInt ProcessInit();

	/**
	* Processes the data (PlayData, RecordData).
	* @since
	* @return void
	*/
	virtual void ProcessData();

	// from base class MMMFAudioDataSupplier
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

	/**
	* Indicates that last buffer has been processed
	*
	* @since
	* @param aSource the source
	*/

	void ProcessingFinished(MAudioStream& /*aStream*/);

	/**
	* Saves tonedata for later use
	* This includes information about tone type, data, length, etc.
	*
	* @since
	* @param aToneData
	* @return error code
	*/
	TInt SetToneData(TToneData& aToneData);


	// from CDevCommonControl
	void FinishWithError(TInt aError);
	
	/**
	* Called when a ProcessingFinished callback is received
	*
	* @since
	* @param TBool& aAyncCompletion
	* @return an error code KErrNone if successful
	*/
	virtual TInt ProcessingFinishedReceived(TBool& aAyncCompletion);
	
	virtual TInt ProcessingError(TBool& aAyncCompletion);

private:
	CDevToneControl();

private: // data

	/**
	* Casted convenience pointer to DevAudio::iAudioSource
	* Not own.
	*/
	MMMFBufferSource *iBufferSource;

	/**
	* Information about tone.
	*/
	TToneData iToneData;

	};

#endif // CDEVTONECONTROL_H
