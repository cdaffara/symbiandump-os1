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

#include <e32math.h>
#include "sgdriverimpl.h"
#include "sgimageimpl.h"
#include "sgimagecollectionimpl.h"


TInt SgAlignedDataStride(TInt aWidthInPixels, TUidPixelFormat aPixelFormat)
	{
#if defined(SYMBIAN_GRAPHICS_USE_MBX)
	// MBX requires 2^n stride. 
	for (TInt width = 8; width & KMaxTInt; width <<= 1)
		{
		if (width >= aWidthInPixels)
			{
			aWidthInPixels = width;
			break;
			}
		}
#elif defined(SYMBIAN_GRAPHICS_USE_SGX)
	// SGX requires a 32 pixel alignment. 
	aWidthInPixels = (aWidthInPixels + 31) & ~31;
#endif
	return Align4(SgMinDataStride(aWidthInPixels, aPixelFormat));
	}


TInt SgOffsetToFirstBuffer(TInt aMetaDataSize)
	{
#if defined(SYMBIAN_GRAPHICS_USE_MBX) || defined(SYMBIAN_GRAPHICS_USE_SGX)
	return (aMetaDataSize + 127) & ~127;
#else
	return Align4(aMetaDataSize);
#endif
	}


TInt SgOffsetBetweenBuffers(TInt aDataStride, TInt aScanLineCount)
	{
#if defined(SYMBIAN_GRAPHICS_USE_MBX)
	for (TInt count = 16; count & KMaxTInt; count <<= 1)
		{
		if (count >= aScanLineCount)
			{
			aScanLineCount = count;
			break;
			}
		}
#endif
	return aDataStride * aScanLineCount;
	}


TBool SgIsValidImageInfo(const TSgImageInfo& aInfo)
	{
	return aInfo.iSizeInPixels.iWidth > 0 && aInfo.iSizeInPixels.iHeight > 0
		&& (aInfo.iUsage & ESgUsageScreenSource ? aInfo.iScreenId >= KSgScreenIdMain : aInfo.iScreenId >= KSgScreenIdAny)
		&& (aInfo.iUserAttributes ? aInfo.iUserAttributeCount > 0 : aInfo.iUserAttributeCount == 0);
	}


TBool SgIsMutableImage(const TSgImageInfo& aInfo)
	{
	return (aInfo.iCpuAccess & ESgCpuAccessWritable) || (aInfo.iUsage & KSgUsageAllTargets);
	}


TBool SgIsCachedImage(const TSgImageInfo& aInfo)
	{
#ifdef SYMBIAN_GRAPHICS_USE_GPU
#ifdef SYMBIAN_GRAPHICS_USE_CACHE
	return aInfo.iCpuAccess != ESgCpuAccessNone;
#else
	(void)aInfo;
	return EFalse;
#endif
#else
	(void)aInfo;
	return ETrue;
#endif
	}


// TSgPixelFormatTableEntry

TBool TSgPixelFormatTableEntry::IsMatch(const TSgImageInfo& aInfo) const
	{
	return iPixelFormat == aInfo.iPixelFormat
		&& !(~iUsage & aInfo.iUsage)
		&& !(~iCpuAccess & aInfo.iCpuAccess)
		&& (iScreenId == KSgScreenIdAny || iScreenId == aInfo.iScreenId);
	}


TBool TSgPixelFormatTableEntry::IsMatchIgnoringPixelFormat(const TSgImageInfo& aInfo) const
	{
	return !(~iUsage & aInfo.iUsage)
		&& !(~iCpuAccess & aInfo.iCpuAccess)
		&& (iScreenId == KSgScreenIdAny || iScreenId == aInfo.iScreenId);
	}


TBool TSgPixelFormatTableEntry::IsMatchIgnoringUsage(const TSgImageInfo& aInfo) const
	{
	return iPixelFormat == aInfo.iPixelFormat
		&& !(~iCpuAccess & aInfo.iCpuAccess)
		&& (iScreenId == KSgScreenIdAny || iScreenId == aInfo.iScreenId);
	}


