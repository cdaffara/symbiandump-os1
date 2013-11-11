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

#include "cmockmsgrouter.h"
#include "cmockmsghandler.h"

#include "cmockltsyengine.h"
#include "cmocksyserver.h"
#include "rmockltsy.h"

/**
Factory function
*/
EXPORT_C CMmMessageRouter* CMmMessageRouter::NewL(CMmMessageManager* aMessageManager)
	{
	CMmMessageRouter* self = new (ELeave)CMmMessageRouter(aMessageManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CMmPhoNetSender* CMmMessageRouter::GetPhoNetSender()
	{
	return NULL;
	}
EXPORT_C CMmPhoNetReceiver* CMmMessageRouter::GetPhoNetReceiver()
	{
	return NULL;
	}

/**
Constructor
*/	
CMmMessageRouter::CMmMessageRouter(CMmMessageManager* aMessageManager)
	: iMessageManager(aMessageManager)
	{
	}

/**
2nd phase construction
*/	
void CMmMessageRouter::ConstructL()
	{
	// create Mock Engine and Server
	iEngine = CMockLtsyEngine::NewL(iMessageManager);
	iServer = CMockSYServer::NewL(KMockLtsyServerName, CActive::EPriorityHigh, *iEngine);

	iMsgHandler = CMockMsgHandler::NewL(this);
	}
	
/**
Destructor
*/	
CMmMessageRouter::~CMmMessageRouter()
	{
	delete iServer;
	delete iEngine;
	}
	
/**
Returns pointer to the message handler for this request, based on IPC number
@param aIpc
@return Pointer to the message handler
*/
MMmMessHandlerBase* CMmMessageRouter::RouteRequest(TInt aIpc)
	{
	return iMsgHandler;
	}
