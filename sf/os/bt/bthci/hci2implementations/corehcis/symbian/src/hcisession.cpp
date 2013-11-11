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
// HCI server session.
// 
//

/**
 @file
 @internalComponent
*/

#include "hcisession.h"
#include "hciserver.h"
#include "corehciutil.h"
#include "hciserverrequestmgr.h"

#include <bluetooth/hci/hciipc.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCISERVER);
#endif

CHCISession* CHCISession::NewL(CHCIServer& aServer)
	{
	LOG_STATIC_FUNC
	
	CHCISession* self = new(ELeave) CHCISession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHCISession::CHCISession(CHCIServer& aServer)
 :	iServer(aServer)
	{
	LOG_FUNC
	
	iServer.OpenClientReference();
	}

void CHCISession::ConstructL()
	{
	LOG_FUNC
	}

CHCISession::~CHCISession()
	{
	LOG_FUNC
	
	if(iService)
		{
		// only release the service if we acquired it.
		iServer.ReleaseService(iService);
		}
	
	iServer.CloseClientReference();
	}

void CHCISession::ServiceL(const RMessage2& aMessage)
	{
	LOG_FUNC;
	LOG1(_L8("\taMessage.Function() = %d"), aMessage.Function());
	
	// This ServiceL only handles the connect to service function, any other
	// service functions are delegated to the actual services.
	if(aMessage.Function() == EConnectToService)
		{
		if(!iService)
			{
			TRAPD(err, iService = iServer.ConnectToServiceL(aMessage.Int0()));
			if(err == KErrNone)
				{
				// We have successfully connected to the service
				__ASSERT_DEBUG(iService, PANIC(KHciServerPanicCat, ENoServiceProvidedOnConnect));
				aMessage.Complete(KErrNone);
				}
			else if(err == KErrInUse)
				{
				// Service is in use so error the request.
				aMessage.Complete(KErrInUse);
				}
			else
				{
				// Some other error meaning the client has made a bad request
				// so punish them
				PanicClient(aMessage, EBadServiceConnectRequest);
				}
			}
		else
			{
			// We are already open and have a service, so the client is
			// misbehaving - punish them
			PanicClient(aMessage, EServiceAlreadyConnected);
			}
		}
	else
		{
		// Actually use the service.
		if(iService)
			{
			iService->ServiceL(*this, aMessage);
			}
		else
			{
			// We need to have been connected to a service, if we haven't
			// then the client is misbehaving.
			PanicClient(aMessage, EServiceNotConnectedYet);
			}
		}
	}
