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

#include "screencapturerenderstage.h"
#include <e32std.h>
#include <e32base.h>
#include "panic.h"
#include <fbs.h>
#include <graphics/wsgraphicscontext.h>
#include <graphics/wsdisplaycontrol.h>
#include <graphics/wsscene.h>
#include <graphics/wsdisplaymapping.h>
#include <graphics/testscreencapture.h>
#include <graphics/wsscreendeviceutils.h>
#include <graphics/wsscreendevice.h>



void Panic(TStdPluginPanic aPanic)
	{
	_LIT(KStdPanicCategory, "WSERV-STD");
	User::Panic(KStdPanicCategory, aPanic);
	}

CScreenCaptureRenderStage* CScreenCaptureRenderStage::NewL(MWsGraphicDrawerEnvironment * aEnv, CWsRenderStage* aNextStage)
	{
	CScreenCaptureRenderStage * stage = new(ELeave) CScreenCaptureRenderStage;
	CleanupStack::PushL(stage);
	stage->ConstructL(aEnv, aNextStage);
	
	CleanupStack::Pop(stage);
	return stage;
	}
	
void CScreenCaptureRenderStage::ConstructL(MWsGraphicDrawerEnvironment * /*aEnv*/, CWsRenderStage* aNextStage)
	{
	BaseConstructL();
	
	// initialise the references to the interfaces of interest
	iScene = aNextStage->ObjectInterface<MWsScene>();
	STD_ASSERT_ALWAYS(iScene, EPluginPanicSceneMissing);
	
	iScreenDevice = aNextStage->ObjectInterface<MWsScreenDevice>();
	STD_ASSERT_ALWAYS(iScreenDevice, EPluginPanicSceneMissing);
	
	iDisplayMapping = aNextStage->ObjectInterface<MWsDisplayMapping>();
	
	// create the local surface
	User::LeaveIfError(iSurfaceManager.Open());
	User::LeaveIfError(iSurfaceUpdateSession.Connect());
	
	CreateLocalSurfaceL();
	
	iTimer.CreateLocal();
	
	SetNext(aNextStage);
	}
	
CScreenCaptureRenderStage::CScreenCaptureRenderStage()
	{
	}
	
CScreenCaptureRenderStage::~CScreenCaptureRenderStage()
	{
	iScene->UnregisterSurface(iLocalSurface);
	iSurfaceManager.CloseSurface(iLocalSurface);
	iSurfaceManager.Close();
	iSurfaceUpdateSession.Close();
	iTimer.Close();
	}

TAny* CScreenCaptureRenderStage::ResolveObjectInterface(TUint aTypeId)
    {
    switch(aTypeId)
        {
        case MWsTestScreenCapture::EWsObjectInterfaceId:
            return iDisplayMapping ? static_cast<MWsTestScreenCapture*>(this): NULL;
        default:
            return CWsRenderStage::ResolveObjectInterface(aTypeId);
        }
    }

void CScreenCaptureRenderStage::Begin(const TRegion* aRegion)
	{
	if (Next())
		{
		Next()->Begin(aRegion);
		}
	}

	
void CScreenCaptureRenderStage::End(TRequestStatus* aFinished)
	{
	if (Next())
		{
		Next()->End(aFinished);
		}
	}

TInt CScreenCaptureRenderStage::TestScreenCaptureL(const TAny* aMessage, MWsScreenDeviceUtils& aUtils)
	{
	const TWsScsComposeScreenCommand* msgPtr = reinterpret_cast<const TWsScsComposeScreenCommand*>(aMessage);
	if (msgPtr == NULL)
		{
		return KErrArgument;
		}
	switch(msgPtr->iCommand)
		{
		case MTestScreenCapture::EWsScsScreenCompose:
			ComposeScreenL(msgPtr->iParameter.iBitmapHandle, aUtils);
			break;
		case MTestScreenCapture::EWsScsTranslateExtent:
			TranslateExtentL(*msgPtr, aUtils);
			break;
		default:
			aUtils.PanicCurrentClient(EPluginPanicScreenCaptureInvalidCommand);
			break;
		}
	return KErrNone;
	}

void CScreenCaptureRenderStage::ComposeScreenL(TInt aHandle, MWsScreenDeviceUtils& aUtils)
	{
	TInt ret = KErrNone;
	aUtils.RedrawNowIfPending();

	//Let's duplicate the bitmap
	CFbsBitmap* bitmap=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	
	TInt r = bitmap->Duplicate(aHandle);
	
	if (r == KErrNoMemory)
		{
		User::Leave(KErrNoMemory);
		}
	
	ret = KErrBadHandle;
	if (r == KErrNone)
		{
		CreateLocalSurfaceL();
		
		// check that the bitmap format is matching our requirements
		if (VerifyBitmapFormatL(*bitmap))
		    {
	        ret = iScene->RegisterSurface(iLocalSurface);
	        if (ret == KErrNone)
	            {
	            TRequestStatus cRs = KErrNone;
	            iScene->ComposePendingScene(iLocalSurface, &cRs);
	            User::WaitForRequest(cRs);
	            ret = CopySurfaceToBitmapL(*bitmap);
	            }
			}
		else
			{
			// we accept only matching bitmap formats
			ret = KErrArgument;
			}
		}
	
	CleanupStack::PopAndDestroy(bitmap);
	aUtils.SetReply(ret);
	}

