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
*    This is the definition for CDosWaitingEvent class.
*
*/


#ifndef __DOSWAITINGEVENT_H__
#define __DOSWAITINGEVENT_H__

//
// CDosWaitingEvent class
//

#include <e32base.h>

/**
* Represents an incoming event that is to be transferred to registered listeners.
*/
NONSHARABLE_CLASS( CDosWaitingEvent ) : public CBase
{
public:
	/**
	* Constructor.
	*/
	CDosWaitingEvent();

	/**
	* Destructor.
	*/
	~CDosWaitingEvent();

	/**
	* It adds itself to a Queue of events.
	* @param aQueue A reference to the queue.
	*/
	void AddMeToQueue(TSglQue<CDosWaitingEvent>& aQueue);

	/**
	* It removes itself from a queue of events.
	* @param aQueue A reference to the queue.
	*/
	void RemoveMeFromQueueD(TSglQue<CDosWaitingEvent>& aQueue);

	/**
	* Tells if the event has a paramater.
	* @return ETrue if it has, EFalse otherwise.
	*/
	inline TBool HasParameter() const;

	/**
	* Returns the event's parameter.
	* @return A buffer containing the parameter.
	*/
	inline const TDesC8& Parameter() const;

	/**
	* Sets the event's parameter. It takes it from a client-server message.
	* @param aMessage Client-server message containing the request.
	*/
	void SetParameterL(const RMessage2& aMessage);

	/**
	* Sets the event's parameter.
	* @param aParameter A pointer to the data.
	* @param aParLength Length of the data.
	*/
	void SetParameterL(TAny* aParameter,TInt aParLength);

	/**
	* Tells if the events has been queued.
	* @return ETrue if it has, EFalse otherwise.
	*/
	inline TBool IsQueued() const;

public:
	//To be part of the waiting list
	TSglQueLink iLink; 
private:
	//The parameter for this event
	HBufC8* iParameter; 

	// Counts in how many queues this object is in.
	TInt iQueueCount; 
};



#include "doswaitingevent.inl"

#endif // __DOSWAITINGEVENT_H__
