// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSPV4STUB_H
#define TSPV4STUB_H

/**
@file
@internalComponent
*/

#include <remcon/remcontargetselectorplugininterface.h>

/**
This class provides default implementations of the TSPv4 functions to allow
AVRCP 1.4 to be used in cases where the TSP has not been updated to support
the new functions.  
*/
NONSHARABLE_CLASS(TTspV4Stub) : public MRemConTargetSelectorPluginInterfaceV4
	{
public:
	TTspV4Stub(MRemConTargetSelectorPluginObserver& aObserver);
	
private: // from MRemConTargetSelectorPluginInterfaceV3
	virtual void PermitIncomingCommand(
		TUid aInterfaceUid,
		TUint aOperationId, 
		const TClientInfo& aClient);
	
	virtual void PermitIncomingNotify(
		TUid aInterfaceUid,
		TUint aOperationId, 
		const TClientInfo& aClient);
	
	virtual void SetRemoteAddressedClient(const TUid& aBearerUid, 
			const TClientInfo& aClient);
	
private:
	MRemConTargetSelectorPluginObserver& iObserver;
	};

#endif //TSPV4STUB_H
