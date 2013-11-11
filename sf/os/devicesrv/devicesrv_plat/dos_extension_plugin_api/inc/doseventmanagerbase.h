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
*    This file describes the class CEventManagerBase.
*
*/


#ifndef __DOSEVENTMANAGERBASE_H__
#define __DOSEVENTMANAGERBASE_H__

class CDosEventRcvService;
class CDosEvent;

/**
* This class provides with the general functionality of a general purpose event manager.
*/
class CEventManagerBase : public CBase
{
public:
	/**
	* Constructor.
	*/
	CEventManagerBase();

	/**
	* Destructor.
	*/
	~CEventManagerBase();

	/**
	* Raises and event contained in the client-server message.
	* @param aMessage Client-server message containing the request.
	* @return Symbian error code.
	*/
	TInt RaiseEvent(const RMessage2& aMessage);

	/**
	* Registers a listener to listen to an event.
	* @param aListener The listener.
	* @param aEvent The event.
	* @return Symbian error code.
	*/
	TInt RegisterListenerL(CDosEventRcvService* aListener,TUint aEvent); 

	/**
	* The listener doesn't want to listen for event anymore.
	* @param aListener The listener.
	* @param aEvent The event.
	* @return Symbian error code.
	*/
	TInt UnRegisterListener(CDosEventRcvService* aListener, TUint aEvent); 

private:
	/**
	* It looks for an event id in the event array.
	* @param aEventName The event id to look for.
	* @param aEventPtr It will contain a pointer to the event if found or NULL if not.
	*/
	void LookForEventInArray(TUint aEventName,CDosEvent*& aEventPtr);

protected:
	/**
	* Raises and event.
	* @param aEvent The evetn id.
	* @param aParameter A pointer to the event's parameter.
	* @param aParLength Length of the parameter.
	* @return Symbian error code.
	*/
	TInt RaiseEvent(TUint aEvent,TAny* aParameter=NULL,TInt aParLength=0);

private:
	//A list of events with at least 1 client registered
	RPointerArray<CDosEvent> iEventArray;

	//Used to maintain iEventArray in order.
	TLinearOrder<CDosEvent> iOrder;
};

#endif // __DOSEVENTMANAGERBASE_H__
