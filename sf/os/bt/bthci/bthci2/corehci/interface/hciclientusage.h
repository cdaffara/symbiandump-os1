// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef HCICLIENTUSAGE_H
#define HCICLIENTUSAGE_H

#include <e32def.h>

class MHCIClientUsage;

/** 
The UID of the HCI Client Usage Callback API. If this API ever has to change,
a new UID and associated M- class will be created. 

Old (non-updated) HCI Client Callback Plugins will still work as long as the old API 
is supported
*/
const TInt KHCIClientUsageCallbackUid = 0x10286FCD;

/**
Mixin for the HCI Client Usage Callback API.
This interface is to be used via CCoreHCIPlugin.
@see CCoreHCIPlugin::Interface(TUid)
*/
class MHCIClientUsageCallback
	{
public:
	/**
	Pass the Client Usage interface into the HCI.

	@param A pointer to a newly constructed instance representing the command frame.
	*/
	virtual void MhcucSetClientUsage(MHCIClientUsage& aClientUsage) = 0;
	};

/**
Mixin for being informed about when the HCI has an external reference that should
stop the Bluetooth stack, and hence the HCI, from being unloaded. This can be used
by the HCI Server for example when it has active connections.
*/
class MHCIClientUsage
	{
public:
	/**
	This method will be called to indicate a new external client.
	*/
	virtual void MhcuOpenClientReference() = 0;

	/**
	This method will be called to indicate an external client has closed. For each
	call to this method a corresponding call to MhcuOpenClientReference must have been
	made previously.
	*/
	virtual void MhcuCloseClientReference() = 0;
	};

#endif // HCICLIENTUSAGE_H
