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




/**
 @file
 @publishedPartner
 @released
*/

#ifndef MAUDIOCODEC_H
#define MAUDIOCODEC_H

#include <a3f/a3fbase.h>
#include <a3f/maudioprocessingunit.h>
#include <a3f/maudiocodecobserver.h>


/**
 * A generic abstract interface for codecs.
 *
 * The client must add a codec to a stream for operation.
 * The codec can only be added to a stream when the codec is in uninitialized state.
 * The codec can only be removed from a stream when the codec is in uninitialized or dead states.
 */
class MAudioCodec
	{
	public:
	
	/**
	 * Sets the codec format.
	 *
	 * For encoders this sets the output format.
	 * For decoders this configures the input format.
	 * Format must be set before the codec can transfer to initialized state.
	 *
	 * @param aFormat codec format.
	 * @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	 */
	virtual TInt SetFormat(TUid aFormat)=0;

	/**
	* Sets the sample rate in samples/sec.
	*
	* sets the sample rate of the encoded data,
	* and says nothing about the sample rate used on the output device
	* or whether re-sampling is required that is up to the adaptation.
	* the result of the operation is returned by MAudioCodecObserver::SampleRateSet().
	*
	* @param aSampleRate sample rate to be used, in samples/sec.
	* @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	*/
	virtual TInt SetSampleRate(TInt aSampleRate)=0;

	/**
	* Sets the mode, e.g. mono/stereo. 
	* The result of the operation is returned by MAudioCodecObserver::ModeSet().
	*
	* @param aMode uid stating the mode.
	* @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	*/
	virtual TInt SetMode(TUid aMode)=0;

	/**
	* This returns a list of supported sample rates. The list need just include those of the standard list 
	* that are supported. Includes rates supported by sample rate conversion. 
	* GetSupportedSampleRatesComplete() callback shows result.
	* TODO Need to document that the implementation is responsible for calling Reset() on aSupportedRates.
	*
	* @param aSupportedRates array that is populated with the supported rates list.
	* @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	*/
	virtual TInt GetSupportedSamplesRates(RArray<TInt>& aSupportedRates)=0;

	/**
	* This returns a list of supported sample modes. The list corresponds to the current configuration 
	* and support may include the use of mono/stereo conversion etc. 
	* GetSupportedModesComplete() callback shows result.
	* TODO Need to document that the implementation is responsible for calling Reset() on aSupportedModes.
	*
	* @param aSupportedModes array that is populated with the supported modes list.
	* @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	*/
	virtual TInt GetSupportedModes(RArray<TUid>& aSupportedModes)=0;

	/**
	* Registers an audio codec observer.
	*
	* Note that this function is meant for clients of the Audio codec. 
	* The client should unregister using UnregisterAudioCodecObserver() when applicable
	*
	* @param aObserver reference to the observer to register.
	* @return an error code. KErrNone if successful.
	*				KErrAlreadyExists if the client is already registered.
	*				KErrOutOfMemory in case of memory exhaustion.
	*/
	virtual TInt RegisterAudioCodecObserver(MAudioCodecObserver& aObserver)=0;

	/**
	* Unregisters an audio codec observer.
	*
	* @param aObserver reference to the observer to unregister.
	* @return an error code.  KErrNone if successful.
	* 						KErrNotFound if the client has not been registered as observer.
	*/
	virtual void UnregisterAudioCodecObserver(MAudioCodecObserver& aObserver)=0;

	};

#endif // MAUDIOCODEC_H
