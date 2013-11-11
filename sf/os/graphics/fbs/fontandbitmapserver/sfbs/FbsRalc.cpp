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
// This file holds the class methods for the Rom Address Lookup Cache feature
// of FBServ: CFbsRalCacheEl, CFbsRalCache.
// 
//

#include "FbsRalc.h"
#include <e32svr.h>


// ============================================================================

/**
Destructor of CFbsRalCacheEl objects releasing memory used by attributes.
@internalComponent 
@post		Object state undefined.
*/
CFbsRalCacheEl::~CFbsRalCacheEl()
	{
	User::Free(iFilename);
	iFilename = 0;
	}

/**
Default constructor for CFbsRalCacheEl objects.
@internalComponent 
@post		CFbsRalCacheEl initialised with a null mapping information.
*/
CFbsRalCacheEl::CFbsRalCacheEl() 
: CBase(), iFilename(0), iAddress(0)
	{
	}

/**
Constructor for constructing TFbsRalCacheEl objects with valid 
mapping information.
@internalComponent 
@param		aFilename full name of the file to store
@param		aMemAddr ROM memory address file starts at
@post		TFbsRalCacheEl initialised with the filename and memory address 
supplied.
@panic		EEmptyFilename panic generated when no chars in name
@panic		EInvalidRomAddress panic generated when address out of range
*/
CFbsRalCacheEl* 
CFbsRalCacheEl::New(
	const TDesC& aFilename,
	TAny* aMemAddr
)
	{
	// Allocate memory for new cache element object and construct.
	CFbsRalCacheEl* ptr = new CFbsRalCacheEl;
	if (!ptr)
		return 0;

	// Initialise attributes of cache element.
	ptr->iFilename = HBufC::New(aFilename.Length());
	if (!ptr->iFilename)
		{
		User::Free(ptr);
		return 0;
		}
	*(ptr->iFilename) = aFilename;
	ptr->iAddress = aMemAddr;
#ifdef _RALC_DEBUG
	ptr->iHitCount = 1;
#endif //_RALC_DEBUG

	// Done, return address to caller.
	return ptr;
	}

/**
This routine is used by callers to release the memory directly used for the 
CFbsRalCacheEl object specified WITHOUT destroying the object first. Users
wishing to destroy the object can do so by calling the destructory directly. 
This is necessary since new/delete are not used for objects of this class as
out of memory panics must be avoided.
Therefore this routine must be used with caution to avoid memory leaks! 
@internalComponent 
@param		aThisRef Ptr to object to free. Ptr set to 0 on exit
*/
void
CFbsRalCacheEl::FreeOnly(
	CFbsRalCacheEl*& aThisRef
)
	{
	User::Free (aThisRef);
	aThisRef = 0;
	}

/**
An optimised routine for checking the filename in the entry matches that
supplied to the routine. It compares filenames in a right to left fashion
since most differences in filenames are in the last 16 characters or so.
ASCII characters are compared in a case insensitive fashion.
@internalComponent 
@param		aSearchKey filename to compare entry with
@return		ETrue if entry matches search key, EFalse otherwise.
*/
TBool
CFbsRalCacheEl::MatchKey(
	const TDesC& aSearchKey
)
	{
	if (!iFilename)
		return EFalse;

	// Check both filename descriptors are the same length
	TInt charIndex = iFilename->Length();	
	if (charIndex != aSearchKey.Length())
		return EFalse;
	
	// Then check every character is the same by comparing right to left.
	while (charIndex)
		{
		charIndex--;
		if (ToLower((*iFilename)[charIndex]) != ToLower(aSearchKey[charIndex]))
			return EFalse;
		}

	// If we reach here we know the entry matches what we are looking for.
	return ETrue;
	}

TInt CFbsRalCacheEl::ToLower(TInt aInt)
	{
	return (0x40 < aInt && 0x5B > aInt) ? (aInt + 0x20) : aInt;
	}

// ============================================================================

/**
Destructor of CFbsRalCache objects by resetting the circular buffer.
@internalComponent 
@post		Object state undefined.
*/
CFbsRalCache::~CFbsRalCache()
	{
#ifdef _RALC_DEBUG
	RDebug::Print(_L("FBS RALCache destructed for process:%x\n"),(TUint)iProcId);
	PrintCache();
#endif //_RALC_DEBUG

	while (Count())
		DropItem();

	Reset();
	}

/**
CFbsRalCache private constructor.
@internalComponent 
@param		aFs valid file serve session handle used in lookups.
@post		TFbsRalCache initialised with empty cache buffer.
@see		Public construction achieved using static New() class method.
*/
CFbsRalCache::CFbsRalCache(
	RFs& aFs
) 
: CCirBuf<CFbsRalCacheEl>(), iFs(aFs)
	{
#ifdef _RALC_DEBUG
	RProcess current;
	iProcId = current.Id();
	RDebug::Print(_L("FBS RALCache constructed for process:%x\n"), 
		(TUint)iProcId);
#endif
	}

