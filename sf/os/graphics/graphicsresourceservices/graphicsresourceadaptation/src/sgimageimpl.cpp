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

#include "sgimageimpl.h"
#include "sgresourceinternal.h"


// TSgImageMetaData

TSgImageMetaData::TSgImageMetaData(const TSgImageInfo& aInfo, TArray<TSgPixelFormatTableEntry> aPixelFormatTable, TBool aIsCached)
	: iCreatorProcess(RProcess().Id()),
	  iSizeInPixels(aInfo.iSizeInPixels),
	  iPixelFormat(aInfo.iPixelFormat),
	  iRequestedUsage(aInfo.iUsage),
	  iPotentialUsage(0),
	  iShareable(aInfo.iShareable),
	  iCpuAccess(aInfo.iCpuAccess),
	  iScreenId(aInfo.iScreenId),
	  iIsCached(aIsCached)
	{
	TUint32 usageMask = (aInfo.iUsage & KSgUsageAllSources ? KSgUsageAllSources : 0)
	                    | (aInfo.iUsage & KSgUsageAllTargets ? KSgUsageAllTargets : 0);
	TInt n = aPixelFormatTable.Count();
	for (TInt i = 0; i < n; ++i)
		{
		const TSgPixelFormatTableEntry& entry = aPixelFormatTable[i];
		if (entry.IsMatchIgnoringUsage(aInfo))
			{
			iPotentialUsage |= entry.iUsage & usageMask;
			}
		}
	}


void TSgImageMetaData::GetInfo(TSgImageInfo& aInfo, TBool aGetPotentialUsage) const
	{
	aInfo.iSizeInPixels = iSizeInPixels;
	aInfo.iPixelFormat = iPixelFormat;
	aInfo.iUsage = aGetPotentialUsage ? iPotentialUsage : iRequestedUsage;
	aInfo.iShareable = iShareable;
	aInfo.iCpuAccess = iCpuAccess;
	aInfo.iScreenId = iScreenId;
	}


// XSgImageImplBase

XSgImageImplBase::XSgImageImplBase(const XSgImageImplBase& aImage, TUint32 aFlags)
	: XSgBase(aImage.iDriverImpl)
	{
	__ASSERT_DEBUG(iDriverImpl.IsMutexHeld(), Panic(ESgPanicMutexNotHeld));
	iId = aImage.iId;
	iId.iId[KSgImageIdFlagsIndex] = aFlags;
	aImage.iState->IncRefCount();
	iState = aImage.iState;
	}


XSgImageImplBase::~XSgImageImplBase()
	{
	__ASSERT_DEBUG(iDriverImpl.IsMutexHeld(), Panic(ESgPanicMutexNotHeld));
	if (iState && iState->DecRefCount() == 0)
		{
		Unmap();
		iState->Delete();
		}
	}


TInt XSgImageImplBase::Compare(const TSgDrawableId* aId, const XSgImageImplBase& aImage)
	{
	return Mem::Compare(reinterpret_cast<const TUint8*>(aId), sizeof(TSgDrawableId),
	                    reinterpret_cast<const TUint8*>(&aImage.Id()), sizeof(TSgDrawableId));
	}


TInt XSgImageImplBase::Compare(const XSgImageImplBase& aImage1, const XSgImageImplBase& aImage2)
	{
	return Compare(&aImage1.Id(), aImage2);
	}


TInt XSgImageImplBase::CompareIgnoringFlags(const TSgDrawableId* aId, const XSgImageImplBase& aImage)
	{
	return Mem::Compare(reinterpret_cast<const TUint8*>(aId), sizeof(TSgDrawableId) - sizeof(TUint32),
	                    reinterpret_cast<const TUint8*>(&aImage.Id()), sizeof(TSgDrawableId) - sizeof(TUint32));
	}


void XSgImageImplBase::Close()
	{
	XSgDriverImpl& driverImpl = iDriverImpl;
	driverImpl.Wait();
	if (DecRefCount() == 0)
		{
		driverImpl.DeleteImage(this);
		}
	driverImpl.Signal();
	}


const TSgDrawableId& XSgImageImplBase::Id() const
	{
	return iId;
	}


TUid XSgImageImplBase::DrawableType() const
	{
	return KSgImageTypeUid;
	}


