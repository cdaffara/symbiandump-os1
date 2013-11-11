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

#ifndef MAUDIOGAINCONTROLOBSERVER_H
#define MAUDIOGAINCONTROLOBSERVER_H

#include <e32base.h>
#include <a3f/a3fbase.h>
#include <a3f/maudiogaincontrol.h>

/**
 *  An interface to a set of AudioGainControl callback functions.
 *
 *  this serves as the method of communication between the client and the
 *  AudioGainControl.
 *
 *  The class is a mixin and is intended to be inherited by the client class
 *  that is interested in observing the Gain operations. The functions
 *  encapsulated by this class are called when specific events occur while
 *  processing audio.
 *
 */
class MAudioGainControlObserver
	{
public:

	/**
	* Notifies the observer that the max ramp time supported by the gain
	* control has changed.
	*
	* @param aGain a reference to GainControl whose max ramp time changed.
	*/
	virtual void MaxRampTimeChanged(MAudioGainControl& aGain)=0;

	/**
	* Notifies the observer that the maximum gain value supported by the
	* gain control has changed.
	*
	* @param aGain a reference to GainControl whose max gain changed.
	*/
	virtual void MaxGainChanged(MAudioGainControl& aGain)=0;

	/**
	* Notifies the observer that the gain has changed, 
	* due to request from the client or otherwise.  
	* If gain change by the client cannot be fulfilled, 
	* a gain change with an error code other than KErrNone will be issued.
	* @param aGain	a reference to the gain control whose gain has changed.
	* @return aError		an error code. 
	* KErrNone if the gain change was requested by the client and was completed successfully.
	* KErrNotSupported if the SetGain() call referred to non-existent channels.
	* KErrArgument if aGain of less than 0 or larger than max gain, 
	* the same channel is specified multiple times, 
	* or aRampDuration is less than 0 or greater than max ramp duration.
	* KErrA3fUnsupportedRamp if the ramp operation requested is not supported, 
	* or the Uid value supplies is unknown.
	* Other system-wide errors are also possible. They will indicate that the gain change failed.
	* Note that these errors could alternatively be returned by the associated SetGain() call.
	*/
	virtual void GainChanged(MAudioGainControl& aGain, TInt aError)=0;

	};

#endif // MAUDIOGAINCONTROLOBSERVER_H
