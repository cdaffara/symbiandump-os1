// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CSIMREDUCETIMERS_H__
#define __CSIMREDUCETIMERS_H__

/**
 * @file
 *
 * Definition of the observer pattern, which is used in the simulator,
 * to give the functionality of reducing the timers that are used
 * throughout.
 */

#include <simtsy.h>
#include "CSimPubSub.h"


const TInt KTimerDelayOnReduceTimeSignal = 3;		//< Constant value of seconds delay to give timers before stopping.

/**
Definition of the observer required for a CSimTimer object.
*/
class MSimTimerUpdateObserver
	{
public:
	/**
	Method to be used for updating the timer associated with this
	observer.

	@param aSecsOffTimer The number of seconds to reduce off the
	                     remaining time duration in this objects
	                     corresponding timer.
	*/
	virtual void Update(TInt aSecsOffTimer) =0;
	/**
	Method for obtaining information regarding this observer, which in
	this case is the time duration remaining on the corresponding
	timer.

	@return The number of seconds remaining on this object's
	        corresponding timer.
	*/
	virtual TInt GetRemainingSeconds() const =0;
	/**
	Method for obtaining information regarding this observer, which in
	this case is the event assocated with the timer
	
	@return The timer event ID as defined in TTimerEventId in simtsy.h
	*/
	virtual TInt GetTimerEventInfo() const =0;
	};

/**
Defines the requirements of a subject for observing and updating
CSimTimer objects.  This is particularly useful for reducing the time
durations originally given to the simulator.
*/
class MSimTimersSubject
	{
public:
	/**
	Method used to remove an observer, aObserver, from this
	object's list of attached/registered observers.

	@param aObserver The observer to add to this subject's list of
	                 observers.
	*/
	virtual void AttachL(MSimTimerUpdateObserver* aObserver) =0;
	/**
	Method used to remove an observer, aObserver, from this
	object's list of attached/registered observers.

	@param aObserver The observer to remove from this subject's list of
	                 observers.
	*/
	virtual void DetachL(MSimTimerUpdateObserver* aObserver) =0;

private:
	/**
	Method for notifying all attached/registered observers that a
	change has occurred.
	*/
	virtual void Notify() =0;
	/**
	Method for notifying an attached/registered observer with specific
	timer event ID and lowest remaining time, that a change has occurred
	*/
	virtual void Notify(TInt aTimerEventId) =0;
	};


/**
Implementation of MSimTimersSubject, used to listen to events
indicating a request to reduce the timers running in the simulator.
This implementation currently uses the Publish & Subscribe API to get a
notification of when a particular property is changed.  On
notification, all registered timer observers will be required to reduce
their timer's duration by the minimum time duration of all these
timers.

The notification must be made by changing a specific property, category
KUidPSSimTsyCategory and sub-key KPSSimTsyTimersReduceTime, to the
value KReduceSimTsyTimers.  (These constants are defined in "SimTsy.h")

Example:

@code
// Optional, but safe to attempt defining property.
TInt ret = RProperty::Define(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime,
								KPSSimTsyTimersReduceTimeSignalKeyType);
if (ret != KErrNone && ret != KErrAlreadyExists)
	{
	User::Leave(ret);
	}

RProperty stopTimerProperty;
// User's code must create a property handle attached to a property with the
// category KUidPSSimTsyCategory and sub-key KPSSimTsyTimersReduceTime.
User::LeaveIfError(stopTimerProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime));
CleanupClosePushL(stopTimerProperty);

//... Use the simulator.

// The property must then be set to KReduceSimTsyTimers in order to indicate that
// the simulator is no longer required to continue running and thus timers can be reduced.
User::LeaveIfError(stopTimerProperty.Set(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime,
											KReduceSimTsyTimers));

CleanupStack::PopAndDestroy(&stopTimerProperty);
@endcode
*/
class CSimReduceTimers : public CBase, MPSSimObserver, MSimTimersSubject
	{
public:
	static CSimReduceTimers* NewL();
	~CSimReduceTimers();
	void SimPSEvent(const CSimPubSub::TPubSubProperty aProperty, TInt aStatus);

	void AttachL(MSimTimerUpdateObserver* aObserver);
	void DetachL(MSimTimerUpdateObserver* aObserver);

private:
	CSimReduceTimers();
	void ConstructL();
	void Notify();
	void Notify(TInt aTimerEventId);
private:
	/** The Publish & Subscribe notification object.  This will call
	    the SimPSEvent() method when the specified property
	    (iPSProperty) is changed.*/
	CSimPubSub* iSimPubSub;
	/**	The property this object subscribes to, in order to listen to
	    value changes indicating a request for timers to be reduced.*/
	CSimPubSub::TPubSubProperty iPSProperty;
	/**	Maintains a list of timers currently active, which can
	    therefore have their duration reduced.*/
	RPointerArray<MSimTimerUpdateObserver> iObservers;
	};

#endif
