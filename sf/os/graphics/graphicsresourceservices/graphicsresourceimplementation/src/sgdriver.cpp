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
// Graphics Resource - driver implementation
//

#include "sgdriver.h"

const TInt KSgMaxLocalChunkSize = 0x100000;
const TInt KSgLocalChunkAlignment = 0x40;

EXPORT_C TInt RSgDriver::Open()
	{
	if (iImpl)
		{
		return KErrInUse;
		}
	if (gPls.iError != KErrNone)
		{
		return gPls.iError;
		}
	gPls.iMutex.Wait();
	if (gPls.iDriver)
		{
		++gPls.iOpenCount;
		iImpl = gPls.iDriver;
		gPls.iMutex.Signal();
		return KErrNone;
		}
	RChunk chunk;
	TInt err = chunk.CreateLocal(KMinHeapGrowBy, KSgMaxLocalChunkSize, EOwnerProcess);
	if (err != KErrNone)
		{
		gPls.iMutex.Signal();
		return err;
		}
	RHeap* heap = UserHeap::ChunkHeap(chunk, 0, KMinHeapGrowBy, KSgMaxLocalChunkSize, KSgLocalChunkAlignment, ETrue);
	__ASSERT_DEBUG(heap, Panic(ESgPanicResourceImplGeneral));
	XSgDriver* driver = static_cast<XSgDriver*>(heap->Alloc(sizeof(XSgDriver)));
	__ASSERT_DEBUG(driver, Panic(ESgPanicResourceImplGeneral));
	new(driver) XSgDriver(heap);
	err = driver->Construct();
	if (err != KErrNone)
		{
		driver->Delete();
		gPls.iMutex.Signal();
		return err;
		}
	gPls.iOpenCount = 1;
	iImpl = gPls.iDriver = driver;
	gPls.iMutex.Signal();
	return KErrNone;
	}

EXPORT_C void RSgDriver::Close()
	{
	if (iImpl)
		{
		__ASSERT_DEBUG(gPls.iError == KErrNone, Panic(ESgPanicResourceImplGeneral));
		gPls.iMutex.Wait();
		__ASSERT_DEBUG(gPls.iOpenCount > 0, Panic(ESgPanicResourceImplGeneral));
		if (--gPls.iOpenCount == 0)
			{
			gPls.iDriver->Delete();
			gPls.iDriver = NULL;
			}
		iImpl = NULL;
		gPls.iMutex.Signal();
		}
	}

EXPORT_C TInt RSgDriver::GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr) const
	{
	aInterfacePtr = NULL;
	if (!iImpl)
		{
		return KErrBadHandle;
		}
	return static_cast<XSgDriver*>(iImpl)->GetInterface(aInterfaceUid, aInterfacePtr);
	}

EXPORT_C TVersion RSgDriver::Version()
	{
	return TVersion(1, 1, 1);
	}

_LIT(KSgResourceImplPanicCategory, "SGRES-IMPL");

void Panic(TSgResourceImplPanicReason aReason)
	{
	User::Panic(KSgResourceImplPanicCategory, aReason);
	}

#ifdef SYMBIAN_GRAPHICS_USE_GPU
const TUint32 KSgResourceAttributes = 0;
#else
const TUint32 KSgResourceAttributes = ESgResourceAttrCpuCached;
#endif

LOCAL_C TInt SgMinDataStride(TInt aWidthInPixels, TInt aPixelFormat)
	{
	switch (aPixelFormat)
		{
	case ESgPixelFormatA_8:
		return aWidthInPixels;
	case ESgPixelFormatRGB_565:
		return aWidthInPixels << 1;
	case ESgPixelFormatXRGB_8888:
	case ESgPixelFormatARGB_8888:
	case ESgPixelFormatARGB_8888_PRE:
		return aWidthInPixels << 2;
	default:
#ifdef _DEBUG
		Panic(ESgPanicResourceImplGeneral);
#endif
		return 0;
		}
	}

LOCAL_C TInt SgAlignedDataStride(TInt aWidthInPixels, TInt aPixelFormat)
	{
#if defined(SYMBIAN_GRAPHICS_USE_MBX)
	// MBX requires 2^n stride
	for (TInt width = 8; width & KMaxTInt; width <<= 1)
		{
		if (width >= aWidthInPixels)
			{
			aWidthInPixels = width;
			break;
			}
		}
#elif defined(SYMBIAN_GRAPHICS_USE_SGX)
	// SGX requires 32-pixel alignment
	aWidthInPixels = (aWidthInPixels + 31) & ~31;
#endif
	return Align4(SgMinDataStride(aWidthInPixels, aPixelFormat));
	}

XSgDriverPls::XSgDriverPls()
	{
	iError = iMutex.CreateLocal();
	iOpenCount = 0;
	iDriver = NULL;
	}

