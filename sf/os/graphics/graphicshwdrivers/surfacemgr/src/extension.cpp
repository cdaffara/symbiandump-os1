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


#include <kernel/kern_priv.h>
#include <graphics/surface.h>
#include <graphics/surfacetypes.h>
#include <graphics/surfacemanager.h>
#include "surfacemanager_dev.h"
#include <kernel/cache.h>

/**
Convert the surface Id to an index of the array 
based on the least significant 4 bits of the first word of the ID
@param	aSurfaceId  Const reference to the surface Id
@internalTechnology
*/
static TInt SurfaceIdToIndex(const TSurfaceId& aSurfaceId)
	{
	return static_cast<TInt>(aSurfaceId.iInternal[0]&(KMaxLists-1));
	}

/**
Removes an item from a linked list
@param	aList  Pointer to the head of a linked list of type T
@param	aOwner  Pointer to the object to be removed
@internalTechnology
*/
template<class T>
static void UnlinkListItem(T** aList, const T* aItem)
	{
	TRACE(Kern::Printf("SM UnlinkListItem list %08x  object %08x \n", aList, aItem);)
	
	__ASSERT_DEBUG(aItem != NULL, Kern::Fault("Surface Manager", __LINE__));
	__ASSERT_DEBUG(*aList != NULL, Kern::Fault("Surface Manager", __LINE__));

	if (*aList == aItem)	//one we want is at the head of the list
		{
		*aList = aItem->iNext;
		return;
		}
	
	T* p = *aList;
	T* q = (*aList)->iNext;
	while (q)
		{
		if (q == aItem)
			{
			p->iNext = q->iNext;
			return;
			}
		p = q;
		q = q->iNext;
		}
	}




/**
Returns a pointer to the surface owner object for the specified process, for this surface.
@param	aProcess  Pointer to the process object
@return pointer to the surface owner object if found, else NULL
@internalTechnology
*/

TProcessListItem* TSurface::ProcessOwnerInfo(const DProcess* aProcess)
	{
	TProcessListItem* so = iOwners;
	while(so)
		{
		if (aProcess == so->iOwningProcess)
			{
			break;
			}
		so = so->iNext;
		}
	return so;
	}

/**
Creates a shared chunk surface.
@param aParams  Package buffer containing the surface creation parameters.  
@param aId  Will be set to the surface id of the newly created surface.
@return KErrNone if successful, KErrArgument if the creation attributes were incorrect,
otherwise one of the other system wide error codes.
@see RSurfaceManager::TSurfaceCreationAttributes
@internalTechnology
*/

TInt DSurfaceManager::CreateSurface(const TDesC8* aParams, TSurfaceId* aId)
	{

	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attribs = buf();
	
	Kern::KUDesGet(buf, *aParams);		//fetch input parameters
	if( (attribs.iHintCount > KMaxHintsPerSurface) || (attribs.iHintCount<0) )
		{
		return KErrArgument;
		}	
	
	RSurfaceManager::THintPair tempSurfaceHints[KMaxHintsPerSurface];
	if( (attribs.iHintCount>0) && attribs.iSurfaceHints)
		{
		kumemget(tempSurfaceHints, attribs.iSurfaceHints, attribs.iHintCount*sizeof(RSurfaceManager::THintPair));		
		attribs.iSurfaceHints = tempSurfaceHints;
		}
	else
		{
		attribs.iSurfaceHints=NULL;
		}

	//validate input parameters and calculate chunk size
	TInt roundedBufferSize = attribs.iOffsetBetweenBuffers;
	TUint dummyActualSize = 0;	
	
	TInt chunkSize = ValidateAndCalculateChunkSize(attribs, roundedBufferSize, dummyActualSize, ETrue);
	if (chunkSize == 0)
		{
		return KErrArgument;
		}	

	TSurfaceId sid;
	TInt r = KErrNone;
	TSurface* surface = NULL;
	do		//in the unlikely event that we generate a duplicate surface id, try again.
		{
		GenerateSurfaceId(sid);

		NKern::FMWait(&iMutex);
		surface = FindSurfaceById(sid);
		NKern::FMSignal(&iMutex);
		}
	while (surface);
	
	//create a shared chunk for the surface memory
	TChunkCreateInfo info;
	info.iType = TChunkCreateInfo::ESharedKernelMultiple;	//multi process mappable
	info.iMaxSize = chunkSize;
	info.iOwnsMemory = ETrue;

//iMapAttr is valid only for hardware devices and will not make any effect in wins	
#ifndef __WINS__
	info.iMapAttr = (attribs.iCacheAttrib == RSurfaceManager::ECached) ? EMapAttrCachedMax : EMapAttrL1Uncached;
#else
	info.iMapAttr = 0;
#endif

	TLinAddr kernAddr;
	TUint32 mapAttr;	
	
	NKern::ThreadEnterCS();
	DChunk* chunk;
	r = Kern::ChunkCreate(info, chunk, kernAddr, mapAttr);
	if (KErrNone != r)
		{
		NKern::ThreadLeaveCS();
		return r;
		}

	//commit the memory
	TUint32 paddr;
	if (attribs.iContiguous)
		{
		r = Kern::ChunkCommitContiguous(chunk, 0, chunkSize, paddr);
		}
	else
		{
		r = Kern::ChunkCommit(chunk, 0, chunkSize);
		}

	if (KErrNone != r)
		{
		//problem committing the memory,
		//destroy the chunk and cleanup
		Kern::ChunkClose(chunk);
		NKern::ThreadLeaveCS();
		return r;
		}

	//create a surface structure for the new surface
	surface = new TSurface;
	TRACE(Kern::Printf("SM A %08x TSurface CreateSurface",surface);)
	if (!surface)
		{
		//destroy the chunk and cleanup, out of memory
		Kern::ChunkClose(chunk);
		NKern::ThreadLeaveCS();
		return KErrNoMemory;
		}

	surface->iId = sid;
	surface->iSize = attribs.iSize;
	surface->iBuffers = attribs.iBuffers;
	surface->iPixelFormat = attribs.iPixelFormat;
	surface->iStride = attribs.iStride;
	surface->iOffsetToFirstBuffer = attribs.iOffsetToFirstBuffer;
	surface->iAlignment = attribs.iAlignment;
	surface->iContiguous = attribs.iContiguous;
	surface->iChunk = chunk;
	surface->iOffsetBetweenBuffers = roundedBufferSize;
	surface->iCacheAttrib = attribs.iCacheAttrib;
	surface->iMappable = attribs.iMappable;
	if(attribs.iHintCount>0)
		{
		memcpy(surface->iSurfaceHints,tempSurfaceHints,attribs.iHintCount*sizeof(RSurfaceManager::THintPair));
		}
	memclr(surface->iSurfaceHints+attribs.iHintCount, (KMaxHintsPerSurface-attribs.iHintCount)*sizeof(RSurfaceManager::THintPair));
	
	//create a surface owner for this surface
	TProcessListItem* owner = new TProcessListItem;
	TRACE(Kern::Printf("SM A %08x TProcessListItem CreateSurface",owner);)

	if (!owner)
		{
		//destroy the chunk and cleanup, out of memory
		Kern::ChunkClose(chunk);
		delete(surface);
		TRACE(Kern::Printf("SM D %08x TSurface CreateSurface",surface);)
		NKern::ThreadLeaveCS();
		return KErrNoMemory;
		}
	
	owner->iCount = 1;		//mark it as open in this process
	owner->iOwningProcess =  &Kern::CurrentProcess();
	owner->iNext = NULL;
		
	surface->iOwners = owner;	//only 1 owner at creation time

	//at this point we have a fully constructed TSurface

	//add surface to head of surfaces list
	NKern::FMWait(&iMutex);
	//Mask off the bottom log2(KMaxLists) bits of the first word of the surfaceID as an index
	//add the new surface to the beginning of the list 
	TInt index = SurfaceIdToIndex(sid);
	surface->iNext = iSurfacesIndex[index];
	iSurfacesIndex[index] = surface;

	NKern::FMSignal(&iMutex);		
	NKern::ThreadLeaveCS();
	
	//write surface id back to user side
	kumemput(aId, &sid, sizeof (TSurfaceId));
	return KErrNone;
	}


