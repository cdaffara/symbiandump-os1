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
// Implements the avdtp transport channel base class
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpTransportChannel.h"
#include "avdtpDirectChannel.h"
#include "avdtpMuxChannel.h"
#include "avdtp.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CTransportChannel::CTransportChannel(CAvdtpProtocol& aProtocol,
							  		 const TBTDevAddr& aRemoteDevice)
	: iProtocol(aProtocol),
	  iRemoteDevice(const_cast<TBTDevAddr&>(aRemoteDevice))
	{
	LOG_FUNC
	}
	
CTransportChannel::~CTransportChannel()
	{
	LOG_FUNC
	iProtocol.TransportChannelDown(*this);

	if (iLogicalChannel)
		{
		iLogicalChannel->Shutdown(CServProviderBase::EImmediate);
		delete iLogicalChannel;
		}
	}

/*static*/ CTransportChannel* CTransportChannel::NewDirectChannelL(CAvdtpProtocol& aProtocol,
							  const TBTDevAddr& aRemoteDevice)
	{
	LOG_STATIC_FUNC
	return CDirectChannel::NewL(aProtocol, aRemoteDevice);
	}


/*static*/ CTransportChannel* CTransportChannel::NewMuxChannelL(CAvdtpProtocol& aProtocol,
							  const TBTDevAddr& aRemoteDevice,
							  TTCID aRemotelyAssignedTCID/*=KInvalidTCID*/)
	{
	LOG_STATIC_FUNC
	return CMuxChannel::NewL(aProtocol, aRemoteDevice, aRemotelyAssignedTCID);
	}

TInt CTransportChannel::SetOption(TUint aLevel, TUint aName, const TDesC8 &aOption)
	{
	LOG_FUNC
	return iLogicalChannel->SetOption(aLevel, aName, aOption);
	}

void CTransportChannel::CanClose(TDelete aDelete)
	{
	LOG_FUNC
	switch (aDelete)
		{
		case EDelete:
			delete iLogicalChannel;
		// fallthrough
		default:
			iLogicalChannel = NULL;
		}
	delete this;
	}
	
void CTransportChannel::LogicalChannelComplete(TLogicalChannelRecord aLogicalChannelRecord, TInt aErr)
	{
	LOG_FUNC
	if (aErr)
		{
		// try to tell any sessions present...
		static_cast<MSocketNotify*>(this)->Disconnect();		
		}
	else
		{
		// take ownership
		iLogicalChannel = aLogicalChannelRecord.iLogicalChannelSAP;
		iLogicalChannel->SetNotify(this);
		UpdateChannelPriority();
		// drain any data
		if (aLogicalChannelRecord.iDataCount)
			{
			NewData(aLogicalChannelRecord.iDataCount);
			
			if (aLogicalChannelRecord.iEndOfData)
				{
				NewData(KNewDataEndofData);
				}
			}
		}
	}
	
void CTransportChannel::AllowServices(TAvdtpAdaptationServices aServices)
	{
	LOG_FUNC
	// retained by base class - the derivee can choose whether to use it
	iAllowedServices = aServices;
	}


void CTransportChannel::CloseLogicalChannel()
	{
	LOG_FUNC
	if (iLogicalChannel)
		{
		iLogicalChannel->Shutdown(CServProviderBase::ENormal);
		}
	else CanClose();
	}


TInt CTransportChannel::MaxPacketSize(TInt& aMTU)
	{
	LOG_FUNC
	// default is just whatever l2cap is
	TInt ret = KErrNotReady;
	if (iLogicalChannel)
		{
		TPckg<TInt> mtuPckg(aMTU);
		ret = iLogicalChannel->GetOption(KSolBtL2CAP, KL2CAPOutboundMTUForBestPerformance, mtuPckg);
		}
	else
		{
		aMTU = 0;
		}
	return ret;
	}
	
TInt CTransportChannel::MaxReceivePacketSize(TInt& aMRU)
	{
	LOG_FUNC
	// default is just whatever l2cap is
	TInt ret = KErrNotReady;
	if (iLogicalChannel)
		{
		TPckg<TInt> mruPckg(aMRU);
		ret = iLogicalChannel->GetOption(KSolBtL2CAP, KL2CAPInboundMTU, mruPckg);
		}
	else
		{
		aMRU = 0;
		}
	return ret;
	}
