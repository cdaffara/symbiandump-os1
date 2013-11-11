// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "rendertarget.h"
#include <graphics/directgdidriver.h>
#include <graphics/sgutils.h>
#include "panic.h"
#include <bitdraworigin.h>
#include <bitdrawinterfaceid.h>
#include "utils.h"
#if defined(__WINS__) && defined(_DEBUG)
#include "osbwin.h"
#endif

RWsOffScreenImageTarget::RWsOffScreenImageTarget()
	{
	}

void RWsOffScreenImageTarget::OpenL(TUint32 aUsage, TSgCpuAccess aCpuAccess, TUidPixelFormat aPixelFormat, const TSize& aSize, TInt aScreenNumber)
	{
	CDirectGdiDriver* theDGdiDriver = CDirectGdiDriver::Static();
	if(!theDGdiDriver)
		{
		User::Leave(KErrNotReady);
		}
	iImageInfos[CRenderTarget::ENormalAspectRatio].iUsage = aUsage;
	iImageInfos[CRenderTarget::ENormalAspectRatio].iPixelFormat = aPixelFormat;
	iImageInfos[CRenderTarget::ENormalAspectRatio].iSizeInPixels = aSize;
	iImageInfos[CRenderTarget::ENormalAspectRatio].iCpuAccess = aCpuAccess;
	iImageInfos[CRenderTarget::ENormalAspectRatio].iScreenId = aScreenNumber;
	
	iImageInfos[CRenderTarget::EInvertedAspectRatio] = iImageInfos[CRenderTarget::ENormalAspectRatio];
	iImageInfos[CRenderTarget::EInvertedAspectRatio].iSizeInPixels.SetSize(aSize.iHeight, aSize.iWidth);

	const TInt KImageCount = 1;
	User::LeaveIfError(RSgImageCollection::Create(iImageInfos, KImageCount, iImageCollections, CRenderTarget::EAspectRatioCount));
	
	for(TInt ii = 0; ii < CRenderTarget::EAspectRatioCount; ii++)
		{
		User::LeaveIfError(iImageCollections[ii].GetInfo(iImageInfos[ii])); //should be the same as requested, this is just belt and braces
		User::LeaveIfError(iImageCollections[ii].OpenImage(0, iImages[ii]));
		iImageTargets[ii] = RDirectGdiImageTarget(*theDGdiDriver);
		User::LeaveIfError(iImageTargets[ii].Create(iImages[ii]));
		}
	}

void RWsOffScreenImageTarget::Close()
	{
	for(TInt ii = 0; ii < CRenderTarget::EAspectRatioCount; ii++)
		{
		iImageTargets[ii].Close();
		iImages[ii].Close();
		iImageCollections[ii].Close();
		}
	}

/**
 Create and construct render target. The function creates RSgImage and target associated with it.
 DirectGc wrapper will also be created at this stage. 
 */ 
CRenderTarget* CRenderTarget::NewL(MWsIniFile* aIniFile, TUint32 aUsage, TSgCpuAccess aCpuAccess, TDisplayMode aDisplayMode, const TSize& aSize, TInt aScreenNumber)
	{
	CRenderTarget* self=new(ELeave) CRenderTarget();
	CleanupStack::PushL(self);
	self->ConstructL(aIniFile, aUsage, aCpuAccess, aDisplayMode, aSize, aScreenNumber);
	CleanupStack::Pop(self);
	return self;
	}

inline CRenderTarget::CRenderTarget()
	: iCurrentAspectRatio(ENormalAspectRatio)
	{}

CRenderTarget::~CRenderTarget()
	{
	delete iDirectGdiGcWrapper;
	iTarget.Close();
#if defined(__WINS__) && defined(_DEBUG)
	delete iOsbWin;
#endif	
	}

/**
 Construct render target. The function creates RSgImage and target associated with it.
 DirectGc wrapper will also be created at this stage. 
 */ 
