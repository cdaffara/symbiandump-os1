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

#include "EnhanceFunctionality.h"
#include "AdvancedSettings.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecam/cameraoverlayconst.h>
#endif

CCamHistogram* CCamHistogram::NewL(CCamUnitTestPlugin& aOwner)
	{
	CCamHistogram* self = new (ELeave) CCamHistogram(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CCamHistogram::ConstructL()
	{
	iHistograms = new (ELeave) CArrayFixFlat<CHistogram*>(1);
	}

CCamHistogram::CCamHistogram(CCamUnitTestPlugin& aOwner):iOwner(aOwner)
	{
	}
	
CCamHistogram::~CCamHistogram()
	{
	if(iHistograms != NULL)
		{
		TInt i=0;
		TInt count = iHistograms->Count();
		while(i<count)
			{
			delete iHistograms->At(i);
			i++;
			}
		iHistograms->Reset();
		delete iHistograms;
		iHistograms = NULL;
		}
	}
	
TUint32 CCamHistogram::SupportedHistograms()
	{
	return CCamera::CCameraHistogram::ELuminance;
	}
	
TUint CCamHistogram::PrepareHistogramL(CCamera::CCameraHistogram::THistogramType aType)
	{
	CHistogram* histogram = CHistogram::NewL(EFalse, aType, TPoint(0,0), TSize(0,0), TRgb(0));
	iHistograms->AppendL(histogram);
	TInt temp = iHistograms->Count();
	return --temp;
	}
	
TUint CCamHistogram::PrepareDSAHistogramL(CCamera::CCameraHistogram::THistogramType aType, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor)
	{
	CHistogram* histogram = CHistogram::NewL(ETrue, aType, aPosition, aSize, aColor);
	iHistograms->AppendL(histogram);
	TInt temp = iHistograms->Count();
	return --temp;
	}
	
void CCamHistogram::StartHistogramL(TUint aHistHandle)
	{
	iHistograms->At(aHistHandle)->iActive = ETrue;
	if(iHistograms->At(aHistHandle)->iDSA == EFalse)
		{
		TUid myuid;
		myuid.iUid = KUidECamEventHistogramUidValue;
		iOwner.Notify(myuid,0);	
		}
	}
	
void CCamHistogram::StopHistogramL(TUint aHistHandle)
	{
	iHistograms->At(aHistHandle)->iActive = EFalse;
	}
	
void CCamHistogram::DestroyHistogramL(TUint aHistHandle)
	{
	delete iHistograms->At(aHistHandle);
	iHistograms->Delete(aHistHandle);
	}
	
void CCamHistogram::Release()
	{
	delete this;
	}
	
void CCamHistogram::GetActiveHistogramsL(RArray<TUint>& /*aActiveHistograms*/)
	{
	//Not implemented
	}
	
void CCamHistogram::UpdateDSAHistogramPropertiesL(TUint aHistHandle, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor)
	{
	iHistograms->At(aHistHandle)->iPosition = aPosition;
	iHistograms->At(aHistHandle)->iSize = aSize;
	iHistograms->At(aHistHandle)->iColor = aColor;
	}
	
void CCamHistogram::GetDSAHistogramPropertiesL(TUint aHistHandle, TPoint& aPosition, TSize& aSize, TRgb& aColor)
	{
	aPosition = iHistograms->At(aHistHandle)->iPosition;
	aSize = iHistograms->At(aHistHandle)->iSize;
	aColor = iHistograms->At(aHistHandle)->iColor;
	}
	
MHistogramBuffer& CCamHistogram::HistogramDataL()
	{
	// not implemented
	TInt a = 10;
	return reinterpret_cast<MHistogramBuffer&> (a);
	}
	
MHistogramBuffer& CCamHistogram::HistogramDataL(TUint /*aHistHandle*/)
	{
	// not implemented
	TInt a = 10;
	return reinterpret_cast<MHistogramBuffer&> (a);
	}
	
/**
* Defination for CHistogram Class 
*/
CCamHistogram::CHistogram * CCamHistogram::CHistogram::NewL(TBool aDSA, CCamera::CCameraHistogram::THistogramType aType, TPoint aPosition, TSize aSize, TRgb aColor)
	{
	return new (ELeave) CCamHistogram::CHistogram(aDSA , aType, aPosition, aSize, aColor);
	}
	
CCamHistogram::CHistogram::CHistogram(TBool aDSA, CCamera::CCameraHistogram::THistogramType /*aType*/, TPoint aPosition, TSize aSize, TRgb aColor):iDSA(aDSA),iPosition(aPosition),iSize(aSize),iColor(aColor)
	{
	}
	
CCamHistogram::CHistogram::~CHistogram()
	{
	}
	
/**
*	For Snapshot
*///-----------------------------------------------------------------------------------

CCamSnapshot* CCamSnapshot::NewL(CCamUnitTestPlugin& aOwner)
	{
	return new (ELeave) CCamSnapshot(aOwner);
	}
	
CCamSnapshot::~CCamSnapshot()
	{
	}
	
CCamSnapshot::CCamSnapshot(CCamUnitTestPlugin& aOwner):iOwner(aOwner)
	{
	}
	
TUint32 CCamSnapshot::SupportedFormats()
	{
	// Dummy implementation : Nothing to implement
	return CCamera::EFormatJpeg;
	}
	
void CCamSnapshot::PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio)
	{
	// Dummy implementation : 
	iFormat = aFormat;
	iPos = aPosition;
	iSize = aSize;
	iBgColor = aBgColor;
	iAspRatio = aMaintainAspectRatio;
	}

void CCamSnapshot::PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio)	
	{
	iFormat = aFormat;
	iSize = aSize;
	iAspRatio = aMaintainAspectRatio;
	}

