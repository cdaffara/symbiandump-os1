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
*    This file defines the class CDosEventSender and RDosEventSnd that are used
*    to raise events using Dos Server from a different process than the one 
*    running the server.   
*
*/


#ifndef __DOSEVENTSENDER_H__
#define __DOSEVENTSENDER_H__

#include <DosSvrServices.h>

/**
* Raises DosServer events from a different thread/proccess.
* This class can't be used directly. Use CDosEventSender class instead
* to fire events.
*/
class RDosEventSnd : public RDosSubSession
{
public:
	/**
	* Registers the Subsession into an already openend session.
	* @param aServer A connected session.
	* @return Symbian error code.
	*/
	TInt Register(RDosServer& aServer);

	/**
	* Sends and event to DosServer.
	* @param aPar Parameter array. Same as the RMessage class uses.
	* @return Symbian error code.
	*/
	TInt SendEvent(TIpcArgs &aParams);
};


/**
* Class to fire events in the DosServer.
*/
class CDosEventSender : public CBase
{
public:
	/**
	* Constructor.
	*/
	CDosEventSender();

	/**
	* Destructor.
	*/
	IMPORT_C ~CDosEventSender();

	/**
	* NewL function that creates object.
	* It uses an already opened session.
	* @param aDosServer An already connected session to the server.
	* @return A pointer to the newly created object.
	*/
	IMPORT_C static CDosEventSender* NewL(RDosServer& aDosServer);

	/**
	* NewL function that creates object.
	* It creates its own Session with the Server.
	* @param aDosServer An already connected session to the server.
	* @return A pointer to the newly created object.
	*/
	IMPORT_C static CDosEventSender* NewL();

	//======================================================
	// Events that can be launched.
	// Look into the definition of CDosEventListenerBase 
	// in DosSvrServices.h for a description of these.
	//======================================================

	IMPORT_C void GeneralEvent(TInt32 aEvent,TAny* aParameter,TInt aParSize);

private:
	/**
	* Symbian two-phased constructor.
	* @param aDosServer A pointer to the Session object.
	*/
	void ConstructL(RDosServer* aDosServer);

private:
	// A pointer to the session object to be used.
	RDosServer* iDosServer;

	// A Event sender subsession object.
	RDosEventSnd iEventSender;

	// Indicates if this object is responsible for destroying iDosServer.
	TBool iDestroyDosServer;
};


#endif // __DOSEVENTSENDER_H__
