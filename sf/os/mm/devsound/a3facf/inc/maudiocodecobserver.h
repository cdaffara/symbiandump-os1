/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Save as expressly licensed to you by Symbian Software Ltd, all rights reserved.
* 
*
*/



/**
 @file
 @publishedPartner
 @released
*/

#ifndef MAUDIOCODECOBSERVER_H
#define MAUDIOCODECOBSERVER_H

#include <e32base.h>
#include <a3f/a3fbase.h>

/**
 *  An interface to a set of AudioGainControl callback functions.
 *
 *  This serves as the method of communication between the client and the
 *  AudioGainControl.
 *
 *  The class is a mixin and is intended to be inherited by the client class
 *  that is interested in observing the Gain operations. The functions
 *  encapsulated by this class are called when specific events occur while
 *  processing audio.
 *
 */
class MAudioCodecObserver
	{
public:

	/**
	* Callback showing completion SetSampleRate().
	*
	* @param aError will be KErrNone if the action succeeded, otherwise it will be a system error
	* specific errors to be defined.
	*/
	virtual void SampleRateSet(TInt aError)=0;

	/**
	* Callback showing completion SetMode().
	*
	* @param aError will be KErrNone if the action succeeded, otherwise it will be a system error
	* specific errors to be defined.
	*/
	virtual void ModeSet(TInt aError)=0;

	/**
	* Callback showing completion GetSupportedSampleRates().
	*
	* @param Error will be KErrNone if the action succeeded, otherwise it will be a system error 
	* specific errors to be defined. If aError is not KErrNone, then the state of aSupportedRates is undefined.
	*/
	virtual void GetSupportedSampleRatesComplete (TInt aError)=0;

	/**
	* Callback showing completion GetSupportedModes().
	*
	* @param aError will be KErrNone if the action succeeded, otherwise it will be a system error
	* specific errors to be defined. If aError is not KErrNone, then the state of aSupportedModes is undefined.
	*/
	virtual void GetSupportedModesComplete (TInt aError)=0;

	};

#endif // MAUDIOCODECOBSERVER_H