TBool CCamSnapshot::IsSnapshotActive() const
	{
	// Dummy implementation :
	return iActive;
	}
	
void CCamSnapshot::StartSnapshot()
	{
	// Dummy implementation 
	iActive = ETrue;
	
	TUid myuid;
	myuid.iUid = KUidECamEventSnapshotUidValue;
	iOwner.Notify(myuid,0);
	}
	
void CCamSnapshot::StopSnapshot()
	{
	// Dummy implementation 
	iActive = EFalse;
	}
	
MCameraBuffer& CCamSnapshot::SnapshotDataL(RArray<TInt>& aFrameIndexOrder)
	{
	// Dummy implementation :
	aFrameIndexOrder.Append(1);
	TInt temp = 10;
	return reinterpret_cast <MCameraBuffer&> (temp);
	}

void CCamSnapshot::SetBgColorL(const TRgb& aBgColor)
	{
	iBgColor = aBgColor;
	if(iBgColor != aBgColor)
		{
		User::Leave(KErrNotSupported);
		}
	}
	
void CCamSnapshot::SetPositionL(const TPoint& aPosition)
	{
	iPos = aPosition;
	if(iPos != aPosition)
		{
		User::Leave(KErrNotSupported);
		}
	}
		
void CCamSnapshot::Release()
	{
	delete this;
	}


/**
*	For Overlay
*/// -----------------------------------------------------------------------------------

CCamOverlay* CCamOverlay::NewL(CCamUnitTestPlugin& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(!globalData)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		
		globalData->iReferenceCount = 0;
		globalData->iCamOverlay = new (ELeave) CCamOverlay(aOwner);
		CleanupStack::PushL(globalData->iCamOverlay);
		globalData->iCamOverlay->ConstructL();
    	globalData->iCamOverlay->iRefCount = 1;
    	CleanupStack::Pop(globalData->iCamOverlay);
		TInt error = Dll::SetTls(globalData);
		if (error)
			{
			delete globalData->iCamOverlay;
			User::Leave(error);
			}
		CleanupStack::Pop(globalData);
    	
		return static_cast <CCamOverlay*> (globalData->iCamOverlay);
		}
	else
		{
		CCamOverlay* self = globalData->iCamOverlay;
		
		globalData->iReferenceCount++;
		self->iRefCount = globalData->iReferenceCount + 1;
		if (globalData->iReferenceCount == 1 )
			{
			delete globalData;
			Dll::FreeTls();
			}
		else
			{
			TInt error = Dll::SetTls(globalData);
			if (error)
				{
				delete globalData->iCamOverlay;
				User::Leave(error);
				}
			}
		return static_cast <CCamOverlay*> (self);		
		}
	}
	
void CCamOverlay::ConstructL()
	{
	iOverlays = new (ELeave) CArrayFixFlat<COverlay*>(1);
	}
	
CCamOverlay::CCamOverlay(CCamUnitTestPlugin& aOwner):iOwner(aOwner)
	{
	}

CCamOverlay::~CCamOverlay()
	{
	Dll::FreeTls();
	// Dummy implementation 
	if(iOverlays != NULL)
		{
		TInt i=0;
		TInt count = iOverlays->Count();
		while(i<count)
			{
			delete iOverlays->At(i);
			i++;
			}
		iOverlays->Reset();
		delete iOverlays;
		iOverlays = NULL;
		}
	}

void CCamOverlay::Release()
	{
	iRefCount--; 
	if(!iRefCount)
		{
		delete this;
		}
	}