TInt XSgImageImplBase::GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr)
	{
	if (aInterfaceUid.iUid == MSgImage_Sw::EInterfaceUid)
		{
		aInterfacePtr = static_cast<MSgImage_Sw*>(this);
		return KErrNone;
		}
	return KErrExtensionNotSupported;
	}


TInt XSgImageImplBase::GetInfo(TSgImageInfo& aInfo) const
	{
	iState->MetaData().GetInfo(aInfo, iId.iId[KSgImageIdFlagsIndex] & ESgDoNotRestrictUsage);
	return iState->GetUserAttributes(aInfo.iUserAttributes, aInfo.iUserAttributeCount);
	}


TInt XSgImageImplBase::MapReadOnly(const TAny*& aDataAddress, TInt& aDataStride)
	{
	TInt err = iState->BeginDataAccess(ESgCpuAccessReadOnly, ETrue);
	if (err != KErrNone)
		{
		return err;
		}
	aDataAddress = iState->DataAddress();
	aDataStride = iState->DataStride();
	return KErrNone;
	}


TInt XSgImageImplBase::MapWriteOnly(TAny*& aDataAddress, TInt& aDataStride)
	{
	TInt err = iState->BeginDataAccess(ESgCpuAccessWriteOnly, ETrue);
	if (err != KErrNone)
		{
		return err;
		}
	aDataAddress = iState->DataAddress();
	aDataStride = iState->DataStride();
	return KErrNone;
	}


TInt XSgImageImplBase::MapReadWrite(TAny*& aDataAddress, TInt& aDataStride)
	{
	TInt err = iState->BeginDataAccess(ESgCpuAccessReadWrite, ETrue);
	if (err != KErrNone)
		{
		return err;
		}
	aDataAddress = iState->DataAddress();
	aDataStride = iState->DataStride();
	return KErrNone;
	}


TInt XSgImageImplBase::Unmap()
	{
	return iState->EndDataAccess(ETrue);
	}


TAny* XSgImageImplBase::DataAddress() const
	{
	return iState->DataAddress();
	}


TInt XSgImageImplBase::DataStride() const
	{
	return iState->DataStride();
	}


TInt XSgImageImplBase::BeginDataAccess(TSgCpuAccess aCpuAccess)
	{
	return iState->BeginDataAccess(aCpuAccess, EFalse);
	}


TInt XSgImageImplBase::EndDataAccess()
	{
	return iState->EndDataAccess(EFalse);
	}


TInt XSgImageImplBase::SetData(const TAny* aDataAddress, TInt aDataStride)
	{
	if (aDataAddress)
		{
		TInt err = BeginDataAccess(ESgCpuAccessWriteOnly);
		if (err != KErrNone)
			{
			return err;
			}
		const TSgImageMetaData& metaData = iState->MetaData();
		const TAny* src = aDataAddress;
		TAny* trg = iState->DataAddress();
		TInt dataStride = iState->DataStride();
		TInt minDataStride = SgMinDataStride(metaData.iSizeInPixels.iWidth, metaData.iPixelFormat);
		__ASSERT_DEBUG(minDataStride > 0, Panic(ESgPanicResourceAdapterGeneral));
		for (TInt y = 0; y < metaData.iSizeInPixels.iHeight; ++y)
			{
			Mem::Copy(trg, src, minDataStride);
			src = PtrAdd(src, aDataStride);
			trg = PtrAdd(trg, dataStride);
			}
		EndDataAccess();
		}
	return KErrNone;
	}


TInt XSgImageStateBase::BeginDataAccess(TSgCpuAccess aCpuAccess, TBool aIsUserAccess)
	{
	if (aCpuAccess == ESgCpuAccessNone)
		{
		return KErrArgument;
		}
	const TSgImageMetaData& metaData = MetaData();
	if (aIsUserAccess && (~metaData.iCpuAccess & aCpuAccess))
		{
		return KErrAccessDenied;
		}
	if (aIsUserAccess && metaData.iCreatorProcess != RProcess().Id())
		{
		return KErrPermissionDenied;
		}
	iDriverImpl.Wait();
	if (iCpuAccess != ESgCpuAccessNone)
		{
		iDriverImpl.Signal();
		return KErrInUse;
		}
	iCpuAccess = aCpuAccess;
	iIsUserAccess = aIsUserAccess;
	iDriverImpl.Signal();
	return KErrNone;
	}


