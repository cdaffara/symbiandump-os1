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
//

inline CSockSessionProxy* CPlayer::CurrentSessionProxyL()
	{
	return static_cast<CSockSessionProxy*>(Den::CCommonPlayer::CurrentSessionProxyL());
	}

inline CSockManData* CPlayer::SockManGlobals() const
	{
	return WorkerThread().SockManGlobals();
	}

inline TBool CPlayer::HasDataPlane() const
	{
	return PlayerRole().HasRole(TCFPlayerRole::EDataPlane);
	}

inline TBool CPlayer::HasSubConnPlane() const
	{
	return PlayerRole().HasRole(TCFPlayerRole::ESubConnPlane);
	}

inline TBool CPlayer::HasConnPlane() const
	{
	return PlayerRole().HasRole(TCFPlayerRole::EConnPlane);
	}

inline TBool CPlayer::HasMetaConnPlane() const
	{
	return PlayerRole().HasRole(TCFPlayerRole::EMetaConnPlane);
	}

inline TBool CPlayer::HasTierMgrPlane() const
	{
	return PlayerRole().HasRole(TCFPlayerRole::ETierMgrPlane);
	}

inline TBool CPlayer::HasTierResolver() const
	{
	return PlayerRole().HasRole(TCFPlayerRole::ETierResolver);
	}

inline TBool CPitBoss::GetWorkerForPlayerRole(TUint aRoleId, CommsFW::TWorkerId& aWorker) const
	{
	return GetWorkerForProtocol(KPlayerRoleProxyAddrFam, KPlayerRoleProxySockType, aRoleId, aWorker);
	}

inline void CPitBoss::AddPlayerRolePairingL(TUint aRoleId, const TDesC& aRoleDesc, CommsFW::TWorkerId aWorker, TProtocolPairingOwner& aPairList)
	{
	AddProtocolToListL(KPlayerRoleProxyAddrFam, KPlayerRoleProxySockType, aRoleId, aRoleDesc, aWorker, aPairList);
	}

inline TBool CPitBoss::TierMappingsLoaded() const
	{
	return iLoadTierMappingPhase == EComplete;
	}



