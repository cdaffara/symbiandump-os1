// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Internal constants for the LBS Host Settings Store implementation
// 
//

/**
 @file
 @internalAll
 @deprecated 
*/

#include <e32base.h>

#ifndef LBSHOSTSETTINGSCONSTS_H_
#define LBSHOSTSETTINGSCONSTS_H_

const TUint32 KSettingIdMask = 0x00FFFFFF;
const TUint32 KRecordTypeMask = ~KSettingIdMask;
const TUint32 KRecordTypeShift = 28; 

enum TLbsHostSettingRecordType 
	{
	EMetaData         = 0x0, // TLbsHostSettingMetaRecordTypes
	ECreatorId        = 0x1, // TLbsHostCreatorId
	EConnectionId     = 0x2, // TInt
	EConnectionIdType = 0x3, // TLbsConnectionType
	EHostAddress      = 0x4, // TLbsHostNameAddress
	EReadOnly         = 0x5, // TBool
	EHostPort         = 0x6, // TInt
	EHostName         = 0x7, // TLbsHostName
	EProviderId       = 0x8, // TInt
	ETimestamp        = 0x9, // TUInt
	EAuthMoLr	  	  = 0xA, // TUInt
	EAuthMtLr         = 0xB, // TUInt
	EMaxRecordType    = 0xFF
	};

enum TLbsHostSettingMetaRecordTypes 
	{
	EInvalid          = 0x0,
	ENextKey          = 0x1,
	EDefaultHost      = 0x2,
	};

#endif // LBSHOSTSETTINGSCONSTS_H_