#if defined(__WINS__) && defined(_DEBUG)
void CRenderTarget::ConstructL(MWsIniFile* aIniFile, TUint32 aUsage, TSgCpuAccess aCpuAccess, TDisplayMode aDisplayMode, const TSize& aSize, TInt aScreenNumber)
#else
void CRenderTarget::ConstructL(MWsIniFile* /*aIniFile*/, TUint32 aUsage, TSgCpuAccess aCpuAccess, TDisplayMode aDisplayMode, const TSize& aSize, TInt aScreenNumber)
#endif
	{
	CDirectGdiDriver* theDGdiDriver = CDirectGdiDriver::Static();
	if(!theDGdiDriver)
		{
		User::Leave(KErrNotReady);
		}
	iTarget.OpenL(aUsage, aCpuAccess, SgUtils::DisplayModeToPixelFormat(aDisplayMode), aSize, aScreenNumber);
	iDirectGdiGcWrapper = CDirectGdiGcWrapper::NewL();
	User::LeaveIfError(iDirectGdiGcWrapper->DirectGdiGc().Activate(iTarget.iImageTargets[iCurrentAspectRatio]));

#if defined(__WINS__) && defined(_DEBUG)
	_LIT(KDebugOsb, "DEBUGOSB");
	if(aIniFile->FindVar(aScreenNumber, KDebugOsb))
		{
		_LIT(KDebugOsbTitleFormatBackBuffer, "Screen %d, back buffer   ");
		_LIT(KDebugOsbTitleFormatScreen,     "Screen %d, display buffer");
		TBuf<32> title;
		title.Format(((aUsage & ESgUsageScreenSource) ? KDebugOsbTitleFormatScreen : KDebugOsbTitleFormatBackBuffer), aScreenNumber);
		iOsbWin = CDebugOsbWin::NewL(title, aSize);
		}
#endif
	}

TAny* CRenderTarget::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
	case MWsUiBuffer::EWsObjectInterfaceId:
		return static_cast<MWsUiBuffer*>(this);
		}
	return DirectGdiGc()->ResolveObjectInterface(aTypeId);
	}

TInt CRenderTarget::MapReadWrite(TAny*& aDataAddress, TInt& aDataStride)
	{
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	if(driver)
		driver->Finish();
	return Image().MapReadWrite(aDataAddress, aDataStride);
	}

TInt CRenderTarget::MapWriteOnly(TAny*& aDataAddress, TInt& aDataStride)
	{
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	if(driver)
		driver->Finish();
	return Image().MapWriteOnly(aDataAddress, aDataStride);
	}

TInt CRenderTarget::MapReadOnly(const TAny*& aDataAddress, TInt& aDataStride) const
	{
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	if(driver)
		driver->Finish();
	return Image().MapReadOnly(aDataAddress, aDataStride);
	}

TInt CRenderTarget::Unmap()
	{
	return Image().Unmap();
	}

TInt CRenderTarget::Unmap() const
	{
	return Image().Unmap();
	}

TUidPixelFormat CRenderTarget::PixelFormat() const
	{
	return ImageInfo().iPixelFormat;
	}

TSize CRenderTarget::SizeInPixels() const
	{
	return ImageInfo().iSizeInPixels;
	}

TDisplayMode CRenderTarget::DisplayMode() const
	{
	return SgUtils::PixelFormatToDisplayMode(PixelFormat());
	}

const TSurfaceId& CRenderTarget::SurfaceId() const
	{
	return ImageCollection().SurfaceId();
	}

const TSgDrawableId& CRenderTarget::ImageId(TAspectRatio aAspectRatio) const
	{
	return iTarget.iImages[aAspectRatio].Id();
	}

void CRenderTarget::SetAspectRatio(TAspectRatio aAspectRatio)
	{
	STD_ASSERT_DEBUG(aAspectRatio == ENormalAspectRatio || aAspectRatio == EInvertedAspectRatio, EPluginPanicTemp);
	if(aAspectRatio != iCurrentAspectRatio)
		{
		iCurrentAspectRatio = aAspectRatio;
		iDirectGdiGcWrapper->Reset();
		}
	iDirectGdiGcWrapper->DirectGdiGc().Activate(iTarget.iImageTargets[iCurrentAspectRatio]);
	}

TInt CRenderTarget::SetDrawDeviceOffset(TPoint& aOrigin)
	{
	//set the offset on both wrappers
	MDrawDeviceOrigin* originInterface=NULL;
	TInt result=	 iDirectGdiGcWrapper->DirectGdiGc().GetInterface(TUid::Uid(KDrawDeviceOriginInterfaceID),reinterpret_cast<TAny*&>(originInterface));
	if (result>=KErrNone && originInterface!=NULL)
		{
		result=originInterface->Set(aOrigin);
		}

	if (result>=KErrNone)
		{
		iOffset=aOrigin;
		iDirectGdiGcWrapper->DirectGdiGc().Activate(iTarget.iImageTargets[iCurrentAspectRatio]);
		}
	return result;
	}

