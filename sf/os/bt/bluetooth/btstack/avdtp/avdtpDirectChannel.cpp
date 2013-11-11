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
// Implements the avdtp "direct" (!muxed) transport channel object
// 
//

/**
 @file
 @internalComponent
*/
#include <bluetooth/logger.h>
#include "avdtpDirectChannel.h"
#include "avdtpTransportSession.h"
#include "avdtp.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CDirectChannel* CDirectChannel::NewL(CAvdtpProtocol& aProtocol,
						 	   		 const TBTDevAddr& aRemoteDevice)
	{
	LOG_STATIC_FUNC
	CDirectChannel* s = new (ELeave) CDirectChannel(aProtocol, aRemoteDevice);
	return s;
	}


CDirectChannel::CDirectChannel(CAvdtpProtocol& aProtocol,
							   const TBTDevAddr& aRemoteDevice)
: CTransportChannel(aProtocol, aRemoteDevice)
	{
	LOG_FUNC
	}
		
TInt CDirectChannel::AttachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType /*aType*/, TL2CapConfig::TChannelPriority aPriority)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iTransportSession, Panic(EAVDTPBadSessionAttachToTransportChannel));
	iTransportSession = &aSession;
	iChannelPriority = aPriority;
	UpdateChannelPriority();
	return KErrNone;
	}
	
void CDirectChannel::UpdateChannelPriority()
	{
	LOG_FUNC
	
	if(iLogicalChannel)
		{
		TPckgBuf<TL2CapConfig> configBuf;
		configBuf().ConfigureChannelPriority(iChannelPriority);
		(void)iLogicalChannel->SetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, configBuf);
		}
	}

TBool CDirectChannel::CouldAttachSession(const TAvdtpSockAddr& /*aAddr*/)
	{
	LOG_FUNC
	// for this channel we can take iff no session
	return (iTransportSession == NULL);
	}

TTCID CDirectChannel::TCID() const
	{
	LOG_FUNC
	return KDirectChannelTCID;
	}

CServProviderBase* CDirectChannel::ObtainSAP()
	{
	CServProviderBase* sap = iLogicalChannel;
	iLogicalChannel = NULL;
	return sap;
	}

void CDirectChannel::TransportSessionBlocked(TAvdtpTransportSessionType /*aSession*/, TBool aBlocked)
	{
	LOG_FUNC
	LOG1(_L("TransportSession blocked(%d"), aBlocked);
	iTransportSessionBlocked = aBlocked;
	}
	
/**
Just pass down directly to logical channel
No marshalling of other sessions to do, and no framing so just go
but this is by invite only! should check for this (only debug)
*/
TUint CDirectChannel::SendPacket(TTSID /*aTSID*/, RMBufChain& aPacket)
	{
	LOG_FUNC
	LOG1(_L("Packet length = %d bytes"), aPacket.Length());
	return iLogicalChannel ? iLogicalChannel->Write(aPacket,0) : 0;
	}
	
	
// from logical channel

/**
To reduce copying just forward indication on
Also this leaves the AV Sap or Session to work out what is best strategy to get data
*/
void CDirectChannel::NewData(TUint aCount)
	{
	LOG_FUNC
	if (iTransportSession)
		{
		RMBufChain chain;
		TInt err = KErrNone;
		while (aCount--)
			{
			err = iLogicalChannel->GetData(chain,0,0);  //returns negative error code or the number of datagrams ( = 1 )
			if (err > 0)
				{
				iTransportSession->NewData(chain);
				}
			}
		}
	}

void CDirectChannel::CanSend()
/**
upcall from logical channel (l2cap)
*/
	{
	LOG_FUNC
	iTransportSession->CanSend();
	}

#ifdef _DEBUG	
void CDirectChannel::DetachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType /*aType */)
#else
void CDirectChannel::DetachTransportSession(CUserPlaneTransportSession& /*aSession*/, TAvdtpTransportSessionType /*aType */)
#endif
	{
	LOG_FUNC
	__ASSERT_DEBUG(&aSession == iTransportSession, Panic(EAVDTPBadSessionDetachFromTransportChannel));
	iTransportSession = NULL;
	iChannelPriority = TL2CapConfig::ELow;
	
	UpdateChannelPriority();
	
	// closing logical channel is async - we die on CanClose
	// but we're invalid, so tell protocol
	Protocol().TransportChannelClosing(*this);
	CloseLogicalChannel();
	}
	
void CDirectChannel::Disconnect()
/**
	upcall from logical channel (l2cap)
*/
	{
	LOG_FUNC
	iLogicalChannel = NULL;
	if (iTransportSession)
		{
		iTransportSession->ChannelError(KErrDisconnected);
		}
	}
	
void CDirectChannel::Error(TInt aError, TUint /*aOperationMask*/)
/**
	upcall from logical channel (l2cap)
*/
	{
	LOG_FUNC
	if (iTransportSession)
		{
		iTransportSession->ChannelError(aError);
		}
	}
	
	
