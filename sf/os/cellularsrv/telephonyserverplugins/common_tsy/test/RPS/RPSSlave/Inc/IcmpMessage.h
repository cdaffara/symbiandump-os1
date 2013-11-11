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
// cmpMessage.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __ICMP_MSG_H__
#define __ICMP_MSG_H__

#include <icmp6_hdr.h>
#include <in_chk.h>


const TUint KIcmpHeaderSize = 8;
const TUint KMinIpHeaderSize = 20;

const TInt KDefaultPingSize = 32;

// ICMP Type and Code values
const TUint KIPv4PingTypeEchoReply				= 0;
const TUint KIPv4PingCodeEcho					= 0;
const TUint KIPv4PingTypeEchoRequest			= 8;
const TUint KIPv4PingTypeUnreachable			= 3;
const TUint	KIPv4PingCodeUnreachNet				= 0;
const TUint	KIPv4PingCodeUnreachHost			= 1;
const TUint	KIPv4PingCodeUnreachProtocol		= 2;
const TUint	KIPv4PingCodeUnreachPort			= 3;
const TUint	KIPv4PingCodeUnreachNeedFrag		= 4;
const TUint	KIPv4PingCodeUnreachSrcRouteFail	= 5;
const TUint	KIPv4PingCodeUnreachNetUnknown		= 6;
const TUint	KIPv4PingCodeUnreachHostUnknown		= 7;
const TUint	KIPv4PingCodeUnreachSrcHostIsolated	= 8;	
const TUint	KIPv4PingCodeUnreachNetProhibited	= 9;
const TUint	KIPv4PingCodeUnreachHostProhibited	= 10;
const TUint	KIPv4PingCodeUnreachNetTOS			= 11;
const TUint	KIPv4PingCodeUnreachHostTOS			= 12;
const TUint	KIPv4PingCodeUnreachProhibited		= 13;
const TUint	KIPv4PingCodeUnreachPrecVolation	= 14;
const TUint	KIPv4PingCodeUnreachPrecCutoff		= 15;
const TUint KIPv4PingTypeSourceQuench			= 4;
const TUint KIPv4PingTypeRedirect				= 5;
const TUint	KIPv4PingCodeRedirectNet			= 0;
const TUint	KIPv4PingCodeRedirectHost			= 1;
const TUint	KIPv4PingCodeRedirectNetTOS			= 2;
const TUint	KIPv4PingCodeRedirectHostTOS		= 3;
const TUint KIPv4PingTypeTimeExceeded			= 11;
const TUint	KIPv4PingCodeExceedInTransit		= 0;
const TUint	KIPv4PingCodeExceedInReasm			= 1;
const TUint KIPv4PingTypeBadParameter			= 12;

const TUint KIPv6PingTypeEchoReply				= 129;
const TUint KIPv6PingTypeEchoRequest			= 128;
const TUint KIPv6PingCodeEcho					= 0;
const TUint KIPv6PingTypeUnreachable			= 1;
const TUint KIPv6PingCodeNoRoute				= 0;
const TUint KIPv6PingCodeAdminProhibited		= 1;
const TUint KIPv6PingCodeAddressUnreachable		= 3;
const TUint KIPv6PingCodePortUnreachable		= 4;
const TUint KIPv6PingTypePacketTooBig			= 2;
const TUint KIPv6PingCodePacketTooBig			= 0;
const TUint KIPv6PingTypeTimeExeeded			= 3;
const TUint KIPv6PingCodeHLExeeded				= 0;
const TUint KIPv6PingCodeFragReassemblyExeeded	= 1;
const TUint KIPv6PingTypeParamProblem			= 4;
const TUint KIPv6PingCodeErroneousHeader		= 0;
const TUint KIPv6PingCodeNextHeaderUnrecognised	= 1;
const TUint KIPv6PingCodeIPv6OptionUnrecognised = 2;

class HPingHeader : public TInet6HeaderICMP_Echo
	{
public:
	static HPingHeader* NewL(TInt aSize = KIcmpHeaderSize, TUint aIPVersion = KAfInet);
	~HPingHeader();

	TBool VerifyRecvEcho(TInt aId);
	TBool VerifyNonEcho(TInt aId);
	void SetVersion(TUint aIPVersion);
	void FormatSend(TUint aId, TUint aSeqNum);
	TInt MaxLength();
	TInt DataLength();
	TPtr8* Grab();
	TPtrC8 IcmpContents();

private:
	void ConstructL(TInt aSize, TUint aIPVersion);
	TBool SetHeader(TUint aOffset = 0);
	
	HBufC8* iData;
	TPtr8* iDataPtr;
	TUint iIPVersion;
	};
	
#endif