// XSgBase

XSgBase::~XSgBase()
	{
	__ASSERT_DEBUG(iDriverImpl.IsMutexHeld(), Panic(ESgPanicMutexNotHeld));
	__ASSERT_DEBUG(iRefCount == 0, Panic(ESgPanicBadReferenceCount));
	}


void XSgBase::Delete()
	{
	XSgDriverImpl& driverImpl = iDriverImpl;
	this->~XSgBase();
	driverImpl.Free(this);
	}


// XSgDriverImpl

TInt XSgDriverImpl::Construct()
	{
	TInt err = iMutex.CreateLocal();
	if (err != KErrNone)
		{
		return err;
		}
	err = iSurfaceManager.Open();
	if (err != KErrNone)
		{
		return err;
		}
	return ConstructPixelFormatTable();
	}


XSgDriverImpl::~XSgDriverImpl()
	{
	__ASSERT_DEBUG(iImages.Count() == 0, Panic(ESgPanicUnclosedResources));
	__ASSERT_DEBUG(iImageCollections.Count() == 0, Panic(ESgPanicUnclosedResources));
	iMutex.Close();
	iSurfaceManager.Close();
	RHeap* prevHeap = User::SwitchHeap(iHeap);
	iPixelFormatTable.Close();
	iLastPixelFormats.Close();
	User::SwitchHeap(prevHeap);
	}


void XSgDriverImpl::Delete()
	{
	RHeap* heap = iHeap;
	this->~XSgDriverImpl();
	heap->Free(this);
	__ASSERT_DEBUG(heap->Count() == 0, Panic(ESgPanicUnclosedResources));
	heap->Close();
	}


TInt XSgDriverImpl::MapSurface(const TSurfaceId& aSurfaceId, RChunk& aChunk)
	{
	RChunk chunk;
	TInt err = iSurfaceManager.MapSurface(aSurfaceId, chunk);
	if (err != KErrNone)
		{
		return err;
		}
	aChunk = chunk;
	err = aChunk.Duplicate(RThread()); // Get a process-wide handle
	chunk.Close();
	return err;
	}


TArray<TSgPixelFormatTableEntry> XSgDriverImpl::PixelFormatTable() const
	{
	return iPixelFormatTable.Array();
	}


TInt XSgDriverImpl::CanCreateImage(const TSgImageInfo& aInfo) const
	{
	__ASSERT_DEBUG(SgIsValidImageInfo(aInfo), Panic(ESgPanicBadImageInfo));
	if (aInfo.iSizeInPixels.iWidth > KMaxTInt16 / 2 || aInfo.iSizeInPixels.iHeight > KMaxTInt16 / 2)
		{
		return KErrTooBig;
		}
	TArray<TSgPixelFormatTableEntry> pixelFormatTable = PixelFormatTable();
	TInt n = pixelFormatTable.Count();
	for (TInt i = 0; i < n; ++i)
		{
		if (pixelFormatTable[i].IsMatch(aInfo))
			{
			return SgMinDataStride(aInfo.iSizeInPixels.iWidth, aInfo.iPixelFormat) > 0 ? KErrNone : KErrNotSupported;
			}
		}
	return KErrNotSupported;
	}


