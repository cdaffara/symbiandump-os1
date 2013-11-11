// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef CONTROLLERINITIALISATIONPLUGIN_H
#define CONTROLLERINITIALISATIONPLUGIN_H

#include <e32base.h>

/**
The ECOM interface definition for the Controller Initialisation Plugin.
*/
class CControllerInitialisationPlugin : public CBase
	{
public:
	/**
	Attempt to load specified implementation
	*/
	IMPORT_C static CControllerInitialisationPlugin* NewL(TUid aImplementationUid);

	/**
	Attempt to load single instance implementation
	*/
	IMPORT_C static CControllerInitialisationPlugin* NewL();
	
	/** 
	Destructor. 
	All interfaces obtained through the Interface will be deleted.
	*/
	IMPORT_C ~CControllerInitialisationPlugin();

	/**
	Retrieves an instance of an interface to functionality provided by the 
	Initialisation plugin.
	
	@param aUid UID of the API to be retrieved.
	@return a pointer to an implementation of the interface given by aUid. This 
		does not transfer ownership of the implmentation.
	*/
	virtual TAny* Interface(TUid aUid) = 0;

protected:
	IMPORT_C CControllerInitialisationPlugin();

private:
	/**
	UID set by ECOM when the instance is created. Used when the instance is
	destroyed.
	*/
	TUid iInstanceId;

	/**
	Pad for BC-friendly future change.
	*/
	TAny* iPad;
	};

#endif // CONTROLLERINITIALISATIONPLUGIN_H
