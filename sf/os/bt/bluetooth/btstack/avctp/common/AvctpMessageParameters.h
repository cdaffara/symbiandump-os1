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
 @internalTechnology
*/

#ifndef AVCTPMESSAGEPARAMETERS_H
#define AVCTPMESSAGEPARAMETERS_H

#include <bluetoothav.h>

/**
AVCTP Message Parameters

Used to set parameters for sending AVCTP messages

@internalTechnology
*/
const static TUint16 KParametersMessageMask		= 0xFF00;
const static TUint16 KParametersTransactionMask	= 0x00F0;

NONSHARABLE_CLASS(TAvctpMessageParameters)
	{
public:
	TAvctpMessageParameters() : iType(SymbianAvctp::EUndefined), iTransaction() {};
	TAvctpMessageParameters(TUint aFlags)
		:	iType(static_cast<SymbianAvctp::TMessageType>((aFlags & KParametersMessageMask) >> 8)),
			iTransaction(static_cast<SymbianAvctp::TTransactionLabel>((aFlags & KParametersTransactionMask) >> 4)) {};
	inline void SetType(SymbianAvctp::TMessageType aType) {iType = aType;};
	SymbianAvctp::TMessageType Type() {return static_cast<SymbianAvctp::TMessageType>(iType);};
	TUint GetFlags() {return (static_cast<TUint8>(iType) << 8) + (((static_cast<TUint8>(iTransaction) & 0xF) << 4));};

public:
	SymbianAvctp::TTransactionLabel iTransaction;
private:
	SymbianAvctp::TMessageType iType;
	};
	
#endif // AVCTPMESSAGEPARAMETERS_H