TInt XSgImageStateBase::EndDataAccess(TBool aIsUserAccess)
	{
	iDriverImpl.Wait();
	if (iCpuAccess == ESgCpuAccessNone || iIsUserAccess != aIsUserAccess)
		{
		iDriverImpl.Signal();
		return KErrGeneral;
		}
	iCpuAccess = ESgCpuAccessNone;
	iIsUserAccess = EFalse;
	iDriverImpl.Signal();
	return KErrNone;
	}


#ifndef SYMBIAN_GRAPHICS_USE_GPU

// XSgImageImpl_SwLocal

TInt XSgImageImpl_SwLocal::New(XSgImageImpl_SwLocal*& aPtr, XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId,
	                           const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride)
	{
	aPtr = static_cast<XSgImageImpl_SwLocal*>(aDriverImpl.Alloc(sizeof(XSgImageImpl_SwLocal)));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageImpl_SwLocal(aDriverImpl, aId);
	TInt err = aPtr->Construct(aInfo, aDataAddress, aDataStride);
	if (err != KErrNone)
		{
		aPtr->Delete();
		aPtr = NULL;
		}
	return err;
	}


TInt XSgImageImpl_SwLocal::New(XSgImageImpl_SwLocal*& aPtr, const XSgImageImpl_SwLocal& aImage, TUint32 aFlags)
	{
	aPtr = static_cast<XSgImageImpl_SwLocal*>(aImage.iDriverImpl.Alloc(sizeof(XSgImageImpl_SwLocal)));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageImpl_SwLocal(aImage, aFlags);
	return KErrNone;
	}


TInt XSgImageImpl_SwLocal::Construct(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride)
	{
	XSgImageState_SwLocal* state;
	TInt err = XSgImageState_SwLocal::New(state, iDriverImpl, aInfo);
	if (err != KErrNone)
		{
		return err;
		}
	state->IncRefCount();
	iState = state;
	return SetData(aDataAddress, aDataStride);
	}


// XSgImageState_SwLocal

TInt XSgImageState_SwLocal::New(XSgImageState_SwLocal*& aPtr, XSgDriverImpl& aDriverImpl, const TSgImageInfo& aInfo)
	{
	TInt dataStride = Align4(SgMinDataStride(aInfo.iSizeInPixels.iWidth, aInfo.iPixelFormat));
	TInt size = _FOFF(XSgImageState_SwLocal, iUserAttributes) + aInfo.iUserAttributeCount * sizeof(TSgUserAttribute) + dataStride * aInfo.iSizeInPixels.iHeight;
	aPtr = static_cast<XSgImageState_SwLocal*>(aDriverImpl.Alloc(size));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageState_SwLocal(aDriverImpl, aInfo, dataStride);
	return KErrNone;
	}


XSgImageState_SwLocal::XSgImageState_SwLocal(XSgDriverImpl& aDriverImpl, const TSgImageInfo& aInfo, TInt aDataStride)
	: XSgImageStateBase(aDriverImpl, aDataStride), iMetaData(aInfo, aDriverImpl.PixelFormatTable())
	{
	iUserAttributeCount = aInfo.iUserAttributeCount;
	Mem::Copy(iUserAttributes, aInfo.iUserAttributes, aInfo.iUserAttributeCount * sizeof(TSgUserAttribute));
	}


const TSgImageMetaData& XSgImageState_SwLocal::MetaData() const
	{
	return iMetaData;
	}


TInt XSgImageState_SwLocal::GetUserAttributes(TSgUserAttribute* aUserAttributes, TInt aUserAttributeCount) const
	{
	for (TInt i = 0; i < aUserAttributeCount; ++i)
		{
		TBool found = EFalse;
		for (TInt j = 0; j < iUserAttributeCount; ++j)
			{
			if (aUserAttributes[i].iUid == iUserAttributes[j].iUid)
				{
				aUserAttributes[i].iValue = iUserAttributes[j].iValue;
				found = ETrue;
				break;
				}
			}
		if (!found)
			{
			return KErrNotFound;
			}
		}
	return KErrNone;
	}


TAny* XSgImageState_SwLocal::DataAddress() const
	{
	//__ASSERT_DEBUG(iCpuAccess != ESgCpuAccessNone, ::Panic(ESgPanicNoCpuAccess));
	return (TAny*)PtrAdd(iUserAttributes, iUserAttributeCount * sizeof(TSgUserAttribute));
	}

#endif


// XSgImageImpl_SurfaceManager


