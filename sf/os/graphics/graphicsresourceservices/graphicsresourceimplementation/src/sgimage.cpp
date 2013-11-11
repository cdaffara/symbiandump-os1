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
// Graphics Resource - image implementation
//

#include "sgdriver.h"

EXPORT_C TInt RSgImage::Create(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride, const TSgAttributeArrayBase* aAttributes)
	{
	if (iImpl)
		{
		return KErrInUse;
		}
	__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
	return gPls.iDriver->CreateImage(aInfo, aDataAddress, aDataStride, aAttributes, iImpl);
	}

EXPORT_C TInt RSgImage::Create(const TSgImageInfo& aInfo, const RSgImage& aImage, const TSgAttributeArrayBase* aAttributes)
	{
	if (iImpl)
		{
		return KErrInUse;
		}
	__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
	return gPls.iDriver->CreateImage(aInfo, static_cast<XSgImage*>(aImage.iImpl), aAttributes, iImpl);
	}

EXPORT_C TInt RSgImage::GetInfo(TSgImageInfo& aInfo) const
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
	__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_ALWAYS(gPls.iDriver->CheckImage(iImpl), Panic(ESgPanicBadDrawableHandle));
	static_cast<XSgImage*>(iImpl)->GetInfo(aInfo);
	return KErrNone;
	}

EXPORT_C TInt RSgImage::GetAttribute(TUid aUid, TInt& aValue) const
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
	__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_ALWAYS(gPls.iDriver->CheckImage(iImpl), Panic(ESgPanicBadDrawableHandle));
	return static_cast<XSgImage*>(iImpl)->GetAttribute(aUid, aValue);
	}

EXPORT_C TInt RSgImage::GetPixelFormats(TUint32 aUsage, RArray<TInt>& aPixelFormats, const TSgAttributeArrayBase* aAttributes)
	{
	static const TInt KSupportedPixelFormats[] =
		{
		ESgPixelFormatA_8,
		ESgPixelFormatRGB_565,
		ESgPixelFormatXRGB_8888,
		ESgPixelFormatARGB_8888,
		ESgPixelFormatARGB_8888_PRE
		};
	static const TInt KNumSupportedPixelFormats = sizeof(KSupportedPixelFormats) / sizeof(TInt);
	if (aUsage == 0 || aPixelFormats.Count() != 0)
		{
		return KErrArgument;
		}
	if (aAttributes)
		{
		return KErrNotSupported;
		}
	if (aUsage & ~KSgUsageAll)
		{
		return KErrNone;
		}
	_LIT(KLibOpenVg, "libOpenVG.dll");
	_LIT(KLibOpenGles, "libGLESv1_CM.dll");
	_LIT(KLibOpenGles2, "libGLESv2.dll");
	RLibrary lib;
	if (aUsage & (ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface))
		{
		if (lib.Load(KLibOpenVg) != KErrNone)
			{
			return KErrNone;
			}
		lib.Close();
		}
	if (aUsage & (ESgUsageBitOpenGlesTexture2D | ESgUsageBitOpenGlesSurface))
		{
		if (lib.Load(KLibOpenGles) != KErrNone)
			{
			return KErrNone;
			}
		lib.Close();
		}
	if (aUsage & (ESgUsageBitOpenGles2Texture2D | ESgUsageBitOpenGles2Surface))
		{
		if (lib.Load(KLibOpenGles2) != KErrNone)
			{
			return KErrNone;
			}
		lib.Close();
		}
	TInt err = KErrNone;
	for (TInt i = 0; i < KNumSupportedPixelFormats; ++i)
		{
		if (KSupportedPixelFormats[i] == ESgPixelFormatA_8 && (aUsage & KSgUsageAllSurfaceTypes))
			{
			continue;
			}
		err = aPixelFormats.Append(KSupportedPixelFormats[i]);
		if (err != KErrNone)
			{
			break;
			}
		}
	return err;
	}

XSgImage::XSgImage(TSgDrawableId aId, TUint32 aAttribs, const TSgImageMetaData& aMetaData, TAny* aDataAddress, TInt aDataStride)
	: iRefCount(1), iId(aId), iInfo(aMetaData.iSizeInPixels, aMetaData.iPixelFormat, aAttribs & KSgUsageBitMask),
	  iDataAddress(aDataAddress), iDataStride(aDataStride)
	{
	}

XSgImage::~XSgImage()
	{
	__ASSERT_DEBUG(iRefCount == 0, Panic(ESgPanicBadRefCount));
	}

TInt XSgImage::Open()
	{
	if (User::SafeInc(iRefCount) == 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

void XSgImage::Close()
	{
	if (User::SafeDec(iRefCount) == 1)
		{
		gPls.iDriver->DeleteImage(this);
		}
	}

TInt XSgImage::RefCount() const
	{
	return iRefCount;
	}

TSgDrawableId XSgImage::Id() const
	{
	return iId;
	}

TUid XSgImage::DrawableType() const
	{
	return KSgImageTypeUid;
	}

TInt XSgImage::GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr)
	{
	if (aInterfaceUid == KNullUid)
		{
		return KErrArgument;
		}
	if (aInterfaceUid.iUid == MSgImage_Sw::EInterfaceUid)
		{
		aInterfacePtr = static_cast<MSgImage_Sw*>(this);
		return KErrNone;
		}
	return KErrExtensionNotSupported;
	}

void XSgImage::GetInfo(TSgImageInfo& aInfo) const
	{
	aInfo = iInfo;
	}

TInt XSgImage::GetAttribute(TUid aUid, TInt& /*aValue*/) const
	{
	if (aUid == KNullUid)
		{
		return KErrArgument;
		}
	return KErrNotSupported;
	}

TInt XSgImage::Compare(const XSgImage& aImage1, const XSgImage& aImage2)
	{
	return Compare(&aImage1.iId, aImage2);
	}

TInt XSgImage::Compare(const TSgDrawableId* aId, const XSgImage& aImage)
	{
	if (aId->iId > aImage.iId.iId)
		{
		return 1;
		}
	if (aId->iId < aImage.iId.iId)
		{
		return -1;
		}
	return 0;
	}

TAny* XSgImage::DataAddress() const
	{
	return iDataAddress;
	}

TInt XSgImage::DataStride() const
	{
	return iDataStride;
	}