void CScreenCaptureRenderStage::TranslateExtentL(const TWsScsComposeScreenCommand& aMessage, MWsScreenDeviceUtils& aUtils)
	{
	TPoint srcTopLeft(aMessage.iParameter.iExtent.iTl.iX, aMessage.iParameter.iExtent.iTl.iY);
	TPoint srcBottomRight(aMessage.iParameter.iExtent.iBr.iX, aMessage.iParameter.iExtent.iBr.iY);
	TRect rectSrc(srcTopLeft, srcBottomRight);
	TRect rectRes;
	TInt ret = iDisplayMapping->MapCoordinates(EApplicationSpace, rectSrc, ECompositionSpace, rectRes);
	aUtils.ReplyBuf(&rectRes, sizeof(TRect));
	aUtils.SetReply(ret);
	}

TSurfaceId CScreenCaptureRenderStage::CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride)
	{
	RSurfaceManager::TSurfaceCreationAttributesBuf bf;
	RSurfaceManager::TSurfaceCreationAttributes& b = bf();
	b.iSize.iWidth = aSize.iWidth;
	b.iSize.iHeight = aSize.iHeight;
	b.iBuffers = 1;				// number of buffers in the surface
	b.iPixelFormat = aPixelFormat;
	b.iStride = aStride;		// Number of bytes between start of one line and start of next
	b.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	b.iAlignment = 4;			// alignment, 1,2,4,8 byte aligned
	b.iContiguous=EFalse;
	b.iMappable = ETrue;

	TSurfaceId surface = TSurfaceId::CreateNullId();
	User::LeaveIfError(iSurfaceManager.CreateSurface(bf, surface));
	return surface;
	}

void CScreenCaptureRenderStage::CreateLocalSurfaceL()
	{
	TSize surfaceSize;
	GetCompositionAreaSize(surfaceSize);
	
	if (!iLocalSurface.IsNull())
		{
        RSurfaceManager::TInfoBuf infoBuf;
    	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
        if ((KErrNone != iSurfaceManager.SurfaceInfo(iLocalSurface, infoBuf)) || 
        	(info.iSize != surfaceSize))
            {
            iScene->UnregisterSurface(iLocalSurface);
    		iSurfaceManager.CloseSurface(iLocalSurface);
    		iLocalSurface = TSurfaceId::CreateNullId();
            }
		}
	
	if (iLocalSurface.IsNull())
		{
		iLocalSurface = CreateSurfaceL(surfaceSize, KSurfacePixelFormat, KSurfaceBytesPerPixel*surfaceSize.iWidth);
		}
	}

TBool CScreenCaptureRenderStage::VerifyBitmapFormatL(const CFbsBitmap& aBitmap)
	{
    RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
    User::LeaveIfError(iSurfaceManager.SurfaceInfo(iLocalSurface, infoBuf));
    		
	return ((info.iSize == aBitmap.SizeInPixels()) && (EColor16MU == aBitmap.DisplayMode()));
	}

void CScreenCaptureRenderStage::GetCompositionAreaSize(TSize& aSize) const
	{
	TSize uiSize;
	uiSize = iScreenDevice->SizeInPixels();
	TRect uiRect(TPoint(0,0), uiSize);
	TRect compositionRect;
	iDisplayMapping->MapCoordinates(EFullScreenSpace, uiRect, ECompositionSpace, compositionRect);
	aSize = compositionRect.Size();
	}

TInt CScreenCaptureRenderStage::CopySurfaceToBitmapL(CFbsBitmap& aCopyToBitmap)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	User::LeaveIfError(iSurfaceManager.SurfaceInfo(iLocalSurface, infoBuf));
	
	TInt bytesPerPixel=0;
	TDisplayMode	bitmapMode = ENone;
	
	switch (info.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
			{
			bitmapMode = EColor16MU;
			bytesPerPixel = 4;
			break;
			}
		default:
			{
			return KErrCorrupt;
			}
		}
	
	if ((aCopyToBitmap.SizeInPixels() != info.iSize) || (aCopyToBitmap.DisplayMode() != bitmapMode))
		{
		return KErrCorrupt;
		}
	
	RChunk chunk;
	CleanupClosePushL(chunk);
	User::LeaveIfError(iSurfaceManager.MapSurface(iLocalSurface, chunk));
	TUint8* surfacePtr = chunk.Base();
	TUint8* bitmapPtr = (TUint8*)aCopyToBitmap.DataAddress();
	TInt copyBytes=info.iSize.iWidth*bytesPerPixel;
	for (TInt y=0; y<info.iSize.iHeight; y++)
		{
		Mem::Copy(bitmapPtr,surfacePtr,copyBytes);
		surfacePtr += info.iStride;
		bitmapPtr += aCopyToBitmap.DataStride();
		}
	CleanupStack::PopAndDestroy(&chunk);
	return KErrNone;
	}
