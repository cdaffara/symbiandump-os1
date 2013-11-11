// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// @internalTechnology
// 
//

inline TRequestWrapper::operator TRequestStatus*()
	{
	return iStatus;
	}
inline void TRequestWrapper::SetPeerThread(const RThread& aPeerThread)
	{
	iPeerThread = &aPeerThread;
	}

inline const RThread& TRequestWrapper::PeerThread () const
	{
	return *iPeerThread;
	}

inline TTransportUserStorage::TTransportUserStorage()
: iStorage(NULL)
	{
	}
inline TAny* TTransportUserStorage::Ptr() const
	{
	return iStorage;
	}
inline TAny*& TTransportUserStorage::RefPtr()
	{
	return iStorage;
	}

inline TTransportUser::TTransportUser()
: iUser(NULL),
  iUserStorage(NULL)
	{
	}

inline TTransportReceiverFactoryArgs::TTransportReceiverFactoryArgs(TTransportUserStorage* aStorage, CommsFW::TWorkerId aWorkerId)
:	iStorage(aStorage),
	iWorkerId(aWorkerId)
	{
	}