TUint CCamOverlay::CreateOverlayL(const CCamera::CCameraOverlay::TOverlayParameters& aParameters, CFbsBitmap* aBitmap)
	{
	// Dummy implementation 
	if((aParameters.Size() != sizeof(CCamera::CCameraOverlay::TOverlayParameters )) ||
		aParameters.Version() != KECamOverlayParametersCurrentVersion)
		{
		User::Leave(KErrNotSupported);
		}
	
	COverlay* overlay = COverlay::NewL(aParameters, aBitmap);
	overlay->iZOrderReverse = 100 - aParameters.iZOrder;
	iOverlays->AppendL(overlay);
	TInt temp = iOverlays->Count();
	return --temp;
	}

void CCamOverlay::SetOverlayBitmapL(TUint aOverlayHandleRef, const CFbsBitmap* aBitmap)
	{
	// Dummy implementation 
	iOverlays->At(aOverlayHandleRef)->iBitmap = const_cast<CFbsBitmap*> (aBitmap);
	}

void CCamOverlay::GetOverlayBitmapL(TUint aOverlayHandleRef, CFbsBitmap* aBitmap)
	{
	// Dummy implementation 
	TSize size = aBitmap->SizeInPixels();
	TDisplayMode dispMode = aBitmap->DisplayMode();
	
	User::LeaveIfError(aBitmap->Duplicate(iOverlays->At(aOverlayHandleRef)->iBitmap->Handle()));
	size = aBitmap->SizeInPixels();
	dispMode = aBitmap->DisplayMode();
	}

void CCamOverlay::GetOverlayParametersL(TUint aOverlayHandle, CCamera::CCameraOverlay::TOverlayParameters& aInfo)
	{
	// Dummy implementation 
	
	aInfo.iCurrentModes	=	iOverlays->At(aOverlayHandle)->iOverlayParms.iCurrentModes;
	aInfo.iCurrentTypes	=	iOverlays->At(aOverlayHandle)->iOverlayParms.iCurrentTypes;
	aInfo.iAlphaValue	=	iOverlays->At(aOverlayHandle)->iOverlayParms.iAlphaValue;
	aInfo.iPosition		=	iOverlays->At(aOverlayHandle)->iOverlayParms.iPosition;
	aInfo.iZOrder		=	iOverlays->At(aOverlayHandle)->iOverlayParms.iZOrder;
	
	}

void CCamOverlay::SetOverlayParametersL(TUint aOverlayHandle, const CCamera::CCameraOverlay::TOverlayParameters& aParameters)
	{
	// Dummy implementation 
	iOverlays->At(aOverlayHandle)->iOverlayParms.iCurrentModes 	= aParameters.iCurrentModes;
	iOverlays->At(aOverlayHandle)->iOverlayParms.iCurrentTypes 	= aParameters.iCurrentTypes;
	iOverlays->At(aOverlayHandle)->iOverlayParms.iAlphaValue 	= aParameters.iAlphaValue;
	iOverlays->At(aOverlayHandle)->iOverlayParms.iPosition 		= aParameters.iPosition;
	iOverlays->At(aOverlayHandle)->iOverlayParms.iZOrder 		= aParameters.iZOrder;
	
	iOverlays->At(aOverlayHandle)->iZOrderReverse = 100 - aParameters.iZOrder;
	}

void CCamOverlay::ReleaseOverlay(TUint aOverlayHandle)
	{
	// Dummy implementation
	delete iOverlays->At(aOverlayHandle);
	iOverlays->Delete(aOverlayHandle);
	}

void CCamOverlay::GetOverlaySupport(CCamera::CCameraOverlay::TOverlaySupportInfo& aInfo)
	{
	// Dummy implementation 
	if((aInfo.Size() != sizeof(CCamera::CCameraOverlay::TOverlaySupportInfo )) ||
		aInfo.Version() != KECamOverlaySupportInfoCurrentVersion)
		{
		User::Leave(KErrNotSupported);
		}
	
	aInfo.iSupportedModes = (CCamera::CCameraOverlay::EModeStillImageBurst << 1) - 1;
	aInfo.iSupportedTypes = 1;
	aInfo.iPerPlane = CCamera::CCameraOverlay::EBinary;
	aInfo.iPerPixel = CCamera::CCameraOverlay::EBinary;
	aInfo.iCanOverlap = EFalse;
	}

void CCamOverlay::GetAllOverlaysInZOrderL(RArray<TUint>& /*aOverlayHandles*/)
	{
	// Dummy implementation 
	}
	
void CCamOverlay::SetAllOverlaysInZOrderL(const RArray<TUint>& /*aOverlayHandles*/)
	{
	// Dummy implementation 
	}
	