/**
Validate that a chunk contains physical memory for the used areas.

This function should be called in Critical Section in order to be completed even if the thread 
or process is killed and so be able to free the memory allocated  (TUint32[pageList])

@param aChunk  Chunk that the user supplied.   
@param aAttribs  Surface Creation Attributes.
@param aBuffersize  Calculated size of each buffer.
@param aMapAttr  Filled in with the mapping attributes of the memory.
@param aIsContiguous  Lets the caller know if the surface is physically contiguous or not. 
@return KErrNone if successful, KErrArgument if the creation attributes were incorrect,
KErrBadHandle if aChunkHandle is of an invalid shared chunk memory,
otherwise one of the other system wide error codes.
@see RSurfaceManager::TSurfaceCreationAttributes
@internalTechnology
*/
TInt DSurfaceManager::ValidatePhysicalMemory(DChunk* aChunk, const RSurfaceManager::TSurfaceCreationAttributes& aAttribs, 
								   TUint aBuffersize, TUint32& aMapAttr, TBool &aIsContiguous) 
	{
	TLinAddr kernAddr;
	TUint32 physAddr;

	//Get the physical address for a region in a shared chunk
	TInt pageSize = Kern::RoundToPageSize(1);
	TInt pageList = 1 + (aChunk->iSize + pageSize - 2) / pageSize;
	TUint32* physAddr2 = new TUint32[pageList];
	if(!physAddr2)
		{
		return KErrNoMemory;
		}
	
	// Unless proven otherwise, the memory is not contiguous. 
	aIsContiguous = EFalse;
	TInt r = Kern::ChunkPhysicalAddress(aChunk, 0, aChunk->iSize, kernAddr, aMapAttr, physAddr, physAddr2);
	if (KErrNone == r)
		{
		aIsContiguous = ETrue;
		}
	
	
	TRACE(Kern::Printf("SM CreateSurface ChunkPhysicalAddress r %d chunk %08x chunk size %d", r, aChunk, aChunk->iSize);)
	TRACE(Kern::Printf("SM CreateSurface kernAddr %08x", kernAddr);)
	TRACE(Kern::Printf("SM CreateSurface mapAttr %08x", aMapAttr);)
	TRACE(Kern::Printf("SM CreateSurface physAddr %08x", physAddr);)
	TRACE(Kern::Printf("SM CreateSurface physAddr2 %08x", physAddr2);)
	
	if(r < KErrNone)
		{
		// Error means that there isn't memory in the whole chunk - so check the
		// relevant areas - it is allowed to have gaps between the buffers, but not 
		// within the actual regions that are used for buffers. 
		
		// So, we first check the area before first buffer up to "offsettofirstbuffer", which all should be valid
		if (aAttribs.iOffsetToFirstBuffer != 0)
			{
			r = Kern::ChunkPhysicalAddress(aChunk, 0, aAttribs.iOffsetToFirstBuffer, 
					kernAddr, aMapAttr, physAddr, physAddr2);
			}
		else
			{
			r = KErrNone;
			}
		
		// If that's a pass, loop through and check the actual buffers (leave loop if it fails).
		for(TInt i = 0; i < aAttribs.iBuffers && KErrNone <= r; i++)
			{
			r = Kern::ChunkPhysicalAddress(aChunk, 
					aAttribs.iOffsetToFirstBuffer + aAttribs.iOffsetBetweenBuffers * i, 
					aBuffersize, kernAddr, aMapAttr, physAddr, physAddr2);
			}
		}

	// Fix up weird ChunkPhysicalAddress behaviour - it returns 1 to indicate that memory is non-contiguous. 
	if (1 == r)
		{
		r = KErrNone;
		}

	delete[] physAddr2;
	return r;
	}


