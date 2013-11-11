// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "avctpOutboundQ.h"
#include "avctpcommon.h"

using namespace SymbianAvctp;

//
// CAvctpFragStartPacket utility functions
//
	
inline TAvctpHeaderInfo::TAvctpHeaderInfo()
	: iTransactionLabel(SymbianAvctp::KMaxTransactionLabel),
	  iPktType(SymbianAvctp::EUnknownPkt),
	  iMsgType(SymbianAvctp::EUndefined)
	{
	}

inline TAvctpHeaderInfo::TAvctpHeaderInfo(SymbianAvctp::TTransactionLabel aTransactionLabel,
										  SymbianAvctp::TPacketType aPktType,
										  SymbianAvctp::TMessageType aMsgType)
	: iTransactionLabel(aTransactionLabel),
	  iPktType(aPktType),
	  iMsgType(aMsgType)
	{
	}

inline SymbianAvctp::TPduHeaderLength TAvctpHeaderInfo::HeaderLength() const
	{
	return ConvertPacketTypeToHeaderLength(iPktType);
	}


inline TAvctpNormalHeaderInfo::TAvctpNormalHeaderInfo()
	: TAvctpHeaderInfo(),
	  iHasValidPid(EFalse), 
	  iPid(0)
	{
	}

inline TAvctpNormalHeaderInfo::TAvctpNormalHeaderInfo(
							  SymbianAvctp::TTransactionLabel aTransactionLabel,
							  SymbianAvctp::TPacketType aPktType,
							  SymbianAvctp::TMessageType aMsgType,
							  TBool aHasValidPid,
							  SymbianAvctp::TPid aPid)
	: TAvctpHeaderInfo(aTransactionLabel, aPktType, aMsgType),
	  iHasValidPid(aHasValidPid),
	  iPid(aPid)			  
	{
	}

inline TAvctpStartHeaderInfo::TAvctpStartHeaderInfo()
	: TAvctpNormalHeaderInfo(),
	  iFragmentsInSdu(0)
	{
	}

inline TAvctpStartHeaderInfo::TAvctpStartHeaderInfo(
								 SymbianAvctp::TTransactionLabel aTransactionLabel,
								 SymbianAvctp::TPacketType aPktType,
								 SymbianAvctp::TMessageType aMsgType,
								 TBool aHasValidPid,
								 SymbianAvctp::TPid aPid,
								 TUint aFragmentsInSdu)
	: TAvctpNormalHeaderInfo(aTransactionLabel, aPktType, aMsgType, aHasValidPid, aPid),
	  iFragmentsInSdu(aFragmentsInSdu)
	{
	}

//
// Outgoing Sdu

inline HAvctpOutgoingSdu::HAvctpOutgoingSdu(
						 const TAvctpNormalHeaderInfo& aHeaderInfo,
					 	 const TBTDevAddr& aAddr)
	: iHeaderInfo(aHeaderInfo),
	  iAddr(aAddr),
	  iOutboundQ(NULL),
	  iChannel(KAvctpPrimaryChannel)
	{
	}
	
inline const TBTDevAddr& HAvctpOutgoingSdu::BTAddr() const
	{
	return iAddr;
	}
	
inline TAvctpNormalHeaderInfo& HAvctpOutgoingSdu::HeaderInfo()
	{
	return iHeaderInfo;
	}

inline RMBufChain& HAvctpOutgoingSdu::Data()
	{
	return iSduData;
	}

inline void HAvctpOutgoingSdu::AddToQueue(TAvctpOutboundQ* aOutboundQ)
	{
	// This needs to be local as it is in a header file
	#ifdef __FLOG_ACTIVE
	_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
	#endif
	
	#ifdef _DEBUG
	PANICCATEGORY("pktinl");
	#endif

// We shouldn't already be on a Q and we need to be given a valid Q
	ASSERT_DEBUG(iOutboundQ == NULL && aOutboundQ != NULL);
	iOutboundQ = aOutboundQ;
	}
	
//
// Incoming Sdu

inline HAvctpIncomingSdu::HAvctpIncomingSdu(const TBTDevAddr& aAddr, 
											RMBufChain& aSduData)
	: iAddr(aAddr)
	{
	iSduData.Assign(aSduData);
	}
	
inline const TBTDevAddr& HAvctpIncomingSdu::BTAddr() const
	{
	return iAddr;
	}

inline TInt HAvctpOutgoingSdu::Channel() const
	{
	return iChannel;
	}

inline const RMBufChain& HAvctpIncomingSdu::Data() const
	{
	return iSduData;
	}
		
inline HAvctpIncomingSdu::~HAvctpIncomingSdu()
	{
	iSduData.Free();
	iQueLink.Deque();
	}
