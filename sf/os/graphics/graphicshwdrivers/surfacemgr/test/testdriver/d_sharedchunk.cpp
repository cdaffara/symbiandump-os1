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

#include <kernel/kern_priv.h>
#include "d_sharedchunk.h"

static TInt ChunkDestroyedCount=1;	// Test counter

//
// Class definitions
//

class DSharedChunkFactory : public DLogicalDevice
	{
public:
	~DSharedChunkFactory();
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	void LockWait();
	void LockSignal();
private:
	NFastMutex iLock;
	};

class DSharedChunkChannel : public DLogicalChannelBase
	{
public:
	DSharedChunkChannel();
	~DSharedChunkChannel();
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	virtual TInt Request(TInt aFunction, TAny* a1, TAny* a2);
	DChunk* OpenChunk(TLinAddr* aKernelAddr=0, TInt* aMaxSize=0);
	inline void LockWait()
		{ iFactory->LockWait(); }
	inline void LockSignal()
		{ iFactory->LockSignal(); }
public:
	DSharedChunkFactory*	iFactory;
	DChunk*					iChunk;
	TLinAddr				iKernelAddress;
	TInt					iMaxSize;

public:
	/** Require physically contiguous memory */
	TInt iContiguous;
	/** Caching attribute to create chunks memory */
	TInt iCacheAttrib;
	};

class TChunkCleanup : public TDfc
	{
public:
	TChunkCleanup(DSharedChunkFactory* aFactory);
	~TChunkCleanup();
	static void ChunkDestroyed(TChunkCleanup* aSelf);
	void Cancel();
public:
	DSharedChunkFactory* iFactory;
	};

//
// TChunkCleanup
//

TChunkCleanup::TChunkCleanup(DSharedChunkFactory* aFactory)
	: TDfc((TDfcFn)TChunkCleanup::ChunkDestroyed,this,Kern::SvMsgQue(),0)
	, iFactory(0)
	{
	aFactory->Open();
	iFactory = aFactory;
	}

TChunkCleanup::~TChunkCleanup()
	{
	if(iFactory)
		iFactory->Close(0);
	}

void TChunkCleanup::ChunkDestroyed(TChunkCleanup* aSelf)
	{
	DSharedChunkFactory* factory = aSelf->iFactory;
	if(factory)
		{
		factory->LockWait();
		++ChunkDestroyedCount;
		factory->LockSignal();
		}
	delete aSelf;
	}

void TChunkCleanup::Cancel()
	{
	if(iFactory)
		{
		iFactory->Close(0);
		iFactory = 0;
		}
	};

//
// DSharedChunkFactory
//

TInt DSharedChunkFactory::Install()
	{
	return SetName(&KSharedChunkLddName);
	}

DSharedChunkFactory::~DSharedChunkFactory()
	{

	}

void DSharedChunkFactory::GetCaps(TDes8& /*aDes*/) const
	{
	// Not used but required as DLogicalDevice::GetCaps is pure virtual
	}

TInt DSharedChunkFactory::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel = NULL;
	DSharedChunkChannel* channel=new DSharedChunkChannel;
	if(!channel)
		return KErrNoMemory;
	channel->iFactory = this;
	aChannel = channel;
	return KErrNone;
	}

void DSharedChunkFactory::LockWait()
	{
	NKern::FMWait(&iLock);
	}

void DSharedChunkFactory::LockSignal()
	{
	NKern::FMSignal(&iLock);
	}

DECLARE_STANDARD_LDD()
	{
	return new DSharedChunkFactory;
	}

//
// DSharedChunkChannel
//

TInt DSharedChunkChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& /*aVer*/)
	{
	return KErrNone;
	}

DSharedChunkChannel::DSharedChunkChannel()
:iFactory(0),iChunk(0),iKernelAddress(0),iMaxSize(0), iContiguous(0),iCacheAttrib(0)
	{
	}

DSharedChunkChannel::~DSharedChunkChannel()
	{
	if(iChunk)
		iChunk->Close(0);
	}

DChunk* DSharedChunkChannel::OpenChunk(TLinAddr* aKernelAddr,TInt* aMaxSize)
	{
	__ASSERT_CRITICAL	// Thread must be in critical section (to avoid leaking access count on chunk)
	LockWait();
	DChunk* chunk=iChunk;
	if(chunk)
		if(chunk->Open()!=KErrNone)
			chunk = NULL;
	if(aKernelAddr)
		*aKernelAddr = chunk ? iKernelAddress : NULL;
	if(aMaxSize)
		*aMaxSize = chunk ? iMaxSize : 0;
	LockSignal();
	return chunk;
	}

