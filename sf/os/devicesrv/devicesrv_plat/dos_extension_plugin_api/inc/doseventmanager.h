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
*    This file describes the class CDosEventManager.
*
*/


#ifndef __DOSEVENTMANAGER_H__
#define __DOSEVENTMANAGER_H__

#include <DosSvrServices.h>
#include "doseventmanagerbase.h"

/**
* This is a class that derives from CEventManagerBase and it contains 
* functions to launch the defined Dos events.
*/
class CDosEventManager : public CEventManagerBase
{
public:
	/**
	* NewL function that creates the object.
	* @return The newly created object.
	*/
	static CDosEventManager* NewL();

	//======================================================
	// Events that can be launched.
	// Look into the definition of CDosEventListenerBase 
	// in DosSvrServices.h for a description of these.
	//======================================================

	IMPORT_C TInt GeneralEvent(TInt32 aEvent,TAny* aParameter,TInt aParSize);
};

#endif // __DOSEVENTMANAGER_H__