TInt XSgDriverImpl::GetPixelFormats(const TSgImageInfo& aInfo, TUidPixelFormat* aPixelFormats, TInt& aCount)
	{
	if (!SgIsValidImageInfo(aInfo) || (aPixelFormats && aCount < 0))
		{
		return KErrArgument;
		}
	iMutex.Wait();
	if (aInfo.iSizeInPixels != iLastSizeInPixels
		|| aInfo.iUsage != iLastUsage
		|| aInfo.iCpuAccess != iLastCpuAccess
		|| aInfo.iScreenId != iLastScreenId)
		{
		iLastSizeInPixels = aInfo.iSizeInPixels;
		iLastUsage = aInfo.iUsage;
		iLastCpuAccess = aInfo.iCpuAccess;
		iLastScreenId = aInfo.iScreenId;
		RHeap* prevHeap = User::SwitchHeap(iHeap);
		iLastPixelFormats.Reset();
		User::SwitchHeap(prevHeap);
		TArray<TSgPixelFormatTableEntry> pixelFormatTable = PixelFormatTable();
		TInt n = pixelFormatTable.Count();
		for (TInt i = 0; i < n; ++i)
			{
			const TSgPixelFormatTableEntry& entry = pixelFormatTable[i];
			if (entry.IsMatchIgnoringPixelFormat(aInfo) && iLastPixelFormats.Find(entry.iPixelFormat) == KErrNotFound)
				{
				User::SwitchHeap(iHeap);
				TInt err = iLastPixelFormats.Append(entry.iPixelFormat);
				User::SwitchHeap(prevHeap);
				if (err != KErrNone)
					{
					iLastSizeInPixels = TSize(0, 0);
					iLastUsage = ESgUsageNone;
					iLastCpuAccess = ESgCpuAccessNone;
					iLastScreenId = KSgScreenIdMain;
					User::SwitchHeap(iHeap);
					iLastPixelFormats.Reset();
					User::SwitchHeap(prevHeap);
					iMutex.Signal();
					return err;
					}
				}
			}
		}
	TInt err = KErrNone;
	if (aPixelFormats)
		{
		TInt n = Min(aCount, iLastPixelFormats.Count());
		for (TInt i = 0; i < n; ++i)
			{
			aPixelFormats[i] = iLastPixelFormats[i];
			}
		if (aCount < iLastPixelFormats.Count())
			{
			err = KErrOverflow;
			}
		}
	aCount = iLastPixelFormats.Count();
	iMutex.Signal();
	return err;
	}


TInt XSgDriverImpl::CreateImage(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride, MSgDrawableAdapter*& aResult)
	{
	if (!SgIsValidImageInfo(aInfo))
		{
		return KErrArgument;
		}
	if (aResult)
		{
		return KErrInUse;
		}
	if (!aDataAddress && !SgIsMutableImage(aInfo))
		{
		return KErrNoInitializationData;
		}
	TInt err = CanCreateImage(aInfo);
	if (err != KErrNone)
		{
		return err;
		}
#if !defined(SYMBIAN_GRAPHICS_USE_GPU) && !defined(__WINS__)
	if (!aInfo.iShareable)
		{
		iMutex.Wait();
		union
			{
			TSgDrawableId id;
			TSgImageId_SwLocal id_SwLocal;
			};
		do
			{
			id_SwLocal.iProcessId = RProcess().Id();
			id_SwLocal.iRandom[0] = Math::Random();
			id_SwLocal.iRandom[1] = Math::Random();
			id_SwLocal.iMinusOne = KErrNotFound;
			id_SwLocal.iFlags = 0;
			}
		while (iImages.FindInOrder(id, XSgImageImplBase::Compare) != KErrNotFound);
		XSgImageImpl_SwLocal* impl;
		err = XSgImageImpl_SwLocal::New(impl, *this, id, aInfo, aDataAddress, aDataStride);
		if (err != KErrNone)
			{
			iMutex.Signal();
			return err;
			}
		RHeap* prevHeap = User::SwitchHeap(iHeap);
		err = iImages.InsertInOrder(impl, XSgImageImplBase::Compare);
		User::SwitchHeap(prevHeap);
		if (err != KErrNone)
			{
			impl->Delete();
			iMutex.Signal();
			return err;
			}
		impl->IncRefCount();
		aResult = impl;
		iMutex.Signal();
		return KErrNone;
		}
#endif
	iMutex.Wait();
	XSgImageImpl_SurfaceManager* impl;
	err = XSgImageImpl_SurfaceManager::New(impl, *this, aInfo, SgIsCachedImage(aInfo), aDataAddress, aDataStride);
	if (err != KErrNone)
		{
		iMutex.Signal();
		return err;
		}
	RHeap* prevHeap = User::SwitchHeap(iHeap);
	err = iImages.InsertInOrder(impl, XSgImageImplBase::Compare);
	User::SwitchHeap(prevHeap);
	if (err != KErrNone)
		{
		impl->Delete();
		iMutex.Signal();
		return err;
		}
	impl->IncRefCount();
	aResult = impl;
	iMutex.Signal();
	return KErrNone;
	}


