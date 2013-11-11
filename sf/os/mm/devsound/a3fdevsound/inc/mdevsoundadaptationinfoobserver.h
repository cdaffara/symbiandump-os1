// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
 
#ifndef MDEVSOUNDADAPTATIONINFOOBSERVER_H
#define MDEVSOUNDADAPTATIONINFOOBSERVER_H

/**
@publishedPartner
@file
@released
 */

#include <e32base.h>
#include <a3f/a3fbase.h>


/**
 *  An interface to a set of A3FDevSoundAdaptationInfo callback functions.
 *
 *  This serves as the method of communication between the A3FDevSoundAdaptationInfo 
 *  and the client
 *
 */
class MA3FDevSoundAdaptationInfoObserver
	{
public:
	/**
	* Notifies the observer of the max gain supported
	*
	* @param aCodecType the UID passed to the original RequestMaxGain() call
	* @param aError an standard symbian error code. KErrNone if succesful
	* @param aResult the supported MaxGain/MaxVolume value, if aError==KErrNone
	*/
	virtual void RequestMaxGainComplete (TUid aCodecType, TInt aError, TInt aResult)=0;


	/**
	* Notifies the observer that the max ramp time supported by the gain
	* control has changed.
	*
	* @param aCodecType the UID passed to the original RequestSupportedForamts() call
	* @param aError an standard symbian error code. KErrNone if succesful
	*/
	virtual void RequestSupportedFormatsComplete(TUid aCodecType, TInt aError)=0;

	};

#endif //MDEVSOUNDADAPTATIONINFOOBSERVER_H