/**
Creates a surface in an existing shared chunk.
@param aParam  Package buf containing the surface creation parameters and id to be set to the surface id of the newly created surface.  
@param aChunkHandle  Existing valid shared chunk handle.
@return KErrNone if successful, KErrArgument if the creation attributes were incorrect,
KErrBadHandle if aChunkHandle is of an invalid shared chunk memory,
otherwise one of the other system wide error codes.
@see RSurfaceManager::TSurfaceCreationAttributes
@internalTechnology
*/
TInt DSurfaceManager::CreateSurface(RSurfaceManagerDriver::TDeviceParam* aParam, TInt aChunkHandle)
	{
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attribs = buf();

	//Get the input parameters
	RSurfaceManagerDriver::TDeviceParam param;
	kumemget(&param, aParam, sizeof(RSurfaceManagerDriver::TDeviceParam));
	Kern::KUDesGet(buf, *(reinterpret_cast<const TDesC8*>(param.iBuffer)));
	if( (attribs.iHintCount > KMaxHintsPerSurface) || (attribs.iHintCount<0) )
		{
		return KErrArgument;
		}	
	
	RSurfaceManager::THintPair tempSurfaceHints[KMaxHintsPerSurface];
	if( (attribs.iHintCount>0) && attribs.iSurfaceHints)
		{
		kumemget(tempSurfaceHints, attribs.iSurfaceHints, attribs.iHintCount*sizeof(RSurfaceManager::THintPair));		
		attribs.iSurfaceHints = tempSurfaceHints;
		}
	else
		{
		attribs.iSurfaceHints=NULL;
		}
		
	//validate input parameters and calc size
	TInt roundedBufferSize = attribs.iOffsetBetweenBuffers;
	TUint actualBufferSize = 0;
	TInt chunkSize = ValidateAndCalculateChunkSize(attribs, roundedBufferSize, actualBufferSize);
	if (chunkSize == 0)
		{
		return KErrArgument;
		}
	
	NKern::ThreadEnterCS();
	
	//Open an existing shared chunk
	DChunk* chunk = Kern::OpenSharedChunk(NULL, aChunkHandle, EFalse);
	if(chunk == NULL)
		{
		NKern::ThreadLeaveCS();
		return KErrBadHandle;
		}
	
	//Check for chunk type as kernel multiple
	if(chunk->iChunkType != ESharedKernelMultiple)
		{
		Kern::ChunkClose(chunk);
		NKern::ThreadLeaveCS();
		return KErrBadHandle;
		}
	
	//Check for enough chunk size to create surface for requested attributes
	if (chunk->iSize < attribs.iOffsetToFirstBuffer + attribs.iBuffers * actualBufferSize)
		{
		Kern::ChunkClose(chunk);
		NKern::ThreadLeaveCS();
		return KErrArgument;
		}

	TSurfaceId sid;
	TSurface* surface = NULL;
	do		//in the unlikely event that we generate a duplicate surface id, try again.
		{
		GenerateSurfaceId(sid);

		NKern::FMWait(&iMutex);
		surface = FindSurfaceById(sid);
		NKern::FMSignal(&iMutex);
		}
	while (surface);

	//create a surface structure for the new surface
	surface = new TSurface;
	TRACE(Kern::Printf("SM A %08x TSurface CreateSurface",surface);)
	if (!surface)
		{
		//destroy the chunk and cleanup, out of memory
		Kern::ChunkClose(chunk);
		NKern::ThreadLeaveCS();
		return KErrNoMemory;
		}

	TUint32 mapAttr = 0;
	TBool isContiguous;
	TInt r = ValidatePhysicalMemory(chunk, attribs, actualBufferSize, mapAttr, isContiguous);
	if (r != KErrNone)
		{
		//destroy the surface and close the chunk
		delete(surface);
		Kern::ChunkClose(chunk);
		NKern::ThreadLeaveCS();
		if (r != KErrNoMemory)
			{
			r = KErrArgument;
			}
		return r;
		}
	
	surface->iId = sid;
	surface->iSize = attribs.iSize;
	surface->iBuffers = attribs.iBuffers;
	surface->iPixelFormat = attribs.iPixelFormat;
	surface->iStride = attribs.iStride;
	surface->iOffsetToFirstBuffer = attribs.iOffsetToFirstBuffer;
	surface->iAlignment = attribs.iAlignment;
	surface->iContiguous = isContiguous;
	surface->iChunk = chunk;
	surface->iOffsetBetweenBuffers = (attribs.iOffsetBetweenBuffers) ? attribs.iOffsetBetweenBuffers : roundedBufferSize;
	surface->iMappable = attribs.iMappable;
#ifndef __WINS__	//Creation attribute field will not considered for iCacheAttrib
	TUint32 level1Info = mapAttr & EMapAttrL1CacheMask;
	TUint32 level2Info = mapAttr & EMapAttrL2CacheMask;
	TBool chunkIsNotcached =  ((level2Info == EMapAttrL2Uncached) && 
	    ((level1Info == EMapAttrFullyBlocking) || (level1Info == EMapAttrBufferedNC) ||
	     (level1Info == EMapAttrBufferedC) || (level1Info == EMapAttrL1Uncached)));
	surface->iCacheAttrib = (chunkIsNotcached) ? RSurfaceManager::ENotCached : RSurfaceManager::ECached;
#else
	surface->iCacheAttrib = RSurfaceManager::ENotCached;	
#endif
	
	if(attribs.iHintCount>0)
		{
		memcpy(surface->iSurfaceHints,tempSurfaceHints,attribs.iHintCount*sizeof(RSurfaceManager::THintPair));
		}
	memclr(surface->iSurfaceHints+attribs.iHintCount, (KMaxHintsPerSurface-attribs.iHintCount)*sizeof(RSurfaceManager::THintPair));

	//create a surface owner for this surface
	TProcessListItem* owner = new TProcessListItem;
	TRACE(Kern::Printf("SM A %08x TProcessListItem CreateSurface",owner);)

	if (!owner)
		{
		//destroy the chunk and cleanup, out of memory
		Kern::ChunkClose(chunk);
		delete(surface);
		TRACE(Kern::Printf("SM D %08x TSurface CreateSurface",surface);)
		NKern::ThreadLeaveCS();
		return KErrNoMemory;
		}
	
	owner->iCount = 1;		//mark it as open in this process
	owner->iOwningProcess =  &Kern::CurrentProcess();
	owner->iNext = NULL;
		
	surface->iOwners = owner;	//only 1 owner at creation time

	NKern::FMWait(&iMutex);
	//at this point we have a fully constructed TSurface
	//add surface to head of surfaces list

	//Mask off the bottom log2(KMaxLists) bits of the first word of the surfaceID as an index
	//add the new surface to the beginning of the list 
	TInt index = SurfaceIdToIndex(sid);
	surface->iNext = iSurfacesIndex[index];
	iSurfacesIndex[index] = surface;
	NKern::FMSignal(&iMutex);		
	NKern::ThreadLeaveCS();
	
	//write surface id back to user side
	kumemput(reinterpret_cast<TSurfaceId*>(param.iSurfaceId), &sid, sizeof (TSurfaceId));
	return KErrNone;
	}


/**
Opens a surface.  If the current process already is in the owners list, its usage count is
incremented.  If this is an open from a different process, a new surface owner object is added
to the surface's list of owners and its usage count is set to 1.
@param aId  The surface id of the surface to be opened.
@return KErrNone if successful, otherwise a system error code
@internalTechnology
*/
TInt DSurfaceManager::OpenSurface(const TSurfaceId* aId)
	{
	TSurfaceId sid;
	//fetch surface id from user memory
	kumemget(&sid, aId, sizeof (TSurfaceId));
	NKern::ThreadEnterCS();
	TProcessListItem* owner = new TProcessListItem;  //speculative creation
	TRACE(Kern::Printf("SM A %08x TProcessListItem OpenSurface", owner);)
	
	NKern::FMWait(&iMutex);
	//look it up
	TSurface* surface = FindSurfaceById(sid);
	if (!surface)	
		{
		NKern::FMSignal(&iMutex);
		delete owner;		//free the memory just allocated
		TRACE(Kern::Printf("SM D %08x TProcessListItem OpenSurface", owner);)
		NKern::ThreadLeaveCS();
		return KErrArgument;
		}

	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (so)
		{
		//already an owner so inc the ref count
		++so->iCount;
		}
	else
		{
		//new process trying to open it
		if (!owner)
			{
			//the creation of the owner information object failed, out of memory
			NKern::FMSignal(&iMutex);
			NKern::ThreadLeaveCS();
			return KErrNoMemory;
			}
			
		owner->iCount = 1;		//mark it open in this process
		owner->iOwningProcess =  &Kern::CurrentProcess();
		
		//add the owner to the list of owners
		owner->iNext = surface->iOwners;
		surface->iOwners = owner;
		owner = NULL;
		}
	NKern::FMSignal(&iMutex);
	delete owner;		//free if not used.
	TRACE(Kern::Printf("SM D %08x TProcessListItem OpenSurface", owner);)
	NKern::ThreadLeaveCS();
	return KErrNone;
	}



/**
Closes a surface.  Decrements the usage count in the surface owner object
if the usage count is then zero, removes this surface owner from the surface.
If this results in a surface with no owners, the surface is deleted and the 
surface shared chunk is closed.
@param aId  The id of the surface to be closed
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a surface,
KErrAccessDenied if the surface is not open in the current process, otherwise a system wide
error code.
@internalTechnology
*/
TInt DSurfaceManager::CloseSurface(const TSurfaceId* aId)
	{

	TSurfaceId sid;
	kumemget(&sid, aId, sizeof (TSurfaceId));	//fetch surface id from user memory
	//look it up
	NKern::ThreadEnterCS();
	NKern::FMWait(&iMutex);
	TSurface* surface = FindSurfaceById(sid);
	if (!surface)	
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrArgument;
		}

	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (!so)
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrAccessDenied;
		}

	//current process is a surface owner so decrement the open count
	TSurface* surfaceToDelete = NULL;
	TProcessListItem* ownerToDelete = NULL;
	DChunk* chunkToClose = NULL;
	if (--so->iCount == 0)
		{
		//if count is now zero remove the owner
		//surface->RemoveOwner(so);
		UnlinkListItem(&surface->iOwners, so);
		ownerToDelete = so;

		//check to see if the surface has any owners
		if (!surface->iOwners)
			{
			//no more owners of the surface
			chunkToClose = surface->iChunk;

			//remove the surface from the list
			UnlinkListItem(&(iSurfacesIndex[SurfaceIdToIndex(surface->iId)]), surface);
			surfaceToDelete = surface;
			}
		}
	
	NKern::FMSignal(&iMutex);

	if (chunkToClose)
		{
		//surface has no more owners so close the chunk
		Kern::ChunkClose(chunkToClose);
		}

	delete surfaceToDelete;
	TRACE(Kern::Printf("SM D %08x TSurface CloseSurface",surfaceToDelete);)
	delete ownerToDelete;
	TRACE(Kern::Printf("SM D %08x TProcessListItem CloseSurface",ownerToDelete);)
	NKern::ThreadLeaveCS();

	return KErrNone;
	}


