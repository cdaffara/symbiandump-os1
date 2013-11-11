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

#include "IcmpMessage.h"	
	
	
HPingHeader::~HPingHeader()
//
// D'tor deletes 
//
	{
	
	delete iData;
	delete iDataPtr;
	}

HPingHeader* HPingHeader::NewL(TInt aSize, TUint aIPVersion)
//
// Create a new ping header
//
	{
	
	HPingHeader* h = new(ELeave) HPingHeader();

	CleanupStack::PushL(h);
	h->ConstructL(aSize, aIPVersion);
	CleanupStack::Pop(h);	

	return h;
	}

void HPingHeader::ConstructL(TInt aSize, TUint aIPVersion)
	{
	
	iData = HBufC8::NewL(aSize);
	iDataPtr = new(ELeave) TPtr8(iData->Des());
	
	iData->Des().FillZ();	
	SetVersion(aIPVersion);
	}

void HPingHeader::SetVersion(TUint aIPVersion)
//
// To set IP version of packet
// 
	{
	
	iIPVersion = aIPVersion; 
	}

TInt HPingHeader::MaxLength()
//
// To get packet data maximum length
// 
	{
	
	return iData->Des().MaxLength(); 
	}

TInt HPingHeader::DataLength()
//
// To get packet data length
// 
	{
	
	return iData->Des().Length(); 
	}

TPtrC8 HPingHeader::IcmpContents()
//
// To get the icmp contents from packet data
// 
	{
       
	return iData->Des().Mid(4, iData->Length()-4); 
	}

TPtr8* HPingHeader::Grab()
//
// To get data far a send or receive operation
// 
	{

	iDataPtr->Copy(iData->Des());
	return iDataPtr;
	}
	
void HPingHeader::FormatSend(TUint aId, TUint aSeqNum)
//
// Format an ICMP packet to send
//
	{
	
	TUint type;
	TUint code;
	TChecksum sum;

	// Configure version
	if(iIPVersion == KAfInet)
		{
		type = KIPv4PingTypeEchoRequest;
		code = KIPv4PingCodeEcho;
		}
	else
		{
		type = KIPv6PingTypeEchoRequest;
		code = KIPv6PingCodeEcho;
		}

	// Fill header
	SetType(static_cast<TUint8>(type));
	SetCode(static_cast<TUint8>(code));
	SetIdentifier(static_cast<TUint16>(aId));
	SetSequence(static_cast<TUint16>(aSeqNum));

	// Compute checksum
	SetChecksum(0);
	sum.Add(reinterpret_cast<TUint16*>(this), HeaderLength());
	SetChecksum(sum.Sum());

	// Fill buffer
	iData->Des().Copy((TUint8*)this, HeaderLength());
	}

TBool HPingHeader::SetHeader(TUint aOffset)
//
// Set the header from an Icmp reply
// 
	{
	
	const TUint8* buffData; 
	
	// Check size
	if(DataLength() < (TInt)KIcmpHeaderSize)
		{
		return EFalse;
		}
	
	buffData = iData->Des().Ptr();

	if(!buffData)
		{
		return EFalse;
		}
	
	// Fill TInet6HeaderICMP_Echo from the buffer
	for(int k=0;k<(TInt)KIcmpHeaderSize;k++)
		{
		i[k] = *(buffData + k + aOffset);
		}

	return ETrue;
	}

TBool HPingHeader::VerifyRecvEcho(TInt aId)
//
// Verifiy header is valid echo reply
//
	{
	
	TBool ret = ETrue;
	TUint typeCheck;
	TUint codeCheck;

	// Fill TInet6HeaderICMP_Echo from packet data
	ret = SetHeader();
	
	// Look at IP version
	if(iIPVersion == KAfInet)
		{
		typeCheck = KIPv4PingTypeEchoReply;
		codeCheck = KIPv4PingCodeEcho;
		}
	else
		{
		typeCheck = KIPv6PingTypeEchoReply;
		codeCheck = KIPv6PingCodeEcho;
		}
		
	// Wrong packet type or code
	if(ret && (Type() != typeCheck || Code() != codeCheck))
		{
		ret = EFalse;
		}

	// Wrong packet identifier
	if(ret && Identifier() != aId)
		{
		ret = EFalse;
		}

	return ret;
	}

TBool HPingHeader::VerifyNonEcho(TInt aId)
//
// Verify header which is not echo reply
// 
	{
	
	// Fill TInet6HeaderICMP_Echo from packet data
	TBool ret = SetHeader();

	// Look at IP version
	if(ret && iIPVersion == KAfInet)	// IP4
		{
		switch(Type())
			{
		case KIPv4PingTypeUnreachable:
		case KIPv4PingTypeSourceQuench:
		case KIPv4PingTypeRedirect:
		case KIPv4PingTypeTimeExceeded:
		case KIPv4PingTypeBadParameter:
			break;
		default:
			ret = EFalse;
			}
	
		if(ret && (DataLength() < (TInt)KIcmpHeaderSize))
			{
		    	ret = EFalse;
			}

		if(ret)
			{
			ret = SetHeader(KIcmpHeaderSize + KMinIpHeaderSize);
			if(ret && (Type() != KIPv4PingTypeEchoRequest || Identifier() != aId))
				{
				ret = EFalse;
				}
			}
		}
	else
		{				// IP6
		switch(Type())
			{
		case KIPv6PingTypeUnreachable:
		case KIPv6PingTypePacketTooBig:
		case KIPv6PingTypeTimeExeeded:
		case KIPv6PingTypeParamProblem:
			break;
		default:
			ret = EFalse;
			}
		}

	return ret;
	}

