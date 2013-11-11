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
 @publishedPartner
 @released
*/

#if !defined (__ES_ENUM_PARTNER_H__)
#define __ES_ENUM_PARTNER_H__

#include <es_enum.h>

class TConnectionInfoV2 : public TConnectionInfo
/**
 * Connection information structure (version 2)
 * Extends basic connection info with connection type information
 * @publishedPartner
 * @released since v7.0s
 * @see TConnectionInfo
*/
	{
public:
	inline TConnectionInfoV2();
	inline TConnectionInfoV2(TUint32 aIapId, TUint32 aNetId, TConnectionType aConnectionType);
	inline TConnectionType ConnectionType();

protected:
	inline TConnectionInfoV2(TUint8 aVersion, TUint32 aIapId, TUint32 aNetId, TConnectionType aConnectionType);

private:
	TConnectionType iConnectionType;
	};

/**
@publishedPartner
@released since v7.0s
*/
typedef TPckgBuf<TConnectionInfoV2> TConnectionInfoV2Buf;


class TSubConnectionInfo : public TConnArgBase
/**
 * Base class for information about a subconnection
 * @publishedPartner
 * @released since v7.0s
 * @note Classes for information about a specific connection type should derive from this class
 * @note Generic classes for most bearers are defined in nifconninfo.h
 * @note Classes with increasingly specific parameters for a connection type should derive from the more generic class of the same connection type, as clients can assume that any less specific TSubConnectionInfo-derived classes are acceptable for use in GetSubConnectionInfo() calls.
 */
	{
public:
	inline TSubConnectionInfo();

public:
	TSubConnectionUniqueId iSubConnectionUniqueId;
	TConnectionType iConnectionType;
	TTime iTimeStarted;
	};

class TSubConnectionEvent
/**
 * Base class for structures that provide information related to a specific subconnection event
 * @note Classes derived from this must not exceed KMaxSubConnectionEventSize bytes in size
 * @publishedPartner
 * @released since v7.0s
 */
	{
public:
	IMPORT_C TSubConnectionEvent* CloneL() const;
	IMPORT_C TUint Length() const;
	inline TSubConnectionEvent()
		: iSubConnectionUniqueId(0), iEventType((TSubConnectionEventType)0), iLength(0)
			{}
			
protected:
	IMPORT_C TSubConnectionEvent(TSubConnectionEventType aEventType, TUint aLength);
public:
	TSubConnectionUniqueId iSubConnectionUniqueId;
	TSubConnectionEventType iEventType;
protected:
	TUint iLength;
	};

class TSubConnectionOpenedEvent : public TSubConnectionEvent
/**
 * Information about a subconnection that has just been opened
 * @publishedPartner
 * @released since v7.0s
 * @note This class currently not passed to clients, as it is (essentially) empty
 */
	{
public:
	IMPORT_C TSubConnectionOpenedEvent();
	};

class TSubConnectionClosedEvent : public TSubConnectionEvent
/**
 * Information about a subconnection that has just been closed
 * @publishedPartner
 * @released since v7.0s
 * @note The time closed uses universal time as a time zone
 */
	{
public:
	IMPORT_C TSubConnectionClosedEvent();
public:
	TUint iTotalUplinkDataVolume;
	TUint iTotalDownlinkDataVolume;
	TTime iTimeClosed;	// universal time
	};

class TSubConnectionQoSChangedEvent : public TSubConnectionEvent
/**
 * Information about a subconnection on which the QoS settings have just changed. This only signals that QoS has changed. If the client wants to know what has changed should then get TSubConnectionInfo
 * @publishedPartner
 * @released since v7.0s
 * @note This will be produced by NIFs when QoS changes
 */
	{
public:
	IMPORT_C TSubConnectionQoSChangedEvent();
	};



/**
The following classes are used to enumerate clients and sockets associated with a 
particular connection.  RConnection::Control() is used to retrieve this information.
*/

class TConnectionEnumArg : public TConnArgBase
/**
for KCoEnumerateConnectionClients and KCoEnumerateConnectionSockets
@publishedPartner
@released since v7.0s
*/
	{
public:
	inline TConnectionEnumArg();
public:
	TUint iIndex;
	TUint iCount;
	};


class TConnectionClientInfo
/**
@publishedPartner
@released since v7.0s
*/
	{
public:
	inline TConnectionClientInfo();
	inline TConnectionClientInfo(TUint aProcId, TUint aUid, TUint aThreadId);
public:
	TProcessId iProcessId;
	TUid iUid;
	TThreadId iThreadId;
	};

class TConnectionGetClientInfoArg : public TConnArgBase
/**
for KCoGetConnectionClientInfo
@publishedPartner
@released since v7.0s
*/
	{
public:
	inline TConnectionGetClientInfoArg();
public:
	TUint iIndex;
	TConnectionClientInfo iClientInfo;
	};

#include <es_enum_partner.inl>

#endif // __ES_ENUM_PARTNER_H__


