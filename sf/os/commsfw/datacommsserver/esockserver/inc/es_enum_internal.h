// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#if !defined (__ES_ENUM_INTERNAL_H__)
#define __ES_ENUM_INTERNAL_H__

#include <es_enum.h>
#include <es_enum_partner.h>

class TConnProviderInfo : public TConnArgBase
/**
@internalTechnology
*/
	{
public:
	inline TConnProviderInfo()
		{
		iVersion = KConnArgVersion3;
		Mem::FillZ(iInfo,sizeof(iInfo));
		}

public:
	TUint32 iInfo[5];
	};

/**
@internalTechnology
*/
typedef TPckgBuf<TInterfaceNotification> TInterfaceNotificationBuf;

/**
@internalComponent
*/
typedef TPckgBuf<TConnectionEnumArg> TConnEnumArgBuf;

/**
@internalComponent
*/
typedef TPckgBuf<TConnectionGetClientInfoArg> TConnGetClientInfoArgBuf;

class TConnectionSocketInfo
/**
@internalComponent
*/
	{
public:
	inline TConnectionSocketInfo();
public:
	TUint iAddressFamily;
	TUint iProtocol;
	TSockAddr iSourceAddress;
	TSockAddr iDestinationAddress;
	};

class TConnectionGetSocketInfoArg : public TConnArgBase
/**
for KCoGetConnectionSocketInfo

@internalComponent
*/
	{
public:
	TConnectionGetSocketInfoArg();
public:
	TUint iIndex;
	TConnectionSocketInfo iSocketInfo;
	};

/**
@internalComponent
*/
typedef TPckgBuf<TConnectionGetSocketInfoArg> TConnGetSocketInfoArgBuf;

/**
This enum is used as an internal extension to TConnInterfaceState

@internalComponent
*/
enum TConnInterfaceStateInternal
    {
    EInterfaceRestarting = 0x100
    };


#include <es_enum_internal.inl>

#endif // __ES_ENUM_INTERNAL_H__

