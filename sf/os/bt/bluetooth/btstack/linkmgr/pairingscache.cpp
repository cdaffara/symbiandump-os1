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

#include "pairingscache.h"
#include "linkutil.h"

/*static*/ CBTPairingsCache* CBTPairingsCache::NewL(MBTPairingsCacheObserver& aObserver, RBTRegServ& aRegServ)
	{
	CBTPairingsCache* self = new(ELeave) CBTPairingsCache(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aRegServ);
	CleanupStack::Pop(self);
	return self;
	}

CBTPairingsCache::CBTPairingsCache(MBTPairingsCacheObserver& aObserver)
	: iObserver(aObserver)
	{
	}

void CBTPairingsCache::ConstructL(RBTRegServ& aRegServ)
	{
	iBTPairedBDAddrGetter = CBTPairedBDAddrGetter::NewL(*this, aRegServ);
	DoUpdateCache();
	}

CBTPairingsCache::~CBTPairingsCache()
	{
	delete iBTPairedBDAddrGetter;
	iPairedBDAddrs.Close();
	}

void CBTPairingsCache::UpdateCache()
	{
	// First we must cancel any outstanding request, as this will be called
	// when the registry has been modified an an external (API) user, and
	// so an in progress 'get' may have a stale view.
	iBTPairedBDAddrGetter->Cancel();
	DoUpdateCache();
	}

void CBTPairingsCache::DoUpdateCache()
	{
    RemoveAllPairedDevices(); // Clear out the cache as it may be stale.
	iCacheState = EUpdateOutstanding;
	iBTPairedBDAddrGetter->Retrieve();
	}

CBTPairingsCache::TPairingState CBTPairingsCache::IsPaired(const TBTDevAddr& aBDAddr)
	{
	// We initially assume that there is no pairing to the remote device.
    CBTPairingsCache::TPairingState ret = ENotPaired;
    
    // We determine whether they are in our pairing cache or not.
    TBool inPairingCache = InPairingCache(aBDAddr);

	// Now we do some additional work based on the state of the cache.
	switch(iCacheState)
		{
	case EUpdateOutstanding:
		// If an update has been requested then we make them
		// wait for the answer.
		ret = EDeferred;
		break;
	case EUpdateFailed:
		// If we failed last time we should try again, and tell
		// them to wait for the answer.
		DoUpdateCache();
		ret = EDeferred;
		break;
	case EValid:
		// If the cache is valid then either we are paired or
		// we aren't, the answer to which is based on whether they
		// are in the cache or not.
		break;
	default:
		Panic(EBTPairingsCacheBadState);
		break;
		}

	// If we were already in the cache then that means there must be a pairing.
	if(inPairingCache)
		{
		ret = EPaired;
		}

	// If we are deferring the decision about whether paired or not, then we 
	// need to remember that we should inform the observer when we have 
	// completed our attempt at getting an answer.
	if(ret == EDeferred)
		{
		iDeferredDecision = ETrue;
		}

	return ret;
	}

TBool CBTPairingsCache::InPairingCache(const TBTDevAddr& aBDAddr)
	{
	return (PairedDeviceIndex(aBDAddr) >= 0) ? ETrue : EFalse;
	}

/**
Wrapper for TBTDevAddr's == operation.
This is needed by the TIdentityRelation parameter in RArray's Find method.
*/
/*static*/ TBool CBTPairingsCache::AreBDAddrsEqual(const TBTDevAddr& aA, const TBTDevAddr& aB)
	{
	return aA == aB;
	}

/**
Returns either the index of the remote device address in the
paired device address list if the specified address is
included therein or KErrNotFound if not
*/
TInt CBTPairingsCache::PairedDeviceIndex(const TBTDevAddr& aBDAddr)
	{
	TIdentityRelation<TBTDevAddr> rel(AreBDAddrsEqual);
	return iPairedBDAddrs.Find(aBDAddr, rel);
	}

TInt CBTPairingsCache::AddPairedDevice(const TBTDevAddr& aAddr)
	{
    __ASSERT_DEBUG(aAddr!=0, Panic(EBTConnectionBadDeviceAddress));
	
	TInt err = KErrNone;
	if(!InPairingCache(aAddr))
		{
		err = iPairedBDAddrs.Append(aAddr);
		}
	return err;
	}

void CBTPairingsCache::RemoveAllPairedDevices()
	{
	for(TInt i=iPairedBDAddrs.Count()-1; i>=0; i--)
		{
		iPairedBDAddrs.Remove(i);
		}
	}

void CBTPairingsCache::InformObserver(TInt aError)
	{
    // Ensure that we inform the observer only if they have been deferred.
	if(iDeferredDecision)
		{
		iDeferredDecision = EFalse;
		iObserver.MbpcoDeferredDecisionResolved(aError);
		}
	}

/*virtual*/ void CBTPairingsCache::MbpdnAddPairedDevices(const RBTDeviceArray& aAddrs)
	{
	__ASSERT_DEBUG(iCacheState == EUpdateOutstanding, Panic(EBTPairingsCacheBadState));
	
	// check that the cache is still empty
	__ASSERT_DEBUG(iPairedBDAddrs.Count() == 0, Panic(EBTPairingsCacheNotEmpty));
	
	TInt err = KErrNone;

	TInt count = aAddrs.Count();
	for (TInt i=0; i<count; i++)
		{
		err = AddPairedDevice(aAddrs[i]->BDAddr());
		if(err != KErrNone)
			{
			break;
			}
		}

	// We handle add failures ourselves.
	iCacheState = (err == KErrNone) ? EValid : EUpdateFailed;

	// We inform the observer, even if there was an error because it is
	// possible that we managed to get the address they were after.
	InformObserver(err);
	}

/*virtual*/ void CBTPairingsCache::MbpdnErrorInGettingPairedDevices(TInt aError)
	{
	__ASSERT_DEBUG(iCacheState == EUpdateOutstanding, Panic(EBTPairingsCacheBadState));
	// Current cache should be ok - we should have cleared it before a getter is
	// started.  This simply sets the flag to indicate that the current (empty) cache 
	// is either complete if aErr is KErrNotFound (or KErrNone) or incomplete
	// if aErr is anything else. In the latter case if the cache is needed in future 
	// another getter should be kicked off.
	TInt err = (aError != KErrNone && aError != KErrNotFound) ? aError : KErrNone;
	if(err != KErrNone)
		{
		iCacheState = EUpdateFailed;
		}
	else
		{
		iCacheState = EValid;
		}
	
	// Ensure that if we have been queried about the pairings and deferred the
	// client, they must be informed.
	InformObserver(err);
	}

