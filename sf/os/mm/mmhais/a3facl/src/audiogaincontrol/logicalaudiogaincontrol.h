/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : logicalaudiogaincontrol.h
* Part of     : ACL Logical Layer
*
*/



#ifndef LOGICALAUDIOGAINCONTROL_H
#define LOGICALAUDIOGAINCONTROL_H

#include <e32base.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/a3fbase.h>
#include <a3f/maudiogaincontrol.h>
#include <a3f/maudiogaincontrolobserver.h>

#include "audioprocessingunit.h"


class MLogicalSettingObserver;

/**
Implementation of audio gain control API.

@lib audiogaincontrol.lib
*/

NONSHARABLE_CLASS(CLogicalAudioGainControl) : 	public CAudioProcessingUnit,
												public MAudioGainControl,
												public MAudioGainControlObserver
	{
friend class CAudioContext;
friend class CLogicalAudioStream;

public:

	/**
	Destructor.

	Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CLogicalAudioGainControl();

	// from base class MAudioGainControl
	virtual TInt GetMaxGain(TInt& aMaxGain) const;
	virtual TInt GetMaxRampTime(TTimeIntervalMicroSeconds& aMaxRampTime)const;
	virtual TInt SetGain(RArray<TAudioChannelGain>& aChannels, TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration);
	virtual TInt SetGain(RArray<TAudioChannelGain>& aChannels);
	virtual TInt GetGain(RArray<TAudioChannelGain>& aChannels) const;
	virtual TInt RegisterAudioGainControlObserver(MAudioGainControlObserver& aObserver);
	virtual void UnregisterAudioGainControlObserver(MAudioGainControlObserver& aObserver);

	// from base class CAudioProcessingUnit
	virtual TBool IsTypeOf(TUid aTypeId) const;
	virtual TAny* Interface(TUid aType);

	// from base class MAudioGainControlObserver
	void MaxRampTimeChanged(MAudioGainControl& aGain);
	void MaxGainChanged(MAudioGainControl& aGain);
	void GainChanged(MAudioGainControl& aGain, TInt aError);

	/**
	Create a new instance.

	@param aGaincontrol the gain implementation. Ownership of the pointer
			is transferred to the  create CAudioGainControl object instance.
	@return CAudioGainControl* a pointer to the created instance.
	*/
	static CLogicalAudioGainControl* NewL(TAny* aParam);


private:

	void ConstructL();

	CLogicalAudioGainControl(TAny* aParam);

protected:
	RArray<TAudioChannelGain> iActiveChannels;

	RArray<TAudioChannelGain> iDesiredChannels;

	TTimeIntervalMicroSeconds iDesiredRampTime;

	TUid iDesiredRampOperation;

private:
	// Maximum value for ramp time
	TTimeIntervalMicroSeconds iMaxRampTime;

	// The adaptation gain control
	MAudioGainControl* iAdaptationGain;

	RPointerArray<MAudioGainControlObserver> iGainObservers;
	};

#endif // LOGICALAUDIOGAINCONTROL_H