TInt DSharedChunkChannel::Request(TInt aFunction, TAny* a1, TAny* a2)
	{
	TInt i1 = (TInt)a1;
	TInt i2 = (TInt)a2;

	TInt r=KErrNotSupported;

	switch(aFunction)
		{

	case RSharedChunkLdd::ECreateChunk:
		{
		if(ChunkDestroyedCount==0)
			NKern::Sleep(NKern::TimerTicks(100)); // Go idle for a while to let chunk cleanup DFCs to be called

		NKern::ThreadEnterCS();

		TChunkCleanup* cleanup = new TChunkCleanup(this->iFactory);
		if(!cleanup)
			{
			NKern::ThreadLeaveCS();
			return KErrNoMemory;
			}

		// Try and create chunk...
		DChunk* chunk;
		TChunkCreateInfo info;

		info.iType		 = (i1&EMultiple)
							? TChunkCreateInfo::ESharedKernelMultiple
							: TChunkCreateInfo::ESharedKernelSingle;

		info.iMaxSize	 = i1&~ECreateFlagsMask;
#ifndef __WINS__
		info.iMapAttr = (i1&ECached) ? EMapAttrCachedMax
						 : EMapAttrFullyBlocking;
#else
		info.iMapAttr = 0;
#endif

		info.iOwnsMemory = (i1&EOwnsMemory)!=0;

		info.iDestroyedDfc = cleanup;

		if(i1&EBadType) *(TUint8*)&info.iType = 0xff;

		TUint32 mapAttr;
		TUint32 kernAddr;
		r = Kern::ChunkCreate(info, chunk, kernAddr, mapAttr);
		if(r!=KErrNone)
			{
			Kern::Printf("Chunk create failed with reason: %d",r);
			delete cleanup;
			NKern::ThreadLeaveCS();
			return r;
			}

		// Setup data members
		LockWait();
		
		if(iChunk)
			r = KErrAlreadyExists;
		else
			{
			iChunk = chunk;
			iKernelAddress = kernAddr;
			iMaxSize = info.iMaxSize;
#ifndef __WINS__
			TUint32 level1Info = mapAttr & EMapAttrL1CacheMask;
			TUint32 level2Info = mapAttr & EMapAttrL2CacheMask;
			TBool chunkIsNotcached =  ((level2Info == EMapAttrL2Uncached) &&
			    ((level1Info == EMapAttrFullyBlocking) || (level1Info == EMapAttrBufferedNC) ||
			     (level1Info == EMapAttrBufferedC) || (level1Info == EMapAttrL1Uncached)));
			iCacheAttrib = (chunkIsNotcached) ? ENotCached : ECached;

#else
			iCacheAttrib = 0;
#endif
			ChunkDestroyedCount = 0;
			}
		LockSignal();

		if(r!=KErrNone)
			{
			// There was an error, so discard created chunk
			cleanup->Cancel();
			Kern::ChunkClose(chunk);
			NKern::ThreadLeaveCS();
			return r;
			}

		NKern::ThreadLeaveCS();

		// Write back kernel address of chunk
		if(a2)
			kumemput32(a2,(TAny*)&kernAddr,4);

		return KErrNone;
		}


	case RSharedChunkLdd::EGetChunkHandle:
		{
		NKern::ThreadEnterCS();
		DChunk* chunk=OpenChunk();
		if(chunk)
			{
			Kern::Printf("We can open a chunk here.");
			r = Kern::MakeHandleAndOpen(0,chunk);
			Kern::Printf("The chunk handle we get is: %d", r);
			chunk->Close(0);
			}
		else
			r = KErrNotFound;
		NKern::ThreadLeaveCS();
		return r;
		}


	case RSharedChunkLdd::ECloseChunkHandle:
		{
		NKern::ThreadEnterCS();
		r = Kern::CloseHandle(0,i1);
		NKern::ThreadLeaveCS();
		return r;
		}


	case RSharedChunkLdd::ECommitMemory:
		{
		NKern::ThreadEnterCS();
		TUint32 chunkKernelAddress;
		DChunk* chunk=OpenChunk(&chunkKernelAddress);
		if(chunk)
			{
			TInt type = i1&ECommitTypeMask;
			i1 &= ~ECommitTypeMask;
			iContiguous = type;
			switch(type)
				{
			case ENonContiguous:
				r = Kern::ChunkCommit(chunk,i1,i2);
				break;

			case EContiguous:
				{
				TUint32 physAddr=~0u;
				r = Kern::ChunkCommitContiguous(chunk,i1,i2,physAddr);
				if(r!=KErrNone || i2==0)
					break;
				if(physAddr==~0u)
					{ r=KErrGeneral; break; }

				// Check that ChunkPhysicalAddress returns addresses consistant with the commit
				TUint32 kernAddr;
				TUint32 mapAttr;
				TUint32 physAddr2;
				r = Kern::ChunkPhysicalAddress(chunk, i1, i2, kernAddr, mapAttr, physAddr2);
				if(r==KErrNone)
					if(kernAddr!=chunkKernelAddress+i1 || physAddr2!=physAddr)
						r=KErrGeneral;
				}
				break;

			default:
				r = KErrNotSupported;
				break;
				}
			chunk->Close(0);
			}
		else
			r = KErrNotFound;
		NKern::ThreadLeaveCS();
		return r;
		}

	case RSharedChunkLdd::ECloseChunk:
		{
		NKern::ThreadEnterCS();

		// Claim ownership of the chunk
		LockWait();
		DChunk* chunk=iChunk;
		iChunk = 0;
		LockSignal();

		// Close the chunk
		if(chunk)
			r = Kern::ChunkClose(chunk);
		else
			r = KErrNotFound;

		NKern::ThreadLeaveCS();
		return r;
		}

	case RSharedChunkLdd::ECacheAttribute:
		{
		Kern::Printf("In my shared chunk, the iCacheAttrib is: %d", iCacheAttrib);
		return iCacheAttrib;
		}

	case RSharedChunkLdd::EContiguousAttribute:
		{
		Kern::Printf("In my shared chunk, the iContiguous is: %d", iContiguous);
		return iContiguous;
		}

	default:
		return KErrNotSupported;
		}
	}