/**
Maps the surface memory into the process of the calling thread. This will fail if
the surface is not open in this process, or if the handle to the chunk cannot be created.
@param aId  The id of the surface to be mapped in.
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a
surface, KErrAccessDenied if the surface is not open in the current process,
KErrNotSupported if the surface is not mappable, KErrOverflow if the chunk limit has been
exceeded in the moving memory model, otherwise a system wide error code.
@internalTechnology
*/
TInt DSurfaceManager::MapSurface(const TSurfaceId* aId)
	{
	TSurfaceId sid;
	kumemget(&sid, aId, sizeof (TSurfaceId));	//fetch surface id from user memory

	//look it up
	NKern::ThreadEnterCS();
	NKern::FMWait(&iMutex);
	TSurface* surface = FindSurfaceById(sid);
	if (!surface)	
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrArgument;	//surface id is not valid or in the list of surfaces
		}
	if(!surface->iMappable)
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();		
		return KErrNotSupported;
		}

	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (!so)
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrAccessDenied;	//can't map it in, it's not open in this process
		}

	DChunk* chunk = surface->iChunk;
	TInt r = chunk->Open();
	NKern::FMSignal(&iMutex);
	TRACE(Kern::Printf("SM MapSurface chunk open r %d\n",r);)

	if (r == KErrGeneral)
		{
		NKern::ThreadLeaveCS();
		return KErrAccessDenied;
		}

	//if we are here, got the surface and we are the owner.
	//if we are the owner we must have it open at least once

	r = Kern::MakeHandleAndOpen(NULL, chunk);
	chunk->Close(NULL);
	TRACE(Kern::Printf("SM MapSurface handle open r: %d\n",r);)

	NKern::ThreadLeaveCS();

	return r;
	}


/**
Record a new connection to the driver.
Adds an element to the reference counted list of connected processes if the connection
is from a new process, otherwise it increments the reference count.
@param aProcess  The process which has opened a driver channel.
@internalTechnology
*/
TInt DSurfaceManager::AddConnection(const DProcess* aProcess)
	{
	TRACE(Kern::Printf("SM AddConnection process %08x\n", aProcess);)
	NKern::ThreadEnterCS();
	TProcessListItem* connectedProcess = new TProcessListItem;  //speculative creation
	TRACE(Kern::Printf("SM A %08x TProcessListItem AddConnection", connectedProcess);)
	NKern::FMWait(&iMutex);
	TProcessListItem* p = FindConnectedProcess(aProcess);
	if (p)	//already connected, found the process
		{
		++p->iCount;
		}
	else
		{
		//add a new connected process
		if (!connectedProcess)
			{
			//the creation of the owner information object failed, out of memory
			NKern::FMSignal(&iMutex);
			NKern::ThreadLeaveCS();
			return KErrNoMemory;
			}
		connectedProcess->iOwningProcess = (DProcess*)aProcess;
		connectedProcess->iCount=1;
		
		connectedProcess->iNext = iConnectedProcesses;
		iConnectedProcesses = connectedProcess;
		connectedProcess = NULL;
		}
	NKern::FMSignal(&iMutex);
	delete connectedProcess;
	TRACE(Kern::Printf("SM D %08x TProcessListItem AddConnection", connectedProcess);)
	NKern::ThreadLeaveCS();
	return KErrNone;
	}
	
	
	
/**
Called when the driver channel is closed.
Decrements the reference count for the connected process, if the last connection
for this process is closed (reference count reaches 0) it removes the process from the list.
@param aProcess  The process which has closed the driver channel.
@internalTechnology
*/
void DSurfaceManager::RemoveConnection(const DProcess* aProcess)
	{
	TRACE(Kern::Printf("SM RemoveConnection process %08x\n", aProcess);)
	NKern::ThreadEnterCS();
	NKern::FMWait(&iMutex);
	TProcessListItem* p =FindConnectedProcess(aProcess);
	TProcessListItem* toDelete = NULL;
	if (p)	//already connected, found the process
		{
		if (--p->iCount == 0) //last connection in process has disconnected
			{
			//remove the process from the list and cleanup
			UnlinkListItem(&iConnectedProcesses, p);
			toDelete = p;
			}
		}
	NKern::FMSignal(&iMutex);
	delete toDelete;
	TRACE(Kern::Printf("SM D %08x TProcessListItem RemoveConnection ", toDelete);)
	
	
	if (toDelete)	// if a process has closed its last channel, remove process from the surface owners.
		{
		CloseSurfaceHandlesForProcess(aProcess);
		}

	NKern::ThreadLeaveCS();
	}
	



/**
Closes all the surfaces belonging to the process which has just terminated.
If this is the only owner of a surface, delete the surface.
@param aProcess  The process which has terminated.
@pre must be called in critical section
@internalTechnology
*/
void DSurfaceManager::CloseSurfaceHandlesForProcess(const DProcess* aProcess)
	{

	NKern::FMWait(&iMutex);

	TSurface* p = NULL;
	TSurface* surfacesTodelete = NULL;
	TProcessListItem* ownersTodelete = NULL;
	TProcessListItem* so;
	// There are 16 doubly linked lists managed by Surface Manager
	for (TInt index = 0; index < KMaxLists; index++)
		{
		p = iSurfacesIndex[index];
		while(p)
			{
			//see if the process which has just died is an owner of any surfaces
			TSurface* surface = p;
			p=p->iNext;
			so = surface->ProcessOwnerInfo(aProcess);
			if (so)
				{
				UnlinkListItem(&surface->iOwners, so);
				so->iNext = ownersTodelete;	//add the owner to the list of owner objects to remove
				ownersTodelete = so;

				if (!surface->iOwners)	//if the surface hasn't any owners
					{
					//remove the surface from the list
					UnlinkListItem(&iSurfacesIndex[index], surface);
					surface->iNext = surfacesTodelete;	//add the surface to the list of surfaces to remove
					surfacesTodelete = surface;
					}
				}
			}
		}
	NKern::FMSignal(&iMutex);

	while(surfacesTodelete)
		{
		p = surfacesTodelete->iNext;
		Kern::ChunkClose(surfacesTodelete->iChunk);
		TRACE(Kern::Printf("SM Close chunk %08x CloseSurfaceHandlesForProcess",surfacesTodelete->iChunk);)
		delete surfacesTodelete;
		TRACE(Kern::Printf("SM D %08x TSurface CloseSurfaceHandlesForProcess",surfacesTodelete);)
		surfacesTodelete = p;
		}

	while(ownersTodelete)
		{
		so = ownersTodelete->iNext;
		delete ownersTodelete;
		TRACE(Kern::Printf("SM D %08x TProcessListItem CloseSurfaceHandlesForProcess",ownersTodelete);)
		ownersTodelete = so;
		}
	}

	
