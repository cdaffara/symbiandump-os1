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
// This file holds the class declarations for the CFbTopMBMCache 
// The cache is used to store the stream ids of bitmaps in a MBM file
// 
//

#ifndef __FBSMBMC_H__
#define __FBSMBMC_H__

#include <e32std.h>
#include <e32base.h>
#include <s32std.h>
#include "ShiftedFileStore.h"


//This class holds the TStreamIds of the bitmaps stored in an MBM file
//and the filestore of the last loaded bitmap file. The idea of the cache
//is to speed up loading of bitmaps from a mbm file by skipping the file
//opening process and mapping the MBM id directly to the Stream ids stored
//in the cache
//
NONSHARABLE_CLASS(CFbTopStreamIdCache): public CBase
	{
public:
	CFbTopStreamIdCache(TInt aForwardSize, TInt aBackwardSize, TInt aMaxFilestores);
	~CFbTopStreamIdCache();
	TStreamId GetStreamIdL(RFile& aFile,const TDesC& aFilename,TInt32 aId,TUint aFileOffset, TInt aSessionHandle);
	TStreamId GetStreamIdL(RFs& aFs,const TDesC& aFilename,TInt32 aId,TUint aFileOffset, TInt aSessionHandle);
	CShiftedFileStore* MruFileStore();
	void FlushCache();
	void CloseFileStores(TInt aSessionHandle=0);	// If aSessionHandle is not passed, all file stores will be closed

private:
	class RCacheEntry;
	RCacheEntry* AddCacheEntryL(const TDesC& aFilename, TUint aFileOffset);
	RCacheEntry* FindCacheEntry(const TDesC& aFilename, TUint aFileOffset);

private:
	RPointerArray<RCacheEntry> iEntries;	// Array of pointers to the cache entries in least-recently-used order
	TUint8* iBuffer;						// Buffer for storage of the cache entries
	const TInt iForwardCacheSize;			// Max number of cached bitmap ids after the last requested bitmap id
	const TInt iBackwardCacheSize;			// Max number of cached bitmap ids before the last requested bitmap id
	const TInt iMaxCacheFilestores;			// Max number of filestores in the cache

	friend class CTStreamIdCache;			// This is used for testing purposes
	};


class CFbTopStreamIdCache::RCacheEntry
	{
public:
	RCacheEntry(const TDesC& aFilename, TUint aFileOffset);
	TBool SameFile(const TDesC& aFilename, TUint aFileOffset);
	TStreamId GetStreamIdL(TInt32 aId, TInt aForwardSize, TInt aBackwardSize);
	CShiftedFileStore* FileStore();
	void CloseFileStore();
	TInt SessionHandle() const;
	void CreateFileStoreL(RFile& aFile, TInt aSessionHandle);
	void CreateFileStoreL(RFs& aFs, const TDesC& aFilename, TInt aSessionHandle);
public:
	static const TInt KBaseSize;		// Size of the class excluding array iStreamIdCache

private:
	TBufC<KMaxFileName>	iFilename;		// Name of the mbm file
	TUint iFileOffset;					// Offset into the mbm file
	CShiftedFileStore *iFilestore;		// File store for the mbm file
	TInt iLastId;						// Last requested bitmap id from the mbm file
	TInt iStreamIdCount;				// Number of stream ids from the mbm file
	TInt iSessionHandle;				// Handle to the associated client session

	// Keep always iStreamIdCache as the last data member
	// Memory for it is allocated in CFbTopStreamIdCache::AddCacheEntryL()
	TStreamId iStreamIdCache[1];		// Array of stream ids from the mbm file

	friend class CTStreamIdCache;		// This is used for testing purposes
	};
#endif