TInt XSgDriverImpl::CreateImage(const TSgImageInfo& aInfo, MSgImageAdapter* aImage, MSgDrawableAdapter*& aResult)
	{
	if (!aImage)
		{
		return KErrArgument;
		}
	__ASSERT_DEBUG(CheckImage(*aImage), Panic(ESgPanicBadImageHandle));
	XSgImageImplBase& impl = static_cast<XSgImageImplBase&>(*aImage);
	if (aInfo.iSizeInPixels != impl.MetaData().iSizeInPixels
		|| aInfo.iPixelFormat != impl.MetaData().iPixelFormat)
		{
		return KErrNotSupported;
		}
	TInt err = impl.BeginDataAccess(ESgCpuAccessReadOnly);
	if (err != KErrNone)
		{
		return err;
		}
	err = CreateImage(aInfo, impl.DataAddress(), impl.DataStride(), aResult);
	impl.EndDataAccess();
	return err;
	}


TInt XSgDriverImpl::CreateImageCollection(const TSgImageInfo& aInfo, TInt aImageCount, MSgImageCollectionAdapter*& aResult)
	{
	if (!SgIsValidImageInfo(aInfo) || aImageCount <= 0)
		{
		return KErrArgument;
		}
	if (aResult)
		{
		return KErrInUse;
		}
	if (!SgIsMutableImage(aInfo))
		{
		return KErrNotSupported;
		}
	TInt err = CanCreateImage(aInfo);
	if (err != KErrNone)
		{
		return err;
		}
	iMutex.Wait();
	XSgImageCollectionImpl* impl;
	const TInt stride = SgAlignedDataStride(aInfo.iSizeInPixels.iWidth, aInfo.iPixelFormat);
	const TInt offsetToFirst = SgOffsetToFirstBuffer(sizeof(TSgImageMetaData));
	const TInt offsetBetween = SgOffsetBetweenBuffers(stride, aInfo.iSizeInPixels.iHeight);
	
	err = XSgImageCollectionImpl::New(impl, *this, aInfo, aImageCount, SgIsCachedImage(aInfo), stride, offsetToFirst, offsetBetween);
	if (err != KErrNone)
		{
		iMutex.Signal();
		return err;
		}
	RHeap* prevHeap = User::SwitchHeap(iHeap);
	err = iImageCollections.InsertInAddressOrder(impl);
	User::SwitchHeap(prevHeap);
	if (err != KErrNone)
		{
		impl->Delete();
		iMutex.Signal();
		return err;
		}
	impl->IncRefCount();
	aResult = impl;
	iMutex.Signal();
	return KErrNone;
	}

TInt XSgDriverImpl::GetBufferOffset(const TSurfaceId& aSurfaceID, TInt aBuffer, TInt &aOffset)
	{
	return iSurfaceManager.GetBufferOffset(aSurfaceID,aBuffer,aOffset);
	}

TInt XSgDriverImpl::GetSurfaceManagerAttrib(RSurfaceManager::TSurfaceManagerAttrib aAttrib, TInt& aValue)
	{
	return iSurfaceManager.GetSurfaceManagerAttrib(aAttrib,aValue);
	}

