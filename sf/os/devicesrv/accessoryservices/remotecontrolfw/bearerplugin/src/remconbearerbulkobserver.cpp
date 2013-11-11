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



/**
 @file
 @internalComponent
*/

#include <remcon/remconbearerbulkobserver.h>

EXPORT_C TInt MRemConBearerBulkObserver::NewCommand(const TRemConAddress& aAddr)
	{
	return MrcbboDoNewCommand(aAddr);
	}

EXPORT_C TInt MRemConBearerBulkObserver::NewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	return MrcbboDoNewCommand(aAddr, aClient);
	}

EXPORT_C TUint MRemConBearerBulkObserver::NewTransactionId()
	{
	return MrcbboDoNewTransactionId();
	}

EXPORT_C void MRemConBearerBulkObserver::CommandExpired(TUint aTransactionId)
	{
	return MrcbboDoCommandExpired(aTransactionId);
	}

EXPORT_C TInt MRemConBearerBulkObserver::SetAddressedClient(const TRemConAddress& aAddr, const TRemConClientId& aClientId)
	{
	return MrcbboDoSetAddressedClient(aAddr, aClientId);
	}

EXPORT_C TInt MRemConBearerBulkObserver::RemoveAddressing(const TRemConAddress& aAddr)
	{
	return MrcbboDoRemoveAddressing(aAddr);
	}

