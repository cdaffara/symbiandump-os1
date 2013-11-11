// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

inline TConnArgBase::TConnArgBase()
: iVersion(0)
/**
Default Constructor

*/
	{}

inline TConnArgBase::TConnArgBase(TUint8 aVersion)
: iVersion(aVersion)
/**
Constructor

@param aVersion, Version number of the connection arguemt.
*/
	{
	iReserved[0]=0;
	iReserved[1]=0;
	iReserved[2]=0;
	}

inline TUint8 TConnArgBase::Version() const
/**
Responsible for retrieving the version number for the Socket Connection argument.

@return version number of the connection arguemt.
*/
	{
	return iVersion;
	}

inline TConnectionInfo::TConnectionInfo()
: TConnArgBase(KConnArgVersion1), iIapId(0), iNetId(0)
/**
Default Constructor

*/
	{}

inline TConnectionInfo::TConnectionInfo(TUint32 aIapId, TUint32 aNetId)
: TConnArgBase(KConnArgVersion1), iIapId(aIapId), iNetId(aNetId)
/**
Constructor

*/
	{}

inline TConnectionInfo::TConnectionInfo(TUint8 aVersion, TUint32 aIapId, TUint32 aNetId)
: TConnArgBase(aVersion), iIapId(aIapId), iNetId(aNetId)
/**
Constructor

*/
	{}
	
inline bool TConnectionInfo::operator == (const TConnectionInfo& aRhs) const
/*
Equality operator
*/
	{
	return (iIapId == aRhs.iIapId && iNetId == aRhs.iNetId);
	}