/**
Returns the metadata information about the specified surface.
@param aId  The id of the surface.
@param aInfo  Pointer to user side descriptor to receive the information.
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a surface,
KErrAccessDenied if the surface is not open in the current process, otherwise a system wide
error code.
@internalTechnology
*/	
TInt DSurfaceManager::SurfaceInfo(const TSurfaceId* aId, TDes8* aInfo)
	{
	TSurfaceId sid;
	//fetch surface id from user memory
	kumemget(&sid, aId, sizeof (TSurfaceId));

	RSurfaceManager::TInfoBuf buf;
	RSurfaceManager::TSurfaceInfoV01& info = buf();

	NKern::FMWait(&iMutex);
	//look it up
	TSurface* surface = FindSurfaceById(sid);
	if (!surface)	
		{
		NKern::FMSignal(&iMutex);
		return KErrArgument;
		}
	
	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (!so)
		{
		NKern::FMSignal(&iMutex);
		return KErrAccessDenied;	//can do this, not open
		}
	
	//at this point, we have a surface, we are the owner and it's mapped in
	info.iSize = surface->iSize; 									// Visible width/height in pixels
	info.iBuffers = surface->iBuffers;								// Number of Buffers
	info.iPixelFormat = surface->iPixelFormat;	      				// pixel format
	info.iStride = surface->iStride;								// Number of bytes between start of one line and start of next
	info.iContiguous = surface->iContiguous;						// is it physically contiguous
	info.iCacheAttrib = surface->iCacheAttrib;						// Underlying chunk is CPU cached or not
	info.iMappable = surface->iMappable;							// Is the surface Mappable
	NKern::FMSignal(&iMutex);
	
	//copy it back to user side
	Kern::InfoCopy(*aInfo, buf);
	return KErrNone;
	}



/**
Generates a unique surface id
@param aId  Surface id reference to receive the generated id.
@internalTechnology
*/	
void DSurfaceManager::GenerateSurfaceId(TSurfaceId& aId)
	{
	TSurfaceId id;
	
	for (TInt x = 0; x < 4; ++x)
		{
		id.iInternal[x] = Kern::Random();
		};
	
	//package up the handle,
	//set the type identifier
	id.iInternal[3] &= 0x00FFFFFF;
	id.iInternal[3] |= TSurfaceTypes::ESurfaceManagerSurface << 24;
	aId = id;
	TRACE(Kern::Printf("SM GenerateSurfaceId id = %u %u %u %u\n",id.iInternal[0],id.iInternal[1],id.iInternal[2],id.iInternal[3]);)
	};
	


/**
Validates the surface creation attributes and calculates the size of the chunk required.
@param aAttribs  The surface creation attributes used to specify the surface requirements.
@param aOffset  Set to the offset between buffers on successfull return.
@param aNewChunk  If this is true, surface is created in a new chunk otherwise the surface is created in an existing chunk
@return The size of chunk required.  A size of 0 indicates a problem.
*/	
TInt DSurfaceManager::ValidateAndCalculateChunkSize(RSurfaceManager::TSurfaceCreationAttributes& aAttribs, 
			TInt& aOffset, TUint &aActualBufferSize, const TBool aNewChunk)
	{
/*	
	TRACE(Kern::Printf("SM width = %d  height = %d\n", aAttribs.iSize.iWidth, aAttribs.iSize.iHeight);)
	TRACE(Kern::Printf("SM buffers = %d\n", aAttribs.iBuffers);)
	TRACE(Kern::Printf("SM format = %d\n", aAttribs.iPixelFormat);)
	TRACE(Kern::Printf("SM stride = %d\n", aAttribs.iStride);)
	TRACE(Kern::Printf("SM offset to first buffer = %d\n", aAttribs.iOffsetToFirstBuffer);)
	TRACE(Kern::Printf("SM offset between buffer = %d\n", aOffset);)
	TRACE(Kern::Printf("SM alignment = %d\n", aAttribs.iAlignment);)
	TRACE(Kern::Printf("SM contiguous = %d\n\n", aAttribs.iContiguous);)
	TRACE(Kern::Printf("SM cacheAttrib = %d\n\n", aAttribs.iCacheAttrib);)
*/
	//check for negative values
	if(aAttribs.iOffsetToFirstBuffer < 0 || aOffset < 0 )
		{
		TRACE(Kern::Printf("SM Validate offset for negative value");)
		return 0;
		}

	//check aligment is sensible
	TInt alignmentMask = 0;
	switch(aAttribs.iAlignment)
		{
		case 1:	
		case 2:	
		case 4: 
		case 8: 
		case 16: 
		case 32: 
			alignmentMask = 31; 
			break;
		case 64: 
			alignmentMask = 63; 
			break;
		case RSurfaceManager::EPageAligned:
			break;
		default:
			TRACE(Kern::Printf("SM Validate alignment");)
			return 0;
		}
	
	//check alignment issues.
	if(aAttribs.iAlignment != RSurfaceManager::EPageAligned)
		{
		if(aNewChunk)	
			{
			if(aAttribs.iCacheAttrib == RSurfaceManager::ECached)	// Surface is CPU cached, so the alignment will be based on either 32 or 64 byte 
				{
				//offset to first buffer needs to fit alignment
				aAttribs.iOffsetToFirstBuffer = aAttribs.iOffsetToFirstBuffer + alignmentMask & ~alignmentMask;
				//alignment with respect to offsetbetweenbuffers
				aOffset = aOffset + alignmentMask & ~alignmentMask;
				}
			else	// Surface is NOT CPU cached, so the alignment will be based on surface attribute alignment
				{
				TUint alignMask = aAttribs.iAlignment-1;
				//offset to first buffer needs to fit alignment
				aAttribs.iOffsetToFirstBuffer = aAttribs.iOffsetToFirstBuffer + alignMask & ~alignMask;
				//alignment with respect to offsetbetweenbuffers
				aOffset = aOffset + alignMask & ~alignMask;
				}
			}
		else	// existing chunk
			{
			TUint alignMask = aAttribs.iAlignment-1;
			//check alignment issues.  offset to first buffer needs to fit alignment
			if (aAttribs.iOffsetToFirstBuffer & alignMask)
				{
				TRACE(Kern::Printf("SM Validate offset to first pixel misaligned");)
				return 0;
				}

			//check alignment for offsetbetweenbuffers.  offset between buffer needs to fit alignment for existing chunks
			if (aOffset & alignMask)
				{
				TRACE(Kern::Printf("SM Validate offset between buffers misaligned");)
				return 0;
				}
			}
		}
	else	//page aligned
		{
		if(aNewChunk)// if its a new chunks and doesn't match exact alignment then do the rounding
			{
			TUint32 pageSize = Kern::RoundToPageSize(1);
			//offset to first buffer needs to fit alignment
			aAttribs.iOffsetToFirstBuffer = (aAttribs.iOffsetToFirstBuffer + (pageSize - 1)) & ~(pageSize - 1);
			//alignment with respect to offsetbetweenbuffers
			aOffset = (aOffset + (pageSize - 1)) & ~((pageSize - 1));
			}
		else	// for existing chunks don't do any rounding operation
			{
			TUint32 pageSize = Kern::RoundToPageSize(1);
			TUint alignmask = aAttribs.iOffsetToFirstBuffer & (pageSize - 1);
			if (alignmask)
				{
				TRACE(Kern::Printf("SM Validate offset to first pixel misaligned");)
				return 0;
				}
			
			alignmask = aOffset & (pageSize - 1);
			if (alignmask)
				{
				TRACE(Kern::Printf("SM Validate offset between buffers misaligned");)
				return 0;
				}
			}
		}

	//check width and height
	if(aAttribs.iSize.iWidth <= 0 || aAttribs.iSize.iHeight <= 0)
		{
		TRACE(Kern::Printf("SM Validate width/height");)
		return 0;
		}
	
	
	//check there is at least 1 buffer
	if (aAttribs.iBuffers <= 0)
		{
		TRACE(Kern::Printf("SM Validate buffers");)
		return 0;
		}

	//Sort the array and also check for duplication
	if (!SortHints(aAttribs.iSurfaceHints,aAttribs.iHintCount)) 
		{
		TRACE(Kern::Printf("SM Validate Duplicate hint key");)
		return 0;
		}

	TUint size = 0;
	//calculate buffer size and round it to alignment or to page size
	TInt64 bufferSize = aAttribs.iStride;
	bufferSize  *= aAttribs.iSize.iHeight;

	if (I64HIGH(bufferSize) > 0) //too big
		{
		TRACE(Kern::Printf("SM Validate chunk buffer size is out of range");)
		return 0;
		}
	
	TUint bsize = I64LOW(bufferSize);
	if (bsize > KMaxTInt)
		{
		TRACE(Kern::Printf("SM Validate buffer size is out of range for TInt");)
		return 0;
		}

	if(aAttribs.iAlignment == RSurfaceManager::EPageAligned)
		{
		bsize = Kern::RoundToPageSize(bsize);	//page alignment
		}
	else if(aAttribs.iCacheAttrib == RSurfaceManager::ECached)
		{
		bsize = bsize + alignmentMask & ~alignmentMask;	//CPU cached byte alignment, for minimum of the specified alignment(32 or 64)
		}
	else
		{
		bsize = bsize + (aAttribs.iAlignment-1) & ~(aAttribs.iAlignment-1);	//NON CPU cached byte alignment for 1, 2, 4, 8, 16, 32 and 64
		}
	
	bufferSize = bsize;
	// Remember the actual size. 
	aActualBufferSize = bsize;

	//if offset between buffers is zero, then assign the calculated value as offset between buffers
	if(aOffset == 0)
		{
		//buffer size rounded to alignment as offset between buffers
		aOffset = I64INT(bufferSize);
		}
	else if(aOffset < I64INT(bufferSize))
		{
		TRACE(Kern::Printf("SM Offset between the buffer is less than the required size");)
		return 0;
		}
	else
		{
		//use the buffer size specified
		bufferSize = aOffset;
		}
	
	
	TInt64 totalSize = aAttribs.iOffsetToFirstBuffer + (aAttribs.iBuffers * bufferSize);
	
	if (I64HIGH(totalSize) > 0) //too big
		{
		TRACE(Kern::Printf("SM Validate chunk size is out of range for RoundToPageSize");)
		return 0;
		}
		
	size = I64LOW(totalSize);
	if (size > KMaxTInt)
		{
		TRACE(Kern::Printf("SM Validate size is out of range for TInt");)
		return 0;
		}

	size = Kern::RoundToPageSize(size);

	//check the size isn't greater than will fit in a TInt
	if (size > KMaxTInt)
		{
		TRACE(Kern::Printf("SM Rounded size is out of range for TInt");)
		return 0;
		}
	
	TRACE(Kern::Printf("SM After validate - offset to first buffer = %d\n", aAttribs.iOffsetToFirstBuffer);)
	TRACE(Kern::Printf("SM After validate - offset between buffer = %d\n", aOffset);)
	TRACE(Kern::Printf("SM CalculateChunkSize size = %d\n", size);)
	return size;
	}