TInt XSgImageImpl_SurfaceManager::New(XSgImageImpl_SurfaceManager*& aPtr, XSgDriverImpl& aDriverImpl,
                                      const TSgImageInfo& aInfo, TBool aIsCached, const TAny* aDataAddress, TInt aDataStride)
	{
	aPtr = static_cast<XSgImageImpl_SurfaceManager*>(aDriverImpl.Alloc(sizeof(XSgImageImpl_SurfaceManager)));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageImpl_SurfaceManager(aDriverImpl);
	TInt err = aPtr->Construct(aInfo, aIsCached, aDataAddress, aDataStride);
	if (err != KErrNone)
		{
		aPtr->Delete();
		aPtr = NULL;
		}
	return err;
	}


TInt XSgImageImpl_SurfaceManager::New(XSgImageImpl_SurfaceManager*& aPtr, XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId)
	{
	aPtr = static_cast<XSgImageImpl_SurfaceManager*>(aDriverImpl.Alloc(sizeof(XSgImageImpl_SurfaceManager)));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageImpl_SurfaceManager(aDriverImpl, aId);
	TInt err = aPtr->Construct(aId);
	if (err != KErrNone)
		{
		aPtr->Delete();
		aPtr = NULL;
		}
	return err;
	}


TInt XSgImageImpl_SurfaceManager::New(XSgImageImpl_SurfaceManager*& aPtr, const XSgImageImpl_SurfaceManager& aImage, TUint32 aFlags)
	{
	aPtr = static_cast<XSgImageImpl_SurfaceManager*>(aImage.iDriverImpl.Alloc(sizeof(XSgImageImpl_SurfaceManager)));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageImpl_SurfaceManager(aImage, aFlags);
	return KErrNone;
	}


TInt XSgImageImpl_SurfaceManager::Construct(const TSgImageInfo& aInfo, TBool aIsCached, const TAny* aDataAddress, TInt aDataStride)
	{
	XSgImageState_SurfaceManager* state;
	TInt err = XSgImageState_SurfaceManager::New(state, iDriverImpl, aInfo, aIsCached);
	if (err != KErrNone)
		{
		return err;
		}
	Mem::Copy(&iId, &state->SurfaceId(), sizeof(TSurfaceId));
	state->IncRefCount();
	iState = state;
	return SetData(aDataAddress, aDataStride);
	}


TInt XSgImageImpl_SurfaceManager::Construct(const TSgDrawableId& aId)
	{
	XSgImageState_SurfaceManager* state;
	TInt err = XSgImageState_SurfaceManager::New(state, iDriverImpl, aId);
	if (err != KErrNone)
		{
		return err;
		}
	state->IncRefCount();
	iState = state;
	return KErrNone;
	}


TInt XSgImageImpl_SurfaceManager::GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr)
	{
	if (aInterfaceUid.iUid == MSgImage_Chunk::EInterfaceUid)
		{
		aInterfacePtr = static_cast<MSgImage_Chunk*>(this);
		return KErrNone;
		}
	return XSgImageImplBase::GetInterface(aInterfaceUid, aInterfacePtr);
	}


const RChunk& XSgImageImpl_SurfaceManager::DataChunk() const
	{
	return static_cast<XSgImageState_SurfaceManager*>(iState)->DataChunk();
	}


TInt XSgImageImpl_SurfaceManager::DataOffset() const
	{
	return static_cast<XSgImageState_SurfaceManager*>(iState)->DataOffset();
	}


TInt XSgImageImpl_SurfaceManager::DataStride() const
	{
	return iState->DataStride();
	}


// XSgImageState_SurfaceManager

TInt XSgImageState_SurfaceManager::New(XSgImageState_SurfaceManager*& aPtr, XSgDriverImpl& aDriverImpl, const TSgImageInfo& aInfo, TBool aIsCached)
	{
	aPtr = static_cast<XSgImageState_SurfaceManager*>(aDriverImpl.Alloc(sizeof(XSgImageState_SurfaceManager)));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageState_SurfaceManager(aDriverImpl);
	TInt err = aPtr->Construct(aInfo, aIsCached);
	if (err != KErrNone)
		{
		aPtr->Delete();
		aPtr = NULL;
		}
	return err;
	}