TInt CRenderTarget::AllocNewTarget(RWsOffScreenImageTarget& aNewTarget, const TSize& aNewSize)
	{
	TRAPD(err,
		aNewTarget.OpenL(
				iTarget.iImageInfos[CRenderTarget::ENormalAspectRatio].iUsage,
				iTarget.iImageInfos[CRenderTarget::ENormalAspectRatio].iCpuAccess,
				iTarget.iImageInfos[CRenderTarget::ENormalAspectRatio].iPixelFormat,
				aNewSize,
				iTarget.iImageInfos[CRenderTarget::ENormalAspectRatio].iScreenId))
	return err;
	}

void CRenderTarget::SwitchTarget(RWsOffScreenImageTarget& aNewTarget)
	{
	iTarget.Close();
	iTarget = aNewTarget;
	iDirectGdiGcWrapper->DirectGdiGc().Activate(iTarget.iImageTargets[iCurrentAspectRatio]);
	}

void CRenderTarget::GetPixel(TRgb& aColor, const TPoint& aPixel) const
	{
	const TAny* dataAddress = NULL; 
	TInt dataStride;
	const TInt err = MapReadOnly(dataAddress, dataStride);
	if(!err)
		{
		const TUidPixelFormat pixelFormat = PixelFormat();
		const TInt bpp = SgUtils::MinDataStride(1, pixelFormat);
		const TUint32 offset = aPixel.iY * dataStride / bpp + aPixel.iX; 
		switch(pixelFormat)
			{
		case EUidPixelFormatARGB_8888_PRE:
		case EUidPixelFormatARGB_8888:
			{
			const TInt32* dataAddress1 = static_cast<const TInt32*>(dataAddress) + offset; 
			const TInt32 colValue = *dataAddress1;
			aColor.SetInternal(colValue);
			}
			break;
		case EUidPixelFormatRGB_565:
			{
			const TInt16* dataAddress1 = static_cast<const TInt16*>(dataAddress) + offset;
			const TInt16 colValue = *dataAddress1;
			aColor = TRgb::Color64K((TInt)colValue);
			}
			break;
			}
		Unmap();
		}
	}

void CRenderTarget::GetScanLine(TDes8& aScanLine, const TPoint& aStartPixel, TInt aPixelLength, TDisplayMode aDispMode) const
	{
	TRect rectSrc(aStartPixel, TSize(aPixelLength, 1)); 
	const TRect  rectClientArea(SizeInPixels()); 
	rectSrc.Intersection(rectClientArea);
	if(rectSrc.IsEmpty())
		return;
	const TUidPixelFormat pixelFormatSource = PixelFormat();
	const TUidPixelFormat pixelFormatDest = SgUtils::DisplayModeToPixelFormat(aDispMode);
	if((pixelFormatSource == EUidPixelFormatUnknown) || (pixelFormatDest == EUidPixelFormatUnknown))
		return;
	
	aPixelLength = rectSrc.Width();
	const TInt dataStrideDest = SgUtils::MinDataStride(aPixelLength, pixelFormatDest);
	if(dataStrideDest <= 0)
		return;
	const TAny* dataAddressSource = NULL; 
	TInt dataStrideSource;
	const TInt err = MapReadOnly(dataAddressSource, dataStrideSource);
	if(!err)
		{
		aScanLine.SetLength(dataStrideDest);
		SgUtils::TransferPixels(const_cast<TUint8*>(aScanLine.Ptr()), dataStrideDest, pixelFormatDest, 
				dataAddressSource, dataStrideSource, pixelFormatSource, rectSrc);
		Unmap();
		}
	}

