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
// Defines the avdtp SEP cache which holds details about the capabilities of remote SEPs
// these details can then be used later by the stack without having to send a GetCaps
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPSEPCACHE_H
#define AVDTPSEPCACHE_H

#include <bluetoothav.h>
#include <es_mbuf.h>

static const TInt KSEPCacheCallbackInterval = 5;	// seconds
static const TInt KSEPCacheRecordDeadAge = 4;		// multiples of CBInterval

class CCapabilityVisitor;

NONSHARABLE_CLASS(CRemoteSEP) : public CBase
	{
friend class CRemoteSEPCache;
public:
	~CRemoteSEP();	// RPointerArray deletion
private:
	static CRemoteSEP* NewL(const TBTDevAddr& aAddr, const TAvdtpSEPInfo& aInfo);
	void SetCapabilities(HBufC8* aCapsAsProtocol);
	TPtrC8 GetCapabilityL(TAvdtpServiceCategory aCat);
	TBool HasCapability(TAvdtpServiceCategory aCat);
	inline TAvdtpServiceCategories GetCapabilities() const;
	void UpdateInfo(const TAvdtpSEPInfo& aInfo);
private:
	CRemoteSEP(const TBTDevAddr& aAddr, const TAvdtpSEPInfo& aInfo);
	void ConstructL();
private:
	TBTDevAddr				iRemoteDev;
	TAvdtpSEPInfo			iInfo;
	RPointerArray<TAvdtpServiceCapability>	iParsedCaps;
	HBufC8*					iCapabilitiesBuf;	// the bit of the packet we got, copied out
	TAvdtpServiceCategories	iKnownCaps;	
	};

NONSHARABLE_CLASS(CRemoteSEPCache) : public CBase
	{
public:
	static CRemoteSEPCache* NewL();
	TInt AddSEP(const TBTDevAddr& aAddr, const struct TAvdtpSEPInfo& aInfo);
	void SetCapabilities(const TBTDevAddr& aAddr, TSEID aSEID, HBufC8* aCapsAsProtocol);
	static TInt PerformMaintenanceCallback(TAny*); // maintains the cache;
	TBool Exists(const TBTDevAddr& aAddr, TSEID aSEID) const;
	TPtrC8 GetCapabilityL(const TBTDevAddr& aAddr, TSEID aSEID, TAvdtpServiceCategory aCat);
	TBool HasCapability(const TBTDevAddr& aAddr, TSEID aSEID, TAvdtpServiceCategory aCat);
	TAvdtpServiceCategories GetCapabilitiesL(const TBTDevAddr& aAddr, TSEID aSEID) const;
	void InvalidateSEPs(const TBTDevAddr& aAddr);
	~CRemoteSEPCache();
private:
	CRemoteSEP* FindSEP(const TBTDevAddr& aAddr, TSEID aSEID) const;
private:
// async callback to age these things
	RPointerArray<CRemoteSEP>	iRemoteSEPs;
	};

inline TAvdtpServiceCategories  CRemoteSEP::GetCapabilities() const
	{
	return iKnownCaps;
	}

#endif //AVDTPSEPCACHE_H
