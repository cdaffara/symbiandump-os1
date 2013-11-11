// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MEDIACLIENTVIDEODISPLAY_H
#define MEDIACLIENTVIDEODISPLAY_H

#include <w32std.h>
#include <mmf/common/mmfvideosurfacecustomcommands.h>

class MMMFSurfaceEventHandler;
class CMediaClientVideoDisplayBody;
/**
@publishedPartner
@released

The class offers a simple interface to add, remove video display window and supports graphics surfaces.
It also offers some display handling funcations like video extension, scalling and rotation. 

This class has one static function through which client can construct, initialise and get 
pointer of the class.
*/
NONSHARABLE_CLASS(CMediaClientVideoDisplay) : public CBase
	{
public:

	IMPORT_C static CMediaClientVideoDisplay* NewL(TInt aDisplayId);
	IMPORT_C static CMediaClientVideoDisplay* NewL(TInt aDisplayId, const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	
	IMPORT_C ~CMediaClientVideoDisplay();
	
	IMPORT_C void AddDisplayWindowL(const RWindowBase* aWindow, const TRect& aClipRect, const TRect& aCropRegion,
							const TRect& aVideoExtent, TReal32 aScaleWidth, TReal32 aScaleHeight,
							TVideoRotation aRotation, TAutoScaleType aAutoScaleType, TInt aHorizPos, TInt aVertPos, RWindow* aWindow2);
	IMPORT_C TInt RemoveDisplayWindow(const RWindowBase& aWindow);
	
	IMPORT_C TInt SurfaceCreated(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio, const TRect& aCropRegion);
	IMPORT_C TInt SurfaceParametersChanged(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	IMPORT_C void RemoveSurface();
	IMPORT_C TInt RedrawWindows(const TRect& aCropRegion);
		
	IMPORT_C void SetAutoScaleL(const RWindowBase& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion);
	IMPORT_C void SetRotationL(const RWindowBase& aWindow, TVideoRotation aRotation, const TRect& aCropRegion);
	IMPORT_C TVideoRotation RotationL(const RWindowBase& aWindow);
	IMPORT_C void SetScaleFactorL(const RWindowBase& aWindow, TReal32 aWidthPercentage, TReal32 aHeightPercentage, const TRect& aCropRegion);
	IMPORT_C void GetScaleFactorL(const RWindowBase& aWindow, TReal32& aWidthPercentage, TReal32& aHeightPercentage);

	IMPORT_C void SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion);
	IMPORT_C void SetRotationL(TVideoRotation aRotation, const TRect& aCropRegion);
	IMPORT_C void SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, const TRect& aCropRegion);
	IMPORT_C void SetWindowClipRectL(const RWindowBase& aWindow, const TRect& aWindowClipRect, const TRect& aCropRegion);
	IMPORT_C void SetVideoExtentL(const RWindowBase& aWindow, const TRect& aVideoExtent, const TRect& aCropRegion);
	
	IMPORT_C void SetExternalDisplaySwitchingL(TBool aControl);
	
private:
	
	CMediaClientVideoDisplay();
	void ConstructL(TInt aDisplayId);
	void ConstructL(TInt aDisplayId, const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	
private:
	CMediaClientVideoDisplayBody* iBody;

	};

#endif // MEDIACLIENTVIDEODISPLAY_H

