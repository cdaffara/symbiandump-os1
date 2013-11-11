/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This is the definition for CDosEventRcvService class.
*
*/


#ifndef __DOSEVENTRCVSERVICE_H__
#define __DOSEVENTRCVSERVICE_H__

#include <DosSvrServices.h>
#include "dosservice.h"

class CDosWaitingEvent;
class CEventManagerBase;

/**
* This class represents a listener registerd to receive certain event
* type. It forms part of the CDosEvent list.
*/
NONSHARABLE_CLASS( CDosEventRcvService ) : public CDosService
{
public:
	/**
	* Constructor.
	*/
	CDosEventRcvService();

	/**
	* Destructor.
	*/
	~CDosEventRcvService();

	/**
	* NewL function that creates the object.
	* @param aEventManager A pointer to the DosServer's Event manager.
	* @return A pointer to the newly created object.
	*/
	static CDosEventRcvService* NewL(CEventManagerBase* aEventManager);

	/**
	* This function is used by the Event Manager to inform the listener
	* about an event.
	* @param aEvent The incoming event.
	*/
	void InformListener(CDosWaitingEvent* aEvent);

private:
	/**
	* Symbian two-phased constructor.
	* @param aEventManager A pointer to the DosServer's event manager.
	*/
	void ConstructL(CEventManagerBase* aEventManager);

	/**
	* Processes the message sent by the client.
	* @param aMessage Client-server message.
	* @return Symbian error code.
	*/
	TInt ExecuteMessageL(const RMessage2& aMessage);

public:
	// To be part of the list for an event
	TSglQueLink iLink; 

private:
	//Pointer to the DosServer's event manager.
	CEventManagerBase* iEventManager;

	//Client-Server message.
	RMessage2 iMessage;

	//Event Id.
	TUint iEvent;

	//Informs if the Service is registered in any Event's list.
	TBool iRegistered; 

	//Informs if the Service is ready to process an Event.
	TBool iReady; 

	//Tells what kind of queue does the event have.
	TQueueType iQueue; 

	//A Queue of events waiting to be processed.
	TSglQue<CDosWaitingEvent> iEventQueue; 

	//Used to browse iEventQueue.
    TSglQueIter<CDosWaitingEvent> iQueueIter;
};

#endif // __DOSEVENTRCVSERVICE_H__
