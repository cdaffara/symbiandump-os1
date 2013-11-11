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
// panincoming.cpp
// 
//

/**
 @file
 @note PAN agent incoming connection listener implementation
*/

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include <comms-infras/eintsock.h>
#include <networking/panuiinterfaces.h>
#include "panagtincoming.h"
#include "panagtpolicy.h"
#include "panagtutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_AGENT);
#endif

using namespace PanAgent;

CPanIncomingConnectionListener* CPanIncomingConnectionListener::NewL(MIncomingConnectionAcceptor& aAcceptor)
/**
Create a new instance of the class
@param aAcceptor The class to notify when a new connection is received
@return A pointer to the new CPanIncomingConnectionListener, or NULL if it could not be created
*/
	{
	CPanIncomingConnectionListener* self = new(ELeave) CPanIncomingConnectionListener(aAcceptor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
	}

CPanIncomingConnectionListener::CPanIncomingConnectionListener(MIncomingConnectionAcceptor& aAcceptor) :
	CActive(KPriorityIncomingListenerAo), iAcceptor(aAcceptor)
/**
Not much to do here
*/
	{	}

CPanIncomingConnectionListener::~CPanIncomingConnectionListener()
/**
Close all the sockets
*/
	{
	Cancel();

	iAcceptingSocket.Close();
	iListeningSocket.Close();
	iListeningSocketIsOpen = EFalse;
	
	delete iDelayedOpenHelper;
	}

void CPanIncomingConnectionListener::ConstructL()
/**

*/
	{
	CActiveScheduler::Add(this);
	
	iDelayedOpenHelper = CPanIncomingSocketDelayedOpenHelper::NewL(*this);
	
	OpenListeningSocketL();
	User::LeaveIfError(iAcceptingSocket.Open());
	iListeningSocket.Accept(iAcceptingSocket, iStatus);

	LOG(_L("IncomingConnectionListener: Incoming socket started and ready to accept connections"));

	SetActive();
	}

TBool CPanIncomingConnectionListener::IsAcceptingSocketOpen() const
/**
Is there an accepting socket open?
*/
	{
	return(IsActive());	// if we're active, then we must have an outstanding Accept()
	}


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Notify the main class of the incoming connection
*/
void CPanIncomingConnectionListener::RunL()
	{
	if(iStatus == KErrNone)
		{
		LOG(_L("IncomingConnectionListener: ...connection completed successfully, handing connected device off to agent core."));

		TRAPD(err, iAcceptor.CreateNewConnectionFromIncomingL(iAcceptingSocket)); // if successful, the socket is transferred away from us here, so a "null" socket is all we have left...
		if (err != KErrNone)
		    {
		    // Something has failed before the message could be sent
		    // so we execute the remainder of the RunL logic synchronously
            RestartIncomingConnectionListener(err);
		    }
		// else {
		//     The rest of the RunL logic is executed asynchonously in
		//     RestartIncomingConnectionListener() once BNEP has responded
		//     to the TCreateChannelController message
		// }
		}
	else // check what went wrong
		{
		LOG1(_L("IncomingConnectionListener: ...connection completed with error %d"), iStatus.Int());
		iDelayedOpenHelper->SocketOpenFailed();
		}
	}

void CPanIncomingConnectionListener::RestartIncomingConnectionListener(TInt aError)
    {
	// ...otherwise we still have a "live" socket
	if(aError) // for some reason the connection could not be completed, so we'll just have to close the socket
		{	// it's possible that the device is already connected
		LOG(_L("IncomingConnectionListener: ...error - could not hand off socket to agent core, closing socket instead."));
		iAcceptingSocket.Close();
		}
			
	// create a new accepting socket...
	TInt err = iAcceptingSocket.Open();
	if(err) // esp. likely if we've just got KErrNoMemory as the last error		
		{
		LOG(_L("IncomingConnectionListener: ...could not open new accepting socket. Will try again later."));						
		iDelayedOpenHelper->SocketOpenFailed();
		}
	else
		{
		LOG(_L("IncomingConnectionListener: ...requeing listening socket..."));

		// ...and repeat the cycle
		iListeningSocket.Accept(iAcceptingSocket, iStatus);
		SetActive();
		}
    }

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

void CPanIncomingConnectionListener::RunL()
/**
Notify the main class of the incoming connection
*/
	{
	if(iStatus==KErrNone)
		{
		LOG(_L("IncomingConnectionListener: ...connection completed successfully, handing connected device off to agent core."));

		TRAPD(err, iAcceptor.CreateNewConnectionFromIncomingL(iAcceptingSocket)); // if successful, the socket is transferred away from us here, so a "null" socket is all we have left...

		// ...otherwise we still have a "live" socket
		if(err) // for some reason the connection could not be completed, so we'll just have to close the socket
			{	// it's possible that the device is already connected
			LOG(_L("IncomingConnectionListener: ...error - could not hand off socket to agent core, closing socket instead."));
			iAcceptingSocket.Close();
			}
			
		// create a new accepting socket...
		err = iAcceptingSocket.Open();
		if(err) // esp. likely if we've just got KErrNoMemory as the last error		
			{
			LOG(_L("IncomingConnectionListener: ...could not open new accepting socket. Will try again later."));						
			iDelayedOpenHelper->SocketOpenFailed();
			}
		else
			{
			LOG(_L("IncomingConnectionListener: ...requeing listening socket..."));

			// ...and repeat the cycle
			iListeningSocket.Accept(iAcceptingSocket, iStatus);
			SetActive();
			}
		}
	else // check what went wrong
		{
		LOG1(_L("IncomingConnectionListener: ...connection completed with error %d"), iStatus.Int());
		iDelayedOpenHelper->SocketOpenFailed();
		}
	}
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY	

void CPanIncomingConnectionListener::DoCancel()
/**
Cancel the outstanding accept
*/
	{
	iListeningSocket.CancelAccept();
	}
	
void CPanIncomingConnectionListener::OpenListeningSocketL()
/**
Create and initialise the listening socket
*/
	{
#if defined(_DEBUG) && defined(SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY)
    TInt err = iListeningSocket.Open(KBTAddrFamily, KSockSeqPacket, KL2CAP);
    __ASSERT_DEBUG(err != KErrBadName, PanAgentPanic(EPanAgtMissingBinding));
    User::LeaveIfError(err);
#else
	User::LeaveIfError(iListeningSocket.Open(KBTAddrFamily, KSockSeqPacket, KL2CAP));
#endif
	
	// listen on the assigned l2cap port for BNEP
	TBTSockAddr incomingConnectionSettings;
	incomingConnectionSettings.SetPort(KBnepPsm);

	// read the required security settings from commdb, and use them to set up the listening socket
	TBTServiceSecurity incomingSecuritySettings;
	incomingSecuritySettings.SetAuthentication(KPanIncomingAuthenticationRequired);
	incomingSecuritySettings.SetAuthorisation(KPanIncomingAuthorisationRequired);
	incomingSecuritySettings.SetEncryption(KPanIncomingEncryptionRequired);
	incomingSecuritySettings.SetUid(KBTPanAuthorisationUid);
	incomingConnectionSettings.SetSecurity(incomingSecuritySettings);
	
	User::LeaveIfError(iListeningSocket.Bind(incomingConnectionSettings));

	// Set the MTU to the required minimum for BNEP
	TUint16 bnepMtu = KBnepMtu;								
	TPckg<TUint16> mtuBuf(bnepMtu);
	User::LeaveIfError(iListeningSocket.SetOpt(KL2CAPInboundMTU, KSolBtL2CAP, mtuBuf));
	User::LeaveIfError(iListeningSocket.SetOpt(KL2CAPNegotiatedOutboundMTU, KSolBtL2CAP, mtuBuf));
	// Register the CoD service - Networking bit (especially relevant for WinXP)
	User::LeaveIfError(iListeningSocket.SetOpt(KBTRegisterCodService, KSolBtSAPBase, EMajorServiceNetworking));

	User::LeaveIfError(iListeningSocket.Listen(KIncomingSockQueueSize));
	iListeningSocketIsOpen = ETrue;
	}

CPanIncomingSocketDelayedOpenHelper* CPanIncomingSocketDelayedOpenHelper::NewL(CPanIncomingConnectionListener& aIncomingConnectionListener)
/**

*/
	{
	CPanIncomingSocketDelayedOpenHelper* self = new(ELeave) CPanIncomingSocketDelayedOpenHelper(aIncomingConnectionListener);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPanIncomingSocketDelayedOpenHelper::CPanIncomingSocketDelayedOpenHelper(CPanIncomingConnectionListener& aIncomingConnectionListener) :
	CTimer(KPriorityIncomingListenerAo), iCurrentTimerPeriod(KMinSocketReopenAttemptInterval), iIncomingConnectionListener(aIncomingConnectionListener)
/**

*/
	{
	CActiveScheduler::Add(this);
	}
	
void CPanIncomingSocketDelayedOpenHelper::ConstructL()
/**

*/
	{
	CTimer::ConstructL();
	}
	
CPanIncomingSocketDelayedOpenHelper::~CPanIncomingSocketDelayedOpenHelper()
/**
Nothing to do
*/
	{
	
	}

void CPanIncomingSocketDelayedOpenHelper::SocketOpenFailed()
/**
A socket open has failed, so it's now up to us to keep retrying
*/
	{
	After(iCurrentTimerPeriod);
	}
	
void CPanIncomingSocketDelayedOpenHelper::RunL()
/**
Attempt to open the listening socket (if necessary) and the accepting socket
*/
	{
	// check that the accepting (and therefore by inference the listening) sockets aren't open already
	__ASSERT_DEBUG(!(iIncomingConnectionListener.IsAcceptingSocketOpen()), PanAgentPanic(EDelayedOpenHelperActiveWhenBothListeningAndAcceptingSocketsAreOpen));
	
	TInt err;
	if(!(iIncomingConnectionListener.IsListeningSocketOpen()))
		{
		TRAP(err, iIncomingConnectionListener.OpenListeningSocketL());
		if(err)	// failed to open listening socket, back off and try again later
			{
			SetNewTimerPeriod();
			After(iCurrentTimerPeriod);	// will set us active
			return;
			}
		}
		
	if(!(iIncomingConnectionListener.IsAcceptingSocketOpen()))
		{
		err = iIncomingConnectionListener.iAcceptingSocket.Open();
		if(err)	// failed to open accepting socket, back off and try again later
			{
			SetNewTimerPeriod();
			After(iCurrentTimerPeriod);	// will set us active
			return;
			}
		else
			{
			iIncomingConnectionListener.iListeningSocket.Accept(iIncomingConnectionListener.iAcceptingSocket, iIncomingConnectionListener.iStatus);
			iIncomingConnectionListener.SetActive();
			return;
			}
		}
	}

void CPanIncomingSocketDelayedOpenHelper::SetNewTimerPeriod()
/**
Set the timer period to a new value - doubles each time up to the limit
*/
	{
	iCurrentTimerPeriod = iCurrentTimerPeriod.Int() * 2;
	
	if(iCurrentTimerPeriod > TTimeIntervalMicroSeconds32(KMaxSocketReopenAttemptInterval))	// this happens rarely enough (once per many secs to minutes) to take the hit of constructing a new object each time
		{
		iCurrentTimerPeriod = KMaxSocketReopenAttemptInterval;
		}
	}
