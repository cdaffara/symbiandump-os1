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
// Graphics Resource - internal functionality
//

#ifndef SGDRIVER_H
#define SGDRIVER_H

#include <sgresource/sgimage.h>
#include <sgresource/sgimage_sw.h>
#include <sgresource/sgdriver_test.h>
#include <sgresource/sgdriver_profiling.h>
#include <pls.h>
#include "sgdevice.h"

const TUid KSgResourceLibraryUid = {0x102872C5};

const TInt KSgUsageAll = ESgUsageBitOpenVgImage | ESgUsageBitOpenGlesTexture2D | ESgUsageBitOpenGles2Texture2D
                       | ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface | ESgUsageBitOpenGles2Surface;
const TInt KSgUsageAllSurfaceTypes = ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface | ESgUsageBitOpenGles2Surface;

enum TSgResourceImplPanicReason
	{
	ESgPanicResourceImplGeneral = 1,
	ESgPanicBadRefCount = 2,
	ESgPanicBadImagePointer = 3
	};

void Panic(TSgResourceImplPanicReason aReason);

NONSHARABLE_CLASS(XSgImage)
	: public MSgImage_Sw
	{
public:
	XSgImage(TSgDrawableId aId, TUint32 aAttribs, const TSgImageMetaData& aMetaData, TAny* aDataAddress, TInt aDataStride);
	~XSgImage();
	TInt Open();
	void Close();
	TInt RefCount() const;
	TSgDrawableId Id() const;
	TUid DrawableType() const;
	TInt GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr);
	void GetInfo(TSgImageInfo& aInfo) const;
	TInt GetAttribute(TUid aUid, TInt& aValue) const;
	static TInt Compare(const XSgImage& aImage1, const XSgImage& aImage2);
	static TInt Compare(const TSgDrawableId* aId, const XSgImage& aImage);
	// From MSgImage_Sw
	TAny* DataAddress() const;
	TInt DataStride() const;
private:
	TInt iRefCount;
	TSgDrawableId iId;
	TSgImageInfo iInfo;
	TAny* iDataAddress;
	TInt iDataStride;
	};

NONSHARABLE_CLASS(XSgDriver)
	: public MSgDriver_Profiling
#ifdef _DEBUG
	, public MSgDriver_Test
#endif
	{
public:
	XSgDriver(RHeap* aHeap);
	~XSgDriver();
	TInt Construct();
	void Delete();
	TInt CreateImage(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride, const TSgAttributeArrayBase* aAttributes, TAny*& aResult);
	TInt CreateImage(const TSgImageInfo& aInfo, const XSgImage* aImageImpl, const TSgAttributeArrayBase* aAttributes, TAny*& aResult);
	TInt FindAndOpenImage(TSgDrawableId aId, const TSgAttributeArrayBase* aAttributes, TAny*& aResult);
	void DeleteImage(XSgImage* aImageImpl);
	TUint32 MatchingEglConfigUsage(TUint32 aUsage) const;
	TInt CheckImageInfo(const TSgImageInfo& aInfo) const;
	TBool CheckImage(const TAny* aImageImpl) const;
	TInt GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr);
	// From MSgDriver_Profiling
	TInt LocalResourceCount() const;
	TInt GlobalResourceCount() const;
	TInt LocalGraphicsMemoryUsed() const;
	TInt GlobalGraphicsMemoryUsed() const;
#ifdef _DEBUG
	// From MSgDriver_Test
	void AllocMarkStart();
	void AllocMarkEnd(TInt aCount);
	void SetAllocFail(RAllocator::TAllocFail aType, TInt aRate);
#endif
private:
	RHeap* iHeap;
	mutable RMutex iMutex;
	mutable RSgDevice iDevice;
	RPointerArray<XSgImage> iImagesByAddress;
	RPointerArray<XSgImage> iImagesById;
	TBool iHasOpenVg;
	TBool iHasOpenGles;
	TBool iHasOpenGles2;
	};

NONSHARABLE_CLASS(XSgDriverPls)
	{
public:
	XSgDriverPls();
public:
	TInt iError;
	RMutex iMutex;
	TInt iOpenCount;
	XSgDriver* iDriver;
	};

#ifdef __WINS__
#define gPls (*Pls<XSgDriverPls>(KSgResourceLibraryUid))
#else
extern XSgDriverPls gPls;
#endif

#endif // SGDRIVER_H
