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

#include "DrawableCache.h"
#include <graphics/wsdrawablesourceprovider.h>
#include <graphics/wsdrawresource.h>
#include "graphicsresourcewrapper.h"


const TAny* CDrawableCacheBase::Resolve(const TSgDrawableId& aDrawableId, TInt aScreenNumber) const
	{
	TCacheEntry entry(aDrawableId, aScreenNumber);
	TInt index = iCachedItems.FindInOrder(entry, TLinearOrder<TCacheEntry>(CDrawableCacheBase::Compare));
	if (index != KErrNotFound)
		{
		return iCachedItems[index].iCachedItem;
		}
	return NULL;
	}

/**
Compare two cache entries by drawable ID and screen number.

@return zero, if the two objects are equal, a negative value, if the first entry is less 
than the second, a positive value, if the first entry is greater than the second.
*/
TInt CDrawableCacheBase::Compare(const TCacheEntry &aFirstEntry, const TCacheEntry &aSecondEntry)
	{
	TInt delta = Mem::Compare(reinterpret_cast<const TUint8*>(&aFirstEntry.iDrawableId), sizeof(TSgDrawableId),
	                          reinterpret_cast<const TUint8*>(&aSecondEntry.iDrawableId), sizeof(TSgDrawableId));
	if (delta != 0)
		{
		return delta;
		}
	return aFirstEntry.iScreenNumber - aSecondEntry.iScreenNumber;
	}


CWindowDrawableCache::CWindowDrawableCache(RWsSession& aSession)
	: iWsSession(aSession)
	{}

CWindowDrawableCache::~CWindowDrawableCache()
	{
	delete iGraphicsResource;
	delete iGrwFactory;
	for (TInt ii=0;ii<iCachedItems.Count();++ii)
		{
		RWsDrawableSource* cachedItem=static_cast<RWsDrawableSource*>(iCachedItems[ii].iCachedItem);
		cachedItem->Close();
		delete cachedItem;
		}
	iCachedItems.Close();
	}

struct TCloseDrawableData
	{
public:
	TCloseDrawableData(CGraphicsResourceWrapper& aGraphicsResource, RSgDrawable& aDrawable)
		: iGraphicsResource(aGraphicsResource), iDrawable(aDrawable) {}
	CGraphicsResourceWrapper& iGraphicsResource;
	RSgDrawable& iDrawable;
	};

void CloseDrawable(TAny* aCleanupData)
	{
	TCloseDrawableData* data = static_cast<TCloseDrawableData*>(aCleanupData);
	data->iGraphicsResource.Close(data->iDrawable);
	}

TInt CWindowDrawableCache::UseL(const TSgDrawableId& aDrawableId, TInt aScreenNumber)
	{
	if(!iGraphicsResource)
		{
		if (!iGrwFactory)
			iGrwFactory = new (ELeave) CGraphicsResourceWrapperFactory;
		iGraphicsResource = iGrwFactory->NewGraphicsResourceWrapper();
		if(!iGraphicsResource)
			User::Leave(KErrNotSupported);
		}
	TCacheEntry entry(aDrawableId, aScreenNumber);
	TInt index = iCachedItems.FindInOrder(entry, TLinearOrder<TCacheEntry>(CDrawableCacheBase::Compare));
	if (index != KErrNotFound)
		{
		return KErrNone;
		}
	RSgDrawable* drawable = iGraphicsResource->NewDrawableL();
	CleanupStack::PushL(drawable);
	TInt err = iGraphicsResource->Open(*drawable, aDrawableId);
	if (err != KErrNone)
		{
		if (err == KErrNoMemory)
			{
			User::LeaveNoMemory();
			}
		return err;
		}
	TCloseDrawableData cleanupData(*iGraphicsResource, *drawable);
	CleanupStack::PushL(TCleanupItem(CloseDrawable, &cleanupData));
	RWsDrawableSource* cachedItem = new(ELeave) RWsDrawableSource(iWsSession);
	err = cachedItem->Create(*drawable, aScreenNumber);
	CleanupStack::PopAndDestroy(); //CloseDrawable()
	CleanupStack::PopAndDestroy(drawable);
	if (err != KErrNone)
		{
		delete cachedItem;
		if (err == KErrNoMemory)
			{
			User::LeaveNoMemory();
			}
		return err;
		}
	entry.iCachedItem = cachedItem;
	err = iCachedItems.InsertInOrder(entry, TLinearOrder<TCacheEntry>(CDrawableCacheBase::Compare));
	if (err != KErrNone)
		{
		cachedItem->Close();
		delete cachedItem;
		User::Leave(err);
		}
	return KErrNone;
	}


CRenderStageDrawableCache::CRenderStageDrawableCache(MWsDrawableSourceProvider* aDrawResource)
	: iDrawResource(aDrawResource)
	{
	}

CRenderStageDrawableCache::~CRenderStageDrawableCache()
	{
	for (TInt i = 0; i < iCachedItems.Count(); ++i)
		{
		iDrawResource->CloseDrawableSource(iCachedItems[i].iCachedItem);
		}
	iCachedItems.Close();
	}

TInt CRenderStageDrawableCache::UseL(const TSgDrawableId& aDrawableId, TInt aScreenNumber)
	{
	if (!iDrawResource)
		{
		return KErrGeneral;
		}
	TCacheEntry entry(aDrawableId, aScreenNumber);
	TInt index = iCachedItems.FindInOrder(entry, TLinearOrder<TCacheEntry>(CDrawableCacheBase::Compare));
	if (index != KErrNotFound)
		{
		return KErrNone;
		}
	TInt err = iDrawResource->CreateDrawableSource(aDrawableId, entry.iCachedItem);
	if (err != KErrNone)
		{
		if (err == KErrNoMemory)
			{
			User::LeaveNoMemory();
			}
		return err;
		}
	err = iCachedItems.InsertInOrder(entry, TLinearOrder<TCacheEntry>(CDrawableCacheBase::Compare));
	if (err != KErrNone)
		{
		iDrawResource->CloseDrawableSource(entry.iCachedItem);
		User::Leave(err);
		}
	return KErrNone;
	}
