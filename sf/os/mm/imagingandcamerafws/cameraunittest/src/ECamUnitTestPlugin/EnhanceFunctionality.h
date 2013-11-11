// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ENHANCEFUNCTIONALITY_H
#define ENHANCEFUNCTIONALITY_H

#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>

#include "ECamUnitTestPlugin.h"
#include <fbs.h>

#include <ecam/mcamerahistogram.h>
#include <ecam/mcameraoverlay.h>
#include <ecam/mcamerasnapshot.h>



class CCamUnitTestPlugin;

class CCamHistogram : public CBase, public MCameraHistogram
	{
	class CHistogram;	
public:	
	static CCamHistogram* NewL(CCamUnitTestPlugin& aOwner);
	~CCamHistogram();
	TUint32 SupportedHistograms();
    TUint PrepareHistogramL(CCamera::CCameraHistogram::THistogramType aType); 
    TUint PrepareDSAHistogramL(CCamera::CCameraHistogram::THistogramType aType, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor); 
    void StartHistogramL(TUint aHistHandle);
    void StopHistogramL(TUint aHistHandle);
    void DestroyHistogramL(TUint aHistHandle);
	void Release();
    void GetActiveHistogramsL(RArray<TUint>& aActiveHistograms); 
	void UpdateDSAHistogramPropertiesL(TUint aHistHandle, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor);
	void GetDSAHistogramPropertiesL(TUint aHistHandle, TPoint& aPosition, TSize& aSize, TRgb& aColor);
	MHistogramBuffer& HistogramDataL();
	MHistogramBuffer& HistogramDataL(TUint aHistHandle);
	
	void ConstructL();
	
private:
	CCamHistogram(CCamUnitTestPlugin& aOwner);
	
	
private:
	CCamUnitTestPlugin& iOwner;
	
	TBool iHistActive;
	TBool iDSAHistActive;
	
	TInt iHistogramCount;
	CArrayFixFlat<CHistogram*>* iHistograms;
	
	HBufC* iHistData;
	};


class CCamHistogram::CHistogram : public CBase
	{
public:	
	static CHistogram* NewL(TBool aDSA, CCamera::CCameraHistogram::THistogramType aType, TPoint aPosition, TSize aSize, TRgb iColor);
	
public:
	CHistogram(TBool aDSA, CCamera::CCameraHistogram::THistogramType aType, TPoint aPosition, TSize aSize, TRgb iColor);
	~CHistogram ();
	
	TBool IsActive();
	
public:
	TBool iDSA;
	CCamera::CCameraHistogram::THistogramType iType;
	TInt iAlpha;
	TPoint iPosition;
	TSize iSize;
	TRgb iColor;

	TBool iActive; // ETrue is StartHistogramL has been called

	};

// For Snapshot
class CCamSnapshot : public CBase, public MCameraSnapshot
	{
public:	
	static CCamSnapshot* NewL(CCamUnitTestPlugin& aOwner);
	~CCamSnapshot();
	
	TUint32 SupportedFormats();
    void PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio);
    void PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio);
    
    void SetBgColorL(const TRgb& aBgColor); 
	void SetPositionL(const TPoint& aPosition);
	
    TBool IsSnapshotActive() const;
    void StartSnapshot();
	void StopSnapshot();
	MCameraBuffer& SnapshotDataL(RArray<TInt>& aFrameIndexOrder);
	void Release();

private:	
	CCamSnapshot(CCamUnitTestPlugin& aOwner);

private:
	CCamUnitTestPlugin& iOwner;
		
	CCamera::TFormat iFormat;
	TSize iSize;
	TRgb iBgColor;
	TBool iAspRatio;
	TPoint iPos;
	TBool iActive; // ETrue is StartHistogramL has been called
	};


//For Overlay
class CCamOverlay : public CBase, public MCameraOverlay, public MCameraOverlay2
	{
	class COverlay;
public:
	static CCamOverlay* NewL(CCamUnitTestPlugin& aOwner);
	~CCamOverlay();
	
	void ConstructL();
	
	//from MCameraOverlay
	TUint CreateOverlayL(const CCamera::CCameraOverlay::TOverlayParameters& aParameters, CFbsBitmap* aBitmap);
	void SetOverlayBitmapL(TUint aOverlayHandleRef, const CFbsBitmap* aBitmap);
    void GetOverlayBitmapL(TUint aOverlayHandleRef, CFbsBitmap* aBitmap);
    void GetOverlayParametersL(TUint aOverlayHandle, CCamera::CCameraOverlay::TOverlayParameters& aInfo);
	void SetOverlayParametersL(TUint aOverlayHandle, const CCamera::CCameraOverlay::TOverlayParameters& aParameters);
	void ReleaseOverlay(TUint aOverlayHandleRef);
	void Release();
	void GetOverlaySupport(CCamera::CCameraOverlay::TOverlaySupportInfo& aInfo);
    void GetAllOverlaysInZOrderL(RArray<TUint>& aOverlayHandles);
	void SetAllOverlaysInZOrderL(const RArray<TUint>& aOverlayHandles);
	
	//from MCameraOverlay2
	void SetModifiableOverlayBitmapL(TUint aOverlayHandle, CFbsBitmap* aBitmap);
	void GetAllOverlaysInZOrderL(CCamera::CCameraOverlay::TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, RArray<TUint>& aOverlayHandles) const;
	void SetAllOverlaysInZOrderL(CCamera::CCameraOverlay::TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, const RArray<TUint>& aOverlayHandles);
	
private:
	CCamOverlay(CCamUnitTestPlugin& aOwner);

public:
	TInt iRefCount;

private:
	CCamUnitTestPlugin& iOwner;
	CArrayFixFlat<COverlay*>* iOverlays;	
	
	CCamera::CCameraOverlay::TOverlaySupportInfo iSupportInfo;
	};
	
class CCamOverlay::COverlay : public CBase
	{
public:	
	static COverlay* NewL(const CCamera::CCameraOverlay::TOverlayParameters& aParameters, CFbsBitmap* aBitmap);
	
public:
	~COverlay ();
	void ConstructL();

private:
	COverlay(const CCamera::CCameraOverlay::TOverlayParameters& aParameters, CFbsBitmap* aBitmap);
		
public:
	CCamera::CCameraOverlay::TOverlayParameters iOverlayParms;
	CFbsBitmap* iBitmap; // doesn't own
	TUint iZOrderReverse; //for sorting on z-order
	};
	
	
class RCamSnapshotBuffer : public MCameraBuffer
	{
public:
	RCamSnapshotBuffer(CFbsBitmap*& aFrame): iAvailable(ETrue), iFrame(aFrame) {}
	virtual TInt NumFrames() {return 0;}
	virtual TDesC8* DataL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return NULL; }
	virtual CFbsBitmap& BitmapL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return *iFrame;}
 	virtual	RChunk& ChunkL() { return iChunk;}
	virtual TInt ChunkOffsetL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return 0;}
	virtual TInt FrameSize(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return 0; }
	virtual void Release() {iAvailable = ETrue;}
	
public:	
	TBool iAvailable;
	CFbsBitmap*& iFrame;
	RChunk iChunk;
	};

#endif // ENHANCEFUNCTIONALITY_H
