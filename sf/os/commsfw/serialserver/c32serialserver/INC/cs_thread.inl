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
// CC32WorkerThread
// 
//


inline	CC32Dealer* CC32WorkerThread::Dealer() const
	{
	return iDealer;
	}

inline	CC32Player* CC32WorkerThread::Player() const
	{
	return iPlayer;
	}

inline	CommsFW::TWorkerId CC32WorkerThread::WorkerId() const
	{
	return iWorkerId;
	}

inline	CC32Dealer& CC32WorkerThread::DealerByRef() const
	{
	__ASSERT_DEBUG(iDealer,Fault(EBadState,_L8("CC32WorkerThread::DealerByRef iDealer is Null, panicking.")));
	return *iDealer;
	}

inline	TBool CC32WorkerThread::DealerShutdownComplete() const
	{
	return iDealerShutdownComplete;
	}


inline	TBool CC32WorkerThread::PlayerShutdownComplete() const
	{
	return iPlayerShutdownComplete;
	}

#ifdef _DEBUG
inline	RAllocator::TAllocFail CC32WorkerThread::AllocFailType() const
	{
	return iFailType;
	}
inline	TInt CC32WorkerThread::AllocFailRate() const 
	{
	return iFailRate;
	}
#endif