void CCamOverlay::SetModifiableOverlayBitmapL(TUint aOverlayHandle, CFbsBitmap* aBitmap)
	{
	// Dummy implementation 
	iOverlays->At(aOverlayHandle)->iBitmap = aBitmap;
	}

void CCamOverlay::GetAllOverlaysInZOrderL(CCamera::CCameraOverlay::TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, RArray<TUint>& aOverlayHandles) const
	{
	// Dummy implementation
	RArray<TInt> overlayFiltered;
	
	aOverlayHandles.Reset();
		
	// filter overlays for 'aOverlayCameraMode' and aViewFinderHandle (if applicable)
	for(TInt index=0; index<iOverlays->Count(); index++)
		{
		if(iOverlays->At(index)->iOverlayParms.iCurrentModes & aOverlayCameraMode)
			{
			if(aOverlayCameraMode == CCamera::CCameraOverlay::EModeDirectViewfinder || aOverlayCameraMode == CCamera::CCameraOverlay::EModeClientViewfinder)
				{
				if(iOverlays->At(index)->iOverlayParms.iViewFinderHandle == aViewFinderHandle)
					{
					overlayFiltered.AppendL(index);
					}
				}
			else
				{
				overlayFiltered.AppendL(index);	
				}
			}
		}
		
	TUint zOrderMax = 0;
	TInt maxZOrderHandle =0;
	
	while(overlayFiltered.Count())	
		{
		zOrderMax = 0;
		maxZOrderHandle =0;
		
		//find out max Zorder and the handle of the overlay which has this max z-order
		for(TInt index=0; index<overlayFiltered.Count(); index++)
			{
			if(iOverlays->At(index)->iOverlayParms.iZOrder > zOrderMax)
				{
				zOrderMax = iOverlays->At(index)->iOverlayParms.iZOrder;
				maxZOrderHandle = index;
				}
			}
			
		aOverlayHandles.AppendL(maxZOrderHandle);
		overlayFiltered.Remove(maxZOrderHandle);
		}
	
	overlayFiltered.Close()	;
	}
	
void CCamOverlay::SetAllOverlaysInZOrderL(CCamera::CCameraOverlay::TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, const RArray<TUint>& aOverlayHandles)
	{
	//boundary checking
	TBool available = EFalse;
	TInt overlayIndex;
	
	for(TInt index=0; index<iOverlays->Count(); index++)
		{
		if(iOverlays->At(index)->iOverlayParms.iCurrentModes & aOverlayCameraMode)
			{
			if(aOverlayCameraMode == CCamera::CCameraOverlay::EModeDirectViewfinder || aOverlayCameraMode == CCamera::CCameraOverlay::EModeClientViewfinder)
				{
				if(iOverlays->At(index)->iOverlayParms.iViewFinderHandle == aViewFinderHandle)
					{
					available = ETrue;
					}
				}
			else
				{
				available = ETrue;	
				}
			}
			
		overlayIndex = aOverlayHandles.Find(index);	
		if(available)
			{
			if(overlayIndex == KErrNotFound)
				{
				User::Leave(KErrArgument);
				}
			}
		else
			{
			if(overlayIndex != KErrNotFound)
				{
				User::Leave(KErrArgument);
				}	
			}
		}// boundary checking complete
		
	//provide Z order values to aOverlayHandles
	TUint decZorder = 100;
	for(TInt index =0; index<aOverlayHandles.Count(); index++ )
		{
		iOverlays->At(aOverlayHandles[index])->iOverlayParms.iZOrder = decZorder;
		iOverlays->At(aOverlayHandles[index])->iZOrderReverse = 100 - decZorder;
		decZorder--;
		}
	
	//setting the overlays in z-order
	TInt offset = _FOFF(CCamOverlay::COverlay, iZOrderReverse);
	TKeyArrayFix key(offset, ECmpTUint);
	User::LeaveIfError(iOverlays->Sort(key));
	}

	
/**
* Defination for COverlay Class 
*/
CCamOverlay::COverlay* CCamOverlay::COverlay::NewL(const CCamera::CCameraOverlay::TOverlayParameters& aParameters, CFbsBitmap* aBitmap)
	{
	CCamOverlay::COverlay* self = new (ELeave) CCamOverlay::COverlay(aParameters, aBitmap);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CCamOverlay::COverlay::ConstructL()
	{
	}

CCamOverlay::COverlay::COverlay(const CCamera::CCameraOverlay::TOverlayParameters& aParameters, CFbsBitmap* aBitmap):iOverlayParms(aParameters), iBitmap(aBitmap)
	{
	}
	
CCamOverlay::COverlay::~COverlay ()
	{
	}
	
