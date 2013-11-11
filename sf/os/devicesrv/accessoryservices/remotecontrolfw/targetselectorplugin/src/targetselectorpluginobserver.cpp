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
 @internalComponent
*/

#include <remcon/remcontargetselectorpluginobserver.h>


EXPORT_C void MRemConTargetSelectorPluginObserver::OutgoingNotifyCommandAddressed(TRemConAddress* aConnection, TInt aError)
	{
	MrctspoDoOutgoingNotifyCommandAddressed(aConnection, aError);
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::OutgoingCommandAddressed(TInt aError)
	{
	MrctspoDoOutgoingCommandAddressed(aError);
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::OutgoingCommandPermitted(TBool aIsPermitted)
	{
	MrctspoDoOutgoingCommandPermitted(aIsPermitted);
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::OutgoingNotifyCommandPermitted(TBool aIsPermitted)
	{
	MrctspoDoOutgoingNotifyCommandPermitted(aIsPermitted);
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::IncomingCommandAddressed(TInt aError)
	{
	MrctspoDoIncomingCommandAddressed(aError);
	}

EXPORT_C TInt MRemConTargetSelectorPluginObserver::GetConnections(TSglQue<TRemConAddress>& aConnections)
	{
	return MrctspoDoGetConnections(aConnections);
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::OutgoingResponsePermitted(TBool aIsPermitted)
	{
	MrctspoDoOutgoingResponsePermitted(aIsPermitted);
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::IncomingNotifyAddressed(TClientInfo* aClientInfo, TInt aError)
	{
	MrctspoDoIncomingNotifyAddressed(aClientInfo, aError);
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::ReAddressNotifies()
	{
	MrctspoDoReAddressNotifies();
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::IncomingCommandPermitted(TBool aIsPermitted)
	{
	MrctspoDoIncomingCommandPermitted(aIsPermitted);
	}

EXPORT_C void MRemConTargetSelectorPluginObserver::IncomingNotifyPermitted(TBool aIsPermitted)
	{
	MrctspoDoIncomingNotifyPermitted(aIsPermitted);
	}

EXPORT_C TInt MRemConTargetSelectorPluginObserver::SetLocalAddressedClient(const TUid& aBearerUid, const TClientInfo& aClientInfo)
	{
	return MrctspoSetLocalAddressedClient(aBearerUid, aClientInfo);
	}