XSgDriver::XSgDriver(RHeap* aHeap)
	: iHeap(aHeap), iHasOpenVg(EFalse), iHasOpenGles(EFalse), iHasOpenGles2(EFalse)
	{
	}

XSgDriver::~XSgDriver()
	{
	iMutex.Close();
	iDevice.Close();
	__ASSERT_DEBUG(iImagesByAddress.Count() == 0, Panic(ESgPanicUnclosedResources));
	__ASSERT_DEBUG(iImagesById.Count() == 0, Panic(ESgPanicUnclosedResources));
	}

TInt XSgDriver::Construct()
	{
	TInt err = iMutex.CreateLocal();
	if (err != KErrNone)
		{
		return err;
		}
	err = User::LoadLogicalDevice(KSgDeviceName);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		return err;
		}
	err = iDevice.Connect();
	if (err != KErrNone)
		{
		return err;
		}
	_LIT(KLibOpenVg, "libOpenVG.dll");
	_LIT(KLibOpenGles, "libGLESv1_CM.dll");
	_LIT(KLibOpenGles2, "libGLESv2.dll");
	RLibrary lib;
	if (lib.Load(KLibOpenVg) == KErrNone)
		{
		lib.Close();
		iHasOpenVg = ETrue;
		}
	if (lib.Load(KLibOpenGles) == KErrNone)
		{
		lib.Close();
		iHasOpenGles = ETrue;
		}
	if (lib.Load(KLibOpenGles2) == KErrNone)
		{
		lib.Close();
		iHasOpenGles2 = ETrue;
		}
	return KErrNone;
	}

void XSgDriver::Delete()
	{
	RHeap* heap = iHeap;
	this->~XSgDriver();
	heap->Free(this);
	__ASSERT_DEBUG(heap->Count() == 0, Panic(ESgPanicUnclosedResources));
	heap->Close();
	}

TInt XSgDriver::CreateImage(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride, const TSgAttributeArrayBase* aAttributes, TAny*& aResult)
	{
	TInt err = CheckImageInfo(aInfo);
	if (err != KErrNone)
		{
		return err;
		}
	TInt minDataStride = SgMinDataStride(aInfo.iSizeInPixels.iWidth, aInfo.iPixelFormat);
	if (aDataAddress && Abs(aDataStride) < minDataStride)
		{
		return KErrArgument;
		}
	if (aAttributes)
		{
		return KErrNotSupported;
		}
	TUint32 attribs = KSgResourceAttributes | MatchingEglConfigUsage(aInfo.iUsage);
	TPckgBuf<TSgImageMetaData> metaDataPckg;
	metaDataPckg().iSizeInPixels = aInfo.iSizeInPixels;
	metaDataPckg().iPixelFormat = aInfo.iPixelFormat;
	TInt dataStride = SgAlignedDataStride(aInfo.iSizeInPixels.iWidth, aInfo.iPixelFormat);
	TInt dataSize = dataStride * aInfo.iSizeInPixels.iHeight;
	TSgDrawableId id;
	err = iDevice.CreateResource(attribs, metaDataPckg, dataSize, id.iId);
	if (err != KErrNone)
		{
		return err;
		}
	iMutex.Wait();
	XSgImage* imageImpl = static_cast<XSgImage*>(iHeap->Alloc(sizeof(XSgImage)));
	if (!imageImpl)
		{
		(void)iDevice.CloseResource(id.iId);
		iMutex.Signal();
		return KErrNoMemory;
		}
	new(imageImpl) XSgImage(id, attribs, metaDataPckg(), iDevice.ResourceDataAddress(id.iId), dataStride);
	RHeap* prevHeap = User::SwitchHeap(iHeap);
	err = iImagesByAddress.InsertInAddressOrder(imageImpl);
	if (err == KErrNone)
		{
		err = iImagesById.InsertInOrder(imageImpl, XSgImage::Compare);
		if (err != KErrNone)
			{
			iImagesByAddress.Remove(iImagesByAddress.FindInAddressOrder(imageImpl));
			iImagesByAddress.GranularCompress();
			}
		}
	User::SwitchHeap(prevHeap);
	if (err == KErrNone)
		{
		if (aDataAddress)
			{
			const TAny* src = aDataStride > 0 ? aDataAddress : PtrAdd(aDataAddress, -aDataStride * (aInfo.iSizeInPixels.iHeight - 1));
			TAny* trg = imageImpl->DataAddress();
			for (TInt y = 0; y < aInfo.iSizeInPixels.iHeight; ++y)
				{
				Mem::Copy(trg, src, minDataStride);
				src = PtrAdd(src, aDataStride);
				trg = PtrAdd(trg, dataStride);
				}
			}
		aResult = imageImpl;
		}
	else
		{
		(void)iDevice.CloseResource(id.iId);
		iHeap->Free(imageImpl);
		}
	iMutex.Signal();
	return err;
	}

