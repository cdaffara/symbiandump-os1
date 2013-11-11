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
*
*/



#ifndef AUDIOGAINCONTROL_H
#define AUDIOGAINCONTROL_H

#include <e32base.h>
#include <a3f/a3fbase.h>
#include <a3f/maudiogaincontrol.h>
#include <a3f/maudiogaincontrolobserver.h>

#include "mgainhelper.h"

#include <a3f/a3f_trace_utils.h>


const TUint KDefaultGain = 5;
const TUint KDefaultMaxGain = 255; // TODO should discover at run time but this is value of current Symbian sound driver adaptor
const TUint KDefaultNumChannels = 2;
const TUint KDefaultMaxRampTime = 5;

enum TGainControlPanicCode
	{
	EAdaptationOldSetGainCalledPanic,
	};

/**
 *  Implementation of audio gain control API.
 *  ?more_complete_description
 *  @lib audiogaincontrol.lib
 */
NONSHARABLE_CLASS(CAudioGainControl) : 	public CBase, public MAudioGainControl
	{
public:

	IMPORT_C static CAudioGainControl* NewL();
	/**
	* Destructor.
	*
	* Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CAudioGainControl();

	IMPORT_C void SetHelper(MGainHelper& aHelper);

	IMPORT_C void IssueGainChangedCallBack(TInt aError);

	IMPORT_C TInt ConfigureRamp(TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration);

	void IssueMaxGainChangedCallBack();
	void IssueMaxRampTimeChangedCallBack();

	// from base class MAudioGainControl
	TInt GetMaxGain(TInt& aMaxGain) const;
	TInt GetMaxRampTime(TTimeIntervalMicroSeconds& aMaxRampTime) const;
	TInt SetGain(RArray<TAudioChannelGain>& aChannels, TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration);
	TInt SetGain(RArray<TAudioChannelGain>& aChannels);
	TInt GetGain(RArray<TAudioChannelGain>& aChannels) const;
	TInt RegisterAudioGainControlObserver(MAudioGainControlObserver& aObserver);
	void UnregisterAudioGainControlObserver(MAudioGainControlObserver& aObserver);

private:
	void ConstructL();

	CAudioGainControl();

private: //data
	RArray<TAudioChannelGain> iGains;
	/**
	 * Reference to observer instance
	 */
	RPointerArray<MAudioGainControlObserver> iGainObservers;

	// The gain cannot talk directly with the sound device driver so it needs a helper to request
	MGainHelper*	iHelper;
	};

#endif // AUDIOGAINCONTROL_H
