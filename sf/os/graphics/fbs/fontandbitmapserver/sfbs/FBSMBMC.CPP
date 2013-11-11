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
// This file holds the class methods for the CFbTopMBMCache
// 
//

#include "FBSMBMC.H"


//
//Constructor for CFbTopMBMCache objects with both forward and backward cache size
//The forward(backward) cache size determines the number of stream ids after(before)
//the current loaded bitmap ids to be stored in the cache
//e.g Loading a bitmap with id=20 from a 40 bitmaps mbm file will load the cache(10,10)
//with stream ids from 30-39(10 backward) and 40-49(10 forward)
CFbTopStreamIdCache::CFbTopStreamIdCache(TInt aForwardSize, TInt aBackwardSize, TInt aMaxFilestores)
	:CBase(),
	iEntries(aMaxFilestores),
	iForwardCacheSize(aForwardSize),
	iBackwardCacheSize(aBackwardSize),
	iMaxCacheFilestores(aMaxFilestores)
	{
	}

//
//Destructor of CFbTopMBMCache objects
CFbTopStreamIdCache::~CFbTopStreamIdCache()
	{
	FlushCache();
	}

//Function to reset all the internal state of the class
void CFbTopStreamIdCache::FlushCache()
	{
	CloseFileStores();
	iEntries.Reset();
	User::Free(iBuffer);
	iBuffer = NULL;
	}


//Function to get the most recently used filestore stored in the cache
CShiftedFileStore* CFbTopStreamIdCache::MruFileStore()
	{
	return iEntries[0]->FileStore();
	}

//Function to get the stream id for a bitmap id from a mbm file with certain offset.
//It also fills up the cache with the stream ids for different bitmap from the same
//mbm file and the number of stream ids stored depend on the initial setting of the
//cache size. Also associates each resource with a client handle to improve the
//performance of cleaning up.
TStreamId CFbTopStreamIdCache::GetStreamIdL(RFile& aFile,const TDesC& aFilename,TInt32 aId,TUint aFileOffset, TInt aSessionHandle)
	{
	if (aId < 0)
		{
		User::Leave(KErrEof);
		}
	RCacheEntry *entry = FindCacheEntry(aFilename, aFileOffset);
	if (entry == NULL)
		{
		entry = AddCacheEntryL(aFilename, aFileOffset);
		}
	entry->CreateFileStoreL(aFile, aSessionHandle);
	return entry->GetStreamIdL(aId, iForwardCacheSize, iBackwardCacheSize);
	}

/**
The overloaded version of GetStreamIdL which works with file session instead of file handle.
*/
TStreamId CFbTopStreamIdCache::GetStreamIdL(RFs& aFs, const TDesC& aFileName,TInt32 aId,TUint aFileOffset, TInt aSessionHandle)
	{
	if (aId < 0)
		{
		User::Leave(KErrEof);
		}
	RCacheEntry *entry = FindCacheEntry(aFileName, aFileOffset);
	if (entry == NULL)
		{
		entry = AddCacheEntryL(aFileName, aFileOffset);
		}
	entry->CreateFileStoreL(aFs, aFileName, aSessionHandle);
	return entry->GetStreamIdL(aId, iForwardCacheSize, iBackwardCacheSize);
	}

void CFbTopStreamIdCache::CloseFileStores(TInt aSessionHandle)
	{
	TInt n = iEntries.Count();
	for (TInt i = 0; i < n; ++i)
		{
		if (!aSessionHandle || aSessionHandle == iEntries[i]->SessionHandle())
			iEntries[i]->CloseFileStore();
		}
 	}

CFbTopStreamIdCache::RCacheEntry* CFbTopStreamIdCache::AddCacheEntryL(const TDesC& aFilename, TUint aFileOffset)
	{
	RCacheEntry *entry;
	TInt n = iEntries.Count();
	if (n < iMaxCacheFilestores)
		{
		TInt size = RCacheEntry::KBaseSize + (iForwardCacheSize + iBackwardCacheSize) * sizeof(TStreamId);
		if (iBuffer == NULL)
			iBuffer = static_cast<TUint8*>(User::AllocL(iMaxCacheFilestores * size));
		entry = reinterpret_cast<RCacheEntry*>(iBuffer + n * size);
		}
	else
		{
		entry = iEntries[--n];
		entry->CloseFileStore();
		iEntries.Remove(n);
		}
	iEntries.InsertL(entry, 0);
	return new(entry) RCacheEntry(aFilename, aFileOffset);
	}

