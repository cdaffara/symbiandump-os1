// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the code for AVDTP SAP
// The SAP is really only a conduit to the socket
// The sessions implement the meaningful protocol stuff
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpsap.h"
#include "avdtp.h"
#include "avdtpStream.h"
#include "avdtpMediaSession.h"
#include "avdtpReportingSession.h"
#include "avdtpRecoverySession.h"
#include "avdtpSignallingSession.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CAvdtpSAP* CAvdtpSAP::NewL(CAvdtpProtocol& aProt)
/**
Static AVDTP SAP factory function

  @internalComponent
  @leave KErrNoMemory if the SAP object could not be allocated
  @param aProt protocol object
  @return A pointer to the new SAP
*/
	{
	LOG_STATIC_FUNC
	// Create and return a new SAP
	CAvdtpSAP* sap = new(ELeave) CAvdtpSAP(aProt);
	CleanupStack::PushL(sap);
	sap->ConstructL();
	CleanupStack::Pop(sap);
	return sap;
	}

void CAvdtpSAP::ConstructL()
/**
Second-phase construction of a SAP

Set up the async. callback.

  @internalComponent
*/
	{
	LOG_FUNC
	CBluetoothSAP::ConstructL();
	}

CAvdtpSAP::CAvdtpSAP(CAvdtpProtocol& aProtocol)
	: CBluetoothSAP(aProtocol.SecMan(), aProtocol.CodMan()),
	  iProtocol(aProtocol),
	  iIndicationQueue(_FOFF(HQueuedAvdtpIndication, iLink))
/**
Constructor for an AVDTP SAP

  @internalComponent
  @param aProt The AVDTP protocol object
*/
	{
	LOG_FUNC
	}


CAvdtpSAP::~CAvdtpSAP()
/**
Called when ESock deletes the SAP, when user app calls RSocket::Close()

  @internalAll
*/
	{
	LOG_FUNC
	// clear (possible) indication queue
	TSglQueIter<HQueuedAvdtpIndication> iter(iIndicationQueue);
	while (iter)
		{
		delete iter++;
		}
	// at this point we don't support idling sessions
	delete iSession;
	}

/**
SetLocalName is here used to specify the *local* session to which this SAP will become attached
As a part of this the SAP tries to tell the session of the stream with which it'll be associated
The stream lookup key is the *remote* SEID of the stream (streams are always searched for by this SEID)
This gives rise to an interesting API, but the intended client is only GAVDP
(via ESOCK).
So: the parts of AVDTPSockAddr for passive sides are
BTAddr - not used
SEID - specify the *remote* SEID
Session - specify the *local* session type

This *could* change if needs be, so that a conversion from local to remote is performed here
but would be fiddly and break the ease of always looking for a stream by its remote SEID
*/
TInt CAvdtpSAP::SetLocalName(TSockAddr& aAddr)
	{
	LOG_FUNC
	// this determines our session
	TRAPD(err, UpdateSessionL(TAvdtpSockAddr::Cast(aAddr)));
	return err;
	}

void CAvdtpSAP::LocalName(TSockAddr& aAddr) const
/**
Read the Local Name into aAddr.


  @internalAll
  @param aAddr The address to read into
*/
	{
	LOG_FUNC
	if (iSession)
		{
		TAvdtpSockAddr avAddr = TAvdtpSockAddr::Cast(aAddr);
		iSession->LocalName(avAddr);
		aAddr = avAddr;
		}
	}

void CAvdtpSAP::RemName(TSockAddr& aAddr) const
/**
Read the remote name into aAddr.

  @internalAll
  @param aAddr The address to read into
*/
	{
	// Return the remote name
	LOG_FUNC
	if (iSession)
		{
		TAvdtpSockAddr avAddr = TAvdtpSockAddr::Cast(aAddr);
		iSession->RemName(avAddr);
		aAddr = avAddr;
		}
	}

/**
SetRemName is here used to specify the *remote* BTAddr, SEID and session to which this SAP will become attached
As a part of this the SAP tries to tell the session of the stream with which it'll be associated
The stream lookup key is the *remote* SEID of the stream (streams are always searched for by this SEID)
This gives rise to an interesting API, but the intended client is only GAVDP
(via ESOCK).
So: the parts of AVDTPSockAddr for active sides are
BTAddr - that of remote
SEID - the remote SEID
Session - specify the local/remote session type
*/
TInt CAvdtpSAP::SetRemName(TSockAddr& aAddr)
	{
	LOG_FUNC
	TRAPD(err, UpdateSessionL(TAvdtpSockAddr::Cast(aAddr)));
	return err;
	}
	

