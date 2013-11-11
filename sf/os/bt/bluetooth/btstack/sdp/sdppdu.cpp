// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sdppdu.h"

CSdpPdu::CSdpPdu(CSdpNetDbProvider* aSdpNetDbProv)
	: iBuf(KSdpPduHeaderSize), //Reserve Space for PDU header
	iNetDbProvider(aSdpNetDbProv)
	{
	}

CSdpPdu::~CSdpPdu()
	{
	iLink.Deque();
	}

void CSdpPdu::SetPduId(TUint8 aPduId)
	{
	iBuf[KSdpPduIdOffset] = aPduId;
	}

TUint8 CSdpPdu::PduId()
	{
	return iBuf[KSdpPduIdOffset];
	}

void CSdpPdu::SetTransid(TUint16 aTransId)
	{
	BigEndian::Put16(&iBuf[KSdpPduTransIdOffset], aTransId);
	}

TUint16 CSdpPdu::TransId()
	{
	return BigEndian::Get16(&iBuf[KSdpPduTransIdOffset]);
	}

void CSdpPdu::SetLength()
	{
	BigEndian::Put16(&iBuf[KSdpPduParamLengthOffset],
					 static_cast<TUint16>(iBuf.Length()-KSdpPduHeaderSize));
	}

void CSdpPdu::PutByte(TUint8 aByte)
	{
	iBuf.Append(aByte);
	}

void CSdpPdu::PutBigEndian16(TUint16 aShort)
	{
	TInt len = iBuf.Length();
	iBuf.SetLength(len+sizeof(aShort));
	BigEndian::Put16(&iBuf[len], aShort);
	}

void CSdpPdu::PutBigEndian32(TUint32 aLong)
	{
	TInt len = iBuf.Length();
	iBuf.SetLength(len+sizeof(aLong));
	BigEndian::Put32(&iBuf[len], aLong);
	}

void CSdpPdu::PutData(const TDesC8& aDes)
	{
	iBuf.Append(aDes);
	}

TDesC8& CSdpPdu::Data()
	{
	return iBuf;
	}

CSdpNetDbProvider* CSdpPdu::NetDbProvider()
	{
	return iNetDbProvider;
	}