TInt XSgDriverImpl::CreateImageCollections(const TSgImageInfo aInfos[], TInt aImageCount,
                                           MSgImageCollectionAdapter* aCollections[], TInt aCollectionCount)
	{
	if (aImageCount <= 0 || aCollectionCount <= 0)
		{
		return KErrArgument;
		}
	TBool isCached = EFalse;
	TInt offsetToFirstBuffer = SgOffsetToFirstBuffer(aCollectionCount * sizeof(TSgImageMetaData));
	TInt maxOffsetBetweenBuffers = 0;
	for (TInt i = 0; i < aCollectionCount; ++i)
		{
		const TSgImageInfo& info = aInfos[i];
		if (!SgIsValidImageInfo(info))
			{
			return KErrArgument;
			}
		if (aCollections[i])
			{
			return KErrInUse;
			}
		if (!SgIsMutableImage(info))
			{
			return KErrNotSupported;
			}
		TInt err = CanCreateImage(info);
		if (err != KErrNone)
			{
			return err;
			}
		if (SgIsCachedImage(info))
			{
			isCached = ETrue;
			}
		TInt stride = SgAlignedDataStride(info.iSizeInPixels.iWidth, info.iPixelFormat);
		TInt offsetBetweenBuffers = SgOffsetBetweenBuffers(stride, info.iSizeInPixels.iHeight);
		if (offsetBetweenBuffers > maxOffsetBetweenBuffers)
			{
			maxOffsetBetweenBuffers = offsetBetweenBuffers;
			}
		}
	iMutex.Wait();
	XSgImageCollectionImpl* firstImpl = NULL;
	for (TInt i = 0; i < aCollectionCount; ++i)
		{
		const TInt stride = SgAlignedDataStride(aInfos[i].iSizeInPixels.iWidth, aInfos[i].iPixelFormat);
		XSgImageCollectionImpl* impl;
		TInt err = XSgImageCollectionImpl::New(impl, *this, aInfos[i], aImageCount, isCached,
		                                       stride, offsetToFirstBuffer, maxOffsetBetweenBuffers, i, firstImpl);
		if (err == KErrNone)
			{
			if (i == 0)
				{
				firstImpl = impl;
				RSurfaceManager::TInfoBuf info;
				TSurfaceId surface=impl->SurfaceId();
				err = SurfaceInfo(surface, info);
				if (err == KErrNone)
				    {
				    // get the actual value used for offset to first buffer
				    err = iSurfaceManager.GetBufferOffset(surface, 0, offsetToFirstBuffer);
				    // get the actual value used for offset between buffers
				    if (aImageCount>1)
				    	{
				    	TInt offsetToSecondBuffer;
				    	err = iSurfaceManager.GetBufferOffset(surface, 1, offsetToSecondBuffer);
				    	maxOffsetBetweenBuffers = offsetToSecondBuffer - offsetToFirstBuffer;
				    	}
				    else
				    	{
				    	maxOffsetBetweenBuffers = 0;
				    	}
				    }       
				if (err != KErrNone)
				    {
				    impl->Delete();
				    iMutex.Signal();
				    return err;
				    } 					
				}
			RHeap* prevHeap = User::SwitchHeap(iHeap);
			err = iImageCollections.InsertInAddressOrder(impl);
			User::SwitchHeap(prevHeap);
			if (err == KErrNone)
				{
				impl->IncRefCount();
				aCollections[i] = impl;
				}
			else
				{
				impl->Delete();
				}
			}
		if (err != KErrNone)
			{
			while (--i >= 0)
				{
				aCollections[i]->Close();
				aCollections[i] = NULL;
				}
			iMutex.Signal();
			return err;
			}
		}
	iMutex.Signal();
	return KErrNone;
	}


TInt XSgDriverImpl::OpenDrawable(const TSgDrawableId& aId, TUint32 aMode, TUid aHandleType, MSgDrawableAdapter*& aResult)
	{
	if (aHandleType == KSgImageTypeUid || aHandleType == KSgDrawableTypeUid)
		{
		return OpenImage(aId, aMode, aResult);
		}
	return KErrNotSupported;
	}


