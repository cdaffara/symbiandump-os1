// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cmocksyserver.h"
#include "cmocksysession.h"

/**
Factory function.
*/
EXPORT_C CServer2* CMockSYServer::NewL(const TDesC& aName, TInt aPriority, CMockSYEngine& aEngine)
	{
	CMockSYServer* self=new(ELeave) CMockSYServer(aPriority,aEngine);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor
*/
CMockSYServer::CMockSYServer(TInt aPriority, CMockSYEngine& aEngine)
	: CServer2(aPriority,EUnsharableSessions), iEngine(aEngine)
	{
	
	}
	
/**
2nd phase construction 
*/
void CMockSYServer::ConstructL(const TDesC& aName)
	{
	StartL(aName);
	}

/**
Cretae a new client session. This should really check the version number.
*/
CSession2* CMockSYServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
	{
	return CMockSYSession::NewL(iEngine);
	}