void CAvdtpSAP::UpdateSessionL(const TAvdtpSockAddr& aAddr)	
	{
	LOG_FUNC
	
	// do we want to support swapping session type? for now no
	if (!iSession)
		{
		// need to get one
		// get to see our TransportSession here
		TAvdtpTransportSessionType s = aAddr.Session();
		CAVStream* stream = NULL; //non-owned
		if (s!=ESignalling)
			{
			stream = iProtocol.FindStream(aAddr);
			if (!stream)
				{
				User::Leave(KErrUnknown);
				}
			}
			
		switch (s)
			{
			case EMedia:
				iSession = CMediaSession::NewL(iProtocol, *this, *stream);
				break;
			case EReporting:
				iSession = CReportingSession::NewL(iProtocol, *this, *stream);
				break;
			case ERecovery:
				iSession = CRecoverySession::NewL(iProtocol, *this, *stream);
				break;
			case ESignalling:
				iSession = CSignallingSession::NewL(iProtocol, *this);
				break;
			default:
				User::Leave(KErrArgument);
			}
		}
		
	// update the device address (the session may go from passive to active)
	iSession->SetRemoteAddress(aAddr.BTAddr());	// session cannot change type
	}

void CAvdtpSAP::AutoBind()
/**
Auto bind from ESock.

@internalComponent
*/
	{
	LOG_FUNC
	}

TInt CAvdtpSAP::GetOption(TUint aLevel, TUint aName, TDes8& aOption) const
/**
Get a socket option.

Currently, just for getting the results of an AVDTP operation to user-land


  @internalAll
  @param aLevel The socket option level
  @param aName The socket option name
  @param aOption The socket option data
*/
	{
	// Do a getopt
	LOG_FUNC
	return iSession ? iSession->GetOption(aLevel, aName, aOption) : KErrNotReady;
	}

	
TInt CAvdtpSAP::SAPSetOption(TUint aLevel, TUint aName, const TDesC8& aOption)
/**
Set a socket option.

  @internalAll
  @param aLevel The socket option level
  @param aName The socket option name
  @param aOption The socket option data
*/
	{
	// Perform a setopt
	LOG_FUNC
	TInt ret = KErrNotSupported;
	// if this is not an internal option then pass to the session
	if (aLevel == KSolBtAVDTPInternal)
		{
		switch (aName)
			{				
			case ESetAsSecondarySAP:
				{
				// assert we haven't had an address set
				__ASSERT_DEBUG(iRemoteDev == TBTDevAddr(0), Panic(ERGAVDPSequenceFailure));
				// this SAP does not have a session, nor an addr
				// tell protocol to look after us until a primary collects us
				iProtocol.AddSecondarySAP(*this);
				iIsSecondary = ETrue;
				ret = KErrNone;		
				}
			break;
			case EBindToSecondarySAP:
				{
				// form binding to secondary sap
				iSecondarySAP = iProtocol.GetSecondarySAP();
				__ASSERT_DEBUG(iSecondarySAP, Panic(ERGAVDPSequenceFailure));
				ret = KErrNone;
				}
			break;
			}
		// default covered by ret.
		}
	else
		{
		ret = iSession ? iSession->SetOption(aLevel, aName, aOption) : KErrNotReady;
		}
	return ret;
	}


// Startup/Shutdown

void CAvdtpSAP::ActiveOpen()
/**
Active open an AVDTP socket...

  @internalAll
*/
	{
	LOG_FUNC
	TInt err = KErrNotReady;
	
	if (iSession)
		{
		err = iSession->ActiveOpen();
		}
		
	if (err!=KErrNone)
		{
		iSocket->Error(err, MSocketNotify::EErrorConnect);
		}
	}

void CAvdtpSAP::ActiveOpen(const TDesC8& /*aConnectionData*/)
/**
Active open an AVDTP socket (data overload)...

  @internalAll
  @param aConnectionData Data to send on connection
*/
	{
	LOG_FUNC
	Error(KErrNotSupported);
	}

TInt CAvdtpSAP::PassiveOpen(TUint /*aQueSize*/)
/**
Passive open an AVDTP socket...
NOT SUPPORTED.
@see CLogicalChannelFactory which acquires inbound logical channels and is aware of their sequence
@see Transport sessions which are ActiveOpened to "find" passive bearers
@internalAll
*/
	{
	LOG_FUNC
	return KErrNotSupported;
	}

TInt CAvdtpSAP::PassiveOpen(TUint /*aQueSize*/, const TDesC8& /*aConnectionData*/)
/**
Passive open an AVDTP socket...
NOT SUPPORTED.
@see CLogicalChannelFactory which acquires inbound logical channels and is aware of their sequence
@see Transport sessions which are ActiveOpened to "find" passive bearers
@internalAll
*/
	{
	LOG_FUNC
	return KErrNotSupported;
	}

