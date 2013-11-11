// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REMCONBEARERPLUGIN_H
#define REMCONBEARERPLUGIN_H

#include <e32base.h>

class MRemConBearerObserver;
class TBearerParams;
class TRemConAddress;

/** 
The UID of the RemCon bearer interface. Implementors of new bearers are 
implementing this interface. The bearer interface is simply a means of 
supplying implementations of the 'bearer API' to RemCon. The bearer API may 
change over time, so this extension pattern is used.
*/
const TInt KRemConBearerInterfaceUid = 0x10204546;

/**
Base class for bearer plugins.
*/
class CRemConBearerPlugin : public CBase
	{
public: // used by RemCon to create and destroy bearer instances
	/** 
	Constructor.
	@param aParams Parameters from RemCon, used to initialise the 
	implementation.
	@return Ownership of a new bearer plugin.
	*/
	IMPORT_C static CRemConBearerPlugin* NewL(TBearerParams& aParams);

	/** Destructor. */
	IMPORT_C ~CRemConBearerPlugin();

public:
	/**
	Called by RemCon server to get a pointer to an object which implements the 
	bearer API with UID aUid. This is a mechanism for allowing future change 
	to the bearer API without breaking BC in existing (non-updated) bearer 
	plugins.
	@param aUid The UID of a bearer API
	@return A pointer to an object which implements the interface identified 
	by aUid or NULL if the interface is not implemented.
	*/
	virtual TAny* GetInterface(TUid aUid) = 0;

	/**
	Called by RemCon to query the bearer's implementation UID.
	@return The bearer's implementation UID.
	*/
	IMPORT_C TUid Uid() const;

protected: // called by concrete bearer plugins
	/** 
	Constructor
	@param aParams Parameters from RemCon, used to initialise the 
	implementation.
	*/
	IMPORT_C CRemConBearerPlugin(TBearerParams& aParams);

	/**
	Accessor for the observer.
	@return The observer.
	*/
	IMPORT_C MRemConBearerObserver& Observer();

private: // unowned
	MRemConBearerObserver& iObserver;

private: // owned
	/**
	UID set by ECOM when the instance is created. Used when the instance is 
	destroyed.
	*/
	TUid iInstanceId;

	/**
	Implementation UID of concrete instance.
	*/
	const TUid iImplementationUid;

	/** 
	Pad for BC-friendly future change.
	*/
	TAny* iPad;
	};

#endif // REMCONBEARERPLUGIN_H