/**
Find the surface in the list.   
@param aId  The surface id of the surface to find in the surface list
@return pointer to the surface object
@internalTechnology
*/
TSurface* DSurfaceManager::FindSurfaceById(const TSurfaceId& aId)
	{
	TSurface *p = iSurfacesIndex[SurfaceIdToIndex(aId)];
	while (p)
		{
		if (aId == p->iId)
			{
			//found it
			return p;
			}
	
		p = p->iNext;
		}
	return NULL;
	}


/**
Find the index of the hint key from the surface list using binary search.   
@param aHintsArray  Pointer to the first element in the array of surface hints
@param aKey  The surface hint key uid value to search in the surface list
@return index of the hint pair key in the surface list, KErrNotFound if key not found
@internalTechnology
*/
TInt DSurfaceManager::FindHintKey(const RSurfaceManager::THintPair* aHintsArray, TUint32 aKey) const
	{
	__ASSERT_DEBUG(aHintsArray != NULL, Kern::Fault("Surface Manager", __LINE__));

	TInt bottom = 0;
	TInt top = KMaxHintsPerSurface - 1;
	TInt mid;
	while (bottom <= top)
		{
	    mid = (bottom + top) / 2;
	    if((TUint) aHintsArray[mid].iKey.iUid == aKey)
	    	{
	    	return mid;
	    	} 
	    else if ((TUint)aHintsArray[mid].iKey.iUid < aKey) 
	    	{
	    	top = mid - 1;
	    	}
	    else
	    	{
	    	bottom = mid + 1;
	    	}
	  }
	return KErrNotFound;	//Hint key not found
    }

TProcessListItem* DSurfaceManager::FindConnectedProcess(const DProcess* aProcess)
	{
	TProcessListItem * p = iConnectedProcesses;
	while (p)
		{
		if (aProcess == p->iOwningProcess)
			{
			//found it
			return p;
			}
		
		p = p->iNext;
		}
	return NULL;
	}

/**
Searches for a right place to insert the new hint pair in a sorted array.
@param aHintsArray  Pointer to the first element in the sorted array
@param aKey  The surface hint key uid value to search in the surface list
@pre, there is at least one empty place in the array
@return KErrNone if a new hint pair key inserted in the surface list, KErrAlreadyExists if duplicated
@internalTechnology
*/
TInt DSurfaceManager::InsertHintKey(RSurfaceManager::THintPair* aHintsArray, const RSurfaceManager::THintPair& aHintPair) const
	{
	__ASSERT_DEBUG(aHintsArray != NULL, Kern::Fault("Surface Manager", __LINE__));
	__ASSERT_DEBUG(aHintsArray[KMaxHintsPerSurface-1].iKey.iUid == NULL, Kern::Fault("Surface Manager", __LINE__));

	TInt pos = 0;
	if (aHintsArray[pos].iKey.iUid != 0)
		{
		while((TUint)aHintsArray[pos].iKey.iUid>(TUint)aHintPair.iKey.iUid && pos < KMaxHintsPerSurface-1)
			{// find the right place to insert
			++pos;
			}
	
		if((TUint)aHintsArray[pos].iKey.iUid==(TUint)aHintPair.iKey.iUid)
			{
			//Duplicate key 
			return KErrAlreadyExists;
			}
		else
			{
			// Shift right
			memmove(aHintsArray+pos+1, aHintsArray+pos, (KMaxHintsPerSurface-pos-1)*sizeof(RSurfaceManager::THintPair));		
			}	
		}
	aHintsArray[pos] = aHintPair;
	return KErrNone;
	}

