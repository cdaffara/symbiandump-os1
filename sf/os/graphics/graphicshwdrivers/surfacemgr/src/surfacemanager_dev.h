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

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef __SURFACEMANAGER_DEV_H__
#define __SURFACEMANAGER_DEV_H__

#include <graphics/surface.h>
#include "surfacemanagerdriver.h"

#if 0
#define TRACE(x) x
#else
#define TRACE(x)
#endif

/** Maximum number of HintPairs per surface */
const TInt KMaxHintsPerSurface = 16;

/** Maximum number of elements in the table. This value must be a power of 2 */
#define KMaxLists 16

/**
  Logical Device (factory class) for Surface manager
*/
class DSurfaceManagerFactory : public DLogicalDevice
	{
public:
	DSurfaceManagerFactory();
	TInt Install();
	void GetCaps(TDes8& aDes) const;
	TInt Create(DLogicalChannelBase*& aChannel);
	};


/**
  Logical Channel class for SurfaceManager
*/
class DSurfaceManagerChannel : public DLogicalChannelBase
	{
public:
	DSurfaceManagerChannel();
	~DSurfaceManagerChannel();

	// Inherited from DLogicalChannelBase
	TInt DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);
	TInt Request(TInt aReqNo, TAny* a1, TAny* a2);
private:
	// Implementation for the different kinds of messages sent through RBusLogicalChannel
	TInt DoControl(TInt aFunction, TAny* a1, TAny* a2);

private:
	DProcess* iOwner;
	};


/**
Class to hold a reference count and process pointer. 
Each surface has a linked list of these owning processes.
The surface manager has a linked list of these to track connections.
@internalTechnology
*/
class TProcessListItem
	{
public:
	TProcessListItem* iNext;	//pointer to next one in list
	DProcess* iOwningProcess;	//pointer to the process object which is being reference counted
	TInt iCount;				//reference count
	};
	

/**
Class to hold the information about a surface.
Each surface has a linked list of owners.  The surfaces are arranged in a linked list,
with these objects being elements in the list.
@internalTechnology
*/
class TSurface					//one of these per surface
	{
public:
	TSurface* iNext;			//singly linked list, points to next surface
	TSurfaceId	iId;			//the surface id
	TSize iSize;				//pixel width/height
	TInt iBuffers;				//number of buffers
	TUidPixelFormat iPixelFormat;			//pixel format
	TInt iStride;				//offset from start of one line to the next, in bytes
	TInt iOffsetToFirstBuffer;	//offset between the start of the surface memory and the start of the first pixel buffer
	TInt iOffsetBetweenBuffers;	//offset between pixel buffers
	TInt  iAlignment;			//byte alignment of the pixel buffers
	TBool iContiguous;			//if it is in contiguous physical memory
	DChunk* iChunk;				//ptr to the shared chunk
	RSurfaceManager::TCacheAttribute iCacheAttrib;			//Caching attribute to create chunks memory
	RSurfaceManager::THintPair iSurfaceHints[KMaxHintsPerSurface];	//Arbitrary key-value pairs associated with a surface
	TBool iMappable;			//Is the Surface Mappable
	TProcessListItem* iOwners;	//owner list. Singly linked list, points to next surface owner

public:	
	TProcessListItem* ProcessOwnerInfo(const DProcess* aProcess);
	};
	



/**
Surface manager extension object.
There is one static instance of this in the kernel extension.
@internalTechnology
*/
class DSurfaceManager : public DBase
	{
public:
	TInt CreateSurface(const TDesC8* aConfig, TSurfaceId* aId);
	TInt SurfaceInfo(const TSurfaceId* aId, TDes8* aInfo);
	TInt OpenSurface(const TSurfaceId* aId);
	TInt CloseSurface(const TSurfaceId* aId);
	TInt MapSurface(const TSurfaceId* aId);
	TInt AddConnection(const DProcess* iProcess);
	void RemoveConnection(const DProcess* iProcess);
	TInt CreateSurface(RSurfaceManagerDriver::TDeviceParam* aParam, TInt aChunkHandle);
	TInt SynchronizeCache(RSurfaceManagerDriver::TDeviceParam* aId, RSurfaceManager::TSyncOperation aOperation);
	TInt GetSurfaceHint(const TSurfaceId* aSurfaceId, RSurfaceManager::THintPair* aHintPair);
	TInt SetSurfaceHint(const TSurfaceId* aSurfaceId, const RSurfaceManager::THintPair* aHintPair);
	TInt AddSurfaceHint(const TSurfaceId* aSurfaceId, const RSurfaceManager::THintPair* aHintPair);
	TInt GetBufferOffset(RSurfaceManagerDriver::TDeviceParam* aParam,TUint* aOffset);
	TInt GetSurfaceManagerAttrib(RSurfaceManager::TSurfaceManagerAttrib* aAttrib,TInt* aValue);
private:
	void GenerateSurfaceId(TSurfaceId& aId);
	TInt  CreateSurfaceChunk(const RSurfaceManager::TSurfaceCreationAttributes& attribs);
	TInt ValidateAndCalculateChunkSize(RSurfaceManager::TSurfaceCreationAttributes& aAttribs, TInt& aOffset, 
			TUint& aActualBufferSize, const TBool aNewChunk = EFalse);
	TInt ValidatePhysicalMemory(DChunk* aChunk, const RSurfaceManager::TSurfaceCreationAttributes& aAttribs, 
			TUint aBuffersize, TUint32& aMapAttr, TBool &aIsContiguous); 
	TSurface* FindSurfaceById(const TSurfaceId& aId);
	void CloseSurfaceHandlesForProcess(const DProcess* iProcess);
	TProcessListItem* FindConnectedProcess(const DProcess* aProcess);
	TInt FindHintKey(const RSurfaceManager::THintPair* aHints, TUint32 aKey) const;
	TBool SortHints(RSurfaceManager::THintPair* aHints, TInt aNumberOfHints) const;
	TInt InsertHintKey(RSurfaceManager::THintPair* aHints, const RSurfaceManager::THintPair& aHintPair) const;

private:
	TSurface* iSurfacesIndex[KMaxLists]; 			// A table with elements pointing to the head of each singly linked list
	NFastMutex iMutex;						// Mutex to protect access to surface lists
	TProcessListItem* iConnectedProcesses;	//reference counted list of processes connected to the driver
	};


#endif





