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
// This file holds the class declarations for the ROM Address Lookup Cache 
// feature of FBServ: CFbsRalCacheEl, CFbsRalCache.
// 
//

#ifndef __FBSRALC_H__
#define __FBSRALC_H__

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

/**
This class holds the mapping between the name of a file held in ROM and
the logical address in the memory map that it can be found at. It is used as 
an array element in the circular buffer in the CFbsRalCache object. 
@internalComponent of Font Bitmap Server client library.
*/
NONSHARABLE_CLASS(CFbsRalCacheEl) : public CBase
	{
public:	
	CFbsRalCacheEl();
	~CFbsRalCacheEl();

	static CFbsRalCacheEl* New(const TDesC& aFilename, TAny* aMemAddr);
	static void FreeOnly(CFbsRalCacheEl*& aThisRef);

	TBool MatchKey(const TDesC& searchKey);
private:
	static TInt ToLower(TInt aInt);
public:
	/** The name of the file held in ROM */
	HBufC*		iFilename;
	/** The logical memory address to that start of the file in ROM */
	TAny*		iAddress;
	/** Counter to track how many lookups are made for this file */
#ifdef _RALC_DEBUG
	TUint iHitCount;
#endif //_RALC_DEBUG
	};


/**
This class extends the fixed size circular buffer class to provide a most
recently used cache of filenames to memory addresses for files that are 
held in ROM on the Z: drive. This facility is used in the client side
FBServ library so that ROM bitmaps are loaded as quickly as possible
without IPC to font bitmap server.
The class has been constructed so that its API does not use the Leave
mechanism and so instead it uses return values.
The caching policy used in its implementation is a Most-Recently-Used policy.
@internalComponent of Font Bitmap Server client library.
*/
NONSHARABLE_CLASS(CFbsRalCache) : public CCirBuf<CFbsRalCacheEl>
	{
public:
	~CFbsRalCache();
	static CFbsRalCache* New(TInt aCacheSize, RFs& aFs);

	TAny* Lookup(const TDesC& aFileKey);

private:
	CFbsRalCache(RFs& aFs);

	TBool AddItem(const TDesC& aFilename, TAny* aMemAddr);
	void DropItem();

#ifdef _RALC_DEBUG
	void PrintCache();
#endif//_RALC_DEBUG

private:
	/** Ref to a File Server session for use by the cache on lookups on 
	a cache miss. */
	RFs& iFs;

#ifdef _RALC_DEBUG
	TProcessId iProcId;
#endif//_RALC_DEBUG
	};


#endif /* __FBSRALC_H__ */