TInt XSgDriver::CreateImage(const TSgImageInfo& aInfo, const XSgImage* aImageImpl, const TSgAttributeArrayBase* aAttributes, TAny*& aResult)
	{
	if (!aImageImpl)
		{
		return KErrArgument;
		}
	__ASSERT_ALWAYS(CheckImage(aImageImpl), Panic(ESgPanicBadDrawableHandle));
	TSgImageInfo info;
	aImageImpl->GetInfo(info);
	if (aInfo.iSizeInPixels != info.iSizeInPixels || aInfo.iPixelFormat != info.iPixelFormat)
		{
		return KErrNotSupported;
		}
	return CreateImage(aInfo, aImageImpl->DataAddress(), aImageImpl->DataStride(), aAttributes, aResult);
	}

TInt XSgDriver::FindAndOpenImage(TSgDrawableId aId, const TSgAttributeArrayBase* aAttributes, TAny*& aResult)
	{
	if (aId == KSgNullDrawableId)
		{
		return KErrArgument;
		}
	if (aAttributes)
		{
		return KErrNotSupported;
		}
	TInt err;
	iMutex.Wait();
	TInt indexById = iImagesById.FindInOrder(aId, XSgImage::Compare);
	if (indexById >= 0)
		{
		XSgImage* imageImpl = iImagesById[indexById];
		err = imageImpl->Open();
		if (err == KErrNone)
			{
			aResult = imageImpl;
			}
		}
	else
		{
		err = iDevice.OpenResource(aId.iId);
		if (err != KErrNone)
			{
			iMutex.Signal();
			return err;
			}
		TPckgBuf<TSgImageMetaData> metaDataPckg;
		err = iDevice.GetResourceMetaData(aId.iId, metaDataPckg);
		if (metaDataPckg.Size() != sizeof(TSgImageMetaData))
			{
			err = KErrGeneral;
			}
		TUint32 attribs;
		if (err == KErrNone)
			{
			attribs = iDevice.ResourceAttributes(aId.iId);
			TSgImageInfo info(metaDataPckg().iSizeInPixels, metaDataPckg().iPixelFormat, attribs & KSgUsageBitMask);
			if (CheckImageInfo(info) != KErrNone)
				{
				err = KErrGeneral;
				}
			}
		TInt dataStride;
		if (err == KErrNone)
			{
			dataStride = SgAlignedDataStride(metaDataPckg().iSizeInPixels.iWidth, metaDataPckg().iPixelFormat);
			if (iDevice.ResourceDataSize(aId.iId) != dataStride * metaDataPckg().iSizeInPixels.iHeight)
				{
				err = KErrGeneral;
				}
			}
		if (err != KErrNone)
			{
			(void)iDevice.CloseResource(aId.iId);
			iMutex.Signal();
			return err;
			}
		XSgImage* imageImpl = static_cast<XSgImage*>(iHeap->Alloc(sizeof(XSgImage)));
		if (!imageImpl)
			{
			(void)iDevice.CloseResource(aId.iId);
			iMutex.Signal();
			return KErrNoMemory;
			}
		new(imageImpl) XSgImage(aId, attribs, metaDataPckg(), iDevice.ResourceDataAddress(aId.iId), dataStride);
		RHeap* prevHeap = User::SwitchHeap(iHeap);
		err = iImagesByAddress.InsertInAddressOrder(imageImpl);
		if (err == KErrNone)
			{
			err = iImagesById.InsertInOrder(imageImpl, XSgImage::Compare);
			if (err != KErrNone)
				{
				iImagesByAddress.Remove(iImagesByAddress.FindInAddressOrder(imageImpl));
				iImagesByAddress.GranularCompress();
				}
			}
		User::SwitchHeap(prevHeap);
		if (err == KErrNone)
			{
			aResult = imageImpl;
			}
		else
			{
			(void)iDevice.CloseResource(aId.iId);
			iHeap->Free(imageImpl);
			}
		}
	iMutex.Signal();
	return err;
	}

void XSgDriver::DeleteImage(XSgImage* aImageImpl)
	{
	iMutex.Wait();
	TInt indexByAddress = iImagesByAddress.FindInAddressOrder(aImageImpl);
	TInt indexById = iImagesById.FindInOrder(aImageImpl, XSgImage::Compare);
	__ASSERT_DEBUG(indexByAddress >= 0 && indexById >= 0, Panic(ESgPanicBadImagePointer));
	RHeap* prevHeap = User::SwitchHeap(iHeap);
	iImagesByAddress.Remove(indexByAddress);
	iImagesById.Remove(indexById);
	iImagesByAddress.GranularCompress();
	iImagesById.GranularCompress();
	User::SwitchHeap(prevHeap);
	(void)iDevice.CloseResource(aImageImpl->Id().iId);
	aImageImpl->~XSgImage();
	iHeap->Free(aImageImpl);
	iMutex.Signal();
	}