/**
Sort the surface hint array in descending order.
@param aHintsArray  The surface hintpair in the surface list
@param aNumberOfHints The number of hints
@return ETrue if sorting is finished or it is an empty array, EFalse if key duplicated
@internalTechnology
*/
TBool DSurfaceManager::SortHints(RSurfaceManager::THintPair* aHintsArray, TInt aNumberOfHints) const
	{
	TInt in = 0;
	TInt out = 0;
	RSurfaceManager::THintPair temp;
	if(!aHintsArray)
		{
		return ETrue;
		}
	for(out = 0; out < aNumberOfHints; ++out) 
		{
		if(aHintsArray[out].iKey.iUid != 0)
			{
			temp = aHintsArray[out];   
			in = out;          // start shifting at out
			while(in > 0 && (TUint)aHintsArray[in-1].iKey.iUid <= (TUint)temp.iKey.iUid)
				{
				if ((TUint)aHintsArray[in-1].iKey.iUid == (TUint)temp.iKey.iUid)
					{
					return EFalse;		//duplicate hint keys are not allowed
					}
				aHintsArray[in] = aHintsArray[in-1];     // shift item to the right
				--in;          // go left one position
				}
			aHintsArray[in] = temp;        // insert marked item
			}
		}
	return ETrue;
	}


/**
Ensures the memory is updated consistently before/after triggering non CPU hardware access. 
@param aParam  The suface id and buffer number (0 based).
@param aOperation  The type of the synchronize operation. 
@return KErrNone if successful, KErrArgument if the surface ID is invalid or
buffer number is invalid, KErrAccessDenied if the surface is not open in this
process, otherwise a system wide error code.
@see RSurfaceManager::TSyncOperation
@internalTechnology
*/	
TInt DSurfaceManager::SynchronizeCache(RSurfaceManagerDriver::TDeviceParam* aParam, RSurfaceManager::TSyncOperation aOperation)
	{
	//Parse the parameters
	RSurfaceManagerDriver::TDeviceParam param;
	kumemget(&param, aParam, sizeof(RSurfaceManagerDriver::TDeviceParam));
	TSurfaceId sid;
	kumemget(&sid, param.iSurfaceId, sizeof(TSurfaceId));
	TInt buffer = (TInt)param.iBuffer;
	
	NKern::ThreadEnterCS();
	NKern::FMWait(&iMutex);
	//look it up
	TSurface* surface = FindSurfaceById(sid);
	if (!surface)	
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrArgument;
		}
	
	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (!so)
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrAccessDenied;
		}

	// surfaces have to have at least one buffer
	__ASSERT_DEBUG(surface->iBuffers > 0, Kern::Fault("Surface Manager", __LINE__));
	
	//Validate the buffer number is within range
	if((buffer >= surface->iBuffers) || (buffer < 0))
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrArgument;
		}

	DChunk* chunk = surface->iChunk;
	TInt offsetBetweenBuffers = surface->iOffsetBetweenBuffers;
	NKern::FMSignal(&iMutex);

	TUint32 kernAddr;
	TUint32 mapAttr;
	TUint32 physAddr;
	TInt pageList = chunk->iSize / Kern::RoundToPageSize(1) + 1;
	TUint32* physAddr2 = new TUint32[pageList];
	if(!physAddr2)
		{
		NKern::ThreadLeaveCS();
		return KErrNoMemory;
		}
	
	TRACE(Kern::Printf("SM %08x DChunk SynchronizeCache", chunk);)
	
	//Retrieve the kernel address and mapping attribute from the chunk
	TInt err = Kern::ChunkPhysicalAddress(chunk, surface->iOffsetToFirstBuffer + (buffer * offsetBetweenBuffers), offsetBetweenBuffers, kernAddr, mapAttr, physAddr, physAddr2);
	delete[] physAddr2;
	if(err >= KErrNone)
		{
		TRACE(Kern::Printf("SM %08x kernAddr SynchronizeCache", kernAddr);)
		TRACE(Kern::Printf("SM %08x mapAttr SynchronizeCache", mapAttr);)
		err = KErrNone;

		// Do the sync operation
		switch(aOperation)
			{
			case RSurfaceManager::ESyncBeforeNonCPURead:
				Cache::SyncMemoryBeforeDmaWrite(kernAddr, offsetBetweenBuffers, mapAttr);
				break;
			case RSurfaceManager::ESyncBeforeNonCPUWrite:
				Cache::SyncMemoryBeforeDmaRead(kernAddr, offsetBetweenBuffers, mapAttr);
				break;
			case RSurfaceManager::ESyncAfterNonCPUWrite:
				Cache::SyncMemoryAfterDmaRead(kernAddr, offsetBetweenBuffers);
				break;
			default: 
				err = KErrArgument;
				break;
			}			
		}
	NKern::ThreadLeaveCS();

	return err;
	}


/**
Get the surface hint value for the given surface ID and hint pair key.
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aHintPair  The hint value for the requested hint pair key.
@return KErrNone if successful, KErrArgument if the surface ID is invalid or
invalid hint pair key used, KErrAccessDenied if the surface is not open in the
current process, otherwise a system wide error code.
@internalTechnology
*/ 
TInt DSurfaceManager::GetSurfaceHint(const TSurfaceId* aSurfaceId, RSurfaceManager::THintPair* aHintPair)
	{
	RSurfaceManager::THintPair hintPair;
	kumemget(&hintPair, aHintPair, sizeof(RSurfaceManager::THintPair));

	if (hintPair.iKey.iUid == 0)
		{
		TRACE(Kern::Printf("SM GetSurfaceHint Hint key is invalid");)
		return KErrArgument;	//Invalid Hint key
		}
	
	TSurfaceId sid;
	//fetch surface id from user memory
	kumemget(&sid, aSurfaceId, sizeof (TSurfaceId));

	NKern::FMWait(&iMutex);
	//look it up
	TSurface* surface = FindSurfaceById(sid);
	if (!surface)	
		{
		NKern::FMSignal(&iMutex);
		return KErrArgument;
		}
	
	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (!so)
		{
		NKern::FMSignal(&iMutex);
		return KErrAccessDenied;
		}
	
	//at this point, we have a surface, we have to find the hint value based on the hint pair key
	TInt index = FindHintKey(surface->iSurfaceHints, hintPair.iKey.iUid);

	if (index == KErrNotFound)
		{
		TRACE(Kern::Printf("SM GetSurfaceHint Hint key not found");)
		NKern::FMSignal(&iMutex);
		return KErrArgument;	//Hint key not found
		}

	RSurfaceManager::THintPair hint = surface->iSurfaceHints[index];
	NKern::FMSignal(&iMutex);
		
	TRACE(Kern::Printf("SM GetSurfaceHint Hint value %d", hint.iValue);)
	//write it back to user side
	kumemput(aHintPair, &hint, sizeof(RSurfaceManager::THintPair));
	return KErrNone;
	}


