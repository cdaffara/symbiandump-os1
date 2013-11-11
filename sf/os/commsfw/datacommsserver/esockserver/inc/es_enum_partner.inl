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


inline TConnectionInfoV2::TConnectionInfoV2()
: TConnectionInfo(KConnArgVersion2, 0, 0)
/**
Default Constructor

*/
	{
	}

inline TConnectionInfoV2::TConnectionInfoV2(TUint32 aIapId, TUint32 aNetId, TConnectionType aConnectionType)
: TConnectionInfo(KConnArgVersion2, aIapId, aNetId), iConnectionType(aConnectionType)
/**
Constructor

@param aIapId
@param aNetId
@param aConnectionType
*/
	{
	}

inline TConnectionInfoV2::TConnectionInfoV2(TUint8 aVersion, TUint32 aIapId, TUint32 aNetId, TConnectionType aConnectionType)
: TConnectionInfo(aVersion, aIapId, aNetId), iConnectionType(aConnectionType)
/**
Constructor

@param aVersion
@param aIapId
@param aNetId
@param aConnectionType
*/
	{
	}

inline TConnectionType TConnectionInfoV2::ConnectionType()
/**
Retrieve the type of socket connection.(version 2)

@return type of connection.
*/
	{
	return iConnectionType;
	}

inline TSubConnectionInfo::TSubConnectionInfo()
: TConnArgBase(KConnArgVersion2), iSubConnectionUniqueId(0), iConnectionType(EConnectionGeneric), iTimeStarted(0)
/**
Default Constructor
*/
	{
	}

inline TConnectionClientInfo::TConnectionClientInfo()
: iProcessId(), iUid(TUid::Null()), iThreadId()
	{}

inline  TConnectionClientInfo::TConnectionClientInfo(TUint aProcId, TUint aUid, TUint aThreadId)
: iProcessId(aProcId), iUid(TUid::Uid(aUid)), iThreadId(aThreadId)
	{}


inline TConnectionEnumArg::TConnectionEnumArg()
: TConnArgBase(KConnArgVersion1), iIndex(0), iCount(0)
	{}


