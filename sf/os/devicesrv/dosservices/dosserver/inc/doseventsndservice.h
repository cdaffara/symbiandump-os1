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
*    This is the definition for CDosEventSndService class.
*
*/



#ifndef __DOSEVENTSNDBASE_H__
#define __DOSEVENTSNDBASE_H__

#include "dosdsy.h"
#include "dosservice.h"

class CEventManagerBase;

/**
* Server side object to send events. It corresponds with RDosEventSnd class in
* the client side.
*/
NONSHARABLE_CLASS( CDosEventSndService ) : public CDosService
{
public:
	/**
	* NewL function that creates the object.
	* @param aEventManager A pointer to the DosServer's event manager.
	* @return A pointer to the newly created object.
	*/
	static CDosEventSndService* NewL(CEventManagerBase* aEventManager);

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

private:
	//Pointer to the DosServer's event manager.
	CEventManagerBase* iEventManager;
};


#endif //__DOSEVENTSNDBASE_H__
