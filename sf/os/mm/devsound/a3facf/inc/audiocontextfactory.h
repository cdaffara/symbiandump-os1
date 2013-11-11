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
 @publishedPartner
 @file
 @released
*/


#ifndef AUDIOCONTEXTFACTORY_H
#define AUDIOCONTEXTFACTORY_H

#include <e32base.h>

#include <a3f/a3fbase.h>

class MAudioContext;

/**
 * An interface to an AudioContextFactory.
 *
 * Audio Context Factory is the entry point to the low level audio functionality of the device.
 * Audio Context Factory is used for creating new audio contexts.
 *
 *  @lib AudioContextFactory.lib
 */
class CAudioContextFactory : public CBase
	{
public:
	/**
	 * Constructs and returns a pointer to a new CAudioContextFactory object.
	 *
	 * @return CAudioContextFactory* - on success, pointer to new class instance.
	 * This call returns ownership, as in the standard NewL() pattern.
	 */
	IMPORT_C static CAudioContextFactory* NewL();

	/**
	 * Destructor.
	 *
	 * Deletes all objects and releases all resource owned by this instance.
	 */
	IMPORT_C virtual ~CAudioContextFactory();

	/**
	 * Creates a new audio context.
	 *
	 * The context must be removed using DeleteAudioContext() when it is no longer needed.
	 * @param aContext on return contains a reference to the created context.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
 	 */
	IMPORT_C TInt CreateAudioContext(MAudioContext*& aContext);


	/**
	 * Deletes the audio context.
	 * All processing units have to be removed from the context by the client.
	 * before the context is removed. This is a null-operation if aContext is NULL on DeleteAudioContext being called.
	 * aContext is set to NULL on completion of the call.
	 * @param aContext a pointer to the context to remove.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	IMPORT_C void DeleteAudioContext(MAudioContext*& aContext);

protected:
	/**
	 * Constructor
	 */
	CAudioContextFactory();
	/**
	 * Second phase contructor.
	 */
	void ConstructL();
	};

#endif // AUDIOCONTEXTFACTORY_H
