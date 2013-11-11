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

#ifndef __RENDERTARGET_H__
#define __RENDERTARGET_H__

#include <graphics/sgimagecollection.h>
#include <graphics/wsuibuffer.h>
#include "directgdigcwrapper.h"

#if defined(__WINS__) && defined(_DEBUG)
class CDebugOsbWin;
#endif

class RWsOffScreenImageTarget
	{
public:
	RWsOffScreenImageTarget();
	void OpenL(TUint32 aUsage,TSgCpuAccess aCpuAccess,TUidPixelFormat aPixelFormat,const TSize& aSize, TInt aScreenNumber);
	void Close();
public:
	static const TInt KAspectRatioCount = 2;
	TSgImageInfo iImageInfos[KAspectRatioCount];
	RDirectGdiImageTarget iImageTargets[KAspectRatioCount];
	RSgImageCollection iImageCollections[KAspectRatioCount];
	RSgImage iImages[KAspectRatioCount];
	};

class CRenderTarget : public CBase, public MWsUiBuffer
	{
public:
	enum TAspectRatio
		{
		ENormalAspectRatio = 0,
		EInvertedAspectRatio = 1,
		EAspectRatioCount = RWsOffScreenImageTarget::KAspectRatioCount
		};
public:
	static CRenderTarget* NewL(MWsIniFile* aIniFile, TUint32 aUsage, TSgCpuAccess aCpuAccess, TDisplayMode aDisplayMode, 
			const TSize& aSize, TInt aScreenNumber);
	~CRenderTarget();

	TAny* ResolveObjectInterface(TUint aTypeId);

	TDisplayMode DisplayMode() const;
	const TSurfaceId& SurfaceId() const;

	inline TAspectRatio AspectRatio() const;
	void SetAspectRatio(TAspectRatio aAspectRatio);
	const TSgDrawableId& ImageId(TAspectRatio aAspectRatio) const;

	inline CDirectGdiGcWrapper* DirectGdiGc() const;
	TInt SetDrawDeviceOffset(TPoint& aOrigin);
	
	TInt AllocNewTarget(RWsOffScreenImageTarget& aNewTarget, const TSize& aNewSize);
	void SwitchTarget(RWsOffScreenImageTarget& aNewTarget);	//takes ownership of aNewTarget

	void GetPixel(TRgb& aColor, const TPoint& aPixel) const;
	void GetScanLine(TDes8& aScanLine, const TPoint& aStartPixel, TInt aPixelLength, TDisplayMode aDispMode) const;
	TBool RectCompare(const TRect& aRect1, const TRect& aRect2) const;
	void CopyToBitmapL(CFbsBitmap* aBitmap, const TRect& aRect) const;

#if defined(__WINS__) && defined(_DEBUG)
	void UpdateDebugWin();
#endif	

public: //from MWsUiBuffer
	TInt MapReadWrite(TAny*& aDataAddress, TInt& aDataStride);
	TInt MapWriteOnly(TAny*& aDataAddress, TInt& aDataStride);
	TInt Unmap();
	TUidPixelFormat PixelFormat() const;
	TSize SizeInPixels() const;
	
public:
	TPoint iOffset;

private:
	CRenderTarget();
	void ConstructL(MWsIniFile* aIniFile, TUint32 aUsage, TSgCpuAccess aCpuAccess, TDisplayMode aDisplayMode, const TSize& aSize, TInt aScreenNumber);
	inline RSgImage& Image();
	inline const RSgImage& Image() const;
	inline const RSgImageCollection& ImageCollection() const;
	inline const TSgImageInfo& ImageInfo() const;
	const TUint8* DataAddress(const TAny* aStartDataAddress, const TPoint& aStartPoint, TInt aDataStride, TInt aBpp) const;
	TInt MapReadOnly(const TAny*& aDataAddress, TInt& aDataStride) const;
	TInt Unmap() const;

private:
	RWsOffScreenImageTarget iTarget;
	TAspectRatio iCurrentAspectRatio;
	CDirectGdiGcWrapper* iDirectGdiGcWrapper;
#if defined(__WINS__) && defined(_DEBUG)
	CDebugOsbWin* iOsbWin;
#endif
	};

inline CRenderTarget::TAspectRatio CRenderTarget::AspectRatio() const
	{ return iCurrentAspectRatio; }

inline CDirectGdiGcWrapper* CRenderTarget::DirectGdiGc() const
	{ return iDirectGdiGcWrapper; }

inline RSgImage& CRenderTarget::Image()
	{ return iTarget.iImages[iCurrentAspectRatio]; }

inline const RSgImage& CRenderTarget::Image() const
	{ return iTarget.iImages[iCurrentAspectRatio]; }

inline const RSgImageCollection& CRenderTarget::ImageCollection() const
	{ return iTarget.iImageCollections[iCurrentAspectRatio]; }

inline const TSgImageInfo& CRenderTarget::ImageInfo() const
	{ return iTarget.iImageInfos[iCurrentAspectRatio]; }

#endif // __RENDERTARGET_H__
