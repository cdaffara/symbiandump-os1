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
// Implements the avdtp media transport session
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpMediaSession.h"
#include "avdtp.h"
#include "avdtpsap.h"
#include "avdtpStream.h"
#include "avdtputil.h"
#include "gavdpinterface.h"

//#include <es_mbman.h> // for MBUF logging

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CMediaSession* CMediaSession::NewLC(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream)
	{
	LOG_STATIC_FUNC
	CMediaSession* s = new (ELeave) CMediaSession(aProtocol, aSAP, aStream);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}
	
CMediaSession* CMediaSession::NewL(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream)
	{
	LOG_STATIC_FUNC
	CMediaSession* s = CMediaSession::NewLC(aProtocol, aSAP, aStream);
	CleanupStack::Pop();
	return s;		
	}
	
CMediaSession::CMediaSession(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream)
: CUserPlaneTransportSession(aProtocol, aSAP, EMedia, aStream)
	{
	LOG_FUNC
	iNotifyMediaPacketDroppedFlag = EFalse;
	iPacketsLost = 0;
	}
	
void CMediaSession::ConstructL()
	{
	LOG_FUNC
	CUserPlaneTransportSession::ConstructL();
	
	// Balking is set to true.  This allows the circular buffer to wrap
	// and overwrite the oldest data.
	User::LeaveIfError(iSendPool.Create(4, ETrue));
	User::LeaveIfError(iReceivePool.Create(16, ETrue));
	}

TInt CMediaSession::DoActiveOpen()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStream, Panic(EAvdtpTransportSessionBaseNotCheckStream));
	TInt ret = KErrGeneral; // may be OOM or notfound

	ret = iStream->AddSession(EMedia,*this,iTransportChannel,TL2CapConfig::EHigh);
	if (ret!=KErrNone)
		{
		// not erroring the stream, as it's not it's fault
		// and it may not exist anyway!
		iStream = NULL;
		iSAP.Error(ret);
		}
	return ret;	
	}

TInt CMediaSession::Send(RMBufChain& aData, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	LOG_FUNC
	// if we get flow controlled should we *could* indicate to reporting session which could complete
	// an ioctl (for rtcp use)
//	if (iStream && iStream->CanSend(EMedia))
	TInt requestedToSend = aData.Length();
	TInt packetsLostThisSend = 0; // Packets lost during this send (if any)

	if (!iTransportChannelBlocked)
		{
		RMBufChain toSend;
		iSendPool.Remove(toSend);
		
		if (toSend.IsEmpty())
			{
			toSend.Assign(aData);
			}
		else
			{
			iSendPool.Add(aData, packetsLostThisSend);
			iPacketsLost += packetsLostThisSend;
			}
			
		DoSend(toSend);
		}
	else
		{
		iSendPool.Add(aData, packetsLostThisSend);
		iPacketsLost += packetsLostThisSend;
		}
	
	// If client app has previous sent pack drop IOCTL it needs to be notified
	if (iNotifyMediaPacketDroppedFlag && iPacketsLost)
		{
		TPckgBuf<TInt> buf(iPacketsLost);
		iSAP.ServiceComplete(&buf);
		iNotifyMediaPacketDroppedFlag = EFalse;
		iPacketsLost = 0;
		}
		
	return requestedToSend;
	}

// if pools become nicely generic then we could move this sending stuff to base class
// and have sessions just configure the Tx, Rx pools
void CMediaSession::DoSend(RMBufChain& aData)
	{
	LOG_FUNC
	if (iTransportChannel->SendPacket(EMedia, aData)==0)
		{
		// as media session, just free stale AV data (gets tricky putting stuff back on pool, and not really needed)
		// but mark TC as blokced so we can begin buffering in future
		aData.Free();
		iTransportChannelBlocked = ETrue;
		}
	}
	

void CMediaSession::CanSend()
	{
	LOG_FUNC
	// pluck off pool
	iTransportChannelBlocked = EFalse;
	//RDebug::Print(_L("MBuf free %d"), CMBufManager::Context()->__DbgGetBufSpace());
	// something was placed on the send pool - remove it and send
	RMBufChain chain;

	do
		{
		iSendPool.Remove(chain);
		if (!chain.IsEmpty())
			{
			DoSend(chain);
			}
		}
	while (!chain.IsEmpty());
	}

void CMediaSession::ReceivePacketLost()
	{
	// Increment the packet lost count.
	iPacketsLost++;
	
	// If client app has previous sent pack drop IOCTL it needs to be notified
	if (iNotifyMediaPacketDroppedFlag)
		{
		TPckgBuf<TInt> buf(iPacketsLost);
		iSAP.ServiceComplete(&buf);
		iNotifyMediaPacketDroppedFlag = EFalse;
		iPacketsLost = 0;
		}
	}

TInt CMediaSession::GetOption(TUint aLevel, TUint aName, TDes8& aOption) const
	{
	LOG_FUNC
	TInt ret = KErrArgument;
	if (aLevel == KSolBtAVDTPMedia)
		{
		switch (aName)
			{
			case EAvdtpMediaGetMaximumPacketSize:
				{
				if (iTransportChannel)
					{
					TInt mtu;
					ret = iTransportChannel->MaxPacketSize(mtu);
					if (ret==KErrNone)
						{
						LOG1(_L("Media session transport channel MTU = %d"), mtu);
						TPckg<TInt> mtuBuf(mtu);
						aOption = mtuBuf;
						}
					}
				else
					{
					ret = KErrNotReady;
					}
				}
			break;
			case EAvdtpMediaGetMaximumReceivePacketSize:
				{
				if (iTransportChannel)
					{
					TInt mru;
					ret = iTransportChannel->MaxReceivePacketSize(mru);
					if (ret==KErrNone)
						{
						LOG1(_L("Media session transport channel MRU = %d"), mru);
						TPckg<TInt> mruBuf(mru);
						aOption = mruBuf;
						}
					}
				else
					{
					ret = KErrNotReady;
					}
				}
			break;
			default:
				ret = KErrNotSupported;
			}
		}
			
	return ret;
	}

void CMediaSession::Ioctl(TUint aLevel, TUint aName, const TDesC8* /* aOption */)
	{
	LOG_FUNC
	TInt res;
	
	if (aLevel == KSolBtAVDTPMedia)	
		{
		switch (aName)
			{
			case ENotifyAvdtpMediaPacketDropped:
				{
				iNotifyMediaPacketDroppedFlag= ETrue;
				res = KErrNone;
				break;
				}
			default:
				{
				res = KErrNotSupported;
				break;
				}
			}
		if (res!=KErrNone)
			{
			iSAP.Error(res);
			}
		}
	}
