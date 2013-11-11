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


#ifndef MAUDIOCONTEXT_H
#define MAUDIOCONTEXT_H

#include <e32base.h>

#include <a3f/a3fbase.h>

class MAudioStream;
class MAudioProcessingUnit;
class MAudioContextObserver;

/**
 * An interface to an audio context.
 *
 * Audio context is a container for a set of logically related audio streams and audio processing units.
 * All streams and processing units are created into a context.
 * A new context can be created using an instance of CAudioContextFactory.
 * When the context is no longer needed, it must be removed using CAudioContextFactory::DeleteAudioContext().
 */
class MAudioContext
	{
public:

	/**
	 * Returns the context identifier.
	 *
	 * @return return the context identifier
	 */
	virtual TAudioContextId ContextId() const = 0;

	/**
	 * Creates a new audio stream into this context. 
	 *
	 * The stream must be removed using DeleteAudioStream() when it is no longer needed.
	 *
	 * @param aStream on return contains a pointer to the created stream.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual TInt CreateAudioStream(MAudioStream*& aStream)=0;

	/**
	 * Deletes an audio stream from this context.
	 *
	 * After removing the stream all references to it will become invalid and must not be used.
	 * At the end of the call, aStream will have been set to NULL. 
	 * Calling DeleteAudioStream() with a parameter whose value is NULL is itself a null operation.
	 * @param aStream a pointer to the stream to Delete.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual void DeleteAudioStream(MAudioStream*& aStream)=0;

	/**
 	 * Creates a new audio processing unit into this context.
	 *
 	 * The processing unit must be removed using DeleteProcessingUnit() when it is no longer needed.
	 *
	 * @param aType the type of the processing unit.
	 * @param aProcessingUnit on return contains a pointer to the created processing unit.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual TInt CreateAudioProcessingUnit(TUid aTypeId, MAudioProcessingUnit*& aProcessingUnit)=0;

	/**
	 * Deletes the audio processing unit from this context.
	 *
	 * After removing the processing unit all references to it will become invalid and must not be used.
	 * At the end of the call, aProcessingUnit will have been set to NULL. 
	 * Calling DeleteAudioProcessingUnit() with a parameter whose value is NULL is itself a null operation.
	 * @param aProcessingUnit a pointer to the processing unit to remove.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual void DeleteAudioProcessingUnit(MAudioProcessingUnit*& aProcessingUnit)=0;

	/**
	 * Apply changes made to audio components associated with this context. 
	 *
	 * A state change applied to an Audio Stream should be considered "pending" until Commit() is called, 
	 * in that the adaptation merely records that the state setting has been requested and does not seek 
	 * to implement the state change until Commit().
	 *
	 * Commit() can be thought of as an asynchronous call, and completion is signalled by the appropriate 
	 * ContextEvent() callback of MAudioContextObserver.
	 * @return An error code. KErrNone on success. KErrNotReady if SetClientSettings() has not been called.
	 */
	virtual TInt Commit()=0;

	/**
	 * Forget any pending, requested changes made to this context or its components. 
	 *
	 * Typically called where the client has started to request changes, has yet to call Commit() and changes its mind.
	 *
 	 * @return A system wide error code. KErrNone on success. KErrNotReady if called during Commit() cycle.
	 */
	virtual TInt Reset()=0;

	/**
	 * Sets the client information used by the underlying MMRC. 
	 *
	 * This must be called before any call to Commit(). The client context contains information about the client application 
	 * which is utilising the audio functionality, since this will typically be a different process than is calling A3F.
  	 *
	 * @param aSettings Essentially the process Id of the client application, 
	 * the one whose details are used to determine capabilities and policy.
	 * @return an error code. KErrNone for success, otherwise one of the system-wide error codes.	
	 */
	virtual TInt SetClientSettings(const TClientContextSettings& aSettings)=0;
	
	/**
	 * A mechanism to obtain extensions to the Context API.
  	 *
	 * @param aType Uid that denotes the type of the interface.
	 * @return the pointer to the specified interface, or NULL if it does not exist. 
	 * Must be cast to correct type by the user.
	 */
	virtual TAny* Interface(TUid aType)=0;

	/**
	 * Registers an audio context observer.
	 *
	 * The observer will be notified about context state changes.
	 *
	 * @param aObserver the observer reference to register.
	 * @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	 */
	virtual TInt RegisterAudioContextObserver(MAudioContextObserver& aObserver) = 0;

	/**
	 * Unregisters an audio context observer.
	 *
	 * @param aObserver the observer reference to unregister.
  	 * @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	 */
	virtual void UnregisterAudioContextObserver(MAudioContextObserver& aObserver) = 0;
	};

#endif // MAUDIOCONTEXT_H
