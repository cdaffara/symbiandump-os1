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
// Implements the avdtp transport session base class
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <bluetoothav.h>
#include "avdtpTransportSession.h"
#include "avdtp.h"
#include "avdtpsap.h"
#include "avdtpStream.h"
#include "avdtpSignallingChannel.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CTransportSession::CTransportSession(CAvdtpProtocol& aProtocol,
									 CAvdtpSAP& aSAP,
									 TAvdtpTransportSessionType aSessionType)
: iSAP(aSAP), iProtocol(aProtocol), iSAPShutdown(ENone)
	{
	LOG_FUNC
	iAddress.SetSession(aSessionType);
	}

CTransportSession::~CTransportSession()
	{
	LOG_FUNC
	}
	
void CTransportSession::ConstructL()
	{
	LOG_FUNC
	// put any common stuff here
	}

void CTransportSession::FastShutdown()
	{
	LOG_FUNC
	// just note whether we need to tell sap later or now to close
	iSAP.DetachSession();
	
	iSAPShutdown = EImmediate;
	// and proceed normally - we now own ourselves...
	DoShutdown();
	}
	
void CTransportSession::Shutdown()
	{
	LOG_FUNC
	iSAPShutdown = ENormal;
	// and proceed normally - we now own ourselves...
	DoShutdown();
	}
	
TInt CTransportSession::GetOption(TUint /*aLevel*/, TUint /*aName*/, TDes8& /*aOption*/) const
	{
	LOG_FUNC
	return KErrNotSupported;
	}
	
TInt CTransportSession::SetOption(TUint /*aLevel*/, TUint /*aName*/, const TDesC8& /*aOption*/)
	{
	LOG_FUNC
	return KErrNotSupported;
	}
	
void CTransportSession::Start()
	{
	LOG_FUNC
	//gulp
	}
	
void CTransportSession::Ioctl(TUint /*aLevel*/, TUint /*aName*/, const TDesC8* /*aOption*/)
	{
	LOG_FUNC
	// default is to error as not supported
	iSAP.Error(KErrNotSupported);
	}
	
void CTransportSession::CancelIoctl(TUint /*aLevel*/, TUint /*aName*/)
	{
	LOG_FUNC
	// do nothing
	}

/**
Stream says this session is ready
Record the address we're serving and tell user
*/
void CTransportSession::Ready(const TBTDevAddr& aRemoteDev)
	{
	LOG_FUNC
	iAddress.SetBTAddr(aRemoteDev);
	iSAP.Ready();
	}
	
	
void CTransportSession::LocalName(TAvdtpSockAddr& aAddr) const
	{
	LOG_FUNC
	// in this current design, local=remote
	RemName(aAddr);
	}
	
void CTransportSession::RemName(TAvdtpSockAddr& aAddr) const
	{
	LOG_FUNC
	// in this current design, local=remote
	aAddr = iAddress;
	}
	
TInt CTransportSession::SetLocalName(TAvdtpSockAddr& aAddr)
	{
	LOG_FUNC
	// in this current design, local=remote
	return SetRemoteName(aAddr);
	}
	
TInt CTransportSession::SetRemoteName(TAvdtpSockAddr& aAddr)
	{
	LOG_FUNC
	// can't trample of session type - that's immutable
	aAddr.SetSession(SessionType());
	iAddress = aAddr;
	return KErrNone;
	}
	
	
RPacketPool::RPacketPool()
	{
	LOG_FUNC
	}

TInt RPacketPool::Create(TUint aSize, TBool aBalk)
	{
	LOG_FUNC
	LOG2(_L("Creating packet pool, size %d, balk %d"), aSize, aBalk);
	__ASSERT_DEBUG(iPool.Count()==0, Panic(EAvdtpTransportSessionPacketPoolNotZeroSize));
	iNewestIndex=0;
	TInt res = AllocatePool(aSize);
	if (res==KErrNone)
		{
		iBalk=aBalk;
		}
	return res;
	}

TInt RPacketPool::AllocatePool(TUint aSize)
	{
	LOG_FUNC
	TRAPD(err,AllocatePoolL(aSize));	
	return err;
	}

void RPacketPool::AllocatePoolL(TUint aSize)
	{
	LOG_FUNC
	// allocates the size requested or fails
	for (TUint count = 0; count<aSize; count++)
		{
		CreateEntryL();
		}
	}