/**
Set the surface hint value for an existing surface hint key of the surface Id.
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aHintPair  The value of the hint pair to set.
@return KErrNone if successful, KErrArgument if the surface ID is invalid or if invalid
hint key used, KErrAccessDenied if the hint pair is immutable or the surface is not open
in the current process, otherwise a system wide error code.
@internalTechnology
*/ 
TInt DSurfaceManager::SetSurfaceHint(const TSurfaceId* aSurfaceId, const RSurfaceManager::THintPair* aHintPair)
	{
	RSurfaceManager::THintPair hintPair;
	kumemget(&hintPair, aHintPair, sizeof(RSurfaceManager::THintPair));

	//Check for valid hint key
	if (!hintPair.iKey.iUid)
		{
		TRACE(Kern::Printf("SM SetSurfaceHint Hint key is invalid");)
		return KErrArgument;	//Invalid Hint key
		}
	
	TSurfaceId sid;
	//fetch surface id from user memory
	kumemget(&sid, aSurfaceId, sizeof (TSurfaceId));

	NKern::ThreadEnterCS();
	NKern::FMWait(&iMutex);
	//look it up
	TSurface* surface = FindSurfaceById(sid);
	if (!surface)	
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrArgument;
		}
	
	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (!so)
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrAccessDenied;
		}
	
	//at this point, we have a surface, we have to find the hint value based on the hint pair key
	TInt index = FindHintKey(surface->iSurfaceHints, hintPair.iKey.iUid);
	if (index == KErrNotFound)
		{
		TRACE(Kern::Printf("SM SetSurfaceHint Hint key not found or invalid");)
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrArgument;	//Hint key not found or invalid
		}
	
	//Check for mutability
	if(!surface->iSurfaceHints[index].iMutable)
		{
		TRACE(Kern::Printf("SM SetSurfaceHint Hint is immutable");)
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrAccessDenied;	//Hint pair is immutable
		}
	TRACE(Kern::Printf("SM SetSurfaceHint Hint key found and updated its value %d for the surface %08x \n", aHintPair->iValue, &sid);)
	
	//set the hint pair value now
	memcpy(&surface->iSurfaceHints[index], &hintPair, sizeof(RSurfaceManager::THintPair));
	NKern::FMSignal(&iMutex);
	NKern::ThreadLeaveCS();

	return KErrNone;
	}

/**
Add a new surface hint value for the surface Id.
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aHintPair  The value of the hint pair to Add.
@return Returns KErrNone if successful, KErrArgument if the surface ID is invalid or the
hint pair has invalid key UID, KErrAccessDenied if the surface is not open in the current
process, KErrAlreadyExists if duplicate hint key used, KErrOverflow if no space to add new
pair, otherwise a system wide error code.
@internalTechnology
*/ 
TInt DSurfaceManager::AddSurfaceHint(const TSurfaceId* aSurfaceId, const RSurfaceManager::THintPair* aHintPair)
	{
	RSurfaceManager::THintPair hintPair;
	kumemget(&hintPair, aHintPair, sizeof(RSurfaceManager::THintPair));

	//Check for valid hint key
	if (hintPair.iKey.iUid == 0)
		{
		TRACE(Kern::Printf("SM AddSurfaceHint Hint key is invalid");)
		return KErrArgument;	//Invalid Hint key
		}
	
	TSurfaceId sid;
	//fetch surface id from user memory
	kumemget(&sid, aSurfaceId, sizeof (TSurfaceId));

	NKern::ThreadEnterCS();
	NKern::FMWait(&iMutex);
	//look it up
	TSurface* surface = FindSurfaceById(sid);
	if (!surface)	
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrArgument;
		}
	
	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (!so)
		{
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrAccessDenied;
		}
	

	//Check for empty hint pair
	if(surface->iSurfaceHints[KMaxHintsPerSurface - 1].iKey.iUid != 0)//at least end of sorted hint array should be 0 to add a new hint
		{
		TRACE(Kern::Printf("SM AddSurfaceHint there is no room to add the hint");)
		NKern::FMSignal(&iMutex);
		NKern::ThreadLeaveCS();
		return KErrOverflow;	//No room for new hint
		}
	//We found room for a new hint pair, so insert it in the array
	// Meanwhile, we check for duplication, if it is, return KErrAlreadyExists
	TInt err = InsertHintKey(surface->iSurfaceHints,hintPair);
	NKern::FMSignal(&iMutex);
	TRACE(Kern::Printf("SM AddSurfaceHint Added new key ");)
	NKern::ThreadLeaveCS();
	return err;
	}

/**
Get the offset of the specified buffer from the base address of the underlying
chunk.

To obtain the address of the buffer, the offset returned must be added onto the
base address of the RChunk returned in a call to MapSurface(). Note that
buffer offsets are immutable during the lifetime of the surface.
@param aParam The input parameters including the surface ID and buffer index.
@pre The surface is open in the calling process.
@return KErrNone if successful, KErrArgument if aSurfaceId or aBuffer are invalid,
KErrAccessDenied if the surface is not open in the current process, KErrNotSupported if
the surface is not mappable, otherwise a system wide error code.
*/
TInt DSurfaceManager::GetBufferOffset(RSurfaceManagerDriver::TDeviceParam* aParam,TUint* aOffset)
	{
	//Get the input parameters
	RSurfaceManagerDriver::TDeviceParam param;
	kumemget(&param, aParam, sizeof(RSurfaceManagerDriver::TDeviceParam));
	TSurfaceId sid;
	//fetch surface id from user memory
	kumemget(&sid, param.iSurfaceId, sizeof(TSurfaceId));
	//(TAny*)iBuffer holds the buffer number in its value
	TInt bufferNumber = (TInt) param.iBuffer;
	
	TSurface* surface = NULL;
	NKern::FMWait(&iMutex);
	surface = FindSurfaceById(sid);
	if(NULL == surface || (bufferNumber >= surface->iBuffers))
		{
		NKern::FMSignal(&iMutex);
		return KErrArgument;
		}
	if(!surface->iMappable)
		{
		NKern::FMSignal(&iMutex);
		return KErrNotSupported;
		}
	//find the owner
	TProcessListItem* so = surface->ProcessOwnerInfo(&Kern::CurrentProcess());
	if (!so)
		{
		NKern::FMSignal(&iMutex);
		return KErrAccessDenied;		
		}
	TInt bufferOffset = surface->iOffsetToFirstBuffer + bufferNumber*surface->iOffsetBetweenBuffers;
	NKern::FMSignal(&iMutex);
	
	kumemput(aOffset, &bufferOffset, sizeof (TInt));
	return KErrNone;
	}

/**
Returns information specific to the Surface Manager implementation.
@param aAttrib: Attribute to retrieve
@param aValue : Output parameter where we write the value for the specified attribute
@return KErrNone if successful or KErrArgument if the attribute UID is not recognized
@internalTechnology
*/
TInt DSurfaceManager::GetSurfaceManagerAttrib(RSurfaceManager::TSurfaceManagerAttrib* aAttrib,TInt* aValue)
	{
	RSurfaceManager::TSurfaceManagerAttrib attrib;
	kumemget(&attrib, aAttrib, sizeof(RSurfaceManager::TSurfaceManagerAttrib));
	
	TInt out=KErrNone;
	TInt value;
	switch (attrib)
		{
		case RSurfaceManager::EMaxNumberOfHints:
			value=KMaxHintsPerSurface;
			break;		
		
		default:
			out=KErrArgument;
			break;			
		};
	
	if (out==KErrNone)
		{
		kumemput(aValue, &value, sizeof (TInt));
		}	
	return out;
	}
