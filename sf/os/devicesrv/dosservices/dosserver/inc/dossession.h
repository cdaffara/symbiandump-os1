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
*    This is the definition for CDosSession class.
*
*/


#ifndef __DOSSESSION_H__
#define __DOSSESSION_H__

#include "dosserver.h"

class CDosService;

/**
* This class represents a server side session.
*/
NONSHARABLE_CLASS( CDosSession ) : public CSession2
{
public:
	/**
	* Constructor.
	*/
	CDosSession();

	/**
	* Destructor.
	*/
	~CDosSession();

	/**
	* 2nd phase constructor called by the CServer framework.
	*/
	void CreateL();

private:
	/**
	* Returns the server that owns the session.
	* @return A reference to the server.
	*/
	inline CDosServer& Server() const;

	/**
	* This function is called by the CServer framework and handles
	* the requests. It just passes them to DispatchMessageL and complete them
	* if they are auto-completeable
	* @param aMessage Client-server message containing the request.
	*/
	void ServiceL(const RMessage2& aMessage);

	/**
	* Sends the request to the proper handler based on the message function.
	* @param aMessage Client-server message containing the request.
	* @return Symbina error code.
	*/
	TInt DispatchMessageL(const RMessage2& aMessage);

	/**
	* Gets a service object using a handle.
	* @param aHandle Handle assigned to the service by the Server framework,
	* @return A pointer to the Service object.
	*/
	inline CDosService* ServiceFromHandle(const TInt aHandle) const;

	/**
	* Sets up a subsession. Assigns it a handle.
	* @param aHandle Handle assigned to the service by the Server framework,
	* @param aService The Service object.
	*/
	void SetupSubSessionL(const RMessage2& aMessage,CDosService* aService);

	/**
	* Closes a subsession.
	* @param aHandle Handle assigned to the service by the Server framework,
	*/
	void CloseSubSession(const RMessage2& aMessage);
	
private:
	friend class CDosServer;

private:
	//A container for Services.
	CObjectCon* iServiceCon;

	//An index for services.
	CObjectIx* iServices;
	TInt iSDCounter;
};

#include "dossession.inl"


#endif //__DOSSESSION_H__
