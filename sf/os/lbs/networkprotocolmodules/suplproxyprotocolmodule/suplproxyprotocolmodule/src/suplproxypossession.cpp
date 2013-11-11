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
// suplproxypossession.cpp
// 
//

#include "lbsdevloggermacros.h"
#include "suplproxypossession.h"
#include "suplproxypossubsession.h"
#include "EPos_PrivacyServerPanic.h"
#include "suplproxyutils.h"

// apparently it is standard to store the subsession handle in the 3rd message slot
const TInt KParamSubsession = 3;

// max subsessions = 5 (2xMOLR + 3xMTLR)
const TInt KMaxPosSubsessions = 5;

/**
Constructor.

@param aProtocol Interface to send supl proxy requests to.
*/
CSuplProxyPosSession::CSuplProxyPosSession(MSuplProxyPosProtocol& aPosProtocol, TInt aRefLocSourceId):
iPosProtocol(aPosProtocol), iRefLocSourceId(aRefLocSourceId)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSession::CSuplProxyPosSession() Begin/End\n");
	}


/**
Destructor.
*/
CSuplProxyPosSession::~CSuplProxyPosSession()
	{
	LBSLOG(ELogP1, "CSuplProxyPosSession::CSuplProxyPosSession() Begin\n");
    delete iSubSessionRegistry;
	LBSLOG(ELogP1, "CSuplProxyPosSession::~CSuplProxyPosSession()\n End");
	}

/**
 * Two-phased constructor.
 */
CSuplProxyPosSession* CSuplProxyPosSession::NewL(MSuplProxyPosProtocol& aPosProtocol, TInt aRefLocSourceId)
    {
	LBSLOG(ELogP1, "CSuplProxyPosSession::NewL() Begin\n");

    CSuplProxyPosSession* self = new (ELeave) CSuplProxyPosSession(aPosProtocol, aRefLocSourceId);     
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
	LBSLOG(ELogP1, "CSuplProxyPosSession::NewL() End\n");
    return self;
    }

/**
 * Symbian destructor.
 */
void CSuplProxyPosSession::ConstructL()
    {
	LBSLOG(ELogP1, "CSuplProxyPosSession::ConstructL() Begin\n");
    iSubSessionRegistry = CPosSubsessionRegistry::NewL();
	LBSLOG(ELogP1, "CSuplProxyPosSession::ConstructL() End\n");
    }

/**
from CSession2
Called when the session is created.
*/
void CSuplProxyPosSession::CreateL()
	{
	LBSLOG(ELogP1, "CSuplProxyPosSession::CreateL() Begin\n");
	Server().IncSessionCount(ELbsProxyApiTypePos);
	LBSLOG(ELogP1, "CSuplProxyPosSession::CreateL() End\n");
	}


/**
Called when a message is received for this session.

@param aMessage The message from the client.
*/
void CSuplProxyPosSession::ServiceL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSession::ServiceL() Begin\n");
	switch (aMessage.Function())
		{
		case ELbsSuplProxyServerPosOpen:  
			OpenSubSessionL(aMessage);
			break;
        case ELbsSuplProxyServerPosClose: 
            CloseSubSessionL(aMessage);
            break;	
        case ELbsSuplProxyServerPosResetRequestId:
        case ELbsSuplProxyServerGetPosition:
        case ELbsSuplProxyServerCancelGetPosition:
        case ELbsSuplProxyServerPosNotifySessionComplete:
		case ELbsSuplProxyServerPosPayload:
		case ELbsSuplProxyServerGetSuplInfo:
		case ELbsSuplProxyServerCancelGetSuplInfo:
		case ELbsSuplProxyServerSetPosMethod:
		case ELbsSuplProxyServerSetSlpAddress:
			{
		    ForwardToSubSessionL(aMessage);
		    break;
			}
		default:
        	{
        	__ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
        	break;
        	}
		}
	LBSLOG(ELogP1, "CSuplProxyPosSession::ServiceL() End\n");
	}



