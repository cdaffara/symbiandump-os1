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

#ifndef __DRAWABLECACHE_H__
#define __DRAWABLECACHE_H__

#include <e32cmn.h>
#include <graphics/sgresource.h>

#include "W32STD.H"

/**
@file
@internalComponent
Provides functionality which is used by RemoteGc.
*/

class RWsDrawableSource;
class MWsDrawableSourceProvider;
class CGraphicsResourceWrapperFactory;
class CGraphicsResourceWrapper;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDrawableCacheBase): public CBase
	{
public:
	virtual TInt UseL(const TSgDrawableId& aDrawableId, TInt aScreenNumber) = 0;
	const TAny* Resolve(const TSgDrawableId& aDrawableId, TInt aScreenNumber) const;
protected:
	class TCacheEntry
		{
	public:
		TCacheEntry(const TSgDrawableId& aDrawableId, TInt aScreenNumber)
			: iDrawableId(aDrawableId), iScreenNumber(aScreenNumber), iCachedItem(NULL)
			{}
	public:
		TSgDrawableId iDrawableId;
		TInt iScreenNumber;
		TAny* iCachedItem;
		};
protected:
	static TInt Compare(const TCacheEntry &aFirstEntry, const TCacheEntry &aSecondEntry);
protected:
	RArray<TCacheEntry> iCachedItems;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CWindowDrawableCache): public CDrawableCacheBase
	{
public:
	CWindowDrawableCache(RWsSession& aSession);
	~CWindowDrawableCache();
	TInt UseL(const TSgDrawableId& aDrawableId, TInt aScreenNumber);
private:
	RWsSession& iWsSession;
	CGraphicsResourceWrapperFactory* iGrwFactory;
	CGraphicsResourceWrapper* iGraphicsResource;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CRenderStageDrawableCache): public CDrawableCacheBase
	{
public:
	CRenderStageDrawableCache(MWsDrawableSourceProvider* aDrawResource);
	~CRenderStageDrawableCache();
	TInt UseL(const TSgDrawableId& aDrawableId, TInt aScreenNumber);
private:
	MWsDrawableSourceProvider* iDrawResource;
	};

#endif //__DRAWABLECACHE_H__
