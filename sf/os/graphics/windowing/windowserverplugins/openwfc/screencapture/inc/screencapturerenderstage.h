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

#ifndef __SCREENCAPTURERENDERSTAGE_H__
#define __SCREENCAPTURERENDERSTAGE_H__

#include <graphics/wsrenderstage.h>
#include <graphics/wstestscreencapture.h>
#include <graphics/testscreencapture.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>

class MWsScene;
class MWsDisplayControl;
class MWsDisplayMapping;
class MWsScreenDevice;

/**
This is the implementation of CWsRenderStage which is created by a CScreenCaptureRenderStageFactory
*/

class CScreenCaptureRenderStage : public CWsRenderStage, public MWsTestScreenCapture
	{
public:
	static CScreenCaptureRenderStage * NewL(MWsGraphicDrawerEnvironment * aEnv, CWsRenderStage* aNextStage);
	virtual ~CScreenCaptureRenderStage();

public: // implementation of CWsRenderStage
	TAny* ResolveObjectInterface(TUint aTypeId);
	virtual void Begin(const TRegion* aRegion);
	virtual void End(TRequestStatus* aFinished);
	
public: // implementation of MWsTestScreenCapture
	virtual TInt TestScreenCaptureL(const TAny* aMessage, MWsScreenDeviceUtils& aUtils);

private:
	CScreenCaptureRenderStage();
	void ConstructL(MWsGraphicDrawerEnvironment * aEnv, CWsRenderStage* aNextStage);
	void ComposeScreenL(TInt aHandle, MWsScreenDeviceUtils& aUtils);
	void TranslateExtentL(const TWsScsComposeScreenCommand& aMessage, MWsScreenDeviceUtils& aUtils);
	void CreateLocalSurfaceL();
	void GetCompositionAreaSize(TSize& aSize) const;
	TSurfaceId CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride);
	TBool VerifyBitmapFormatL(const CFbsBitmap& aBitmap);
	TInt CopySurfaceToBitmapL(CFbsBitmap& aCopyToBitmap);
	
private:
	MWsScene* iScene;
	MWsDisplayMapping* iDisplayMapping;
	MWsScreenDevice* iScreenDevice;
	TSurfaceId iLocalSurface;
	RSurfaceManager iSurfaceManager;
	RSurfaceUpdateSession iSurfaceUpdateSession;
	RTimer iTimer;
	static const TUidPixelFormat KSurfacePixelFormat = EUidPixelFormatXRGB_8888;
	static const TInt KSurfaceBytesPerPixel = 4;
	};

#endif //__SCREENCAPTURERENDERSTAGE_H__
