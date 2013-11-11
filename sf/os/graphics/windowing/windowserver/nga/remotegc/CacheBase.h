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
//

#ifndef __CACHEBASE_H__
#define __CACHEBASE_H__

#include <e32base.h>
#include <e32cmn.h>

/**
A cache base-class providing basic functionality for resolving a cached item.
*/
NONSHARABLE_CLASS(CCacheBase): public CBase
	{	
public:	
	void BeginUpdate();	
	void EndUpdate();

protected:
	CCacheBase();
	~CCacheBase();
	
	virtual TInt UseL(TInt aHandle) = 0;
	virtual const CBase* Resolve(TInt aHandle);
	
protected:
	struct TCacheEntry
		{
		TCacheEntry(TInt aHandle)
			{
			iHandle = aHandle;
			}
			
		TInt iHandle;
		TBool iIsUsed;
		CBase* iCachedItem;
		};

protected:		
	RArray<TCacheEntry> iCachedItems;
	};
	
#endif //__CACHEBASE_H__




