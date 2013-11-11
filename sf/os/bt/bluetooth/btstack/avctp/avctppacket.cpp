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

#include <bluetooth/logger.h>
#include <es_sock.h>
#include "avctppacket.h"
#include "avctpcommon.h"
#include "avctpmuxer.h"
#include "avctputils.h"
#include "avctpconstants.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
#endif

#ifdef _DEBUG
PANICCATEGORY("avctppkt");
#endif

using namespace SymbianAvctp;

/*static*/ TPtrC8 CAvctpPacket::GetHeader(const RMBufChain& aChain)
	{
	const RMBuf* mbuf = aChain.First();
	// set the descriptor to read out the whole of the first mbuf (into which the header fits)
	return TPtrC8(mbuf->Ptr(), mbuf->Length());
	}


/**
Fills in the TAvctpHeaderInfo 

This function takes a TAvctpStartHeaderInfo rather than a TAvctpHeaderInfo to
avoid problems with casting from TAvctpHeaderInfo to TAvctpStartHeaderInfo & 
mangling data.

  @internalComponent
  @param aData The data buffer that contains the AVCTP packet to be parsed
  @param aHeaderInfo will contain the result of the parsing.
  @return KErrNone if there was no problem parsing the header, otherwise returns an 
  				   Avctp specific error.
*/
/*static*/ TInt CAvctpPacket::ParseHeader(const RMBufChain& aData, TAvctpStartHeaderInfo& aHeaderInfo)
	{
	LOG_STATIC_FUNC

	TInt err = KErrNone;
	
	// We require the packet to be long enough to at least contain the packet type
	const TPtrC8 dataP(GetHeader(aData));
	
	if (dataP.Length() < EMinimumHeaderLength)
		{
		err = KErrPacketTooShort;
		}
		
	if (err == KErrNone)
		{
		aHeaderInfo.iMsgType = SymbianAvctp::MessageType(dataP);
		aHeaderInfo.iTransactionLabel = SymbianAvctp::TransactionLabel(dataP);
		}

	// No checking needs to be done on the TAvctpHeaderInfo variables since
	// all values are allowed.
		
	// get the packet type		
	if (err == KErrNone)
		{
		aHeaderInfo.iPktType = SymbianAvctp::PacketType(dataP);
		switch (aHeaderInfo.iPktType)
			{
			case ENormalPkt:
				if (dataP.Length() < ENormalHeaderLength)
					{
					err = KErrPacketTooShort;
					}
				else 
					{
					err = ParseNormalHeader(dataP, aHeaderInfo);
					}
				break;
			case EStartFrag:
				if (dataP.Length() < EStartFragHeaderLength)
					{
					err = KErrPacketTooShort;
					}
				else 
					{
					err = ParseStartHeader(dataP, aHeaderInfo);									
					}
				break;
			case EContinueFrag:
			case EEndFrag:
				// the Size check before this switch statement ensures that
				// the given data is long enough since EOtherFragHeaderLength = 1
				break;
			default:
				err = KErrUnknownPacketType;
				break;
			}
		}	
	return err;
	}
	
/**
Fills in the TAvctpNormalHeaderInfo 
Assumes aData is at least ENormalHeaderLength long

  @internalComponent
  @param aData The data buffer that contains the AVCTP packet to be parsed
  @param aHeaderInfo will contain the result of the parsing.
  @return KErrNone if there was no problem parsing the header, otherwise returns an 
  				   Avctp specific error.
*/
/*static*/ TInt CAvctpPacket::ParseNormalHeader(const TDesC8& aData, TAvctpNormalHeaderInfo& aHeaderInfo)
	{
	LOG_STATIC_FUNC

	aHeaderInfo.iHasValidPid = IsValidPid(aData);
	aHeaderInfo.iPid = Pid(aData);
	return CheckNormalHeaderInfo(aHeaderInfo);
	}
	
