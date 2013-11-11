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

/**
 @file
 @test
 @internalComponent
*/

#ifndef __T_PSEUDOAPPSURFACEDWINDOW_H__
#define __T_PSEUDOAPPSURFACEDWINDOW_H__

#include <e32base.h>
#include <gdi.h>			//Display modes
#include <iniparser.h>

#include <graphics/surface.h>		//TSurfaceId
#include <graphics/surfacemanager.h>	//RSurfaceManager

#include "t_pseudoappwindow.h"
#include "t_pseudoappanim.h"
#include "t_pseudoapptestresults.h"
#include "t_pseudoappshared.h"

class CTestSurfacedWindow : public CBase
	{
enum TWindowType
	{
	EUiWindow,
	EEcomWindow,
	EEmptyWindow
	};

enum TApplicationType
	{
	ENoApp,
	EFbsBitmap,
	EOpenVG,
	EOpenGLes,
	EPerformance
	};

public:
	static CTestSurfacedWindow* NewL(TInt aScreenNo, TInt aWindowNumber, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,
							const TDesC& aConfigFileName, CTPseudoAppShared& aPseudoAppShared);
	void UpdateL(SurfaceDetails& aSurfDetails);
	void RotateL(TInt aScreenNo, TInt aWindowNumber, const TSize& aScreenSize, TInt aFrameNumber);
	TBool RotationSupported();
	~CTestSurfacedWindow();

private:
	CTestSurfacedWindow(TInt aScreenNo, TInt aWindowNumber, CTPseudoAppShared& aPseudoAppShared);
	void ConstructL(TInt aScreenNo, TInt aWindowNumber, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,
							const TDesC& aConfigFileName);
	void CreateSurfaceManager();
	void DestroySurfaceManager();
	void DestroySurface();
	CTestAnimation*  CreateAnimationL(TInt aAppType, const TSize& aAnimSize, const TSize& aSurfaceSize,
                                     TPtrC& aBitmapFile, TDisplayMode aDisplayMode,
                                     TInt aHorizontalRate, TInt aVerticalRate, TPixelFormat aPixelFormat);
	void ImportWindowConfigL(TInt aScreenNo, TInt aWindowNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults);
	void ImportSurfaceConfigL(TInt aScreenNo, TInt aWindowNo, const TSize& aScreenSize, TSize& aSurfaceSize, TInt& aSurfacePresent, TPixelFormat& aPixelFormat);
	void ImportAnimConfigL(TInt aScreenNo, TInt aWindowNo, TDisplayMode aMode, const TSize& aSurfaceSize, TPixelFormat aPixelFormat);
	CArrayFixFlat<TInt>* ImportWindowSizeChangeParamsL(const TDesC& tempStore);
	CArrayFixFlat<TInt>* ImportWindowPositionChangeParamsL(const TDesC& tempStore);
	CArrayFixFlat<TRect>* ImportWindowBitmapParamsL(const TDesC& tempStore, TInt aMultiBitmapUiFileSize, const TSize& aScreenSize);
	void BuildDimensionArrayL(const TSize& aScreenSize, TInt aFrameDuration, TInt aWindowChangeGeometry, TInt aWindowChangeBehavior);
	void ParamChangeCyclicPosition(TDrawParameters& aDrawParams, TInt aFrameCounter, TInt aFrameDuration, const TSize& aScreenSize);
	void ParamChangeCyclicPositionAndSize(TDrawParameters& aDrawParams, TInt aFrameCounter, TInt aFrameDuration, const TSize& aScreenSize);
	void ParamChangeOpacity(TInt aFrameCounter, TInt aFrameDuration, TDrawParameters& aDrawParams);
	void CalcNewWindowLocation(const TSize& aScreensize, TInt aIncrement, TInt aDirection,TDrawParameters& aDrawParams);

private:
	CTestWindow* iTestWindow;
	CTestAnimation* iTestAnimation;
	CArrayFixFlat<TInt>* iSizeArray;
	CArrayFixFlat<TInt>* iPositionArray;
	CArrayFixFlat<TRect>* iLoadsarects;
	CArrayFixFlat<TDrawParameters>* iLoadsaParameters;
	TGceTestResults* iGceTestResults;

	TSurfaceId iSurfaceId;
	RSurfaceManager iSurfaceManager;
	RSurfaceUpdateSession iSurfaceUpdateSession;
	TInt iSurfaceScreenNumber;
	TInt iSurfaceBufferNumber;
	CTestSurfaceRasterizer* iSurfaceRasterizer;
	CIniData* iUtils;
	TInt iScreenNo;
	TInt iWindowNumber;
	CTPseudoAppShared& iPseudoAppShared;
	};

#endif //__T_PSEUDOAPPSURFACEDWINDOW_H__
