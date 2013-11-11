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

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <elements/sd_minterceptmsgs.h>

using namespace Den;

//
// ECOM implementation
//

TAny* NewMessageL(TAny* aParams)
	{
	TInt32 type = reinterpret_cast<TInt32>(aParams);
	switch (type)
		{
	case ECheckFallibleMsgModeResponseMsg:
		return TCheckFallibleMsgModeResponseMsg::NewL();
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}


const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KMessageInterceptResponseMsgImplementationUid, NewMessageL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy (TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}