/**
Fills in the TAvctpStartHeaderInfo 
Assumes aData is at least EStartFragHeaderLength long

  @internalComponent
  @param aData The data buffer that contains the AVCTP packet to be parsed
  @param aHeaderInfo will contain the result of the parsing.
  @return KErrNone if there was no problem parsing the header, otherwise returns an 
  				   Avctp specific error.
*/
/*static*/ TInt CAvctpPacket::ParseStartHeader(const TDesC8& aData, TAvctpStartHeaderInfo& aHeaderInfo)
	{
	LOG_STATIC_FUNC

	TInt err = ParseNormalHeader(aData, aHeaderInfo);
	if (err == KErrNone)
		{
		aHeaderInfo.iFragmentsInSdu = FragmentsInSdu(aData);
		err = CheckStartHeaderInfo(aHeaderInfo);
		}
		
	return err;
	}

/*
Checks the validity of a TAvctpNormalHeaderInfo object. Note that the variables
derived from TAvctpHeaderInfo cannot be incorrect since all values that might appear
in a air packet are allowed.

(NB the RFA field in Continue and End packets [the IPID field in other types of packet]
must be set to 0 in the AVCTP v1.0 spec. However, we don't enforce this on incoming
in case it is used in future.)
*/ 
/*static*/ TInt CAvctpPacket::CheckNormalHeaderInfo(const TAvctpNormalHeaderInfo& aHeaderInfo)
	{
	LOG_STATIC_FUNC

	ASSERT_DEBUG(aHeaderInfo.iPktType == ENormalPkt ||
		   aHeaderInfo.iPktType == EStartFrag);
		
	TInt ret = KErrNone;
	if (!aHeaderInfo.iHasValidPid && aHeaderInfo.iMsgType == ECommand)
		{
		ret = KErrMalformedPacketFromRemote;
		}
	return ret;
	}

/*
Checks the validity of a TAvctpStartHeaderInfo object. 
*/ 	
/*static*/ TInt CAvctpPacket::CheckStartHeaderInfo(const TAvctpStartHeaderInfo& aHeaderInfo)
	{
	LOG_STATIC_FUNC

	ASSERT_DEBUG(aHeaderInfo.iPktType == EStartFrag);
	
	TInt ret = KErrNone;
	if (aHeaderInfo.iFragmentsInSdu > 1)
		{
		ret = CheckNormalHeaderInfo(aHeaderInfo);
		}
	else
		{
		ret = KErrMalformedPacketFromRemote;
		}
		
	return ret;
	}
	

//
//                                                                                       //
// HAvctpOutgoingSdu Implementation                                                    //
//                                                                                       //
//

HAvctpOutgoingSdu* HAvctpOutgoingSdu::NewL(const TAvctpNormalHeaderInfo& aHeaderInfo,
							 			   const TBTDevAddr& aAddr,
					 		 			   RMBufChain& aData)
	{
	LOG_STATIC_FUNC

	HAvctpOutgoingSdu* sdu = new(ELeave) HAvctpOutgoingSdu(aHeaderInfo, aAddr);
	CleanupStack::PushL(sdu);
	sdu->ConstructL(aData);
	CleanupStack::Pop(sdu);
	return sdu;
	}

/** 
Create the buffer by copying over the other descriptor allowing space for a header
at the beginning
*/	
void HAvctpOutgoingSdu::ConstructL(RMBufChain& aData)
	{
	LOG_FUNC
	iSduData.Assign(aData);
	}

HAvctpOutgoingSdu* HAvctpOutgoingSdu::NewIpidResponseL(const HAvctpIncomingSdu& aIncomingSdu, TInt aChannel)
	{
	LOG_STATIC_FUNC
	
	TPtrC8 hdr(CAvctpPacket::GetHeader(aIncomingSdu.Data()));
	
	LOG1(_L("to Pid 0x%x"), Pid(hdr));
	
	TAvctpNormalHeaderInfo headerInfo = 
		TAvctpNormalHeaderInfo(TransactionLabel(hdr),
							   ENormalPkt,
							   EResponse,
							   EFalse,
							   Pid(hdr)
							   );

	HAvctpOutgoingSdu* sdu = new(ELeave) HAvctpOutgoingSdu(headerInfo, aIncomingSdu.BTAddr());
	sdu->iChannel = aChannel;
	
	return sdu;
	}
				
HAvctpOutgoingSdu::~HAvctpOutgoingSdu()
	{
	LOG_FUNC

	iSduData.Free();
	
	if (iOutboundQ)
		{
		iOutboundQ->Deque(*this);
		}
	else
		{
		iQueLink.Deque();	
		}
	}


