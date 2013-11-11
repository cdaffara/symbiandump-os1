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

#ifndef TARGETSELECTORPLUGIN_H
#define TARGETSELECTORPLUGIN_H

#include <e32base.h>

class MRemConTargetSelectorPluginObserver;
class TRemConAddress;
class TClientInfo;
class TBearerSecurity;

/**
The UID of the Target Selector Plugin interface.
*/
const TInt KRemConTargetSelectorInterfaceUid = 0x10204B28;

/**
Abstract base class for target selector plugins.
*/
class CRemConTargetSelectorPlugin : public CBase
	{
public:
	/** 
	Constructor.
	@param aImplementationUid The UID of the implementation.
	@param aObserver The observer of the TSP.
	*/
	IMPORT_C static CRemConTargetSelectorPlugin* NewL(TUid aImplementationUid, 
		MRemConTargetSelectorPluginObserver& aObserver);

	/** Destructor. */
	IMPORT_C ~CRemConTargetSelectorPlugin();

public:
	/**
	Called by RemCon server to get a pointer to an object which implements the 
	TSP interface with UID aUid. This is a mechanism for allowing future 
	change to the TSP API without breaking BC in existing (non-updated) 
	TSPs.
	*/
	virtual TAny* GetInterface(TUid aUid) = 0;

protected:
	/** 
	Constructor.
	@param aObserver The observer of the TSP.
	*/
	IMPORT_C CRemConTargetSelectorPlugin(MRemConTargetSelectorPluginObserver& aObserver);

protected:
	/**
	Accessor for the observer.
	@return The observer.
	*/
	IMPORT_C MRemConTargetSelectorPluginObserver& Observer();

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

private: // unowned
	MRemConTargetSelectorPluginObserver& iObserver;
	};

#endif // TARGETSELECTORPLUGIN_H