/**
CFbsRalCache public constructor. This method allocates memory for a new
object on the default heap and initialises it with the supplied data.
@internalComponent 
@param		aCacheSize number (fixed) of entries in the cache. 
@param		aFs valid file serve session handle used in lookups.
@return		Pointer to new object, otherwise 0 when error detected.
@post		TFbsRalCache initialised with empty cache buffer.
*/
CFbsRalCache* 
CFbsRalCache::New(
	TInt aCacheSize,
	RFs& aFs
)
	{
	if (aCacheSize == 0)
		return 0;

	// Allocate memory for new cache object
	// and call its constructor to initialise it.
	CFbsRalCache* ptr = new CFbsRalCache(aFs);
	if (!ptr) 
		return 0;

	// Reserve capacity in circular buffer for cache entries.
	TRAPD(lc, ptr->SetLengthL(aCacheSize));
	if (lc != KErrNone)
		{
		User::Free(ptr);
		return 0;
		}

	// Successful, new cache created.
	return ptr;
	}

/**
This is the main cache lookup method for getting a file's start address
should it be found in ROM. It first scans the cache for a match and on a hit
it returns the stored address. On a miss it uses the file server to get
the file's ROM address. This result if +ve is stored and the Most-recently-used
cache, otherwise 0 is returned.
@internalComponent 
@param		aFileKey Full file name of file to get address for.
@return		Start address in ROM for the file, 0 if not in ROM.
@post		Cache updated with new entry if ROM file and not in cache.
*/
TAny*
CFbsRalCache::Lookup(
	const TDesC& aFileKey
)
	{

	// Search the cache from the head to the tail should it have any entries
	// based on a MRU policy.
	if (Count())
		{
		TUint8* ptr = iHead;
		for (TInt num = Count(); num; num--)
			{
			// Calculate the address of the entry.
			if (ptr <= iPtr)
				ptr = iPtrE;
			ptr-=iSize;
			CFbsRalCacheEl* entry = (CFbsRalCacheEl*)ptr;

			// Compare the entry key with that suppled for a match and return.
			if (entry->MatchKey(aFileKey))
				{
#ifdef _RALC_DEBUG
				RDebug::Print(_L("FBS RALCache lookup HIT: %S\n"), &aFileKey);
				entry->iHitCount++;
#endif //_RALC_DEBUG
				return entry->iAddress;
				}
			}
		}

#ifdef _RALC_DEBUG
	RDebug::Print(_L("FBS RALCache lookup MISS: %S\n"), &aFileKey);
#endif //_RALC_DEBUG

	// Filename not in cache, ask file server for it's ROM address.
	TAny *romAddr = iFs.IsFileInRom (aFileKey);
	if (romAddr)
		{
		// Store new filename/address mapping in cache and return.
		(void)AddItem (aFileKey, romAddr);
		return romAddr;
		}

	// It's not a file in ROM
	return 0;
	}

/**
This method will create a cache entry from the supplied data and add it to the
head of the circular cache buffer. If the cache is full this will result in the
entry at the tail being overwritten with the new entry inserted at the head.
@internalComponent 
@param		aFilename full name of the file to store
@param		aMemAddr ROM memory address file starts at
@return		ETrue when successfully added, EFalse otherwise.
@post		Cache updated as described above.
*/
TBool 
CFbsRalCache::AddItem(
	const TDesC& aFilename, 
	TAny* aMemAddr
)
	{
	// Construct the new cache entry from the supplied data.
	CFbsRalCacheEl* entry = CFbsRalCacheEl::New(aFilename, aMemAddr);
	if (!entry)
		return EFalse;

	// Make room in the cache if we need to based on MRU policy.
	if (Count() == Length()) // Is it full?
		DropItem();

	// Add a copy of the cache entry to the circular buffer.
	if (Add(entry) == 0)
		{
		// Failed, can't cache it!
		entry->~CFbsRalCacheEl();
		CFbsRalCacheEl::FreeOnly(entry);
		return EFalse;
		}

	// Item now cached so clean up local entry memory only
	CFbsRalCacheEl::FreeOnly(entry);
	return ETrue;
	}

/**
This method will create a cache entry from the supplied data and add it to the
head of the circular cache buffer. If tha cache is full this will result in the
entry at the tail being overwritten with the new entry inserted at the head.
@internalComponent 
@post		Cache updated as described above.
*/
void
CFbsRalCache::DropItem()
	{
	// No items to drop from cache!?
	if (Count() == 0)
		return;

	// Remove cache entry at tail copying into temporary variable.
	CFbsRalCacheEl entry;
	Remove(&entry);

#ifdef _RALC_DEBUG
	RDebug::Print(_L("FBS RALCache droped element: %S, %x, %d\n"),
				entry.iFilename, entry.iAddress, entry.iHitCount); 
#endif //_RALC_DEBUG

	// 'entry' destroyed on exit and cleaned up.
	}


#ifdef _RALC_DEBUG
void 
CFbsRalCache::PrintCache()
	{
	if (Count())
        {
        TUint8* ptr = iHead;
		TInt num = Count();
		RDebug::Print(_L("FBS RALCache contents (%d):\n"), num);
        for (; num; num--)
            {
            if (ptr <= iPtr)
                ptr = iPtrE;
            ptr-=iSize;
            CFbsRalCacheEl* entry = (CFbsRalCacheEl*)ptr;
			RDebug::Print(_L("FBS RALCache El %d: %S, %x, %d\n"), num, 
				entry->iFilename, entry->iAddress, entry->iHitCount); 
            }
        }
	else
		{
		RDebug::Print(_L("FBS RALCache emtpy.\n"));
		}
	}
#endif

// ==========================================================================