TBool CRenderTarget::RectCompare(const TRect& aRect1, const TRect& aRect2) const
	{
	const TAny* startDataAddress = NULL; 
	TInt dataStride;
	
	const TRect clientRect(SizeInPixels());
	TRect rect1 = aRect1;
	TRect rect2 = aRect2;
	
	rect1.Intersection(clientRect);
	rect2.Intersection(clientRect);
	if(rect1.IsEmpty() || rect2.IsEmpty() || (rect1 != aRect1) || (rect2 != aRect2) ||
			(rect1.Width() != rect2.Width()) ||	(rect1.Height() != rect2.Height()))
		return EFalse;

	TUidPixelFormat pixelFormat = PixelFormat();
	if(pixelFormat == EUidPixelFormatUnknown)
		return EFalse;
		
	TInt bpp = SgUtils::MinDataStride(1, pixelFormat);
	if(bpp == 0)
		return EFalse;
	
	TInt err = MapReadOnly(startDataAddress, dataStride);
	if(err != KErrNone)
		return EFalse;
	TBool res = ETrue;
	TPoint startPoint1 = rect1.iTl;
	TPoint startPoint2 = rect2.iTl;
	const TInt length1 = SgUtils::MinDataStride(rect1.Width(), pixelFormat);
	const TInt length2 = SgUtils::MinDataStride(rect2.Width(), pixelFormat);
		
	for(; (startPoint1.iY < rect1.iBr.iY) && (startPoint2.iY < rect2.iBr.iY); startPoint1.iY++, startPoint2.iY++)
		{
		const TUint8* dataAddress1 = DataAddress(startDataAddress, startPoint1, dataStride, bpp); 
		const TUint8* dataAddress2 = DataAddress(startDataAddress, startPoint2, dataStride, bpp); 
		
		if(Mem::Compare(dataAddress1, length1, dataAddress2, length2) != 0)
			{
			res = EFalse; 
			break;
			}
		}
	Unmap();
	
	return res;
	}

void CRenderTarget::CopyToBitmapL(CFbsBitmap* aBitmap, const TRect& aRect) const
	{
	const TUidPixelFormat pixelFormatDest = SgUtils::DisplayModeToPixelFormat(aBitmap->DisplayMode());
	const TUidPixelFormat pixelFormatSrc = PixelFormat();
	if((pixelFormatSrc == EUidPixelFormatUnknown) || (pixelFormatDest == EUidPixelFormatUnknown))
		User::LeaveIfError(KErrNotSupported);
	
	aBitmap->BeginDataAccess();
	const TSize sizeDest = aBitmap->SizeInPixels();
	TRect rectSrc = aRect;
	rectSrc.Intersection(TRect(SizeInPixels()));
	if(rectSrc.IsEmpty())
		return;
	if(rectSrc.Height() > sizeDest.iHeight)
		{
		rectSrc.SetHeight(sizeDest.iHeight);
		}
	if(rectSrc.Width() > sizeDest.iWidth)
		{
		rectSrc.SetWidth(sizeDest.iWidth);
		}

	const TAny* dataAddressSrc = NULL; 
	TInt dataStrideSrc;
	User::LeaveIfError(MapReadOnly(dataAddressSrc, dataStrideSrc));

	SgUtils::TransferPixels(aBitmap->DataAddress(), aBitmap->DataStride(), pixelFormatDest,  
			dataAddressSrc, dataStrideSrc, pixelFormatSrc, rectSrc);

	aBitmap->EndDataAccess();
	Unmap();
	}

/*
 Helper function to obtain the address of the buffer   
 */
const TUint8* CRenderTarget::DataAddress(const TAny* aStartDataAddress, const TPoint& aStartPoint, TInt aDataStride, TInt aBpp) const
	{
	const TInt offset = aStartPoint.iX * aBpp + aStartPoint.iY * aDataStride;  
	const TUint8* dataAddress = static_cast<TUint8*>(const_cast<TAny*> (aStartDataAddress)); 

	return(dataAddress + offset);
	}

#if defined(__WINS__) && defined(_DEBUG)
void CRenderTarget::UpdateDebugWin()
	{
	if (iOsbWin)
		{
		const TAny* dataAddress = NULL; 
		TInt dataStride;
		const TInt err = MapReadOnly(dataAddress, dataStride);
		if(!err)
			{
			const TUint32* dataAddress1 = static_cast<const TUint32*>(dataAddress); 
			iOsbWin->Refresh(SizeInPixels(), DisplayMode(), dataAddress1);
			Unmap();
			}
		}
	}
#endif