TInt XSgDriverImpl::OpenImage(const TSgDrawableId& aId, TUint32 aMode, MSgDrawableAdapter*& aResult)
	{
	if (aResult)
		{
		return KErrInUse;
		}
	iMutex.Wait();
	TSgDrawableId id = aId;
	id.iId[KSgImageIdFlagsIndex] |= aMode;
	TInt i = iImages.FindInOrder(id, XSgImageImplBase::CompareIgnoringFlags);
	if (i != KErrNotFound)
		{
		XSgImageImplBase* impl;
		TInt j = iImages.FindInOrder(id, XSgImageImplBase::Compare);
		if (j != KErrNotFound)
			{
			impl = iImages[j];
			}
		else
			{
			impl = iImages[i];
#ifndef SYMBIAN_GRAPHICS_USE_GPU
			if (TSgImageId_SwLocal::IsMatch(id))
				{
				XSgImageImpl_SwLocal* impl2;
				TInt err = XSgImageImpl_SwLocal::New(impl2, *static_cast<XSgImageImpl_SwLocal*>(impl), id.iId[KSgImageIdFlagsIndex]);
				if (err != KErrNone)
					{
					iMutex.Signal();
					return err;
					}
				impl = impl2;
				}
			else
#endif
			if (TSgImageId_SurfaceManager::IsMatch(id))
				{
				XSgImageImpl_SurfaceManager* impl2;
				TInt err = XSgImageImpl_SurfaceManager::New(impl2, *static_cast<XSgImageImpl_SurfaceManager*>(impl), id.iId[KSgImageIdFlagsIndex]);
				if (err != KErrNone)
					{
					iMutex.Signal();
					return err;
					}
				impl = impl2;
				}
			else
				{
				iMutex.Signal();
				return KErrNotFound;
				}
			RHeap* prevHeap = User::SwitchHeap(iHeap);
			TInt err = iImages.InsertInOrder(impl, XSgImageImplBase::Compare);
			User::SwitchHeap(prevHeap);
			if (err != KErrNone)
				{
				impl->Delete();
				iMutex.Signal();
				return err;
				}
			}
		impl->IncRefCount();
		aResult = impl;
		iMutex.Signal();
		return KErrNone;
		}
	TInt err = KErrNotFound;
	if (TSgImageId_SurfaceManager::IsMatch(id))
		{
		XSgImageImpl_SurfaceManager* impl;
		err = XSgImageImpl_SurfaceManager::New(impl, *this, id);
		if (err != KErrNone)
			{
			iMutex.Signal();
			return err;
			}
		if (!impl->MetaData().iShareable && impl->MetaData().iCreatorProcess != RProcess().Id())
			{
			impl->Delete();
			iMutex.Signal();
			return KErrPermissionDenied;
			}
		RHeap* prevHeap = User::SwitchHeap(iHeap);
		err = iImages.InsertInOrder(impl, XSgImageImplBase::Compare);
		User::SwitchHeap(prevHeap);
		if (err != KErrNone)
			{
			impl->Delete();
			iMutex.Signal();
			return err;
			}
		impl->IncRefCount();
		aResult = impl;
		}
	iMutex.Signal();
	return err;
	}


void XSgDriverImpl::DeleteImage(XSgImageImplBase* aImage)
	{
	__ASSERT_DEBUG(iMutex.IsHeld(), Panic(ESgPanicMutexNotHeld));
	TInt i = iImages.FindInOrder(aImage, XSgImageImplBase::Compare);
	__ASSERT_DEBUG(i != KErrNotFound, Panic(ESgPanicBadImageHandle));
	RHeap* prevHeap = User::SwitchHeap(iHeap);
	iImages.Remove(i);
	iImages.GranularCompress();
	User::SwitchHeap(prevHeap);
	aImage->Delete();
	}