/**
Add a chain into the pool. Wrapping if necessary.  Cannot fail since the creation preallocated the chains.
*Can* return an error though if wrap would occur 
*/
TInt RPacketPool::Add(RMBufChain& aChain, TInt& aWrappedChainSize)
	{
	LOG_FUNC
	TInt res = KErrNone;
	if (iNewestIndex==(iPool.Count()-1) && !iBalk)
		{
		LOG(_L("No balking allowed, and no space - return KErrAvdtpPacketPoolBalk"));
		// last position filled, but balking not allowed, return error
		res = KErrAvdtpPacketPoolBalk;
		}
	else if (iNewestIndex==(iPool.Count()))
		{
		LOG(_L("Balking allowed, and no space - wrapping"));
		// can wrap
		__ASSERT_DEBUG(iBalk, Panic(EAvdtpTransportSessionPacketBalkingNotSet));
		iNewestIndex=0;
		}
	if (res==KErrNone)
		{
		// if we've wrapped the chain in the array needs destroying
		RMBufChain& inPool = iPool[iNewestIndex];
		
		if (!inPool.IsEmpty())
			{
			// The size of the chain dropped
			aWrappedChainSize = inPool.Length();
			inPool.Free();
			}
			
		LOG2(_L("Taking ownership of Chain. NewestIndex=%d, OldestIndex=%d"), iNewestIndex, OldestIndex())
		inPool.Assign(aChain);

		// move newest index, and check where oldest index is now
		if (++iNewestIndex == iOldestIndex)
			{
			// wrapped (in terms of age rather than array bounds)
			iOldestIndex = iNewestIndex-1;
			// might have wrapped that off (nb this is a TUint hence logic)
			if (iOldestIndex>Count()-1)
				{
				iOldestIndex = Count()-1;
				}
			}		
		}
	return res;
	}
	
TInt RPacketPool::OldestIndex() const
	{
	LOG_FUNC
	// if balking, one in front of the newest index (modulo poolsize). else zeroth
	return iOldestIndex;
	}
	
TInt RPacketPool::Remove(RMBufChain& aReturnedMBufChain)
	{
	LOG_FUNC
	LOG(_L("Removing oldest packet"));
	// return the oldest chain to the client
	aReturnedMBufChain.Assign(iPool[OldestIndex()]);
	// move the oldestindex on, modulo poolsize
	++iOldestIndex;
	iOldestIndex %= iPool.Count();
	LOG1(_L("Oldest packet index = %d"), OldestIndex());
	return KErrNone;
	}
	
void RPacketPool::Close()
	{
	LOG_FUNC
	while (iPool.Count())
		{
		// delete the head entry each time
		DestroyEntry(0);
		}
	iPool.Close();
	}

void RPacketPool::DestroyEntry(TUint aIndex)
	{
	LOG_FUNC
	iPool[aIndex].Free();
	iPool.Remove(aIndex);
	}

void RPacketPool::CreateEntryL()
	{
	LOG_FUNC
	RMBufChain chain;
	// don't need to alloc the chains as the entries claim ownership of inbound ones
	TInt err = iPool.Append(chain);
	if (err!=KErrNone)
		{
		chain.Free();
		User::Leave(err);
		}
	}
	
TInt RPacketPool::Resize(TUint aSize)
	{
	LOG_FUNC
	TInt res = KErrNone;
	// try to grow/shrink the pool
	if (aSize<iPool.Count())
		{
		LOG1(_L("Reducing pool to %d entries"), aSize);
		// throw away any packets (whatever age) if we need to
		for (TUint index = iPool.Count()-1;index>=(aSize-1);index--)
			{
			DestroyEntry(index);
			}
		}
	else if (aSize > iPool.Count())
		{
		LOG1(_L("Growing pool to %d entries"), aSize);
		// grow if possible
		for (TUint index=0;index<(aSize-iPool.Count());index++)
			{
			TRAP(res,CreateEntryL());
			}
		}
	if (res==KErrNone)
		{
		LOG(_L("Resize sucessful"));
		iOldestIndex = 0;
		iNewestIndex = iOldestIndex;
		}
	return res;
	}

CUserPlaneTransportSession::CUserPlaneTransportSession(CAvdtpProtocol& aProtocol,
					  CAvdtpSAP& aSAP,
					  TAvdtpTransportSessionType aType,
					  CAVStream& aStream)
: CTransportSession(aProtocol,aSAP, aType), iStream(&aStream)
	{
	LOG_FUNC
	SetSEID(iStream->RemoteSEID());
	};
	

