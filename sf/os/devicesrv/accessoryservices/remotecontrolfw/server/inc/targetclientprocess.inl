// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Server-side representation of a target client, which may have multiple 
// sessions associated with it.
// 

/**
 @file
 @internalComponent
*/


inline TRemConClientId CRemConTargetClientProcess::Id() const
	{
	return iClientId;
	}

inline TClientInfo& CRemConTargetClientProcess::ClientInfo()
	{	
	return iClientInfo;
	}

inline const TClientInfo& CRemConTargetClientProcess::ClientInfo() const
	{	
	return iClientInfo;
	}

inline TBool CRemConTargetClientProcess::IsClient(const TClientInfo& aClientInfo) const
	{
	return (iClientInfo.ProcessId() == aClientInfo.ProcessId() && iClientInfo.SecureId() == aClientInfo.SecureId());
	}

inline TBool CRemConTargetClientProcess::ClientAvailable() const
	{
	return iClientAvailable;
	}

inline TInt CRemConTargetClientProcess::TargetSessionCount() const
	{
	return iTargetSessions.Count();
	}

inline TBool CRemConTargetClientProcess::HasPlayerInformation() const
	{
	return iPlayerInfoSet;
	}

inline TPlayerType CRemConTargetClientProcess::PlayerType() const
	{
	return iPlayerType.iPlayerType;
	}

inline TPlayerSubType CRemConTargetClientProcess::PlayerSubType() const
	{
	return iPlayerType.iPlayerSubType;
	}

inline const TDesC8& CRemConTargetClientProcess::Name() const
	{
	return iPlayerName;
	}

inline TBool CRemConTargetClientProcess::PlayerInformationMatches(const TPlayerTypeInformation& aPlayerType, const TDesC8& aPlayerName) const
	{
	return (aPlayerType.iPlayerType == PlayerType() && aPlayerType.iPlayerSubType == PlayerSubType() && aPlayerName == Name());
	}
