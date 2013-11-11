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
//


/**
 @file
 @publishedAll
 @released
*/

#if !defined (__ES_ENUM_H__)
#define __ES_ENUM_H__

#include <e32std.h>
#include <nifvar.h>
#include <es_sock.h>


class TConnArgBase
/**
 * Base class for many arguments to RConnection functions.  Provides versioning information.
 * @note Not intended for user instantiation
 * @publishedAll
 * @released since v7.0s
 */
	{
public:
	inline TConnArgBase();
	inline TConnArgBase(const TDesC8& aConnectionInfo)
	:	iVersion(*((TUint8*)aConnectionInfo.Ptr()))
		{
		}
	inline TUint8 Version() const;

protected:
	inline TConnArgBase(TUint8 aVersion);

protected:
	TUint8 iVersion;
	TUint8 iReserved[3];
	};

/**
 * Versioning information for connection arguments
 * @publishedAll
 * @released since v7.0s
 */
const TUint8 KConnArgVersion1 = 1;

/**
Versioning information for connection arguments

@publishedAll
@released
*/
const TUint8 KConnArgVersion2 = 2;

class TConnectionInfo : public TConnArgBase
/**
@publishedAll
@released since v7.0s
*/
	{
public:
	inline TConnectionInfo();
	TConnectionInfo(const TDesC8& aConnectionInfo /*as passed to ::Bearer*/);
	inline TConnectionInfo(TUint32 aIapId, TUint32 aNetId);

	static TBool IsLocalBearer(const TDesC8& aConnectionInfo);

	inline bool operator == (const TConnectionInfo& aRhs) const;

protected:
	inline TConnectionInfo(TUint8 aVersion, TUint32 aIapId, TUint32 aNetId);

public:
	TUint32 iIapId;
	TUint32 iNetId;
	};

const TUint8 KConnArgVersion3 = 3;


/**
@publishedAll
@released since v7.0s
*/
typedef TPckgBuf<TConnectionInfo> TConnectionInfoBuf;




/**
The following classes are used with RConnection::AllInterfaceNotification() to receive information
about interfaces coming up and going down.
*/

/**
@publishedAll
@released since v7.0s
*/
enum TConnInterfaceState
	{
	EInterfaceUp,
	EInterfaceDown
	};

class TInterfaceNotification
/**
@publishedAll
@released since v7.0s
*/
	{
public:
	TConnectionInfo iConnectionInfo;
	TConnInterfaceState iState;
	};


/**
Types of events that can occur on a subconnection

@publishedAll
@released
*/
enum TSubConnectionEventType
/**
 * Types of events that can occur on a subconnection
 * @publishedPartner
 * @released since v7.0s
 */
	{
	ESubConnectionOpened,           //< Connection values 0-9999 for agent-based events
	ESubConnectionClosed,           //< Connection values 0-9999 for agent-based events
	ESubConnectionQoSChanged = 1000 //< Connection values 10000-19999 for nif-based events
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_partner.h>
#include <es_enum_internal.h>
#endif

#include <es_enum.inl>


#endif // __ES_ENUM_H__