TInt XSgImageState_SurfaceManager::New(XSgImageState_SurfaceManager*& aPtr, XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId)
	{
	aPtr = static_cast<XSgImageState_SurfaceManager*>(aDriverImpl.Alloc(sizeof(XSgImageState_SurfaceManager)));
	if (!aPtr)
		{
		return KErrNoMemory;
		}
	new(aPtr) XSgImageState_SurfaceManager(aDriverImpl);
	TInt err = aPtr->Construct(aId);
	if (err != KErrNone)
		{
		aPtr->Delete();
		aPtr = NULL;
		}
	return err;
	}


TInt XSgImageState_SurfaceManager::Construct(const TSgImageInfo& aInfo, TBool aIsCached)
	{
	TInt maxNumberOfHints;
	TInt err;
	err=iDriverImpl.GetSurfaceManagerAttrib(RSurfaceManager::EMaxNumberOfHints,maxNumberOfHints);
	if (err!=KErrNone)
		{
		return err;
		}
	
	if (aInfo.iUserAttributeCount > maxNumberOfHints)
		{
		return KErrOverflow;
		}
	RSurfaceManager::THintPair* hints = new RSurfaceManager::THintPair[aInfo.iUserAttributeCount];
	if(!hints)
		{
		return KErrNoMemory;
		}
	RSurfaceManager::TSurfaceCreationAttributesBuf reqs;
	reqs().iSurfaceHints = hints;
	reqs().iHintCount = aInfo.iUserAttributeCount;
	reqs().iSize = aInfo.iSizeInPixels;
	reqs().iBuffers = 1;
	reqs().iPixelFormat = aInfo.iPixelFormat;
	reqs().iStride = SgAlignedDataStride(aInfo.iSizeInPixels.iWidth, aInfo.iPixelFormat);
	reqs().iOffsetToFirstBuffer = SgOffsetToFirstBuffer(sizeof(TSgImageMetaData));
	reqs().iAlignment = 4;
	reqs().iContiguous = EFalse;
	reqs().iCacheAttrib = aIsCached ? RSurfaceManager::ECached : RSurfaceManager::ENotCached;
	reqs().iOffsetBetweenBuffers = SgOffsetBetweenBuffers(reqs().iStride, aInfo.iSizeInPixels.iHeight);
	reqs().iMappable = ETrue;
	for (TInt i = 0; i < aInfo.iUserAttributeCount; ++i)
		{
		reqs().iSurfaceHints[i].iKey = aInfo.iUserAttributes[i].iUid;
		reqs().iSurfaceHints[i].iValue = aInfo.iUserAttributes[i].iValue;
		reqs().iSurfaceHints[i].iMutable = EFalse;
		}
	TSurfaceId surfaceId;
	err = iDriverImpl.CreateSurface(reqs, surfaceId);
	delete[] hints;
	hints = NULL;
	reqs().iSurfaceHints = NULL;
	if (err != KErrNone)
		{
		return err;
		}
	iSurfaceId = surfaceId;
	RSurfaceManager::TInfoBuf info;
	err = iDriverImpl.SurfaceInfo(surfaceId, info);
	if (err != KErrNone)
		{
		return err;
		}
	err=iDriverImpl.GetBufferOffset(surfaceId,0,iDataOffset);	
	if (err != KErrNone)
		{
		return err;
		}	
	iDataStride = info().iStride;
	RChunk chunk;
	err = iDriverImpl.MapSurface(surfaceId, chunk);
	if (err != KErrNone)
		{
		return err;
		}
	iDataChunk = chunk;
	new(chunk.Base()) TSgImageMetaData(aInfo, iDriverImpl.PixelFormatTable(), aIsCached);
	return err;
	}


