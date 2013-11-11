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

// Include
#include "lbsnetsimserver.h"
#include "lbsnetsimhandlerretriever.h"
#include "lbsnetsimsessionhandler.h"
#include "lbsnetsimsession.h"

/**
*/
CLbsNetSimSession::CLbsNetSimSession(MLbsNetSimHandlerRetriever* aRetriever) :
	iHandler(NULL), iRetriever(aRetriever)
	{
	}


/**
*/
CLbsNetSimSession::~CLbsNetSimSession()
	{
	}

/**
*/
CLbsNetSimServer* CLbsNetSimSession::Server()
	{
	return static_cast<CLbsNetSimServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

/**
*/
void CLbsNetSimSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case EInitialise:
			{
			if (aMessage.Int0() == ETestHandler)
				{
#ifdef _DEBUG
				_LIT(KTestName, "__TEST_HANDLER__");
				iWhich = KTestName;
#endif			
				RetrieveTestHandler();
				}
			else if (aMessage.Int0() == EGatewayHandler)
				{
#ifdef _DEBUG
				_LIT(KGatewayName, "__GATEWAY_HANDLER__");
				iWhich = KGatewayName;
#endif			
				RetrieveGatewayHandler();
				}
			else
				{
				User::Leave(KErrNotSupported);
				}
				
			// Complete the message
			aMessage.Complete(KErrNone);
				
			break;
			} // case
		default:
			{
			ForwardToHandlerL(aMessage);
			}
		} // switch
	}
	
void CLbsNetSimSession::Disconnect(const RMessage2& aMessage)
	{
	iHandler->Disconnected();
	
	iHandler = NULL;
	
	Server()->DecClientCount();
	
	CSession2::Disconnect(aMessage);
	}
	
//
// Private methods

/**
*/
void CLbsNetSimSession::RetrieveTestHandler()
	{
	iHandler = iRetriever->GetTestHandler();
	iHandler->Connected();
	}


/**
*/
void CLbsNetSimSession::RetrieveGatewayHandler()
	{
	iHandler = iRetriever->GetGatewayHandler();
	iHandler->Connected();
	}


/**
*/
void CLbsNetSimSession::ForwardToHandlerL(const RMessage2& aMessage)
	{
	ASSERT(iHandler != NULL);
	
	if (iHandler == NULL)
		{
		User::Leave(KErrNotReady);
		}
		
	iHandler->ServiceL(aMessage);
	}
