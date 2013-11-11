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

#include "fbrenderstage.h"
#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>

#ifdef USE_DEBUG_FRAME_CAPTURE
#include "../debuglog/DEBUGLOG.H"
#endif

CFbRenderStage* CFbRenderStage::NewL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen, MWsScreenRedraw * aScreenRedraw)
	{
	CFbRenderStage * stage = new(ELeave) CFbRenderStage;
	CleanupStack::PushL(stage);
	stage->ConstructL(aEnv, aScreen, aScreenRedraw);
	CleanupStack::Pop(stage);
	return stage;
	}
	
void CFbRenderStage::ConstructL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen, MWsScreenRedraw * aScreenRedraw)
	{
	BaseConstructL();
	iEnv = aEnv;
	iScreen = aScreen;
	iScreenRedraw = aScreenRedraw;
	iBackBuffer = iScreen->ObjectInterface<MWsBackBuffer>();
	User::LeaveIfNull(iBackBuffer);
	iScreenConfig = iScreen->ObjectInterface<MWsScreenConfig>();
	User::LeaveIfNull(iScreenConfig);
#ifdef USE_DEBUG_FRAME_CAPTURE
	MWsIniFile* wsini = iEnv->ObjectInterface<MWsIniFile>();
	_LIT(KWSERVIniFileVarFrameCapture,"FRAMECAPTURE");
	iFrameCapture = wsini->FindVar(KWSERVIniFileVarFrameCapture);
	
	// Location to save captured images
	if (!wsini->FindVar(KWSERVIniFileVarFrameCapture, iFrameCaptureLocation) || iFrameCaptureLocation.Length() == 0)
		{
		iFrameCaptureLocation.Set(KWSERVIniFileVarFrameCaptureLocation);
		}
	
	iDebugBitmap = new (ELeave) CFbsBitmap;
	TDisplayMode displayMode = iScreenConfig->DisplayMode();
	TSize screenSize = iScreenConfig->SizeInPixels();
	// Create bitmap with largest dimension of width and height to cope with screen rotation 
	// assuming largest screen dimension doesn't increase during rotation
	TUint largestDimension = ((screenSize.iWidth > screenSize.iHeight)? screenSize.iWidth : screenSize.iHeight);
	User::LeaveIfError(iDebugBitmap->Create(TSize(largestDimension, largestDimension), displayMode));
	iDebugBitmapDevice = CFbsBitmapDevice::NewL(iDebugBitmap);
	User::LeaveIfError(iDebugBitmapDevice->CreateContext(iDebugBitmapGc));
#endif
	}
	
CFbRenderStage::CFbRenderStage()
	{
	}
	
CFbRenderStage::~CFbRenderStage()
	{
	}

CFbsBitGc * CFbRenderStage::Begin()
	{
	return iBackBuffer->GetBitGcCurrent();
	}
	
void CFbRenderStage::End()
	{
	if (Next())
		{
		CFbsBitGc * gc = Next()->Begin();
		const TRegion * region = iScreenRedraw->AnimationRegion();
				
#ifdef USE_DEBUG_FRAME_CAPTURE
		if (iFrameCapture)
			{
			CaptureFrame(region);
			}
#endif
		BlitRegion(region, gc);
		Next()->End();
 		}
 	}

void CFbRenderStage::BlitRegion(const TRegion* aRegion, CFbsBitGc* aGc)
	{
	if(aRegion && !aRegion->IsEmpty() && !aRegion->CheckError())
		{
		if (iBackBuffer->Observer())
			iBackBuffer->Observer()->BeforeUpdate(*iBackBuffer,*aRegion);
		
		aGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		aGc->SetClippingRegion(aRegion);
		aGc->BitBlt(-iScreenConfig->ScaledOrigin(),iBackBuffer->GetBitmap());
		aGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
		aGc->CancelClipping();
		
		if (iBackBuffer->Observer())
			iBackBuffer->Observer()->AfterUpdate(*iBackBuffer,*aRegion);
		}	
	}


#ifdef USE_DEBUG_FRAME_CAPTURE
class TTruncateOverflow : public TDesOverflow
	{
	public:
	virtual void Overflow(TDes&) {};
	};

void CFbRenderStage::CaptureFrame(const TRegion* aRegion)
	{
	iDebugBitmapGc->SetBrushColor(TRgb(255,255,255));
	iDebugBitmapGc->Clear();
	iDebugBitmapGc->SetBrushColor(TRgb(55,55,55));
	iDebugBitmapGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TSize screenSize = iScreenConfig->ScreenModeSizeInPixels();
	iDebugBitmapGc->DrawRect(TRect(0, 0, screenSize.iWidth, screenSize.iHeight));
	BlitRegion(aRegion, iDebugBitmapGc);
	
	TUint timestamp = User::FastCounter();
	TBuf<LogTBufSize> filename;
	TTruncateOverflow overflow;
	filename.AppendFormat(iFrameCaptureLocation, &overflow);
	filename.AppendFormat(_L("frame_%010u.mbm"), &overflow, timestamp);
	iDebugBitmap->Save(filename);
	LogRegion(filename, _L(" CFbRenderStage::CaptureFrame() "), aRegion);	
	}

void CFbRenderStage::LogRegion(const TDesC& aPrefix, const TDesC& aFunctionName, const TRegion* aRegion)
	{
	TBuf<LogTBufSize> log;
	TTruncateOverflow overflow;
	TInt rectCount = (aRegion == NULL ? 0 : aRegion->Count());
	log.AppendFormat(aPrefix, &overflow);
	log.AppendFormat(aFunctionName, &overflow);
	log.AppendFormat(_L("region [%d,"), &overflow, rectCount);
	if (rectCount > 0)
		{
		const TRect* rectangles = aRegion->RectangleList();
		TBuf<1> comma;
		for (TInt ii = 0; ii < rectCount; ii++)
			{
			TRect current = rectangles[ii];
			log.AppendFormat(_L("%S{{%d,%d},{%d,%d}}"), &overflow, &comma,
                             current.iTl.iX,current.iTl.iY,current.iBr.iX,current.iBr.iY);
			comma = _L(",");
			}
		}
	else
		{
		log.AppendFormat(_L("NULL"), &overflow);
		}
	log.AppendFormat(_L("]"), &overflow);
	iEnv->Log(ELogEverything, log);
	}
#endif
