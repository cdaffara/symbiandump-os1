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
//

inline CCommonPitBoss& CCommonPlayer::PitBoss() const
	{
	return WorkerThread().PitBoss();
	}

inline CommsFW::TWorkerId CCommonPlayer::WorkerId() const
	{
	return WorkerThread().WorkerId();
	}

inline const RSafeMessage& CCommonPlayer::SafeMessage() const
	{
	return *iCurrentMessage;
	}

inline CWorkerSession* CCommonPlayer::Session() const
	{
	return iSession;
	}

inline CCommonPlayer::TSubSessionContainer& CCommonPlayer::SubSessions()
	{
	return iSubSessions;
	}

inline CCommonWorkerThread& CCommonPlayer::WorkerThread() const
	{
	return *iOwnerThread;
	}

inline CommsFW::CCommsTransport& CCommonPlayer::Transport() const
	{
	return *WorkerThread().Transport();
	}


inline TPlayerRole CCommonPlayer::PlayerRole() const
	{
	return iPlayerRole;
	}

inline CCommonWorkerThread& CCommonPitBoss::WorkerThread() const
	{
	return *iOwnerThread;
	}

inline TSubSessionUniqueId CCommonPitBoss::NextSubSessionUniqueId()
	{
	return static_cast<TSubSessionUniqueId>(iNextUniqueId++);
	}

inline TSessionUniqueId CCommonPitBoss::NextSessionUniqueId()
	{
	return static_cast<TSessionUniqueId>(iNextUniqueId++);
	}

