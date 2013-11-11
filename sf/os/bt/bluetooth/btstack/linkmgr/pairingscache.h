// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PAIRINGSCACHE_H
#define PAIRINGSCACHE_H

#include <e32base.h>
#include "RegistryHelpers.h"

class CBTPairedBDAddrGetter;

class MBTPairingsCacheObserver
	{
public:
	virtual void MbpcoDeferredDecisionResolved(TInt aError) = 0;
	};

NONSHARABLE_CLASS(CBTPairingsCache) : public CBase, public MBTPairedDeviceNotifier
	{
public:
	enum TPairingState
		{
		ENotPaired,
		EPaired,
		EDeferred, // Issued when we are waiting for a registry update.
		};
public:
	static CBTPairingsCache* NewL(MBTPairingsCacheObserver& aObserver, RBTRegServ& aRegServ);
	~CBTPairingsCache();

   	void UpdateCache();
	TPairingState IsPaired(const TBTDevAddr& aBDAddr);

private:
	CBTPairingsCache(MBTPairingsCacheObserver& aObserver);
	void ConstructL(RBTRegServ& aRegServ);

	static TBool AreBDAddrsEqual(const TBTDevAddr& aA, const TBTDevAddr& aB);
    TInt PairedDeviceIndex(const TBTDevAddr& aBDAddr);
	TBool InPairingCache(const TBTDevAddr& aBDAddr);
	void DoUpdateCache();
	void RemoveAllPairedDevices();
	void InformObserver(TInt aError);
	TInt AddPairedDevice(const TBTDevAddr& aAddr);

private: // MBTPairedDeviceNotifier
	virtual void MbpdnAddPairedDevices(const RBTDeviceArray& aAddrs);
	virtual void MbpdnErrorInGettingPairedDevices(TInt aError);

private:
	enum TCacheState
		{
		EValid,
		EUpdateOutstanding,
		EUpdateFailed,
		};

private:
	MBTPairingsCacheObserver& iObserver;
	CBTPairedBDAddrGetter* iBTPairedBDAddrGetter;
	RArray<TBTDevAddr> iPairedBDAddrs;
	TCacheState iCacheState;
	TBool iDeferredDecision;
	};


#endif // PAIRINGSCACHE_H