void CAvdtpSAP::Start()
/**
@internalAll
*/
	{
	LOG_FUNC
	// sessions do not need to be forwarded this at present
	// (primarily as no "usual" passive connections)
	}

void CAvdtpSAP::Shutdown(TCloseType aCloseType)
/**
Close the SAP down.  We don't declare support for Normal shutdown, but for future use
we pass onto transport session.

@internalAll
@param aCloseType How fast we're going down
*/
	{
	LOG_FUNC
	if (iSession)
		{
		aCloseType==ENormal ? iSession->Shutdown() : iSession->FastShutdown();
		}
		
	// fast shutdown will delete us after this unwinds
	}

void CAvdtpSAP::Shutdown(TCloseType aCloseType, const TDesC8& /*aDisconnectionData*/)
/**
Close the SAP down (data overload).

We don't declare support for Normal or data shutdowns, but for future use
we pass onto transport session.

@internalAll
@param aCloseType How fast we're going down
@param aDisconnectionData Data to send on disconnect
*/
	{
	// this one's an error
	LOG_FUNC
	Shutdown(aCloseType);
	}

// Data flow & ioctl

TInt CAvdtpSAP::Write(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr)
	{
	LOG_FUNC

	if (iSession)
		{
		return iSession->Send(aData, aOptions, aAddr);
		}
	else
		{
		iSocket->Error(KErrNotReady, MSocketNotify::EErrorSend);
		return 0;
		}
	}

TInt CAvdtpSAP::GetData(RMBufChain& aData, TUint /*aLength*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	LOG_FUNC
	return iSession ? iSession->GetData(aData) : KErrNotSupported;
	}


void CAvdtpSAP::Ioctl(TUint aLevel, TUint aName, TDes8* aOption)
/**
Perform an Ioctl.
Primary SAPs ask the session to actually perform the ioctl.
Secondary SAPs do the ioctl themselves.

Perform ioctl-guarding (one at a time stuff) then forward to session
@internalAll
@param aLevel The Ioctl level
@param aName The Ioctl name
@param aOption The Ioctl data
*/
	{
	LOG_FUNC
	if (!iIsSecondary)
		{
		// we are a *primary* SAP, and should forward to our session
		if (!iSession)
			{
			iSocket->Error(KErrDisconnected, MSocketNotify::EErrorIoctl);
			}
		else
			{
			iIoctlLevel = aLevel;
			iIoctlName = aName;
			iSession->Ioctl(aLevel, aName, aOption);
			}
		}
	else
		{
		if (aLevel == KSolBtAVDTPSignalling)
			{
			// we are a secondary sap, so just deal with ioctls ourselves
			iIoctlLevel = aLevel;
			iIoctlName = aName;

			switch (aName)
				{
				case EAwaitIndication:
					{
					// if there is one on the queue then deliver
					if (!iIndicationQueue.IsEmpty())
						{
						HQueuedAvdtpIndication* ind = iIndicationQueue.First();
						iIndicationQueue.Remove(*ind);
						ServiceComplete(ind->Indication());
						delete ind;
						}
					}
				break;
				default:
					Error(KErrNotSupported);				
				}
			}
		}
	}
	
	

void CAvdtpSAP::CancelIoctl(TUint aLevel, TUint aName)
/**
Cancel an Ioctl.

@internalAll
@param aLevel The Ioctl level
@param aName The Ioctl name
*/
	{
	LOG_FUNC
	__ASSERT_DEBUG(aLevel == iIoctlLevel && aName == iIoctlName, Panic(EAvdtpBadIoctl));
	if (iSession)
		{
		iSession->CancelIoctl(aLevel, aName);
		}
	
	ClearIoctl();
	}

void CAvdtpSAP::Error(TInt aError)
/**
Error has occurred - tell the socket.

@internalAll
@param aErr The error code
@param aType The type of error
*/
	{
	LOG_FUNC
	iSocket->Error(aError, iIoctlLevel ? MSocketNotify::EErrorIoctl
									   : MSocketNotify::EErrorAllOperations);
#ifdef _DEBUG									   
	iCanSignalDisconnect = EFalse; // esock doesn't allow error then disconnect
#endif
	ClearIoctl();
	}
	
void CAvdtpSAP::SendError(TInt aError)
	{
	iSocket->Error(aError, MSocketNotify::EErrorSend);
	}
	
void CAvdtpSAP::SessionDisconnect()
	{
#ifdef _DEBUG
	if (iCanSignalDisconnect)
#endif
		{
		iSocket->Disconnect();
#ifdef _DEBUG
		iCanSignalDisconnect = EFalse;
#endif
		}
	}


/************************************************************************

  Events interface (from TransportSession)

 ************************************************************************/

void CAvdtpSAP::NewData(TUint aCount)
/**
There is new data from the session for us.

@internalComponent
@param aPacket The new data from the muxer
*/
	{
	// Write the data out to the socket, if possible.
	LOG_FUNC
	if (iSocket)
		{
		iSocket->NewData(aCount);
		}
	}

void CAvdtpSAP::CanSend()
/**
Notification from the session that we can send again.

Pass this through to our state machine.

  @internalComponent
*/
	{
	LOG_FUNC
	iSocketBlocked = EFalse;
	iSocket->CanSend();
	}

void CAvdtpSAP::ServiceComplete(const TDesC8* aBuf)
/**
Notification that an AVDTP command has completed.
May be called when no Ioctl called (ag stack-initiated Aborts) - in which case NOP
@param aBuf The Ioctl data
*/
	{
	LOG_FUNC
	if (iIoctlLevel)
		{
		// need to cast away constness!
		iSocket->IoctlComplete(const_cast<TDesC8*>(aBuf));
		ClearIoctl();
		}
	}

// Bearer (=signalling channel in this case) interface

void CAvdtpSAP::Ready()
	{
	LOG_FUNC
	// tell socket that session is ready 
		{
#ifdef _DEBUG
		iCanSignalDisconnect = ETrue;
#endif
		iSocket->ConnectComplete();
		}
	}


/*inline*/ void CAvdtpSAP::ClearIoctl()
	{
	LOG_FUNC
	iIoctlLevel = 0;
	iIoctlName = 0;
	}

/* for the case of normal shutdowns from socket this is called when session
is ready to go
*/
void CAvdtpSAP::CanClose()
	{
	LOG_FUNC
	iSocket->CanClose(MSocketNotify::EDelete);
	}
	
/**
Raise an indication to the client
Only secondary saps can propagate indications
*/
void CAvdtpSAP::Indication(const TDesC8& aIndicationData)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iSecondarySAP, Panic(EAvdtpSAPIndicationEngineFailure));
	iSecondarySAP->DoIndication(aIndicationData);
	}

