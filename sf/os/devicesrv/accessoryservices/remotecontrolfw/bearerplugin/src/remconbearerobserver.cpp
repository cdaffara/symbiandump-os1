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

#include <remcon/remconbearerobserver.h>

EXPORT_C TInt MRemConBearerObserver::NewResponse(const TRemConAddress& aAddr)
	{
	return MrcboDoNewResponse(aAddr);
	}

EXPORT_C TInt MRemConBearerObserver::NewNotifyResponse(const TRemConAddress& aAddr)
	{
	return MrcboDoNewNotifyResponse(aAddr);
	}
	
EXPORT_C TInt MRemConBearerObserver::NewCommand(const TRemConAddress& aAddr)
	{
	return MrcboDoNewCommand(aAddr);
	}

EXPORT_C TInt MRemConBearerObserver::NewNotifyCommand(const TRemConAddress& aAddr)
	{
	return MrcboDoNewNotifyCommand(aAddr);
	}


EXPORT_C TInt MRemConBearerObserver::ConnectIndicate(const TRemConAddress& aAddr)
	{
	return MrcboDoConnectIndicate(aAddr);
	}
	
EXPORT_C void MRemConBearerObserver::DisconnectIndicate(const TRemConAddress& aAddr)
	{
	MrcboDoDisconnectIndicate(aAddr);
	}
	
EXPORT_C TInt MRemConBearerObserver::ConnectConfirm(const TRemConAddress& aAddr, TInt aError)
	{
	return MrcboDoConnectConfirm(aAddr, aError);
	}
	
EXPORT_C void MRemConBearerObserver::DisconnectConfirm(const TRemConAddress& aAddr, TInt aError)
	{
	MrcboDoDisconnectConfirm(aAddr, aError);
	}
	
EXPORT_C TInt MRemConBearerObserver::InterfaceToBearer(TUid aBearerUid,
		TUid aInterfaceUid, 
		TUint aOperationId,
		const TDesC8& aData,
		TRemConMessageType aMsgType, 
		TDes8& aBearerData) const
	{
	return MrcboDoInterfaceToBearer(aBearerUid,
		aInterfaceUid, 
		aOperationId,
		aData,
		aMsgType, 
		aBearerData);
	}
	
EXPORT_C TInt MRemConBearerObserver::BearerToInterface(TUid aBearerUid,
		const TDesC8& aInterfaceData, 
		const TDesC8& aBearerData,
		TUid& aInterfaceUid, 
		TUint& aOperationId,
		TRemConMessageType& aMsgType, 
		TDes8& aData) const
	{
	return MrcboDoBearerToInterface(aBearerUid,
		aInterfaceData,
		aBearerData,
		aInterfaceUid, 
		aOperationId,
		aMsgType, 
		aData);
	}
	
EXPORT_C TUint MRemConBearerObserver::NewTransactionId()
	{
	return MrcboDoNewTransactionId();
	}

EXPORT_C void MRemConBearerObserver::CommandExpired(TUint aTransactionId)
	{
	return MrcboDoCommandExpired(aTransactionId);
	}

EXPORT_C TInt MRemConBearerObserver::NewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	return MrcboDoNewCommand(aAddr, aClient);
	}

EXPORT_C TInt MRemConBearerObserver::NewNotifyCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	return MrcboDoNewNotifyCommand(aAddr, aClient);
	}

EXPORT_C TInt MRemConBearerObserver::SupportedInterfaces(const TRemConClientId& aId, RArray<TUid>& aUids)
	{
	return MrcboDoSupportedInterfaces(aId, aUids);
	}

EXPORT_C TInt MRemConBearerObserver::SupportedOperations(const TRemConClientId& aId, TUid aInterfaceUid, RArray<TUint>& aOperations)
	{
	return MrcboDoSupportedOperations(aId, aInterfaceUid, aOperations);
	}

EXPORT_C void MRemConBearerObserver::SetRemoteAddressedClient(const TUid& aBearerUid, const TRemConClientId& aId)
	{
	MrcboDoSetRemoteAddressedClient(aBearerUid, aId);
	}

EXPORT_C TInt MRemConBearerObserver::RegisterLocalAddressedClientObserver(const TUid& aBearerUid)
	{
	return MrcboDoRegisterLocalAddressedClientObserver(aBearerUid);
	}

EXPORT_C TInt MRemConBearerObserver::UnregisterLocalAddressedClientObserver(const TUid& aBearerUid)
	{
	return MrcboDoUnregisterLocalAddressedClientObserver(aBearerUid);
	}


