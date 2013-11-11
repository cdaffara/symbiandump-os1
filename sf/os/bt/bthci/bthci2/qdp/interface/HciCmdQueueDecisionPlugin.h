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

#ifndef HCICMDQUEUEDECISIONPLUGIN_H
#define HCICMDQUEUEDECISIONPLUGIN_H

#include <e32base.h>

/**
Base class for HCI Command Queue Decision plugins.

This class is intended to be derived from.
*/
class CHCICmdQueueDecisionPlugin : public CBase
	{
public:
	IMPORT_C static CHCICmdQueueDecisionPlugin* NewL(TUid aImplementationUid);
	IMPORT_C static CHCICmdQueueDecisionPlugin* NewL();
	IMPORT_C ~CHCICmdQueueDecisionPlugin();
	
	/**
	Called by Command Queue Manager to get a pointer to an object which implements
	the QDP API with UID aUid. This is a mechanism for allowing future change 
	to this API without breaking BC in existing (non-updated) plugins.
	@param aUid The UID of the interface to find.
	@return a pointer to an implementation of the interface given by aUid. This 
			does not transfer ownership of the implementation.
	*/
	virtual TAny* Interface(TUid aUid) = 0;

protected:
	IMPORT_C CHCICmdQueueDecisionPlugin();
	
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
	
#endif // HCICMDQUEUEDECISIONPLUGIN_H