CFbTopStreamIdCache::RCacheEntry* CFbTopStreamIdCache::FindCacheEntry(const TDesC& aFilename, TUint aFileOffset)
	{
	TInt n = iEntries.Count();
	for (TInt i = 0; i < n; ++i)
		{
		RCacheEntry *entry = iEntries[i];
		if (entry->SameFile(aFilename, aFileOffset))
			{
			if (i > 0)
				{
				iEntries.Remove(i); // Keep least-recently-used order
				iEntries.Insert(entry, 0); // Cannot fail
				}
			return entry;
			}
		}
	return NULL;
	}


const TInt CFbTopStreamIdCache::RCacheEntry::KBaseSize = _FOFF(CFbTopStreamIdCache::RCacheEntry, iStreamIdCache);


CFbTopStreamIdCache::RCacheEntry::RCacheEntry(const TDesC& aFilename, TUint aFileOffset)
:	iFilename(aFilename),
	iFileOffset(aFileOffset),
	iFilestore(NULL),
	iLastId(KErrNotFound),
	iStreamIdCount(0),
	iSessionHandle(0)
	{
	}

TBool CFbTopStreamIdCache::RCacheEntry::SameFile(const TDesC& aFilename, TUint aFileOffset)
	{
	return !aFilename.Compare(iFilename) && aFileOffset == iFileOffset;
	}

TStreamId CFbTopStreamIdCache::RCacheEntry::GetStreamIdL(TInt32 aId, TInt aForwardSize, TInt aBackwardSize)
	{
	if (iLastId >= 0 && aId >= iLastId - aBackwardSize && aId < iLastId + aForwardSize)
		{
		TInt startindex = iLastId < aBackwardSize ? 0 : iLastId - aBackwardSize;
		// ensure aId exists in the MBM file
		// cache size previously set using the number of bitmaps
		if (aId - startindex >= iStreamIdCount)
			User::Leave(KErrEof);
		return iStreamIdCache[aId - startindex];
		}
	//Flush the cache array
	iLastId = KErrNotFound;
	iStreamIdCount = 0;

	//Extracting the header stream parameters numbitmaps and the stream ids
	TStreamId bmpstreamid(iFilestore->Root().Value() + iFileOffset);
	RStoreReadStream readstream;
	readstream.OpenLC(*iFilestore, bmpstreamid);
	TInt numbitmaps = readstream.ReadInt32L();
	if (aId >= numbitmaps)
		User::Leave(KErrEof);
	TInt endcount = aId + aForwardSize;
	//if endcount is greater than the numbitmaps we need to truncate it so we
	//never overread and cause panic.
	if (endcount > numbitmaps)
		endcount = numbitmaps;
	for (TInt count = 0; count < endcount; ++count)
		{
		bmpstreamid.InternalizeL(readstream);
		bmpstreamid=TStreamId(bmpstreamid.Value() + iFileOffset);
		if (count >= aId - aBackwardSize)
			iStreamIdCache[iStreamIdCount++] = bmpstreamid;
		}
	//get the stream id of the requested bitmap id
	TInt startindex = aId < aBackwardSize ? 0 : aId - aBackwardSize;
	bmpstreamid = iStreamIdCache[aId - startindex];
	CleanupStack::PopAndDestroy();
	// iLastId is updated just before returning to ensure that for error cases
	// the cache is not used in subsequent calls to this method.
	iLastId = aId;
	return bmpstreamid;
	}

CShiftedFileStore* CFbTopStreamIdCache::RCacheEntry::FileStore()
	{
	return iFilestore;
	}

TInt CFbTopStreamIdCache::RCacheEntry::SessionHandle() const
	{
	return iSessionHandle;
	}

void CFbTopStreamIdCache::RCacheEntry::CloseFileStore()
	{
	delete iFilestore;
	iFilestore = NULL;
	iSessionHandle = 0;
 	}

/**
It creates a file store if is not there i.e. if iFileStore is NULL.

@param  aFile the mbm or rsc file handle
@param  aSessionHandle the associated client session handle
*/
void CFbTopStreamIdCache::RCacheEntry::CreateFileStoreL(RFile& aFile, TInt aSessionHandle)
	{
	if (iFilestore == NULL)
		{
		iFilestore = CShiftedFileStore::FromL(aFile, iFileOffset);
		iSessionHandle = aSessionHandle;
		}
	}

/**
It creates a file store if is not there i.e. if iFileStore is NULL.

@param  aFs the associated file session handle
@param  aFileName the mbm or rsc file name
@param  aSessionHandle the associated client session handle
*/
void CFbTopStreamIdCache::RCacheEntry::CreateFileStoreL(RFs& aFs, const TDesC& aFileName, TInt aSessionHandle)
	{
	if (iFilestore == NULL)
		{
		RFile file;
		User::LeaveIfError(file.Open(aFs,aFileName,EFileShareReadersOnly));
		iFilestore = CShiftedFileStore::FromL(file, iFileOffset);
		iSessionHandle = aSessionHandle;
		}
	}