void CUserPlaneTransportSession::ConstructL()
	{
	LOG_FUNC
	// acquire TSID from stream or by ourselves
	iStream->GetTSID(iTSID, SessionType());
	// we don't need a TSID if not muxing
	}


TInt CUserPlaneTransportSession::ActiveOpen()
	{
	// check for race - between an ACP stream establishment, and GAVDP attaching
	// sessions, the stream may have been released... so check if still there
	// if so, forward active open to derivers
	TAvdtpSockAddr addr;
	RemName(addr);
	
	return iProtocol.FindStream(addr) ? DoActiveOpen() : KErrDisconnected;
	}
	

void CUserPlaneTransportSession::ReceivePacketLost()
	{
	// Default impl.  No action taken.
	}

void CUserPlaneTransportSession::NewData(RMBufChain& aChain)
	{
	LOG_FUNC
	
	if (EImmediate != iSAPShutdown)
		{
		TInt wrapped; // Used to indicate if packet pool wrapped around
		TInt poolErr = iReceivePool.Add(aChain, wrapped);
		if (poolErr==KErrNone)
			{
			iSAP.NewData(1);
			}
		else
			{
			// The data can't be added to the revc pool.  Free the chain
			// and notify the base session (i.e., media, reporting, recovery)
			// that a data packet has been dropped.
			aChain.Free();
			ReceivePacketLost();
			}
		}
	else 
		{
		//Fast Shutdown in progress, do not pass any more data to the socket.
		//therefore free the chain.
		aChain.Free();
		}
	}

TInt CUserPlaneTransportSession::GetData(RMBufChain& aData)
	{
	LOG_FUNC
	// fetch from pool
	return iReceivePool.Remove(aData);
	}


/**
we handle the channelerror, rather than the channel
as we do not want to necessarily abandon ALL *streams* to the same remote
just because one channel errored
*/
void CUserPlaneTransportSession::ChannelError(TInt aError)
	{
	LOG_FUNC
	if (iStream)
		{
		// this transport session is a user-plane one
		__ASSERT_DEBUG(SessionType()!=ESignalling, Panic(EAvdtpBadTransportSessionUpcallFromTransportChannel));
		iStream->NotifyUserPlaneTransportSessionsError(this, aError);
		iStream = NULL;
		LeaveTransportChannel();
		}
	// else the stream has gone from an earlier Release, and we're still here for client
	}
	
void CUserPlaneTransportSession::StreamError(TInt aError)
	{
	LOG_FUNC
	if (iStream)
		{
		iStream->ClearSession(*this);//this will cause the iSession pointer in the stream to be null'd
		}
	// backcall from stream - default is to unbind and pass onto SAP
	iStream = NULL;
	if (iSAPShutdown!=EImmediate)
		{
		iSAP.Error(aError);
		iSAP.SessionDisconnect();
		}
	LeaveTransportChannel();
	}
	

void CUserPlaneTransportSession::DoShutdown()
	{
	LOG_FUNC
	if (iStream)
		{
		// remove ourselves from the stream
		iStream->DropSession(SessionType(), *this);
		iStream = NULL;
		}
	else
		{
		/* If we don't have a stream pointer, then the logical channel underneath
		us has gone away, and we've just been detached from the socket, so we need
		to delete ourself */
		StreamReleased();
		}
	}

/*
Called from stream after Release has completed
*/
void CUserPlaneTransportSession::StreamReleased()
	{
	LOG_FUNC
	LeaveTransportChannel();

	switch (iSAPShutdown)
		{
		case ENormal:	
		iSAP.CanClose();
		break;
		
		case EImmediate:
		delete this;
		break;
		
		case ENone:
		iStream = NULL;
		iSAP.SessionDisconnect();
		break;
		}
	}

void CUserPlaneTransportSession::LeaveTransportChannel()
	{
	LOG_FUNC
	if (iTransportChannel)
		{
		// he session is now removed from the stream
		// time to detach from the channel
		iTransportChannel->DetachTransportSession(*this, SessionType());
		iTransportChannel = NULL;
		}
	}


CUserPlaneTransportSession::~CUserPlaneTransportSession()
	{
	LOG_FUNC
	if (iStream)
		{
		iStream->ClearSession(*this);//this will cause the iSession pointer in the stream to be null'd
		}
	iTSID.Close();
	iSendPool.Close();
	iReceivePool.Close();
	}
