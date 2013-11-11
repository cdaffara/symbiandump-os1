// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#if !defined(__MBufPanic_h__)
#define __MBufPanic_h__

// Ideally, this constant should not be published
// - For 3rd party source compatibility this mbuf size will always be available, refer CMBufManager::CreateL() & ::InitL() notes.
// - DO NOT ASSUME UNDER NO CIRCUMSTANCES THAT ALL MBUFS ARE OF THIS SIZE!.  Instead, inspect the size via RMBuf::Size.
const TInt KMBufSmallSize = 128;

// MBuf Manager Error codes - using ESock range
const TInt KErrNoMBufs = -17200;

const TInt KMBufMaxAllocSize = 0x10000;  // Just to keep things sensible

/**
MBuf Manager Panics
@publishedPartner
@released
*/
enum TMBufPanic
	{
	EMBuf_NoHeap = 0,
	EMBuf_CheckFailSize = 1,
	EMBuf_CheckFailType = 2,
	EMBuf_CheckFailLength = 3,
	EMBuf_VCPPFault = 4,
	EMBuf_AlreadyInit = 5,
	EMBuf_NoManager = 6,
	EMBuf_BadBufferSize = 7,
	EMBuf_AllreadyAlloc = 8,
	EMBuf_AllreadyFree = 9,
	EMBuf_NegativeLength = 10,
	EMBuf_NegativeOffset = 11,
	EMBuf_BadOffset = 12,
	EMBuf_EmptyChain = 13,
	EMBuf_NotAligned = 14,
	EMBuf_CircularRef = 15,
	EMBuf_SillyAlloc = 16,
	EMBuf_PrematureDestruct = 17,
	EMBuf_BadNewInPlace = 18,
	EMBuf_NotEmptyChain = 19,
	EMBuf_IllegalAlign = 20,
	EMBuf_NotAnMBuf = 21,
	EMBuf_CorruptMBuf = 22,
	EMBuf_NoHeader = 23,
	EMBuf_TooBig = 24,
	EMBuf_AlreadyActive = 25,
	EMBuf_TooSmall = 26,
	EMBuf_NoPoolChain = 27,
	EMBuf_PoolManager_NegativeMBufSize = 28,
	EMBuf_PoolManager_NegativePoolSize = 29,
	EMBuf_PoolManager_NegativeMinGrowth = 30,
	EMBuf_PoolManager_NegativeGrowthThreshold = 31,
	EMBuf_InvalidMBufSize = 32,
	EMBuf_NegativeMinMBufSize = 33,
	EMBuf_NegativeMaxMBufSize = 34,
	EMBuf_MinExceedsMaxMBufSize = 35,
	EMBuf_CannotPrependChainedMBuf = 36,
	EMBuf_NoPoolManager = 37,
	EMBuf_InvalidRefCount = 38,
	EMBuf_NotifierThreadOpenFail = 39,
	EMBuf_MinExceedsLargestMBufSize = 40,
	EMBuf_MaxLessThanSmallestMBufSize = 41,
	EMBuf_FreeLockNotOwned = 42
	};

#endif // __MBufPanic_h__
