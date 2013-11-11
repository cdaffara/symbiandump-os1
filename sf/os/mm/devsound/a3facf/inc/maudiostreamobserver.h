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


#ifndef MAUDIOSTREAMOBSERVER_H
#define MAUDIOSTREAMOBSERVER_H

#include <e32base.h>
#include <a3f/a3fbase.h>
#include <a3f/maudiostream.h>

/**
 * An interface to a set of AudioStream callback functions.
 *
 * this serves as the method of communication between the client and the
 * AudioStream.
 *
 * The class is a mixin and is intended to be inherited by the client class
 * that is interested in observing the AudioStream operation. The functions
 * encapsulated by this class are called when specific events occur while
 * processing audio.
 */
class MAudioStreamObserver
	{
public:
	/**
	 * Handles audio stream state change event.
	 *
	 * @param aStream A reference to the stream whose state changed.
	 * @param aReason A set of values describing why the state change occurred 
	 * corresponding to the system-wide error codes. The value will be KErrNone on successful calls.
	 * @param aNewState indicating the state in which the audio stream has changed.
	 */
	virtual void StateEvent(MAudioStream& aStream , TInt aReason, TAudioState aNewState)=0;

	/**
	 * Notifies that adding of processing unit to the stream has been completed successfully or otherwise.
	 *
	 * if the processing unit is added succesfully, then it must be removed from the
	 * stream before the processing unit is removed from the context or before the stream is removed.
	 *
	 * @param aStream A reference to the stream to which the processing unit was added.
	 * @param aInstance a pointer to the processing unit instance.
	 * @param aError an error code. KErrNone on success. Otherwise one of the system wide error codes.
	 */
	virtual void AddProcessingUnitComplete(MAudioStream& aStream, MAudioProcessingUnit* aInstance, TInt aError)=0;

	/**
	 * Notifies that removing of processing unit from the stream has been completed
	 * successfully or otherwise.
	 * 
	 * if the processing unit has been added succesfully, then it must be removed
	 * from the stream, before the processing unit is removed from the
	 * context or before the stream is removed.
	 * 
	 * @param aStream A reference to the stream from which the processing unit has been removed.
	 * @param aType the processing unit type.
	 * @param aInstance a pointer to the processing unit instance.
	 * @param aError an error code. KErrNone on success. Otherwise one of the system wide error codes.
	 */
	virtual void RemoveProcessingUnitComplete(MAudioStream& aStream, MAudioProcessingUnit* aInstance, TInt aError)=0;

	/**
	 * Call-back indicating that is the last buffer has been processed by the sink.
	 *
	 * @param aStream A reference to the stream.
	 */
	virtual void ProcessingFinished (MAudioStream& aStream)=0;

	/**
	 * Signals completion of a Flush() operation.
	 *
	 * @param aStream A reference to the stream on which Flush() was called.  
	 * @param aError an error code. KErrAbort if the associated stream has been unloaded, and the buffers cease to exist.
	 */
	virtual void FlushComplete (MAudioStream& aStream, TInt aError) = 0;

	};

#endif // MAUDIOSTREAMOBSERVER_H
