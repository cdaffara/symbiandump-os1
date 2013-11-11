/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This is the definition for CDosEvent class.
*
*/


#ifndef __DOSEVENT_H__
#define __DOSEVENT_H__

#include "doseventrcvservice.h"

class CDosEventRcvService;
class CDosWaitingEvent;

/**
* It represents an event type that has at least one listener. It forms part of a list of
* of events that is checked by the EventManager when an event arrives to the DosServer.
* It maintains a list of listeners wishing to receive the type of event.
*/
NONSHARABLE_CLASS( CDosEvent ) : public CBase
{
public:
	/**
	* Constructor.
	*/
	inline CDosEvent();

	/**
	* Destructor.
	*/
	~CDosEvent();

	/**
	* NewL function that creates the object.
	* @param aEvent Unique Id of the event.
	* @return A pinter to the newly created object.
	*/
	static CDosEvent* NewL(TUint aEvent);

	/**
	* The function compares two events. It's used to maintain an order in the Event array owned by 
	* CEventManagerBase.
	* @param aFirst One event.
	* @param aSecond The other event.
	* @return -1 if first < second, 1 if first > second or 0 if equal.
	*/
	static TInt Compare(const CDosEvent& aFirst,const CDosEvent& aSecond);

	/**
	* Returns the event Id.
	* @return Event Id.
	*/
	inline TUint Event() const;
	
	/**
	* Returns the number of listeners registered to this event.
	* @return Number of listeners.
	*/
	inline TInt ListenerCount() const;

	/**
	* Adds a listener to the event's list.
	* @param aListener A pointer to the listener to be added.
	*/
	void AddListener(CDosEventRcvService* aListener);

	/**
	* Removes a listener from the event's list.
	* @param aListener A pointer to the listener to be removed.
	*/
	void RemoveListener(CDosEventRcvService* aListener);

	/**
	* It delivers an event to all the listeners in the event's list.
	* @param aEvent A pointer to the event.
	*/
	void InformListeners(CDosWaitingEvent* aEvent);

private:
	/**
	* Symbian two-phased constructor.
	* @param aEvent Unique Id of the event type.
	*/
	void ConstructL(TUint aEvent);

	/**
	* It tells if a certain listener is registered for this event type.
	* @param aListener A pointer to the listener to be checked.
	* @return ETrue if the listener is on the list, EFalse otherwise.
	*/
	TBool ListenerRegistered(CDosEventRcvService* aListener);

private:
	//A list of clients listening to this event type.
	TSglQue<CDosEventRcvService> iListenerList; 

	//Use to browser trough the iListenerList.
    TSglQueIter<CDosEventRcvService> iListenerIter;

	//Id of the event.
	TUint iEventName;

	//Number of listeners in the list.
	TInt iListenerCount;
};

#include "dosevent.inl"

#endif // __DOSEVENT_H__