TInt XSgImageState_SurfaceManager::Construct(const TSgDrawableId& aId)
	{
	const TSgImageId_SurfaceManager& id_SurfaceManager = reinterpret_cast<const TSgImageId_SurfaceManager&>(aId);
	if (id_SurfaceManager.iBufferIndex < 0)
		{
		return KErrNotFound;
		}
	if (id_SurfaceManager.iMetaDataIndex < 0)
		{
		return KErrNotFound;
		}
	TInt err = iDriverImpl.OpenSurface(id_SurfaceManager.iSurfaceId);
	if (err != KErrNone)
		{
		return err;
		}
	iSurfaceId = id_SurfaceManager.iSurfaceId;
	RSurfaceManager::TInfoBuf info;
	err = iDriverImpl.SurfaceInfo(id_SurfaceManager.iSurfaceId, info);
	if (err != KErrNone)
		{
		return err;
		}
	iDataStride = info().iStride;
	if (id_SurfaceManager.iBufferIndex >= info().iBuffers)
		{
		return KErrNotFound;
		}
	iBufferIndex = id_SurfaceManager.iBufferIndex;
	TInt offsetToFirstBuffer;
	err=iDriverImpl.GetBufferOffset(id_SurfaceManager.iSurfaceId,0,offsetToFirstBuffer);
	if (err != KErrNone)
		{
		return err;
		}	
	if ((id_SurfaceManager.iMetaDataIndex + 1) * sizeof(TSgImageMetaData) > offsetToFirstBuffer)
		{
		return KErrNotFound;
		}
	iMetaDataOffset = id_SurfaceManager.iMetaDataIndex * sizeof(TSgImageMetaData);
	RChunk chunk;
	err = iDriverImpl.MapSurface(id_SurfaceManager.iSurfaceId, chunk);
	if (err != KErrNone)
		{
		return err;
		}
	iDataChunk = chunk;
	err=iDriverImpl.GetBufferOffset(id_SurfaceManager.iSurfaceId,id_SurfaceManager.iBufferIndex,iDataOffset);	
	return err;
	}


XSgImageState_SurfaceManager::~XSgImageState_SurfaceManager()
	{
	if (!iSurfaceId.IsNull())
		{
		iDriverImpl.CloseSurface(iSurfaceId);
		iDataChunk.Close();
		}
	}


const TSgImageMetaData& XSgImageState_SurfaceManager::MetaData() const
	{
	return *reinterpret_cast<TSgImageMetaData*>(iDataChunk.Base() + iMetaDataOffset);
	}


TInt XSgImageState_SurfaceManager::GetUserAttributes(TSgUserAttribute* aUserAttributes, TInt aUserAttributeCount) const
	{
	for (TInt i = 0; i < aUserAttributeCount; ++i)
		{
		RSurfaceManager::THintPair hint;
		hint.iKey = aUserAttributes[i].iUid;
		TInt err = iDriverImpl.GetSurfaceHint(iSurfaceId, hint);
		if (err != KErrNone)
			{
			return err;
			}
		aUserAttributes[i].iValue = hint.iValue;
		}
	return KErrNone;
	}


TAny* XSgImageState_SurfaceManager::DataAddress() const
	{
#ifdef SYMBIAN_GRAPHICS_AUTOFLUSH_CACHE
	__ASSERT_DEBUG(iCpuAccess != ESgCpuAccessNone, ::Panic(ESgPanicNoCpuAccess));
#endif
	return iDataChunk.Base() + iDataOffset;
	}


#ifdef SYMBIAN_GRAPHICS_AUTOFLUSH_CACHE

TInt XSgImageState_SurfaceManager::BeginDataAccess(TSgCpuAccess aCpuAccess, TBool aIsUserAccess)
	{
	TInt err = XSgImageStateBase::BeginDataAccess(aCpuAccess, aIsUserAccess);
	if (err != KErrNone)
		{
		return err;
		}
	if (aCpuAccess != ESgCpuAccessWriteOnly && MetaData().iIsCached && (MetaData().iPotentialUsage & KSgUsageAllTargets))
		{
		// Cache::SyncMemoryAfterDmaRead() cannot fail so the following should not fail if the arguments are correct
		err = iDriverImpl.SynchronizeCache(iSurfaceId, iBufferIndex, RSurfaceManager::ESyncAfterNonCPUWrite);
		__ASSERT_DEBUG(err == KErrNone, Panic(ESgPanicResourceAdapterGeneral));
		}
	return KErrNone;
	}


TInt XSgImageState_SurfaceManager::EndDataAccess(TBool aIsUserAccess)
	{
	TSgCpuAccess prevCpuAccess = iCpuAccess;
	TInt err = XSgImageStateBase::EndDataAccess(aIsUserAccess);
	if (err != KErrNone)
		{
		return err;
		}
	if (prevCpuAccess != ESgCpuAccessReadOnly && MetaData().iIsCached)
		{
		// Cache::SyncMemoryBeforeDmaWrite() cannot fail so the following should not fail if the arguments are correct
		err = iDriverImpl.SynchronizeCache(iSurfaceId, iBufferIndex, RSurfaceManager::ESyncBeforeNonCPURead);
		__ASSERT_DEBUG(err == KErrNone, Panic(ESgPanicResourceAdapterGeneral));
		}
	return KErrNone;
	}

#endif
