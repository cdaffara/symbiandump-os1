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


#ifndef MAUDIOPROCESSINGUNITOBSERVER_H__
#define MAUDIOPROCESSINGUNITOBSERVER_H__

#include <e32base.h>

#include <a3f/a3fbase.h>

class MAudioProcessingUnit;

/**
 * An interface to a set of audio processing unit callback functions.
 * 
 * this serves as the method of communication between the client and the audio processing unit.
 *
 * The class is a mixin and is intended to be inherited by the client class that is
 * interested in observing the audio processing unit operation. The functions
 * encapsulated by this class are called when specific events occur while
 * processing audio.
 * 
 * this interface is mainly usable for advanced audio clients, that are interested in detailed
 * operation of the processing chain. Most audio clients should be fine with only observing the audio stream.
 * 
 */
class MAudioProcessingUnitObserver
	{
public:
	/**
	* Handles processing unit error event.
	*
	* @param aInstance A pointer to the processing unit instance.
	* @param aError. Propagated error.
	*/
	virtual void ProcessingUnitError(MAudioProcessingUnit* aInstance, TInt aError)=0;
	};

#endif // MAUDIOPROCESSINGUNITOBSERVER_H__