TUint32 XSgDriver::MatchingEglConfigUsage(TUint32 aUsage) const
	{
	if (aUsage & KSgUsageAllSurfaceTypes)
		{
		if (iHasOpenVg)
			{
			aUsage |= ESgUsageBitOpenVgSurface;
			}
		if (iHasOpenGles)
			{
			aUsage |= ESgUsageBitOpenGlesSurface;
			}
		if (iHasOpenGles2)
			{
			aUsage |= ESgUsageBitOpenGles2Surface;
			}
		}
	return aUsage;
	}

TInt XSgDriver::CheckImageInfo(const TSgImageInfo& aInfo) const
	{
	if (aInfo.iSizeInPixels.iWidth <= 0 || aInfo.iSizeInPixels.iHeight <= 0
		|| aInfo.iPixelFormat == EUidPixelFormatUnknown || aInfo.iUsage == 0)
		{
		return KErrArgument;
		}
	if (aInfo.iSizeInPixels.iWidth > KMaxTInt16 / 2 || aInfo.iSizeInPixels.iHeight > KMaxTInt16 / 2)
		{
		return KErrTooBig;
		}
	if (aInfo.iUsage & ~KSgUsageAll)
		{
		return KErrNotSupported;
		}
	if ((aInfo.iUsage & (ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface)) && !iHasOpenVg
		|| (aInfo.iUsage & (ESgUsageBitOpenGlesTexture2D | ESgUsageBitOpenGlesSurface)) && !iHasOpenGles
		|| (aInfo.iUsage & (ESgUsageBitOpenGles2Texture2D | ESgUsageBitOpenGles2Surface)) && !iHasOpenGles2)
		{
		return KErrNotSupported;
		}
	switch (aInfo.iPixelFormat)
		{
	case ESgPixelFormatA_8:
		if (aInfo.iUsage & KSgUsageAllSurfaceTypes)
			{
			return KErrNotSupported;
			}
		// coverity[fallthrough]
	case ESgPixelFormatRGB_565:
	case ESgPixelFormatXRGB_8888:
	case ESgPixelFormatARGB_8888:
	case ESgPixelFormatARGB_8888_PRE:
		return KErrNone;
	default:
		return KErrNotSupported;
		}
	}

TBool XSgDriver::CheckImage(const TAny* aImageImpl) const
	{
	iMutex.Wait();
	TInt indexByAddress = iImagesByAddress.FindInAddressOrder(static_cast<const XSgImage*>(aImageImpl));
	iMutex.Signal();
	return indexByAddress >= 0;
	}

TInt XSgDriver::GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr)
	{
	if (aInterfaceUid == KNullUid)
		{
		return KErrArgument;
		}
	if (aInterfaceUid.iUid == MSgDriver_Profiling::EInterfaceUid)
		{
		aInterfacePtr = static_cast<MSgDriver_Profiling*>(this);
		return KErrNone;
		}
#ifdef _DEBUG
	if (aInterfaceUid.iUid == MSgDriver_Test::EInterfaceUid)
		{
		aInterfacePtr = static_cast<MSgDriver_Test*>(this);
		return KErrNone;
		}
#endif
	return KErrExtensionNotSupported;
	}

TInt XSgDriver::LocalResourceCount() const
	{
	TInt count = 0;
	iMutex.Wait();
	TInt n = iImagesByAddress.Count();
	for (TInt i = 0; i < n; ++i)
		{
		count += iImagesByAddress[i]->RefCount();
		}
	iMutex.Signal();
	return count;
	}

TInt XSgDriver::GlobalResourceCount() const
	{
	return iDevice.GlobalResourceCount();
	}

TInt XSgDriver::LocalGraphicsMemoryUsed() const
	{
	return iDevice.LocalGraphicsMemoryUsed();
	}

TInt XSgDriver::GlobalGraphicsMemoryUsed() const
	{
	return iDevice.GlobalGraphicsMemoryUsed();
	}

#ifdef _DEBUG

void XSgDriver::AllocMarkStart()
	{
	iMutex.Wait();
	iHeap->__DbgMarkStart();
	iMutex.Signal();
	}

void XSgDriver::AllocMarkEnd(TInt aCount)
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

void XSgDriver::SetAllocFail(RAllocator::TAllocFail aType, TInt aRate)
	{
	iMutex.Wait();
	iHeap->__DbgSetAllocFail(aType, aRate);
	iMutex.Signal();
	}

#endif // _DEBUG

#ifndef __WINS__
XSgDriverPls gPls;
#endif