/**
Actually do the raising of the indication
*/	
void CAvdtpSAP::DoIndication(const TDesC8& aIndicationData)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iSocket, Panic(EAvdtpSAPIndicationEngineFailure));

	// since the protocol allows multiple outstanding commands it can be that we
	// have to raise subsequent indications before the client has responded to the first
	// therefore we queue the indication data

	if (iIoctlName == 0)
		// means there is no pending ioctl.
		// the queue can or cannot be empty. However, since there is no
		// pending ioctl, we queue the indication
		{
	    HQueuedAvdtpIndication* ind = HQueuedAvdtpIndication::New(aIndicationData);
		if (ind)
			{
			LOG(_L("Adding Indication to queue"));
			iIndicationQueue.AddLast(*ind);
			}
		else
			{
			// OOM'd - better tell client...they've lost an indication
			Error(KErrNoMemory);
			}
		}
	else
		// if we have an outstanding Ioctl means the queue is empty.
		// infact when an Ioctl (on sec sap) is performed and there are items
		// in the queue, it completes the Ioctl immediately with a queued
		// indication and then it clears the iIoctlName.
		// Otherwise, if it cannot find queued indications then sets the iIoctlName
		// and iIoctlLevel that means having a pending Ioctl 
		{
		__ASSERT_DEBUG(iIndicationQueue.IsEmpty(), Panic(EAvdtpUnexpectedIndicationsInQueue));
		// tell client straight away
		if (iSocket)
			{
			ServiceComplete(&aIndicationData);
			}
		}
	}
	

HQueuedAvdtpIndication* HQueuedAvdtpIndication::New(const TDesC8& aIndicationData)
    {
	LOG_STATIC_FUNC
    HQueuedAvdtpIndication* ind = new HQueuedAvdtpIndication;
    TInt err = KErrNone;
    if (ind)
    	{
    	TRAPD(err, ind->ConstructL(aIndicationData));
    	if (err)
    	    {
    	    delete ind;
    	    ind=NULL;
    	    }
    	}
    return (err==KErrNone) ? ind : NULL;
    }
	
void HQueuedAvdtpIndication::ConstructL(const TDesC8& aIndicationData)
    {
	LOG_FUNC
	iBuf = NULL;
    iBuf = aIndicationData.AllocL();
    }
    
HQueuedAvdtpIndication::~HQueuedAvdtpIndication()
    {
	LOG_FUNC
    delete iBuf;
    }