void XSgDriverImpl::DeleteImageCollection(XSgImageCollectionImpl* aImageCollection)
	{
	__ASSERT_DEBUG(iMutex.IsHeld(), Panic(ESgPanicMutexNotHeld));
	TInt i = iImageCollections.FindInAddressOrder(aImageCollection);
	__ASSERT_DEBUG(i != KErrNotFound, Panic(ESgPanicBadImageCollectionHandle));
	RHeap* prevHeap = User::SwitchHeap(iHeap);
	iImageCollections.Remove(i);
	iImageCollections.GranularCompress();
	User::SwitchHeap(prevHeap);
	aImageCollection->Delete();
	}


TBool XSgDriverImpl::CheckDrawable(const MSgResourceAdapter& aDrawable) const
	{
	iMutex.Wait();
	TInt i = iImages.Find(&static_cast<const XSgImageImplBase&>(aDrawable));
	iMutex.Signal();
	return i != KErrNotFound;
	}


TBool XSgDriverImpl::CheckImage(const MSgResourceAdapter& aImage) const
	{
	iMutex.Wait();
	TInt i = iImages.Find(&static_cast<const XSgImageImplBase&>(aImage));
	iMutex.Signal();
	return i != KErrNotFound;
	}


TBool XSgDriverImpl::CheckImageCollection(const MSgResourceAdapter& aImageCollection) const
	{
	iMutex.Wait();
	TInt i = iImageCollections.Find(&static_cast<const XSgImageCollectionImpl&>(aImageCollection));
	iMutex.Signal();
	return i != KErrNotFound;
	}


TInt XSgDriverImpl::ResourceCount() const
	{
	iMutex.Wait();
	TInt count = 0;
	for (TInt i = 0; i < iImages.Count(); ++i)
		{
		count += iImages[i]->RefCount();
		}
	for (TInt i = 0; i < iImageCollections.Count(); ++i)
		{
		count += iImageCollections[i]->RefCount();
		}
	iMutex.Signal();
	return count;
	}


#ifdef _DEBUG

void XSgDriverImpl::AllocMarkStart()
	{
	iMutex.Wait();
	iHeap->__DbgMarkStart();
	iMutex.Signal();
	}


void XSgDriverImpl::AllocMarkEnd(TInt aCount)
	{
	iMutex.Wait();
	TUint32 badCell = iHeap->__DbgMarkEnd(aCount);
	iMutex.Signal();
	if (badCell != 0)
		{
		_LIT(KPanicCategoryFormat, "SGALLOC:%08x");
		TBuf<0x10> category;
		category.Format(KPanicCategoryFormat, badCell);
		User::Panic(category, 0);
		}
	}


void XSgDriverImpl::SetAllocFail(RAllocator::TAllocFail aType, TInt aRate)
	{
	iMutex.Wait();
	iHeap->__DbgSetAllocFail(aType, aRate);
	iMutex.Signal();
	}

#else

void XSgDriverImpl::AllocMarkStart()
	{
	}


void XSgDriverImpl::AllocMarkEnd(TInt /*aCount*/)
	{
	}


void XSgDriverImpl::SetAllocFail(RAllocator::TAllocFail /*aType*/, TInt /*aRate*/)
	{
	}

#endif


// MSgDriverAdapter::New()

EXPORT_C TInt MSgDriverAdapter::New(MSgDriverAdapter*& aPtr)
	{
	RHeap* heap = UserHeap::ChunkHeap(NULL, 0, KSgMaxLocalChunkSize);
	if (!heap)
		{
		return KErrNoMemory;
		}
	XSgDriverImpl* driverImpl = static_cast<XSgDriverImpl*>(heap->Alloc(sizeof(XSgDriverImpl)));
	if (!driverImpl)
		{
		heap->Close();
		return KErrNoMemory;
		}
	new(driverImpl) XSgDriverImpl(heap);
	TInt err = driverImpl->Construct();
	if (err != KErrNone)
		{
		driverImpl->Delete();
		return err;
		}
	aPtr = driverImpl;
	return KErrNone;
	}
