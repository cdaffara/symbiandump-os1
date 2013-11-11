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

#ifndef HCTLBASE_H
#define HCTLBASE_H

#include <e32base.h>

/**
The ECOM interface definition for the HCTL Plugin.

This class is intended to be derived from.
*/
class CHCTLBase : public CBase
	{
public:
	/**
	Attempt to load specified implementation
	*/
	IMPORT_C static CHCTLBase* NewL(TUid aImplementationUid);

	/**
	Attempt to load single instance implementation
	*/
	IMPORT_C static CHCTLBase* NewL();
    IMPORT_C ~CHCTLBase();

public:
	/**
	Retrieves an instance of an interface to functionality provided by the HCTL.
	
	@param aUid UID of the API to be retrieved.
	@return A pointer to the instance of the interface specified by aUid.
	*/
	virtual TAny* Interface(TUid aUid) = 0;

protected:
    IMPORT_C CHCTLBase();

private: // owned
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

#endif // HCTLBASE_H