void CSuplProxyPosSession::OpenSubSessionL(const RMessage2& aMessage)
    {
	LBSLOG(ELogP1, "CSuplProxyPosSession::OpenSubSessionL() Begin\n");

    TInt requestId = aMessage.Int0();
    TInt err = CreateSubSessionL(aMessage, requestId);
    aMessage.Complete(err);

	LBSLOG(ELogP1, "CSuplProxyPosSession::OpenSubSessionL() End\n");
    }

void CSuplProxyPosSession::CloseSubSessionL(const RMessage2& aMessage)
    {
	LBSLOG(ELogP1, "CSuplProxyPosSession::CloseSubSessionL() Begin\n");

    TInt handle = aMessage.Int3();
    ForwardToSubSessionL(aMessage);	// to allow sessioncomplete
    iSubSessionRegistry->CloseSubSession(handle);

	LBSLOG(ELogP1, "CSuplProxyPosSession::CloseSubSessionL() End\n");

    }


TInt CSuplProxyPosSession::CreateSubSessionL(const RMessage2& aMessage, TInt aRequestId)
    {
	LBSLOG(ELogP1, "CSuplProxyPosSession::CreateSubSessionL() Begin\n");

    TInt err = KErrNone;
    
    if(iSubSessionRegistry->Count() < KMaxPosSubsessions)
        {
        CSuplProxyPosSubSession* subSession = CSuplProxyPosSubSession::NewL(iPosProtocol, aRequestId, iRefLocSourceId);
        CleanupStack::PushL(subSession);
        TInt subSessionHandle = iSubSessionRegistry->AddInstanceL(subSession);
        CleanupStack::Pop(subSession); // Now owned by registry
        
        // Set the client subsession identifier. This is then available to the session in all subsequent messages
        TPckg<TInt> handlePackage(subSessionHandle);
        TInt err = SuplProxyUtils::Write(aMessage, KParamSubsession, handlePackage);
        if (err)
            {
            iSubSessionRegistry->CloseSubSession(subSessionHandle);
            User::Leave(err);
            }
        LBSLOG(ELogP1, "Subsession created successfully");
        
        // tell pos protocol about the new sub session
        iPosProtocol.NotifySubSessionOpen(subSession,aRequestId);
        }
    else
        {
        err = KErrInUse;
        }

	LBSLOG(ELogP1, "CSuplProxyPosSession::CreateSubSessionL() End\n");

    return err;
    }

void CSuplProxyPosSession::ForwardToSubSessionL(const RMessage2& aMessage)
    {
	LBSLOG(ELogP1, "CSuplProxyPosSession::ForwardToSubSessionL() Begin\n");

    CSuplProxyPosSubSession* subSession =
        iSubSessionRegistry->SubSessionFromHandleL(aMessage.Int3());

    if (!subSession)
        {
        User::Leave(KErrNotSupported);
        }

    subSession->ServiceL(aMessage); 

	LBSLOG(ELogP1, "CSuplProxyPosSession::ForwardToSubSessionL() End\n");

    }

/**
Sent by the kernel when all client handles to the session have been closed
from CSession2
@param aMessage The message from the client.
*/
void CSuplProxyPosSession::Disconnect(const RMessage2& aMessage)   
	{
	LBSLOG(ELogP1, "CSuplProxyPosSession::Disconnect() Begin\n");

	__ASSERT_DEBUG(iSubSessionRegistry->Count() == 0, User::Invariant());
	LBSLOG(ELogP1, "CSuplProxyPosSession::Disconnect() Begin\n");
	Server().DecSessionCount(ELbsProxyApiTypePos);   
	CSession2::Disconnect(aMessage);

	LBSLOG(ELogP1, "CSuplProxyPosSession::Disconnect() End\n");


	}


/**
Get a reference to the server for this session.

@return A reference to the CSuplProxyServer.
*/
CSuplProxyServer& CSuplProxyPosSession::Server()
	{
	LBSLOG(ELogP1, "CSuplProxyPosSession::Server() Begin and End \n");

	return *static_cast<CSuplProxyServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

