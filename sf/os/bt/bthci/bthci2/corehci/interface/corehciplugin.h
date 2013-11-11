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

#ifndef COREHCIPLUGIN_H
#define COREHCIPLUGIN_H

#include <e32base.h>

/**
The ECOM interface definition for the Core HCI Plugin.
This class is intended to be derived from.

This class is designed to be the access point for the stack of all HCI components 
with the exception of the Queue Decision Plugin. This is done by repeated calls 
to the Interface() function below.
*/
class CCoreHCIPlugin : public CBase
	{
public:
	/**
	Attempt to load specified implementation
	*/
	IMPORT_C static CCoreHCIPlugin* NewL(TUid aImplementationUid);
	
	/**
	Attempt to load single instance implementation
	*/
	IMPORT_C static CCoreHCIPlugin* NewL();

	/** 
	Destructor. 
	All interfaces obtained through the Interface will be deleted.
	*/
	IMPORT_C ~CCoreHCIPlugin();

public:
	/**
	Retrieves an instance of an interface to functionality provided by the HCI Layer.
	
	@param aUid UID of the API to be retrieved.
	@return a pointer to an implementation of the interface given by aUid. This 
		does not transfer ownership of the implementation.
	*/
	virtual TAny* Interface(TUid aUid) = 0;

protected:
    IMPORT_C CCoreHCIPlugin();
	
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

#endif // COREHCIPLUGIN_H
